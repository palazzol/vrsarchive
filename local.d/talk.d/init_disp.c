/*
 *	@(#)init_disp.c	1.3 12/3/84
 *
 * Init_disp contains the initialization code for the display package,
 * as well as the signal handling routines.
*/

#include "talk.h"
#include <signal.h>
#ifndef A_REVERSE	/* No A_REVERSE if using old TERMCAP stuff */
#define erasechar()	_tty.sg_erase
#define killchar()	_tty.sg_kill
#endif /*!TERMCAP*/

sig_sent()
{   static char eot = '\004';

    message("Connection closing. Exiting");
    (void) write(other, &eot, 1);
    quit();
}

/* 
 *	Set up curses, catch the appropriate signals, and build the
 *	various windows
*/
init_display()
{
    (void) initscr();
    curses_initialized = 1;
    (void) clear();
    (void) refresh();
    noecho();
    crmode();
    (void) signal(SIGINT, sig_sent);
    (void) signal(SIGPIPE, sig_sent);
    my_win.x_nlines = LINES / 2;
    my_win.x_ncols = COLS;
    my_win.x_win = newwin(my_win.x_nlines, my_win.x_ncols, 0, 0);
    scrollok(my_win.x_win, FALSE);
    (void) wclear(my_win.x_win);
    other_win.x_nlines = LINES / 2 - 1;
    other_win.x_ncols = COLS;
    other_win.x_win = newwin(other_win.x_nlines, other_win.x_ncols,
					     my_win.x_nlines+1, 0);
    scrollok(other_win.x_win, FALSE);
    (void) wclear(other_win.x_win);
    line_win = newwin(1, COLS, my_win.x_nlines, 0);
    box(line_win, '-', '-');
    (void) wrefresh(line_win);
    state = "No connection yet";
}

/*
 * Trade edit characters with the other talk. By agreement
 * the first three characters each talk transmits after
 * connection are the three edit characters
*/
set_edit_chars()
{
    char buf[3];
    int cc;
    
    my_win.cerase = erasechar();
    my_win.kill = killchar();
    my_win.werase = '\027';	/* Control W	*/
    buf[0] = my_win.cerase;
    buf[1] = my_win.kill;
    buf[2] = my_win.werase;
    cc = write(other, buf, sizeof(buf));
    if (cc != sizeof(buf) ) {
	p_error("Lost the connection");
    }
    cc = read(mine, buf, sizeof(buf));
    if (cc != sizeof(buf) ) {
	p_error("Lost the connection");
    }
    other_win.cerase = buf[0];
    other_win.kill = buf[1];
    other_win.werase = buf[2];
}

/*
 * All done talking...hang up the phone and reset terminal thingy's
*/
quit()
{
    if (curses_initialized) {
        (void) wmove(other_win.x_win, other_win.x_nlines-1, 0);
        (void) wclrtoeol(other_win.x_win);
        (void) wrefresh(other_win.x_win);
        endwin();
    }
    (void) unlink(my_tty);	/* In case we never got connected	*/
    (void) exit(0);
}
