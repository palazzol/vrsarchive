#ifdef SCCS
static char *sccsid = "@(#)dpyread.c	1.13	2/16/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "dpy.h"		/* window definitions */

static int readline();		/* default read routine */

/*
 *	Modifications to correct usage of routine to standard C for
 *	portability by Vincent R. Slyngstad.
*/
/* Read some input while possibly showing it in the current window.
 * If the prompt string is NULL, then editing is performed without
 * any windowing activity (useful when reading commands from scripts).
 * Otherwise, the prompt is shown in the window along with any input.
 * The given routine is called for each character, with an argument
 * which is the previous character (or -1 on the first call).
 * The routine returns the next input character, or -1 to stop reading.
 * A null routine defaults to one which reads until an end of line.
 * Scrolling of the window is automatically performed when necessary.
 * Editing of the input is handled.  If the buffer fills up, the user
 * is warned with beeps and further input is ignored.
 * Returns number of bytes of data read.
*/
dpyread(prompt, routine, buf, count)
	register char	*prompt;	/* prompt string (if any) */
	int	(*routine)();		/* routine to call to get character */
	register char	*buf;		/* address of the storage buffer */
	int	count;			/* maximum number of bytes allowed */
{
	register struct	window	*wp;	/* window pointer */
	register int	ch;		/* character which was read */
	register char	*bp;		/* current buffer pointer location */
	char	*endbp;			/* end of buffer */
	char	redraw;			/* need to redisplay input */
	char	oldscroll;		/* old scrolling flag */
	int	promptlen;		/* length of prompt string */

	wp = &window;
	promptlen = 0;
	if (prompt) promptlen = strlen(prompt);
	if (routine == (int (*)())0) routine = readline;
	bp = buf;
	endbp = bp + count - 1;
	redraw = 1;
	ch = -1;
	oldscroll = wp->scroll;
	wp->scroll = 1;
	while (1) {
		if (prompt && redraw) {		/* recompute window data */
			redraw = 0;
			dpyhome();
			dpywrite(prompt, promptlen);
			dpywrite(buf, bp - buf);
			dpyclrwindow();
		}
		if (prompt) dpyupdate();
		ch = (*routine)(ch);
		if (ch < 0) {				/* end of file */
			wp->scroll = oldscroll;
			return(bp - buf);
		}
		if (ch == wp->c_lnext) {		/* literal input */
			ch = (*routine)(ch);
			if (ch < 0) {
				wp->scroll = oldscroll;
				return(bp - buf);
			}
			if (bp >= endbp) {		/* buffer is full */
				write(STDERR, "\07", 1);
				continue;
			}
			*bp = ch;
			if (prompt) dpywrite(bp, 1);
			bp++;
			continue;
		}
		if (ch == wp->c_eof) {			/* end of file */
			wp->scroll = oldscroll;
			return(bp - buf);
		}
		if (ch == wp->c_erase) {		/* character erase */
			if (bp <= buf) continue;
			bp--;
			redraw = 1;
			continue;
		}
		if (ch == wp->c_werase) {		/* word erase */
			if (bp <= buf) continue;
			while ((bp > buf) && ((bp[-1] == '\n')
				|| (bp[-1] == ' ') || (bp[-1] == '\t'))) bp--;
			while ((bp > buf) && (bp[-1] != '\n')
				&& (bp[-1] != ' ') && (bp[-1] != '\t')) bp--;
			redraw = 1;
			continue;
		}
		if (ch == wp->c_kill) {			/* line erase */
			if (bp <= buf) continue;
			if (bp[-1] == '\n') bp--;
			while ((bp > buf) && (bp[-1] != '\n')) bp--;
			redraw = 1;
			continue;
		}
		if (ch == wp->c_rprnt) {		/* retype line */
			if (prompt) dpyredraw();
			continue;
		}
		if (bp >= endbp) {			/* buffer is full */
			write(STDERR, "\07", 1);
			continue;
		}
		*bp = ch;				/* normal character */
		if (prompt) dpywrite(bp, 1);
		bp++;
	}
}


/* Local routine to read until end of line character is reached */
static
readline(ch)
{
	if ((ch == '\n') || (read(STDIN, &ch, 1) < 1)) return(-1);
	return(ch & 0xff);
}
