
/****** F_pred.c ******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Dec 1, 1985          **/
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
#include <math.h>
#include "struct.h"
#include "node.h"

/************************** boolean functions **************************/

/*
 * PairTest
 *
 * Check if object is a pair of <type1,type2>
 *
 * Input
 *      X = object to test
 *      Mask1,Mask2 = masks representing type1 and type2 respectively.
 *                    E.g 1<<INT is type INT, (1<<INT)|(1<<FLOAT) is numeric.
 *
 * Output
 *      result = 1 if true, 0 if false
 */
boolean PairTest (X,Mask1,Mask2)
   ObjectPtr X;
   int Mask1,Mask2;
   {
      register ListPtr P,Q;

      if (X->Tag != LIST) 
	 if (X->Tag == NODE) NodeExpand (X);
	 else return 0;

      if ((P=X->List) == NULL || (Q=P->Next) == NULL || Q->Next!=NULL) return 0;
      if (P->Val.Tag == NODE) NodeExpand (&P->Val);
      if (Q->Val.Tag == NODE) NodeExpand (&Q->Val);
      return Mask1 >> P->Val.Tag & Mask2 >> Q->Val.Tag & 1; 
   }

/*
 * Anytime two objects are found to be equal, we can replace one with
 * the other to save memory.  Clearly the memory savings is offset by
 * a little more time, program complexity, and bringing obscure bugs
 * out of the woodwork!  Therefore the replacing action is enabled if
 * MERGE=1, disabled if MERGE=0.
 *
 * P.S. Someone should check if the merging is really worth the cost.
 */
#define MERGE 0

/*
 * BoolOp
 *
 * Boolean operation
 *
 * Input
 *      InOut = argument
 *      Op = boolean op (4-bit vector representing truth table)
 *
 * Output
 *      *A = first element of pair if result is true, undefined otherwise
 *      *B = second ...
 */
private BoolOp (InOut,Op)
   ObjectPtr InOut;
   int Op;
   {
      extern void RepBool ();
      register ListPtr P;

      if (PairTest (InOut,1<<BOOLEAN,1<<BOOLEAN)) {
	 P = InOut->List;
	 RepBool (InOut, (Op >> (P->Next->Val.Bool << 1) + P->Val.Bool) & 1);
      } else
	 FunError ("not a boolean pair",InOut);
   }


/*
 * F_Not
 *
 * Boolean negation
 */
private F_Not (InOut)
   ObjectPtr InOut;
   {
      if (InOut->Tag == BOOLEAN) InOut->Bool ^= 1;
      else FunError ("not boolean",InOut);
   }


/* 
 * F_L2
 */
private F_L2 (InOut)
   ObjectPtr InOut;
   {
      switch (InOut->Tag) {
	 case INT: RepBool (InOut,InOut->Int < 2); break;
	 case FLOAT: RepBool (InOut,InOut->Float < 2); break;
	 default: FunError ("not numeric",InOut); break;
      }
   } 

/*
 * F_False
 *
 * Check if argument is boolean false (#f).
 */
private F_False (InOut)
   ObjectPtr InOut;
   {
      if (InOut->Tag == BOTTOM)
	 FunError (ArgBottom,InOut);
      else
	 if (InOut->Tag == BOOLEAN) InOut->Bool ^= 1;
	 else RepBool (InOut,0);
   }

/*
 * F_Odd
 *
 * Check if integral argument is odd.
 */
private F_Odd (InOut)
   ObjectPtr InOut;
   {
      FPint N;

      switch (GetFPInt (InOut,&N)) {
	  case 0:
	     RepBool (InOut,(int)N & 1);
	     return;
	  case 2:
	     FunError ("not enough precision",InOut);
	     return;
	  default:
	     FunError ("not an integer",InOut);
	     return;
      }
   }

/*
 * BoolSeq
 *
 * Evaluate "any" or "all" predicate.
 *
 * Input
 *      *InOut = argument
 *      Op = identity element of operation
 *
 * Output
 *      *InOut = result
 */
private BoolSeq (InOut,Op)
   ObjectPtr InOut;
   int Op;
   {
      register boolean R;
      register ListPtr P;

      if (InOut->Tag != LIST) FunError (ArgNotSeq,InOut);
      else {
	 R = 0;
	 for (P = InOut->List; P != NULL; P=P->Next) 
	    if (P->Val.Tag == BOOLEAN) R |= P->Val.Bool ^ Op;
	    else {
	       FunError ("non-boolean element",InOut);
	       return;
	    }
	 RepBool (InOut, R ^ Op);
      }
   }


#if MERGE
/*
 * StrMerge
 *
 * Compare two strings.  Merge together if they are equal.
 *
 * Output
 *      result = 1 if equal, 0 otherwise
 */
static int StrMerge (S,T)
   register StrPtr *S,*T;
   {
      if (*S == *T) return 2;               /* strings are identical */
      else if (StrComp (*S,*T)) return 0;   /* strings are different */
      else {
	 register StrPtr *U;                /* equal and not identical */
	 if ((*S)->SRef < (*T)->SRef) 
	    U=S, S=T, T=U;
	 if ((*S)->SRef + 1) {              /* S has larger SRef */
	    DelSPtr (*T);
	    *T = *S;
	    (*S)->SRef++;
	 }
	 return 1;
      }
   }
#endif

/*
 * ObEqual
 *
 * Compare two objects.  A comparison tolerance is used for floating point
 * comparisons.
 *
 * Output
 *       result = 0 if objects are not equal
 *                1 if objects are equal within comparison tolerance
 */
boolean ObEqual (X,Y)
   ObjectPtr X,Y;
   {
      if (X->Tag != Y->Tag) {

	 switch (X->Tag) {

	    case INT:
	       return Y->Tag==FLOAT && 
		      !FloatComp ((double) X->Int,(double) Y->Float);

	    case FLOAT:
	       return Y->Tag==INT && 
		      !FloatComp ((double) X->Float,(double) Y->Int);

	    case NODE:
	       NodeExpand (X);
	       break;

	    case LIST:
	       if (Y->Tag==NODE) NodeExpand (Y); 
	       break;

	    default: return 0;
	 }
      }
      switch (X->Tag) {

	 case BOTTOM:  return 1;
	 case BOOLEAN: return X->Bool == Y->Bool;
	 case INT:     return X->Int == Y->Int;
	 case FLOAT:   return !FloatComp ((double) X->Float, (double) Y->Float);
	 case STRING:
#if MERGE
	    return StrMerge (&X->String,&Y->String);
#else
	    return !StrComp (X->String,Y->String);
#endif
	 case LIST: {
	    register ListPtr P=X->List, Q=Y->List;
	    while (1) {
	       if (P == NULL) return Q == NULL;
	       if (Q == NULL || !ObEqual (&P->Val,&Q->Val)) return 0;
	       P = P->Next; Q = Q->Next;
	    }
	 }
	 case NODE: return X->Node == Y->Node; 
	 default:   return 0; /* Tag error */
      }
   }

#define max(A,B) ((A) > (B) ? (A) : (B))

/*
 * FloatComp
 *
 * X ~= Y if abs(X-Y) / max(abs(X),abs(Y)) <= comparison tolerance.
 *
 * Output
 *      result = -1 if X < Y
 *                0 if X ~= Y
 *                1 if X > Y
 */
int FloatComp (X,Y)
   double X,Y;
   {
      double Xm,Ym,D;
      Xm = fabs (X);
      Ym = fabs (Y);
      D = X-Y;
      if (fabs (D) <= CompTol*max(Xm,Ym)) return 0;
      else return D>0 ? 1 : -1;
   }

/*
 * F_Equal
 *
 * Object comparison for equality or inequality
 */
private F_Equal (InOut,Not)
   ObjectPtr InOut;
   int Not;
   {
      if (!PairTest (InOut,~0,~0))
	 FunError ("argument not a pair",InOut);
      else 
	 RepBool (InOut, Not ^ (0 < ObEqual (&InOut->List->Val,
					     &InOut->List->Next->Val)));
   }


/*
 * F_Null
 *
 * Null sequence test
 */
private F_Null (InOut)
   ObjectPtr InOut;
   {
      switch (InOut->Tag) {
	 case LIST:
	    RepBool (InOut, InOut->List == NULL);
	    break;
	 default: 
	    FunError (ArgNotSeq,InOut);
	    break;
      }
   }


/*
 * F_Pair
 *
 * Check if argument is a pair.
 */
private F_Pair (InOut)
   ObjectPtr InOut;
   {
      RepBool (InOut, PairTest (InOut,~0,~0));
   }


/*
 * F_Tag
 *
 * Check for specified tag
 */
private F_Tag (InOut,TagSet)
   ObjectPtr InOut;
   {
      if (InOut->Tag) 
	 RepBool (InOut,TagSet >> InOut->Tag & 1);
      else 
	 FunError (ArgBottom,InOut);
   }


/*
 * CompAtom
 *
 * Compare two atoms for <,<=,=>, or >
 *
 * Strings are ordered lexigraphically.
 * Numbers are ordered in increasing value.
 *
 * Input
 *      *InOut = <X,Y>
 *      Op = comparison bit vector [>,=,<]
 *
 * Output
 *      *InOut = sign (X - Y) or BOTTOM
 */
private CompAtom (InOut,Op)
   ObjectPtr InOut;
   int Op;
   {
      register ObjectPtr X,Y;
      int D,E;
      static char *ErrMessage [3] = {
	 "not an atomic pair",
	 "booleans not comparable",
	 "strings and numbers not comparable"
      };

      E = 0;
      if (!PairTest (InOut,ATOMIC,ATOMIC)) E = 1;
      else {
	 X = &InOut->List->Val;
	 Y = &InOut->List->Next->Val;
	 if (X->Tag == BOOLEAN || Y->Tag == BOOLEAN) E = 2;
	 else if (X->Tag == STRING || Y->Tag == STRING) {
	    if (X->Tag != Y->Tag) E = 3;
	    else {
	       D = StrComp (X->String,Y->String);
	       if (D) D = (D>0) ? 1 : -1;
	    }
	 } else
	    if (X->Tag == INT)
	       if (Y->Tag == INT)
		  D = (X->Int > Y->Int) - (X->Int < Y->Int);
	       else
		  D = FloatComp ((double) X->Int,(double) Y->Float);
	    else
	       if (Y->Tag == INT)
		  D = FloatComp ((double) X->Float,(double) Y->Int);
	       else
		  D = FloatComp ((double) X->Float,(double) Y->Float);
	 }
      if (E) FunError (ErrMessage [E-1],InOut);
      else RepBool (InOut, (Op >> (D+1)) & 1);
   }


/*
 * CompLength
 *
 * Compare the length of two sequences.
 *
 * Input
 *      InOut = argument
 *      Shorter = if 0 then "longer" comparison, "shorter" otherwise.
 */
private CompLength (InOut,Shorter)
   ObjectPtr InOut;
   int Shorter;
   {
      register ListPtr P,Q;

      if (!PairTest (InOut,1<<LIST,1<<LIST))
	 FunError ("not a pair of sequences",InOut);
      else {
	 P = InOut->List;
	 Q = P->Next->Val.List;
	 P = P->Val.List;
	 while (P != NULL && Q != NULL) {
	    P = P->Next;
	    Q = Q->Next;
	 }
	 RepBool (InOut, (Shorter ? Q : P) != NULL);
      }
   }

/*
 * F_Member
 */
private F_Member (InOut)
   ObjectPtr InOut;
   {
      register ListPtr P;
      register ObjectPtr X;

      if (! PairTest (InOut,1 << LIST,~0))

	 FunError (ArgSeqOb,InOut);

      else {

	 P = InOut->List;
	 X = & P->Next->Val;
	 for (P = P->Val.List; P!=NULL; P=P->Next)
	    if (ObEqual (& P->Val,X)) break;
	 RepBool (InOut, P != NULL);
      }
   }

private OpDef LogicOps [] = {
   {"all",      1,      BoolSeq},
   {"and",      0x8,    BoolOp},
   {"any",      0,      BoolSeq},
   {"atom",     ATOMIC, F_Tag},
   {"boolean",  1<<BOOLEAN,     F_Tag},
   {"false",    -1,     F_False},
   {"imply",    0xD,    BoolOp},
   {"longer",   0,      CompLength},
   {"member",   -1,     F_Member},
   {"null",     -1,     F_Null},
   {"numeric",  NUMERIC,F_Tag},
   {"odd",      -1,     F_Odd},
   {"or",       0xE,    BoolOp},
   {"pair",     -1,     F_Pair},
   {"shorter",  1,      CompLength},
   {"xor",      0x6,    BoolOp},
   {"=",        0,      F_Equal},
   {"~=",       1,      F_Equal},
   {"~",        -1,     F_Not},
   {">",        0x4,    CompAtom},
   {"<",        0x1,    CompAtom},
   {">=",       0x6,    CompAtom},
   {"<=",       0x3,    CompAtom},
   {"l2",	0,	F_L2}
};

void D_pred ()
   {
      GroupDef (LogicOps, OpCount (LogicOps), LogicNode);
   }

/******************************* end of F_pred *******************************/

