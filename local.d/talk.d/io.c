/*
 *	@(#)io.c	1.3 12/3/84
 *
 *	This file contains the I/O handling and the exchange of edit
 *	characters. The connection itself is established in ctl.c.
*/

#include "talk.h"
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

extern int	errno;
extern int	sys_nerr;
extern char	*sys_errlist[];
extern unsigned alarm();

SIG_T
check_pipe(dummy)
{   unsigned nb;
    char buf[1];

    (void) signal(SIGALRM, check_pipe);
    (void) fcntl(mine, F_SETFL, O_NDELAY|fcntl(mine, F_GETFL, 0));
    while (nb = read(mine, buf, 1)) {
        if (buf[0] == '\004') {
            message("Connection closed. Exiting");
            quit();
        } else {
            display(&other_win, buf, nb);
        }
    }
    (void) alarm(2);
}

/*
 *	The routine to do the actual talking.  The pipe side is monitored
 *	every 2 seconds by check_pipe(), which lurks in the background
 *	whenever an alarm is pending.
*/
talk()
{   unsigned secs;
    int nb;
    char buf[1];

    message(" Connection established\007\007\007 ");
    line = 0;
    /*
     * wait on both the other process and standard input
    */
    check_pipe();
    forever {
	nb = read(0, buf, 1);	/* Try to read the keyboard		*/
	if (nb > 0) {		/* If we got a character		*/
	    secs = alarm(0);	/* Don' take an AST while in curses	*/
	    display(&my_win, buf, (unsigned)nb);
	    (void) write(other, buf, (unsigned)nb);
	    if (buf[0] == '\004') {
		message("Connection closed. Exiting");
		quit();
	    }
	    (void) alarm(secs);
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
