
/****** list.c ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Jan 15, 1986          **/
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
#include "stats.h"

/* 
 * FreeList
 *
 * ListCells in free-list always contain:
 *
 *      LRef == LRefOne 
 *      Val.Tag == BOTTOM
 *      Next == pointer to next cell in free list.
 */
ListPtr FreeList = NULL;
#define LRefAdd(P,Delta) ((P)->LRef+=(Delta))

/*************** Fundamental List Manipulation Routines ***************/

private ListPtr FixCopyLPtr ();         /* forward reference */

/*
 * Rot3
 */
void Rot3 (A,B,C)
   MetaPtr A,B,C;
   {
      register ListPtr P;
      P = *A; *A = *B; *B = *C; *C = P;
   }

/*
 * ListLength
 *
 * Input
 *      P = pointer to list
 *
 * Output
 *      result = length of list
 */
long ListLength (P)
   register ListPtr P;
   {
      register long N;
      for (N=0; P!=NULL; P=P->Next) N++;
      return N;
   }

/*
 * CopyObject
 *
 * Copy object: X := Y
 *
 * A SysError may occur.
 */
void CopyObject (X,Y)
   ObjectPtr X,Y;
   {
      register ListPtr P;

      switch (X->Tag = Y->Tag) {
	 case BOTTOM: break;
	 case BOOLEAN: X->Bool   = Y->Bool;              break;
	 case INT:     X->Int    = Y->Int;               break;
	 case FLOAT:   X->Float  = Y->Float;             break;
	 case LIST:
	     /* CopyLPtr expanded inline for speed */
	     P = Y->List;
	     if (P!=NULL && LRefAdd (P,1) == LRefOne-1) 
		/*
		 * This won't work for multiprocessor version
		 * since other processors will not detect overflow.
		 */
		P = FixCopyLPtr (P);
	     X->List = P;
	     break;
	 case STRING:  X->String = CopySPtr (Y->String);        break;
	 case NODE:    X->Node   = CopyNPtr (Y->Node);  	break;
      }
   }

/*
 * NewList
 *
 * Point *A to list of N cells with last cell's Next set to old value of *A.
 *
 * Each cell value is set to BOTTOM
 *
 * A SysError may occur, in which case *A remains unchanged.
 *
 * Implementation note: 
 *     (x >= 0) is faster than (x > 0) on 16-bit machines since only
 *     the sign bit must be checked.
 */
void NewList (A,N)
   MetaPtr A;
   register long N;
   {
      extern ListPtr AllocListPage ();
      register MetaPtr B;
      ListPtr P;

      Stat (StatNewList (N));
      if (--N >= 0) {
	 B = &FreeList;
	 do {
	    if (*B == NULL && (*B = AllocListPage ()) == NULL) {
	       SysError = NO_LIST_FREE;
	       printf ("NO MORE LIST CELLS LEFT\n");
	       return;
	    }
	    B = &(*B)->Next;
	 } while (--N >= 0);
	 P = FreeList;
	 FreeList = *B;
	 *B = *A;
	 *A = P;
      }
   }

/*
 * Repeat
 *
 * Create a new list containing N copies of an object
 *
 * Output
 *      result = pointer to list
 *
 * A SysError may occur, in which case NULL is returned.
 */
ListPtr Repeat (X,N)
   register ObjectPtr X; 
   long N;
   {
      ListPtr P=NULL;
      register ListPtr Q;

      NewList (&P,N);
      if (!SysError)
	 for (Q=P; Q!=NULL; Q=Q->Next) 
	    CopyObject (&Q->Val,X); 
      return P;
   }

/*
 * DelLPtr
 *
 * Delete a list pointer: decrement reference count and return to free-list
 *                        if not used anymore.
 *
 * Routine is "vectorized" in that it is optimized to return long lists
 * to the freelist.
 */
void DelLPtr (P)
   register ListPtr P;
   {
      register ListPtr Q,R;

      Stat (StatDelLPtr (P));

      for (R=P; R!=NULL; R=R->Next) {
         if (R->LRef != LRefOne) {
	    R->LRef--;
	    break;
	 }
	 if (!Scalar (R->Val.Tag)) {
	    switch (R->Val.Tag) {
	       case LIST:     DelLPtr (R->Val.List);     break;
	       case STRING:   DelSPtr (R->Val.String);   break;
	       case NODE:     DelNPtr (R->Val.Node);     break;
	    }
	    R->Val.Tag = BOTTOM;
	 }
	 Q = R;
      }
      if (R != P) {
	 Q->Next = FreeList; 
	 FreeList = P;
      }
   }

/*
 * CopyLPtr
 *
 * Make a copy of a list pointer, incrementing the reference count.
 * If the reference count would overflow, a new list cell is generated.
 *
 * A SysError may occur, in which case the result is NULL.
 */
ListPtr CopyLPtr (P)
   ListPtr P;
   {
      if (P!=NULL) {
         if (LRefAdd (P,1) == LRefOne-1) {
            return FixCopyLPtr (P);
         }
      }
      return P;
   }

/*
 * FixCopyLPtr 
 * 
 * Copy a list pointer which overflowed.
 *
 * Input
 *	P = pointer to list cell
 */
private ListPtr FixCopyLPtr (P)
   ListPtr P;
   {
      ListPtr Q;                        /* Reference count overflowed */

      LRefAdd (P,-1);
      Q = CopyLPtr (P->Next);
      if (SysError) return NULL;
      NewList (&Q,1L);
      if (SysError) return NULL;
      CopyObject (&Q->Val,&P->Val);
      return Q;
   }

/*
 * RepTag
 *
 * Replace an object tag with another tag.
 */
void RepTag (Dest,NewTag)
   ObjectPtr Dest;
   char NewTag;
   {
      switch (Dest->Tag) {
	 case LIST:     DelLPtr (Dest->List);     break;
	 case STRING:   DelSPtr (Dest->String);   break;
	 case NODE:     DelNPtr (Dest->Node);     break;
	 /* default: break; */
      }
      Dest->Tag = NewTag;
   }

/*
 * RepBool
 *
 * Replace an object with a boolean object
 */
void RepBool (Dest,Value)
   ObjectPtr Dest;
   boolean Value;
   {
      RepTag (Dest,BOOLEAN);
      Dest->Bool = Value;
   }

/*
 * RepObject
 *
 * Replace an Object by another Object.
 *
 * A SysError may occur.
 */
boolean RepObject (Y,X)
   register ObjectPtr Y,X;
   {
      Object Z;

      switch (Z.Tag = Y->Tag) {
	 case LIST:   Z.List   = Y->List;   break;
	 case STRING: Z.String = Y->String; break;
	 case NODE:   Z.Node   = Y->Node;   break;
      }
      switch (Y->Tag = X->Tag) {
	 case BOTTOM:    break;
	 case BOOLEAN:   Y->Bool   = X->Bool;              break;
	 case INT:       Y->Int    = X->Int;               break;
	 case FLOAT:     Y->Float  = X->Float;             break;
	 case LIST:      Y->List   = CopyLPtr (X->List);   break;
	 case STRING:    Y->String = CopySPtr (X->String); break;
	 case NODE:      Y->Node   = CopyNPtr (X->Node);   break;
      }
      switch (Z.Tag) {
	 case LIST:   DelLPtr (Z.List);   break;
	 case STRING: DelSPtr (Z.String); break;
	 case NODE:   DelNPtr (Z.Node);   break;
      }
   }


/*
 * RepLPtr
 *
 * Replace pointer variable *A by value B.
 *
 * A SysError may occur, in which case *A remains unchanged.
 */
void RepLPtr (A,P)
   MetaPtr A; 
   ListPtr P;
   {
      P = CopyLPtr (P); /* Copy P first so DelLPtr can't trash *P */
      if (SysError) return;
      DelLPtr (*A);
      *A = P;
   }


/*
 * MakeCopy
 *
 * Make a copy of a non-empty list.
 *
 * Input
 *      P = pointer to list
 *
 * Output
 *      *A = pointer to identical list with LRef == LRefOne
 *      result = metapointer to Next field of end of result list
 *
 * A SysError may occur, in which case *A remains unchanged.
 *
 * All sublist-head reference-counts are incremented if no error occurs.
 */
MetaPtr MakeCopy (A,P)
   register ListPtr *A,P;
   {
      register ListPtr Q;
      ListPtr R=NULL;		 /* R = root of new list */

      NewList (&R,ListLength (P));
      if (SysError) return NULL;

      Q = R;
      while (1) {
	 if (Scalar (P->Val.Tag)) {
	    Q->Val.Data = P->Val.Data;
	    Q->Val.Tag  = P->Val.Tag;
	 } else {
	    CopyObject (& Q->Val,& P->Val);
	    if (SysError) {DelLPtr (R); return NULL;};
	 }
	 P = P->Next;
	 if (P == NULL) break;
	 Q = Q->Next;
      };

      *A = R;
      return &Q->Next;
   }


/*
 * CopyTop
 *
 * Replace *A with a pointer to a fresh (top level) copy of *A.
 *
 * Input
 *      *A = pointer to list
 * Output
 *      *A = pointer to identical list with LRef == LRefOne for top level
 *
 * A SysError may occur, in which case *A remains unchanged.
 */
void CopyTop (A)
   register MetaPtr A;
   {
      register ListPtr P;

      while (1) {                 /* Search for shared part of list */
	 P = *A;
	 if (P == NULL) return;
	 if (P->LRef != LRefOne) break;
	 Stat (StatRecycle++);
	 A = & P->Next;
      }

      (void) MakeCopy (A,P);
      P->LRef--;
      if (SysError) (*A)->LRef++;
   }


/*
 * Copy2Top
 *
 * Replace *A with a pointer to a fresh (top 2 levels) of *A.
 *
 * Input
 *      *A = pointer to list
 * Output
 *      *A = pointer to identical list with LRef == LRefOne
 *           for both top level and any immediate sublists.
 *
 * A SysError may occur, in which case *A remains unchanged.
 */
void Copy2Top (A)
   register MetaPtr A;
   {
      register ListPtr P;

      while (1) {                 /* Search for shared part of list */
	 P = *A;
	 if (P == NULL) return;
	 if (P->LRef != LRefOne) break;
	 if (P->Val.Tag == LIST) {
	    CopyTop (&P->Val.List);
	    if (SysError) return;
	 }
	 Stat (StatRecycle++);
	 A = & P->Next;
      }

      /* (*A) now points to shared list */

      (void) MakeCopy (A,(P = *A));

      if (SysError) return;
      P->LRef--;
      P = *A;

      do
	 if (P->Val.Tag == LIST && *(A = &P->Val.List) != NULL) {
	    /*
	     * There must some more elegant way to efficiently merge these 
	     * two cases.
	     */
	    (*A)->LRef--; /* will be incremented by MakeCopy */
            (void) MakeCopy (A,*A);
	    if (SysError) return;
	 }
      while ((P=P->Next) != NULL);
   }


/****************************** end of list.c ******************************/

