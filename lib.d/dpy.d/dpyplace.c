#ifdef SCCS
static char *sccsid = "@(#)dpyplace.c	1.5	1/28/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "dpy.h"

/* Place a single character to the window at a particular location.
 * The change will not be seen until a call to dpyupdate.
 * The current write location is unaffected.
 * Returns nonzero if coordinates are illegal.
 * The coordinates are relative to the current window.
 */
dpyplace(row, col, ch)
	register int	row;		/* row to place character at */
	register int	col;		/* column to place character at */
	char	ch;			/* character to be placed */
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
		return(1);		/* out of window */
	}
	cp = wp->begwin + (row * wp->delta) + col;
	if (*cp != ch) {		/* do only if char needs changing */
		if (cp < wp->begchange) wp->begchange = cp;
		*cp++ = ch;
		if (cp > wp->endchange) wp->endchange = cp;
	}
	return(0);
}
