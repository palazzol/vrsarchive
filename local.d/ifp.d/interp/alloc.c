
/****** alloc.c *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Dec 2, 1985          **/
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

/*
 * Storage is divided into 4 classes:
 *
 *      free storage
 *      list cells
 *      strings
 *      nodes descriptors 
 *  
 * Storage is allocated by pages.
 */

/*
 * Currently, the page table and descriptors don't do anything,
 * so we define them out of existence.  Their intended use t is to allow
 * reclamation of pages.
 */

#define PAGETABLE 0

#if PAGETABLE

#define FreePage 0     /* Defines for PageType field of PageDesc structure */
#define ListPage 1
#define StrPage 2
#define NodePage 3

typedef struct {
   char PageType;       /* Type of page.  See defines above */
   char *PageBase;      /* Base address of page             */
   unsigned PageLen;    /* Length of page in bytes          */
} PageDesc;

PageDesc PageTable [MaxPages];

int PageCount=0;

#endif /* PAGETABLE */

#if (OPSYS==UNIX || OPSYS==CTSS) 
#define MaxPages 256
#define SizeListPage (512 * sizeof (ListCell))
#define SizeStrPage  (512 * sizeof (StrCell))
#define SizeNodePage (256 * sizeof (NodeDesc))
#endif                   

#if OPSYS==MSDOS 
#define MaxPages 128
#define SizeListPage (256 * sizeof (ListCell))
#define SizeStrPage  (256 * sizeof (StrCell))
#define SizeNodePage (128 * sizeof (NodeDesc))
#endif

/*
 * AllocListPage
 *
 * Returns pointer to list of cells in new list page.
 * Each cell's value is initialized to NULL.
 *
 * NULL is returned if there are no more list pages available.
 */
ListPtr AllocListPage ()
   {
#if PAGETABLE
      register PageDesc *PDp;
#endif
      register ListPtr P;
      register int K;

      if (Debug & DebugAlloc) {
         LineWait ();
         printf ("AllocListPage ()\n");
         LineSignal ();
      }
#if PAGETABLE
      if (PageCount >= MaxPages) return NULL;
#endif
      P = (ListPtr) malloc (SizeListPage);
      if (P == NULL) return NULL;
#if PAGETABLE
      PDp = PageTable + PageCount++;
      PDp->PageType = ListPage;
      PDp->PageLen = SizeListPage;
      PDp->PageBase = (char *) P;
#endif
      P->Next = NULL;
      for (K = SizeListPage/(sizeof (ListCell));;) {
	 P->LRef = LRefOne;
	 P->Val.Tag = BOTTOM;
	 if (!--K) break;
	 P++;
	 P->Next = P-1;
      }
      return P; 
   }


StrPtr AllocStrPage ()
   {
#if PAGETABLE
      register PageDesc *PDp;
#endif
      register StrPtr P;
      register int K;

      if (Debug & DebugAlloc) printf ("AllocStrPage ()\n");
#if PAGETABLE
      if (PageCount >= MaxPages) return NULL;
#endif
      P = (StrPtr) malloc (SizeStrPage);
      if (P == NULL) return NULL;
#if PAGETABLE
      PDp = PageTable + PageCount++;
      PDp->PageType = StrPage;
      PDp->PageLen = SizeStrPage;
      PDp->PageBase = (char *) P;
      PDp = PageTable + PageCount++;
#endif
      P->StrNext = NULL;
      for (K = SizeStrPage/(sizeof (StrCell));;) {
	 P->StrChar [0] = '\0';
	 P->SRef = 1;
	 if (!--K) break;
	 P++;
	 P->StrNext = P-1;
      }
      return P;
   }

/*
 * AllocNodePage
 *
 * Returns pointer to list of nodes in new node page.
 */
NodePtr AllocNodePage ()
   {
#if PAGETABLE
      register PageDesc *PDp;
#endif
      register NodePtr P;
      register int K;

      if (Debug & DebugAlloc) printf ("AllocNodePage ()\n");
#if PAGETABLE
      if (PageCount >= MaxPages) return NULL;
#endif
      P = (NodePtr) malloc (SizeNodePage);
      if (P == NULL) return NULL;
#if PAGETABLE
      PDp = PageTable + PageCount++;
      PDp->PageType = NodePage;
      PDp->PageLen = SizeNodePage;
      PDp->PageBase = (char *) P;
      PDp = PageTable + PageCount++;
#endif
      P->NodeSib = NULL;
      for (K = SizeNodePage/(sizeof (NodeDesc));;) {
	 P->NRef = 1;
	 if (!--K) break;
	 P++;
	 P->NodeSib = P-1;
      };
      return P; 
   }

