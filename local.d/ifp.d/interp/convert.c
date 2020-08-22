
/****** convert.c *****************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  July 2, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/* Type conversion functions */

#include <stdio.h>
#include <ctype.h>
#include "struct.h"
#include "string.h"
#include <math.h>

#define BUFSIZE 80 /* Maximum length of numeric string */

/*
 * GetFPInt
 *
 * Get value of FP integer.
 *
 * Input
 *    X = FP object
 *
 * Output
 *    *K = FPint value of X
 *    result = error code: 0 = X was converted to integer *K
 *                         1 = X not an integer
 *                         2 = X too big
 */
int GetFPInt (X,K)
   ObjectPtr X;
   FPint *K;
   {
      switch (X->Tag) {
	 default: return 1;
	 case INT: *K = X->Int; return 0;
	 case FLOAT: {
	    double F;
	    F = X->Float;
	    if (fabs (F) <= (double) FPMaxInt) {
	       *K = (FPint) F;
	       F -= (double) *K;
	       return fabs (F) >= CompTol;
	    } else return 2;
	 }
      }
   }

#if OPSYS==CTSS
/*
 * IsFloat
 *
 * Determine if a string represents floating point number as defined
 * by C's atof function.  This function is necessary for the CRAY
 * since there is a bug in sscanf for the CRAY.
 *
 * Input
 *	S = string
 *
 * Output
 *	result = true iff string represents number.
 */
int IsFloat (S)
   register char *S;
   {
      int Digits = 0;
      if (*S == '+' || *S == '-') S++;
      while (isdigit (*S)) {
	 S++;
	 Digits++;
      }
      if (*S == '.') 
	 while (isdigit (*++S)) Digits++;
      if (!Digits) return 0;
      if (*S == '\0') return 1;
      if (*S++ != 'e') return 0;
      if (*S == '+' || *S == '-') S++;
      while (isdigit (*S)) S++;
      return *S == '\0';
   }
#endif /* OPSYS==CTSS */

/*
 * StrToFloat
 *
 * Convert object to float representation if possible.
 *
 * Input
 *    *X = object
 *
 * Output
 *    *X = new representation of object
 *    result = 1 if *X is float, 0 otherwise.
 */
boolean StrToFloat (X)
   ObjectPtr X;
   {
      CharPtr U;
      char Buf[BUFSIZE+1];
      double F;
#if OPSYS!=CTSS
      char Term;
#endif
      CPInit (&U,&X->String);
      (void) CPRead (&U,Buf,BUFSIZE);

#if OPSYS==CTSS
      if (!IsFloat (Buf)) return 0;
      F = atof (Buf);
#else 
      Buf [strlen (Buf)] = '\1';
      if (2 != sscanf (Buf,"%lf%c",&F,&Term) || Term != '\1') return 0;
#endif
      RepTag (X,FLOAT);
      X->Float = (FPfloat) F;
      return 1;
   }

/*
 * GetDouble
 *
 * Output
 *    result = 0 if *D is valid, 1 otherwise.
 */
int GetDouble (X,D)
   ObjectPtr X;
   double *D;
   {
     switch (X->Tag) {
	case INT:   *D = X->Int; return 0;
	case FLOAT: *D = X->Float; return 0;
	default: return 1;
      }
   }


/****************************** end of convert.c *****************************/
