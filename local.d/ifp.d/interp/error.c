
/****** error.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Sept 8, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/************************ Error Message Routines **********************/

#include <stdio.h>
#include <ctype.h>
#include "struct.h"
#include "node.h"
#include "umax.h"
#include "inob.h"

/* Some common error messages */

char ArgNotSeq[] = "not a sequence",
     ArgSeqOb [] = "must be <sequence object>",
     ArgObSeq [] = "must be <object sequence>",
     ArgNull  [] = "empty sequence",
     ArgBottom[] = "argument is ?";         

/*
 * PrintErr
 *
 * Check if error message should be printed.  Error messages are not printed if
 * the argument is BOTTOM (in which case the error has already been noted), or
 * SysStop is set (the user has interrupted execution).
 *
 * Input
 *      InOut = argument to function                  
 *
 * Output
 *      result = true iff error message should be printed
 */
boolean PrintErr (InOut)
   ObjectPtr InOut;
   {
      return InOut->Tag != BOTTOM && !SysStop;
   }

/*
 * FormError
 *
 * Print PFO error message.
 *
 * Input
 *	InOut = input to form
 *	Message = error message
 *	N = offended form's index in FormTable
 *	P = form parameter list
 */
void FormError (InOut,Message,N,P)
   ObjectPtr InOut;              
   char *Message;
   int N;
   ListPtr P;
   {
      extern int TraceDepth;

      if (PrintErr (InOut)) {
         LineWait ();
	 OutForm (FormTable[N].FormNode,P,TraceDepth);
	 printf (": %s\n",Message);
	 OutObject (InOut);
	 printf ("\n");
         LineSignal ();
      }
      RepTag (InOut,BOTTOM);
   }

/*
 * FunError
 *
 * Print primitive function error.
 *
 * Input
 *      Message = error message
 *      InOut = offending object
 *      ApplyFun {global} = offended function
 */
void FunError (Message,InOut)
   char *Message;
   ObjectPtr InOut;
   {
      if (PrintErr (InOut)) {
         LineWait ();
	 printf ("\n");
	 OutNode (ApplyFun);
	 printf (": %s\n",Message);
	 OutObject (InOut);
	 printf ("\n");
         LineSignal ();
      }
      RepTag (InOut,BOTTOM);
   }

/*
 * DefError
 *
 * Print definition error display.
 * 
 * Input
 *      Caller = calling node
 *      F = name of erroneous function
 *      Message = error message to print 
 */
void DefError (Caller,F,Message)
   NodePtr Caller;
   ObjectPtr F;
   char *Message;
   {
      LineWait ();
      OutObject (F);
      if (Caller != NULL) {
	 printf (" (from ");
	 OutNode (Caller);
	 printf (")");
      }
      printf (": %s\n",Message);
      LineSignal ();
   }

/*
 * IntError
 *
 * Print internal error message.
 *
 * Input
 *	Message = error message
 */
void IntError (Message)
   char *Message;
   {
      fprintf (stderr,"\nINTERNAL ERROR (%s)\n",Message);
      if (Debug) abort (); 
      SysError = INTERNAL;
   }

/*
 * InError
 *
 * Print input error message.
 *
 * Input
 *	F = input descriptor
 *	Message = error message 
 *	
 * Output
 *	result = 0
 */
int InError (F,Message)
   InDesc *F;
   char *Message;
   {
      char *S;

      if (F->ComLevel > 0) Message = "open comment";
      printf ("Input error");
      if (F->InLineNum >= 0) {
	 printf (" in ");
	 OutNode (F->InDefMod); 
	 printf ("/");
	 if (F->InDefFun != NULL) OutString (F->InDefFun);
	 else printf ("%IMPORT");
	 printf (" on line %d:\n%s",F->InLineNum,F->InBuf);
         if (F->InBuf[strlen (F->InBuf)-1] != '\n') printf ("\n");
      } else printf ("\n");
      for (S=F->InBuf; S<F->InPtr; S++) 
	 printf ("%c", isspace (*S) ? *S : ' ');
      printf ("^\n%s\n",Message);
      return F->ComLevel = 0;
   }  

/****************************** end of error.c *******************************/

