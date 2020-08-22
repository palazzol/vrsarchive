/*
 *	@(#)announce.c	1.3 12/3/84
*/

#include "talk.h"
#include <time.h>
#include <sys/stat.h>

/*
 *	See if the user is accepting messages. If so, announce that 
 *	a talk is requested.
*/
announce()
{
    char full_tty[SYS_SIZE+10];
    FILE *tf;
    struct stat stbuf;

    (void) sprintf(full_tty, "%s/dev/%s", other_system, other_tty);
    if (access(full_tty, 0) != 0) {
	return(FAILED);
    }
    if ((tf = fopen(full_tty, "w")) == NULL) {
	return(PERMISSION_DENIED);
    }
    if (fstat(fileno(tf), &stbuf) < 0) {
	return(PERMISSION_DENIED);
    }
    if ((stbuf.st_mode&02) == 0) {
	return(PERMISSION_DENIED);
    }
    print_mesg(tf);
    (void) fclose(tf);
    return(SUCCESS);
}

#define max(a,b) ( (a) > (b) ? (a) : (b) )
#define N_LINES 4
#define N_CHARS 120

/*
 *	Build a block of characters containing the message.  It is sent
 *	blank filled and in a single block to try to keep the message in
 *	one piece if the recipient is in vi at the time.
*/
print_mesg(tf)
FILE *tf;
{
    long time(), tim;
    struct tm *localtime();
    struct tm *localclock;
    char line_buf[N_LINES][N_CHARS];
    int sizes[N_LINES];
    char big_buf[N_LINES*N_CHARS];
    char *bptr, *lptr;
    int i, j, max_size;

    i = 0;
    max_size = 0;
    tim = time((long *) 0);
    localclock = localtime(&tim);
    (void) sprintf(line_buf[i], " ");
    sizes[i] = strlen(line_buf[i]);
    max_size = max(max_size, sizes[i]);
    i++;
    (void)sprintf(line_buf[i],"Talk: Connection requested by %s@%s at %d:%02d.",
	my_name, my_system+2, localclock->tm_hour , localclock->tm_min );
    sizes[i] = strlen(line_buf[i]);
    max_size = max(max_size, sizes[i]);
    max_size = max(max_size, sizes[i]);
    i++;
    (void) sprintf(line_buf[i], "Talk: respond with:  talk %s@%s %s",
		my_name, my_system+2, my_tty);
    sizes[i] = strlen(line_buf[i]);
    max_size = max(max_size, sizes[i]);
    i++;
    (void) sprintf(line_buf[i], " ");
    sizes[i] = strlen(line_buf[i]);
    max_size = max(max_size, sizes[i]);
    i++;
    bptr = big_buf;
    *(bptr++) = '';	/* send something to wake them up */
    *(bptr++) = '\r';	/* add a \r in case of raw mode */
    *(bptr++) = '\n';
    for(i = 0; i < N_LINES; i++) {
	lptr = line_buf[i];
	while (*lptr != '\0') {
	    *(bptr++) = *(lptr++);
	}			/* copy the line into the big buffer */
	for(j = sizes[i]; j < max_size + 2; j++) {
	    *(bptr++) = ' ';
	}			/* pad out the rest of the lines with blanks */
	*(bptr++) = '\r';	/* add a \r in case of raw mode */
	*(bptr++) = '\n';
    }
    *bptr = '\0';
    (void) fprintf(tf, big_buf);
    (void) fflush(tf);
}
