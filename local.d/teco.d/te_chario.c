/* TECO for Ultrix   Copyright 1986 Matt Fichtenbaum
 * This program and its components belong to GenRad Inc, Concord MA 01742
 * They may be copied if this copyright notice is included
*/

/* te_chario.c   character I/O routines   10/9/86 */
#include <errno.h>
#include "te_defs.h"

#include <fcntl.h>
#ifndef FNDELAY
#include <sys/file.h>
#endif
#ifndef DEBUG
#include <signal.h>
extern int int_handler();
extern int quit_handler();
extern int hup_handler();
#endif

int lf_sw;		/* nonzero: make up a LF following an entered CR*/
struct termio tty_orig;	/* original tty flags				*/
struct termio tty_new;	/* new tty flags				*/
struct termio tty_noint;/* new tty flags, no intr character		*/
int inp_noterm;		/* nonzero if standard input is not a terminal	*/
int out_noterm;		/* nonzero if standard output is not a terminal	*/

/*
 *	Set tty (stdin) mode.  TECO mode is CBREAK, no ECHO, sep CR & LF
 *	operation; normal mode is none of the above.  TTY_OFF and TTY_ON
 *	do this absolutely; TTY_SUSP and TTY_RESUME use saved signal status.
*/
setup_tty(arg)
int arg;
{
	extern int errno;
	int ioerr;

	/* initial processing: set tty mode */
	if (arg == TTY_ON) {
		ioerr = ioctl(fileno(stdin), TCGETA, &tty_orig);/* get input characteristics */
		inp_noterm = (ioerr && (errno == ENOTTY));	/* nonzero if input not a terminal */
		ioerr = ioctl(fileno(stdout), TCGETA, &ttybuf);	/* get output characteristics */
		out_noterm = (ioerr && (errno == ENOTTY));	/* nonzero if output not a terminal */
		ttybuf = tty_new = tty_orig;		/* make a copy of tty control structure */
		tty_new.c_lflag &= ~ECHO & ~ICANON;	/* Set up teco modes */
		tty_new.c_lflag |= NOFLSH;		/* Set up teco modes */
		tty_new.c_cc[VQUIT] = CTL(O);		/* ^O is quit char */
		tty_new.c_cc[VEOL] = -1;		/* No "eol" char */
		tty_noint = tty_new;
		tty_noint.c_cc[VINTR] = -1;	/* disable interrupt char in this one */
	}
	if ((arg == TTY_ON) || (arg == TTY_RESUME)) {
		ioctl(fileno(stdin), TCSETAW, &tty_new);/* set flags for teco */
#ifndef DEBUG
		signal(SIGINT, int_handler);	/* and "^C" signal */
		signal(SIGQUIT, quit_handler);	/* and "^O" signal */
		signal(SIGHUP, hup_handler);	/* and "hangup" signal */
#endif
	} else {
		ioctl(fileno(stdin), TCSETAW, &tty_orig);/* set flags back */
#ifndef DEBUG
		signal(SIGINT,  SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGHUP,  SIG_DFL);
#endif
	}
}

/* routines to handle keyboard input */

int in_read = 0;	/* flag for "read busy" (used by interrupt handler) */

/*
 *	Routine to get a character without waiting, used by ^T when ET & 64 is
 *	set.
*/
int gettty_nowait()
{
	int c, ttyflags;

	if (lf_sw) {
		lf_sw = 0;
		return(LF);		/* LF to be sent: return it */
	}
	++in_read;			/* set "read busy" switch */
	ttyflags = fcntl(fileno(stdin), F_GETFL, 0); /* get standard modes */
	fcntl(fileno(stdin), F_SETFL, ttyflags | FNDELAY);/* "no delay" mode */
	while (!(c = getchar()));	/* read character, skip nulls */
	fcntl(fileno(stdin), F_SETFL, ttyflags);/* reset to normal mode */
	--in_read;
	if (c == CR) ++lf_sw;		/* CR: set switch to make LF */
	return(c);
}

/* normal routine to get a character */
char gettty()
{
	int c;

	if (lf_sw) {
		lf_sw = 0;
		return(LF);		/* if switch set, make up a line feed */
	}
	++in_read;			/* set "read busy" switch */
	while (1) {
		c = getchar();		/* get character */
		if (!c) continue;	/* skip nulls */
		if ((c == EOF) && (errno == EINTR))
			continue;
		break;			/* Have character */
	}
	--in_read;			/* clear switch */
	if (c == CR) ++lf_sw;		/* CR: set switch to make up a LF */
	if (c == EOF) ERROR(E_EFI);	/* end-of-file on standard input */
	return( (char) c & 0177);	/* else return the 7-bit char */
}

#ifndef DEBUG

int_handler()
{	signal(SIGINT, int_handler);	/* prepare for another */
	if (exitflag <= 0) {		/* if executing commands */
		if (et_val & ET_CTRLC)
			et_val &= ~ET_CTRLC;/* if "trap ^C", clear it, ignore */
		else
			exitflag = -2;	/* else set flag to stop execution */
	}
	if (in_read) {			/* if interrupt happened in "getchar" */
		in_read = 0;		/* clear "read" switch */
		qio_char(CTL (C));	/* send a ^C to input stream */
	}
}
#endif

SIG_T (*old_func)();		/* storage for previous signal handler */

/*
 *	Routine to disable (1) and enable (0) ^C interrupt.
 *	(Used to block interrupts during display update.)
*/
block_inter(func)
int func;
{
#ifndef DEBUG
	if (func)
		old_func = signal(SIGINT, SIG_IGN);	/* block interrupt */
	else
		signal(SIGINT, old_func);		/* otherwise handle */
#endif
}


/* simulate a character's having been typed on the keyboard */
qio_char(c)
char c;
{
	ungetc(c, stdin);	/* send char to input stream */
}

#ifndef DEBUG

/* routine to handle "hangup" signal */
hup_handler()
{	signal(SIGHUP, hup_handler);	/* prepare for another */
	if (!exitflag)
		exitflag = -3;	/* if executing, set flag to terminate */
	else {
		panic();	/* dump buffer and close output files */
		exit(1);
	}
}
#endif



/* type a crlf */
crlf()
{
	type_char(CR);
	type_char(LF);
}


int flusho = 0;

#ifndef DEBUG
/* routine to handle ^O signal */
quit_handler()
{	signal(SIGQUIT, quit_handler);	/* prepare for another */
	ioctl(fileno(stdout), TCFLSH, (char *)0);
	crlf();
	flusho = !flusho;
}
#endif

/* reset ^O status */
reset_ctlo()
{	flusho = 0;
}

/* routine to type one character */
type_char(c)
char c;
{
	if (flusho)
		return;
	if ((char_count >= WN_width) && (c != CR) && !(spec_chars[c] & A_L)) {
		/* spacing char beyond end of line */
		if (et_val & ET_TRUNC)
			return;		/* truncate output to line width */
		else
			crlf();		/* do automatic new line */
	}

	if ((c & 0140) == 0) {				/* control char? */
		switch (c & 0177) {
		case CR:
			putchar(c);
			char_count = 0;
			break;

		case LF:
			putchar(c);
			break;

		case ESC:
			if ((et_val & ET_IMAGE) && !exitflag)
				putchar(c);
			else {
				putchar('$');
				char_count++;
			}
			break;

		case TAB:
			if ((et_val & ET_IMAGE) && !exitflag)
				putchar(c);
			else
				for (type_char(' '); (char_count & tabmask) != 0; type_char(' '));
			break;

		default:
			if ((et_val & ET_IMAGE) && !exitflag)
				putchar(c);
			else {
				putchar('^');
				putchar(c + 'A'-1);
				char_count += 2;
			}
			break;
		}
	} else {
		putchar(c);
		char_count++;
	}
}
