memcpy(to, from, count)		/* no alignment assumptions */
register char *from, *to;
register int count;
{
	while (count-- > 0)
		*to++ = *from++;
}
