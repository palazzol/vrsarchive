# @(#)vaxsubs.s	1.5	1/17/85
# @(#)Copyright (C) 1985 by D Bell


	.text
	.globl	_strdif
	.globl	_clear

_strdif:
	.word	0x0
	cmpc3	12(ap),*4(ap),*8(ap)	#compare the strings
	subl3	4(ap),r1,r0		#return difference
	ret

_clear:
	.word	0x0
	subl3	4(ap),8(ap),r0		#number of bytes to fill
	movc5	$0,0,$32,r0,*4(ap)	#fill the bytes
	ret
