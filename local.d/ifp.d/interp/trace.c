
/****** trace.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Sept 9, 1986          **/
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
#include "umax.h"

int TraceIndent = 0;	/* Indentation level of trace 		*/
int TraceDepth = 2;	/* Depth to which functions are printed */

/*
 * PrintTrace
 *
 * Print a trace messages "ENTER>" or "EXIT> " with their arguments.
 * Each message is preceeded by an indentation pattern.  Each '|' in
 * the pattern represents one level of indentation; each '.' in the
 * patttern represents DOTSIZE levels of indentation.  The latter
 * abbreviation keeps us from going off the deep end.
 */
#define DOTSIZE 20

void PrintTrace (F,InOut,EnterExit)
   ObjectPtr F,InOut;
   char *EnterExit;
   {
      int K;

      /*
       * A SysStop >= 2 indicates multiple user interrupts, i.e. the user
       * does not want to see trace information.
       */
      if (SysStop < 2) {
	 LineWait ();
	 for (K = TraceIndent; K>=DOTSIZE; K-=DOTSIZE) printf (".");
	 while (--K >= 0) printf (" |");
	 printf (EnterExit);
	 OutObject (InOut);
	 printf (" : ");
	 OutFun (F,TraceDepth);
	 printf ("\n");
	 LineSignal ();
      }
   }

/******************************* end of trace.c ******************************/

