memcpy(to, from, count)		/* no alignment assumptions */
register char *from, *to;
register int count;
{
	/*
	 * This code uses Duff's Device (tm Tom Duff)
	 * to unroll the copying loop:
	 * while (count-- > 0)
	 *	*to++ = *from++;
	 * Sorry the code is so ugly.
	 */
	if (count > 0) {
		register int loops = (count+8-1) >> 3;	/* /8 round up */

		switch (count&(8-1)) {	/* %8 */
		case 0: do {
#define COPYBYTE *to++ = *from++
				COPYBYTE;
			case 7:	COPYBYTE;
			case 6:	COPYBYTE;
			case 5:	COPYBYTE;
			case 4:	COPYBYTE;
			case 3:	COPYBYTE;
			case 2:	COPYBYTE;
			case 1:	COPYBYTE;
			} while (--loops > 0);
		}
	}
}
