/*
 *	These are routines to manage the physical display using curses.
*/
#include <curses.h>

redraw_screen()
{
	clearok(stdscr, TRUE);
	wrefresh(stdscr);
}

message(msg)
char *msg;
{	int y, x;

	getyx(stdscr, y, x);
	move(LINES-1, 0);
	addstr(msg);
	move(y, x);
	refresh();
}

/* Initialize the physical display properly. */
pdisp_init()
{
	initscr();
	raw();
	noecho();
}

/*
 * Clean up physical display stuff.
 */
pwrapup()
{
	move(LINES-1, 0);
	clrtoeol();
	endwin();
}

#ifndef A_REVERSE
beep()
{
	refresh();
	write(1, "\007", 1);
}
#endif
