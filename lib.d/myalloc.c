/*
 * Storage allocator
 *
 *	malloc, realloc, free - modified to put each chunk in a seperate
 *	segment.  This means that code that would corrupt the arena of the
 *	regular routines will drop dead at the point of error, not 10 minutes
 *	later.
 *
*/
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
#define NULL	0
#define ERROR	((Header *)-1)
#define	MAGIC_NO 0x5a6b		/*
				 * Magic number for free blocks that are
				 * unlinked.  Helps protect from arena
				 * corruption.
				*/

typedef int Align;		/* Fudge factor to force alignment	*/
struct header {	/* free block header */
	struct {
		unsigned size;		/* size of this free block	*/
		struct header *ptr;	/* next free block		*/
	} s;
	Align x;		/* Force alignment of blocks		*/
};
typedef struct header Header;

char *
malloc(nbytes)	
unsigned nbytes;
{
	char *brkctl();
	register Header *p;

	nbytes += sizeof *p;		/* Allocate header + data	*/
	if((int)nbytes <= 0)
		return((char *) NULL);
	p = (Header *)brkctl(BR_NEWSEG,(long)nbytes,p);
					/* Create segment big enough	*/
	p->s.ptr = (Header *)MAGIC_NO;
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

	p = (Header *)ap - 1;
	/*
	 *	  Sanity check to make sure that we are actually freeing a
	 *	block that we gave away before.  If not, abort() so the user
	 *	knows a mistake was made.  This also cehcks for segments whose
	 *	first few bytes (before those the user was given) have been
	 *	trashed.
	*/
	if (p->s.ptr != (Header *)MAGIC_NO) {
		abort();
		return;
	}
	p = (Header *)brkctl(BR_ARGSEG,(long)-p->s.size,p);
					/* Make segment empty		*/
	p = (Header *)brkctl(BR_IMPSEG,(long)0,p);
					/* Free up empty segments	*/
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

	newsize += sizeof *p;		/* Allocate header + data	*/
	if ((int)newsize <= 0)
		return((char *) NULL);
	p = ((Header *)oldmem - 1);
	oldsize = p->s.size;
	p = (Header *)brkctl(BR_ARGSEG,(long)newsize-oldsize,p);
					/* Adjust segment size		*/
	return(oldmem);
}
