/*
 *	@(#)invite.c	1.2 12/2/84
*/

#include "talk.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 *	Routine called on interrupt to re-invite the callee.
*/
re_invite()
{
    (void) signal(SIGALRM, re_invite);	/* Resend invite, update screen	*/
    message("Ringing your party");
    line++;
    switch (announce()) {
	case NOT_HERE:
	    message("Your party is not logged on");
            quit();
        case PERMISSION_DENIED:
	    message("Your party is refusing messages");
            quit();
    }
    disp_msg();
}

/*
 *	There wasn't an invitation waiting, so send a request to the
 *	invitee.
*/
invite()
{   
    state = "Waiting for your party to respond";
    re_invite();
}

/*
 *	See if the talkee has invited us (his pipe exists)
*/
have_invite()
{   struct stat other_buf;

    message("Checking for invitation");
    return(stat(other_tty, &other_buf) == 0);
}
