
/****** debug.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Dec 5, 1985          **/
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
#include "string.h"

#if DEBUG
int Debug = 0;     /* Print debugging statements if true */
#endif

#if DUMP
/*
 * DumpNode
 *
 * Print out node N and all its decendants.
 */
void DumpNode (N,Indent)
   register NodePtr N;
   int Indent;
   {
      extern void OutIndent ();

      OutIndent (3*Indent);
      if (N == NULL) printf ("DumpNode: N = NULL\n");
      else {
	 OutString (N->NodeName);
	 switch (N->NodeType) {
	     case NEWNODE: printf ("(new) "); break;
	     case MODULE:
		printf (" module\n");
		for (N = N->NodeData.NodeMod.FirstChild; N!=NULL; N=N->NodeSib)
		   DumpNode (N,Indent+1);
		break;
	     case DEF:
	       printf (" function");
	       if (N->NodeData.NodeDef.DefFlags & TRACE)
		  printf ("(trace) ");
	       OutObject (&N->NodeData.NodeDef.DefCode);
	       printf ("\n");
	       break;
	    case IMPORT:
	       printf (" import");
	       OutObject (&N->NodeData.NodeImp.ImpDef);
	       printf ("\n");
	       break;
	    default:
	       printf (" invalid NodeType (%x)\n",N->NodeType);
	       break;
	 }
      }
   }

#endif /* DUMP */


/*************************** end of debug.c *********************************/

