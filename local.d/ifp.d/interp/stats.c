
/****** stats.c *******************************************************/
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

/* Statistics collection routines */ 

#include "struct.h"
#include "stats.h"
#include <stdio.h>

#if STATS

long StatRecycle=0,StatFresh=0;
long StatArg [MAXTAG+1];
long Stat_Apply      [StatLimLen+1];
long Stat_NewList    [StatLimLen+1];
long Stat_DelLPtrIn  [StatLimLen+1];
long Stat_DelLPtrOut [StatLimLen+1];
long Stat_Construct  [StatLimLen+1];
long Stat1Simple,Stat2Simple;
long StatC = 0;

void StatConstant (InOut)
   ObjectPtr InOut;
   {
      StatC++;
   }

void StatConstruct (P)
   ListPtr P;
   {
      register int N;
      N = ListLength (P);
      if (N >= StatLimLen) N = StatLimLen;
      ++Stat_Construct[N];
   }

void StatNewList (N)
   long N;
   {
      StatFresh += N;
      if (N > StatLimLen) N = StatLimLen;
      ++Stat_NewList [N];
   }

void StatDelLPtr (P)
   register ListPtr P;
   {
      register int N;

      N = ListLength (P);
      if (N >= StatLimLen) N = StatLimLen;

      ++Stat_DelLPtrIn [N];
      for (N=0; P!=NULL; P=P->Next)
	 if (P->LRef > LRefOne || ++N >= StatLimLen) break; 
      ++Stat_DelLPtrOut [N];
   }

#define SCALAR ((1<<INT)|(1<<FLOAT)|(1<<BOOLEAN)|(1<<STRING))

void StatApply (InOut)
   ObjectPtr InOut;
   {
      ListPtr P;
      long L;
 
      StatArg [InOut->Tag] ++;
      if (InOut->Tag == LIST) {
	 L = ListLength (InOut->List);
	 if (L > StatLimLen) L = StatLimLen;
	 Stat_Apply [L] ++;
	 if (L == 2) {
	    P = InOut->List;
	    if ((1<<P->Val.Tag) & SCALAR) Stat1Simple++;
	    if ((1<<P->Next->Val.Tag) & SCALAR) Stat2Simple++;
	 }
      }
   }


/*
 * ShowDist
 */
void ShowDist (Title,Dist)
   char *Title;
   long Dist[];
   {
      int k;
      long S,Z;

      for (S=0, k=0; k<=StatLimLen; k++) S += Dist[k];
 
      printf ("   %s (total = %ld)\n      ",Title,S);
      if (S) 
	 for (k=0; k<=StatLimLen; k++) {
	    Z = 1000 * Dist[k]/S;
	    printf ("%ld.%ld%% [%s%ld]   ",Z/10,Z%10,k==StatLimLen?">=":"",k);
	    Dist[k] = 0;
	 }
      printf ("\n");
   }

/*
 * ShowStats
 */
void ShowStats ()
   {
      long Total;
      int k;

      printf ("\n"); 
      Total = StatRecycle + StatFresh;
      printf ("Memory management\n");
      printf ("   Total cells created = %ld\n",Total);
      printf ("   Percent of cells recycled = %ld\n",
	      Total ? 100*StatRecycle/Total : 0L);
      ShowDist ("New list length distribution",Stat_NewList);
      StatRecycle = StatFresh = 0;
      ShowDist ("Deleted list (total) length distribution",Stat_DelLPtrIn);
      ShowDist ("Deleted list (partial) length distribution",Stat_DelLPtrOut);
      ShowDist ("Constructor list length distribution",Stat_Construct);
      printf ("\n");

      printf ("Constant function applications = %d\n",StatC);
      StatC = 0;
      printf ("\n");

      if (Stat_Apply [2]) {
	 Stat1Simple = 100 * Stat1Simple / Stat_Apply [2];
	 Stat2Simple = 100 * Stat2Simple / Stat_Apply [2];
      }
      if (StatArg[LIST])
	 for (k=0; k<=StatLimLen; k++)
	    Stat_Apply [k] = 100 * Stat_Apply [k] / StatArg[LIST];
      Total = 0;
      for (k=0; k<=MAXTAG; k++) Total += StatArg [k];
      if (Total)
	 for (k=0; k<=MAXTAG; k++) StatArg [k] = 100 * StatArg [k] / Total;
      printf ("\n");
      printf ("Apply arguments (Total = %ld)\n",Total);
      printf ("   Boolean = %ld, Int = %ld, Float = %ld, String = %ld\n",
		  StatArg[BOOLEAN],StatArg[INT],StatArg[FLOAT],StatArg[STRING]);
      printf ("   List = %ld\n",StatArg[LIST]);
      printf ("   ");
      for (k=0; k<StatLimLen; k++) printf ("%ld [%ld], ",Stat_Apply [k],k);
      printf ("%ld [>=%d]\n",Stat_Apply [StatLimLen],StatLimLen);
      printf ("   Pair elements [scalar]<%ld,%ld>\n",Stat1Simple,Stat2Simple);
      Stat1Simple = Stat2Simple = 0;
      for (k=0; k<=StatLimLen; k++) Stat_NewList [k] = Stat_Apply[k] = 0;
      for (k=0; k<=MAXTAG; k++) StatArg[k] = 0;
   }
#endif

/**************************** end of stats.c ****************************/

