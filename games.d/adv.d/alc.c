/*
 * Return pointer to len bytes (if zero, set to 0).
 */

#include "adv.h"

extern char *calloc();
extern char *malloc();

char *
alc(len, zero)
	int len, zero;
{
	register char *s;

	if (zero)
		s = calloc(1, len);
	else
		s = malloc(len);
	if (s == NULL) {
		perror("adv: out of memory");
		if (TopWin != NULL)
			Wexit(100);
		exit(100);
	}
#ifdef DEBUG
	printf ("Allocated %d bytes at %X (%szeroed)\n", len, (long)s,
	    zero ? "" : "not ");
#endif
	return (s);
}
