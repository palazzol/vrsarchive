
/****** node.c ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Nov 23, 1985          **/
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
#include "node.h"
#include "umax.h"
#include "string.h"

/********************************* NODE RULES ******************************

Function definitions are stored in nodes, which are arranged in a tree
structure mimicking the UNIX file structure.  Below is an example:

		   Rm
		   |
		   Am---Bi----Cm-------Dd
		   |          |
		   Xd         Yd--Zd

Rm is the root node, with children Am,Bi,Cm, and Dd. Nodes can be one of three
types: module (m), import (i), or definition (d).  Only definition nodes
have a reference count greater than 1.  Only module nodes have children.

 ****************************** end of node rules **************************/

NodePtr RootNode,SysNode,LogicNode,ArithNode;

/* Free nodes have NREF == 0 and are linked by NodeSib field */
NodePtr FreeNode = NULL;

/*
 * DelNPtr
 *
 * Note: node pointers always have a parent pointer to them, so
 *       we don't have to delete them here.
 *
 * Input
 *	N = pointer to node
 */
void DelNPtr (N)
   NodePtr N;
   {
      rsemaphore_enter (NRefSemaphore);
      if (N != NULL) N->NRef--;
      rsemaphore_exit (NRefSemaphore);
   }


/*
 * CopyNPtr
 */
NodePtr CopyNPtr (N)
   NodePtr N;
   {
      rsemaphore_enter (NRefSemaphore);
      if (N != NULL && !++N->NRef) IntError ("CopyNPtr: too many refs");
      rsemaphore_exit (NRefSemaphore);
      return N;
   }
    

/*
 * NewNode
 *
 * Point *N to new node from free list.  The input value of *N is
 * put in the NodeSib field of the new node.
 *
 * A SysError may occur, in which case *N is unchanged.
 */
private void NewNode (N)
   NodePtr *N;
   {
      extern NodePtr AllocNodePage ();
      register NodePtr T;

      rsemaphore_enter (NRefSemaphore);
      if (FreeNode == NULL && (FreeNode = AllocNodePage ()) == NULL) {
	 printf ("NO MORE NODE CELLS LEFT\n");
	 SysError = NO_NODE_FREE;
      } else {
	 T = FreeNode;
	 FreeNode = FreeNode->NodeSib;
	 T->NodeSib = *N;
	 *N = T;
      }
      rsemaphore_exit (NRefSemaphore);
   }
    

/*
 * FindNode
 *
 * Find a node within a module with a specified name.
 *
 * Input
 *      M = pointer to module node
 *      S = pointer to string
 *
 * Output
 *      result = NULL if node not found, pointer to node otherwise
 */
NodePtr FindNode (M,S)
   register NodePtr M;
   StrPtr S;
   {
      if (M->NodeType == MODULE)
	 for (M = M->NodeData.NodeMod.FirstChild; M!=NULL; M=M->NodeSib)
	    if (0==StrComp (M->NodeName,S)) return M;
      return NULL;
   }
    

/*
 * MakePath
 *
 * Make the path list for a given node
 *
 * Input
 *      *N = module node
 * Output
 *      *result = path list
 */
ListPtr MakePath (N)
   NodePtr N;
   {
      ListPtr P;

      rsemaphore_enter (NRefSemaphore);
      P = NULL;
      while (N->NodeParent != NULL) {
	 NewList (&P,1L);
	 P->Val.Tag = STRING;
	 P->Val.String = CopySPtr (N->NodeName);
	 N = N->NodeParent;
      }
      rsemaphore_exit (NRefSemaphore);
      return P;
   }


/*
 * MakeChild
 *
 * Find (or create if necessary) a new child node with a specified name.
 *
 * Input
 *    M = Parent node
 *    S = name of child
 *
 * Output
 *    N = pointer to child
 *
 * A SysError may occur.
 */
NodePtr MakeChild  (M,S)
   NodePtr M;
   StrPtr S;
   {
      register NodePtr N;

      rsemaphore_enter (NRefSemaphore);
      N = FindNode (M,S);
      if (N==NULL) {
	 NewNode (&M->NodeData.NodeMod.FirstChild);
	 if (SysError) {
	    N = NULL;
	    goto exit;
	 }
	 N = M->NodeData.NodeMod.FirstChild;
	 N->NodeParent = M;
	 N->NodeName = CopySPtr (S);
	 N->NodeType = NEWNODE;
      }
exit:
      rsemaphore_exit (NRefSemaphore);
      return N;
   }

/*
 * Initialize a module node
 *
 * Input
 *      M = pointer to new node
 */
void InitModule (M)
   register NodePtr M;
   {
      M->NodeType = MODULE;
      M->NodeData.NodeMod.FirstChild = NULL;
      ReadImport (M);
   }

/*
 * MakeNode
 *
 * Create all nodes required by a path.
 *
 * Input
 *      Path = pointer to path list
 *      Type = type to make node if new node
 * Output
 *      result = pointer to node specified by path or
 *               NULL if an error occurred.
 */
NodePtr MakeNode (Path,Type)
   ListPtr Path;
   int Type;
   {
      register NodePtr M;
      register ListPtr P;

      rsemaphore_enter (NRefSemaphore);
      M = RootNode;
      for (P=Path; P != NULL; P=P->Next)
	 if (P->Val.Tag != STRING) return NULL;
	 else {
	    M = MakeChild (M,P->Val.String);
	    if (M->NodeType == NEWNODE)
	       if (P->Next!=NULL) InitModule (M);
	       else
		  switch (M->NodeType = Type) {
		     case DEF:
			M->NodeData.NodeDef.DefCode.Tag = BOTTOM;
			M->NodeData.NodeDef.DefFlags = 0;
			break;
		     case MODULE:
			InitModule (M);
			break;
		  }
	 }
      rsemaphore_exit (NRefSemaphore);
      return M;
   }


/*
 * DelImport
 *
 * Delete all information affected by the %IMPORT file for a module node
 * in preparation for rereading the %IMPORT file.
 *
 * Input
 *      M = pointer to module node
 *
 * Notes
 *      IMPORT nodes can be returned to the free list since their
 *      reference counts are always 1.
 */
void DelImport (M)
   NodePtr M;
   {
      register NodePtr *L;
      register NodePtr N;

      rsemaphore_enter (NRefSemaphore);
      for (L = &M->NodeData.NodeMod.FirstChild; (N = *L)!= NULL; )

	 switch (N->NodeType) {
	
	    case IMPORT:        /* Return IMPORT nodes to free list */
	       DelSPtr (N->NodeName);
	       RepTag (&N->NodeData.NodeImp.ImpDef,BOTTOM);
	       Rot3 ((MetaPtr) &FreeNode, (MetaPtr) L, (MetaPtr) &N->NodeSib);
	       break;

	    case DEF:           /* Delete local function definitions */
	       if (N->NodeData.NodeDef.DefCode.Tag != CODE) 
		  RepTag (&N->NodeData.NodeDef.DefCode,BOTTOM);
	       L = &N->NodeSib;
	       break;

	    case MODULE:
	       L = &N->NodeSib;
	       break;

	    default:
	       printf ("Invalid NodeType in node tree: %d\n",N->NodeType);
	       L = &N->NodeSib;
	       break;
	 }
      rsemaphore_exit (NRefSemaphore);
   }


/*
 * LinkPath
 *
 * Convert a path list to a node if possible.
 *
 * Input
 *      *Def = path list
 *      Type = NodeType value if new node
 *
 * Output
 *      *Def = node or not changed if error occurs
 */
void LinkPath (Path,Type)
   ObjectPtr Path;
   int Type;
   {
      register NodePtr N;

      rsemaphore_enter (NRefSemaphore);
      N = MakeNode (Path->List,Type);
      if (N != NULL) {
	 RepTag (Path,NODE);
	 Path->Node = CopyNPtr (N);
      }
      rsemaphore_exit (NRefSemaphore);
   }

/*
 * SignExtend
 *
 * Sign extend a byte.  Not all machines have signed characters.
 */    
#define SignExtend(B) ((((B) + 0x80) & 0xFF) - 0x80)

/*
 * PrimDef
 *
 * Define a primitive function
 *
 * Input
 *      *F = object code for function
 *      S = name of function
 *      M = module to put function in
 *      K = code parameter value
 *
 * Output
 *      result = pointer to node containing function
 */
/* VARARGS3 */
NodePtr PrimDef (F,S,M,K)
   int (*F) ();
   char *S;        
   NodePtr M;
   char K;
   {
      register NodePtr N;
      StrPtr T;
      T = MakeString (S);
      N = MakeChild (M,T);
      N->NodeType = DEF;
      N->NodeData.NodeDef.DefCode.Tag = CODE;
      N->NodeData.NodeDef.DefFlags = 0;
      N->NodeData.NodeDef.DefCode.Code.CodePtr = F;
      N->NodeData.NodeDef.DefCode.Code.CodeParam = SignExtend (K);
      DelSPtr (T);
      return N;
   }


/*
 * GroupDef
 *
 * Define a group of functions
 *
 * Input
 *     T = pointer to table of functions
 *     N = number entries in table
 *     M = module node
 */
void GroupDef (T,N,M)
   register OpDef *T;
   int N;
   NodePtr M;
   {
      while (--N >= 0) 
	 (void) PrimDef (T->OpPtr,T->OpName,M,T->OpParam),
	 T++;
   }


/*
 * Initialize root node and 'sys' subnode.
 */
void InitNode ()
   {
      register NodePtr R;

      if (Debug & DebugInit) printf ("enter InitNode\n");
      RootNode = NULL;
      NewNode (&RootNode);
      R = RootNode;
      R->NodeSib = NULL;
      R->NodeParent = NULL;
      R->NodeType = MODULE;
      R->NodeName = MakeString ("ROOT");
      R->NodeData.NodeMod.FirstChild = NULL;
      SysNode = MakeChild (R,MakeString ("sys"));
      InitModule (SysNode);
      R = MakeChild (R,MakeString ("math"));
      InitModule (R);
      ArithNode = MakeChild (R,MakeString ("arith"));
      InitModule (ArithNode);
      LogicNode = MakeChild (R,MakeString ("logic"));
      InitModule (LogicNode);
      if (Debug & DebugInit) printf ("exit InitNode\n");
   }

/****************************** end of node.c ******************************/
