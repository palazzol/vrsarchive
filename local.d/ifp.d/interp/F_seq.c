
/****** F_seq.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Aug 5, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/******************* sequence (structural) functions ******************/

#include <stdio.h>
#include "struct.h"
#include "node.h"

/*
 * F_Cat
 *
 * Sequence catenation
 */
private F_Cat (InOut)
   register ObjectPtr InOut;
   {
      register MetaPtr E;
      register ListPtr P;

      if (InOut->Tag != LIST) {
	 FunError (ArgNotSeq,InOut);
	 return;
      }
      P = InOut->List;
      if (P == NULL) return;

      do
	 if (P->Val.Tag != LIST) {
	    FunError ("elements not sequences",InOut);
	    return;
	 }
      while ((P=P->Next)!=NULL);

      Copy2Top (& InOut->List);
      if (SysError) return;

      P = InOut->List;
      E = &P->Val.List;
      for (P=P->Next; P!=NULL; P=P->Next) {
	 while (*E!=NULL) E = &(*E)->Next;
	 *E = P->Val.List;
	 P->Val.Tag = BOTTOM;
      }
      E = &InOut->List;
      RepLPtr (E,(*E)->Val.List);
   }


/*
 * F_Iota
 *
 * Generate <1...id>
 */
private F_Iota (InOut)
   register ObjectPtr InOut;
   {
      FPint N;
      register FPint K;
      register ListPtr Pr;

      switch (GetFPInt (InOut,&N)) {
	 case 1: FunError ("not an integer",InOut); return;
	 case 2: FunError ("too big"       ,InOut); return;
	 case 0:
	    if (N < 0) FunError ("negative",InOut);
	    else {
	       InOut->Tag = LIST;
	       InOut->List = NULL;  
	       NewList (&InOut->List,N);
	       if (SysError) return;
	       for (Pr=InOut->List,K=0; Pr!=NULL; Pr=Pr->Next) 
		  Pr->Val.Tag = INT,
		  Pr->Val.Int = ++K;
	    }
	    return;
      }
   }


/*
 * F_Id
 */
private F_Id ()
   {
      return; /* do nothing */;
   }


/*
 * F_Length
 *
 * Find sequence length
 */
private F_Length (InOut)
   ObjectPtr InOut;
   {
      register FPint N;

      switch (InOut->Tag) {
	 default:
	    FunError (ArgNotSeq,InOut);
	    return;
	 case LIST:
	    N = ListLength (InOut->List);
	    break;
      }
      RepTag (InOut,INT);
      InOut->Int = N;
   }

/*
 * F_LApnd
 *
 *           +--------+
 * InOut --->|  list  |
 *           +----+---+ A
 *                |     |
 *                V     V
 *           +------------+      +------------+
 *           | object | o-+----->|  list  |///|
 *           +------------+      +---+--------+
 *                                   |
 *                                   V
 *                                  ...
 */
private F_LApnd (InOut)
   ObjectPtr InOut;
   {
      MetaPtr A;
      if (! PairTest (InOut, ~0, SEQUENCE))
	 FunError (ArgObSeq,InOut);
      else {
	 CopyTop (&InOut->List);
	 A = & InOut->List->Next;
	 RepLPtr (A,(*A)->Val.List);
      }
   }


/*
 * F_RApnd
 *
 *           +--------+
 * InOut --->|  list  |
 *           +----+---+
 *                |
 *                V
 *           +------------+      +------------+
 *           |  list  | o-+----->| object |///|
 *           +------------+      +------------+
 *                |
 *                V
 *               ...
 *
 */
private F_RApnd (InOut)
   ObjectPtr InOut;
   {
      register MetaPtr E;
      ListPtr P;

      if (! PairTest (InOut,1 << LIST,~0))
	 FunError (ArgSeqOb,InOut);

      else {
	 Copy2Top (& InOut->List);
	 if (SysError) return;
	 P = InOut->List;
	 for (E = &P->Val.List; (*E)!=NULL; E = &(*E)->Next) continue;
	 *E = P->Next;
	 P->Next=NULL;
	 RepLPtr (&InOut->List,P->Val.List);
	 /* No system error possible since source is fresh list */
      }
   }

/*
 * F_LDist
 *
 * Distribute from left
 */
private F_LDist (InOut)
   ObjectPtr InOut;
   {
      ListPtr R=NULL;
      register ListPtr P1,P2,P3,PT;
      long N;

      if (!PairTest (InOut, ~0, SEQUENCE))

	 FunError (ArgObSeq,InOut);

      else {

	 Copy2Top (&InOut->List);
	 if (SysError) return;
	 P1 = InOut->List;         	/* P1 = pointer to arg list     */
	 P2 = P1->Next;
	 P3 = P2->Val.List; 		/* P3 = pointer to 2nd arg list */
	 P2->Val.List = NULL;
	 N = ListLength (P3); 
	 NewList (&R,N);		/* R = pointer to result list   */
	 if (SysError) return;
	 P2 = Repeat (&P1->Val,N);	/* P2 = pointer to 1st arg list */
	 if (SysError) {DelLPtr (R); return;}

	 for (P1=R; P1!=NULL; P1=P1->Next) {
	    P1->Val.Tag = LIST;
	    P1->Val.List = P2;
	    PT = P2;
	    P2 = P2->Next;
	    PT->Next = P3;
	    PT = P3;
	    P3 = P3->Next;
	    PT->Next = NULL;
	 }

	 DelLPtr (InOut->List);
	 InOut->List = R;
      }
   }


/*
 * F_RDist
 *
 * Distribute from right
 */
private F_RDist (InOut)
   ObjectPtr InOut;
   {
      ListPtr R,P,P1,P2;
      long N;

      if (! PairTest (InOut, SEQUENCE, ~0))

	 FunError (ArgSeqOb,InOut);

      else {

	 Copy2Top (&InOut->List);
	 if (SysError) return;
	 P = InOut->List;            /* P = pointer to arg list */
	 P2 = P->Val.List;        /* P2 = pointer to first arg list */
	 P->Val.Tag = BOTTOM;
	 P = P->Next;                     /* P = pointer to 2nd arg */
	 N = ListLength (P2);
	 R = NULL; NewList (&R,N);        /* R = pointer to result list */
	 if (SysError) return;

	 for (P1=R; P1!=NULL; P1=P1->Next) {
	    P1->Val.Tag = LIST;
	    P1->Val.List = CopyLPtr (P);
	    if (SysError) {DelLPtr (R); return;}
	    Rot3 (&P1->Val.List,&P2,&P2->Next);
	 }
	 RepLPtr (&InOut->List,R);
	 DelLPtr (R);
      }
   }


/*
 * F_Reverse
 *
 * Reverse a list
 */
F_Reverse (InOut)     /* Imported by F_RInsert in forms.c */
   ObjectPtr InOut;
   {
      ListPtr P,Q;

      switch (InOut->Tag) {
	 default:
	    FunError (ArgNotSeq,InOut);
	    break;
	 case LIST:
	    P = InOut->List;
	    CopyTop (&P);
	    if (SysError) return;
	    for (Q=NULL; P!=NULL; Rot3 (&P,&P->Next,&Q)) continue; 
	    InOut->List = Q;
	    break;
      }
   }


/*
 * TransCheck
 *
 * Check that InOut is matrix
 *
 * Input
 *     InOut = pointer to object
 *
 * Output
 *     result = NULL iff a matrix, error code otherwise.
 *     *Cols = number of columns
 */
private char *TransCheck (InOut,Cols)
   ObjectPtr InOut;
   long *Cols;
   {
      register ListPtr V,VR;

      if (InOut->Tag != LIST)
	 return "argument not a sequence.";
      else if (NULL == (VR = InOut->List))
	 return "argument is empty sequence.";
      else
	 for (V = VR; V !=NULL; V = V->Next)
	    if (V->Val.Tag != LIST)
	       return "argument subelements must be sequences.";
	    else if (V==VR) *Cols = ListLength (V->Val.List);
	    else if (*Cols != ListLength (V->Val.List))
	       return "argument not rectangular.";
	    else continue;
      return NULL;
   }


/*
 * F_Trans
 *
 * Transpose a matrix (sequence of sequences)
 */
private F_Trans (InOut)
   ObjectPtr InOut;
   {
      char *E; long Cols;
      ListPtr VR,HR,H;
      register ListPtr U,V;
      register MetaPtr A;

      /* Check for rectangularness */
      if (NULL != (E = TransCheck (InOut,&Cols))) {
	 FunError (E,InOut);
	 return;
      }

      /* Make fresh copy of vertical top level  and rows */
      Copy2Top (&InOut->List);
      if (SysError) return;
      else VR = InOut->List;
	
      /* Make horizontal top level */
      HR = NULL;
      NewList (&HR,Cols);

      /* Transpose matrix column by column */
      for (H=HR; H!=NULL; H=H->Next) {
	 H->Val.Tag = LIST;
	 H->Val.List = VR->Val.List;

	 /* Relink the column and advance the VR list to the next column */
	 for (V=VR; V!=NULL; V=U) {
	    U = V->Next;
	    A = &V->Val.List->Next;
	    V->Val.List = *A;
	    *A = U==NULL ? NULL : U->Val.List;
	 }
      }
      /* Delete the old vertical top level and return new matrix */
      DelLPtr (VR); InOut->List = HR;
   }


/*
 * F_Tail
 */
private F_Tail (InOut)
   ObjectPtr InOut;
   {
      register ListPtr P;
      switch (InOut->Tag) {
	 default:
	    FunError (ArgNotSeq,InOut);
	    break;
	 case LIST:
	    if (NULL == (P = InOut->List)) FunError (ArgNull,InOut);
	    else RepLPtr (&InOut->List,P->Next);
	    break;
      }
   }


/*
 * F_RTail
 *
 * Drop last element
 */
private F_RTail (InOut)
   ObjectPtr InOut;
   {
      register MetaPtr A;
      if (InOut->Tag != LIST)
	 FunError (ArgNotSeq,InOut);
      else if (NULL == InOut->List)
	 FunError (ArgNull,InOut);
      else {
	 CopyTop (A = &InOut->List);
	 if (SysError) return;
	 while ((*A)->Next != NULL) A = &(*A)->Next;
	 RepLPtr (A,(ListPtr) NULL);
      }
   }


OpDef SeqOps [] = {
   {"apndl",    -1,     F_LApnd},
   {"apndr",    -1,     F_RApnd},
   {"cat",      -1,     F_Cat},
   {"distl",    -1,     F_LDist},
   {"distr",    -1,     F_RDist},
   {"id",       -1,     F_Id},
   {"iota",     -1,     F_Iota},
   {"length",   -1,     F_Length},
   {"reverse",  -1,     F_Reverse},
   {"tl",       -1,     F_Tail},
   {"tlr",      -1,     F_RTail},
   {"trans",    -1,     F_Trans}
};

void D_seq ()
   {
      GroupDef (SeqOps, OpCount (SeqOps), SysNode);
   }  

/************************** end of F_seq **************************/

