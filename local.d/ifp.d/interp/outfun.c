
/****** outfun.c ******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date: June 30, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Dec 12, 1985          **/
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

/*
 * OutLongNode - internal to OutNode
 */
void OutLongNode (N)
   register NodePtr N;
   {
      if (N->NodeParent != NULL) {
	 OutLongNode (N->NodeParent);
	 printf ("/");
	 OutString (N->NodeName);
      }
   }

/*
 * OutNode
 *
 * Output a node in UNIX path format.  
 * Abbreviate if it is in the current directory.
 */
void OutNode (N)
   register NodePtr N;
   {
      register NodePtr M;
      extern boolean LongPathFlag;

      if (N == NULL) printf ("(NULL NODE)");
      else {
	 if (!LongPathFlag && NULL != (M = FindNode (CurWorkDir,N->NodeName)) &&
			      (M->NodeType == IMPORT || M->NodeType == DEF)) 
	    OutString (N->NodeName);
	 else OutLongNode (N);
      }
   }

/*
 * OutForm
 * 
 * Print a functional form and its parameters.
 *
 * Input
 *      N = pointer to form node
 *      P = pointer to parameter list
 *      Depth = depth to print function (ellipses used at that depth)
 */
void OutForm (N,P,Depth)
   register NodePtr N;
   ListPtr P;
   int Depth;
   {
      long L;
      register FormEntry *T;

      L = ListLength (P);

      for (T=FormTable; T<ArrayEnd(FormTable); T++) 
	 if (T->FormNode == N) {
	    switch (T-FormTable) {

	       case NODE_Comp:
		  while (P!=NULL) {
		    OutFun (&P->Val,Depth);
		    if (NULL != (P=P->Next)) printf ("|");
		  }
		  break;

	       case NODE_Cons:
		  printf ("[");
		  while (P!=NULL) {
		     OutFun (&P->Val,Depth);
		     if (NULL != (P=P->Next)) printf (",");       
		  }
		  printf ("]");
		  break;

	       case NODE_RInsert:
	       case NODE_Filter:
	       case NODE_Each:
		  printf ("%s ",T->FormInPrefix); 
		  OutFun (&P->Val,Depth);
		  printf (" END");
		  break;
 
	       case NODE_If:
		  printf ("IF ");    OutFun (&P->Val,Depth);
		  printf (" THEN "); OutFun (&(P=P->Next)->Val,Depth);
		  printf (" ELSE "); OutFun (&P->Next->Val,Depth);
		  printf (" END"); 
		  break;
   
	       case NODE_C: 
		  if (!L) {
		     printf ("?");
		     break;
		  } 
		  /* else drop through */
#if FETCH
	       case NODE_Fetch:
#endif
	       case NODE_Out: 
		  printf ("%s",T->FormInPrefix); OutObject (&P->Val);
		  break;

	       case NODE_Sel:
		  if (P->Val.Int >= 0) printf ("%d",P->Val.Int);
		  else printf ("%dr",-P->Val.Int);
		  break;

	       case NODE_While:
		  printf ("WHILE "); OutFun (&P->Val,Depth);
		  printf (" DO ");   OutFun (&P->Next->Val,Depth);
		  printf (" END");
		  break;
#if XDEF
	       case NODE_XDef: {
		  extern void OutLHS ();
		  printf ("{");    OutLHS (&P->Val);
		  printf (" := "); OutFun (&P->Next->Val,Depth); 
		  printf ("} ");
		  OutFun (&P->Next->Next->Val,Depth); 
		  break;
	       }
#endif
	    }
	    return;
	 }

      printf ("(");
      OutNode (N); 
      for (; P != NULL; P=P->Next) {    
	 printf (" ");
	 OutObject (&P->Val);
      }
      printf (")");
   }


/*
 * OutFun
 *
 * Print function *F. *F may be linked if it was unlinked.
 *
 * The possible representations for the function are described
 * in the comments for "Apply" in apply.c.
 *
 * Input
 *      *F = function
 *      Depth = depth to print function, 0 = "..."
 *
 * Output
 *      *F = may be linked function
 */
void OutFun (F,Depth)
   register ObjectPtr F;
   int Depth;
   {
      register ListPtr P;

      if (SysStop > 1) return;

      if (F == NULL) printf ("(null)");          /* Internal error */
      else if (--Depth < 0) printf ("..");
      else 

	 switch (F->Tag) {

	    default:
	       printf ("(tag = %d)",F->Tag);     /* Internal error */
	       break;

	    case LIST:
	       P = F->List;
	       if (P == NULL) printf ("()");
	       else

		  switch (P->Val.Tag) {
	
		     case LIST:   /* unlinked form */
			LinkPath (&P->Val,DEF);
			if (P->Val.Tag!=NODE||P->Val.Node->NodeType!=DEF) {
			   printf ("(");
			   OutObject (&P->Val);
			   for (; P != NULL; P=P->Next) {    
			      printf (" ");
			      OutObject (&P->Val);
			   }
			   printf (")");
			   return;
			} /* else drop down to case NODE */

		     case NODE:   /* linked form */
			OutForm (P->Val.Node,P->Next,Depth);
			return;

		     case STRING:
			LinkPath (F,DEF);
			if (F->Tag == NODE) break; /* drop down to case NODE */

		     default: /* unlinked function or internal error */
			for (; P!=NULL; P=P->Next) {
			   printf ("/");
			   OutObject (&P->Val);
			}
			return;
		  }

	    case NODE:
	       OutNode (F->Node);
	       break;

	    case STRING:
	       OutString (F->String);
	       break;
	 }
   }


/******************************* end of outfun.c ******************************/

