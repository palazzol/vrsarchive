#ifdef SCCS
static char *sccsid = "@(#)gensubs.c	1.3	1/17/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

/*
 * Generic subroutines usable on any machine.  These subroutines should
 * be replaced by assembly-level routines if possible, to make dpy run
 * as fast as possible.
 * Modified to fake the non-portable _doprnt() routine.  People who use
 * undocumented routines are no help.
*/


/*
 * Compare two strings of a given length, and return the number of leading
 * bytes which are identical, or the length if the strings are identical.
 * Nulls are not treated specially.  Examples:
 *	strdif("hi mom", "hi pop", 6) returns 3.
 *	strdif("aaaa1", "aaaa2", 2) returns 2.
*/
strdif(s1, s2, len)
	register char	*s1;		/* first string */
	register char	*s2;		/* second string */
{
	register char	*end;		/* ending character */
	char	*beg;			/* beginning character */

	beg = s1;
	end = s1 + len;
	while ((s1 < end) && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return(s1 - beg);
}



/*
 * Clear a number of bytes to spaces, from the original character location
 * up to but not including the ending location.
 */
clear(beg, end)
	register char	*beg;		/* beginning of string to clear */
	register char	*end;		/* end of string to clear */
{
	while (beg < end) *beg++ = ' ';
}

#ifdef M_XENIX
#  include <stdio.h>

_doprnt(fmt, args, fil)
char *fmt;
char **args;
FILE *fil;
{ extern FILE *_pfile;

  _pfile = fil;
  _print(fmt, &args);
}

bcopy(f, t, s)
register char *f, *t;
register unsigned s;
{ while (s--) *t++ = *f++;
}
#endif
