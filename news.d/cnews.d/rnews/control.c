/*
 * Implement the control messages.
 * These are fairly infrequent and can afford to be done by
 * separate programs.  They are:
 *
 * cancel message-ID		restricted to Sender: else From: or root, in theory
 *
 * newgroup groupname		restrict this, in theory
 * rmgroup groupname		allow some local control over this
 *
 * sendsys			to Reply-To: else From:
 * senduuname			to Reply-To: else From:
 * version			to Reply-To: else From: (NB seismo)
 *
 * ihave message-ID-list remotesys	some day ...
 * sendme message-ID-list remotesys	some day ...
 *
 * TODO: one shell file only for controls? or more common scripts?
 */

#include <stdio.h>
#include <ctype.h>
#ifdef lint
#include <sgtty.h>			/* for TIOCNOTTY, if present */
#ifdef TIOCNOTTY			/* 4.2BSD or later berklix */
#include <sys/wait.h>
#endif					/* TIOCNOTTY */
#endif					/* lint */
#include <sys/types.h>

#include "news.h"
#include "newspaths.h"
#include "headers.h"
#include "history.h"

#define SUBDIR "ctl"			/* holds shell scripts */

#ifndef USENET
#define USENET "usenet"			/* mail address of usenet admin. */
#endif

/*
 * These are shell meta-characters, except for /, which is included
 * since it allows people to escape from the control directory.
 */
#define SHELLMETAS "<>|&;({$=*?[`'\"/"

/*
 * Implement control message specified in hdrs.
 * Because newgroup and rmgroup may modify the active file, for example,
 * we must flush in-core caches to disk first and reload them afterward.
 * TODO: pass header values to scripts as args or in environ,
 *	as NEWS* variables.
 */
int
ctlmsg(hdrs)
struct headers *hdrs;
{
	int status = ST_OKAY, pid, deadpid;
#ifdef lint
#ifdef TIOCNOTTY			/* 4.2BSD or later berklix */
	union wait wstatus;
#else					/* TIOCNOTTY */
	int wstatus;
#endif					/* TIOCNOTTY */
#else					/* lint */
	int wstatus;
#endif					/* lint */
	char *inname = hdrs->h_tmpf, *ctlcmd = hdrs->h_ctlcmd;
	static char nmcancel[] = "cancel ";

	/* process cancels in this process for speed and dbm access */
	if (STREQN(ctlcmd, nmcancel, STRLEN(nmcancel)))
		return cancelart(ctlcmd + STRLEN(nmcancel));

	status |= synccaches();		/* sync in-core copies to disk */
	if ((pid = fork()) == 0) {	/* child process */
		int cmdstat;
		char *cmd;

		/*
		 * Enforce at least minimal security:
		 * standardise the environment, including PATH and IFS -
		 * a local addition to libc (TODO: do it by hand);
		 * reject shell metacharacters in ctlcmd.
		 */
		standard();			/* close most files, etc. */
		if (!safecmd(ctlcmd))
			_exit(1);		/* don't flush stdio buffers */

		cmd = emalloc((unsigned)STRLEN("exec ") +
			strlen(libfile(SUBDIR)) + STRLEN(SFNDELIM) +
			strlen(ctlcmd) + STRLEN(" <") + strlen(inname) + 1);
		(void) sprintf(cmd, "exec %s/%s <%s", libfile(SUBDIR),
			ctlcmd, inname);

		cmdstat = system(cmd);			/* punt */
		if (cmdstat != 0) {			/* intercepted */
			char *mailcmd;
			FILE *mailf;
			extern char *progname;
	
			mailcmd = emalloc((unsigned)STRLEN("PATH=") +
				STRLEN(STDPATH) + STRLEN(" mail ") +
				STRLEN(USENET) + 1);
			/*
			 * Honk at the usenet administrator &
			 * mark this article as dropped.
			 */
			(void) sprintf(mailcmd, "PATH=%s mail %s",
				STDPATH, USENET);
			mailf = popen(mailcmd, "w");
			if (mailf == NULL)
				mailf = stderr;
			(void) fprintf(mailf,
			"%s: control message `%s' exited with status 0%o\n",
				progname, cmd, cmdstat);
			if (mailf != stderr)
				(void) pclose(mailf);
			free(mailcmd);
			_exit(1);			/* don't flush */
		}
		free(cmd);
		_exit(0);				/* don't flush */
	}
	while ((deadpid = wait(&wstatus)) != pid && deadpid != -1)
		;

	/* wrong kid returned, fork failed or child screwed up? */
	if (deadpid != pid || pid == -1
#ifndef lint
	    || wstatus != 0
#endif						/* lint */
	    )
		status |= ST_DROPPED;		/* admin got err.msg. by mail above */
	status |= loadcaches();			/* reload in-core copies */
	return status;
}

static int
safecmd(cmd)			/* true if it's safe to system(3) cmd */
char *cmd;
{
	register char *s;

	for (s = cmd; *s != '\0'; s++)
		if (STREQN(s, "..", strlen("..")))
			return NO;
	for (s = SHELLMETAS; *s != '\0'; s++)
		if (index(cmd, *s) != NULL)
			return NO;
	return YES;
}

static int
cancelart(msgidstr)
char *msgidstr;
{
	register char *wsp;
	/* TODO: someday free this storage */
	/* copy msgidstr into malloc'ed store */
	register char *msgid = strsave(msgidstr);
	int status = ST_OKAY;

	/* skip leading whitespace in msgid */
	while (*msgid != '\0' && isascii(*msgid) && isspace(*msgid))
		++msgid;

	/* eliminate trailing whitespace in msgid */
	for (wsp = msgid + strlen(msgid) - 1; wsp >= msgid &&
	    isascii(*wsp) && isspace(*wsp); --wsp)
		*wsp = '\0';

	/* cancel article if seen, else generate history entry for it */
	if (alreadyseen(msgid)) {
		char *histent, *filelist;

		/*
		 * In theory (RFC 850), inews should verify that the
		 * user issuing the cancel (the Sender: of this
		 * article or From: if no Sender) is the Sender: or
		 * From: of the original article or the super-user on
		 * this machine.
		 *
		 * In practice, this is a lot of work and since
		 * anyone can forge news (and thus cancel anything),
		 * not worth the effort.
		 *
		 * Ignore ST_ACCESS since the article may have been
		 * cancelled before or may have a fake history entry
		 * because the cancel arrived before the article.
		 */
		histent = gethistory(msgid);
		if (histent != NULL &&
		    (filelist = findfiles(histent)) != NULL)
			status |= snufffiles(filelist) & ~ST_ACCESS;
	} else {
		/*
		 * Generate a history file entry for the cancelled article
		 * in case it arrives after the cancel control.
		 * The history file entry will cause the cancelled article
		 * to be rejected as a duplicate.
		 *
		 * Forge up the necessary header struct members
		 * (see history() for details).
		 */
		static struct headers hdrs;	/* static to zero parts */

		hdrs.h_msgid = msgid;
		/* any unlinkable file is OK, so use root */
		(void) strcpy(hdrs.h_files, "/");
		hdrs.h_expiry = "-";		/* use default expiry */
		status |= history(&hdrs);
		(void) putchar('\n');		/* end log line */
	}
	return status;
}
