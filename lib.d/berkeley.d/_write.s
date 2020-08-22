/*
 *	%W% %G%
 *	Write system call, used when fd is not a socket.
*/
	TITLE   _write

#define READ	3
#define WRITE	4
#define OPEN	5
#define CLOSE	6

#ifdef M_LDATA
	EXTRN	sysn:far
#endif M_LDATA

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
	PUBLIC	__write
#ifdef M_STEXT
__write	PROC	NEAR
	mov	__Syspc,OFFSET __write
#else  M_LTEXT
__write	PROC	FAR
	mov	__Syspc,OFFSET __write
	mov	__Syspc+2,SEG __write
#endif M_TEXT
	mov	ax,WRITE
#ifdef M_SDATA
	EXTRN	sys3a:near
	jmp	sys3a
#else  M_LDATA
	jmp	sysn
#endif M_DATA
__write	ENDP
_TEXT	ENDS
END
