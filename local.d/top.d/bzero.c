/*
 *	@(#)bzero.c	1.7	10/5/86
 *	Portable version - doesn't use any undocumented features.
*/
bzero(memory, amount)
register char *memory;
register int amount;
{ while (amount-- > 0)
    *memory++ = 0;
}
