bzero(memory, amount)
register char *memory;
register int amount;
{ while (amount-- > 0)
    *memory++ = 0;
}
