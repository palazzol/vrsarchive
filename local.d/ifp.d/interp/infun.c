
/****** infun.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
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


#include <stdio.h>
#include <ctype.h>
#include "struct.h"
#include "node.h"
#include "string.h"
#include "inob.h"

/*
 * PATTERN should be 0.  Setting it to 1 enables a parser extension
 * for experimental compiler work.
 */
#define PATTERN 0

/*
 * MakeForm
 *
 * If correct, create form with node N and function list Funs.
 *
 * Output
 *      result = 1 if no error, 0 otherwise
 */
boolean MakeForm (Correct,N,Funs,InOut)
   boolean Correct;
   NodePtr N;
   ListPtr Funs;
   ObjectPtr InOut;
   {
#ifdef PARAMBUG		/* cure for CRAY C-compiler bug (see struct.h) */
{
      ListPtr T = Funs;
      NewList (&T,1L);
      Funs = T;
}
#else
      NewList (&Funs,1L); 
#endif
      if (SysError || !Correct) {
	 DelLPtr (Funs);
	 return 0;
      } else {
	 Funs->Val.Tag = NODE;
	 Funs->Val.Node = CopyNPtr (N);
	 RepTag (InOut,LIST);
	 InOut->List = Funs;
	 return 1;
      }
   }

/*
 * InNext
 *
 * Input next composition, which should be followed by Token.
 *
 * Input
 *      *F = input
 *      End = pointer to MetaPtr to end of list.
 *      Token = token expected.
 *	K = pointer to entry of form being parsed 
 */
boolean InNext (F,End,Token,K,Env)
   InDesc *F;
   MetaPtr *End;
   char *Token;
   FormEntry *K;
   ListPtr Env;
   {
      NewList (*End,1L);
      if (SysError || !InComp (F,&(**End)->Val,Env)) return 0;
      if (!IsTok (F,Token)) {
	 char Error [80];
	 extern char *sprintf();
	 (void) sprintf (Error,"'%s' part of '%s' expected",
			 Token,K->FormComment);
	 return InError (F,Error);
      }
      *End = &(**End)->Next;
      return 1;
   }

/*
 * InPFO
 *
 * Input a PFO.
 *
 * Input
 * 	F = input descriptor pointing to 1st token after 1st keyword of form
 *      K = index of form
 *	Env = environment list
 *
 * Output
 *	InOut = form
 */
private boolean InPFO (F,InOut,K,Env)
   register InDesc *F;
   ObjectPtr InOut;
   FormEntry *K;
   ListPtr Env;
   {
      ListPtr R = NIL;
      MetaPtr A = &R;
      boolean Correct;

      switch (K-FormTable) {
	 case NODE_If:
	    Correct = 0;
	    if (InNext (F,&A,"THEN",K,Env) && InNext (F,&A,"\0",K,Env))
	       if (IsTok (F,"ELSIF")) {
		  NewList (A,1L);
		  Correct = !SysError && InPFO (F,&(*A)->Val,K,Env);
	       } else
		  if (IsTok (F,"ELSE")) Correct = InNext (F,&A,"END",K,Env);
		  else (void) InError (F,"'ELSE' or 'ELSIF' expected");
	    break;

	 case NODE_Each:
	 case NODE_RInsert:
	 case NODE_Filter:
	    Correct = InNext (F,&A,"END",K,NIL);
	    break;

	 case NODE_While:
	    Correct = InNext (F,&A,"DO",K,NIL) && InNext (F,&A,"END",K,NIL);
	    break;
#if XDEF
	 case NODE_XDef: {
	    ListPtr OldEnv = Env;
	    Correct = 0;
	    NewList (A,1L);
	    if (SysError || !InLHS (F,&(*A)->Val,&Env)) break;
	    if (!IsTok (F,":=")) (void) InError (F,"':=' expected");
	    else {
	       A = &(*A)->Next;
	       if (!InNext (F,&A,"}",K,OldEnv)) break;
	       NewList (A,1L);
	       if (InSimple (F,&(*A)->Val,Env)) Correct = 1;
	    }
	    break;
	 }
#endif
	 case NODE_C:
	    NewList (A,1L);
	    if (Correct = !SysError && InObject (F,&(*A)->Val))
	       if ((*A)->Val.Tag == BOTTOM) {
		  /* Convert #? to #(null) */
		  DelLPtr (R);
		  R = NIL;
	      }
	    break;

	 case NODE_Cons:
	    if (!(Correct = IsTok (F,"]"))) {
	       while ((Correct = InNext (F,&A,"\0",K,Env)) && IsTok (F,",")) 
		  continue;
	       if (Correct) 
		  if (Correct = IsTok (F,"]"));
		  else (void) InError (F,"']' or ',' expected");
	    }
	    break;

#if FETCH
	 case NODE_Fetch:
#endif
	 case NODE_Out:
	    NewList (A,1L);
	    Correct = !SysError && InObject (F,&(*A)->Val);
	    break;

      }
      return MakeForm (Correct,K->FormNode,R,InOut);
   }

/*
 * InSelector
 *
 * Input
 * 	F = input descriptor pointing to selector
 *
 * Output
 *	InOut = selector PFO
 */
private boolean InSelector (F,InOut)
   register InDesc *F;
   ObjectPtr InOut;
   {
      register ListPtr P;
      long Index = 0;

      do 
	 Index = 10*Index + (*F->InPtr++) - '0';
      while isdigit (*F->InPtr);

      RepTag (InOut,LIST);
      InOut->List = NIL;
      NewList (&InOut->List,2L);
      if (SysError) {
	 InOut->Tag = BOTTOM;
	 return 0;
      }
      P = InOut->List;
      P->Val.Tag = NODE;
      P->Val.Node = FormTable [NODE_Sel].FormNode;
      P = P->Next;
      P->Val.Tag = INT;
      P->Val.Int = IsTok (F,"r") ? -Index : Index;
      return 1;
   }

/*
 * InSimple
 *
 * Read a simple function
 *
 * Output
 *      result = 1 iff error occurs, 0 otherwise
 *      InOut = simple function if no error
 *
 * A SysError may occur, in which case InOut is unchanged.
 */
boolean InSimple (F,InOut,Env)
   InDesc *F;
   ObjectPtr InOut;
   ListPtr Env;
   {
      static char InFirst[] = {	 /* First characters of InPrefix */
	  'I','E','W','#','[','F','@'
#if FETCH
	 ,'^'
#endif
#if XDEF
	 ,'{'
#endif
	 ,'\0'
      };
      register FormEntry *K;
      extern char *index ();

      if (Debug & DebugParse) {
	 printf ("InSimple: Env = "); OutList (Env); 
	 printf (", F = %s\n",F->InPtr);
      } 
      InBlanks (F);
#ifdef PATTERN
      if (IsTok (F,"!")) return InObject (F,InOut);
#endif
      /* 
       * The "index" lookup below quickly rejects strings which
       * cannot be key words.
       */
      if (NULL != index (InFirst,*F->InPtr)) {
	 for (K=FormTable; K < ArrayEnd(FormTable); K++) 
	    if (*K->FormInPrefix != '\0' && IsTok (F,K->FormInPrefix))
	       return InPFO (F,InOut,K,Env);
      } else
	 if (isdigit (*F->InPtr)) 
	    return InSelector (F,InOut);

      if (!InNode (F,InOut,Env)) 
	 return 0;
      else if (InOut->List == NULL) 
	 return InError (F,"'/' not a function");
      else
	 return 1;
   }

/*
 * InComp
 *
 * Input a composition
 */
boolean InComp (F,InOut,Env)
   register InDesc *F;
   ObjectPtr InOut;
   ListPtr Env;
   {
      Object X;

      if (Debug & DebugParse) {
	 printf ("InComp: Env = "); 
	 OutList (Env); 
	 printf (", F = %s\n",F->InPtr);
      }
      X.Tag = BOTTOM;
      if (!InSimple (F,&X,Env)) return 0;
      else {
	 InBlanks (F);
	 if (!IsTok (F,"|")) {
	    RepObject (InOut,&X);
	    RepTag (&X,BOTTOM);
	    return !SysError;
	 } else {
	    ListPtr P,R=NIL; 
	    boolean Correct;
	    NewList (&R,1L);
	    if (SysError) Correct = 0;
	    else {
	       CopyObject (&(P=R)->Val,&X);
	       RepTag (&X,BOTTOM);
	       do {
		  NewList (&P->Next,1L);
		  Correct = !SysError && InSimple (F,&(P=P->Next)->Val,NIL);
		  InBlanks (F);
	       } while (Correct && IsTok (F,"|"));
	    }
	    return MakeForm (Correct,FormTable[NODE_Comp].FormNode,R,InOut);
	 }
      }
   }

/*
 * InDef
 *
 * Input a function definition
 *
 * Input
 *      FunName = Name of function
 * Output
 *      InOut = function definition
 *      result = 1 iff successful, 0 otherwise
 */
boolean InDef (F,FunName,InOut)
   register InDesc *F;
   StrPtr FunName;
   ObjectPtr InOut;
   {
      Object Fun,S;

      Fun.Tag = BOTTOM;
      S.Tag = BOTTOM;
      F->InDefFun = FunName;

      InBlanks (F);
      if (!IsTok (F,"DEF")) return InError (F,"DEF expected");
      else {
	 InBlanks (F);
	 (void) InString (F,&S,NodeDelim,0);
	 if (StrComp (S.String,FunName))
	    (void) InError (F,"Definition name wrong");
	 else {
	    InBlanks (F);
	    if (!IsTok (F,"AS")) (void) InError (F,"AS expected");
	    else {
	       InBlanks (F);
	       if (InComp (F,&Fun,NIL)) {
		  InBlanks (F);
		  if (!IsTok (F,";")) (void) InError (F,"semicolon expected");
		  else {
		     InBlanks (F);
		     if (*F->InPtr) (void) InError (F,"end of file expected");
		     else {
			RepTag (&S,BOTTOM);
			CopyObject (InOut,&Fun);
			RepTag (&Fun,BOTTOM);
			return 1;
		     }
		  }
	       }
	    }
	 }
      }
      RepTag (&S,BOTTOM);
      RepTag (&Fun,BOTTOM);
      return 0;
   }


/********************************** infun.c **********************************/

