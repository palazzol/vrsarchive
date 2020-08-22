/*
 * strsav(s)
 * Save string in memory; return pointer to it.
 */

#include "adv.h"

char *
strsav(s)
	register char *s;
{
	register char *t;
	char *rv;

	t = s;
	while (*t++)
		;
	rv = t = alcnz(t - s);
	while (*t++ = *s++)
		;
	return (rv);
}

/*
 * itoa(i)
 * Convert int to ascii; return ptr to it.
 */
char *
itoa(i)
	int i;
{
	static char buf[10];

	sprintf(buf, "%d", i);
	return (buf);
}
