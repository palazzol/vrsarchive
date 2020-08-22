/*
 * fgetmfs - read an arbitrarily long, possibly continued line;
 * return a pointer to it, in malloced memory.
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <fgetmfs.h>

/* One could make these arguments, with defaults. */
#define INITLN 90		/* initial allocation per line */
#define GROWLN 200		/* additional allocation size */

/* imports from libc */
char *malloc(), *realloc(), *rindex();

char *
fgetmfs(fp, cont)		/* fget malloced, flagged string */
FILE *fp;
int cont;			/* honour \ continuations? */
{
	register unsigned sz;
	register char *line;
	register char *segment;
	register char *morep;
	register int another = 0;

	/* allocate room for an initial segment of a line */
	sz = INITLN;
	line = malloc(sz);
	if (line == NULL)
		return NULL;		/* no memory, can't go on */
	segment = line;
	morep = line + sz - 2;
	do {
		register char *nlp;

		/* read the first segment of a line */
		*morep = '\0';			/* mark end of segment */
		if (fgets(segment, (int)sz-(segment-line), fp) == NULL) {
			free(line);		/* EOF: give up */
			return NULL;
		}
		while (*morep != '\0' && *morep != '\n') {	/* line didn't fit */
			/* extend the allocation */
			sz += GROWLN;
			line = realloc(line, sz);
			if (line == NULL)
				return NULL;	/* no memory, can't go on */
			segment = line + sz - GROWLN - 1;
			morep = line + sz - 2;

			/* read the next segment */
			*morep = '\0';
			if (fgets(segment, GROWLN+1, fp) == NULL) {
				free(line);	/* EOF: give up */
				return NULL;
			}
		}

		/* got a whole line: is it to be continued? */
		if (cont && (nlp = rindex(line, '\n')) != NULL &&
		    nlp > line && *--nlp == '\\') {
			*nlp = '\0';		/* delete "\\\n" */
			segment = nlp;
			another = 1;		/* read next line */
		    	if (cont == CONT_NOSPC) {
				register int c;

				/* discard leading whitespace */
				while ((c = getc(fp)) != EOF && c != '\n' &&
				   isascii(c) && isspace(c))
					;
				if (c != EOF)
					(void) ungetc(c, fp);
		    	}
		} else
			another = 0;
	} while (another);
#ifdef pdp11
	line = realloc(line, strlen(line)+1);	/* save space */
#endif
	return line;
}
