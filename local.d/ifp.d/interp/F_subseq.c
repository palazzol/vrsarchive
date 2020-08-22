
/****** F_subseq.c ****************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Apr 28, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

#include <stdio.h>    /* defines NULL */
#include "struct.h"
#include "node.h"

/*
 * ListIndex
 *
 * Check an argument to make sure it is of the form <sequence integer>
 *
 * Input
 *      InOut = argument
 *
 * Output
 *      *L = sequence or array if no error
 *      result = -1 if error occurred, index otherwise
 */
private long ListIndex (InOut,L)
   ObjectPtr InOut;
   ListPtr *L;
   {
      register ListPtr P;
      FPint N;
 
      if (!PairTest (InOut, SEQUENCE, NUMERIC)) {
	 FunError ("not <sequence number>",InOut);
	 return -1;
      } else {
	 P = InOut->List;
	 *L = P->Val.List;
	 P = P->Next;
	 switch (GetFPInt (&P->Val,&N)) {
	    default: /* actually case 0, but we need to keep lint happy */
	       if (N >= 0) return N;
	       else {
		  FunError ("negative index",InOut);
		  return -1;
	       }
	    case 1: 
	       FunError ("index not integral",InOut); 
	       return -1;
	    case 2:
	       FunError ("index too big",InOut);
	       return -1;
	 }
      }
   }

#define SCATTER_STORE 0

#if SCATTER_STORE
/*
 * F_Scatter
 *
 * Scatter store function
 *
 * Input
 *      <<D1 D2 ... Dn> <<V1 I1> <V2 I2> ... <Vm Im>>>
 *
 * Output
 *      <E1 E2 ... En>
 *
 * Ek = Dk if there is no Ij == k
 *      Vj if Ij == k
 *
 * Result is BOTTOM if Ij==Ik for j!=k or Ij < 1 or Ij > n
 *
 * Perversions: uses LRef field for markers
 */
private F_Scatter (InOut)
   ObjectPtr InOut;
   {
      register ListPtr P1,P2,Q,R;
      register long N;
      FPint M;

      if (!PairTest (InOut,1<<LIST,1<<LIST))
	 FunError ("not <sequence sequence>",InOut);

      else {

	 Copy2Top (&InOut->List); /* only need fresh first element */
	 P1 = InOut->List;
	 R = P1->Val.List;
	 N = ListLength (R);

	 for (P1 = P1->Next->Val.List; P1!=NULL; P1=P1->Next) {
	    if (!PairTest (&P1->Val,~0,NUMERIC)) {
	       FunError ("invalid store pair",InOut);
	       return;
	    }
	    P2 = P1->Val.List;
	    if (GetFPInt (&P2->Next->Val,&M) || M < 1 || M > N) {
	       FunError ("invalid index",InOut);
	       return;
	    }
	    for (Q=R; --M; Q=Q->Next) continue;
	    if (++Q->LRef > 2) {
	       for (Q=R; Q!=NULL; Q=Q->Next) Q->LRef = 1;
	       FunError ("duplicate index",InOut);
	       return;
	    }
	    RepObject (&Q->Val,&P2->Val);
	 }
	 for (Q=R; Q!=NULL; Q=Q->Next) Q->LRef = 1;
	 RepObject (InOut,&InOut->List->Val);
      }
   }
#endif

/*
 * F_Pick
 * 
 * Pick the nth element of a sequence
 *
 * Input
 *      InOut = pointer to <sequence number>
 */
private F_Pick (InOut)
   ObjectPtr InOut;
   {
      register FPint N;
      ListPtr P; 

      if ((N = ListIndex (InOut,&P)) >= 0) {
	 if (N <= 0) {
	    FunError ("non-positive index",InOut);
	 } else if (P == NULL) FunError ("empty sequence",InOut);
	 else {
	    while (--N > 0)
	       if ((P = P->Next) == NULL) {
		  FunError ("index out of bounds",InOut);
		  return;
	       }
	    RepObject (InOut,&P->Val);
	 }
      }
   }


/*
 * F_Repeat
 *
 * Create a repetition of an item.
 *
 * E.g. <x 8> == <x x x x x x x x>
 */
private F_Repeat (InOut)
   register ObjectPtr InOut;
   {
      FPint N;
      register ListPtr P;

      if (!PairTest (InOut,~0,NUMERIC))
	 FunError ("not <object number>",InOut);

      else {
	 P = InOut->List;
	 switch (GetFPInt (&P->Next->Val,&N)) {
	    case 1:
	       FunError ("repetition value not integer",InOut);
	       break;
	    case 2:
	       FunError ("repetition value too big",InOut);
	       break;
	    case 0:
	       if (N < 0) FunError ("negative repetition",InOut);
	       else {
		  P = Repeat (&P->Val,(long) N);
		  DelLPtr (InOut->List);
		  InOut->List = P;
	       }
	       break;
	    }
      }
   }


/*
 * F_RDrop
 *
 * Drop the last n elements from a sequence
 *
 * Input
 *      InOut = pointer to <sequence number>
 */
private F_RDrop (InOut)
   ObjectPtr InOut;
   {
      register FPint N;
      ListPtr P,Result;
      register ListPtr R;

      if ((N = ListIndex (InOut,&P)) >= 0) 
	 if ((N = ListLength (P) - N) < 0) 
	    FunError ("sequence too short",InOut);
	 else {
	    Result = NULL;
	    NewList (&Result,N);
	    for (R = Result; R!=NULL; P=P->Next,R=R->Next) 
	       CopyObject (&R->Val,&P->Val);
	    DelLPtr (InOut->List);
	    InOut->List = Result;
	 }
   }


/*
 * F_LDrop
 *
 * Drop the first n elements from a sequence
 *
 * Input
 *      InOut = pointer to <sequence number>
 */
private F_LDrop (InOut)
   ObjectPtr InOut;
   {
      register FPint N;   
      ListPtr P; 

      if ((N = ListIndex (InOut,&P)) >= 0) {
	 for (; --N >= 0; P = P->Next)
	    if (P == NULL) {
	       FunError ("sequence too short",InOut);
	       return;
	    }
	 RepLPtr (&InOut->List,P);
      }
   }


/*
 * F_LTake
 *
 * Take the first n elements from a sequence
 *
 * Input
 *      InOut = pointer to <sequence number>
 */
private F_LTake (InOut)
   ObjectPtr InOut;
   {
      register long N;
      ListPtr P,Result;
      register ListPtr R;

      if ((N = ListIndex (InOut,&P)) >= 0) {
	 Result = NULL;
	 NewList (&Result,N);
	 for (R = Result; R!=NULL; P=P->Next, R=R->Next)
	    if (P != NULL)
	       CopyObject (&R->Val,&P->Val);
	    else {
	       FunError ("sequence too short",InOut);
	       DelLPtr (Result);
	       return;
	    } 
	 DelLPtr (InOut->List);
	 InOut->List = Result;
      }
   }


/*
 * F_RTake
 *
 * Take the last n elements from a sequence
 *
 * Input
 *      InOut = pointer to <sequence number>
 */
private F_RTake (InOut)
   ObjectPtr InOut;
   {
      register FPint N;
      ListPtr P;

      if ((N = ListIndex (InOut,&P)) >= 0) 
	 if ((N = ListLength (P) - N) < 0)
	    FunError ("sequence too short",InOut);
	 else {
	    while (--N >=0) P = P->Next;
	    RepLPtr (&InOut->List,P);
	 }
   }

private OpDef SubSeqOps [] = {
   {"dropl",    -1,     F_LDrop},
   {"dropr",    -1,     F_RDrop},
   {"pick",     -1,     F_Pick},
   {"repeat",   -1,     F_Repeat},
   {"takel",    -1,     F_LTake},
   {"taker",    -1,     F_RTake}
#if SCATTER_STORE
   {"scatter",  -1,     F_Scatter},
#endif
};

void D_subseq ()
   {
      GroupDef (SubSeqOps, OpCount (SubSeqOps), SysNode);
   }

/************************** end of F_subseq **************************/

