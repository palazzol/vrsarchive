/*
 *	@(#)talk.c	1.1 12/2/84
*/

#include "talk.h"

char *state;

xwin_t my_win;
xwin_t other_win;
WINDOW *line_win;
int curses_initialized;
int line;

char my_tty[];			/* My tty name			*/
char my_name[];			/* My user name			*/
int mine;			/* My pipe fd			*/

char other_tty[];		/* Other tty name		*/
char other_name[];		/* Other user name		*/
int other;			/* Other pipe fd		*/

/*
 * talk:	A visual form of write. Using named pipes, a two way 
 *		connection is set up between the two people talking. 
 *		With the aid of curses, the screen is split into two 
 *		windows, and each users text is added to the window,
 *		one character at a time...
 *
 *		Written by Kipp Hickman
 *		
 *		Modified to run under 4.1a by Clem Cole and Peter Moore
 *		Modified to run between hosts by Peter Moore, 8/19/82
 *		Modified to run under 4.1c by Peter Moore 3/17/83
 *		Rewritten to use System 3/5 named pipes by vrs 11/1/84
*/
main(argc, argv)
int argc;
char *argv[];
{
	(void) chdir("/tmp");		/* Do talking in public area	*/
	(void) umask(0);		/* Do talking with public files	*/
	get_names(argc, argv);		/* Get participants		*/
	init_display();			/* Set up screen		*/
	open_mine();			/* Create my pipe		*/
	start_msgs();			/* Begin reporting status	*/
	if (!have_invite()) {		/* Check if other party waiting	*/
	    invite();			/* Nope, invite him		*/
            wait_open();		/* Wait for slave to open	*/
	    open_other();		/* Open other pipe		*/
	} else {
	    open_other();		/* Open other pipe		*/
            wait_open();		/* Wait for master to open	*/
	}
	end_msgs();			/* Stop reporting status	*/
	set_edit_chars();		/* Exchange edit characters	*/
	talk();				/* Exchange characters		*/
}
