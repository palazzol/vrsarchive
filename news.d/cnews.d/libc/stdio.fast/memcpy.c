/*
 * memcpy(3) stub in case none in C library
 */

memcpy(to, from, len)
register char *from, *to;
register unsigned len;
{
	/* TODO: copy a word or long at a time */
	for (++len; --len > 0; )
		*to++ = *from++;
}
