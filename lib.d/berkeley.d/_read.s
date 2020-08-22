/*
 *	%W% %G%
 *	Read system call, used when fd is not a socket.
*/
	TITLE   _read

#define READ	3
#define WRITE	4
#define OPEN	5
#define CLOSE	6

#ifdef M_LDATA
	EXTRN	sysn:far
#endif

_TEXT	SEGMENT  BYTE PUBLIC 'CODE'
_TEXT	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
	EXTRN	__Syspc:word
_BSS	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
DGROUP	GROUP	CONST,	_BSS,	_DATA
	ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP
_TEXT	SEGMENT
	PUBLIC	__read
#ifdef M_STEXT
__read 	PROC	NEAR
	mov	__Syspc,OFFSET __read
#else  M_LTEXT
__read 	PROC	FAR
	mov	__Syspc,OFFSET __read
	mov	__Syspc+2,SEG __read
#endif M_TEXT
	mov	ax,READ
#ifdef M_SDATA
	EXTRN	sys3a:near
	jmp	sys3a
#else  M_LDATA
	jmp	sysn
#endif M_DATA
__read	ENDP
_TEXT	ENDS
END
