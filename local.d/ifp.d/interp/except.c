
/****** except.c ******************************************************/
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

/************************* Exception Handlers *************************/

#include <stdio.h>
#include "struct.h"
#include "umax.h"

#if OPSYS!=CTSS
#include <signal.h>
#endif

/*
 * There are currently two exceptions which must be dealt with.
 * 
 *	1.  Interpreter (system) errors, e.g. out of memory
 *	    These are indicated by setting the variable 'SysError' to the
 *	    appropriate non-zero value.  The values are listed in struct.h
 *
 *	2.  User interrupts, i.e. ctrl-C.
 *          These are counted by the variable SysStop.
 *
 *		0 = process normally
 *		1 = stop processing and print back trace
 *		2 = return to top level without printing back trace
 */
short SysError = 0;     /* An error occurred if SysError != 0 */
short SysStop = 0;

#if OPSYS!=CTSS
private int SetStop ()
   { 
      SysStop++; 
      (void) signal (SIGINT,SetStop);
   }
#endif OPSYS!=CTSS

/*
 * ResetExcept
 *
 * Reset exception handling to normal state.
 */
void ResetExcept ()
   {
      extern int UDump();
      SysError = 0;
      SysStop = 0;
#if OPSYS!=CTSS
      (void) signal (SIGINT,SetStop);
#endif
#if OPSYS==DOS
      SetCBrk ();
#endif
   }

