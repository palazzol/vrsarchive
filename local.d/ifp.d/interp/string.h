
/****** string.h ******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Jan 20, 1987          **/
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
 * CharPtr
 *
 * Character pointer
 *
 * Character pointers are for an IFP string what file pointers are
 * for a UNIX file.  Character pointers are used for both creating
 * (writing) and scanning (reading) IFP strings.  The structure of
 * IFP strings (type String) is described in struct.h.
 */
typedef struct {
   int CPCount;    /* number of characters left in current segment */
   char *CPChar;   /* pointer to current character */
   StrPtr *CPStr;  /* pointer to root of string */
   StrPtr CPSeg;   /* pointer to current segment of string */
} CharPtr;

extern StrPtr *CharString;		/* from string.c */
extern StrPtr MakeString ();
extern void DelSPtr ();     
extern StrPtr CopySPtr (); 
extern void CPInit (), CPAppend ();
extern boolean CPRead ();
extern FPint LenStr ();

/************************* end of string.h *************************/

