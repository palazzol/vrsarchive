/               (c) P. (Rabbit) Cockcroft 1982
/   This file contains the routines to implement the some of the
/   more complex mathematical functions.
/   It currently contains the code for sqrt() , log() and exp()

/   The sqrt() routine is based on the the standard Newtonian method.
/   It uses mull and divv from nfp.s

.globl  _sqrt  , sqrt
/
/       for ( i = 0 ; i < 6 ; i++ )
/               areg = ( areg + creg / areg ) >> 1 ;
/

_sqrt:
	jsr     r5,csv
	mov     4(r5),r2
	mov     $asign,r0
	jsr     pc,seta
	jsr     pc,sqrt
	mov     4(r5),r2
	mov     $asign,r0
	jmp     retng


/ value in  areg

sqrt:
	tst     asign           / test for zero
	bne     1f
	rts     pc
1:
	bit     $1,aexp         / sort out the exponent
	beq     1f
	mov     $areg,r0        / shifting as need be
	asr     (r0)+
	ror     (r0)+
	ror     (r0)+
	ror     (r0)+
	inc     aexp
1:
	mov     $asign,r0       / save in creg
	mov     $csign,r1
	mov     $6,r2
1:
	mov     (r0)+,(r1)+
	sob     r2,1b

	asr     aexp            / initial guess in areg
	mov     $6.,-(sp)       / number of iterations

				/ main loop starts here
5:
	mov     $4,r2
	mov     $areg,r0
	mov     $breg,r1        / set up to do the division
1:                              / areg/breg
	mov     (r0)+,(r1)+
	sob     r2,1b
	mov     $4,r2
	mov     $creg,r0
	mov     $areg,r1
1:
	mov     (r0)+,(r1)+
	sob     r2,1b
	jsr     pc,divv         / the division
1:      mov     $areg+8,r0      / add result to old value
	mov     $breg+8,r1
	jsr     pc,addm
	mov     $areg,r0        / divide by two
	asr     (r0)+
	ror     (r0)+
	ror     (r0)+
	ror     (r0)+
	dec     (sp)            / decrement iteration counter
	bne     5b
	tst     (sp)+
	jsr     pc,norm         / normalise result
	rts     pc

/   The routines below handle the log and exp functions
/   They return zero if there is an error or on overflow
/   these routines are almost totally incomprehensible but the algorithms
/   are discussed in the report.


	ITER=11.        / loop count

.globl  _log
_log:
	jsr     r5,csv
	mov     4(r5),r2
	mov     $asign,r0
	jsr     pc,seta
	jsr     pc,log
	mov     4(r5),r2
	mov     $asign,r0
	jmp     retng

.globl  log

log:
	clr     pt
	mov     $creg,r0
	clr     (r0)+
	clr     (r0)+
	clr     (r0)+
	clr     (r0)+
1:
	mov     pt,r1
	mov     r1,r4
	mul     $3,r1
	mov     r1,pt1
3:
	mov     $areg,r0
	mov     $breg,r1
	jsr     pc,movm
	mov     pt1,r1
	beq     5f
	mov     $breg,r0
	jsr     pc,shiftl
5:
	mov     $breg+8,r0
	mov     $areg+8,r1
	jsr     pc,addm
	cmp     breg,$400
	bhi     2f
	blo     5f
	tst     breg+2
	bne     2f
	tst     breg+4
	bne     2f
	tst     breg+6
	bne     2f
5:
	mov     $areg,r1
	mov     $breg,r0
	jsr     pc,movm
	mov     pt,r1
	ash     $3,r1
	add     $logtable+8,r1
	mov     $creg+8,r0
	jsr     pc,addm
	br      3b
2:
	inc     pt
	cmp     pt,$ITER
	blt     1b                      / first loop finished

	sub     $400,areg
	mov     $creg+8,r1
	mov     $areg+8,r0
	jsr     pc,subm

	mov     aexp,r4                 / deal with the exponent
	beq     3f
	bmi     2f
1:
	mov     $logtable+8,r1          /log2n
	mov     $areg+8,r0
	jsr     pc,addm
	dec     r4
	bne     1b
	br      3f
2:
	mov     $logtable+8,r1          /log2n
	mov     $areg+8,r0
	jsr     pc,subm
	inc     r4
	bne     2b
3:
	tst     areg
	bpl     1f
	mov     $areg+8,r0
	jsr     pc,negat
	neg     asign
1:
	clr     aexp
	jsr     pc,norm
	rts     pc


.globl  _exp

_exp:
	jsr     r5,csv
	mov     4(r5),r2
	mov     $asign,r0
	jsr     pc,seta
	jsr     pc,exp
	bec     1f
	clr     r0
	jmp     cret
1:
	mov     4(r5),r2
	mov     $asign,r0
	jmp     retng

.globl  exp

exp:    clr     cexp
	tst     aexp            / test of exponent.
	bmi     1f
	beq     5f
	cmp     aexp,$7
	ble     4f
	sec
	rts     pc
4:
	mov     $areg+8,r0
	asl     -(r0)
	rol     -(r0)
	rol     -(r0)
	rol     -(r0)
	dec     aexp
	bne     4b
4:
	tstb    areg+1
	beq     5f
	mov     $logtable+8,r1
	mov     $areg+8,r0
	jsr     pc,subm
	inc     cexp
	br      4b
5:      mov     $logtable+8,r1
	mov     $areg+8,r0
	jsr     pc,subm
	tst     areg
	bpl     3f
	mov     $logtable+8,r1
	mov     $areg+8,r0
	jsr     pc,addm
	br      5f
3:      inc     cexp
	br      5f
1:
	mov     $areg,r0
	mov     aexp,r1
	neg     r1
	jsr     pc,shiftl

5:      mov     $1,r4           / main loop starts here
3:
	clrb    count(r4)
	mov     r4,r1
	ash     $3,r1
	add     $logtable+8,r1
	mov     r1,r3
2:
	mov     $areg+8,r0
	jsr     pc,subm
	tst     areg
	bmi     1f
	incb    count(r4)
	mov     r3,r1
	br      2b
1:
	mov     r3,r1
	mov     $areg+8,r0
	jsr     pc,addm
	inc     r4
	cmp     r4,$ITER
	blt     3b              / end of first loop
6:

	add     $400,areg
	mov     $1,pt
1:
	mov     pt,r1
	mul     $3,r1
	mov     r1,pt1
2:
	mov     pt,r4
	tstb    count(r4)
	beq     2f
	decb    count(r4)
	mov     $areg,r0
	mov     $breg,r1
	jsr     pc,movm
	mov     pt1,r1
	beq     5f
	mov     $breg,r0
	jsr     pc,shiftl
5:
	mov     $breg+8,r1
	mov     $areg+8,r0
	jsr     pc,addm
	br      2b
2:
	inc     pt
	cmp     pt,$ITER
	blt     1b
	tst     asign
	bne     3f
	inc     asign
3:
	mov     cexp,aexp
	jsr     pc,norm
	tst     asign
	bpl     1f
	jsr     pc,recip
	neg     asign
1:
	cmp     aexp,$177
	ble     1f
	sec
	rts     pc
1:
	clc
	rts     pc

.globl  recip
recip:
	mov     $areg,r0                / return reciprical of areg
	mov     $breg,r1                / done by division
	jsr     pc,movm
	mov     $200,areg
	clr     areg+2
	clr     areg+4
	clr     areg+6
	jsr     pc,divv
	neg     aexp
	inc     aexp
	jsr     pc,norm
	rts     pc


.bss
count:  .=.+12.                 / counters for the log and exp functs.
pt:     .=.+2
pt1:    .=.+2

.globl  logtable

.data
	/ log2n is in fact the first entry in logtable

logtable:
	000261; 071027; 173721; 147572
	000036; 023407; 067052; 171341
	000003; 174025; 013037; 100174
	000000; 077740; 005246; 126103
	000000; 007777; 100005; 052425
	000000; 000777; 177000; 001252
	000000; 000077; 177770; 000001
	000000; 000007; 177777; 160000
	000000; 000000; 177777; 177600
	000000; 000000; 017777; 177777
	000000; 000000; 001777; 177777
.text

.globl  _fexp
_fexp:  jsr     r5,csv              / do exponentiation
	mov     4(r5),r2
	mov     $asign,r0
	jsr     pc,seta
	tst     asign               / deal with 0^x
	beq     1f
	bmi     2f
	jsr     pc,log              / call log
	mov     6(r5),r2
	mov     $bsign,r0
	jsr     pc,seta
	jsr     pc,mull             / multiply
	add     bexp,aexp
	dec     aexp
	jsr     pc,xorsign
	jsr     pc,norm
1:
	jsr     pc,exp              / exponentiate
	bes     1f
	mov     6(r5),r2
	jmp     retng
1:      mov     $40.,-(sp)          / overflow in ^
	jsr     pc,_error
2:      mov     $41.,-(sp)          / negative value to ^
	jsr     pc,_error

/ trig functions that are not as yet implemented
/ put in as place holders. Calls error with illegal function

.globl  _sin , _cos , _atan
_sin:
_cos:
_atan:
	mov     $11.,-(sp)
	jsr     pc,_error

/   These routines do quad precision arithmetic and are called by many of
/   the higher mathematical functions. These are usually called with the
/   addresses of the operands in r0 and r1. (r0 is usually destination )

.globl  addm , subm , movm , shiftl , negat

addm:
	mov     $4,r2                  / add quad length
	clc
1:
	adc     -(r0)
	bcs     3f
	add     -(r1),(r0)
	sob     r2,1b
	rts     pc
3:
	add     -(r1),(r0)
	sec
	sob     r2,1b
	rts     pc


subm:                               / subtract quad length
	mov     $4,r2
	clc
1:
	sbc     -(r0)
	bcs     3f
	sub     -(r1),(r0)
	sob     r2,1b
	rts     pc
3:
	sub     -(r1),(r0)
	sec
	sob     r2,1b
	rts     pc

shiftl:                             / a misnomer
	mov     r5,-(sp)            / it actually shifts right
	mov     r1,r5               / the number of places in r1
	mov     (r0)+,r1
	mov     (r0)+,r2
	mov     (r0)+,r3
	mov     (r0)+,r4
1:
	asr     r1
	ror     r2
	ror     r3
	ror     r4
	sob     r5,1b
	mov     r4,-(r0)
	mov     r3,-(r0)
	mov     r2,-(r0)
	mov     r1,-(r0)
	mov     (sp)+,r5
	rts     pc

movm:                               / quad move - the parameters are the
	mov     (r0)+,(r1)+         / other way around
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	mov     (r0)+,(r1)+
	rts     pc


negat:                              / quad negation
	mov     $4,r1
	clc
1:
	adc     -(r0)
	bcs     2f
	neg     (r0)
2:
	sob     r1,1b
	rts     pc
