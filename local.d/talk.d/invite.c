/*
 *	@(#)invite.c	1.3 12/3/84
*/

#include "talk.h"
#include <signal.h>
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
    char opipe[SYS_SIZE+10];

    sprintf(opipe, "%s/tmp/%s", other_system, other_tty);
    message("Checking for invitation");
    return(stat(opipe, &other_buf) == 0);
}
