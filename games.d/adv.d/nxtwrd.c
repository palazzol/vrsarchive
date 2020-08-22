/*
 * Break off the next word from the string *s; leave *s pointing
 * to the 'separator' character that ended the scan.  Return a
 * pointer to a strsav() of the word.
 */

#include "adv.h"
#include <ctype.h>

#define	notsep(c)	(c && c != ' ' && c != ',' && c != ':')

char *nxtwrd(sp, ml)
	register char **sp;
	register int ml;	/* max length */
{
	register char *bp = buf2, *s = *sp;
	register int n = 0;

	skpwht(s);		/* skip whitespace */
	while (notsep(*s)) {
		*bp++ = islower (*s) ? toupper(*s) : *s;
		s++;
		if (++n >= ml)
			break;
	}
	if (notsep(*s))		/* if *s is not a separator */
		while (notsep(*s))/* then search forward for one */
			s++;
	*bp = 0;
	*sp = s;
	return (strsav(buf2));
}
