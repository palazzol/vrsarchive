/*  File   : bmove.c
    Author : Richard A. O'Keefe.
    Updated: 23 April 1984
    Defines: bmove()

    bmove(dst, src, len) moves exactly "len" bytes from the source "src"
    to the destination "dst".  It does not check for NUL characters as
    strncpy() and strnmov() do.  Thus if your C compiler doesn't support
    structure assignment, you can simulate it with
	bmove(&to, &from, sizeof from);
    The standard 4.2bsd routine for this purpose is bcopy.  But as bcopy
    has its first two arguments the other way around you may find this a
    bit easier to get right.
    No value is returned.

    Note: the "b" routines are there to exploit certain VAX order codes,
    but the MOVC3 instruction will only move 65535 characters.   The asm
    code is presented for your interest and amusement.
*/

#include "strings.h"

#if	VaxAsm

void bmove(dst, src, len)
    char *dst, *src;
    int len;
    {
	asm("movc3 12(ap),*8(ap),*4(ap)");
    }

#else  ~VaxAsm

void bmove(dst, src, len)
    register char *dst, *src;
    register int len;
    {
	while (--len >= 0) *dst++ = *src++;
    }

#endif	VaxAsm

