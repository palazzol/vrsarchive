
/****** F_arith.c *****************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  June 4, 1986          **/
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
#include <errno.h>
#include "struct.h"
#include "node.h"

#if OPSYS!=CTSS
extern int errno;      /* exists somewhere in UNIX */
#endif

/* NOTE - function Dyadic assumes integers are in two's complement form! */

private F_Minus (), F_AddN (), Monadic (), Dyadic (), F_Sum ();

private OpDef OpArith [] = {
#if OPSYS!=CTSS
   {"ln",       0,      Monadic},
   {"exp",      1,      Monadic},
   {"sqrt",     2,      Monadic},
   {"sin",      3,      Monadic},
   {"cos",      4,      Monadic},
   {"tan",      5,      Monadic},
   {"arcsin",   6,      Monadic},
   {"arccos",   7,      Monadic},
   {"arctan",   8,      Monadic},
#endif
   {"minus",    -1,     F_Minus},
   {"add1",     1,      F_AddN},
   {"sub1",     -1,     F_AddN},
   {"+",        0,      Dyadic},
   {"-",        1,      Dyadic},
   {"*",        2,      Dyadic},
   {"%",        3,      Dyadic},
#if OPSYS!=CTSS
   {"mod",      4,      Dyadic},
   {"div",      5,      Dyadic},
#endif
   {"min",      6,      Dyadic},
   {"max",      7,      Dyadic},
#if OPSYS!=CTSS
   {"power",    8,      Dyadic},
#endif
   {"sum",      -1,     F_Sum}
};


/*
 * Monadic
 *
 * Evaluate a monadic function
 *
 * Input
 *      InOut = argument to apply function
 *      Op = operation - see array F_Name in code for values
 *
 * Output
 *      InOut = result of applying function
 */
private Monadic (InOut,Op)
   ObjectPtr InOut;
   int Op;
   {
      double X,Z;
      register int E;

      if (GetDouble (InOut,&X)) FunError ("not numeric",InOut);
      else {
	 E = 0;
	 switch (Op) {
#if OPSYS!=CTSS
	    case 0:                     /* base e log */
	       if (X <= 0) E = EDOM;
	       else Z = log (X);
	       break;
	    case 1:                     /* base e power */
	       if (X >= LNMAXFLOAT) E = ERANGE;
	       else Z = exp (X);
	       break;
	    case 2:                     /* square root */
	       if (X < 0) E = EDOM;
	       else Z = sqrt (X);
	       break;
	    case 3:                     /* sin */
	       Z = sin (X);
	       break;
	    case 4:                     /* cos */
	       Z = cos (X);
	       break;
	    case 5:                     /* tan */
	       Z = tan (X);
	       break;
	    case 6:                     /* arcsin */
	       Z = asin (X);
	       E = errno;
	       break;
	    case 7:                     /* arccos */
	       Z = acos (X);
	       E = errno;
	       break;
	    case 8:                     /* arctan */
	       Z = atan (X);
	       E = errno;
	       break;
#endif /* OPSYS!=CTSS */
	    case 9:                     /* minus */
	       Z = -X;
	       E = 0;
	       break;
	 }
	 switch (E) {
#if OPSYS!=CTSS
	    case EDOM:
	       FunError ("domain error",InOut);
	       break;
	    case ERANGE:
	       FunError ("range error",InOut);
	       break;
#endif
	    default:
	       InOut->Tag = FLOAT;
	       InOut->Float = Z;
	       break;
	 }
      }
   }


private F_Minus (InOut)
   register ObjectPtr InOut;
   {
      if (InOut->Tag == INT && InOut->Int != FPMaxInt+1)
	 InOut->Int = - InOut->Int;
      else Monadic (InOut,9);
   }


/*
 * F_Sum
 */
private F_Sum (InOut)
   ObjectPtr InOut;
   {
      Object S;
      register ListPtr P;

      switch (InOut->Tag) {
	 default:
	    FunError (ArgNotSeq,InOut);
	    return;
	 case LIST:
	    S.Tag = INT;
	    S.Int = 0;
	    for (P=InOut->List; P!=NULL; P=P->Next) {
	       if (P->Val.Tag != INT && P->Val.Tag != FLOAT) {
		  FunError ("non-numeric sequence",InOut); 
		  return;
	       }
	       if (S.Tag == INT) {
		  if (P->Val.Tag == INT) {

		     /* Both arguments are integers. See if we can avoid    */
		     /* floating arithmetic.                                */

		     FPint Zi = S.Int + P->Val.Int;
		     if ((S.Int ^ P->Val.Int) < 0 || (S.Int^Zi)) 
			 /* arithmetic overflow occured - float result */;
		     else {
			S.Int = Zi; 
			continue;
		     }
		  }
		  S.Float = S.Int; 
		  S.Tag = FLOAT;
	       }
	       S.Float += P->Val.Tag==INT ? P->Val.Int : P->Val.Float;
	    }
	    break;
      }
      RepObject (InOut,&S);
   }
 
/*
 * Dyadic
 *
 * Evaluate a dyadic function
 *
 * Input
 *      InOut = argument to apply function
 *      Op = operation - see case statement in code for possibilities
 *
 * Output
 *      InOut = result of applying function
 *
 * The author sold his anti-GOTO morals for speed.
 */
private Dyadic (InOut,Op)
   register ObjectPtr InOut;
   register int Op;
   {
      double X,Y,Z;
      register FPint Xi,Yi,Zi;
      register ListPtr P,Q;
      static char *DivZero = "division by zero";

      if (InOut->Tag != LIST ||
	  NULL == (P=InOut->List) ||
	  NULL == (Q=P->Next) ||
	  Q->Next != NULL ||
	  NotNumPair (P->Val.Tag,Q->Val.Tag)) {

	 FunError ("not a numeric pair",InOut);
	 return;
      }

      if (IntPair (P->Val.Tag,Q->Val.Tag)) {

	 /* Both arguments are integers. See if we can avoid floating point */
	 /* arithmetic.                                                     */

	 Xi = P->Val.Int;
	 Yi = Q->Val.Int;

	 switch (Op) {

	    case 0:
	       /* assume two's complement arithmetic */
	       Zi = Xi+Yi;
	       if (((Xi ^ Yi) | ~(Xi ^ Zi)) < 0) goto RetInt;
	       break;
	       /* else arithmetic overflow occured */

	    case 1:
	       /* assume two's complement arithmetic */
	       Zi = Xi - Yi;
	       if (((Xi ^ Yi) & (Xi ^ Zi)) >= 0) goto RetInt;
	       /* else arithmetic overflow occured */
	       break;

	    case 2:
	       Zi = Xi * Yi;
	       if (Yi==0 || Zi/Yi == Xi) goto RetInt;
	       /* else arithmetic overflow occured */
	       break;

	 /* case 3: division  result always FLOAT */

#if OPSYS!=CTSS
	    case 4:                     /* mod */
	       if (Xi >= 0 && Yi > 0) {
		  Zi = Xi % Yi;
		  goto RetInt;
	       }
	       break;

	    case 5:                     /* div */
	       if (Xi >= 0 && Yi > 0) {
		  Zi = Xi / Yi;
		  goto RetInt;
	       }
	       break;
#endif /* OPSYS!=CTSS */

	    case 6:
	       Zi = Xi > Yi ? Yi : Xi;
	       goto RetInt;

	    case 7:
	       Zi = Xi < Yi ? Yi : Xi;
	       goto RetInt;

	 /* case 8: power result always FLOAT */
	 }
      }

      X = P->Val.Tag==INT ? P->Val.Int : P->Val.Float;
      Y = Q->Val.Tag==INT ? Q->Val.Int : Q->Val.Float;

      switch (Op) {
	 case 0: Z = X + Y; break;
	 case 1: Z = X - Y; break;
	 case 2: Z = X * Y; break;
	 case 3: 
	    if (Y==0.0) {
	       FunError (DivZero,InOut);
	       return;
	    }
	    Z = X / Y; 
	    break;
#if OPSYS!=CTSS
	 case 4:
	    Z = Y==0.0 ? 0.0 : X - floor (X / Y) * Y;   /* mod */
	    break;
	 case 5:
	    if (Y==0.0) {                               /* div */
	       FunError (DivZero,InOut);
	       return;
	    }
	    Z = floor (X / Y);
	    break;
#endif
	 case 6: Z = X > Y ? Y:X; break;
	 case 7: Z = X > Y ? X:Y; break;
#if OPSYS!=CTSS
	 case 8: Z = pow (X,Y);   break;
#endif
      }
      InOut->Tag = FLOAT;
      InOut->Float = Z;

   Return:
      DelLPtr (P);
      return;

   RetInt: 
      InOut->Tag = INT;
      InOut->Int = Zi;
      goto Return;
   }


/*
 * F_Add1
 */
private F_AddN (InOut,N)
   register ObjectPtr InOut;
   int N;
   {
      register FPint K;

      switch (InOut->Tag) {
	 case INT:
	    K = InOut->Int + N;
	    if (N >= 0 ? InOut->Int <= K : InOut->Int >  K) {
	       InOut->Int = K;
	       return;
	    }
	    /* else integer overflow - convert and drop down */
	    InOut->Float = ((FPfloat) InOut->Int);
	    InOut->Tag = FLOAT;
	 case FLOAT:
	    InOut->Float = InOut->Float + N;
	    break;
	 default:
	    FunError ("not a number",InOut);
	    break;
      }
   }

void D_arith ()
   {
      GroupDef (OpArith,OpCount (OpArith), ArithNode);
   }

/************************** end of F_arith.c **************************/

