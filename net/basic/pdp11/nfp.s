/       (c) P. (Rabbit) Cockcroft 1982
/   this file contains all the floating point routines to execute the four
/   basic mathematical functions. Also routines for exponentiation and the
/   floating mod function.
/
/   These routines are the same as used in the floating point simulator
/   but have been changed to make them more flexible and to enable the use
/   of C calling and return conventions.
/   They have also been modified so that they use instructions in the
/   extended arithmetic option for the PDP-11's e.g. sob's.
/

/   It is expected that during the reading of these routines that the
/   general principles behind floating point work and the general operation
/   of the floating point interpreter are understood.

/   definiton of all global variables.

.globl  _fadd  , _fsub  , _fmul , _fdiv , csv , cret  , areg , asign , aexp
.globl  seta , retng , norm , saret , divv , bsign , breg , bexp , retb , reta
.globl  csign , creg , cexp , mull , xorsign

/   All the standard mathematical functions expect the second argument to
/   be the place where the result is to be put. This is exactly how they are
/   called from the eval() routine. ( via mbin ).


_fadd:  jsr     r5,csv              / save the registers
	jsr     pc,setab            / set up the parameters (in areg and breg)
	br      1f

_fsub:  jsr     r5,csv
	jsr     pc,setab
	neg     bsign
1:
	tst     bsign               / test for adding zero
	beq     reta
	tst     asign
	beq     retb
	mov     areg+8,r1           / compare the exponents
	sub     breg+8,r1
	blt     1f
	beq     2f
	cmp     r1,$56.             / test for underflows
	bge     reta
	mov     $breg,r0
	br      4f
1:
	neg     r1
	cmp     r1,$56.
	bge     retb
	mov     $areg,r0
4:
	mov     r1,-(sp)
	mov     (r0)+,r1
	mov     (r0)+,r2
	mov     (r0)+,r3
	mov     (r0)+,r4
	add     (sp),(r0)
1:
	asr     r1                      / shift the required value
	ror     r2
	ror     r3
	ror     r4
	dec     (sp)
	bgt     1b
	mov     r4,-(r0)
	mov     r3,-(r0)
	mov     r2,-(r0)
	mov     r1,-(r0)
	tst     (sp)+
2:
	mov     $areg+8,r1
	mov     $breg+8,r2
	mov     $4,r0
	cmp     asign,bsign             / compare sign of arguments
	bne     4f
	clc
1:
	adc     -(r1)                   / signs are equal so add
	bcs     3f
	add     -(r2),(r1)
	sob     r0,1b
	br      5f
3:
	add     -(r2),(r1)
	sec
	sob     r0,1b
	br      5f
4:
	clc
1:
	sbc     -(r1)                       / signs are not so subtract
	bcs     3f
	sub     -(r2),(r1)
	sob     r0,1b
	br      5f
3:
	sub     -(r2),(r1)
	sec
	sob     r0,1b
saret:                                      / return of a signed areg
	mov     $areg,r1
5:
	tst     (r1)                        / is it negative
	bge     3f
	mov     $areg+8,r1
	mov     $4,r0
	clc
1:
	adc     -(r1)                       / yes then make positive
	bcs     2f
	neg     (r1)
2:
	sob     r0,1b
	neg     -(r1)                       / negate sign of areg
3:
creta:

	jsr     pc,norm                     / normalise result
	br      reta

retb:
	mov     $bsign,r1
	mov     $asign,r2
	mov     $6,r0
1:
	mov     (r1)+,(r2)+
	sob     r0,1b
reta:
	mov     6(r5),r2                    / get return address
retng:
	mov     $asign,r0                / convert into normal representation
	tst     (r0)
	beq     unflo
	mov     aexp,r1                 / check for overflow
	cmp     r1,$177
	bgt     ovflo
	cmp     r1,$-177
	blt     unflo                   / check for overflow
	add     $200,r1
	swab    r1
	clc
	ror     r1
	tst     (r0)+
	bge     1f
	bis     $100000,r1
1:
	bic     $!177,(r0)
	bis     (r0)+,r1
	mov     r1,(r2)+
	mov     (r0)+,(r2)+
	mov     (r0)+,(r2)+
	mov     (r0)+,(r2)+
	jmp     cret
unflo:                                  / return zero on underflow
	clr     (r2)+
	clr     (r2)+
	clr     (r2)+
	clr     (r2)+
	jmp     cret

.globl  _error
ovflo:
	mov     $34.,-(sp)              / call error on overflow
	jsr     pc,_error
zerodiv:
	mov     $25.,-(sp)              / call error for zero divisor
	jsr     pc,_error

_fdiv:  jsr     r5,csv
	jsr     pc,setab                / setup parameters
	tst     bsign                   / check for zero divisor
	beq     zerodiv
	sub     bexp,aexp
	jsr     pc,xorsign              / set the signs correctly
	jsr     pc,divv                 / call the division routine
	jmp     creta                   / jump to return

divv:
	mov     r5,-(sp)                / this routine is taken straight
	mov     $areg,r0                / out of the floating point
	mov     (r0),r1                 / interpreter. If you have enough
	clr     (r0)+                   / time, try to find out how it
	mov     (r0),r2                 / works.
	clr     (r0)+
	mov     (r0),r3
	clr     (r0)+
	mov     (r0),r4
	clr     (r0)+
	mov     $areg,r5
	mov     $400,-(sp)              / ??????
1:
	mov     $breg,r0
	cmp     (r0)+,r1
	blt     2f
	bgt     3f
	cmp     (r0)+,r2
	blo     2f
	bhi     3f
	cmp     (r0)+,r3
	blo     2f
	bhi     3f
	cmp     (r0)+,r4
	bhi     3f
2:
	mov     $breg,r0
	sub     (r0)+,r1
	clr     -(sp)
	sub     (r0)+,r2
	adc     (sp)
	clr     -(sp)
	sub     (r0)+,r3
	adc     (sp)
	sub     (r0)+,r4
	sbc     r3
	adc     (sp)
	sub     (sp)+,r2
	adc     (sp)
	sub     (sp)+,r1
	bis     (sp),(r5)
3:
	asl     r4
	rol     r3
	rol     r2
	rol     r1
	clc
	ror     (sp)
	bne     1b
	mov     $100000,(sp)
	add     $2,r5
	cmp     r5,$areg+8
	blo     1b
	tst     (sp)+
	mov     (sp)+,r5
	rts     pc

_fmul:  jsr     r5,csv                  / almost same as _fdiv
	jsr     pc,setab
	add     bexp,aexp
	dec     aexp
	jsr     pc,xorsign
	jsr     pc,mull
	jmp     creta
mull:
	mov     r5,-(sp)                / also taken from the interpreter
	mov     $breg+8,r5
	clr     r0
	clr     r1
	clr     r2
	clr     r3
	clr     r4
1:
	asl     r0
	bne     2f
	inc     r0
	tst     -(r5)
2:
	cmp     r0,$400
	bne     2f
	cmp     r5,$breg
	bhi     2f
	mov     $areg,r0
	mov     r1,(r0)+
	mov     r2,(r0)+
	mov     r3,(r0)+
	mov     r4,(r0)+
	mov     (sp)+,r5
	rts     pc
2:
	clc
	ror     r1
	ror     r2
	ror     r3
	ror     r4
	bit     r0,(r5)
	beq     1b
	mov     r0,-(sp)
	mov     $areg,r0
	add     (r0)+,r1
	clr     -(sp)
	add     (r0)+,r2
	adc     (sp)
	clr     -(sp)
	add     (r0)+,r3
	adc     (sp)
	add     (r0)+,r4
	adc     r3
	adc     (sp)
	add     (sp)+,r2
	adc     (sp)
	add     (sp)+,r1
	mov     (sp)+,r0
	br      1b

.globl  _integ
_integ:
	jsr     r5,csv
	mov     4(r5),r2
	mov     $asign,r0
	jsr     pc,seta
	clr     r0
	mov     $200,r1
	clr     r2
1:
	cmp     r0,aexp
	blt     2f
	bic     r1,areg(r2)
2:
	inc     r0
	clc
	ror     r1
	bne     1b
	mov     $100000,r1
	add     $2,r2
	cmp     r2,$8
	blt     1b
	mov     4(r5),r2
	jmp     retng


.globl  _fmod
_fmod:
	jsr     r5,csv              / this routine cheats.
	jsr     pc,setab
	jsr     pc,divv             / the function 'a mod b' ==
	sub     bexp,aexp
	jsr     pc,norm
	clr     r0                  / count
	mov     $200,r1             / bit
	clr     r2                  / reg offset
1:
	cmp     r0,aexp
	bge     2f                  / in fraction
	bic     r1,areg(r2)         / this bit of code is taken from
2:                                  / the f.p. interpreter's mod function
	inc     r0                  / N.B. this does not do the same thing
	clc                         / as _fmod.
	ror     r1
	bne     1b
	mov     $100000,r1
	add     $2,r2
	cmp     r2,$8
	blt     1b
	jsr     pc,norm
	jsr     pc,mull
	add     bexp,aexp
	dec     aexp
	jmp     creta

xorsign:
	cmp     asign,bsign
	beq     1f
	mov     $-1,asign
	rts     pc
1:
	mov     $1,asign
	rts     pc

setab:
	mov     $asign,r0       / set up both areg and breg
	mov     4(r5),r2
	jsr     pc,seta
	mov     6(r5),r2
	mov     $bsign,r0

seta:
	clr     (r0)            / set up one register
	mov     (r2)+,r1
	mov     r1,-(sp)
	beq     1f
	blt     2f
	inc     (r0)+
	br      3f
2:
	dec     (r0)+
3:
	bic     $!177,r1
	bis     $200,r1
	br      2f
1:
	clr     (r0)+
2:
	mov     r1,(r0)+
	mov     (r2)+,(r0)+
	mov     (r2)+,(r0)+
	mov     (r2)+,(r0)+
	mov     (sp)+,r1
	asl     r1
	clrb    r1
	swab    r1
	sub     $200,r1
	mov     r1,(r0)+                / exp
	rts     pc

norm:
	mov     $areg,r0                / normalise the areg
	mov     (r0)+,r1
	mov     r1,-(sp)
	mov     (r0)+,r2
	bis     r2,(sp)
	mov     (r0)+,r3
	bis     r3,(sp)
	mov     (r0)+,r4
	bis     r4,(sp)+
	bne     1f
	clr     asign
	rts     pc
1:
	bit     $!377,r1
	beq     1f
	clc
	ror     r1
	ror     r2
	ror     r3
	ror     r4
	inc     (r0)
	br      1b
1:
	bit     $200,r1
	bne     1f
	asl     r4
	rol     r3
	rol     r2
	rol     r1
	dec     (r0)
	br      1b
1:
	mov     r4,-(r0)
	mov     r3,-(r0)
	mov     r2,-(r0)
	mov     r1,-(r0)
	rts     pc

.bss
asign:  .=.+2               / the areg      - sign
areg:   .=.+8                   /           - mantissa
aexp:   .=.+2                   /           - exponent
bsign:  .=.+2               / the breg
breg:   .=.+8
bexp:   .=.+2
csign:  .=.+2    / the creg - this register was added so that other functions
creg:   .=.+8               / could use this set up. e.g. sqrt()
cexp:   .=.+2               / it could be that when sin() is implemented a
			    / fourth register might be needed
