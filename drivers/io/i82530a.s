|
|	%Z% %M% %I% %D% %Q%
|
|	This Module contains Proprietary Information of Microsoft
|	Corporation and AT&T, and should be treated as Confidential.
|
|
| Subroutines to read and write to 82530 registers
|
| Written by Linda Kopecky	Jan 9, 1984
|
| w530(port, reg, value)	-- write a value to reg on port
| r530(port, reg) --	   read a value from reg on port
|
.text
.globl	_w530,_r530
port=4
reg=6
value=8
_w530:
	cli			| disable interrupts
	mov	bx,sp		| index to parameters passed
	mov	dx,#port(bx)	| dx <-- port
	mov	ax,#reg(bx)	| ax <-- register

	cmp	ax,*0		| if reg 0
	beq	w530out		| jump
	out			| output register to port
	push	ax		| waste time between out and out
	pop	ax
	|push	ax		| waste time between out and out (vrs)
	|pop	ax
	|push	ax		| waste time between out and out (vrs)
	|pop	ax
w530out:	mov	ax,#value(bx)	| ax <-- value to output
	out			| output byte value to port
	sti
	reti

_r530:
	cli
	mov	bx,sp		| index to parameters passed
	mov	dx,#port(bx)	| dx <-- port
	mov	ax,#reg(bx)	| ax <-- register

	cmp	ax,*0		| if reg 0
	beq	r530in		| jump
	out			| output register to port
	push	ax		| waste time between out and in
	pop	ax
	|push	ax		| waste time between out and in (vrs)
	|pop	ax
	|push	ax		| waste time between out and in (vrs)
	|pop	ax
r530in:	in			| input value from port
	subb	ah,ah		| clear upper byte
	sti
	reti
