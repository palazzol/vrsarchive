
/****** umax.h *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   Nov 4, 1986          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Jan 27, 1987          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/* 
 * Defining UMAX=1 in "struct.h" compiles the ifp interpreter for parallel
 * processing on the Multimax.
 */

#define semaphore_wait(s)
#define semaphore_signal(s)
#define rsemaphore_enter(r)
#define rsemaphore_exit(r)
#define spin_lock(s)
#define spin_unlock(s)
#define LineWait()
#define LineSignal()
#define Terminate()


/**************************** end of umax.h ****************************/
