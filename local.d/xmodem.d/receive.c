/*  Version 1.0		6/12/85
	Jim Hein
	Nicolet Instrument Corp
	5225 Verona Rd
	Madison, WI  53611-0288
*/
/*
**	receive filename
*/
# include <sys/types.h>
# include <fcntl.h>
# include <curses.h>
# include <signal.h>
# include <time.h>

# define	SOH	0x1
# define	NAK	0x15
# define	ACK	0x6
# define	EOT	0x4
# define	CANCEL	0x18

char filename[64];
int retries;
int seq_num;

SIG_T
caught(dummy) {
	char c;

	c = CANCEL;
	write(1,&c,1);
	reset();
	kill(0,SIGKILL);
}

main(argc,argv)
int argc;
char *argv[]; {

	int fd;
	register i;
	char c;
	char block[131];
	int length, pid;

	setpgrp(getpid());
	if (!((argc == 2) || (argc == 3))) {
		printf("ILLEGAL NUMBER OF ARGUMENTS.\n");
		printf("SYNOPSIS: xmodrec [-t] filename\n");
		exit(-1);
	}

	if (argc == 3) {
		if (strcmp(argv[1],"-t") && strcmp(argv[1],"-m")) {
			printf("bad argument `%s'\n",argv[1]);
			printf("SYNOPSIS: xmodrec [-t] filename\n");
			exit(-1);
		}
	}

	strcpy(filename,argv[argc-1]);
	if ((fd = open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644)) < 0) {
		perror(filename);
		exit(-1);
	}

	initscr();
	raw();			/* set terminal to 8-bit I/O */
	noecho();

/*
**	Ignore interrupts from the user.
**	If the user could delete this program the terminal
**	would be left in a undiserable state of mind.
*/
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGALRM, caught);

	if ((pid = fork()) == 0) {   /* child sends ready prompt */
		c = NAK;
		for ( i = 0; i < 10; i++) {
			write(1,&c,1);
			sleep(10);
		}
		reset(); 	     /* restore terminal to normal state */
		kill(0,SIGKILL);     /* kill all processes */
	}

	read(0,&c,1);
	kill(pid,SIGKILL);  /* received SOH response, so kill child */
	wait(0);

	seq_num = 1; retries = 0;
	for(;;) {

		if (retries == 10) {
			c = CANCEL;
			write(1,&c,1);
			RESET("Terminate after 10 retries",retries);
		}

		switch( c & 0x7f ) {
			case SOH:
				BLOCK(fd);
				break;
			case CANCEL:
				RESET("main: CANCEL IN switch",0);
			case EOT:
				c = ACK;
				write(1,&c,1);
				noraw();
				echo();
				endwin();
				goto END;
			default:
				SET_TIMER();
				for ( i = 0; i < 131; i++) read(0,&c,1);
				DISABLE_TIMER();
				c = NAK;
				write(1,&c,1);
				retries++;
				break;
		}
		SET_TIMER();
		read(0,&c,1);
		DISABLE_TIMER();
	}
END:
	DISABLE_TIMER();
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGALRM, SIG_DFL);

	close(fd);
	if (argc == 3) {
		switch( argv[1][1] ) {
			case 't':
				printf("Deleting carriage returns\n");
				DELCR(filename);
				break;
			case 'm':
				printf("Changing cr's to lf's\n");
				CR2LF(filename);
		}
	}
}

BLOCK(fd) 
int fd; {
	
	register i;
	register char *p;
	char block[131];
	char c;
	int length, pid;
	int xsum;

	SET_TIMER();
	length = 0;
	for( i = 0, p = block; i < 131; i++) length += read(0,p++,1);
	DISABLE_TIMER();
	
	if (length != 131) {
		c = NAK;
		write(1,&c,1);
		retries++;
		return;
	}

	if ( (block[0] & 0xff) != seq_num ) {
		i = (seq_num ? seq_num - 1 : 255);
		if ((block[0] & 0xff) != i ) {
			if ( (block[1] & 0xff) != (255 - seq_num) ) {
				i = 255 - i;
				if ((block[1] & 0xff) == i ) {
					c = NAK;
					write(1,&c,1);
					retries++;
					return;
				}
				c = CANCEL; 	/* loss of synchronization */
				write(1,&c,1);
				RESET("loss of syn: block = %d, seq_num = %d",block[0]);
			}
		}
		retries++;
		goto leave;
	}

	if ( (block[1] & 0xff) != (255 - seq_num) ) {
		c = NAK;
		write(1,&c,1);
		retries++;
		return;
	}
	
	xsum = 0;
	for (i = 2; i < 130; i++) xsum += block[i];
	xsum &= 0xff;
	if ( (block[130] & 0xff) != xsum ) {
		c = NAK;
		write(1,&c,1);
		retries++;
		return;
	}

	write(fd,&block[2],128);
	retries = 0;
	seq_num = ++seq_num % 256;
leave:
	c = ACK;
	write(1,&c,1);
}

reset() {
	
	noraw();
	endwin();
	unlink(filename);
}
	

RESET(s,b) 
char *s; 
int b;	{

	FILE *debug;

	reset();
	if ((debug = fopen("debug","w")) == NULL ) {
		perror("debug");
		exit(1);
	}
	fprintf(debug,s,b&0xff,seq_num);
	exit(-1);
}

/*
DEBUG(b,bl,dat,s)
char *b;
int bl, dat; 
char *s; {

	char buf[128];
	write(debug,b,131);
	sprintf(buf,s,bl,dat);
	write(debug,buf,strlen(buf));
}
*/

SET_TIMER() {
	alarm(0);
	alarm(60);
}

DISABLE_TIMER() {
	alarm(0);
}
