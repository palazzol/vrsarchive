
/****** inob.h ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Sept 9, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

#define INBUFSIZE 255     /* 65 <= INBUFSIZE <= 255 for DOS */

/*
 * InDesc
 *
 * Input descriptor.
 *
 * Currently, there are three forms of IFP input:
 *
 *	1. Definition files
 *	2. Import files
 *	3. Terminal input
 *
 * All three forms are managed by input descriptors.  An input descriptor
 * buffers the file, and keeps track of context (e.g. line number).
 */

typedef struct {
   char *InPtr;           /* Pointer to current character being scanned   */
   int InLineNum;         /* Line number of line being read [1]           */
   int ComLevel;	  /* Current comment nesting level [2]		  */
   NodePtr InDefMod;      /* Module node of current definition being read */
   StrPtr InDefFun;       /* Name of current definition                   */
   FILE *InFile;          /* File descriptor of file being read           */
   char InBuf[INBUFSIZE]; /* Buffer for current line being scanned        */
} InDesc;

/*
 * Footnotes
 *
 * [1] A line number of -1 indicates unnumbered lines, i.e. terminal input. 
 *
 * [2] ComLevel should always be zero outside of function "InBlanks".
 *     A non-zero value indicates an "open comment" error.
 */

extern StrPtr InString ();
extern char NodeDelim[];

/******************************* end of inob.h *******************************/

