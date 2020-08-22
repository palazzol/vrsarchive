
/****** stats.h *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Dec 8, 1985          **/
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
 * Defining STATS=1 causes interpreter to collect statistics. 
 * Define STATS=0 for production work since statistics collection
 * slows the interpreter.
 */
#define STATS 0
      
#if STATS

#define StatLimLen 5
#define Stat(X) X
extern long StatRecycle,StatFresh;
extern long StatArg [];
extern long Stat_Apply [];
extern long Stat_NewList [];
extern long Stat1Simple,Stat2Simple;
extern void ShowStats();
extern void StatApply(), StatConstruct(), StatConstant();
extern void StatNewList(), StatDelLPtr();
#else

#define Stat(X) 

#endif

/**************************** end of stats.h ****************************/

