/*
 *	%W% %G%
 *	Copy memory to memory.
*/

void
bcopy(from, to, size)
register char *from, *to;
register size;
{ if (from > to)
    while (size-- > 0)
     *to++ = *from++;
  else {
    from += size;
    to += size;
    while (size-- > 0)
     *--to = *--from;
  }
}
