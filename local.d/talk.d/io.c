/*
 *	@(#)io.c	1.1 12/2/84
 *
 *	This file contains the I/O handling and the exchange of edit
 *	characters. The connection itself is established in ctl.c.
*/

#include "talk.h"
#include <errno.h>
#include <time.h>

extern int	errno;
extern int	sys_nerr;
extern char	*sys_errlist[];
extern unsigned alarm();

/*
 *	The routine to do the actual talking.
*/
talk()
{   unsigned nb;
    char buf[1];

    (void) alarm(0);
    message(" Connection established\007\007\007 ");
    line = 0;
    /*
     * wait on both the other process and standard input
    */
    forever {
	if (rdchk(mine) > 0) { 
	    nb = read(mine, buf, 1);
	    if ((nb == 0) || (buf[0] == '\004')) {
		message("Connection closed. Exiting");
		quit();
	    } else {
		display(&other_win, buf, nb);
	    }
	}
	if (rdchk(0) > 0) {
	    nb = read(0, buf, 1);
	    display(&my_win, buf, nb);
	    (void) write(other, buf, nb);
	    if ((nb == 0) || (buf[0] == '\004')) {
		message("Connection closed. Exiting");
		quit();
	    }
	}
    }
}

/*
 *	The p_error routine prints the system error message at the standard
 *	location on the screen and then exits. (i.e. a curses version of
 *	perror).
*/
p_error(string) 
char *string;
{   char *sys;

    sys = "Unknown error";
    if(errno < sys_nerr) {
	sys = sys_errlist[errno];
    }
    (void) wmove(my_win.x_win, line % my_win.x_nlines, 0);
    (void) wprintw(my_win.x_win, "[%s : %s (%d)]\n", string, sys, errno);
    (void) wrefresh(my_win.x_win);
    (void) move(LINES-1, 0);
    (void) refresh();
    quit();
}

/*
 *	Display a message at the standard location.
*/
message(string)
char *string;
{
    (void) wmove(my_win.x_win, line % my_win.x_nlines, 0);
    (void) wprintw(my_win.x_win, "[%s]\n", string);
    (void) wrefresh(my_win.x_win);
}
