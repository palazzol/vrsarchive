
/****** apply.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date: July 29, 1986          **/
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
#include "stats.h"
#include "cache.h"

/*
 * ApplyCheck
 *
 * Check if a function definition is internally consistent
 */
boolean ApplyCheck (F)
   ObjectPtr F;
   {
      register ListPtr P;
      ObjectPtr D;

      switch (F->Tag) {

	 case LIST:

	    if ((P = F->List) == NULL) return 0;
	    else {

	       switch (P->Val.Tag) {

		  case NODE: return 1;

		  case LIST:   /* unlinked form */
		     LinkPath (&P->Val,DEF);
		     if (P->Val.Tag==NODE && P->Val.Node->NodeType==DEF) {
			D = &P->Val.Node->NodeData.NodeDef.DefCode;
			if (D->Code.CodeParam >= 0 &&
			    D->Code.CodeParam != ListLength (P->Next)) {
			   DefError ((NodePtr) NULL,F,
				     "wrong number of parameters");
			   return 0;
			}
		     } else {
			DefError ((NodePtr) NULL,F,"not a PFO");
			return 0;
		     }
		     if (P->Val.Node == FormTable[NODE_Sel].FormNode)
			return P->Next->Val.Tag == INT;
		     else if (P->Val.Node == FormTable[NODE_C].FormNode)
			return (P=P->Next) == NULL || P->Next == NULL;
		     else if (P->Val.Node == FormTable[NODE_Out].FormNode)
			return 1;
#if FETCH
		     else if (P->Val.Node == FormTable[NODE_Fetch].FormNode)
			return 1;
#endif
		     else {
			while ((P=P->Next) != NULL)
			   if (!ApplyCheck (&P->Val)) return 0;
			return 1;
		     }
	       
		     case STRING: /* unlinked function */
			LinkPath (F,DEF);
			if (F->Tag != NODE || F->Node->NodeType != DEF) {
			   DefError ((NodePtr) NULL,F,"not a definition");
			   return 0;
			} else return 1;

		     default:
			IntError ("ApplyCheck: illegal P->Val.Tag value");
			return 0;
		  }
	    }

	 case NODE: return 1;   /* Linked function */
#if XDEF
	 /* We should check that the string is a functional variable */
	 case STRING: return 1;
#endif
	 default:
	    DefError ((NodePtr) NULL,F,"Invalid function/form definition");
	    return 0;
      }
   }

/*----------------------------------------------------------------------*/

extern int TraceIndent;	/* Indentation level of trace  */
boolean Trace = 0;	/* Print function trace if set */
#define ENTER "ENTER> "
#define EXIT  "EXIT>  "

/*
 * ApplyFun points to node whenever a compiled function is being applied.
 * It is undefined at all other times.
 * It is undefined when running multithread.
 */ 
NodePtr ApplyFun;

/*
 * Apply
 *
 * Apply function *F to argument *InOut.  Put result in *InOut.
 * *F is linked if it was unlinked.
 *
 * There are five possible representations for the function:
 *
 *      <string ...>           Unlinked function
 *      node                   Linked function
 *      <<string ...> ...>     Unlinked PFO
 *      <node ...>             Linked PFO
 *	string		       Functional variable
 *
 * Input
 *      *InOut = function argument
 *      *F = function
 *
 * Output
 *      *InOut = result of applying F to InOut
 *      *F = linked function
 *
 * Note: There is some weird casting for the linked form case.
 *       This is merely to avoid putting another pointer on the stack,
 *       which we want to avoid since that case is recursive.
 */
void Apply (InOut,F)
   ObjectPtr InOut;
   register ObjectPtr F;
   {
      extern void PrintTrace ();
      register ListPtr P;

      if (SysStop) {
	 RepTag (InOut,BOTTOM);
	 return;
      }

#if OPSYS==MSDOS
      StackCheck ();		/* Check for stack overflow or interrupt */
#endif

      Stat (StatApply (InOut));		/* Collect "apply()" statistics */

      switch (F->Tag) {

	 case LIST:

	    if ((P=F->List)->Val.Tag == NODE) {

	       if (Trace) PrintTrace (F,InOut,ENTER);	/* linked PFO */
	       TraceIndent++;
	       P = (ListPtr) P->Val.Node;
#define Fn ((NodePtr) P)->NodeData.NodeDef.DefCode
	       if (Fn.Tag == CODE)
		  (*Fn.Code.CodePtr) (InOut,F->List->Next);
#undef Fn
	       else
		  DefError ((NodePtr) NULL,&F->List->Val,
			    "No compiled def for form");
	       TraceIndent--;
	       if (Trace || InOut->Tag==BOTTOM) PrintTrace (F,InOut,EXIT);

	    } else if (P->Val.Tag == STRING) { 		/* unlinked function */

	       LinkPath (F,DEF);
	       if (F->Tag==NODE && F->Node->NodeType==DEF)
		  goto FunApply;
	       else {
		  DefError ((NodePtr) NULL,F,"not a definition");
		  RepTag (InOut,BOTTOM);
	       }
	    } else {
	       printf ("INTERNAL ERROR in Apply: illegal P->Val = ");
	       OutObject (F);
	       printf ("\n");
	    }
	    break;

FunApply:
	 case NODE: {					/* linked function */
	    int SaveTrace;

	    /* Evaluate linked function */
	    P = (ListPtr) &(ApplyFun=F->Node)->NodeData.NodeDef;
#define D ((DefPtr) P)
	    SaveTrace = Trace;
	    Trace = D->DefFlags & TRACE;
	    if (Trace|SaveTrace) PrintTrace (F,InOut,ENTER);
	    TraceIndent++;

	    if (D->DefCode.Tag != CODE) {
	       if (D->DefCode.Tag == BOTTOM) ReadDef ((NodePtr) NULL,F);
	       if (D->DefCode.Tag != BOTTOM) 
		  CheckCache (&Cache[CacheUser],Apply (InOut,&D->DefCode))
	       else {
		  DefError ((NodePtr) NULL,F,"no source definition");
		  RepTag (InOut,BOTTOM);
	       }
	    } else 
	       CheckCache (&Cache[CachePrim],
			   (*D->DefCode.Code.CodePtr) 
			   (InOut,D->DefCode.Code.CodeParam));
#undef D
	    TraceIndent--;
	    if (Trace|SaveTrace || InOut->Tag == BOTTOM)
	       PrintTrace (F,InOut,EXIT);
	    Trace = SaveTrace;

	    return;
	 }
#if XDEF
	 case STRING: {
	    extern ListPtr Environment;
	    P = Environment;

	    for (P=Environment; P!=NULL; P=P->Next->Next)
	       if (P->Val.String == F->String) {
		  RepObject (InOut,&P->Next->Val);
		  return;
	       }
	    IntError ("Apply: variable not in environment\n");
	    return;
	 }
#endif 
	 default:
	    DefError ((NodePtr) NULL,F,"Invalid function/form definition");
	    RepTag (InOut,BOTTOM);
	    return;
      }
   }


#if REFCHECK || UMAX
/*
 * RefCheck
 *
 * Check if all references required to apply function *F are defined and
 * resolved.
 *
 * *F is linked if it was unlinked.
 *
 * See function 'apply' above for the function representations
 *
 * Input
 *      Caller = &node of calling function, NULL for top level
 *      *F = function
 *
 * Output
 *      *F = linked function
 *	result = 1 iff all references resolved, 0 otherwise.
 *
 * Note: There is some weird casting for the linked form case.
 *       This is merely to avoid putting another pointer on the stack,
 *       which we want to avoid since that case is recursive.
 */
boolean RefCheck (Caller,F)
   NodePtr Caller;
   register ObjectPtr F;
   {
      register ListPtr P;

      if (SysStop) return 0;

#if OPSYS==MSDOS
      StackCheck ();
#endif

      switch (F->Tag) {

	 case LIST:
	    P = F->List;
	    if (P == NULL) {
	       IntError ("RefCheck: empty list");
	       return 0;

	    } else {

	       switch (P->Val.Tag) {

		  case LIST:   /* unlinked form */
		     LinkPath (&P->Val,DEF);
		     if (P->Val.Tag!=NODE || P->Val.Node->NodeType!=DEF) {
			DefError (Caller,&P->Val,"not a form");
			return 0;
		     } /* else drop down to case NODE */

		  case NODE: {  /* linked form */

		     register NodePtr Fn;

		     if ((Fn = P->Val.Node) == NULL) {
			IntError ("RefCheck: empty NodePtr");
			return 0;
		     } else if (Fn->NodeData.NodeDef.DefCode.Tag != CODE) {
			DefError (Caller,&F->List->Val,
				 "No compiled def for form");
			return 0;	
		     } else {
		        int OK = 1;
		        FormEntry *T;
      			for (T=FormTable; T<ArrayEnd(FormTable); T++) 
	 		   if (T->FormNode == Fn) break;
	    		switch (T-FormTable) {
			   case NODE_Comp:
			   case NODE_Cons:
			   case NODE_Each:
			   case NODE_Filter:
			   case NODE_If:
			   case NODE_RInsert:
			   case NODE_While:
			      for (P = F->List; (P=P->Next) != NULL; )
			         OK &= RefCheck (Caller,&P->Val);
		        }
		        return OK;
		     }
		  }

		  case STRING: /* unlinked function */
		     LinkPath (F,DEF);
		     if (F->Tag != NODE || F->Node->NodeType != DEF) {
			DefError (Caller,F,"Not a function");
			return 0;
		     } else break; /* down to case NODE */
	       
		  default:
		     IntError ("Apply: illegal P->Val.Tag value");
		     return 0;
	       }
	    }

	 case NODE: {
	    /* Evaluate linked function */

	    boolean OK=1;

	    P = (ListPtr) &F->Node->NodeData.NodeDef;
#define D ((DefPtr) P)

	    if (D->DefCode.Tag != CODE) {
	       if (!(D->DefFlags & RESOLVED)) {
		  D->DefFlags |= RESOLVED;
		  if (D->DefCode.Tag == BOTTOM) ReadDef (Caller,F);
		  if (D->DefCode.Tag != BOTTOM)
		     OK = RefCheck (F->Node,&D->DefCode);
		  else {
		     DefError (Caller,F,"no source definition");
		     OK = 0;
		  }
		  D->DefFlags &= ~RESOLVED;
	       }
	    }
#undef D
	    return OK;
	 }

	 default:
	    DefError (Caller,F,"Invalid function/form definition");
	    return 0;
      }
   }
#endif /* REFCHECK */

/******************************* end of apply.c *******************************/

