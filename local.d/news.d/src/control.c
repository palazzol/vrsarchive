/*
 * Control message handling code.  Deal with messages which are to be
 * acted on by netnews itself rather than by people.
 *
 * See defs.h "news_version" for the real version of netnews.
 */

#ifndef lint
static char	*SccsId = "@(#)control.c	2.29	9/3/84";
#endif !lint

#include "iparams.h"

#define eq(msg) (strcmp(msg, cargv[0]) == 0)

int cargc;
char **cargv;

FILE *hfopen();
FILE *popen(), *mhopen(), *mailhdr();

char *senderof();

extern log(char *fmt, ...);

control(h)
struct hbuf *h;
{
	register char *ctlmsgtext;

	if (strncmp(h->title, "cmsg ", 5) == 0)
		strcpy(h->title, h->title+5);

	if (*h->ctlmsg)
		ctlmsgtext = h->ctlmsg;
	else
		ctlmsgtext = h->title;
	log("Ctl Msg %s from %s: %s", h->nbuf, h->path, ctlmsgtext);
	/*
	 * Control messages have the standard format
	 *	command [args]
	 * much like shell commands.  Each site has the option
	 * of customizing this code to deal with control messages
	 * as they see fit, but we would like to buy back the
	 * code, ifdeffed or otherwise parameterized, to simplify
	 * the maintenence issues.
	 */
	argparse(ctlmsgtext);

	if (eq("cancel"))
		c_cancel(cargc, cargv);
	else if (eq("newgroup"))
		c_newgroup(cargc, cargv);
	else if (eq("ihave"))
		c_ihave(cargc, cargv);
	else if (eq("sendme"))
		c_sendme(cargc, cargv);
	else if (eq("sendbad"))
		c_sendme(cargc, cargv);
	else if (eq("rmgroup"))
		c_rmgroup(cargc, cargv);
	else if (eq("sendsys"))
		c_sendsys(cargc, cargv);
	else if (eq("senduuname"))
		c_senduuname(cargc, cargv);
	else if (eq("version"))
		c_version(cargc, cargv);
	else if (eq("checkgroups"))
		c_checkgroups(cargc, cargv);
	else if (eq("delsub"))
		c_unimp(cargc, cargv);
	else
		c_unknown(h, ctlmsgtext);
}

/*
 * Parse the string str into separate words in cargc and cargv
 * as per the usual UNIX convention.  Nothing fancy here, just
 * blanks and tabs separating words.
 */
argparse(str)
char *str;
{
	static char *cavpbuf[20];
	static char cavbuf[256];
	char *nextfree = cavbuf;

	if (str == '\0')
		xerror("Control message %s has no title", header.ident);
	cargc = 0;
	cargv = cavpbuf;
	cargv[0] = cavbuf;

	while (*str) {
		if (*str <= ' ') {
			*nextfree++ = 0;
			cargc++;
			cargv[cargc] = nextfree;
			/* skip over white space */
			while (*str != '\0' && *str <= ' ')
				str++;
			if (*str == '\0')	/* line ends in white space */
				return;
		} else
			*nextfree++ = *str++;
	}
}

/*
 * ihave <artid> <remotesys>
 * The other system is telling you it has article <artid>, in case
 * you decide you want it to transmit it to you.
 */
c_ihave(argc, argv)
char **argv;
{
	char tl[256], ng[256];
	struct hbuf htmp;

	if (argc < 2)
		xerror("ihave: Not enough arguments.");
	/*
	 * Check that we haven't already seen it (history)
	 * and then send back a "sendme" message if we subscribe.
	 */
	strncpy(htmp.ident, argv[1], BUFLEN);
	htmp.oident[0] = '\0';
	if (history(&htmp) == 0) {
		/* Should probably check SUBFILE here. */
		sprintf(tl, "sendme %s %s", argv[1], FULLSYSNAME);
		sprintf(ng, "to.%s.ctl", argv[2]);
		xmitmsg(argv[2], tl, ng);
	}
}

/*
 * sendme <artid> ... <remotesys>
 * The other system wants me to send him article <artid>.
 */
c_sendme(argc, argv)
char **argv;
{
	struct srec srec;
	int i;
	FILE *fp;
	struct hbuf h;

	if (argc < 2)
		xerror("sendme: Not enough arguments.");
	/* Don't ask for it from myself */
	if (strncmp(FULLSYSNAME, argv[argc], SNLN) == 0)
		return;
	/* Find the sys record */
	s_openr();
	while (s_read(&srec)) {
		if (strncmp(srec.s_name, argv[argc], SNLN))
			continue;
		/* It's the right one.  Send them. */
		for (i=1; i<argc; i++) {
			fp = hfopen(argv[i]);
			if (hread(&h, fp, TRUE) == NULL)
				xerror("Article %s is garbled.", argv[i]);
			fseek(fp, 0L, 0);
			if (strcmp(argv[0], "sendme") == 0) {
				/* check that other sys subscribes. */
				if (!ngmatch(h.nbuf, srec.s_nbuf) ||
					!(h.distribution[0] == '\0' ||
					ngmatch(h.distribution, srec.s_nbuf)))
					continue;
			}
			transmit(&srec, fp, 0);
			/* transmit does fclose(fp) */
		}
		return;
	}
	xerror("Cannot find system %s to send article %s to.", argv[argc],
		argv[1]);
}

/*
 * newgroup <groupname>
 * A new newsgroup has been created.
 * The body of the article, if present, is a description of the
 * purpose of the newsgroup.
 *
 */
c_newgroup(argc, argv)
char **argv;
{
	FILE *fd;

	if (argc < 1)
		xerror("newgroup: Not enough arguments.");
	if (validng(argv[1]))
		return;

	strcpy(bfr, dirname(argv[1]));
	mknewsg(bfr, argv[1]);

#ifdef NOTIFY
	/*
	 * Sample code to notify the contact person.
	 * Probably should dig up the text of the article
	 * and enclose that, too.  It can be found in the
	 * file ARTICLE.  Also, there needs to be
	 * an automatic provision to help you add the newsgroup.
	 *
	 */
	fd = mailhdr((struct hbuf *)NULL, "creation of new newsgroup");
	if (fd != NULL) {
		fprintf(fd, "\nA new newsgroup called '%s' has been created by %s.\n\n",
			argv[1], header.path);
		mclose(fd);
	}
#endif
}

/*
 * rmgroup <groupname>
 * An old newsgroup is being cancelled on a network wide basis.
 */
c_rmgroup(argc, argv)
char **argv;
{
	FILE *fd;
	char *groupname;
	char groupdir[128];
#ifndef MANUALLY
	int rc;
#endif MANUALLY

	if (argc < 1)
		xerror("rmgroup: Not enough arguments.");
	groupname = argv[1];
	verifyname(groupname);
	if (groupname[0] == '.' || groupname[0] <= ' ')
		xerror("Illegal group name in rmgroup");

	strcpy(groupdir, dirname(groupname));
	if (access(groupdir, 0)) {
		/*
		 * If the group already is gone, it's a nonfatal error - we
		 * want to propagate the message anyway, since what probably
		 * happened is somebody locally already removed it.
		 */
		log("Cannot remove newsgroup '%s'", groupname);
		return;
	}
#ifdef NOTIFY
	fd = mailhdr((struct hbuf *)NULL, "rmgroup control message");
	if (fd != NULL) {
# ifndef MANUALLY
		fprintf(fd, "\nA newsgroup called '%s' has been removed by %s.\n\n",
			argv[1], header.path);
#  ifdef USG
		fprintf(fd, "You may need to remove the directory %s by hand\n",
			dirname(argv[1]));
#  endif
# else
		fprintf(fd, "\n%s has requested that newsgroup %s be removed.\n",
			header.path, argv[1]);
		fprintf(fd, "You should remove it by hand\n");
# endif
		mclose(fd);
	}
#endif

#ifndef MANUALLY
	/* We let the shell do all the work.  See the rmgrp shell script. */
	setuid(geteuid());	/* otherwise it won't rmdir the dir */
	sprintf(bfr, "exec %s/rmgroup %s", LIB, groupname);
	rc = system(bfr);
	log("system(%s) status %d", bfr, rc);
#endif
}

/*
 * cancel <artid>
 * Cancel the named article
 */
c_cancel(argc, argv)
char **argv;
{
	char *line, *p, *q, *r, *poster;
	char *findhist();
	register FILE *fp;
	char whatsisname[150];
	char msgng[64];
	time_t t;
	int su = 0;

	if (argc < 1)
		xerror("cancel: Not enough arguments.");
	strcpy(whatsisname, senderof(&header));
	strcpy(msgng, header.nbuf);
	line = findhist(argv[1]);
	if (line == NULL) {
		log("Can't cancel %s:  non-existent", argv[1]);
		(void) time(&t);
		sprintf(bfr,"%s\t%.12s\tcancelled", argv[1], &ctime(&t)[4]);
		savehist(bfr);
		return;
	}

	q = index(line, '\t');
	p = index(q+1, '\t');
	p++;
	if (strcmp(p, "cancelled") == 0) {
		*q = '\0';
		log("Already Cancelled %s", line);
		return;
	} else
		log("Cancelling %s", line);
	while (*p) {
		q = index(p, ' ');
		if (q)
			*q = '\0';
		strcpy(filename, dirname(p));
		fp = xfopen(filename, "r");
		if (hread(&header, fp, TRUE) == NULL)
			xerror("Article is garbled.");
		fclose(fp);
		if((uid==ROOTID||uid==0) && strncmp(msgng,"to.",3) == 0)
			su = 1;
		poster = senderof(&header);
		/* only compare up to '.' or ' ' */
		r = index(poster,'.');
		if (r == NULL)
			r = index(poster,' ');
		if (r != NULL)
			*r = '\0';
		if (!su && strncmp(whatsisname, poster,strlen(poster))) {
			xerror("Not contributor: posted by %s, and you are %s", poster, whatsisname);
		}

		cancel();
		p = q+1;
	}
}

/*
 * sendsys	(no arguments)
 *
 * Mail the sys file to the person submitting the article.
 * POLICY: the contents of your sys file are public information
 * and as such, you should not change this code.  You may feel
 * free to arrange for it to manually notify you, in the event
 * that you want to do something to clean it up before it goes out.
 * Secret sites on the net are expressly frowned on.
 *
 * The purpose of this command is for making a network map.  The
 * details of your link and which newsgroups are forwarded are not
 * important, in case you want to sanitize them.  Since the definition
 * of USENET is those sites getting net.general, you can disable this
 * on sites not getting net articles, but if you take out the list of
 * forwarded newsgroups, and you have sites that only get local newsgroups,
 * you should make this clear, or remove those sites from what you send out.
 */
c_sendsys(argc, argv)
char **argv;
{
	register FILE *f, *u;
	int c;

#ifdef lint
	argc += **argv;
#endif lint

#ifdef NOTIFY
	f = mailhdr((struct hbuf *)NULL, "sendsys control message");
	if (f != NULL) {
		fprintf(f, "\n%s requested your sys file.\n", header.path);
		fprintf(f, "It has been sent.\n");
		mclose(f);
	}
#endif
	f = mailhdr(&header, "Subject: response to your sendsys request\n\n");
	u = fopen(SUBFILE, "r");
	if (f != NULL && u != NULL) {
		while ((c=getc(u)) != EOF)
			putc(c, f);
		fclose(u);
		mclose(f);
	}
}

/*
 * senduuname	(no arguments)
 *
 * Run the "uuname" command and send it back to the person who submitted
 * the article.  The purpose of this control message is for attempting to
 * make a uucp net map.
 *
 * POLICY: If you view this information as not public (because you have
 * a connection you consider secret, or know a site that considers itself
 * secret) you can feel free to change this code in whatever way is
 * appropriate, so long as it sends some response back to the sender.  If
 * you don't run uucp, this code does not make sense, and so an error
 * message (or garbage, such as "research") will be mailed back.
 *
 * If you wish to add or remove sites from the output of uuname, you
 * may wish to use the euuname.sh shell script here.
 */
c_senduuname(argc, argv)
char **argv;
{
	char buf[256];
	FILE *fd, *u;
	int c;

#ifdef lint
	argc += **argv;
#endif lint

#ifdef NOTIFY
	fd = mailhdr((struct hbuf *)NULL, "uuname control message");
	fprintf(fd, "\n%s requested your uuname output\n", header.path);
	mclose(fd);
#endif
	fd = mailhdr(&header, "response to your senduuname request");
#ifdef UUPROG
	if (UUPROG[0] == '/')
		strcpy(buf, UUPROG);
	else
		sprintf(buf, "%s/%s", LIB, UUPROG);
#else
	strcpy(buf, "uuname");
#endif
	u = popen(buf, "r");
	if (fd != NULL && u != NULL) {
		while ((c=getc(u)) != EOF)
			putc(c, fd);
		pclose(u);
		mclose(fd);
	}
}

/*
 * Send the version number to the right person.
 */
c_version(argc, argv)
char **argv;
{
	register FILE *f;

#ifdef lint
	argc += **argv;
#endif lint

	f = mailhdr(&header, "Our news version");
	if (f == NULL)
		xerror("Cannot send back error message");
	fprintf(f, "\nCurrently running news version %s.\n\n", news_version);
	fprintf(f, "The header of your message follows:\n");
	hwrite(&header, f);
	mclose(f);
}

/*
 * Check the active file for old or missing newsgroups
 * Body of article is list of valid groups
 */
c_checkgroups(argc, argv)
char **argv;
{
	int rc;

#ifdef lint
	argc += **argv;
#endif lint

	setuid(geteuid());
	sprintf(bfr, "sed '1,/^$/d' %s | %s/checkgroups %s",
		ARTICLE, LIB, (TELLME && *TELLME) ? TELLME : NEWSUSR);
	rc = system(bfr);
	log("system(%s) status %d", bfr, rc);
}

/*
 * An unknown control message has been received.
 */
c_unknown(h, ctlmsgtext)
struct hbuf *h;
char *ctlmsgtext;
{
	register FILE *f;

	log("UNKNOWN Ctl Msg %s from %s", ctlmsgtext, h->path);
	f = mailhdr(h, "Unrecognized Control Message");
	if (f == NULL)
		xerror("Cannot send back error message");
	fprintf(f, "Currently running news B version %s.\n\n", news_version);
	fprintf(f, "The header of the message follows:\n");
	hwrite(h, f);
	mclose(f);
}

c_unimp(argc, argv)
char **argv;
{
	register FILE *f;

#ifdef lint
	argc += **argv;
#endif lint

	f = mailhdr(&header, "Unimplemented Control Message");
	if (f == NULL)
		xerror("Cannot send back error message");
	fprintf(f, "Currently running news B version %s.\n\n", news_version);
	fprintf(f, "The header of the message follows:\n");
	hwrite(&header, f);
	mclose(f);
}

xmitmsg(tosys, title, ng)
char *tosys, *title, *ng;
{
	struct hbuf h;
	struct srec srec;
	FILE *tfp;
	char *fname;

	/* Make an article called ARTICLE */
	sprintf(h.from, "%s@%s%s", "usenet", FULLSYSNAME, MYDOMAIN);
	strcpy(h.path, NEWSUSR);
	strcpy(h.nbuf, ng);
	strcpy(h.title, title);
	strcpy(h.ctlmsg, title);
	strcpy(h.subdate, "");
	strcpy(h.recdate, "");
	strcpy(h.expdate, "");
	getident(&h);
	dates(&h);
	tfp = xfopen(fname = mktemp("/tmp/xmsgXXXXXX"), "w");
	hwrite(&h, tfp);
	fclose(tfp);

	/* Find the sys record */
	s_openr();
	while (s_read(&srec)) {
		if (strncmp(srec.s_name, tosys, SNLN))
			continue;
		tfp = xfopen(fname, "r");
		transmit(&srec, tfp, 0);
		unlink(fname);
		return;
	}
	log("Can't find sys record for %s", tosys);
	xerror("Cannot find sys record");
}

/*
 * This is a modified version of popen, made more secure.  Rather than
 * forking off a shell, you get a bare process.  You must have exactly
 * one argument, and the command must be mail.
 */
#include <stdio.h>
#include <signal.h>
#define	RDR	0
#define	WTR	1
static	int	mopen_pid[20];
char *replyname();

FILE *
mhopen(hptr)
struct hbuf *hptr;
{
	int p[2];
	register myside, hisside, pid;
	char *sendto = NULL;

	if (hptr)
		sendto = replyname(hptr);
	else {
#ifdef NOTIFY
		if (TELLME && *TELLME)
			sendto = TELLME;
#endif NOTIFY
		if (sendto == NULL)
			return NULL;
	}
	verifyname(sendto);
	if(pipe(p) < 0)
		return NULL;
	myside = p[WTR];
	hisside = p[RDR];
	if((pid = fork()) == 0) {
		/* myside and hisside reverse roles in child */
		close(myside);
		close(0);
		dup(hisside);
		close(hisside);
		execl("/bin/mail", "mail", sendto, (char *)NULL);
		execl("/usr/bin/mail", "mail", sendto, (char *)NULL);
		execl("/usr/ucb/mail", "mail", sendto, (char *)NULL);
		_exit(1);
	}
	if(pid == -1)
		return NULL;
	mopen_pid[myside] = pid;
	close(hisside);
	return(fdopen(myside, "w"));
}

mclose(ptr)
FILE *ptr;
{
	register f, r, (*hstat)(), (*istat)(), (*qstat)();
	int status;

	f = fileno(ptr);
	fclose(ptr);
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	hstat = signal(SIGHUP, SIG_IGN);
	while((r = wait(&status)) != mopen_pid[f] && r != -1)
		;
	if(r == -1)
		status = -1;
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	signal(SIGHUP, hstat);
	return(status);
}

/*
 * mhopen a pipe to mail, write out a std header, and return the file ptr.
 *
 * We don't include a From: field because this is probably uucp, i.e.
 * explicitly routed.  Leave it up to the recipient's mailer.
 * Always include the To: field because if we ge back failed mail, we
 * might be able to deliver it by hand if we know to wom it was addressed.
 * By convention, hptr==NULL means to send the message to the local contact person.
 */
FILE *
mailhdr(hptr, subject)
struct hbuf *hptr;
char  *subject;
{
	FILE *fp;
	time_t now;
	char *to = "nobody";

	if (hptr)
		to = replyname(hptr);
#ifdef NOTIFY
	if (TELLME && *TELLME)
		to = TELLME;
#endif NOTIFY
	if ((fp = mhopen(hptr)) != NULL) {
		time(&now);
		fprintf(fp, "Date: %s\n", arpadate(&now));
		fprintf(fp, "To: %s\n", to);
		fprintf(fp, "Subject: %s\n", subject);
		fprintf(fp, "Responding-System: %s%s\n", FULLSYSNAME, MYDOMAIN);
	}
	return fp;
}

/*
 * verify that the name mail is being sent to does not contain any
 * nasty hooks to invoke funny functions from the shell or the like.
 */
verifyname(sendto)
char *sendto;
{
	/* Be sure we DO allow alphabetics, !, :, ., -, @. *. */
	char *nasty = "\"'\\`^|;& <>/~";
	register char *p;

	if (sendto[0] <= ' ') {
		log("nasty mail name %s from %s", sendto, header.path);
		xxit(1);
	}
	for (p=sendto; *p; p++) {
		if (*p == ' ') {
			*p = 0;
			break;
		}
	}
	while (*nasty) {
		if (index(sendto, *nasty++))
			xerror("nasty mail name %s from %s", sendto, header.path);
	}
	for (nasty = sendto; (nasty = index(nasty, '.')) != NULL; ) {
		if (*++nasty == '.')	/* check for .. */
			xerror("nasty mail name %s from %s", sendto, header.path);
	}
}

/*
 * Checks to make sure the control message is OK to post.
 */
ctlcheck()
{
	char msg[150];
	char *p;

	if (!is_ctl)
		return;

	if (header.ctlmsg[0])
		strcpy(msg, header.ctlmsg);
	else
		strcpy(msg, header.title);

	p = index(msg, ' ');
	if (p)
		*p = 0;

	if (strcmp(msg, "ihave") == 0) {
	} else if (strcmp(msg, "sendme") == 0) {
		return;	/* no restrictions */
	} else if (strcmp(msg, "newgroup") == 0) {
		suser();
	} else if (strcmp(msg, "rmgroup") == 0) {
		suser();
		checkpass("mTnyckAVEMXWk");
	} else if (strcmp(msg, "sendsys") == 0) {
		suser();
	} else if (strcmp(msg, "senduuname") == 0) {
		suser();
	} else if (strcmp(msg, "checkgroups") == 0) {
		suser();
	} else if (strcmp(msg, "version") == 0) {
		return;	/* no restrictions */
	} else if (strcmp(msg, "cancel") == 0) {
		return;	/* no restrictions at this level */
	} else if (strcmp(msg, "delsub") == 0) {
		if (!prefix(header.nbuf, "to.")) {
			printf("Must be in a 'to.system' newsgroup.");
			xxit(0);
		}
		return;
	} else {
		printf("Unrecognized control message - %s\n", msg);
		xxit(0);
	}
}

/* Make sure this guy is special. */
suser()
{
	if (uid == 0)
		return;
	if (uid == ROOTID)
		return;
	/*
	 * We assume that since our real uid is the same as NEWSUSR
	 * (the euid) we were run by rootid and it did a setuid.
	 * Too bad we can't set just the effective uid like suid does.
	 */
	if (uid == geteuid())
		return;
#ifdef IHCC
	printf("Please use the command:\n\ttoolnews providers\n");
	printf("then call one of the news people.\n");
#else
	printf("Get your local netnews contact to do it for you.\n");
#endif
	xxit(0);
}

/*
 * Demand a password from the user.
 */
checkpass(encpw)
char *encpw;
{
	char *crypt(), *getpass();

	if (strcmp(encpw, crypt(getpass("Password:"), "mT"))) {
		printf("Sorry\n");
		xxit(0);
	}
}
