/       (c) P. (Rabbit) Cockcroft 1982
/   This file contains machine code routines that either can't
/   be implemented or are very slow in C.
/

/   When the 'shell' command was first added it was noticed that
/   it would bus-error about five times ( an old form of memory
/   allocation was being used at the time ) before it started to
/   do the wait. The reason for this is rather awful. In the call
/   it uses _nargs to find how many arguments it has got. This is
/   a routine that will not work in split i and d space, since it tries
/   to access the text segment.
/       The routine was thus taken from the C library and has been changed
/   to need no parameters. It just returns -1 on error or the waited for's
/   process id.
/
/ pid == -1 if error

.globl  _wait, cerror

wait = 7.

_wait:
	mov     r5,-(sp)
	mov     sp,r5
	sys     wait
	bec     1f
	jmp     cerror
1:
	tst     4(r5)
	beq     1f
	mov     r1,*4(r5)
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
/   this routine does nothing in non-floating point hardware machines
/
.globl  _startfp  ,  _fpfunc

_startfp:
	clr     _fpfunc
	rts     pc

.bss
_fpfunc: .=.+2
.text

/   getop() will convert a number into in ascii form to a binary number
/   it returns non-zero if the number is ok, with the number in
/   the union 'res'. It uses the floating point routines (nfp.s) and
/   some of its storage locations ( areg ) to do the hard work.
/   If the number will fit into an integer, then the value returned is an
/   integer, with 'vartype' set accordingly. This convertion to integers
/   is only operative if the convertion needed is an easy one.
/   Zero is always returned as an integer.
/       This routine was written in assembler since it is impossible
/   to write in C.

.globl  _getop
_getop:
	jsr     r5,csv
	mov     $areg,r0
	clr     (r0)+
	clr     (r0)+
	clr     (r0)+
	clr     (r0)+
	clr     aexp
	clr     dpoint
	clr     dflag
	mov     $1,asign
	clrb    _vartype
	clr     count           / number of actual digits
1:      movb    *_point,r4
	inc     _point
	cmp     r4,$'.
	bne     4f
	tst     dflag           / decimal point
	bne     out1            / already had one so get out of loop
	inc     dflag           / set the decimal point flag.
	br      1b
4:
	cmp     r4,$'0
	blt     out1
	cmp     r4,$'9
	bgt     out1
	inc     count               / we have a digit
	bit     $!07,areg           / enough space for another digit
	bne     2f                  / no
	sub     $'0,r4              / multiply number by ten
	mov     r4,r2               / and add the new digit.
	jsr     pc,tenmul
	tst     dflag               / if we have not had a decimal point
	beq     1b                  / don't decrement the significance
	dec     dpoint              / counter.
	br      1b
2:                                  / get here if all digits are filled
	tst     dflag               / if decimal point , forget it
	bne     1b
	inc     dpoint              / increment the significance counter
	br      1b                  / get some more.
out1:
	tst     count               / check to see that we have had a digit
	bne     9f                  / yes then continue.
	jmp     bad                 / no goto bad.
9:      cmp     r4,$'e              / do we have an exponent.
	bne     out2                / no.
	clr     count               / count number of exponent digits
	clr     r3                  / clear exponent value
	clr     r2                  / clear exponent sign
	movb    *_point,r4
	inc     _point
	cmp     r4,$'-              / exponents sign
	bne     1f
	inc     r2                  / set the flag
	br      2f
1:      cmp     r4,$'+
	bne     3f
2:      movb    *_point,r4
	inc     _point
3:
	cmp     r4,$'0              / get the exponent digits
	blt     1f
	cmp     r4,$'9
	bgt     1f
	inc     count               / we have a digit.
	sub     $'0,r4
	cmp     r3,$1000.           / if the digit would make the exponent
	blt     7f                  / greater than ten thousand
3:                                  / for get the following digits
	movb    *_point,r4          / ( we are heading for an overflow )
	inc     _point
	cmp     r4,$'0
	blt     1f
	cmp     r4,$'9
	ble     3b
	br      1f
7:
	mul     $12,r3              / multiply the exponent by ten and
	add     r4,r3               / add the new digit.
	br      2b                  / get some more
1:
	tst     r2                  / check sign of exponent
	beq     1f
	neg     r3
1:      add     r3,dpoint           / add the exponent to the decimal
	tst     count               / point counter
	beq     bad                 / check to see if we had any digits
out2:
	dec     _point              / adjust the character pointer
	tst     dpoint              / check to see if number can be
	ble     1f                  / multiplied by ten if need be.
2:      bit     $!07,areg
	bne     1f                  /  no
	clr     r2
	jsr     pc,tenmul           / multiply by ten
	dec     dpoint
	bne     2b
1:
	tst     areg                / check to see if we have an integer
	bne     1f
	tst     areg+2
	bne     1f
	tst     areg+4
	bne     1f
	tst     dpoint
	bne     2f
	bit     $100000,areg+6
	beq     3f
2:      tst     areg+6              / test for zero
	bne     1f
3:      mov     areg+6,_res         / yes we have an integer put the
	movb    $1,_vartype         / value in 'res' and set 'vartype'
	inc     r0                  / stop bad number error, since at this
	jmp     cret                / point r0 is zero.
1:
	mov     $56.,aexp           / convert to floating point format
	jsr     pc,norm
	tst     dpoint              / number wants to be multiplied
	ble     2f                  / by ten
	cmp     dpoint,$1000.
	bgt     bad
1:      clr     r2
	jsr     pc,tenmul           /  do it
3:      bit     $!377,areg          / normalise the number
	bne     1f
	dec     dpoint              / decrement the counter
	bne     1b
	br      2f
1:      mov     $areg,r0            / shift right to normalise
	asr     (r0)+
	ror     (r0)+
	ror     (r0)+
	ror     (r0)+
	inc     aexp
	cmp     aexp,$177
	bgt     bad
	br      3b
2:
	tst     dpoint              / wants to be divided by ten
	bge     2f
3:      mov     $3,r1
1:      mov     $areg+8,r0          / shift left to save significant
	asl     -(r0)               / digits
	rol     -(r0)
	rol     -(r0)
	rol     -(r0)
	dec     aexp
	sob     r1,1b
	jsr     pc,tendiv           / divide number by ten
1:      bit     $200,areg           / normalise number
	bne     1f
	mov     $areg+8,r0          / shift left
	asl     -(r0)
	rol     -(r0)
	rol     -(r0)
	rol     -(r0)
	dec     aexp
	br      1b
1:      inc     dpoint
	bne     3b
2:
	cmp     aexp,$177       / check for overflow
	bgt     bad
	mov     $_res,r2        / return value to 'res' via the floating
	jmp     retng           / point return routine, r0 is non-zero
bad:    clr     r0              / bad number , clear r0
	jmp     cret            / return

.bss
dflag:  .=.+2                   / temporary space for decimal point counter

.text

/   cmp() is used to compare two numbers , it uses 'vartype' to decide
/   which kind of variable to test.
/   The result is -1,0 or 1 , depending on the result of the comparison
/

.globl  _cmp   , _vartype

_cmp:   mov     2(sp),r0
	mov     4(sp),r1
	tstb    _vartype
	beq     6f
	cmp     (r0)+,(r1)+
	blt     4f
	bgt     3f
5:      clr     r0
	rts     pc
3:      mov     $1,r0
	rts     pc
4:      mov     $-1,r0
	rts     pc
				/ floating point comparisons
6:      tst     (r0)            / straight out of the floating
	bge     1f              / point trap routines
	tst     (r1)
	bge     1f
	cmp     (r0),(r1)
	bgt     4b
	blt     3b
1:
	cmp     (r0)+,(r1)+
	bgt     3b
	blt     4b
	cmp     (r0)+,(r1)+
	bne     1f
	cmp     (r0)+,(r1)+
	bne     1f
	cmp     (r0)+,(r1)+
	beq     5b
1:
	bhi     3b
	br      4b

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

/   The calling routines for the maths functions ( from bas3.c).
/   The arguments passed to the routines are as follows.
/       at  6(sp)   The operator funtion required.
/       at  4(sp)   The pointer to second parameter and
/                   the location where the result is to be put.
/       at  2(sp)   The pointer to the first parameter.

/   The jump table is called by the following sequence:-
/       (*mbin[priority*2+vartype])(&j->r1,&res,j->operator)
/
/   So the values in this table are such that integer and real
/   types are dealt with separately, and the different types of operators
/   are also dealt with seperately.
/       e.g. *, /, mod for reals are dealt with by 'fmdm'
/            and , or , xor for integers are dealt with by 'andor'
/

.globl  _mbin   , csv , cret , _error  , _fmul , _fdiv , _fadd , _fsub

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

ex:     mov     2(sp),-(sp)
	jsr     pc,_cvt
	mov     6(sp),(sp)
	jsr     pc,_cvt
	tst     (sp)+
	clrb    _vartype
fex:    jmp     _fexp


fmdm:
	cmp     $'*,6(sp)       / times
	bne     1f
	jmp     _fmul
1:
	cmp     $'/,6(sp)       / div
	bne     1f
	jmp     _fdiv
1:
	jmp     _fmod           / mod


mdm:    cmp     $'*,6(sp)       / integer multiply
	bne     1f
	mov     *2(sp),r0
	mul     *4(sp),r0
	bcs     over            / overflow
	br      2f
1:      mov     *2(sp),r1       / divide or mod
	sxt     r0
	div     *4(sp),r0
	bvs     1f
	cmp     $'/,6(sp)       / div
	bne     2f              / no , must be mod.
	tst     r1
	bne     3f
	mov     r0,*4(sp)
	rts     pc
2:      mov     r1,*4(sp)
	rts     pc
1:      mov     $25.,-(sp)      / zero divisor error
	jsr     pc,_error
				/ code to do integer divisions.. etc.
3:      mov     2(sp),-(sp)     / if the result of the integer division
	jsr     pc,_cvt         / is not an integer then convert to
	mov     6(sp),(sp)      / float and call the floationg point
	jsr     pc,_cvt         / routine
	clrb    _vartype
	tst     (sp)+
	jmp     _fdiv

fads:                           / floating add and subtract
	cmp     $'+,6(sp)
	bne     1f
	jmp     _fadd
1:
	jmp     _fsub


ads:    mov     *2(sp),r1
	cmp     $'+,6(sp)           / add or subtract
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
	mov     2(sp),r1
	mov     (r1)+,r0
	beq     3f
	mov     (r1),r1
	asl     r0
	clrb    r0
	swab    r0
	sub     $200,r0
	cmp     r0,$20
	bge     1f                  / overflow or underflow
	sub     $8,r0
	mov     r0,-(sp)            / counter
	mov     *4(sp),r0
	bic     $!0177,r0
	bis     $200,r0
	ashc    (sp)+,r0
	tst     *2(sp)
	bpl     3f
	neg     r0
3:
	mov     r0,*2(sp)
	clr     r0
	rts     pc

1:      bne     1f
	cmp     *2(sp),$144000  / check for -32768
	bne     1f
	bit     r1,$177400
	bne     1f
	mov     $-32768.,r0
	br      3b
1:      rts     pc


/   convert from integer to floating point , this will never fail.
/

.globl  _cvt
_cvt:   mov     r2,-(sp)
	clr     r0
	mov     *4(sp),r1
	beq     4f
	bpl     1f
	neg     r1
1:      mov     $220,r2                 /counter
	ashc    $8,r0
1:      bit     $200,r0
	bne     1f
	ashc    $1,r0
	dec     r2
	br      1b
1:      swab    r2
	ror     r2
	tst     *4(sp)
	bpl     1f
	bis     $100000,r2
1:      bic     $!177,r0
	bis     r2,r0
4:      mov     4(sp),r2
	mov     r0,(r2)+
	mov     r1,(r2)+
	clr     (r2)+
	clr     (r2)+
	mov     (sp)+,r2
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

/   This routine converts a floating point number into decimal
/   It uses the following algorithm:-
/   forever{
/       If X > 1 then {
/               X = X / 10
/               decpoint++
/               continue
/               }
/       If X < 0.1 then {
/               X = X * 10
/               decpoint--
/               continue
/               }
/           }
/       for i = 1 to 10 do {
/               digit[i] = int ( X * 10)
/               X = frac ( X * 10 )
/               }
/   This routine is not very complicated but very fiddly so was one
/   of the last ones written.
/


.globl  _necvt  , tendiv , tenmul

_necvt: jsr     r5,csv          / needs to look like ecvt to
	clr     dpoint          / the outside world
	clr     *10.(r5)
	mov     $buf,r3
	mov     6(r5),r2
	mov     r2,mdigit
	inc     r2
	mov     r2,count
	tst     *4(r5)
	beq     zer
	bpl     1f
	inc     *10.(r5)        / sign part of ecvt
1:      mov     4(r5),r2
	mov     $asign,r0
	jsr     pc,seta         / set up number in areg
1:      tst     aexp
	ble     1f
	mov     $3,r1           / number is greater than one
2:      mov     $areg+8,r0
	asl     -(r0)           / save significant digits
	rol     -(r0)
	rol     -(r0)
	rol     -(r0)
	dec     aexp
	sob     r1,2b
	jsr     pc,tendiv
	inc     dpoint          / increment decimal point
2:      bit     $200,areg
	bne     1b
	mov     $areg+8,r0      / normalise after the division
	asl     -(r0)
	rol     -(r0)
	rol     -(r0)
	rol     -(r0)
	dec     aexp
	br      2b
1:
	cmp     aexp,$-3            / number greate than 0.1
	bgt     5f
	blt     2f
	cmp     areg,$314
	bgt     5f
	blt     2f
	mov     $3,r1
	mov     $areg+2,r0
3:      cmp     (r0)+,$146314
	bhi     5f
	blo     2f
	sob     r1,3b
2:                                  / no
	clr     r2
	jsr     pc,tenmul           / multiply by ten
3:      tstb    areg+1
	bne     4f
	dec     dpoint
	br      1b
4:
	mov     $areg,r0            / normalise
	asr     (r0)+
	ror     (r0)+
	ror     (r0)+
	ror     (r0)+
	inc     aexp
	br      3b
5:
	tst     aexp                / get decimal point in correct place
	beq     9f
1:      mov     $areg,r0
	asr     (r0)+
	ror     (r0)+
	ror     (r0)+
	ror     (r0)+
	inc     aexp
	bne     1b
9:
	clr     r2                  / get  the digits
	jsr     pc,tenmul
	bic     $!377,areg
	clrb    r1                  / top word in r1
	swab    r1
	add     $'0,r1
	movb    r1,(r3)+
	dec     count               / got all digits
	bne     9b
	br      out

zer:    inc     dpoint              / deal with zero
1:      movb    $'0,(r3)+
	sob     r2,1b
out:                                / correct the last digit
	mov     $buf,r0
	add     mdigit,r0
	movb    (r0),r2
	add     $5,r2
	movb    r2,(r0)
1:
	cmpb    (r0),$'9
	ble     1f                  / don't correct it
	movb    $'0,(r0)
	cmp     r0,$buf
	blos    2f
	incb    -(r0)
	br      1b
2:
	inc     dpoint
	movb    $'1,(r0)            / correction has made number a one
1:
	mov     mdigit,r0           / pass values back
	clrb    buf(r0)
	mov     $buf,r0
	mov     dpoint,*8(r5)
	jmp     cret

tenmul:                             / multiply value in areg by 10
	mov     $areg+8.,r4
1:      mov     -(r4),r0
	mul     $12,r0
	bpl     2f
	add     $12,r0
2:      add     r2,r1
	adc     r0
	mov     r1,(r4)
	mov     r0,r2
	cmp     r4,$areg
	bne     1b
	rts     pc

tendiv:                             / divide value in areg by 10
	mov     $areg,r4
	clr     r0
1:      mov     (r4),r1             / has to divide by 20 to stop
	div     $24,r0              / multiply thinking there is an
	asl     r0                  / overflow
	cmp     r1,$9
	ble     2f
	inc     r0
	sub     $12,r1
2:      mov     r0,(r4)+
	mov     r1,r0
	cmp     r4,$areg+8
	bne     1b
	rts     pc

	.bss
mdigit: .=.+2
count:  .=.+2
buf:    .=.+20.
dpoint: .=.+2
	.text

/   convert a long in 'overfl' to a real. uses the floating point
/   routines. returns via these routines.

.globl  _over
_over:
	jsr     r5,csv
	clrb    _vartype
	mov     _overfl,areg
	mov     _overfl+2,areg+2
	clr     areg+4
	clr     areg+6
	mov     $1,asign
	mov     $32.-8,aexp
	jmp     saret

/
/   put a value into a variable , does the convertions from integer
/   to real and back as needed.
/

.globl  _putin
_putin: cmpb    4(sp),_vartype
	beq     3f
	mov     $_res,-(sp)
	tstb    6(sp)
	beq     2f
	jsr     pc,_conv
	tst     r0
	beq     1f
	mov     $35.,(sp)
	jsr     pc,_error       / no return
2:      jsr     pc,_cvt
1:      tst     (sp)+
3:      mov     $_res,r0
	mov     2(sp),r1
	mov     (r0)+,(r1)+
	tstb    4(sp)           / type of variable that is to be assigned
	bne     1f              / to
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
1:      rts     pc

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
1:      rts     pc
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
1:      mov     $44.,(sp)
	jsr     pc,_error       / no return
