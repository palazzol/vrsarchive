#include <stdio.h>
#include "memcpy.h"

#define THRESHOLD 12			/* memcpy vs in-line threshold */

char *
fgets(s, lim, fp)			/* optimised version */
char *s;
int lim;
register FILE *fp;
{
	char *rets = s;			/* normal return by default */

	--lim;				/* leave room for terminating null */
	while (lim > 0) {		/* room left in s */
		int origbytesleft;
		char *nlp = NULL;	/* points at newline */

		/*
		 * Find next newline (if any) by running through the
		 * stdio buffer until it runs out or a newline is seen.
		 * This dominates the running time for long lines.
		 */
		{
			register char *bp = fp->_ptr;
			register int loops;
			/* bytes to the end of s or the stdio buffer */
			register int bytesleft = fp->_cnt;	/* to EOB */

			if (bytesleft > lim)	/* buffer longer than s */
				bytesleft = lim; /* only copy to s's end */
			origbytesleft = bytesleft;

			/*
			 * This code uses Duff's Device (tm Tom Duff)
			 * to unroll the newline recogniser:
			 * for (++bytesleft; --bytesleft > 0; )
			 *	if (*bp++ == '\n') {
			 *		nlp = bp;	# NB points after \n
			 *		break;
			 *	}
			 * Sorry the code is so ugly.
			 */
			if (bytesleft > 0) {
				/* TODO: #ifdef for non-twos-complement mchs */
				loops = (bytesleft+8-1) >> 3;	/* /8 round up */

				switch (bytesleft&(8-1)) {	/* %8 */
				case 0: do {
#define SPOTNL if (*bp++ == '\n') { nlp = bp; break; }
						SPOTNL;
					case 7:	SPOTNL;
					case 6:	SPOTNL;
					case 5:	SPOTNL;
					case 4:	SPOTNL;
					case 3:	SPOTNL;
					case 2:	SPOTNL;
					case 1:	SPOTNL;
					} while (--loops > 0);
				}
			}
		}
		/*
		 * If no newline was seen, copy remaining bytes from stdio
		 * buffer; else copy up to and including the newline.
		 * Adjust counts, then copy the bytes & adjust pointers.
		 * This dominates the running time for short lines.
		 */
		{
			register int copy;

			if (nlp == NULL)
				copy = origbytesleft;
			else
				copy = nlp - fp->_ptr;
			lim -= copy;
			fp->_cnt -= copy;
			if (copy < THRESHOLD) {
				register char *rs = s, *bp = fp->_ptr;

				for (++copy; --copy > 0; )
					*rs++ = *bp++;
				s = rs;
				fp->_ptr = bp;
			} else {
				memcpy(s, fp->_ptr, copy);
				s += copy;
				fp->_ptr += copy;
			}
		}
		/*
		 * Quit if we saw a newline or "s" is full,
		 * else refill the stdio buffer and go again.
		 */
		if (nlp != NULL || lim <= 0)
			break;
		else if (fp->_cnt <= 0) {		/* buffer empty */
			register int c = getc(fp);	/* fill buffer */

			if (c == EOF) {
				if (s == rets)		/* s is empty */
					rets = NULL;
				break;			/* EOF return */
			} else {
				if ((*s++ = c) == '\n')
					break;		/* newline return */
				--lim;
			}
		}
	}
	*s = '\0';	/* terminate s */
	return rets;
}
