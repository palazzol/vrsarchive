/*
 * sortlib.c
 * sorting subroutines
 *
 * Copyright (c) 1990 by Craig Counterman. All rights reserved.
 *
 * This software may be redistributed freely, not sold.
 * This copyright notice and disclaimer of warranty must remain
 *    unchanged. 
 *
 * No representation is made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty, to the extent permitted by applicable law.
 *
 */

#ifndef  lint
static char rcsid[] =
  "$Header: /home/Vince/cvs/net/starchart/observe/sortlib.c,v 1.1 1990-03-30 16:38:02 vrs Exp $";
#endif


#include <math.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

void HeapSort();

void HeapSort0(darray, iarray, n)
     double darray[];
     int iarray[];
     int n;
{

  darray--; /* make darray[0] look like darray[1] */
  iarray--; /* make iarray[0] look like iarray[1] */
  HeapSort(darray, iarray, n);
}


/* Heap sort algorithm
   Sorts a double array darray of length n into ascending numerical order,
   indexed by iarray */

void HeapSort(darray, iarray, n)
     double darray[];
     int iarray[];
     int n;
{
  int i, j;
  int l, ir, indxt;
  double q;



                /* Initialize the index array */
  for (i = 1; i <= n; i++)
    iarray[i] = i;

  l = n/2 + 1;
  ir = n;
  /* The index l will be decremented from its initial value down to 1 during
     the hiring (heap creation) phase.  Once it reaches 1, the index ir will
     be decremented from its initial value down to 1 during the
     "retirement-and-promotion" (heap selection) phase.
  */

  while(TRUE) {			    /* Will exit with a return; */
    if (l > 1) {		    /* Still in hiring phase. */
      l--;
      indxt = iarray[l];
      q = darray[indxt];
    } else {			    /* In retirement and promotion phase */
      indxt = iarray[ir];
      q = darray[indxt];	    /* Clear a space at the top of the array */
      iarray[ir] = iarray[1];	    /* Retire the top of the heap into it */
      ir--;			    /* Decrease the size of the corporation */
      if (ir == 1) {		    /* Done with last promotion */
	iarray[1] = indxt;	    /* The least competent worker of all! */
	for (i = 1; i <= n; i++)    /* Reset iarray for C offset */
	  iarray[i]--;
	return;			    /* Done */
      }
    }
    i = l;			    /* Here set up to sift down element q */
    j = l+l;
    while (j <= ir) {
      if (j < ir)		    /* Compare to better underling */
	if (darray[iarray[j]] < darray[iarray[j+1]]) j++;

      if (q < darray[iarray[j]]) {  /* Demote q */
	iarray[i] = iarray[j];
	i = j;
	j = j+j;
      } else {
	j = ir+1;		    /* This is q's level, set j to terminate */
      }				    /*   sift down */
    }
    iarray[i] = indxt;		    /* Put q in its slot */
  }
}



