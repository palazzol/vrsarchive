#ifdef SCCS
static char *sccsid = "@(#)dpymove.c	1.3	1/17/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "dpy.h"

/* Set the current write location to the given row and column.
 * The position given is relative to the current window.
 * Negative numbers indicate backwards from last row or column.
 * Returns nonzero if arguments are out of bounds.
 */
dpymove(row, col)
	register int	row;		/* desired row number */
	register int	col;		/* desired column number */
{
	register struct	window	*wp;	/* window pointer */
	register int	winrows;	/* number of rows in window */
	register int	wincols;	/* number of columns in window */

	wp = &window;
	winrows = ((wp->endwin - wp->begwin) / wp->delta) + 1;
	wincols = wp->endrow - wp->begrow;
	if (row < 0) row += winrows;
	if (col < 0) col += wincols;
	if (((unsigned)row >= winrows) || ((unsigned)col >= wincols)) {
		return(1);		/* out of window */
	}
	wp->begrow = wp->begwin + (row * wp->delta);
	wp->endrow = wp->begrow + wincols;
	wp->cp = wp->begrow + col;
	wp->full = 0;
	return(0);
}
