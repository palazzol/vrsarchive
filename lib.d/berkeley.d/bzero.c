/*  File   : bzero.c
    Author : Richard A. O'Keefe.
    Updated: 23 April 1984
    Defines: bzero()

    bzero(dst, len) moves "len" 0 bytes to "dst".
    Thus to clear a disc buffer to 0s do bzero(buffer, BUFSIZ).

    Note: the "b" routines are there to exploit certain VAX order codes,
    but the MOVC5 instruction will only move 65535 characters.   The asm
    code is presented for your interest and amusement.
*/

#include "strings.h"

#if	VaxAsm

void bzero(dst, len)
    char *dst;
    int len;
    {
	asm("movc5 $0,*4(ap),$0,8(ap),*4(ap)");
    }

#else  ~VaxAsm

void bzero(dst, len)
    register char *dst;
    register int len;
    {
	while (--len >= 0) *dst++ = 0;
    }

#endif	VaxAsm

