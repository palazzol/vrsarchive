#ifndef lint
static char *sccsid = "@(#)misc.c	1.1 11/18/84";
#endif

#include "externs.h"

card(array, size)		/* for beenthere, injuries */
	register char *array;
	int size;
{
	register char *end = array + size;
	register int i = 0;

	while (array < end)
		if (*array++)
			i++;
	return (i);
}

ucard(array)
	register unsigned *array;
{
	register int j = 0, n;

	for (n = 0; n < NUMOFOBJECTS; n++)
		if (testbit(array, n))
			    j++;
	return (j);
}
