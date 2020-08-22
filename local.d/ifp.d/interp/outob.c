
/****** out.c *********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Feb 8, 1987          **/
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
#include <ctype.h>
#include "struct.h"
#include "string.h"

#define BerkMode 0

#define INDENT 3

/*
 * OutIndent
 *
 * Indent N places
 */
void OutIndent (N)
   int N;
   {
      for (; N >= 8; N-=8) printf ("\t");
      while (--N >=0) printf (" ");
   }

/*
 * QuoteCheck
 *
 * Check if string should be quoted.
 *
 * Input
 *      S = string
 * Output
 *      result = quote character ('\0','\'', or '\"');
 */
char QuoteCheck (S)
   StrPtr S;
   {
      CharPtr U;
      char Buf[256];
      boolean Single=0,Double=0,Quote=0;
      register char *T;

      if (S==NULL) return ('\"');
      else {
	 CPInit (&U,&S);
	 if (CPRead (&U,Buf,sizeof (Buf))) {
	    if (Buf [1] == '\0' && (Buf[0]=='f' || Buf[0]=='t' || Buf[0]=='?'))
	       return '\"';
	    do
	       for (T = Buf; *T; T++)
		  if (!isalpha (*T)) {
		     Quote=1;
		     if (*T == '\'') Single = 1;
		     if (*T == '\"') Double = 1;
		  }
	    while (CPRead (&U,Buf,sizeof (Buf)));
	 }

	 if (!Quote) return '\0';
	 else if (Single) return '\"';
	 else if (Double) return '\'';
	 else return '\"';             /* Should be something else */
      }
   }

/*
 * OutString
 *
 * Output a string.
 */
void OutString (S)
   StrPtr S;
   {
      char Buf[256];
      CharPtr U;

      if ((Debug & DebugRef) && S != NULL) printf ("[%d]",S->SRef);
      CPInit (&U,&S);
      while (CPRead (&U,Buf,sizeof (Buf))) printf ("%s",Buf);
   }

/*
 * OutList
 *
 * Input
 *      P = list to output
 */
void OutList (P)
   register ListPtr P;
   {
      printf ("<");
      if (P!=NIL)
	 while (1) {
	    if (Debug & DebugRef) printf ("{%d}",P->LRef + (1 - LRefOne));
	    OutObject (& P->Val);
	    if ((P=P->Next) == NULL) break; 
	    else printf (",");
	 }
      printf (">");
   }


/*
 * OutObject
 *
 * Output an object
 *
 * No reference counts change.
 */
void OutObject (X)
   ObjectPtr X;
   {
      if (SysStop > 1) return;
      else if (X == NULL) printf ("(NULL)");
      else
	 switch (X->Tag) {
	    case BOTTOM: printf ("?"); break;
	    case BOOLEAN:
	       switch (X->Bool) {
		  case 0: printf (BerkMode ? "F" : "f"); break;
		  case 1: printf (BerkMode ? "T" : "t"); break;
		  default: printf ("(BOOLEAN %d)",X->Bool); break;
	       }
	       break;
	    case INT:
	       printf ("%ld",X->Int);
	       break;

	    case FLOAT:
	       printf ("%g",X->Float);
	       break;

	    case LIST:
	       OutList (X->List);
	       break;

	    case STRING: {
	       register char Q;
	       Q = QuoteCheck (X->String);
	       if (Q) printf ("%c",Q);
	       OutString (X->String);
	       if (Q) printf ("%c",Q);
	    }  break;

	    case NODE:
	       OutNode (X->Node);
	       break;
	    default:
	       printf ("(tag = %d)",X->Tag);
	       break;
	 }
   }

#define LineLength 80

/*
 * OutLength
 *
 * Compute approximate number of characters required to output an object.
 * The count is stopped prematurely if it goes over LineLength.
 * 
 * No reference counts change.
 */
private int OutLength (X,Limit)
   ObjectPtr X;
   int Limit;
   {
      register ListPtr P;
      register int K;

      if (X == NULL) K = 6;     /* "(null)" */
      else
	 switch (X->Tag) {

	    case BOTTOM:
	    case BOOLEAN:
	       K = 1; /* "?","t","f" */
	       break;

	    case INT:
	       K = 5;
	       break;
 
	    case FLOAT:
	       K = 8;
	       break;

	    case LIST:
	       K = 2;                                   /* <> */
	       for (P=X->List; P!=NULL && K <= Limit; P=P->Next) 
		  K += 1 + OutLength (&P->Val,Limit); /* 1 for space between */
	       break;

	    case STRING:
	       K = 2 + LenStr (X->String);  /* "'...'" */
	       break;
	    default:
	       K=0;
	       break;
	 }
      return K;
   }

/*
 * OutPretty
 *
 * Output an object with indented sublists
 *
 * No reference counts change.
 */
void OutPretty (X,Indent)
   ObjectPtr X;
   int Indent;
   {
      register ListPtr P;

      if (SysStop > 1) return;
      OutIndent (Indent);
      if (X == NULL) printf ("(null)");
      else if (X->Tag != LIST) OutObject (X);
      else {
	 if ((OutLength (X,LineLength) + Indent) > LineLength) {
	    printf ("<\n");
	    for (P = X->List; P!=NULL; P=P->Next)
	       OutPretty (&P->Val,Indent+INDENT);
	    OutIndent (Indent);
	    printf (">\n");
	    return;
	 } else OutList (X->List);
      }
      printf ("\n");
   }


/************************** end of outob.c **************************/

