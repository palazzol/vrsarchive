/*
 * time utilities
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"

/*
 * Write a timestamp of the form "Jun 12 12:34:56" on fp.
 * N.B.: no trailing newline is written.
 */
timestamp(fp, timep, ctmp)
FILE *fp;
time_t *timep;	/* if non-null, return time() here for later use */
char **ctmp;	/* if non-null, return ctime(&now) here for later use (needed?) */
{
	char *prtime;
	time_t now;
	char *ctime();
	time_t time();

	now = time(&now);
	if (timep != NULL)
		*timep = now;
	prtime = ctime(&now);
	if (ctmp != NULL)
		*ctmp = prtime;
	/* .15 excludes yyyy\n\0; + 4 omits day-of-week */
	(void) fprintf(fp, "%.15s", prtime + 4);
}
