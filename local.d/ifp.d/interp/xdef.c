
/****** xdef.c ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   Aug 4, 1986          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Aug 4, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/************************* Extended Definitions ************************/

#include <stdio.h>
#include "struct.h"
#include "node.h"
#include "inob.h"

#if XDEF

ListPtr Environment = NIL;

/*
 * OutLHS
 *
 * Input
 *      P = LHS to output
 */
void OutLHS (InOut)
   ObjectPtr InOut;
   {
      switch (InOut->Tag) {
	 case LIST: {
	    register ListPtr P=InOut->List;
	    printf ("[");
	    if (P!=NIL)
	       while (1) {
		  if (Debug & DebugRef) printf ("{%d}",P->LRef + (1 - LRefOne));
		  OutLHS (& P->Val);
		  if ((P=P->Next) == NULL) break; 
		  else printf (",");
	       }
	    printf ("]");
	    break;
	 }
	 default: OutObject (InOut);
      }
   }

/*
 * Assign
 *
 * Assign functional variables.
 *
 * Input
 *	X = object to be matched with LHS.
 *	F = LHS
 */
private boolean Assign (X,F)
   ObjectPtr X,F;
   {
      register ListPtr P,Q;
      extern StrPtr CopySPtr();

      switch (F->Tag) {

	 case STRING:
	    NewList (&Environment,2L);
	    P = Environment;
	    P->Val.Tag = STRING;
	    P->Val.String = CopySPtr (F->String);
	    CopyObject (&P->Next->Val,X);
	    return 1;

	 case LIST:
	    if (X->Tag != LIST) return 0;
	    else {
	       for (Q=X->List,P=F->List; P!=NULL; Q=Q->Next,P=P->Next) 
		  if (Q==NULL || !Assign (&Q->Val,&P->Val)) return 0;
	       return 1;
	    }

	 default: 
	    return 0;
      } 
   }

/*
 * FF_XDef
 *
 * Apply function F to each element of list InOut
 *
 * Input
 *      InOut = list of elements to apply function
 *      Funs = <lhs rhs function>
 *
 * Output
 *      InOut = result
 */
FF_XDef (InOut,Funs)
   ObjectPtr InOut;
   register ListPtr Funs;
   {
      ListPtr P;
      Object X;
      boolean InRange;

      if (3L != ListLength (Funs)) {
	 FormError (InOut,"invalid xdef",NULL,Funs);
	 return;
      }
      CopyObject (&X,InOut);
      Apply (&X,&Funs->Next->Val);
      P = Environment;
      InRange = Assign (&X,&Funs->Val);
      RepTag (&X,BOTTOM);
      if (InRange) 
	 Apply (InOut,&Funs->Next->Next->Val);
      else if (PrintErr (InOut)) {
	 OutLHS (&Funs->Val);
	 printf (": domain error\n");
	 OutObject (InOut);
	 printf ("\n");
	 RepTag (InOut,BOTTOM);
      }
      RepLPtr (&Environment,P);
   }

/*
 * InLHSC
 * 
 * Input
 *     F = input descriptor pointing to '['
 *
 * Output
 *     result = true iff no error occurs
 *     *X = sequence, or unchanged if error occurs.
 */
private boolean InLHSC (F,X,Env)
   register InDesc *F;
   ObjectPtr X;
   ListPtr *Env;
   {
      register MetaPtr A;
      ListPtr R;

      *(A = &R) = NULL;
      F->InPtr++; 
      InBlanks (F);
  
      while (']' != *F->InPtr) {
	 if (!*F->InPtr) {
	    DelLPtr (R);
	    return InError (F,"unfinished construction");
	 }
	 NewList (A,1L);
	 if (SysError || !InLHS (F,&(*A)->Val,Env)) {
	    DelLPtr (R);
	    return 0;
	 }
	 A = & (*A)->Next;
	 if (*F->InPtr == ',') {
	    F->InPtr++;
	    InBlanks (F);
	 }
      }
      F->InPtr++;              /* Skip closing ']' */
      InBlanks (F);
      RepTag (X,LIST);
      X->List = R;
      return 1;
   }

/*
 * InLHS
 *
 * Read a left-hand-side of a functional variable definition.
 * Return true iff no error occurred.
 *
 * Input
 *      *F = input descriptor pointing to LHS
 *
 * Output
 *      *F = input descriptor pointing to next token
 *      *Lhs = left hand side	
 *	*Env = list of functional variables in LHS
 *
 * A SysError may occur, in which case X is unchanged.
 */
boolean InLHS (F,LHS,Env)
   register InDesc *F;
   register ObjectPtr LHS;
   ListPtr *Env;
   {
      register ListPtr P;

      if (Debug & DebugParse) printf ("InLHS: %s",F->InPtr);
      
      if (*F->InPtr == '[') return InLHSC (F,LHS,Env);
      else {
	 if (NULL == InString (F,LHS,NodeDelim,0)) 
	    return InError (F,"variable name expected");
	 for (P= *Env; P!=NULL; P=P->Next)
	    if (ObEqual (&P->Val,LHS)) 
	       return InError (F,"redefinition of variable (to left of caret)");
	 NewList (Env,1L);
	 CopyObject (&(*Env)->Val,LHS);
	 return 1;
      }
   }

#endif /* XDEF */

/******************************* end of xdef.c *******************************/

