/*
 * relaynews - relay Usenet news (version C)
 * See the file COPYRIGHT for the copyright notice.
 *
 * relaynews should be setuid-news, setgid-news.  You'll need to install
 * setnewsids setuid-root if setuid(geteuid()) doesn't work on your
 * machine (e.g. on V7 and possibly SystemIII).
 *
 * Written by Geoff Collyer, 15-20 November 1985 and revised periodically
 * since.
 *
 * relaynews parses Newsgroups: headers, rejects articles by newsgroup &
 * message-id, files articles, updates the active & history files,
 * transmits articles, and honours (infrequent) control messages, which do
 * all sorts of varied and rococo things.  Control messages are implemented
 * by separate programs.  relaynews reads a "sys" file to control the
 * transmission of articles but can function as a promiscuous leaf node
 * without one.
 *
 * A truly radical notion: people may over-ride via environment variables
 * the compiled-in default directories so IHCC kludges are not needed and
 * testing is possible (and encouraged) in alternate directories.  This
 * does cause a loss of privilege, to avoid spoofing.
 *
 * The disused I-have/send-me protocol is going to work; it's been broken
 * in B news for ages but no one has noticed because it's essentially
 * useless on the uucp network, especially when batching news articles,
 * but NNTP may breathe new life into it.
 *
 * Portability vs SystemV.  relaynews uses dbm(3) and makes no apologies
 * for so doing.  Imitation UNIX (registered trademark of AT&T in the
 * United States) brand operating systems that lack dbm are going to
 * have to use my incredibly slow dbm simulation.
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>		/* to make locking safe */
#include <sys/types.h>

#include "news.h"
#include "newspaths.h"
#include "active.h"
#include "cpu.h"
#include "headers.h"
#include "system.h"

/*
 * setuid-root program to set ids to news/news & rexec rnews with
 * NEWSPERMS in the environment to break loops.
 */
#ifndef SETNEWSIDS
#define SETNEWSIDS "setnewsids"
#endif

#ifndef NEWSUSER
#define NEWSUSER "news"
#endif
#ifndef NEWSGROUP
#define NEWSGROUP "news"
#endif

char *progname;

int remote = NO;			/* articles are being relayed? */

char *exclude = NULL;			/* site to exclude, for erik */
static int userealids = NO;

/*
 * main - parse arguments and handle options, lock & unlock news system.
 */
main(argc, argv)
int argc;
char *argv[];
{
	int c, errflg = 0;
	int status = ST_OKAY;
	char origdir[MAXFILE];		/* current directory at start */
	char *newpath;
	extern int optind;
	extern char *optarg;

	progname = argv[0];

	/* setuid daemon prelude; various precautions */
	(void) umask(newsumask());	/* undo silly umasks */
	(void) alarm(0);		/* cancel any pending alarm */
	/*
	 * Reset certain environment variables to sane values.
	 */
	newpath = malloc(STRLEN("PATH=") + STRLEN(STDPATH) + 1);
	if (newpath == NULL)
		exit(1);		/* no chatter until stdfdopen */
	(void) strcpy(newpath, "PATH=");
	(void) strcat(newpath, STDPATH);
	if (!putenv(newpath) ||
	    !putenv("IFS= \t\n"))
		exit(1);		/* no chatter until stdfdopen */
	closeall(1);			/* closes all but std descriptors */
	stdfdopen();			/* ensure standard descriptors are open */

	setids(argv);			/* change of real and effective ids */
	/* we are now running as news, so you can all relax */

	/* ignore signals (for locking). relaynews runs quickly, so don't worry. */
	(void) signal(SIGINT, (sigarg_t)SIG_IGN);
	(void) signal(SIGQUIT, (sigarg_t)SIG_IGN);
	(void) signal(SIGHUP, (sigarg_t)SIG_IGN);
	(void) signal(SIGTERM, (sigarg_t)SIG_IGN);

	/* parse options & set flags */
	while ((c = getopt(argc, argv, "pd:x:")) != EOF)
		switch (c) {
		case 'p':		/* "rnews" mode: */
			++remote;	/* just relay, don't fuck about */
			break;
		/* all options below are new in C news */
		case 'd':		/* -d debug-options; thanks, henry */
			if (!debugon(optarg))
				errflg++;	/* debugon already complained */
			break;
		case 'x':			/* -x site: don't send to site */
			/* you're welcome, erik */
			if (exclude != NULL) {
				(void) fprintf(stderr,
					"%s: more than one -x site (%s)\n",
					progname, optarg);
				errflg++;
			} else
				exclude = optarg;
			break;
		default:
			errflg++;
			break;
		}
	if (errflg) {
		(void) fprintf(stderr, "usage: %s [-p][-d fhlmt][-x site]\n",
			progname);
		exit(2);
	}

	/* lock the news system, set up log files */
	newslock();			/* done here due to dbm internal cacheing */
	if (remote) {		/* TODO: test this some other way */
		redirectlogs();		/* rnews daemon: redirect to logs */
#ifdef MANYERRORS
		(void) putc('\n', stderr);	/* leave a blank line */
		/* prints "Jun  5 12:34:56" */
		timestamp(stderr, (time_t *)NULL, (char **)NULL);
		(void) putc('\n', stderr);
#endif
	}

	/* process file name arguments */
#ifdef RELATIVE_FILES_ALLOWED
	if (getwd(origdir) == 0)
#endif
		(void) strncpy(origdir, "/dunno/man/like/somewhere.", MAXFILE);
	cd(fullartfile((char *)NULL));		/* move to spool directory */

	if (optind == argc)
		status |= process(stdin, "stdin");
	else
		for (; optind < argc; optind++)
			status |= relnmprocess(argv[optind], origdir);

	trclose();			/* close open batch files */
	status |= synccaches();		/* just being cautious */
	newsunlock();			/* unlock the news system */
	exit(status);
}

setids(argv)				/* change of real and effective ids */
char **argv;
{
	int newsuid = getuid(), newsgid = getgid();	/* default to real ids */

	(void) ctlfile((char *)NULL);	/* trigger unprivileged(), set userealids */
	if (!userealids) {
		/*
		 * If setuid(geteuid()) fails, try execing a small,
		 * setuid-root program to just do getpwnam(), getgrnam()
		 * (with NEWSPERMS set), setgid(), setuid(),
		 * and exec this program again.  If NEWSPERMS is set,
		 * the failure is a fatal error (recursive loop).
		 * Then this program can be setuid-news.
		 */
		(void) setgid(getegid());
		if (setuid(geteuid()) < 0) {
			if (getenv("NEWSPERMS") != 0)
				error("recursive loop setting ids", "");
			execv(libfile(SETNEWSIDS), argv);
			error("can't exec %s to set ids", libfile(SETNEWSIDS));
			/* NOTREACHED */
		}
		/* you can relax, we are now running as news */
	} else {
		(void) setgid(newsgid);
		(void) setuid(newsuid);
	}
	/* we are now running as news, so you can all relax */
}

void
unprivileged()			/* called if NEWSARTS, NEWSCTL or NEWSBIN present */
{
	userealids = YES;
}

int					/* YES/NO */
debugon(dbopt)
register char *dbopt;
{
	int status = YES;

	for (; *dbopt != '\0'; dbopt++)
		switch (*dbopt) {
		case 'f':
			filedebug(YES);
			break;
		case 'h':
			hdrdebug(YES);
			break;
		case 'l':
			lockdebug(YES);
			break;
		case 'm':
			matchdebug(YES);
			break;
		case 't':
			transdebug(YES);
			break;
		default:
			status = NO;	/* unknown debugging option */
			(void) fprintf(stderr, "%s: bad -d %c\n",
				progname, *dbopt);
			break;
		}
	return status;
}

/*
 * Redirect stdout and stderr into log files at known locations.
 */
redirectlogs()
{
	logfile(stdout, libfile("log"));
	logfile(stderr, libfile("errlog"));
}

logfile(stream, name)			/* redirect stream into name */
FILE *stream;
char *name;
{
	if (freopen(name, "a", stream) == NULL)
		errunlock("can't redirect standard stream to %s", name);
}

int					/* status */
relnmprocess(name, origdir)		/* process a (relative) file name */
char *name, *origdir;
{
	register int status = ST_OKAY;
	register FILE *in;
	register char *fullname;

	fullname = emalloc((unsigned)strlen(origdir) + STRLEN(SFNDELIM) +
		strlen(name) + 1);
	fullname[0] = '\0';

	if (name[0] != FNDELIM) {	/* relative path */
		(void) strcat(fullname, origdir);
		(void) strcat(fullname, SFNDELIM);
	}
	(void) strcat(fullname, name);

	in = fopen(fullname, "r");
	if (in == NULL)
		warning("can't open argument `%s'", fullname);
	else {
		status |= process(in, fullname);
		(void) fclose(in);
	}
	free(fullname);
	return status;
}

/*
 * process - process input file
 * If it starts with '#', assume it's a batch and unravel it,
 * else it's a single article, so just inject it.
 */
int
process(in, inname)
FILE *in;
char *inname;
{
	register int c;

	if ((c = getc(in)) == EOF)
		return ST_OKAY; 		/* normal EOF */
	(void) ungetc(c, in);
	if (c == '#')
		return unbatch(in, inname);
	else
		/* ST_SHORT should always come on with a count of MAXLONG */
		return cpinsart(in, inname, MAXLONG) & ~ST_SHORT;
}

/*
 * Unwind in and insert each article.
 * For each article, call cpinsart to copy the article
 * from in into a temporary file and rename the temp file
 * into the news spool directory.
 */
int
unbatch(in, inname)
FILE *in;
char *inname;
{
	register int c;
	long charcnt;
	int status = ST_OKAY;
	char line[MAXLINE];

	while (!(status&ST_DISKFULL) && (c = getc(in)) != EOF) {
		(void) ungetc(c, in);
		/*
		 * While out of sync, eat input lines,
		 * then eat the tail end of the "#! rnews" line.
		 */
		while (fgets(line, sizeof line, in) != NULL &&
		    !batchln(line, &charcnt)) {
			status |= ST_DROPPED;		/* argh! a bad batch */
			(void) fprintf(stderr, "%s: unbatcher out of synch, tossing: ",
				progname);
		    	(void) fputs(line, stderr);
		}
		if (!feof(in))
			status |= cpinsart(in, inname, charcnt);
	}
	return status;
}

/*
 * Is line a batcher-produced line (#! rnews count)?
 * If so, return the count through charcntp.
 * This is slightly less convenient than sscanf, but a lot smaller.
 */
int					/* YES/NO */
batchln(line, charcntp)
register char *line;
register long *charcntp;
{
	register char *countp;
	static char batchtext[] = "#! rnews ";

	countp = line + STRLEN(batchtext);
	if (STREQN(line, batchtext, STRLEN(batchtext)) &&
	    isascii(*countp) && isdigit(*countp)) {
		*charcntp = atol(countp);
		return YES;
	} else {
		*charcntp = 0;
		return NO;
	}
}
