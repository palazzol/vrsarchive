/*
 *	@(#)display.c	1.2 12/2/84
 *
 *	The window 'manager', initializes curses and handles the actual
 *	displaying of text.
*/

#include "talk.h"

xwin_t my_win;
xwin_t other_win;
WINDOW *line_win;

int curses_initialized = 0;

/*
 * Display some text on somebody's window, processing some control
 * characters while we are at it.
*/
display(win, text, size)
register xwin_t *win;
register char *text;
unsigned size;
{
    register int i;
    char cch;

    for (i = 0; i < size; i++) {

	if (*text == '\n') {
	    xscroll(win, 0);
	    text++;
	    continue;
	}

	    /* erase character */

	if (*text == win->cerase) {
	    if (win->x_col > 0)
	        win->x_col--;
	    (void) wmove(win->x_win, win->x_line, win->x_col);
	    getyx(win->x_win, win->x_line, win->x_col);
	    (void) waddch(win->x_win, ' ');
	    (void) wmove(win->x_win, win->x_line, win->x_col);
	    getyx(win->x_win, win->x_line, win->x_col);
	    text++;
	    continue;
	}
	/*
	 * On word erase search backwards until we find
	 * the beginning of a word or the beginning of
	 * the line.
	 */
	if (*text == win->werase) {
	    int endcol, xcol, j, c;

	    endcol = win->x_col;
	    xcol = endcol - 1;
	    while (xcol >= 0) {
		c = readwin(win->x_win, win->x_line, xcol);
		if (c != ' ')
			break;
		xcol--;
	    }
	    while (xcol >= 0) {
		c = readwin(win->x_win, win->x_line, xcol);
		if (c == ' ')
			break;
		xcol--;
	    }
	    (void) wmove(win->x_win, win->x_line, xcol + 1);
	    for (j = xcol + 1; j < endcol; j++)
		(void) waddch(win->x_win, ' ');
	    (void) wmove(win->x_win, win->x_line, xcol + 1);
	    getyx(win->x_win, win->x_line, win->x_col);
	    continue;
	}
	    /* line kill */
	if (*text == win->kill) {
	    (void) wmove(win->x_win, win->x_line, 0);
	    (void) wclrtoeol(win->x_win);
	    getyx(win->x_win, win->x_line, win->x_col);
	    text++;
	    continue;
	}
	if (*text == '\f') {
	    if (win == &my_win)
		(void) wrefresh(curscr);
	    text++;
	    continue;
	}

	if (win->x_col == COLS-1) {
		/* check for wraparound */
	    xscroll(win, 0);
	}

	if (*text < ' ' && *text != '\t') {
	    (void) waddch(win->x_win, '^');
	    getyx(win->x_win, win->x_line, win->x_col);

	    if (win->x_col == COLS-1) {
		    /* check for wraparound */
		xscroll(win, 0);
	    }

	    cch = (*text & 63) + 64;
	    (void) waddch(win->x_win, cch);
	} else {
	    (void) waddch(win->x_win, *text);
	}

	getyx(win->x_win, win->x_line, win->x_col);
	text++;

    }
    (void) wrefresh(win->x_win);
}

/*
* Read the character at the indicated position in win
*/
readwin(win, lin, col)
WINDOW *win;
{
    int oldlin, oldcol;
    register int c;

    getyx(win, oldlin, oldcol);
    (void) wmove(win, lin, col);
    c = winch(win);
    (void) wmove(win, oldlin, oldcol);
    return(c);
}

/*
* Scroll a window, blanking out the line following the current line
* so that the current position is obvious
*/

xscroll(win, flag)
register xwin_t *win;
int flag;
{
    if (flag == -1) {
	(void) wmove(win->x_win, 0, 0);
	win->x_line = 0;
	win->x_col = 0;
	return;
    }
    win->x_line = (win->x_line + 1) % win->x_nlines;
    win->x_col = 0;
    (void) wmove(win->x_win, win->x_line, win->x_col);
    (void) wclrtoeol(win->x_win);
    (void) wmove(win->x_win, (win->x_line + 1) % win->x_nlines, win->x_col);
    (void) wclrtoeol(win->x_win);
    (void) wmove(win->x_win, win->x_line, win->x_col);
}
