/*
 * active file access functions (in-memory version)
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "active.h"
#include "news.h"
#include "newspaths.h"

#define MAXNGS 2000

static FILE *fp = NULL;
static char filerelname[] = "active";
static char *active = NULL;	/* points at entire active file */
static int actsize;		/* bytes in active: type int determined by fread */
static unsigned artlines;	/* lines in artlnps actually used */
/* TODO: make this a linked list or realloc to avoid the MAXNGS limit */
static char *artlnps[MAXNGS];	/* point at lines in active file */

long
incartnum(ng, inc)
char *ng;
int inc;
{
	register int nglen = strlen(ng);
	register char *pos;
	register unsigned line = 0;
	register long nextart = -1;
	char artnumstr[ARTNUMWIDTH + 1];
	extern char ldzeropad[];

	if (artload() != ST_OKAY)
		return nextart;
	while (pos = artlnps[line], line++ < artlines && pos[0] != '\0')
		if (STREQN(pos, ng, nglen) && pos[nglen] == ' ') {
			nextart = atol(&pos[nglen + 1]) + inc;
			(void) sprintf(artnumstr, ldzeropad,
				ARTNUMWIDTH, ARTNUMWIDTH, nextart);
			(void) strncpy(&pos[nglen + 1], artnumstr, ARTNUMWIDTH);
			break;
		}
	return nextart;
}

int
artload()				/* reload any cached data */
{
	int status = 0;

	if (fp == NULL)
		if ((fp = fopenwclex(libfile(filerelname), "r+")) == NULL)
			status |= ST_DROPPED;
	if (fp != NULL && active == NULL) {	/* file open, no cache */
		struct stat sb;

		if (fstat(fileno(fp), &sb) < 0)
			warning("can't fstat %s", libfile(filerelname));
		else if (actsize = sb.st_size, /* squeeze into an int */
		    (unsigned)actsize != sb.st_size)
			warning("%s won't fit into memory", libfile(filerelname));
		else if ((active = malloc((unsigned)actsize+1)) == NULL)
			warning("can't allocate memory for %s", libfile(filerelname));
		else {
			rewind(fp);
			/* TODO: read with fgets to avoid linescan() */
			if (fread(active, 1, actsize, fp) != actsize) {
				warning("error reading %s", libfile(filerelname));
				free(active);
				active = NULL;
			} else {
				active[actsize] = '\0';	/* make a proper string */
				if ((artlines = linescan(active, artlnps,
				    MAXNGS)) >= MAXNGS) {
				    	extern char *progname;

					(void) fprintf(stderr,
						"%s: too many newsgroups in %s\n",
						progname, libfile(filerelname));
					free(active);
					active = NULL;
				}
			}
		}
		if (active == NULL)
			status |= ST_DROPPED;	/* give up! */
	}
	return status;
}

int
artsync()				/* sync to disk any cached data */
{
	int status = 0;

	if (fp != NULL) {
		rewind(fp);
		if (active != NULL && fwrite(active, actsize, 1, fp) != 1 ||
		    fclose(fp) == EOF) {
			warning("error writing %s", libfile(filerelname));
			status |= ST_DROPPED;
		}
	}
	fp = NULL;
	if (active != NULL)
		free(active);		/* give back memory active used */
	active = NULL;
	return status;
}

/*
 * Store in lnarray the addresses of the starts of lines in s.
 * Return the number of lines found; if greater than nent,
 * store only nent and return nent.
 */
int
linescan(s, lnarray, nent)
register char *s;
register char **lnarray;
register int nent;
{
	register int i = 0;
	register char *nlp = s;

	if (i < nent)
		lnarray[i++] = s;
	while (i < nent && (nlp = index(nlp, '\n')) != NULL)
		lnarray[i++] = ++nlp;
	return i;
}

/* ARGSUSED hdrs */
moderated(hdrs)
struct headers *hdrs;
{
	/* TODO: look at 4th field of active file; needs new active.c hook */
	return NO;			/* stub */
}
