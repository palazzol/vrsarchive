
/****** inob.c ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Aug 6, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/*************** object input parser (recursive descent) ***************/


#include <stdio.h>
#include <ctype.h>
#include "struct.h"
#include "node.h"
#include "string.h"
#include "inob.h"

/*
 * ObDelim
 *
 * Theses characters delimit objects.
 * Compare with NodeDelim in inimport.c 
 */
private char ObDelim[] = " ,<>|[](){};:\t\n";

/*
 * InBlanks
 *
 * Skip to first non-blank character not in comment.
 *
 * Input
 * 	F = input descriptor
 *
 * Output
 *	F = input descriptor pointing to non-blank character
 */
void InBlanks (F)
   register InDesc *F;
   {
      while (1) {

	 while (1) {
	    if (!*F->InPtr)
	       if (F->InLineNum >= 0) 
		  if (NULL != fgets (F->InBuf,INBUFSIZE,F->InFile)) {
		     F->InPtr = F->InBuf;
		     F->InLineNum++;
		  }
	    if (!isspace (*F->InPtr)) break;
	    F->InPtr++;
	 }

	 if (*F->InPtr == '(' && F->InPtr[1] == '*') {
	    F->ComLevel++;
	    F->InPtr+=2;
	 } else if (*F->InPtr == '*' && F->InPtr[1] == ')') {
	    F->ComLevel--;
	    F->InPtr+=2;
	 } else if (F->ComLevel && *F->InPtr) F->InPtr++;
	 else break;
      }
   }

/*
 * IsTok
 *
 * Check if next token in input is S.  Skip if found.
 */
boolean IsTok (F,S)
   InDesc *F;
   register char *S;
   {
      register char *T;

      for (T = F->InPtr; *S; S++,T++)
	 if (*S != *T) return 0;

      /* Check if alphabetic token is prefix of longer token */
      if (isalpha (T[-1]) && isalpha (T[0])) return 0; 

      F->InPtr = T;
      InBlanks (F);
      return 1;
   }

/*
 * InString
 *
 * Input a string. 
 *
 * Input
 *      *F = input descriptor pointing to first character of string
 *      Delim = string of non-alphanumeric delimiters
 *      Quoted = skip closing delimiter
 *
 * Output
 *      *F = input descriptor pointing to next token after string
 *      X = string object
 *      result = pointer to string, NULL if SysError or empty string.
 *
 * A SysError may occur, in which case X = bottom.
 */
StrPtr InString (F,X,Delim,Quoted)
   register InDesc *F;
   ObjectPtr X;
   char *Delim;
   boolean Quoted;
   {
      CharPtr U;
      register char C;

      RepTag (X,STRING);
      X->String = NULL;
      CPInit (&U,&X->String);
      do {
         extern char *strchr ();
	 C = *F->InPtr++;
	 if (!isalnum (C) && NULL != strchr (Delim,C)) C = '\0';
	 CPAppend (&U,C);
	 if (SysError) {RepTag (X,BOTTOM); return NULL;}
      } while (C);
      if (!Quoted) F->InPtr--;
      InBlanks (F);
      return X->String;
   }


/*
 * InList
 *
 * Input a list
 *
 * Input
 *     F = input descriptor pointing to first token after '<'
 *
 * Output
 *     result = true iff no error occurs
 *     *X = sequence, or unchanged if error occurs.
 */
private boolean InList (F,X)
   register InDesc *F;
   ObjectPtr X;
   {
      ListPtr R=NULL;
      register MetaPtr A = &R;

      while (!IsTok (F,">")) {
	 if (!*F->InPtr) {
	    DelLPtr (R);
	    return InError (F,"unfinished sequence");
	 }   
	 NewList (A,1L);
	 if (SysError || !InObject (F,&(*A)->Val)) {
	    DelLPtr (R);
	    return 0;
	 }
	 A = & (*A)->Next;
	 (void) IsTok (F,",");
      }
      RepTag (X,LIST);
      X->List = R;
      return 1;
   }

/*
 * InObject
 *
 * Read an object.
 *
 * Input
 *      *F = input descriptor pointing to object
 *
 * Output
 *      *F = input descriptor pointing to next token
 *	result = true iff object is read successfully.
 *
 * A SysError may occur, in which case X is unchanged.
 */
boolean InObject (F,X)
   register InDesc *F;
   register ObjectPtr X;
   {
      if (IsTok (F,"<")) return InList (F,X);

      else if (IsTok (F,"(")) {

	 (void) InComp (F,X,NIL);
	 if (!IsTok (F,")")) return InError (F,"')' expected");

      } else { 

	 /* Input atom */

	 static char Delim[2] = {'\0','\0'};
	 *Delim = *F->InPtr;

	 if (*Delim == '\"' || *Delim == '\'') {
	    F->InPtr++;
	    (void) InString (F,X,Delim,1);
	 } else {

	    FPint K;
	    register StrPtr S = InString (F,X,ObDelim,0);
	    if (S == NULL) return SysError || InError (F,"object expected");
	    if (S->StrChar[1] == '\0')
	       switch (S->StrChar[0]) {
		  case 'f':
		     RepBool (X,0);
		     return 1;
		  case 't':
		     RepBool (X,1);
		     return 1;
		  case '?':
		     RepTag (X,BOTTOM);
		     return 1;
	       }
	    if (StrToFloat (X) && !GetFPInt (X,&K)) {
	       X->Tag = INT;
	       X->Int = K;
	    } 
	 }
      }
      return 1;
   }

/*
 * InitIn
 *
 * Initialize input descriptor for node N and file FileDesc.
 * Advance the input pointer to the first token.
 *
 * Input
 *	*F = input descriptor
 *	M = module pointer
 *	FileDesc = open file descriptor
 *	LineNum = 0 for normal input, -1 if single-line mode
 */
void InitIn (F,M,FileDesc,LineNum)
   register InDesc *F;
   NodePtr M;
   FILE *FileDesc;
   int LineNum;
   {
      F->InFile = FileDesc;
      F->InLineNum= LineNum;
      F->InPtr = F->InBuf;
      *F->InPtr = '\0';
      F->InDefMod = M;
      F->ComLevel = 0;
      InBlanks (F);
   }


/******************************* end of inob.c *******************************/

