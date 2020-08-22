
/****** cache.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date: July 29, 1986          **/
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
 * NOTE: Function HashOb assumes a FPfloat is either 1x or 2x the size
 * 	 of a long.
 */

#include "struct.h"
#include "cache.h"
#include <stdio.h>

#if ECACHE

CacheEntry ECache[CACHE_SIZE];

CacheRec Cache [4] = {
   {0,0,0,0,"Prim"},
   {0,0,0,0,"User"},
   {0,0,0,0,"PFO"},
   {0,0,0,0,"Total"},
};

#define ArraySize(A) (sizeof(A)/sizeof(A[0]))

/*
 * Print the cache statistics on stdout and clear the cache statistics tallies.
 */
void ShowCache ()
   {
      CacheRec *C,*T= &Cache[CacheTotal];
      CacheEntry *E;
      int Tally=0;
      
      for (E=ECache; E < ArrayEnd(ECache); E++) 
	 if (E->EC_Fun != NULL) Tally++;

      printf ("%d/%d = %g full cache\n", 
	      Tally, ArraySize (ECache), (double) Tally / ArraySize (ECache));

      T->Enable = 0;
      for (C= &Cache[0]; C<&Cache[4]; C++) {
	 if (C->Enable) {
	    Cache[CacheTotal].Enable=1;
	    printf ("%s:\t%d hits in %d looks = %g%% hit rate [%d evictions]\n",
		    C->Name,C->Hits,C->Looks,
		    100.0 * C->Hits / (C->Looks ? C->Looks : 1), C->Evictions);
	    T->Hits  += C->Hits; 
	    T->Looks += C->Looks; 
	    T->Evictions += C->Evictions; 
	    C->Hits = C->Looks = C->Evictions = 0;
	 }
      }
      if (!T->Enable) printf ("The cache is disabled\n");
   }

#if DEBUG
void PrintCache (Message,E)
   char *Message;
   CacheEntry *E;
   {
      printf ("ECache %s ",Message);	OutObject (&E->EC_In);
      printf (" : ");			OutNode   (E->EC_Fun);
      printf (" -> ");			OutObject (&E->EC_Out);
      printf ("\n");
   }
#endif /* DEBUG */

/*
 * HashOb
 *
 * HashOb computes an integer function (hash code) of an object.
 *
 * Input
 *	X = object
 * Output
 * 	result = hash code
 */
int HashOb (X)
   ObjectPtr X;
   {
      register long H;
      register ListPtr P;

      switch (X->Tag) {
	  case BOTTOM:  H = 2305;                       break;
	  case BOOLEAN: H = X->Bool;                    break;
	  case INT:     H = X->Int * 9;                 break;
	  case FLOAT:
	     if (sizeof (FPfloat) == 2*sizeof (long))
		H = ((long *)&(X->Float))[0] + ((long *)&(X->Float))[1];
	     else if (sizeof (FPfloat) == sizeof (long))
		H = ((long *)&(X->Float))[0];
	     else
		fprintf (stderr,"HashOb: can't hash floats on this machine!\n");
	     break;
	  case STRING:  H = (long) X->String;           break;
	  case LIST:
	     H = 5298;
	     for (P=X->List; P!=NULL; P=P->Next)
		H = H * 0x1243 + HashOb (&P->Val);
	     break;
	  case NODE:    H = (long) X->Node * 5;         break;
	  case CODE:    H = (long) X->Code.CodePtr +
			    (long) X->Code.CodeParam;   break;
	  default:
	     fprintf (stderr,"HashOb: invalid tag (%d)\n",X->Tag);
	     break;
      }
      return H;
   }

ClearCache ()			/* Clear all entries from the cache. */
   {
      CacheEntry *C;

      for (C=ECache+CACHE_SIZE; --C >= ECache; ) {
	 RepTag (&C->EC_In, BOTTOM);
	 C->EC_Fun = NULL;
	 RepTag (&C->EC_Out,BOTTOM);
      }
   }

InitCache ()			/* Initialize the cache */
   {
      register CacheEntry *E;
      CacheRec *C;

      printf (" (cache");
      for (C=Cache; C<&Cache[3]; C++)
	 if (C->Enable) printf (" %s",C->Name);
      printf (")");

      for (E=ECache+CACHE_SIZE; --E >= ECache; ) {
	 E->EC_In. Tag = BOTTOM;
	 E->EC_Fun = NULL;
	 E->EC_Out.Tag = BOTTOM;
      }
   }

#endif /* ECACHE */

