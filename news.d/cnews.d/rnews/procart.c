/*
 * process a single incoming article
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"
#include "active.h"
#include "headers.h"
#include "system.h"

#ifndef COPYSIZE
#ifdef pdp11
#define COPYSIZE BUFSIZ
#else
#define COPYSIZE 8192
#endif				/* pdp11 */
#endif				/* COPYSIZE */

extern char *exclude;		/* for erik */

/*
 * Copy the article on "in" to a temporary name in the news spool directory,
 * unlink temp name; *or* copy into the final names, if known earlier enough.
 * (Sets h_tmpf in or near mungehdrs() or hdrdump().)
 * If the spool file opened, install the article it contains.
 */
int
cpinsart(in, inname, maxima)
FILE *in;
char *inname;
long maxima;
{
	int status = ST_OKAY;
	FILE *tf = NULL;
	struct headers hdrs;

	hdrinit(&hdrs);
	status |= copyart(in, inname, maxima, &tf, &hdrs);
	if (status&ST_NUKED) {			/* no good ngs (in fileart) */
		char *msgid = "", *ngs = "";

		status &= ~ST_NUKED;		/* nuking isn't serious */
		if (hdrs.h_msgid != NULL)
			msgid = hdrs.h_msgid;	/* caution */
		if (hdrs.h_ngs != NULL)
			ngs = hdrs.h_ngs;	/* caution */
		timestamp(stdout, (time_t *)NULL, (char **)NULL);
		(void) printf(" refused %s bad groups in `%s' and no junk group\n",
			msgid, ngs);
	} else if (tf == NULL) {
		warning("can't open spool file `%s'", hdrs.h_tmpf);
		status |= ST_DROPPED;
	} else {
		status |= insart(tf, &hdrs);
		(void) fclose(tf);
	}
	freeheaders(&hdrs);
	return status;
}

/*
 * Copy the next charcnt bytes of "in" (may be not a disk file)
 * to a permanent file under a (possibly) temporary name.
 * Must munge certain headers on the way & remember certain values.
 * mungehdrs() or hdrdump() sets hdrs->h_tmpf & *tfp.
 */
/* ARGSUSED inname */
static int
copyart(in, inname, charcnt, tfp, hdrs)
register FILE *in;
char *inname;
register long charcnt;
register FILE **tfp;
struct headers *hdrs;
{
	register int readcnt;
	int status = ST_OKAY;
	char *s = NULL;
	char line[COPYSIZE];

	hdrwretch();				/* reset the header parser */
	/*
	 * people think this loop is ugly; not sure why.
	 * if the byte count is positive, read a line; if it doesn't return
	 * EOF and is a header, then adjust byte count, eat and munge headers.
	 * strlen(line) must be computed before hdrmutate is called, as it
	 * removes newlines.
	 */
	while (charcnt > 0 &&
	    (s = fgets(line, (int)min(sizeof line-1, charcnt)+1, in)) != NULL
	    && ishdr(line)) {
	    	charcnt -= strlen(line);
		status |= hdrmutate(hdrs, line, tfp);	/* eat & munge headers */
	    	/* hdrdump() counts hdrs->h_charswritten */
	}
	hdrdeflt(hdrs);

	/* write any saved headers, trigger fileart */
	status |= hdrdump(tfp, hdrs, YES);

	/* Copy first body line. */
	if (charcnt > 0 && s != NULL) {	/* fgets worked: not a header line */
		register int linelen = strlen(line);

		if (*tfp != NULL && fputs(line, *tfp) == EOF)
			status = fulldisk(status|ST_DROPPED,
				(hdrs->h_unlink? hdrs->h_tmpf: hdrs->h_files));
		charcnt -= linelen;	
		hdrs->h_charswritten += linelen;
	}
	/*
	 * Copy at most "sizeof line" bytes at a time
	 * and exactly charcnt bytes in total, barring EOF.
	 */
	for (; charcnt > 0 && !(status&ST_DISKFULL) &&
	    (readcnt=fread(line, 1, (int)min(charcnt, sizeof line), in)) > 0;
	    charcnt -= readcnt, hdrs->h_charswritten += readcnt)
		if (*tfp != NULL && fwrite(line, 1, readcnt, *tfp) != readcnt)
			status = fulldisk(status|ST_DROPPED,
				(hdrs->h_unlink? hdrs->h_tmpf: hdrs->h_files));
	if (*tfp != NULL && fflush(*tfp) == EOF)	/* force to disk */
		status = fulldisk(status|ST_DROPPED,
			(hdrs->h_unlink? hdrs->h_tmpf: hdrs->h_files));
	if (charcnt > 0 && remote) {	/* TODO: don't use "remote" */
		(void) fprintf(stderr, "%s: article %s short by %ld bytes\n",
			progname, (hdrs->h_msgid != NULL? hdrs->h_msgid: ""),
			(long)charcnt);
		status |= ST_SHORT;	/* N.B.: do not uninstall this article */
	}
	return status;
}

/*
 * Read headers from "in".
 * Iff we haven't seen this article already and we like the groups,
 * install the article on "in" & hdrs->h_tmpf.
 * Rename hdrs->h_tmpf into the news spool directory.
 * Add history entries for the article.
 * Transmit the article, like a dose of clap, to our neighbours.
 * Process control mess(age)es.
 * Unlink hdrs->h_tmpf, if a temporary link.
 */
static int
insart(in, hdrs)
FILE *in;
struct headers *hdrs;
{
	register int status = ST_OKAY;

	status |= reject(hdrs);
	if (status&(ST_DROPPED|ST_NUKED))
		uninsart(hdrs);	/* remove existing links; give back assigned #s */
	else {
		/*
		 * Ordinary filing: if not already filed,
		 * make links to hdrs->h_tmpf (which will exist).
		 */
		status |= fileart(hdrs, &in, 0);	/* generates Xref: */
		if (!(status&(ST_DROPPED|ST_NUKED))) {
			status |= history(hdrs);	/* writes "received: " */
			/* writes systems on stdout */
			status |= transmit(hdrs, remote, exclude);
			(void) putc('\n', stdout);	/* ends the line */
			(void) fflush(stdout);		/* crash-proofness */
			if (hdrs->h_ctlcmd != NULL)
				status |= ctlmsg(hdrs);
		}
	}
    	status &= ~ST_NUKED;			/* nuking is quite casual */
	if (hdrs->h_unlink && unlink(hdrs->h_tmpf) < 0) {
		warning("can't unlink `%s'", hdrs->h_tmpf);
		status |= ST_ACCESS;
	}
	return status;
}

/*
 * Reject articles.  This can be arbitrarily picky.
 */
int
reject(hdrs)
register struct headers *hdrs;
{
	if (alreadyseen(hdrs->h_msgid)) {
		timestamp(stdout, (time_t *)NULL, (char **)NULL);
		(void) printf(" refused %s duplicate\n", hdrs->h_msgid);
	} else if (!ngmatch(oursys()->sy_ngs, hdrs->h_ngs)) {
		timestamp(stdout, (time_t *)NULL, (char **)NULL);
		(void) printf(" refused %s bad groups in %s\n",
			hdrs->h_msgid, hdrs->h_ngs);
	} else if (moderated(hdrs) && hdrs->h_approved == NULL) {
		timestamp(stdout, (time_t *)NULL, (char **)NULL);
		(void) printf(" refused %s unapproved article in moderated group(s) %s\n",
			hdrs->h_msgid, hdrs->h_ngs);
	} else
		return ST_OKAY;
	if (remote)		/* TODO: test this some other way */
		return ST_NUKED;
	else
		return ST_NUKED|ST_DROPPED;	/* more serious if local */
}

/*
 * Remove hdrs->h_files (permanent names) and h_tmpf (temporary names),
 * and return assigned article numbers.
 */
uninsart(hdrs)
register struct headers *hdrs;
{
	if (hdrs->h_unlink && hdrs->h_tmpf[0] != '\0') {	/* temp name */
		(void) unlink(hdrs->h_tmpf);	/* I don't wanna know... */
		hdrs->h_unlink = NO;
	}
	(void) snuffmayreturn(hdrs->h_files, YES);
}

int
snufffiles(filelist)		/* just unlink all files in filelist */
char *filelist;
{
	return snuffmayreturn(filelist, NO);
}

int
snuffmayreturn(filelist, artret)	/* unlink all files in filelist (return artids?) */
char *filelist;
int artret;
{
	register char *arts, *spacep, *slashp;
	int status = ST_OKAY;
	char artnm[MAXFILE];

	/* this is a deadly tedious job and I really should automate it */
	for (arts = filelist; arts != NULL && arts[0] != '\0';
	     arts = (spacep == NULL? NULL: spacep+1)) {
		spacep = index(arts, ' ');
		if (spacep != NULL)
			spacep[0] = '\0';	/* will be restored below */
	     	(void) strcpy(artnm, arts);
	     	if (spacep != NULL)
	     		spacep[0] = ' ';	/* restore space */

	     	slashp = index(artnm, FNDELIM);
	     	if (slashp != NULL)
	     		slashp[0] = '\0';	/* will be restored below */
	     	if (artret)
			(void) prevartnum(artnm);	/* return assigned # */
	     	if (slashp != NULL)
	     		slashp[0] = FNDELIM;	/* restore slash */

	     	mkfilenm(artnm);
		if (unlink(artnm) < 0) {	/* remove a link & hope */
			(void) fprintf(stderr, "%s: can't unlink %s\n",
				progname, artnm);
	     		status |= ST_ACCESS;
		}
	}
	return status;
}
