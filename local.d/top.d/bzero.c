/*
 *	@(#)bzero.c	1.5	3/30/85
 *	Portable version - doesn't use any undocumented features.
*/
bzero(memory, amount)
register char *memory;
register int amount;
{ while (amount-- > 0)
    *memory++ = 0;
}
