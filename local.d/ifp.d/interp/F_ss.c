
/****** F_ss.c ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  July 4, 1985          **/
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

/*************************** Searching and Sorting ***************************/

/*
 * F_Assoc
 *
 * Just like LISP assoc, except that #f is returned if the key is not found.
 *
 * [association-list,key] | assoc == element of association list or #f
 */
private F_Assoc (InOut)
   ObjectPtr InOut;
   {
      register ListPtr P;
      register ObjectPtr Key;

      if (!PairTest (InOut,1<<LIST,~0))
	  FunError (ArgSeqOb,InOut);

      else {

	 P = InOut->List;
	 Key = &P->Next->Val;

	 for (P = P->Val.List; P != NULL; P=P->Next)
	    if (P->Val.Tag != LIST) {
	       FunError ("element not sequence",InOut);
	       return;
	    } else
	       if (ObEqual (&P->Val.List->Val,Key)) {
		  RepObject (InOut,&P->Val);
		  return;
	       }

	 RepBool (InOut,0);     /* key not found, return #f */
      }
   }


void D_ss ()
   {
      (void) PrimDef (F_Assoc,"assoc",SysNode);
   }

/******************************* end of F_ss.c *******************************/

