#define LUMP long

memcpy(to, from, count)	/* assumes unaligned LUMPs can be copied */
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
		register int loops = count >> 3;	/* /8 */
		register LUMP *fromlump = (LUMP *)from, *tolump = (LUMP *)to;

		count %= 8;		/* about to copy loops*8 bytes */
		while (loops-- > 0) {
			*tolump++ = *fromlump++;
			*tolump++ = *fromlump++;
		}
		from = (char *)fromlump;
		to = (char *)tolump;
	}
	/*
	 * This code uses Duff's Device (tm Tom Duff)
	 * to unroll the copying loop the last count%8 times:
	 * while (count-- > 0)
	 *	*to++ = *from++;
	 * Sorry the code is so ugly.
	 */
	if (count > 0) {
		switch (count&(8-1)) {	/* %8 */
		case 0:
#define COPYBYTE *to++ = *from++
				COPYBYTE;
			case 7:	COPYBYTE;
			case 6:	COPYBYTE;
			case 5:	COPYBYTE;
			case 4:	COPYBYTE;
			case 3:	COPYBYTE;
			case 2:	COPYBYTE;
			case 1:	COPYBYTE;
		}
	}
}
