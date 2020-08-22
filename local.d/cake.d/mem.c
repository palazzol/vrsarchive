/*
**	Memory management module.
*/

static	char
rcs_id[] = "$Header: /home/Vince/cvs/local.d/cake.d/mem.c,v 1.1 1987-11-16 17:21:31 vrs Exp $";

#include	"cake.h"

/*
**	Allocate space if possible. In the future it may record
**	all pointers returned, so that old can do some checks.
*/

Cast
newmem(size)
int	size;
{
	reg	char	*space;

	if ((space = malloc((unsigned) size)) == NULL)
	{
		fprintf(stderr, "cake system error: no more malloc\n");
		exit_cake(FALSE);
	}

#ifdef	EXTRACHECK
	if (((int) space & 03) != 0)
	{
		fprintf(stderr, "cake system error: malloc not aligned\n");
		exit_cake(FALSE);
	}
#endif

	return (Cast) space;
}

/*
**	Return some storage to the free list. This storage must
**	have been obtained from new and malloc.
*/

/*ARGSUSED*/
oldmem(ptr)
Cast	ptr;
{
#ifdef	MEMUSED
	free((char *) ptr);
#endif
}
