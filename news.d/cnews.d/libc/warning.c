/*
 * warning - print best error message possible and clear errno
 */

#include <stdio.h>

warning(s1, s2)
char *s1;
char *s2;
{
	char *cmdname;
	extern int errno, sys_nerr;
	extern char *sys_errlist[];
	extern char *progname;
	extern char *getenv();

	cmdname = getenv("CMDNAME");
	if (cmdname != NULL && *cmdname != '\0')
		fprintf(stderr, "%s:", cmdname);	/* No space after :. */
	if (progname != NULL)
		fprintf(stderr, "%s: ", progname);
	fprintf(stderr, s1, s2);
	if (errno > 0 && errno < sys_nerr)
		fprintf(stderr, " (%s)", sys_errlist[errno]);
	fprintf(stderr, "\n");
	errno = 0;
}
