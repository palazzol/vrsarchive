/*
 *	These are routines to manage the physical display using curses.
*/
#include <curses.h>
#include <signal.h>

extern void done();

redraw_screen()
{
	clearok(curscr, TRUE);
	wrefresh(curscr);
}

message(msg)
char *msg;
{	int y, x;

	getyx(curscr, y, x);
	wmove(curscr, LINES-1, 0);
	waddstr(curscr, msg);
	wmove(curscr, y, x);
	wrefresh(curscr);
}

/* Initialize the physical display properly. */
pdisp_init()
{
	initscr();
	signal(SIGHUP, done);
	signal(SIGTERM, done);
	scrollok(curscr, TRUE);
	nonl();
	raw();
	noecho();
}

/*
 * Clean up physical display stuff.
 */
pdisp_wrapup()
{
	clear();
	refresh();
	endwin();
}

#ifndef A_REVERSE
beep()
{
	write(1, "\007", 1);
}
#endif
