
/****** string.c ******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Jan 20, 1987          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

#include <stdio.h>
#include "struct.h"
#include "node.h"
#include "umax.h"
#include "string.h"

/* Single character strings, CharString [0] = null string */
StrPtr *CharString;  

/* Free string segments have SRef = 1 and are linked by StrNext link */
StrPtr FreeString = NULL;

/*
 * NewSCell
 *
 * return pointer to fresh string cell with SRef = 1 and StrNext = NULL.
 *
 * A SysError may occur, in which case the NULL pointer is returned.
 */
private StrPtr NewSCell ()
   {
      extern StrPtr AllocStrPage ();
      register StrPtr S;

      semaphore_wait (SRefSemaphore);
      if (FreeString != NULL || (FreeString = AllocStrPage ()) != NULL) {
	 S = FreeString;
	 FreeString = S->StrNext;
	 S->SRef = 1;
	 S->StrNext = NULL;
      }
      else {
	 SysError = NO_STR_FREE;
	 printf ("NO MORE STRING CELLS LEFT\n");
	 S = NULL;
      }
      semaphore_signal (SRefSemaphore);
      return S;
   }

/*
 * CPInit
 *
 * Initialize a character pointer.
 */
void CPInit (U,S)
   register CharPtr *U;
   register StrPtr *S;
   {
      if ((U->CPSeg = *(U->CPStr = S)) == NULL) U->CPCount = 0;
      else {
	 U->CPCount = StrHeadLen;
	 U->CPChar = (*S)->StrChar;
      }
   }

/*
 * CPRead
 *
 * Read up to N-1 characters from and advance a character pointer.
 * '\0' is returned as the last character of the string.
 *
 * Input
 *     *U = character pointer
 *     Buf = buffer into which to read characters
 *     N-1 = number of characters to read
 *
 * Output
 *      result = true if characters were read, 0 if end of string.
 *      Buf = string of characters terminated by '\0'
 */
boolean CPRead (U,Buf,N)
   register CharPtr *U;
   register char *Buf;
   register int N;
   {
      register char *S;
      register int K;

      if (!U->CPCount && (NULL==U->CPSeg || NULL==U->CPSeg->StrNext) ||
	  !*(S = U->CPChar)) {

	 *Buf = '\0';
	 return 0;

      } else {

	 --N;
	 while (N > 0) {
	    K = U->CPCount;
	    if (K > N) K = N;
	    N -= K;
	    U->CPCount -= K;
	    while (--K >= 0) *Buf++ = *S++;
	    if (!U->CPCount) {
	       if (NULL == (U->CPSeg = U->CPSeg->StrNext)) break;
	       else {
		  U->CPCount = StrTailLen;
		  S = U->CPSeg->StrChar;
	       }
	    }
	 }
	 U->CPChar = S;
	 *Buf = '\0';
	 return 1;
      }
   }


/*
 * CPAppend
 *
 * Append a character to the end of a string.
 *
 * A SysError may occur.
 */
void CPAppend (U,C)
   register CharPtr *U;
   char C;
   {
      if (U->CPCount-- == 0)
	 if (C == '\0') return;
	 else {
	    register StrPtr S = NewSCell ();
	    if (SysError) return;
	    else {
	       U->CPChar = S->StrChar;
	       if (*U->CPStr == NULL) {
		  U->CPSeg = (*U->CPStr = S); 	      /* Append head segment */
		  U->CPCount = StrHeadLen-1;
	       } else {
		  U->CPSeg = (U->CPSeg->StrNext = S); /* Append tail segment */
		  U->CPCount = StrTailLen-1;
	       }
	    }
	 }
      *U->CPChar++ = C;
   }


/*
 * LenStr
 *
 * Find the length of a FP string
 * 
 * Input
 * 	S = IFP string
 *
 * Output
 *	result = length of string in characters
 */
FPint LenStr (S)
   register StrPtr S;
   {
      register int J = StrHeadLen;
      register FPint K = 0;
      register char *T;

      for (; S!=NULL; S = S->StrNext) {
	 for (T = S->StrChar; --J >= 0 && *T; T++) K++;
	 J = StrTailLen;
      }
      return K;
   }


/*
 * DelSPtr
 *
 * Delete a string pointer: decrement reference count and remove string
 * if reference count is zero.
 */
void DelSPtr (S)
   register StrPtr S;
   {
      register StrPtr T;

      semaphore_wait (SRefSemaphore);
      if (S != NULL && !-- S->SRef) {
	 for (T=S; T->StrChar[0]='\0', T->StrNext!=NULL; T=T->StrNext) continue;
	 T->StrNext = FreeString;
	 FreeString = S;
      }
      semaphore_signal (SRefSemaphore);
   }

/*
 * NewString
 *
 * Make a copy of a string.  The old string retains its reference count.
 *
 * Input
 *    S = pointer to string
 *
 * Output
 *    result = pointer to new string
 *
 * A SysError may occur, in which case NULL is returned.
 */
private StrPtr NewString (S)
   register StrPtr S;
   {
      extern char *strncpy ();
      register StrPtr R,T;

      if (S == NULL) return NULL;
      R = T = NewSCell ();   /* R = root of copy */
      if (SysError) return NULL;
      (void) strncpy (T->StrChar,S->StrChar,StrHeadLen);
      while ((S=S->StrNext) != NULL) {
	 T->StrNext = NewSCell ();
	 T = T->StrNext;
	 (void) strncpy (T->StrChar,S->StrChar,StrTailLen);
	 if (SysError) {
	    DelSPtr (R);   /* flush copy */
	    return NULL;
	 }
      }
      return R;
   }


/*
 * MakeString
 *
 * Make an IFP string from a C string.
 *
 * Input
 *      S = pointer to character array terminated by '\0'
 *
 * Output
 *      result = pointer to IFP (segmented) string
 *
 * A SysError may occur, in which case a NULL pointer is returned.
 */
StrPtr MakeString (S)
   char *S;
   {
      extern char *strncpy ();
      int L=strlen(S);

      if (L <= 0) return NULL;
      else {
         StrPtr R,T;
	 int N = StrHeadLen;
	 R = T = NewSCell ();                /* R = root of copy */
	 if (SysError) return NULL;
	 while (1) {
	    (void) strncpy (T->StrChar,S,N);
	    if ((L -= N) <= 0) return R;
	    else {
	       S += N;
	       T->StrNext = NewSCell ();
	       if (SysError) {
		  DelSPtr (R);   /* flush copy */
		  return NULL;
	       }
	       T = T->StrNext;
	       N = StrTailLen;
	    }
	 }
      }
   }

/*
 * StrComp
 *
 * Compares two strings.  Returns P-Q
 */
int StrComp (P,Q)
   StrPtr P,Q;
   {
      register int Diff,Len;
      Len = StrHeadLen;
      while (1) {
	 if (Q == NULL) return P!=NULL;
	 else if (P == NULL) return -(Q!=NULL);
	 else if (Diff = strncmp (P->StrChar,Q->StrChar,Len)) return Diff;
	 else {
	    Len = StrTailLen;
	    P = P->StrNext;
	    Q = Q->StrNext;
	 }
      }
   }

/*
 * Make a copy of a non-null string pointer, incrementing the reference count.
 *
 * A SysError may occur, in in which case a NULL pointer is returned.
 */
StrPtr CopySPtr (S)
   StrPtr S;
   {
      semaphore_wait (SRefSemaphore);
      if (S != NULL && !++S->SRef) {
	 S->SRef--;
	 S = NewString (S);
      }
      semaphore_signal (SRefSemaphore);
      return S;
   }

/*
 * InitString
 *
 * Initialize this module
 */
void InitString ()
   {
      int C; 
      StrPtr S;

      CharString = (StrPtr *) malloc (128 * sizeof (StrPtr));
      CharString [0] = NULL;
      for (C = 1; C<128; C++) {
	 CharString [C] = S = NewSCell ();
	 S->StrChar [0] = C;
	 S->StrChar [1] = '\0';
      }
   }

/************************** end of string.c **************************/

