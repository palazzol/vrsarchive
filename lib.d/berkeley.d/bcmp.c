/*  File   : bcmp.c
    Author : Richard A. O'Keefe.
    Updated: 23 April 1984
    Defines: bcmp()

    bcmp(s1, s2, len) returns 0 if the "len" bytes starting at "s1" are
    identical to the "len" bytes starting at "s2", non-zero if they are
    different.   The 4.2bsd manual page doesn't say what non-zero value
    is returned, though the BUGS note says that it takes its parameters
    backwards from strcmp.  This suggests that it is something like
	for (; --len >= 0; s1++, s2++)
	    if (*s1 != *s2) return *s2-*s1;
	return 0;
    There, I've told you how to do it.  As the manual page doesn't come
    out and *say* that this is the result, I tried to figure out what a
    useful result might be.   (I'd forgotten than strncmp stops when it
    hits a NUL, which the above does not do.)  What I came up with was:
    the result is the number of bytes in the differing tails.  That is,
    after you've skipped the equal parts, how many characters are left?
    To put it another way, N-bcmp(s1,s2,N) is the number of equal bytes
    (the size of the common prefix).  After deciding on this definition
    I discovered that the CMPC3 instruction does exactly what I wanted.
    The code assumes that N is non-negative.

    Note: the "b" routines are there to exploit certain VAX order codes,
    but the CMPC3 instruction will only test 65535 characters.   The asm
    code is presented for your interest and amusement.
*/

#include "strings.h"

#if	VaxAsm

int bcmp(s1, s2, len)
    char *s1, *s2;
    int len;
    {
	asm("cmpc3 12(ap),*4(ap),*8(ap)");
    }

#else  ~VaxAsm

int bcmp(s1, s2, len)
    register char *s1, *s2;
    register int len;
    {
	while (--len >= 0 && *s1++ == *s2++) ;
	return len+1;
    }

#endif	VaxAsm

