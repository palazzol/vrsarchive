/*
 * news sys file reading functions (in-memory version)
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "news.h"
#include "newspaths.h"
#include "system.h"

static FILE *fp = NULL;		/* descriptor for libfile("sys") */
static char filerelname[] = "sys";

static struct system *firstsys = NULL;	/* cache */
static struct system *currsys = NULL;	/* current system */

/* forward decls */
char *parsecolon();

struct system *
oursys()			/* return our sys entry */
{
	register struct system *sys;
	static struct system fakesys;
	static struct system *thissys = NULL;

	if (thissys != NULL)		/* in-core-only optimisation */
		return thissys;
	rewsys();
	while ((sys = nextsys()) != NULL && !STREQ(sys->sy_name, hostname()))
		;
	if (sys == NULL) {		/* no entry; cook one up */
		/* TODO: test this by some other means instead */
		if (!remote)		/* no -p, local posting */
			(void) fprintf(stderr,
		"%s: no %s file - your news will not leave this machine\n",
				progname, libfile(filerelname));
		fakesys.sy_name = hostname();
		fakesys.sy_ngs = "all";
		fakesys.sy_flags = 0;
		fakesys.sy_lochops = 0;
		fakesys.sy_cmd = "";
		fakesys.sy_next = NULL;
		sys = &fakesys;
	}
	thissys = sys;			/* for future reference */
	return sys;
}

/*
 * Returned pointer points at a static struct whose members
 * point at static storage.
 */
struct system *
nextsys()				/* return next sys entry */
{
	struct system *retsys;

	if (firstsys == NULL && fp == NULL)
		if ((fp = fopenwclex(libfile(filerelname), "r")) == NULL)
			return NULL;
	if (fp != NULL && firstsys == NULL)	/* file open, no cache */
		readsys();			/* read & parse fp */
	retsys = currsys;			/* save current ptr. */
	if (currsys != NULL)
		currsys = currsys->sy_next;	/* advance current ptr. */
	return retsys;
}

rewsys()
{
	currsys = firstsys;
}

static char *curr, *next;			/* parsing state */

STATIC
readsys()
{
	register char *sysline;

	rewind(fp);
	/* read possibly continued lines of arbitrary length */
	while ((sysline = cfgetms(fp)) != NULL) {
		if (sysline[0] != '#' && sysline[0] != '\n') {	/* not a comment */
			register struct system *sysp;
			register char *slashp;
			char *flagstring;

			/* This storage is never freed. */
			sysp = (struct system *) malloc(sizeof *sysp);
			if (sysp == NULL)
				errunlock("out of memory for system structs", "");

			/* parse into sysp */
			trim(sysline);
			next = sysline;
			parse(&sysp->sy_name);
			parse(&sysp->sy_ngs);
			parse(&flagstring);
			parse(&sysp->sy_cmd);
			/* could check for extra fields here */

			parseflags(flagstring, sysp);
			free(flagstring);		/* malloced by parse */
			sysp->sy_next = NULL;

			/* reparse for embedded slashes */
			slashp = index(sysp->sy_name, '/');
			if (slashp != NULL) {		/* parse name/excl1,excl2,... */
				*slashp = '\0';		/* terminate name */
				sysp->sy_excl = slashp + 1;
			} else
				sysp->sy_excl = NULL;	/* no exclusions */
			slashp = index(sysp->sy_ngs, '/');
			if (slashp != NULL) {		/* parse ngs/distrs */
				*slashp = '\0';		/* terminate ngs */
				sysp->sy_distr = slashp + 1;
			} else
				sysp->sy_distr = sysp->sy_ngs;

			/* expand ME if any */
			if (STREQ(sysp->sy_name, "ME")) {
				free(sysp->sy_name);	/* malloced by parse */
				sysp->sy_name = hostname();	/* NB not malloced */
			}

			/* fill in any defaults */

			/*
			 * If no batch file name was given, use the default
			 * ($NEWSCTL/batch/b.system/togo).
			 */ 
			if (sysp->sy_flags&FLG_BATCH && sysp->sy_cmd[0] == '\0') {
				char *deffile = emalloc((unsigned)STRLEN("batch/b.") +
					strlen(sysp->sy_name) + STRLEN("/togo") + 1);

				(void) strcpy(deffile, "batch/b.");
				(void) strcat(deffile, sysp->sy_name);
				(void) strcat(deffile, "/togo");
				free(sysp->sy_cmd);	/* malloced by parse */
				sysp->sy_cmd = libfile(deffile); /* NB not malloced */
				free(deffile);
			}
			/*
			 * If no command was given, use the default
			 * (uux - -r -z system!rnews).
			 * (This *is* yucky and uucp-version-dependent.)
			 */ 
			if (!(sysp->sy_flags&FLG_BATCH) && sysp->sy_cmd[0] == '\0') {
				/* TODO: send mail to usenet, harassing him. */
				/* TODO: search PATH including $NEWSCTL/syscmd */
				free(sysp->sy_cmd);	/* malloced by parse */
				sysp->sy_cmd = emalloc((unsigned)STRLEN("uux - -r -z ") +
					strlen(sysp->sy_name) + STRLEN("!rnews") + 1);
				(void) strcpy(sysp->sy_cmd, "uux - -r -z ");
				(void) strcat(sysp->sy_cmd, sysp->sy_name);
				(void) strcat(sysp->sy_cmd, "!rnews");
			}

			/* stash *sysp away on the tail of the current list */
			if (firstsys == NULL)
				firstsys = sysp;		/* 1st system */
			else
				currsys->sy_next = sysp;	/* tack on tail */
			currsys = sysp;
		}
		free(sysline);
	}
	(void) fclose(fp);		/* file no longer needed */
	fp = NULL;			/* mark file closed */
	rewsys();
}

STATIC
parse(into)
register char **into;
{
	curr = next;
	if (curr == NULL)
		*into = strsave("");
	else {
		next = parsecolon(curr);
		*into = strsave(curr);
	}
	/* *into is never freed. */
	if (*into == NULL)
		errunlock("out of memory for sys strings", "");
}

STATIC char *
parsecolon(line)		/* return NULL or ptr. to byte after colon */
char *line;
{
	register char *colon;

	INDEX(line, ':', colon);
	if (colon != NULL)
		*colon++ = '\0';	/* turn colon into a NUL */
	return colon;
}

STATIC
parseflags(flags, sysp)
register char *flags;			/* flags string */
register struct system *sysp;		/* result here */
{
	sysp->sy_flags = 0;
	sysp->sy_lochops = 0;		/* default L value */
	for (; *flags != '\0'; flags++)
		switch (*flags) {
		case 'A':
			errunlock("A news format not supported", "");
			/* NOTREACHED */
		case 'B':				/* mostly harmless */
			break;
		case 'f':
			sysp->sy_flags |= FLG_SZBATCH;
			break;
		case 'F':
			sysp->sy_flags |= FLG_BATCH;
			break;
		case 'I':				/* NNTP */
			/* TODO: I sys flag: I-have, write msg-ids */
			break;
		case 'L':				/* Ln */
			sysp->sy_flags |= FLG_LOCAL;
			sysp->sy_lochops = 0;
			if (isascii(flags[1]) && isdigit(flags[1])) {
				sysp->sy_lochops *= 10;
				sysp->sy_lochops += *++flags - '0';
			}
			break;
		case 'm':
			/* TODO: m sys flag: send only moderated groups */
			break;
		case 'N':
			sysp->sy_flags |= FLG_IHAVE;
			errunlock("N flag given but I-have/send-me is not supported", "");
			/* NOTREACHED */
		case 'u':
			/* TODO: u sys flag: send only unmoderated groups */
			break;
		case 'U':			/* mostly harmless */
			/* sysp->sy_flags |= FLG_PERM; */
			break;
		case 'H':			/* bugger off */
		case 'S':			/* bugger off */
		case 'M':			/* multicast: obs., see batcher */
		case 'O':			/* multicast: obs., see batcher */
		default:
			errunlock("unknown sys flag `%c' given", *flags);
			/* NOTREACHED */
		}
}
