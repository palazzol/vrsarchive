#include <sys/brk.h>

extern	char	*brkctl();
/*
 *	brkctl -- data allocation operations
 *
 *	char *bas;
 *	bas = brkctl(cmd, incr, segbas)
 *	int cmd;
 *	long incr;
 *	char *segbas;
 *
 *	command		incr	seg	action
 *	BR_ARGSEG	0	NULL	report on last seg
 *	BR_ARGSEG	other	other	incr specified seg
 *	BR_NEWSEG	0	-	alloc new seg, size 0
 *	BR_NEWSEG	other	-	alloc new seg, specified incr
 *	BR_IMPSEG	0	-	report on last seg; may free up
 *					empty seg(s).
 *	BR_IMPSEG	other	-	incr last seg; may alloc new
 *					seg(s).
 *
 *	In all cases, the return value points to the base of the
 *	effected region on success, is (char *) -1 on error.
*/

/*
 * Storage allocator
 *
 *	malloc, realloc, free - modified to put each chunk in a seperate
 *	segment.  This means that code that would corrupt the arena of the
 *	regular routines will drop dead at the point of error, not 30 routines
 *	later.
 *
*/
#define NULL	0
#define ERROR	((Header *)-1)

struct header {	/* free block header */
	struct {
		unsigned size;		/* size of this free block	*/
		struct header *ptr;	/* next free block		*/
	} s;
};
typedef struct header Header;

/*static*/ Header *freelist = 0;		/* List of reusable segments	*/

char *
malloc(nbytes)	
unsigned nbytes;
{
	char *brkctl();
	register Header *p;

	nbytes += sizeof *p;		/* Allocate header + data	*/
	if(nbytes < nbytes-sizeof(*p))
		return((char *) NULL);
	if (freelist == NULL)		/* Create segment big enough	*/
		p = (Header *)brkctl(BR_NEWSEG,(long)nbytes,p);
	else {
		p = freelist;
		freelist = p->s.ptr;	/* Pop from freelist		*/
		p = (Header *)brkctl(BR_ARGSEG,(long)nbytes-sizeof(*p),p);
					/* Grow to new size		*/
	}
	p->s.ptr = p;
	p->s.size = nbytes;
	return(p == ERROR? NULL : (char *)(p+1));
}

/*
 * free memory from previous malloc or realloc
*/
free(ap)
char *ap;
{
	register Header *p;
	register Header *q;

	p = (Header *)ap - 1;
	/*
	 *	  Sanity check to make sure that we are actually freeing a
	 *	block that we gave away before.  If not, abort() so the user
	 *	knows a mistake was made.
	 * 
	 */
	if (p->s.ptr != p)
		abort();
	q = (Header *)brkctl(BR_ARGSEG,(long)sizeof(Header)-p->s.size,p);
					/* Shrink to just a header	*/
	if (q-1 != p)
		abort();		/* brkctl failed		*/
	p->s.ptr = freelist;
	freelist = p;			/* Link into freelist		*/
}

/*
 *	realloc - Increases the size of the allocated segment.  We never have
 *	to do any moves, thanks to the wonders of the architecture.
*/
char *
realloc(oldmem, newsize)
char *oldmem;
unsigned newsize;
{
	register Header *p;
	int oldsize;

	p = ((Header *)oldmem - 1);
	if (p->s.ptr != p)
		abort();
	newsize += sizeof *p;		/* Allocate header + data	*/
	if ((int)newsize <= 0)
		return((char *) NULL);
	oldsize = p->s.size;
	p = (Header *)brkctl(BR_ARGSEG,(long)newsize-oldsize,p);
					/* Adjust segment size		*/
	return(oldmem);
}
