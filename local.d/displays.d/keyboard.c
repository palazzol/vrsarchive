/*
 *	Routines to handle the keyboard.
*/
#include <fcntl.h>
#include <ctype.h>
char uncbuf[] = "^c";
#define unctrl(c)	(uncbuf[1] = (c)|0100, isprint(c) ? uncbuf+1 : uncbuf)
#include "globals.h"

extern void done();

char help_msg[80];

/*
 *	Init for kchar()
*/
keybd_init()
{
	int flags;

	(void) close(0);
	if (open("/dev/tty", 2) == -1) {
		perror("keyboard reopen");
		exit(1);
	}
	flags = fcntl(0, F_GETFL, 0);
	if (flags == -1) {
		perror("keyboard F_GETFL");
		exit(1);
	}
	if (fcntl(0, F_SETFL, flags|O_NDELAY) == -1) {
		perror("keyboard F_SETFL");
		exit(1);
	}
}

/*
 * Input one or more characters from the keyboard and process them.
 * Right now all control commands are single characters.
 */
kchar()
{
	char c;
	static int n, kcharstate = 0;

	nap(20);
	n = read(0, &c, 1);
	if (n <= 0)
		return;
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
