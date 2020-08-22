/*
 * inews sys file reading functions (in-memory version)
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "news.h"
#include "system.h"

static FILE *fp = NULL;		/* descriptor for libfile("sys") */
static char filerelname[] = "sys";
static char defaultcmd[] = "exit 1";	/* reminder to bozo admins */
static struct system *firstsys = NULL;	/* cache */
static struct system *currsys = NULL;	/* current system */

struct system *
oursys()			/* return our sys entry */
{
	register struct system *sys;
	static struct system fakesys;
	char *hostname();
	struct system *nextsys();

	rewsys();
	while ((sys = nextsys()) != NULL && !STREQ(sys->sy_name, hostname()))
		;
	if (sys == NULL) {		/* no entry; cook one up */
		fakesys.sy_name = hostname();
		fakesys.sy_ngs = "all";
		fakesys.sy_flags = 0;
		fakesys.sy_lochops = 0;
		fakesys.sy_cmd = defaultcmd;
		fakesys.sy_next = NULL;
		sys = &fakesys;
	}
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
	char *libfile();
	FILE *fopenwclex();

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

static
readsys()
{
	char sysline[MAXLINE];

	rewind(fp);
	/* TODO: write & use cfgets to read continued lines (need bigger MAXLINE) */
	while (fgets(sysline, sizeof sysline, fp) != NULL)
		if (sysline[0] != '#' && sysline[0] != '\n') {	/* not a comment */
			register struct system *sysp;
			char *flagstring;
			char *malloc();

			sysp = (struct system *)malloc(sizeof *sysp);
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
			sysp->sy_flags = flgtobits(flagstring);
			sysp->sy_lochops = 0;		/* Ln value someday */
			free(flagstring);		/* malloced by parse */
			sysp->sy_next = NULL;

			/* fill in any defaults */
			if (sysp->sy_cmd[0] == '\0') {
				free(sysp->sy_cmd);	/* malloced by parse */
				sysp->sy_cmd = defaultcmd;	/* NB not malloced */
			}

			/* stash *sysp away on the tail of the current list */
			if (firstsys == NULL)
				firstsys = sysp;		/* 1st system */
			else
				currsys->sy_next = sysp;	/* tack on tail */
			currsys = sysp;
		}
	(void) fclose(fp);		/* file no longer needed */
	fp = NULL;			/* mark file closed */
	rewsys();
}

parse(into)
register char **into;
{
	char *strsave(), *parsecolon();

	curr = next;
	if (curr == NULL)
		*into = strsave("");
	else {
		next = parsecolon(curr);
		*into = strsave(curr);
	}
	if (*into == NULL)
		errunlock("out of memory for sys strings", "");
}

char *
parsecolon(line)		/* return NULL or ptr. to byte after colon */
char *line;
{
	register char *colon;

	INDEX(line, ':', colon);
	if (colon != NULL)
		*colon++ = '\0';	/* turn colon into a NUL */
	return colon;
}

static int
flgtobits(s)
register char *s;
{
	register int bits = 0;

	for (; *s != '\0'; s++)
		switch (*s) {
		case 'A':
			errunlock("A news format not supported", "");
			/* NOTREACHED */
		case 'B':				/* mostly harmless */
			break;
		case 'F':
			bits |= FLG_BATCH;
			break;
		case 'L':
			bits |= FLG_LOCAL;
			/* TODO: parse Ln, but maybe not here */
			break;
		case 'N':
			bits |= FLG_IHAVE;
			errunlock("N flag given but I-have/send-me is not supported", "");
			/* NOTREACHED */
		case 'U':
			bits |= FLG_PERM;
			break;
		default:
			errunlock("unknown sys flag `%s' given", s);
			/* NOTREACHED */
		}
	return bits;
}
