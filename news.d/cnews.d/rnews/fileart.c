/*
 * fileart - file an article, given its temp file name and its headers
 * TODO: change in junk policy, suggested by gnu (has this been done already?):
 *	3 classes of ng: wanted, not wanted (!ng in sys -> ignore it),
 *		don't know it (not in active -> change to junk).
 * TODO: clean up packaging of fileart: push the 3-arg open caller into vers/*?
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include "fcntl.h"			/* try to define O_* and F_* */

#include "news.h"
#include "newspaths.h"
#include "active.h"
#include "headers.h"
#include "system.h"

#ifndef F_OK
#define F_OK 0
#endif

#define JUNK "junk"			/* name of lost+found ng. */
#define CONTROL "control"

static long artnum;			/* asgnartnum sets artnum */
static int goodngs;			/* asgnartnum reads goodngs */

static int debug = NO;

filedebug(state)		/* set debugging state */
int state;
{
	debug = state;
}

/*
 * TODO: new proposed junk policy:
 * delay until the end, then file *once* in junk if any ngs
 * were not in active but were in sys file subscription list.
 */
/*
 * File the article in "*tfp" in the spool directory.
 * hdrs are the associated headers.  fileart fills in hdrs->h_files too.
 *
 * If openfirst is true, fill in h_tmpf with the name of the first link,
 * fopen it (into *tfp), and make any remaining links.
 * openfirst really means "Newsgroups:" has been seen.
 * Generates Xref: header if needed.
 */
int
fileart(hdrs, tfp, openfirst)
register struct headers *hdrs;
FILE **tfp;
int openfirst;
{
	register char *comma;
	int status = ST_OKAY;
	char *ng;			/* point at current newsgroup */
	char artnumstr[MAXCOMP];	/* article number in ascii */
	char group[MAXFILE];		/* a group */

	if (hdrs->h_filed)
		return status;
	artnum = 0;
	goodngs = 0;
	/*
	 * Store in spooldir.
	 * Link temp file to spooldir/ng/article-number for each ng.
	 */
	for (ng = hdrs->h_ngs; ng != NULL; ng = comma) {
		int lstatus = ST_OKAY;

		comma = index(ng, NGSEP);
		if (comma != NULL)
			*comma = '\0';		/* restored below */
		if (hdrs->h_ctlcmd != NULL)	/* ctl. msg.s go in CONTROL */
			(void) strcpy(group, CONTROL);
		else
			(void) strcpy(group, ng); /* copy out newsgroup name */
		if (comma != NULL)
			*comma++ = NGSEP;	/* step past comma */

		lstatus |= asgnartnum(hdrs, tfp, openfirst, group, artnumstr);
		/*
		 * No such group in active or link failed but our
		 * subscription list permits this group,
		 * so file it under "junk", if it exists.
		 */
		if ((artnum < 1 || lstatus != ST_OKAY) &&
		    ngmatch(oursys()->sy_ngs, group)) {
			(void) strcpy(group, JUNK);
			lstatus = asgnartnum(hdrs, tfp, openfirst,
				group, artnumstr);
			/*
			 * You could set ST_DROPPED here if you think
			 * one might accidentally not have "junk"
			 * in active.  I tend to think the absence
			 * of "junk" would be deliberate, to prevent
			 * filing of junk articles.
			 */
			if (artnum < 1 || lstatus != ST_OKAY)
				lstatus |= ST_NUKED;	/* no junk ng */
		}
		if (artnum >= 1 && lstatus == ST_OKAY) {
			/*
			 * Article # was assigned and the link succeeded.
			 * Update hdrs->h_files list for history.
			 */
			hdrs->h_filed = YES;			/* make a note */
			if (hdrs->h_files[0] != '\0')
				(void) strcat(hdrs->h_files, " ");
			(void) strcat(hdrs->h_files, group);	/* normal case */
			(void) strcat(hdrs->h_files, SFNDELIM);
			(void) strcat(hdrs->h_files, artnumstr);
			++goodngs;
		}
		status |= lstatus & ~ST_NUKED;
	}
	/*
	 * No good ngs if article was accepted by our subscription list,
	 * yet none of the groups are in our active file
	 * (e.g. net.rec.drugs,net.chew-the-fat) & no junk group exists.
	 *
	 * TODO: could call asgnartnum here instead.  Hmmm...
	 * Current policy makes a junk link for each bad group,
	 * this would instead make one junk link, no matter how
	 * many bad groups, and only if all are bad.
	 * But we want to know *why* there are no good ngs; it could
	 * be because they are all denied by our subscription list.
	 * Again, could set ST_DROPPED here (no junk group).
	 */
	if (goodngs == 0)
		status |= ST_NUKED;		/* TODO: complain: no good ngs? */
	else if (goodngs > 1 && *tfp != NULL)	/* cross-posted? */
		status |= emitxref(*tfp, hdrs);
	return status;
}

/*
 * Assign a permanent name and article number to the temporary name hdrs->h_tmpf
 * in newsgroup "ng" & store the ascii form of the article number into "artnumstr",
 * returning the article number in "artnum".
 *
 * If openfirst is true and goodngs is zero, set inname to artname,
 * fopen artname and store the result through tfp.
 */
static int
asgnartnum(hdrs, tfp, openfirst, ng, artnumstr)
struct headers *hdrs;
FILE **tfp;
int openfirst;
char *ng;
char *artnumstr;
{
	register int status = ST_OKAY;
	char *inname = hdrs->h_tmpf;
	char slashng[MAXFILE];			/* a group, slashed */
	extern int errno;

#define openorlink(inname, artname, tfp, openfirst, goodngs) \
    (openfirst && goodngs == 0? openlink(inname, artname, tfp): \
    link(inname, artname) == 0)

	(void) strcpy(slashng, ng);
	mkfilenm(slashng);
	while ((artnum = nxtartnum(ng)) >= 1) {
		char artname[MAXFILE];		/* article file name */

		(void) strcpy(artname, slashng);
		(void) strcat(artname, SFNDELIM);
		(void) sprintf(artnumstr, "%ld", artnum);
		(void) strcat(artname, artnumstr);

		/*
		 * we changed directory to spooldir in main(),
		 * so artname is relative to spooldir,
		 * therefore artname can be used as is.
		 */
#ifdef notdef
		(void) strcpy(artname, spoolfile(artname));
#endif

		if (debug && !(openfirst && goodngs == 0))
			(void) fprintf(stderr, "about to link %s to %s... ",
				inname, artname);
		if (openorlink(inname, artname, tfp, openfirst, goodngs)) {
			if (debug)
				(void) fprintf(stderr, "success!\n");
			break;		/* link succeeded */
		} else {
			/*
			 * Link failed.  Maybe some directories are missing,
			 * so create any missing directories and try again.
			 */
			if (debug)
				warning("failed!", "");
			(void) checkdir(artname, getuid(), getgid());
			if (openorlink(inname, artname, tfp, openfirst, goodngs))
				break;		/* link succeeded this time */
			else if (errno != EEXIST) {
				warning("can't link to %s", artname);
				status |= ST_DROPPED;
				break;
			}
			/*
			 * Else artname exists.  It must be a numeric subgroup name,
			 * such as net.micro.432; try another article.
			 */
		}
	}
	return status;
}

/*
 * Open artname, save the name on sp & the FILE pointer through tfp.
 */
static int
openlink(sp, artname, tfp)
char *sp;
char *artname;
FILE **tfp;
{
	(void) strcpy(sp, artname);	/* save a copy */
#ifdef O_EXCL
	/* This is the cheaper way. */
	{
	int fd = open(sp, O_WRONLY|O_CREAT|O_EXCL, 0666);

	if (fd < 0)
		*tfp = NULL;
	else
		*tfp = fdopen(fd, "w");
	}
#else
	if (access(sp, F_OK) >= 0)	/* sp exists */
		*tfp = NULL;		/* refuse to write on it */
	else		
		*tfp = fopen(sp, "w");	/* try to create it */
#endif
	return *tfp != NULL;
}
