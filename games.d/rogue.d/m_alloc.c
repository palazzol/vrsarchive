/*
 *	malloc - a memory allocator
 *
 *	  This is an emulation of the malloc library routine.  It is used on
 *	systems which put the stack between the data/bss and sbrk(0) so that
 *	save routines can write the area from 0 to _end as a very unportable
 *	save routine.
*/
#include <assert.h>
#include <stdio.h>

#ifndef M_NEEDED
#  define M_NEEDED 18000	/* You only get this much by default	*/
#endif


struct m_chunk {
  int  size;			/* Size, including OVERHEAD		*/
  struct m_chunk *next;		/* Pointer to next in free list		*/
  char data[M_NEEDED];
};

struct m_chunk m_buf = {
  M_NEEDED,			/* A very large chunk			*/
  NULL				/* Last on free list			*/
};
#define OVERHEAD (sizeof(struct m_chunk)-M_NEEDED)
struct m_chunk *m_free = &m_buf;

/*
 *	Invariants:
 *		m_head points to a linked list of m_chunks which are free.
 *		m_chunks in this list are sorted by address and no two chunks
 *		  have contiguous addresses.
 *		All chunks
 *	Notes:
 *		malloc returns (char *)&m_chunk.data for some m_chunk with
 *		length some multiple of OVERHEAD greater than or equal to the
 *		size the user requested.
*/
char *malloc(size)
unsigned size;
{ struct m_chunk *l, *p, *q; /* Search pointers				*/
  register int r;

  if (r = size % OVERHEAD)
    size += OVERHEAD - r;/* Round up to allocation granularity		*/
  size += OVERHEAD;	/* Size actually allocated			*/
  assert(size%OVERHEAD == 0);
  p = m_free;		/* Search for first fit				*/
  l = NULL;		/* No previous cell yet				*/
  while ((p != NULL) && (p->size < size)) {
    l = p;		/* Can't build it here				*/
    p = p->next;	/* Look at next cell				*/
  }
  if (p == NULL)
    return(NULL);
  /*
   *	This next test looks over defensive, but is not.  We always allocate
   *	in multiples of OVERHEAD, but we have probably started with an arena
   *	which is not a multiple.  That is, we do not require
   *		(M_NEEDED % OVERHEAD == 0)
   *	If we did, a simple (p->size > size) would do here.
  */
  if (p->size >= size+OVERHEAD) {
    q = (struct m_chunk *)((char *)p + size);
    q->size = p->size - size;
    q->next = p->next;
    p->size = size;
    p->next = q;
  }			/* Have split a large cell			*/
  if (l == NULL)	/* Allocated in first cell			*/
    m_free = p->next;	/* Just drop old first cell			*/
  else
    l->next = p->next;	/* Else drop cell from middle			*/
  return(p->data);
}

/*
 *	Invariants:
 *		m_head points to a linked list of m_chunks which are free.
 *		m_chunks in this list are sorted by address and no two chunks
 *		have contiguous addresses.
 *	Notes:
 *		free takes a pointer to some m_chunk's data area and coerces it
 *		to a pointer to the m_chunk, then links the m_chunk back into
 *		the list, coalescing any adjacent m_chunk.
*/
free(ptr)
char *ptr;
{ struct m_chunk *l, *p, *q; /* Search pointers				*/

  q = (struct m_chunk *)(ptr-OVERHEAD);
  assert(q >= &m_buf);
  assert(q <= &m_buf+1);/* This detects (crudely) an out of range free	*/
  p = m_free;
  l = NULL;
  while ((p != NULL) && (p < q)) {
    l = p;		/* Keep pointer behind				*/
    p = p->next;	/* Look for pointer ahead			*/
  }			/* Now l is NULL or behind q, p NULL or ahead	*/
  assert(l == NULL || l < q);
  assert(p == NULL || q < p);
  if (l == NULL) {	/* Need (l < q) and (l->next==q), (q->next==p)	*/
    q->next = m_free;
    l = m_free = q;
    q = q->next;
    p = q->next;
  } else {
    q->next = p;
    l->next = q;
  }
  if ((char *)q == (char *)l+l->size) {
    l->size += q->size;
    l->next = q->next;
  }
  if (p == NULL)	/* Nothing to coalesce ahead			*/
    return;
  if ((char *)p == (char *)q+q->size) {
    q->size += p->size;
    q->next = p->next;
  }
}
