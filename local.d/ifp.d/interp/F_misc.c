
/****** F_misc.c ******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Nov 24, 1985          **/
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
#include <stdio.h>
#include "node.h"
#include "string.h"

/************************** miscellaneous functions *********************/

/*
 * NodeExpand
 *
 * Replace object with equivalent object not containing nodes or bottoms.
 *
 * Nodes are converted to equivalent path lists.
 * Bottoms are converted to "?".
 */
void NodeExpand (InOut)
   register ObjectPtr InOut;
   {
      register ListPtr P;
      register NodePtr N;

      switch (InOut->Tag) {

	 case LIST:
	    CopyTop (&InOut->List);
	    for (P=InOut->List; P!=NULL; P=P->Next) NodeExpand (&P->Val);
	    break;

	 case NODE:
	    N = InOut->Node;
	    RepTag (InOut,LIST);
	    InOut->List = MakePath (N);
	    break;
      }
   }

/*
 * F_Def
 *
 * Return the object representation of a function definition.
 *
 * Input
 *      *InOut = pathname list
 *
 * Output
 *      *InOut = function definition representation
 */
int F_Def (InOut)               /* imported by Compile in C_comp.c */
   register ObjectPtr InOut;
   {
      extern void ReadDef (), RepBool ();
      register DefPtr D;

      if (InOut->Tag != LIST) FunError (ArgNotSeq,InOut);
      else {
	 LinkPath (InOut,DEF);
	 if (InOut->Tag==NODE && InOut->Node->NodeType==DEF) {
	    D = &InOut->Node->NodeData.NodeDef;
	    if (D->DefCode.Tag != CODE) {
	       if (D->DefCode.Tag == BOTTOM) ReadDef ((NodePtr) NULL,InOut);
	       if (D->DefCode.Tag != BOTTOM) {
		  RepObject (InOut,&D->DefCode);
		  NodeExpand (InOut);
		  return;
	       }
	    }
	 }
      }
      RepBool (InOut,0);   /* function not defined */
   }

/*
 * F_Apply
 *
 * Apply a function to an object. 
 *
 * Input
 *     InOut = <X F> where F is a function
 *
 * Output
 *     InOut = X : F
 */
private int F_Apply (InOut)
   ObjectPtr InOut;
   {
      register ListPtr P;

      /* 
       * We don't want to use PairTest test here, since it would expand
       * the function if its a node.  This would not affect the behavior
       * at all, but would slow things down since the function must be
       * converted to its node representation anyway.
       */
      if (InOut->Tag != LIST || 2 != ListLength (InOut->List))
	 FunError ("not a pair",InOut);
      else {
	 CopyTop (&InOut->List);
	 P = InOut->List;
	 if (ApplyCheck (&P->Next->Val)) {
	    Apply (&P->Val,&P->Next->Val);
	    RepObject (InOut,&P->Val);
	 } else 
	    FunError ("invalid function",InOut);
      }
   }

void D_misc ()
   {      
      (void) PrimDef (F_Apply,"apply",SysNode);
      (void) PrimDef (F_Def,"def",SysNode);
   }

/**************************** end of F_misc ****************************/

