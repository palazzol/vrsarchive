/*
 *	%W% %G%
 *	Close system call, used when descriptor is not a socket.
*/
	TITLE   _close

#define READ	3
#define WRITE	4
#define OPEN	5
#define CLOSE	6

#ifdef M_LDATA
	EXTRN	sysc:far
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
	PUBLIC	__close
#ifdef M_STEXT
__close	PROC	NEAR
	mov	__Syspc,OFFSET __close
#else  M_LTEXT
__close	PROC	FAR
	mov	__Syspc,OFFSET __close
	mov	__Syspc+2,SEG __close
#endif M_TEXT
	mov	ax,CLOSE
#ifdef M_SDATA
	EXTRN	sys1a:near
	jmp	sys1a
#else  M_LDATA
	jmp	sysc
#endif M_DATA
__close	ENDP
_TEXT	ENDS
END
