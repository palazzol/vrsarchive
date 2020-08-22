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
		register LUMP *fromlump = (LUMP *)from, *tolump = (LUMP *)to;
		register int inloops;
		register int loops = count >> 3;	/* /8 */

		count %= 8;		/* about to copy loops*8 bytes */
		inloops = (loops+8-1) >> 3;	/* /8 round up */
#define COPY8BYTES	*tolump++ = *fromlump++; \
			*tolump++ = *fromlump++
		if (loops > 0) {
			switch (loops&(8-1)) {	/* %8 */
			case 0:	do {
					COPY8BYTES;
				case 7:	COPY8BYTES;
				case 6:	COPY8BYTES;
				case 5:	COPY8BYTES;
				case 4:	COPY8BYTES;
				case 3:	COPY8BYTES;
				case 2:	COPY8BYTES;
				case 1:	COPY8BYTES;
				} while (--inloops > 0);
			}
			from = (char *)fromlump;
			to = (char *)tolump;
		}
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
