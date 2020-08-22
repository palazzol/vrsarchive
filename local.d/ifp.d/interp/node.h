 
/****** node.h ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  July 8, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

#ifndef INCLUDE_NODE_H
#define INCLUDE_NODE_H 1

/*
 * Define FETCH as 1 to define "fetch" (^k) functional form, 0 otherwise.
 * Define XDEF  as 1 to define "xdef"  ({...} f) functional form, 0 otherwise.
 */
#define FETCH 0
#define XDEF 1

extern ListPtr MakePath ();
extern NodePtr CopyNPtr (), FindNode ();
extern NodePtr MakeNode (), MakeChild (), PrimDef ();
extern NodePtr RootNode, SysNode, ArithNode, LogicNode;
extern void DelNPtr (), FormPath (), GroupDef (), LinkPath ();
void InitNode ();

typedef struct {        /* Used for node initialization tables */
   char *OpName;
   char OpParam;
   int (*OpPtr) ();     /* Actually void, but compiler complains about void */
} OpDef;                /* in static initializations of this structure      */

#define OpCount(OpTable) (sizeof(OpTable)/sizeof(OpTable[0])) 

extern NodePtr FormNode[];

/*
 * Subscripts for FormNode
 *
 * These must correspond to the entries in the FormOpTable in forms.c
 */
#define NODE_C          0
#define NODE_Comp       1
#define NODE_Cons       2
#define NODE_Each       3
#define NODE_Fetch	4
#define NODE_Filter     (4 + FETCH)
#define NODE_If         (5 + FETCH)
#define NODE_RInsert    (6 + FETCH)
#define NODE_Out        (7 + FETCH)
#define NODE_Sel        (8 + FETCH)
#define NODE_While      (9 + FETCH)
#define NODE_XDef	(9 + FETCH + XDEF) 
#define FORM_TABLE_SIZE (10 + FETCH + XDEF)


typedef struct {
   NodePtr FormNode;            /* Node pointer for form */
   char *FormInPrefix;
   OpDef FormOp;
   char *FormComment;		/* Comment for `expected' error message */
} FormEntry;

extern FormEntry FormTable[FORM_TABLE_SIZE];

#endif

/****************************** end of node.h ******************************/

