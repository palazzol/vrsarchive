/*
 * memset - set bytes
 *
 * CHARBITS should be defined only if the compiler lacks "unsigned char".
 * It should be a mask, e.g. 0377 for an 8-bit machine.
 */

#ifndef CHARBITS
#	define	UNSCHAR(c)	((unsigned char)(c))
#else
#	define	UNSCHAR(c)	((c)&CHARBITS)
#endif

VOIDSTAR
memset(s, ucharfill, size)
CONST VOIDSTAR s;
register int ucharfill;
SIZET size;
{
	register CONST char *scan;
	register SIZET n;
	register int uc;

	scan = s;
	uc = UNSCHAR(ucharfill);
	for (n = size; n > 0; n--)
		*scan++ = uc;

	return(s);
}
