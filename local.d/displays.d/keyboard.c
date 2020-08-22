/*
 *	Routines to handle the keyboard.
*/
#include <signal.h>
#include <ctype.h>
char uncbuf[] = "^c";
#define unctrl(c)	(uncbuf[1] = (c)|0100, isprint(c) ? uncbuf+1 : uncbuf)
#include "globals.h"
#include "pipe.h"

extern void done();

char help_msg[80];
int keybd_pid;

/*
 *	Init for kchar()
*/
keybd_init(pipe_fd)
int pipe_fd;
{
	struct pipe_msg buf;

	if ((keybd_pid = fork()) == 0) {
		buf.id = -1;
		while (1) {
			buf.count = read(0, &buf.ch, 1);
			(void) write(pipe_fd, (char *)&buf, sizeof buf);
		}
	}
}

/*
 * Input one or more characters from the keyboard and process them.
 * Right now all control commands are single characters.
 */
kchar(c)
char c;
{
	static int kcharstate = 0;

	c &= 0177;

	/* Process special tty commands */
	switch(kcharstate) {
	case 0:
		if (c == cmdchar)
			kcharstate = 1;
		else {
			/*
			 * Ordinary character.
			 * Pass it through to current pty.
			*/
			put_pty(curvdsp, c);
		}
		break;
	case 1:
		if (c == cmdchar)
			c = 037; /* stutter shifts virtual display */
		switch (c) {
		/* ^q: exit */
		case 'q':
		case '\177':
			done(0);

		/* ^_: jump to next virtual display */
		case 037:
		case 'n':
			next_vdisp();
			break;
		
		/* ^L: clear and redraw the screen */
		case 014:
			redraw_screen();
			break;
		
		/* _: pass through literal ^_ */
		case '_':
			c = cmdchar;
			put_pty(curvdsp, c);
			break;
		default:
			/* usage */
			sprintf(help_msg,
"Usage: %s then %s (next display), q (quit), ^L (redraw), _ (lit. %s)",
			unctrl(cmdchar), unctrl(cmdchar), unctrl(cmdchar));
			message(help_msg);
			break;
		}
		kcharstate = 0;
	}
}

/*
 *	Kill off the keyboard process.
*/
keybd_wrapup()
{
	kill(keybd_pid, SIGTERM);
	wait((int *)0);
}
