	TITLE   alloca
#ifdef M_LTEXT
#define CALLT	FAR
#define RETSZ	4
#else
#define RETSZ	2
#define CALLT	NEAR
#endif
_ALLOCA	SEGMENT  BYTE PUBLIC 'CODE'
_ALLOCA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
DGROUP	GROUP	CONST,	_DATA
	ASSUME  CS: _ALLOCA, DS: DGROUP, SS: DGROUP, ES: DGROUP
_ALLOCA SEGMENT
EXTRN	__chkstk:CALLT
	PUBLIC	_alloca
_alloca	PROC CALLT
	push	bp		; Save pointer to caller's args
	mov	bp,sp		; Save my sp, allow use as base
	mov	ax,[bp+RETSZ+2]	; Find out how much is wanted
	cmp	ax,RETSZ+8	; Must alloca RETSZ+8 or more
	jg	ok		; User want more than 10?
	mov	ax,RETSZ+8	; No, give RETSZ+8
ok:	mov	cx,ax		; Remember through _chkstk call
	call	__chkstk	; Check for room, mangle sp
	mov	sp,bp		; Restore my sp
	sub	sp,cx		; Allocate the space
	add	sp,RETSZ+8	; his bp, return addr, my arg, saved si & di
	mov	ax,sp		; Return value is this sp
	push	[bp+RETSZ+6]	; Copy saved di register (for caller's return)
	push	[bp+RETSZ+4]	; Copy saved si register (for caller's return)
	push	0		; Push dummy argument for caller to pop
#ifdef M_LTEXT
	push	[bp+4]		; Push the return address
#endif
	push	[bp+2]		; Push the return address
	mov	bp,[bp]		; Restore user arg pointer
	ret
_alloca	ENDP
_ALLOCA	ENDS
END
