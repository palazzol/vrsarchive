/*
 *	%W% %G%
 *	Internal allocation routines for memory within sockets.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <stdio.h>
/*
 *	_mbuf_init is called with the address of a shared data segment
 *	and initializes a socket's memory allocation structures.  This
 *	allows later calls to mbuf_alloc and mbuf_free to function
 *	correctly.
*/
void
_mbuf_init(seg)
struct socket *seg;
{ seg->so_free = offset(seg,so_mbuf);
  seg->so_mbuf.sm_next = 0;
  seg->so_mbuf.sm_length = SDSIZE - sizeof(*seg) + sizeof(struct mbuf);
}

#define _mbuf_at(seg,off)	((struct mbuf *)((char *)(seg)+(off)))
#define _mbuf_off(seg, buf)	((unsigned)((char *)buf - (char *)seg))

short
_mbuf_alloc(seg, count)
struct socket *seg;
int count;
{ short *mptr;				/* Pointer to offset of next	*/
  short *best;				/* Pointer to offset of best	*/
  struct mbuf *tmp;			/* Walk down free list		*/
  struct mbuf *new;			/* Use to split memory chunk	*/
  short length = 32767;			/* Length of best fit mbuf	*/
  short onext;				/* Temp during return		*/

  count += sizeof (struct mbuf);	/* Need this many bytes		*/
  mptr = &seg->so_free;			/* Start at head of free list	*/
  while (*mptr) {			/* While more segments		*/
    tmp = _mbuf_at(seg, *mptr);		/* Get mbuf for segment		*/
    if ((tmp->sm_length > count) && (tmp->sm_length < length)) {
      best = mptr;			/* New best fit			*/
      length = tmp->sm_length;		/* Keep this handy		*/
    }
    mptr = &tmp->sm_next;		/* Follow chain			*/
  }
  if (length == 32767)
    return(0);				/* No suitable mbuf was found	*/
  tmp = _mbuf_at(seg, *best);		/* Get the mbuf to use		*/
  if (length-count > sizeof(struct mbuf)) {
    /* 
     *	Split the region found into allocated and unallocated parts.
    */
    new = _mbuf_at(seg, *best+count);	/* Get pointer to new mbuf	*/
    new->sm_length = length-count;	/* Fill in length		*/
    new->sm_next = tmp->sm_next;	/* Inherit forward link		*/
    tmp->sm_length = count;		/* Old mbuf gets smaller	*/
    tmp->sm_next = *best+count;		/* Adjust forward link		*/
  }
  /*
   *	Unhook the memory at tmp and return it.
  */
  onext = tmp->sm_next;			/* Save forward link		*/
  tmp->sm_next = *best;			/* Allocated, point at yourself	*/
  *best = onext;			/* Delete tmp from free list	*/
  return(tmp->sm_next+sizeof(*tmp));	/* Return offset of data	*/
}

/*
 *	Invariants:
 *		The free list is sorted by address.
 *		No two entries in the free list are contiguous.
*/
void
_mbuf_free(seg, buf_off)
struct socket *seg;
short buf_off;
{ struct mbuf *bptr;
  struct mbuf *mptr;
  short *fptr;

  /*
   *	Find the mbuf associated with the free memory and insure that
   *	it was actually allocated.
  */
  buf_off -= sizeof(struct mbuf);
  bptr = _mbuf_at(seg, buf_off);
  if (bptr->sm_next != buf_off) {
    fprintf(stderr,"Internal error in socket: freeing unallocated memory\n");
    abort();
  }
  /*
   *	Find the right spot in the free list to insert the new memory.
  */
  fptr = &seg->so_free;
  while (*fptr) {
    if (*fptr > buf_off)
      break;
    mptr = _mbuf_at(seg, *fptr);
    fptr = &mptr->sm_next;
  }
  /*
   *	If possible, coalesce with prior segment.
  */
  if (fptr != &seg->so_free) {
    /*
     *	Set mptr to point at prior segment.
    */
    mptr = (struct mbuf *)((char *)fptr - offset(mptr, sm_next));
    if ((char *)mptr+mptr->sm_length == (char *)bptr) {
      mptr->sm_length += bptr->sm_length;
      return;
    }
  }
  /*
   *	Insert the new memory into the chain.  This may cause two adjacent
   *	entries in the free list (temporarily).
  */
  bptr->sm_next = *fptr;
  *fptr = buf_off;
  /*
   *	Now coalesce with subsequent segment if possible.  If inserting at
   *	end of free list, mptr == (struct mbuf *)seg.  This means the test
   *	below will not succeed, and all will be well.
  */
  mptr = _mbuf_at(seg, bptr->sm_next);
  if ((char *)bptr+bptr->sm_length == (char *)mptr) {
    bptr->sm_length += mptr->sm_length;
    bptr->sm_next = mptr->sm_next;
  }
}
