/*
**	Trail module.
*/

static	char
rcs_id[] = "$Header: /home/Vince/cvs/local.d/cake.d/trail.c,v 1.1 1987-11-16 17:23:21 vrs Exp $";

#ifdef	CAKEDEBUG
#include	"cake.h"

#define		TRAILSIZE	100

static	int	trail_entries = 0;	/* no. of calls to put_tr */
static	int	slot = 0;		/* next slot		  */
static	char	*trailfunc[TRAILSIZE];	/* functions and events	  */
static	char	*trailevent[TRAILSIZE];	/* init to NULL by C	  */

/*
**	Register this function on the trail.
*/

put_trail(func, event)
reg	char	*func;
reg	char	*event;
{
	trailfunc[slot]  = func;
	trailevent[slot] = event;

	trail_entries++;
	if (++slot == TRAILSIZE)
		slot = 0;
}

/*
**	Print the trail on the given file.
*/

get_trail(fp)
reg	FILE	*fp;
{
	reg	int	maxent;

	fprintf(fp, "^^^^^^^^^^^^^^^^^^^ TRAIL ^^^^^^^^^^^^^^^^^^^\n");
	fprintf(fp, "%d entries\n", trail_entries);

	maxent = (trail_entries < TRAILSIZE)? trail_entries: TRAILSIZE;
	while (maxent-- > 0)
	{
		if (--slot == -1)
			slot = TRAILSIZE - 1;

		fprintf(fp, "%s %s\n", trailfunc[slot], trailevent[slot]);
	}
}
#endif
