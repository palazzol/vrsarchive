/*  File   : ffs.c
    Author : Richard A. O'Keefe.
    Updated: 20 April 1984
    Defines: ffs(), ffc()

    ffs(i) returns the index of the least significant 1 bit in i,
	   where 1 means the least significant bit and 32 means the
	   most significant bit, or returns -1 if i is 0.

    ffc(i) returns the index of the least significant 0 bit in i,
	   where 1 means the least significant bit and 32 means the
	   most significant bit, or returns -1 if i is ~0.

    These functions mimic the VAX FFS and FFC instructions, except that
    the latter return much more sensible values.  This file only exists
    to make it easier to move 4.2bsd programs to System III (which is
    rather like moving up from a Rolls Royce to a model T Ford), and so
    I haven't bother with assembly code versions.
*/

#include "strings.h"

int ffs(i)
    register int i;
    {
	register int N;

	for (N = 8*sizeof(int); --N >= 0; i >>= 1)
	    if (i&1) return 8*sizeof(int)-N;
	return -1;
    }

int ffc(i)
    register int i;
    {
	register int N;

	for (N = 8*sizeof(int); --N >= 0; i >>= 1)
	    if (!(i&1)) return 8*sizeof(int)-N;
	return -1;
    }


