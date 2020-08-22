#ifdef SCCS
static char *sccsid = "@(#)dpyget.c	1.1	1/28/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "dpy.h"

/* Return the character which is at the specified location in the current
 * window.  The character returned is the one in our internal screen image,
 * not what is actually on the screen (these will be identical if dpyupdate
 * was just called).  Returns negative if the coordinates are illegal.
 */
dpyget(row, col)
	register int	row;		/* row to get character from */
	register int	col;		/* column to get character from */
{
	register struct	window	*wp;	/* window pointer */
	register char	*cp;		/* character pointer */
	register int	winrows;	/* number of rows in window */
	register int	wincols;	/* number of columns in window */

	wp = &window;
	winrows = ((wp->endwin - wp->begwin) / wp->delta) + 1;
	wincols = wp->endrow - wp->begrow;
	if (row < 0) row += winrows;
	if (col < 0) col += wincols;
	if (((unsigned)row >= winrows) || ((unsigned)col >= wincols)) {
		return(-1);		/* out of window */
	}
	cp = wp->begwin + (row * wp->delta) + col;
	return(*cp & 0xff);
}
