/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
static char	*sccsid = "@(#)mem.c	7.1 (down!honey) 85/08/06";
#endif /*lint*/

#include "def.h"

link	*
newlink()
{
	register link	*rval;

	if ((rval = (link * ) malloc(sizeof(link))) == 0)
		nomem();
	strclear((char *) rval, sizeof(link));	/* fresh as a daisy */
	Lcount++;
	return(rval);
}

node	*
newnode()
{
	register node	*rval;

	if ((rval = (node * ) malloc(sizeof(node))) == 0)
		nomem();
	strclear((char *) rval, sizeof(node));	/* fresh as a daisy */
	Ncount++;
	return(rval);
}

char	*
strsave(s)
register char	*s;
{
	register char *r;

	if ((r = malloc(strlen(s) + 1)) == 0)
		nomem();
	(void) strcpy(r, s);
	return(r);
}

#ifndef strclear
void
strclear(dst, len)
register char *dst;
register int len;
{
	while (--len >= 0)
		*dst++ = 0;
}
#endif /*strclear*/

node	**
newtable(size)
register int	size;
{
	register node	**rval;

	if ((rval = (node **) malloc(size * sizeof(*rval))) == 0) 
		nomem();
	strclear((char *) rval, size * sizeof(*rval));
	return(rval);
}

freetable(t, size)
register node	**t;
{
#ifdef MYMALLOC
	addtoheap((char *) t, (long) (size * sizeof(*t)));
#else /*!MYMALLOC*/
	free((char *) t);
#endif /*MYMALLOC*/
}

nomem()
{
	fprintf(stderr, "%s: Out of memory\n", ProgName);
	badmagic(1);
}

#ifdef MYMALLOC
typedef struct heap heap;
struct heap {
	heap	*h_next;
	long	h_size;
};

STATIC heap	*Heap;	/* not to be confused with a priority queue */

addtoheap(p, size)
register char	*p;
register long	size;
{
	register heap	*hptr = (heap *) p;

	hptr->h_next = Heap;
	hptr->h_size = size;
	Heap = hptr;
}

char	*
mymalloc(n)
register int	n;
{
	static long	size;
	static char	*mem;
	register char	*rval;
	register heap	*hptr;

	if (n > BUFSIZ)
		rval = memget(n);
	else {
#ifdef ALIGN
		int adjustment;

		adjustment = align(mem);
		mem += adjustment;
		size -= adjustment;
#endif /*ALIGN*/
		if (n > size) {
			/* look in the heap -- already aligned */
			if (Heap) {
				if (Heap->h_size >= size) {
					mem = (char *) Heap;
					size = Heap->h_size;
					Heap = Heap->h_next;
				} else {
					for (hptr = Heap; hptr->h_next; hptr = hptr->h_next)
						if (hptr->h_next->h_size >= size)
							break;
					if (hptr->h_next) {
						mem = (char *) hptr->h_next;
						size = hptr->h_next->h_size;
						hptr->h_next = hptr->h_next->h_next;
					}
				}
			} else {
				mem = memget(BUFSIZ);
				size = BUFSIZ;
			}
		}
		rval = mem;
		mem += n;
		size -= n;
	}
	return(rval);
}

myfree(s)
char	*s;
{
#ifdef lint
	s = s;
#endif /*lint*/
}

#ifdef ALIGN
char	*
memget(n)
register int	n;
{
	register char	*rval;
	register int	adjustment = 0;

	adjustment = align(sbrk(0));
	rval = sbrk(n + adjustment);
	if (rval <= 0)
		return(0);
	return(rval + adjustment);
}

align(n)
register char	*n;
{
	register int	abits;	/* alignment bits */
	register int	adjustment = 0;

	abits = (int) n & ~(0xff << ALIGN) & 0xff;
	if (abits != 0)
		adjustment = (1 << ALIGN) - abits;
	return(adjustment);
}
#endif /*ALIGN*/

#endif /*MYMALLOC*/
