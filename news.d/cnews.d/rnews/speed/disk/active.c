/*
 * active file access functions (on-disk version)
 */

#include <stdio.h>
#include "inews.h"

static FILE *fp = NULL;

long
nxtartnum(ng)
char *ng;
{
	return incartnum(ng, 1);
}

long
prevartnum(ng)
char *ng;
{
	return incartnum(ng, -1);
}

static long
incartnum(ng, inc)
char *ng;
int inc;
{
	register int nglen = strlen(ng);
	register long pos, nextart = -1;
	char line[MAXLINE];
	extern char ldzeropad[];
	long atol();

	if (artload() != ST_OKAY)
		return nextart;
#ifdef MICROLOCKING
	filelock(libfile("active"));
#endif
	(void) fseek(fp, 0L, 0);	/* start at the start */
	for (pos = ftell(fp); fgets(line, sizeof line, fp) != NULL;
	     pos = ftell(fp))
		if (strncmp(line, ng, nglen) == 0 &&
		    line[nglen] == ' ') {
			nextart = atol(&line[nglen + 1]) + inc;
			(void) fseek(fp, pos, 0);	/* back up */
			(void) fprintf(fp, "%s ", ng);
			(void) fprintf(fp, ldzeropad,
				ARTNUMWIDTH, ARTNUMWIDTH, nextart);
			(void) fflush(fp);
			break;
		}
#ifdef MICROLOCKING
	fileunlock(libfile("active"));
#endif
	return nextart;
}

int
artload()				/* reload any cached data */
{
	int status = 0;
	char *libfile();
	FILE *fopenwclex();

	if (fp == NULL)
		if ((fp = fopenwclex(libfile("active"), "r+")) == NULL)
			status |= ST_DROPPED;
	return status;
}

int
artsync()				/* sync to disk any cached data */
{
	int status = 0;

	if (fp != NULL && fclose(fp) == EOF)
		status |= ST_DROPPED;
	fp = NULL;
	return status;
}
