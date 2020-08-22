
/****** F_string.c ****************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  July 5, 1985          **/
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
#include "struct.h"
#include "string.h"
#include "node.h"

/*
 * F_Patom
 *
 * Convert an atom to it's string representation.
 */
private F_Patom (InOut)
   register ObjectPtr InOut;
   {
      CharPtr U;
      char Buf[255];
      StrPtr S;
      register char *T;
      extern char *sprintf();

      T = Buf;
      switch (InOut->Tag) {
	 case INT:
	    (void) sprintf (T,"%d",InOut->Int);
	    break;
	 case FLOAT:
	    (void) sprintf (T,"%g",InOut->Float);
	    break;
	 case BOOLEAN:
	    (void) sprintf (T,InOut->Bool ? "t":"f");
	    break;
	 case STRING:
	    return;
	 default:
	    FunError ("not atomic",InOut);
	    return;
      }
      S = NULL;
      CPInit (&U,&S);
      do CPAppend (&U,*T); while (*T++);
      RepTag (InOut,STRING);
      InOut->String = S;
   }


/*
 * F_Explode
 *
 * Convert a string to a list of characters
 */
private F_Explode (InOut)
   ObjectPtr InOut;
   {
      ListPtr Result = NULL;
      MetaPtr A = &Result;
      CharPtr U;
      char C[2];

      if (InOut->Tag != STRING)
	 FunError ("not a string",InOut);
      else {
	 CPInit (&U,&InOut->String);
	 while (CPRead (&U,C,2)) {
	    NewList (A,1L);
	    if (SysError) {DelLPtr (Result); return;}
	    (*A)->Val.Tag = STRING;
	    (*A)->Val.String = CopySPtr (CharString [C[0] & 0x7F]);
	    A = &(*A)->Next;
	 }
	 RepTag (InOut,LIST);
	 InOut->List = Result;
      }
   }


/*
 * F_Implode
 *
 * Catenate a list of strings into a single string.
 */
private F_Implode (InOut)
   ObjectPtr InOut;
   {
      CharPtr U,V;
      char C[2];
      ListPtr P;
      StrPtr S;

      if (InOut->Tag != LIST)
	 FunError ("not a sequence",InOut);
      else {
	 S = NULL;
	 CPInit (&U,&S);
	 for (P = InOut->List; P != NULL; P=P->Next) {
	    if (P->Val.Tag != STRING) {
	       FunError ("non-string in sequence",InOut);
	       CPAppend (&U,'\0');
	       DelSPtr (S);
	       return;
	    } else {
	       CPInit (&V,&P->Val.String);
	       while (CPRead (&V,C,2)) CPAppend (&U,C[0]);
	    }
	 }
	 CPAppend (&U,'\0');
	 RepTag (InOut,STRING);
	 InOut->String = S;
      }
   }


void D_string ()
   {                             
      (void) PrimDef (F_Explode,"explode",SysNode);
      (void) PrimDef (F_Implode,"implode",SysNode);
      (void) PrimDef (F_Patom,"patom",SysNode);
   }

/************************** end of F_string **************************/

