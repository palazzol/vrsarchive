/*  File   : bfill.c
    Author : Richard A. O'Keefe.
    Updated: 23 April 1984
    Defines: bfill()

    bfill(dst, len, fill) moves "len" fill characters to "dst".
    Thus to set a buffer to 80 spaces, do bfill(buff, 80, ' ').

    Note: the "b" routines are there to exploit certain VAX order codes,
    but the MOVC5 instruction will only move 65535 characters.   The asm
    code is presented for your interest and amusement.
*/

#include "strings.h"

#if	VaxAsm

void bfill(dst, len, fill)
    char *dst;
    int len;
    int fill;	/* actually char */
    {
	asm("movc5 $0,*4(ap),12(ap),8(ap),*4(ap)");
    }

#else  ~VaxAsm

void bfill(dst, len, fill)
    register char *dst;
    register int len;
    register int fill;	/* char */
    {
	while (--len >= 0) *dst++ = fill;

    }

#endif	VaxAsm

