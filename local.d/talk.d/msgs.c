/*
 *	@(#)msgs.c	1.1 12/2/84
 *
 *	A package to display what is happening every second if we are
 *	slow connecting.
*/

#include <signal.h>
#include "talk.h"

extern unsigned alarm();

char *state;
int  line = 0;

disp_msg()
{
    message(state);
    (void) alarm(15);
}

start_msgs()
{
    message(state);
    (void) signal(SIGALRM, disp_msg);
    (void) alarm(15);
}

end_msgs()
{
    (void) signal(SIGALRM, SIG_IGN);
    (void) alarm(15);
}
