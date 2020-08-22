/*
 * inews sys file reading functions (on-disk version)
 */

#include <stdio.h>
#include "inews.h"
#include "system.h"

static FILE *fp = NULL;		/* descriptor for libfile("sys") */

struct system *
oursys()			/* return our sys entry */
{
	register struct system *sys;
	static struct system fakesys;
	char *rindex();
	char *hostname();
	struct system *nextsys();

	rewsys();
	while ((sys = nextsys()) != NULL &&
	    strcmp(sys->sy_name, hostname()) != 0)
		;
	if (sys == NULL) {		/* no entry; cook one up */
		fakesys.sy_name = hostname();
		fakesys.sy_ngs = "all";
		fakesys.sy_flags = "";
		fakesys.sy_cmd = "";
		sys = &fakesys;
	}
	return sys;
}

/*
 * Returned pointer points at a static struct whose members
 * point at static storage.
 */
struct system *
nextsys()			/* return next sys entry */
{
	register char *s;
	register char **sp;
	static char sysent[MAXLINE];
	static struct system sys;	/* pointers into sysent */
	char *strcpy(), *strcat(), *rindex();
	char *hostname(), *libfile();
	FILE *fopenwclex();

	if (fp == NULL)
		if ((fp = fopenwclex(libfile("sys"), "r")) == NULL)
			return NULL;
	while ((s = fgets(sysent, sizeof sysent, fp)) != NULL)
		if (s[0] != '#')		/* not a comment */
			break;
	if (s == NULL)
		return NULL;

	s = rindex(sysent, '\n');
	if (s != NULL)
		*s = '\0';		/* trim newline */
	sp = &sys.sy_name;		/* point at string pointers */
	*sp++ = sysent;
	for (s = sysent; *s != '\0'; s++)
		if (*s == ':') {
			*s = '\0';	/* turn colons into NULs */
			if (sp <= &sys.sy_cmd)
				*sp++ = s + 1;	/* point at next field */
		}
	for (; sp <= &sys.sy_cmd; sp++)
		*sp = "";		/* fill remaining fields */
	return &sys;
}

rewsys()
{
	if (fp != NULL)
		(void) fseek(fp, 0L, 0);
}
