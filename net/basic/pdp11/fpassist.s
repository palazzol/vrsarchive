/       (c) P. (Rabbit) Cockcroft 1982

.globl  _wait, cerror

wait = 7.

_wait:
	mov     r5,-(sp)
	mov     sp,r5
	sys     wait
	bec     1f
	jmp     cerror
1:	
	tst	4(r5)
	beq	1f
	mov	r1,*4(5)
1:
	mov     (sp)+,r5
	rts     pc

/   getch() is used all over the place to get the next character on the line.
/   It uses 'point' ( _point ) as the pointer to the next character.
/   It skips over all leading spaces.
/       It was put into machine code for speed since it does not have to
/   call csv and cret ( the C subroutine call and return routines ).
/   this saves a lot of time. It can also be written more efficiently
/   in machine code.
/

.text
.globl  _point , _getch

_getch:
	mov     _point,r1
1:      cmpb    $40,(r1)+       / ignore spaces
	beq     1b
	mov     r1,_point
	clr     r0
	bisb    -(r1),r0
	rts     pc

/   check() is used by many routines that want to know if there is any
/   garbage characters after its arguments. e.g. in 'goto' there
/   should be nothing after the line number. It gives a SYNTAX
/   error if the next character is not a terminator.
/   check() was also taken out of C for speed reasons.

.globl  _point  , _check   , _elsecount  , _error

ELSE=   0351

_check:
	mov     _point,r0
1:      cmpb    $40,(r0)+
	beq     1b
	movb    -(r0),r1
	beq     1f
	cmpb    r1,$':
	beq     1f
	cmpb    r1,$ELSE
	bne     2f
	tstb    _elsecount
	beq     2f
1:      mov     r0,_point
	rts     pc
2:      mov     $1,-(sp)        / syntax error
	jsr     pc,_error

/   startfp() this is called in main to intialise the floating point
/   hardware if it is used. it is only called once to set up fpfunc()
/   this routine does nothing in non-floating point hardware machines.
/

 .globl _startfp , _fpfunc

ldfps = 0170100 ^ tst

_startfp:
	mov     $fpcrash,_fpfunc
	ldfps   $1200
	rts     pc
.bss
_fpfunc: .=.+2
.text

fpcrash:
	mov     $34.,-(sp)
	jsr     pc,_error       / no return

/   cmp() is used to compare two numbers , it uses 'vartype' to decide
/   which kind of variable to test.
/   The result is -1,0 or 1 , depending on the result of the comparison
/

.globl  _cmp   , _vartype

_cmp:
	tstb    _vartype
	beq     6f
	cmp     *2(sp),*4(sp)
1:
	blt     4f
	bgt     3f
5:      clr     r0
	rts     pc
3:      mov     $1,r0
	rts     pc
4:      mov     $-1,r0
	rts     pc
				/ floating point comparisons
6:      movf    *4(sp),fr0
	cmpf    *2(sp),fr0
	cfcc
	br      1b


/   routine to multiply two numbers together. returns zero on overflow
/   used in dimensio() only.

.globl  _dimmul

_dimmul:
	mov     2(sp),r1
	mul     4(sp),r1
	bcc     1f
	clr     r1
1:      mov     r1,r0
	rts     pc

.globl  _mbin

/   jump table for the maths functions
/   straight from the eval() routine in bas3.c

.data
_mbin:  0
	0
	fandor
	andor
	comop
	comop
	fads
	ads
	fmdm
	mdm
	fex
	ex
.text

/   locations from the jump table
/   integer exponentiation , convert to reals then call the floating
/   point convertion routines.
/
.globl  _exp , _log

exp:    movf    fr0,-(sp)
	jsr     pc,_exp
	tstf    (sp)+
	rts     pc

log:    movf    fr0,-(sp)
	jsr     pc,_log
	tstf    (sp)+
	rts     pc


ex:     movif   *2(sp),fr0
	movif   *4(sp),fr1
	movf    fr1,*4(sp)
	clrb    _vartype
	br      1f
fex:
	movf    *2(sp),fr0
1:
	tstf    fr0
	cfcc
	beq     1f
	bmi     2f
	jsr     pc,log              / call log
	mulf    *4(sp),fr0
1:
	jsr     pc,exp              / exponentiate
	bes     1f
	movf    fr0,*4(sp)
	rts     pc
1:      mov     $40.,-(sp)          / overflow in ^
	jsr     pc,_error
2:      mov     $41.,-(sp)          / negative value to ^
	jsr     pc,_error

fmdm:
	movf    *2(sp),fr0
	cmp     $52,6(sp)       / times
	bne     1f
	mulf    *4(sp),fr0
	movf    fr0,*4(sp)
	rts     pc
1:
	movf    *4(sp),fr2
	cfcc
	beq     zerodiv
	divf    fr2,fr0
	cmp     $'/,6(sp)       / div
	beq     1f
	modf    $040200,fr0     / mod
	mulf    fr2,fr0
1:
	movf    fr0,*4(sp)
	rts     pc


mdm:    cmp     $52,6(sp)       / integer multiply
	bne     1f
	mov     *2(sp),r0
	mul     *4(sp),r0
	bcs     over            / overflow
	br      2f
1:      mov     *2(sp),r1       / divide or mod
	sxt     r0
	div     *4(sp),r0
	bvs     1f
	cmp     $57,6(sp)       / div
	bne     2f              / no , must be mod.
	tst     r1
	bne     3f
	mov     r0,r1
2:      mov     r1,*4(sp)
	rts     pc
1:
zerodiv:
	mov     $25.,-(sp)      / zero divisor error
	jsr     pc,_error
				/ code to do integer divisions.. etc.
3:      movif   *2(sp),fr0
	movif   *4(sp),fr1
	divf    fr1,fr0
	movf    fr0,*4(sp)
	clrb    _vartype
	rts     pc

fads:                           / floating add and subtract
	movf    *2(sp),fr0
	cmp     $53,6(sp)
	bne     1f

	addf    *4(sp),fr0
	movf    fr0,*4(sp)
	rts     pc
1:
	subf    *4(sp),fr0
	movf    fr0,*4(sp)
	rts     pc


ads:    mov     *2(sp),r1
	cmp     $53,6(sp)           / add or subtract
	bne     1f
	add     *4(sp),r1           / add
	br      2f
1:      sub     *4(sp),r1           / subtract
2:      bvs     over1               / branch on overflow
	mov     r1,*4(sp)
	rts     pc

over1:  tst     *2(sp)              / move value to 'overfl'
	sxt     r0
over:   mov     r0,_overfl
	mov     r1,_overfl+2
	jmp     _over               /  return via call to 'over'

/   comparison operators ( float and integer )
/   cmp() expects to have only two parameters . So save return address
/   and so simulate environment.

comop:  mov     (sp)+,comsav        / save return address
	jsr     pc,_cmp             / call comparison routine
	mov     r0,-(sp)
	mov     6(sp),-(sp)         / call routine to convert
	jsr     pc,_compare         / this result into logical result
	tst     (sp)+
	mov     comsav,(sp)         / restore return address
	rts     pc                  / return
.bss
comsav: .=.+2
.text

/   floating logical operators
/   convert floating types into integers. If the value is non zero
/   then value has a true (-1) value.
/

fandor:
	mov     *2(sp),r0
	beq     2f
	mov     $-1,r0
2:      mov     *4(sp),r1
	beq     2f
	mov     $-1,r1
2:      movb    $1,_vartype
	br      2f

/   integer logical operators
/   does a bitwise operaotion on the two numbers ( in r0 , r1 ).
/

andor:
	mov     *2(sp),r0
	mov     *4(sp),r1
2:      cmpb    $356,6(sp)
	bne     2f
	com     r1
	bic     r1,r0
	br      1f
2:      cmp     $357,6(sp)
	bne     2f
	bis     r1,r0
	br      1f
2:      xor     r1,r0
1:      mov     r0,*4(sp)
	rts     pc

/   This routine converts a floationg point number into an integers
/   if the result would overflow then return non zero.
/

.globl  _conv

_conv:
	movf    *2(sp),fr0
	movfi   fr0,r0
	cfcc
	bcs     1f
	mov     r0,*2(sp)
	clr     r0
	rts     pc
1:
	mov     $1,r0
	rts     pc


/   add two numbers used in the 'next' routine
/   depends on the type of the number. calls error on overflow.
/

.globl  _foreadd
_foreadd:
	add     2(sp),*4(sp)
	bvs     1f
	rts     pc
1:      mov     $35.,-(sp)      / integer overflow
	jsr     pc,_error

/   convert a long in 'overfl' to a real. uses the floating point
/   routines. returns via these routines.

.globl  _over
_over:
	setl
	movif   _overfl,fr0
	clrb    _vartype
	movf    fr0,*4(sp)
	seti
	rts     pc
/
/   put a value into a variable , does the convertions from integer
/   to real and back as needed.
/

.globl  _putin
_putin: cmpb    4(sp),_vartype
	beq     1f
	tstb    4(sp)
	beq     2f
	movf    _res,fr0
	movfi   fr0,r0
	cfcc
	bes     3f
	mov     r0,*2(sp)
	rts     pc
3:
	mov     $35.,-(sp)
	jsr     pc,*$_error     / no return
2:
	movif   _res,fr0
	movf    fr0,*2(sp)
	rts     pc
1:
	tstb    4(sp)
	bne     1f
	movf    _res,fr0
	movf    fr0,*2(sp)
	rts     pc
1:
	mov     _res,*2(sp)
	rts     pc

/   high speed move of variables
/   can't use floating point moves because of '-0'.

.globl  _movein
_movein: mov     2(sp),r0
	mov     4(sp),r1
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	rts     pc

/   puts the value from a variable into 'res'. It might be thought
/   that 'movein' could be used but it can't for the reason given in
/   the report.
/

.globl  _getv
_getv:  mov     2(sp),r0
	mov     $_res,r1
	mov     (r0)+,(r1)+
	tstb    _vartype
	bne     1f
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
1:      rts     pc

/   move the value in res onto the maths 'stack'. A simple floating
/   move cannot be used due to the possibility of "minus zero" or
/   -32768 being in 'res'. This could check 'vartype' but for speed just
/   does the move.

.globl  _push
_push:  mov     2(sp),r1
	mov     $_res,r0
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	rts     pc

/   negate a number , checks for overflow and for type of number.
/

.globl  _negate
_negate:
	tstb    _vartype
	beq     1f
	neg     _res
	bvs     2f                      / negating -32768
	rts     pc
1:      tst     _res                    / stop -0
	beq     1f
	add     $100000,_res
1:
	rts     pc
2:
	mov     $044000,_res            / 32768 in floating form
	clr     _res+2
	clr     _res+4
	clr     _res+6
	clrb    _vartype
	rts     pc

/ unary negation

.globl  _notit

_notit: tstb    _vartype
	beq     1f
	com     _res
	rts     pc
1:      movb    $1,_vartype
	tst     _res
	bne     1f
	com     _res
	rts     pc
1:      clr     _res
	rts     pc

/ routine to dynamically check the stack

.globl  _checksp

_checksp:
	cmp     sp,$160000+1024.
	blos    1f
	rts     pc
1:      mov     $44.,(sp)       / expression too complex
	jsr     pc,_error       / no return
