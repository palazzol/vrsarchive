/*
 *	@(#)ctl.c	1.3 12/3/84
 *
 *	This file handles the creation of the named pipes for the
 *	communication.  Note that errors from mknod are fatal.
 *
 *	Master announces request
 *	Master creates pipe		Slave creates pipe
 *	Master waits for open		Slave opens master pipe
 *	Master unlinks pipe		Wait for master open
 *	Master opens slave pipe		Slave unlinks pipe
 *
 *	The opens here are read/write opens because (like it or not)
 *	a one way open on a pipe will block until another process
 *	opens the pipe the other way.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "talk.h"

extern int errno;

wait_open()
{   char c;

    while (read(mine,&c,1) <= 0) { /* Flush dummy byte from open_other	*/
        if (errno != EINTR)
            p_error("Cannot read in wait_open");
    }
    (void) unlink(my_tty);	/* Protect communications link		*/
}

open_other()
{   char c = 'X';

    while ((other = open(other_tty, 2)) < 0) {
        if (errno != EINTR)
            p_error("Cannot open other named pipe");
    }
    (void) write(other,&c,1);	/* Write byte checked for by wait_opn	*/
}

open_mine() 
{   mine = mknod(my_tty, S_IFIFO|0666, 0);
    if (mine < 0) {
        p_error("Can't create my named pipe");
    }
    if ((mine = open(my_tty, 2)) < 0) {
        p_error("Cannot open my named pipe");
    }
}
