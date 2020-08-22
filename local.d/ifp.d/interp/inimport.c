
/****** inimport.c ****************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Oct 28, 1985          **/
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
#include "node.h"
#include "string.h"
#include "inob.h"

/*
 * DoubleDot
 *
 * Append a ".." to path list by deleting last element.
 *       
 * Input
 *      *F = file descriptor
 *      *C = pointer to path list 
 *
 * Output
 *      result = pointer to last null field, null if error.
 */
MetaPtr DoubleDot (F,C)           
   InDesc *F;
   register MetaPtr C;
   {
      register MetaPtr A;

      if (*C == NULL) {
	 (void) InError (F,"Too many ..'s.");
	 return NULL;
      } else {        /* Remove last element from path list R */
	 do {
	    A = C;
	    C = &(*A)->Next;
	 } while (*C != NULL);
	 DelLPtr (*A);
	 *A = NULL;
	 return A;
      }
   }

/*
 * NodeDelim is the set of pathname delimiters.  Note that '>' and '<' are not
 * in the set since they are (perversely) legal function names.  
 */
char NodeDelim[] = " ,[](){}|;:/\t\n";

/*
 * InNode
 *
 * Input a path.  A path may represent a module, function, or functional
 * variable.  Local functions are linked if possible to save time and space.
 *
 * The EBNF production definition for a node is:
 *
 *	["/"] string { "/" (string | "..") }
 *
 * Input
 *	*F = input descriptor pointing to path
 *	Env = environment
 *
 * Output
 *      InOut = node (path list or node format) or functional variable (string)
 *	*F = input descriptor pointing to next token after path
 *
 * A SysError may occur, in which case InOut is unchanged.
 */
boolean InNode (F,InOut,Env)
   InDesc *F;
   ObjectPtr InOut;
   ListPtr Env;
   {
      ListPtr R = NULL;        /* path list accumulator                       */
      register MetaPtr A = &R; /* pointer to Next field at end of accumulator */
      register NodePtr N;
      boolean FirstSlash;
   
      if (Debug & DebugParse) printf ("InNode: '%s'",F->InPtr); 
      if (!(FirstSlash = *F->InPtr == '/')) {

	 if (IsTok (F,"..")) {
	    if (F->InDefMod != NULL) R = MakePath (F->InDefMod);
	    if (NULL == (A = DoubleDot (F,&R))) goto Error;
	 } else {

	    Object S; 				      /* relative path */
	    S.Tag = BOTTOM;
	    if (NULL == InString (F,&S,NodeDelim,0)) {
	       if (!SysError) (void) InError (F,"path expected");
	       goto Error;
	    }	
	    if (!IsTok (F,"/")) {

	       for (; Env!=NULL; Env=Env->Next) 
		  if (ObEqual (&Env->Val,&S)) {
		     RepObject (InOut,&Env->Val);     /* functional variable */
		     return 1;
		  }

	       N = FindNode (F->InDefMod,S.String);   /* local function */
	       if (N != NULL) {
		  if (N->NodeType == IMPORT) {

		     /* Imported function - resolve alias */
		     RepObject (InOut,&N->NodeData.NodeImp.ImpDef);

		  } else { /* Local function already linked */
     
		     RepTag (InOut,NODE);
		     InOut->Node = CopyNPtr (N);
		  }
		  RepTag (&S,BOTTOM);
		  return 1;
	       }
	    }
	    if (F->InDefMod != NULL) R = MakePath (F->InDefMod);
	    while (*A != NULL) A = &(*A)->Next;
	    NewList (A,1L);
	    (*A)->Val.Tag = STRING;
	    (*A)->Val.String = S.String;
	 }
      }

      while (IsTok (F,"/")) {
	 if (IsTok (F,".."))
	    if (NULL == (A = DoubleDot (F,&R))) return 0;
	    else continue;
	 else {
	    NewList (A,1L);
	    if (SysError) goto Error;
	    if (NULL == InString (F,&(*A)->Val,NodeDelim,0)) {
	       if (SysError) goto Error;
	       else if (*F->InPtr != '/' && FirstSlash) {
		  (void) DoubleDot (F,&R);
		  break;
	       } else {
		  (void) InError (F,"Invalid path name");
		  goto Error;
	       }
	    }
	    A = &(*A)->Next;
	 }
	 FirstSlash = 0;
      }

      RepTag (InOut,LIST);
      InOut->List = R;
      return 1;

Error:
      DelLPtr (R);
      return 0;
   }

/*
 * InImport
 *
 * Input from an import file.
 *
 * An import file has the following format:
 *
 *      { 'FROM' path 'IMPORT' string {,string} ';' }
 *
 * Input
 *      F = input
 *      M = pointer to module node
 */
void InImport (F,M)
   register InDesc *F;
   register NodePtr M;
   {
      Object Path,Def;
      register NodePtr N;
      MetaPtr A;

      F->InDefFun = NULL;
      Path.Tag = BOTTOM;
      Def.Tag = BOTTOM;

      while (*F->InPtr) {

	 if (!IsTok (F,"FROM")) {
	    (void) InError (F,"FROM expected");
	    break;
	 }

	 (void) InNode (F,&Path,NIL); 
	 if (!IsTok (F,"IMPORT")) {
	    (void) InError (F,"IMPORT expected");
	    break;
	 }

	 while (1) {

	    if (NULL == InString (F,&Def," ,;\n",0)) {
	       if (!SysError) (void) InError (F,"function name expected");
	       goto Return;
	    }

	    N = MakeChild (M,Def.String);

	    switch (N->NodeType) {

	       case IMPORT:
		  (void) InError (F,"duplicate imported identifier");
		  break;

	       case DEF:
		  if (N->NRef > 1) {
		     (void) InError (F,"identifies function elsewhere");
		     break;
		  } /* else continue on down to NEWNODE */

	       case NEWNODE: {
		  extern MetaPtr MakeCopy ();
		  N->NodeType = IMPORT;
		  N->NodeData.NodeImp.ImpDef.Tag = LIST;
		  A = MakeCopy (&N->NodeData.NodeImp.ImpDef.List, Path.List);
		  NewList (A,1L);
		  RepObject (&(*A)->Val,&Def);
		  break;
	       }
	    }
		
	    if (IsTok (F,";")) break;
	    if (!IsTok (F,",")) {
	       (void) InError (F,"comma or semicolon expected");
	       goto Return;
	    }
	 }
      }
Return:
      RepTag (&Path,BOTTOM);
      RepTag (&Def,BOTTOM);
      return;
   }


/******************************* inimport.c *******************************/

