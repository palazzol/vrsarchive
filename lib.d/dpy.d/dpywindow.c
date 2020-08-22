#ifdef SCCS
static char *sccsid = "@(#)dpywindow.c	1.3	1/17/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "dpy.h"

/* Set the row and column boundaries of the current window.
 * Negative numbers indicate backwards from last row or column.
 * The write location is set to the top left of the window.
 * Returns nonzero if arguments are out of bounds.
 */
dpywindow(minrow, maxrow, mincol, maxcol)
	register int	minrow, maxrow;		/* range of rows */
	register int	mincol, maxcol;		/* range of columns */
{
	register struct	window	*wp;		/* window pointer */

	wp = &window;
	if (minrow < 0) minrow += wp->nrows;
	if (maxrow < 0) maxrow += wp->nrows;
	if (mincol < 0) mincol += wp->ncols;
	if (maxcol < 0) maxcol += wp->ncols;
	if (((unsigned) minrow > maxrow)
		|| ((unsigned) maxrow >= wp->nrows)
		|| ((unsigned) mincol > maxcol)
		|| (maxcol >= wp->ncols)) {
			return(1);		/* illegal */
	}
	wp->begwin = wp->begdata + (minrow * wp->delta) + mincol;
	wp->endwin = wp->begwin + ((maxrow - minrow) * wp->delta);
	wp->begrow = wp->begwin;
	wp->endrow = wp->begrow + (maxcol - mincol + 1);
	wp->cp = wp->begrow;
	wp->full = 0;
	return(0);
}
