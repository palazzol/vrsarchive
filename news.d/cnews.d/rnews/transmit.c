/*
 * transmit - transmit incoming articles to neighbouring machines
 * TODO: deal with multiple references to the same batch file.
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include "news.h"
#include "newspaths.h"
#include "headers.h"
#include "system.h"

#define NOPENTFS 10	/* # of file descriptors kept open for batching */

static FILE *tfs[NOPENTFS];			/* keep these open always */
static int debug = NO;

transdebug(state)
int state;
{
	debug = state;
}

/*
 * For each system in "sys" other than this one,
 * transmit this article when its ng pattern matches
 * hdrs->h_distr (which may be just a copy of hdrs->h_ngs).
 */
int
transmit(hdrs, rmt, exclude)
register struct headers *hdrs;
int rmt;					/* inews/rnews flag */
char *exclude;					/* no copy to him */
{
	register struct system *sys;
	register int fno = 0;
	int status = 0;

	rewsys();
	if (debug)
		(void) fprintf(stderr, "just rewound sys file\n");
	while ((sys = nextsys()) != NULL) {
		if (debug) {
			(void) fprintf(stderr, "hdrs->h_distr=%s\n",
				hdrs->h_distr);
			(void) fprintf(stderr, "sy_name=%s sy_ngs=%s\n",
				sys->sy_name, sys->sy_ngs);
		}
		if (oktransmit(hdrs, sys, sys->sy_flags, rmt, exclude))
			status |= ejaculate(hdrs, sys, fno);
		if (sys->sy_flags&(FLG_BATCH|FLG_SZBATCH))
			++fno;		/* count lines with F or f flag */
	}
	if (debug)
		(void) fprintf(stderr, "just finished reading sys file\n");
	return status; 			
}

/*
 * Is it okay to send the article corresponding to hdrs to sys,
 * given flags (derived from sys) and rmt?
 *
 * Never send to this host, nor any host named in Path:.
 * Newsgroups must match sys's subscription list.
 * Also, Distribution: must match sys's distribution list.
 * If L flag is on, must be a local posting.
 *
 * TODO: implement Ln restriction:
 *	forward articles generated within sysp->sy_lochops hops of here.
 * TODO: implement exclusions by site, from sy_excl (can be NULL).
 */
static int
oktransmit(hdrs, sys, flags, rmt, exclude)
register struct headers *hdrs;
register struct system *sys;
int flags, rmt;
char *exclude;				/* no copy to him */
{
	return (!(flags&FLG_LOCAL) || !rmt) &&		/* local & */
	    !STREQ(hostname(), sys->sy_name) &&		/* not to ME & */
	    (exclude == NULL || !STREQ(exclude, sys->sy_name)) &&	/* not excluded & */
	    (hdrs->h_path == NULL || !hostin(hdrs->h_path, sys->sy_name)) &&	/* not been here & */
	    ngmatch(sys->sy_ngs, hdrs->h_ngs) &&	/* ngs match & */
	    /* RFC 850 is wrong, yea, verily: Distribution:s are *not* patterns */
	    ngmatch(sys->sy_distr, hdrs->h_distr);	/* distrs match! */
}

/*
 * send the article denoted by hdrs to the system denoted by sys.
 */
STATIC int				/* status */
ejaculate(hdrs, sys, fno)		/* kick the article to its reward */
register struct headers *hdrs;
register struct system *sys;
int fno;
{
	int status = ST_OKAY;
	char filename[MAXLINE];
	char *fullname;

    	/* can't use hdrs->h_tmpf because we need a permanent name */
	first(hdrs->h_files, filename);
	mkfilenm(filename);
    	(void) printf(" %s", sys->sy_name);	/* logging */
	if (debug)
		(void) fprintf(stderr, "transmitting %s to %s\n",
			hdrs->h_msgid, sys->sy_name);

    	/* must supply a full pathname to the outside world */
    	fullname = fullspoolfile(filename);
	if (sys->sy_flags&(FLG_BATCH|FLG_SZBATCH))
    		status |= trbatch(sys, hdrs, fullname, fno);
	else
    		status |= trcmd(sys, fullname);
	return status;
}

/*
 * Append filename to sys->sy_cmd. fno is the ordinal number of this sys line.
 * If fno is low enough, use the tfs cache of batch file descriptors.
 */
int
trbatch(sys, hdrs, filename, fno)
struct system *sys;
struct headers *hdrs;
char *filename;
register int fno;
{
	register int status = 0;
	char *batfile = sys->sy_cmd;

	if (fno >= NOPENTFS) {				/* not cachable */
		register FILE *batchf = fopenclex(batfile, "a");

		if (batchf == NULL) {
			warning("can't open batch file %s", batfile);
			status |= ST_DROPPED;
		} else {
			status |= trappend(batchf, sys, hdrs, filename);
			if (fclose(batchf) == EOF)
				status = fulldisk(status|ST_DROPPED, batfile);
		}
	} else {					/* cachable */
		if (tfs[fno] == NULL) {			/* closed */
			tfs[fno] = fopenclex(batfile, "a");
			if (tfs[fno] == NULL) {		/* didn't open */
				register int openf;

				/*
				 * Assume open failed due to lack of file
				 * descriptors.  Find an open one and close it,
				 * then retry the open.  Honk at someone too?
				 */
				for (openf = 0; openf < NOPENTFS; openf++)
					if (tfs[openf] != NULL)	/* open */
						break;
				if (openf < NOPENTFS && tfs[openf] != NULL) {
					if (fclose(tfs[openf]) == EOF)
						status = fulldisk(status|ST_DROPPED,
							"some batch file");
					tfs[openf] = NULL;	/* mark closed */
					tfs[fno] = fopenclex(batfile, "a");
				}
			}
		}
		if (tfs[fno] == NULL) {			/* still closed! */
			warning("can't open batch file %s", batfile);
			status |= ST_DROPPED;
		} else
			status |= trappend(tfs[fno], sys, hdrs, filename);
	}
	return status;
}

static int
trappend(fp, sys, hdrs, name)			/* write name\n on fp */
register FILE *fp;
register struct system *sys;
register struct headers *hdrs;
char *name;
{
	int status = ST_OKAY;

	if (fputs(name, fp) == EOF)	/* append to batch file */
		status = fulldisk(status|ST_DROPPED, "some batch file");
	/* for Henry's new batcher */
	if (sys->sy_flags&FLG_SZBATCH &&
	    fprintf(fp, " %ld", hdrs->h_charswritten) == EOF)
		status = fulldisk(status|ST_DROPPED, "some batch file");
	/* don't check putc return value for portability; use ferror */
	(void) putc('\n', fp);
	(void) fflush(fp);		/* for crash-proofness */
	if (ferror(fp))
		status = fulldisk(status|ST_DROPPED, "some batch file");
	return status;
}

/*
 * Execute sys->sy_cmd with the current article as stdin
 * and filename substituted for %s in sys->sy_cmd (if any).
 */
int
trcmd(sys, filename)
struct system *sys;
char *filename;
{
	int status = ST_OKAY, exitstat;
	char *cmd;
	char *syscmd = sys->sy_cmd;

	cmd = emalloc((unsigned)STRLEN("PATH=") + STRLEN(STDPATH) + STRLEN(" <") +
		strlen(filename) + STRLEN(" ") + strlen(syscmd) +
		strlen(filename) + 1);
	(void) strcpy(cmd, "PATH=");
	(void) strcat(cmd, STDPATH);
	(void) strcat(cmd, " <");
	/*
	 * redirect stdin to prevent consuming my stdin & so cmd's stdin
	 * is filename by default.
	 */
	(void) strcat(cmd, filename);
	(void) strcat(cmd, " ");
	(void) sprintf(cmd+strlen(cmd), syscmd, filename);
	exitstat = system(cmd);
	if (exitstat != 0) {
		extern char *progname;

		status |= ST_DROPPED;
		(void) fprintf(stderr, "%s: `%s' returned exit status 0%o\n",
			progname, cmd, exitstat);
	}
	free(cmd);
	return status;
}

trclose()
{
	register int fno;

	for (fno = 0; fno < NOPENTFS; fno++)
		if (tfs[fno] != NULL) {
			(void) fclose(tfs[fno]);
			tfs[fno] = NULL;
		}
}

/*
 * Return true iff host appears in s, with no characters from the alphabet
 * of legal hostname characters immediately adjacent.
 * This function is a profiling hot spot, so it has been optimised.
 */
int
hostin(s, host)
register char *s, *host;
{
	register int hostlen = strlen(host);
	register int ch;		/* use by hostchar macro */

/* If c is NUL, hostchar will be false, so don't test (optimisation: ==). */
#define nothostchar(c) (!hostchar(c) /* || (c) == '\0' */ )	/* ! or EOS */
/*
 * True if c can be part of a hostname. RFC 850 allows letters, digits, periods,
 * and hyphens and specifically disallows blanks. False may mean c is NUL.
 */
#define hostchar(c) (ch = (c) , \
	(isascii(ch) && (isalnum(ch) || (ch) == '.' || (ch) == '-')))

	/*
	 * Special case: match host!path or host.
	 */
	if (STREQN(s, host, hostlen) && nothostchar(s[hostlen]))
		return YES;
	/*
	 * Match path2!host!path or path2!host.
	 */
	while (*s != '\0')
		if (hostchar(s[0]))		/* can't start after here */
			++s;
		else if ((++s, STREQN(s, host, hostlen)) &&
		    nothostchar(s[hostlen]))
			return YES;
	return NO;
}
