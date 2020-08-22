
/****** forms.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date: July 28, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

#include "struct.h"
#include "node.h"
#include "umax.h"
#include "stats.h"
#include <stdio.h>

/*
 * FF_Each
 *
 * Apply function F to each element of list InOut
 *
 * Input
 *      InOut = list of elements to apply function
 *      Funs = singleton list of function to be applied
 *
 * Output
 *      InOut = result
 */
private FF_Each (InOut,Funs)
   ObjectPtr InOut;
   register ListPtr Funs;
   {
      register ListPtr P;

      switch (InOut->Tag) {
	 default:
	    FormError (InOut,ArgNotSeq,NODE_Each,Funs);
	    return;
	 case LIST:
	    CopyTop (&InOut->List);
	    break;
      }
      for (P = InOut->List; P!=NULL; P=P->Next) {
	 Apply (&P->Val,&Funs->Val);
	 if (P->Val.Tag == BOTTOM) {
	    RepTag (InOut,BOTTOM);      /* Error already reported */
	    return;
	 }
      }
   }


/*
 * FF_Filter
 *
 * Input
 *      InOut = list of elements to apply predicate
 *      Funs = singleton list of function to be applied
 *
 * Output
 *      InOut = result - list of element for which predicate is true.
 */
private FF_Filter (InOut,Funs)
   register ObjectPtr InOut;
   register ListPtr Funs;
   {
      register ListPtr P;
      register MetaPtr E;
      ListPtr Result;
      Object X;

      if (InOut->Tag != LIST)
	 FormError (InOut,ArgNotSeq,NODE_Filter,Funs);
      else {
	 Result = NULL;
	 E = &Result;
	 for (P = InOut->List; P!=NULL; P=P->Next) {
	    CopyObject (&X,&P->Val);
	    Apply (&X,&Funs->Val);
	    if (X.Tag != BOOLEAN) {
	       FormError (&X,"non-boolean predicate",NODE_Filter,Funs);
	       RepTag (InOut,BOTTOM);
	       DelLPtr (Result);
	       return;
	    } else if (X.Bool) {   /* append element to result list */
	       NewList (E,1L);
	       CopyObject (&(*E)->Val,&P->Val);
	       E = &(*E)->Next;
	    }
	 }
	 DelLPtr (InOut->List);
	 InOut->List = Result;
      }
   }


/*
 * FF_Compose
 *
 * Function composition
 *
 * Input
 *      InOut = object to apply composition
 *      Funs = list of functions to compose in reverse order
 *
 * Output
 *      InOut = result of composition
 */
private FF_Compose (InOut,Funs)
   register ObjectPtr InOut;
   register ListPtr Funs;
   {
      for (; Funs != NULL && InOut->Tag != BOTTOM; Funs = Funs->Next)
	 Apply (InOut,&Funs->Val);
   }

/*
 * FF_RInsert
 *
 * Function right insert
 */
private FF_RInsert (InOut,Funs)
   register ObjectPtr InOut;
   register ListPtr Funs;
   {
      ListPtr Terms;

      if (InOut->Tag != LIST)
	 FormError (InOut,ArgNotSeq,NODE_RInsert,Funs);
      else if (InOut->List == NULL) 
	 FormError (InOut,"empty sequence",NODE_RInsert,Funs);
      else {
	 F_Reverse (InOut); /* Copy top and reverse */
	 Terms = InOut->List->Next;
	 InOut->List->Next = NULL;
	 RepObject (InOut,&InOut->List->Val);
	 while (Terms != NULL) {
	    /* form pair and apply function */
	    NewList (&Terms->Next,1L);
	    Terms->Next->Val.Tag = InOut->Tag;
	    Terms->Next->Val.Data = InOut->Data;
	    InOut->Tag = LIST;
	    InOut->List = NULL;
	    Rot3 (&InOut->List,&Terms,&Terms->Next->Next);
	    Apply (InOut,&Funs->Val);
	    if (InOut->Tag == BOTTOM) {
	       DelLPtr (Terms);
	       break;
	    }
	 }
      }
   }

/*
 * FF_C
 *
 * Constant function
 */
private FF_C (InOut,Funs)
   ObjectPtr InOut;
   register ListPtr Funs;
   {
      Stat (StatConstant (InOut));
      if (Funs == NULL) 
	 FormError (InOut,"(constant bottom)",NODE_C,Funs);
      else 
	 RepObject (InOut,&Funs->Val);
   }

/*
 * FF_Out
 *
 * Print debugging message
 */
private FF_Out (InOut,Funs)
   ObjectPtr InOut;
   register ListPtr Funs;
   {
      LineWait ();
      OutObject (&Funs->Val),
      printf (": "),
      OutObject (InOut),
      printf ("\n");
      LineSignal ();
   }


#if FETCH 
/*
 * FF_Fetch
 *
 * Fetch form
 */
private FF_Fetch (InOut,Funs)
   ObjectPtr InOut;
   register ListPtr Funs;
   {
      register ListPtr P,Q,R;

      if (InOut->Tag != LIST)
	 FormError (InOut,ArgNotSeq,NODE_Fetch,Funs);
      else {
	 R = NULL;

	 for (P = InOut->List; P != NULL; P=P->Next)
	    if (P->Val.Tag != LIST || (Q=P->Val.List) == NULL ||
		Q->Next == NULL || Q->Next->Next != NULL) {
	       FormError (InOut,"element not a pair",NODE_Fetch,Funs);
	       return;
	    } else
	       if (R == NULL && ObEqual (&Q->Val,&Funs->Val)) R = Q;

	 if (R!=NULL) RepObject (InOut,&R->Next->Val);
	 else FormError (InOut,"key not found",NODE_Fetch,Funs);
	 return;

      }
   }
#endif FETCH


/*
 * FF_If
 *
 * Conditional p->f;g
 *
 * Input
 *      InOut = object to apply conditional
 *      Funs = <p f g>
 *
 * Output
 *      InOut = result of conditional
 */
private FF_If (InOut,Funs)
   ObjectPtr InOut;
   ListPtr Funs;
   {
      Object P;

      CopyObject (&P,InOut);
      Apply (&P,&Funs->Val);
      if (P.Tag == BOOLEAN) 
	 Apply (InOut, & (P.Bool ? Funs : Funs->Next)->Next->Val);
      else {
	 FormError (&P,"non-boolean predicate",NODE_If,Funs);
	 RepTag (InOut,BOTTOM);
      } 
   }

/*
 * FF_Construct
 *
 * Function construction
 *
 * Input
 *      InOut = object to apply construction
 *      Funs = list of functions to construct
 *
 * Output
 *      InOut = result
 */
private FF_Construct (InOut,Funs)
   register ObjectPtr InOut;
   ListPtr Funs;
   {
      register ListPtr P,F;
      Stat (StatConstruct (Funs));
      P = Repeat (InOut, ListLength (F = Funs));
      if (SysError) return;
      RepTag (InOut,LIST);
      for (InOut->List = P; F != NULL; P=P->Next,F=F->Next) {
	 Apply (& P->Val,& F->Val);
	 if (P->Val.Tag == BOTTOM) {
	    RepTag (InOut,BOTTOM);     /* Error was already reported */
	    return;
         }
      }
   }


/*
 * FF_Select
 *
 * Selector form (e.g. 1,2r)
 *
 * Input
 *      InOut = object
 *      Funs = index parameter list - positive values are left selectors
 *                                    negative values are right selectors
 */
private FF_Select (InOut,Funs)
   ObjectPtr InOut;
   ListPtr Funs;
   {
      register ListPtr P;
      register long N;
      char *E;

      N = Funs->Val.Int; 
      switch (InOut->Tag) {
	 default:
	    E = ArgNotSeq;
	    break;
	 case NODE:
	    NodeExpand (InOut);

	 case LIST:
	    P = InOut->List;
	    if (N < 0) N += ListLength (P) + 1;
	    if (--N >= 0) {
	       for (; P!=NULL; P=P->Next)
		  if (--N < 0) {
		     RepObject (InOut,&P->Val);
		     return;
		  }
	       E = "index off right end";
	    } else
	       E = "index off left end";
	    break;
      }
      FormError (InOut,E,NODE_Sel,Funs);
   }


/*
 * FF_While
 *
 * While P is true, apply F to X
 *
 * Input
 *      InOut = X
 *      Funs = pair <P F>
 *
 * Output
 *      InOut = result
 */
private FF_While (InOut,Funs)
   register ObjectPtr InOut;
   register ListPtr Funs;
   {
      Object P;

      P.Tag = BOTTOM;
      while (InOut->Tag!=BOTTOM) {
	 CopyObject (&P,InOut);       /* old P was element of {?,f,t} */
	 Apply (&P,&Funs->Val);
	 if (P.Tag != BOOLEAN) {
	    FormError (&P,"non-boolean predicate",NODE_While,Funs);
	    RepTag (InOut,BOTTOM);
	 } else
	    if (P.Bool) Apply (InOut,&Funs->Next->Val);
	    else break;
      }
   }


#if XDEF
extern FF_XDef();
#endif

/*
 * FormTable
 *
 * These entries must be ordered to correspond with the #defines in "node.h".
 */
FormEntry FormTable[] = {
   {NULL, "#",      {"constant" ,-1,FF_C        }, "#c"},
   {NULL, "",       {"compose"  ,-1,FF_Compose  }, ""},
   {NULL, "[",      {"construct",-1,FF_Construct}, "[...]"},
   {NULL, "EACH",   {"each"     , 1,FF_Each     }, "EACH g END"},
#if FETCH
   {NULL, "^",      {"fetch"    , 1,FF_Fetch    }, "^c"},
#endif
   {NULL, "FILTER", {"filter"   , 1,FF_Filter   }, "FILTER p END"},
   {NULL, "IF",     {"if"       , 3,FF_If       }, "IF p THEN g ELSE h END"},
   {NULL, "INSERT", {"insertr"  , 1,FF_RInsert  }, "INSERT g END"},
   {NULL, "@",      {"out"      , 1,FF_Out      }, "@message"},
   {NULL, "",       {"select"   , 1,FF_Select   }, "digit"},
   {NULL, "WHILE",  {"while"    , 2,FF_While    }, "WHILE p DO g END"}
#if XDEF
  ,{NULL, "{",      {"xdef"     , 3,FF_XDef     }, "{...}"},
#endif
};

void D_form ()
   {
      FormEntry *N;

      for (N=FormTable; N<ArrayEnd (FormTable); N++) 
	 N->FormNode = PrimDef (N->FormOp.OpPtr,
				N->FormOp.OpName,
				SysNode,
				N->FormOp.OpParam);
   }

/******************************* end of forms.c *******************************/

