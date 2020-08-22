; @(#)m16subs.s	1.4	1/28/85
; @(#)Copyright (C) 1985 by D Bell
;
;machine dependent subroutines for the National 32032 microprocessor.
;strdif - return number of bytes until two strings differ or count is reached.
;clear	- from first address up till last address, make memory spaces.

	.program

_strdif::
	movd	16(sp),r0	;byte count
	movd	8(sp),r1	;first string
	movd	12(sp),r2	;second string
	cmpsb			;compare bytes
	subd	8(sp),r1	;get length of search
	movd	r1,r0		;make return value
	rxp	0		;return


_clear::
	movd	12(sp),r0	;ending address
	movd	8(sp),r1	;beginning address
	subd	r1,r0		;compute byte count
	cmpqd	0,r0		;see if any to do
	bge	done		;nope
	addqd	-1,r0		;fix count
	addr	1(r1),r2	;destination address
	movb	32,0(r1)	;start with a blank
	movsb			;fill rest too
done:	rxp	0		;return

	.endseg
