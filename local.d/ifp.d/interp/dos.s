;
;/****** dos.s**********************************************************/
;/**                                                                  **/
;/**                    University of Illinois                        **/
;/**                                                                  **/
;/**                Department of Computer Science                    **/
;/**                                                                  **/
;/**   Tool: IFP                         Version: 0.5                 **/
;/**                                                                  **/
;/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
;/**                                                                  **/
;/**   Revised by: Arch D. Robison       Date: Sept 28, 1985          **/
;/**                                                                  **/
;/**   Principal Investigators: Prof. R. H. Campbell                  **/
;/**                            Prof. W. J. Kubitz                    **/
;/**                                                                  **/
;/**                                                                  **/
;/**------------------------------------------------------------------**/
;/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
;/**                       All Rights Reserved.                       **/
;/**********************************************************************/

;/***** Assembly Language Routines for MS-DOS Implementation of IFP *****/

TITLE   dos

PUBLIC  _StackCheck, _SetCBrk
EXTRN	__chkstk:FAR

DOS_TEXT	SEGMENT  BYTE PUBLIC 'CODE'


	ASSUME  CS: DOS_TEXT
;
; SetCBrk
;
; Set control-C trapping for any DOS call.
;
_SetCBrk     PROC FAR
        mov ax,3301H
        mov dl,01H
        int 21H
	ret	
_SetCBrk     ENDP

;
; StackCheck
;
; Check if there is enough room on the stack and check for break signal
;
_StackCheck  PROC FAR
	push	bp
	mov	bp,sp
	mov	ax,64H
	call	FAR PTR __chkstk
        push es
        mov ah,2FH
        int 21H		; Dummy GET_DTA to look for control-C
        pop es
	mov	sp,bp
	pop	bp
	ret	
_StackCheck  ENDP

DOS_TEXT	ENDS
END

;/************************** end of dos.s **************************/
