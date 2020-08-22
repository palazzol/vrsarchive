/*
 *	%Z% %M% %I% %D% %Q%
 *
 *	Copyright (C) Microsoft Corporation, 1983
 *
 *  Simple dialer program for the Hayes "Smart" Modem 1200
 *
 *	See Hayes manual for command definitions
 *
 *  Usage:  dial ttyname telnumber speed 
 *
 *	returns 0 if a connection was made
 *		-1 otherwise
 */

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termio.h>

#define SAME	0
char *setup = "M1 F1 DT";		/* Speaker on, Full Duplex, Touch tone*/
struct termio term;
int baudrate;				/* baud rate of modem */
char buffer[80];
int alrmint();


static char dial_version[] ="@(#) dial.c $State: Exp $ $Date: 1986-10-04 14:29:42 $";

main(argc,argv)
int argc;
char *argv[];
{
	FILE *fdr,*fdw;
	int fd;

	if( argc != 4) {
		fprintf(stderr,"Usage: dial  devicename [number] speed\n");
		exit(-1);
	}
	if( (fd=open(argv[1],O_RDWR|O_NDELAY)) < 0 ) {
		fprintf(stderr,"dial: Can't open device: %s for reading.\n",argv[1]);
		exit(-1);
	}
	switch(atoi(argv[3])) {
		case 300:
			baudrate = B300;
			break;
		case 1200:
			baudrate = B1200;
			break;
		case 9600:
			/*
			 *	Special hack to connect via OpenNet.
			 *	We use vt as a daemon to ship the characters
			 *	to and from node argv[2].
			*/
			if (fork() == 0) {	/* Child does the open	*/
				setpgrp();	/* Nuke /dev/tty	*/
				argv[1][5] = 't';/* Make tty name	*/
				if( (fd=open(argv[1],O_RDWR)) < 0 ) {
	fprintf(stderr,"dial: Can't open device: %s for reading.\n",argv[1]);
					exit(-1);
				}		/* Have new /dev/tty	*/
				close(0); dup(fd);
				close(1); dup(fd);
				close(2); dup(fd);
				close(fd);	/* Have std{in,out,err}	*/
				execlp("vt", "vt", argv[2], (char *)0);
			}
			exit(0);		/* Indicate success	*/
		default:
			baudrate = B1200;
	}		
	/*
	 * set line for no echo and specific speed
	 */
	ioctl(fd, TCGETA, &term);
	term.c_cflag &= ~CBAUD;
	term.c_cflag |= CLOCAL|HUPCL|baudrate;
	term.c_lflag &= ~ECHO;
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	ioctl(fd, TCSETA, &term);
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NDELAY);
	if( (fdr=fopen(argv[1],"r")) == NULL ) {
		fprintf(stderr,"dial: Can't open device: %s for reading.\n",argv[1]);
		exit(-1);
	}
	if( (fdw=fopen(argv[1],"w")) == NULL ) {
		fprintf(stderr,"dial: Can't open device: %s for writing.\n",argv[1]);
		exit(-1);
	}
	setbuf(fdw,0);				/* Want unbuffered I/O */
	/*
	 * setup for timeout in 10 seconds if no response
	 */
	signal(SIGALRM, alrmint);
	alarm(10);
reread:
	fprintf(fdw,"AT\r");			/* Put Hayes into command mode */
	if( fgets(buffer,sizeof buffer, fdr) == (char *)NULL )
		exit(-1);
	if( strncmp(buffer, "OK",2) != SAME ) {		/* got back an OK? */
		sleep(1);
		goto reread;
	}
	alarm(0);					/* turn off alarm */
	sleep(1);
	fprintf(fdw,"AT %s %s\r",setup,argv[2]);	/* put out dialing string */
	/*
	 * turn off CLOCAL now, since we want modem interrupts to work
	 * setup alarm.  (Longer timeout period for longer numbers)
	 *
	ioctl(fd, TCGETA, &term);
	term.c_cflag &= ~CLOCAL;
	ioctl(fd, TCSETA, &term);
	*/
	alarm((4*strlen(argv[2])) + 5);
again:
	if( fgets(buffer,sizeof buffer,fdr) == (char *)NULL )
			exit(-1);
	if( strncmp(buffer, "NO CARRIER",10) == SAME ) {
		exit(-1);
	}
	if( strncmp(buffer, "CONNECT",7) != SAME ) {
		goto again;
	}
	exit(0);
}

alrmint()
{
	exit(-1);
}

