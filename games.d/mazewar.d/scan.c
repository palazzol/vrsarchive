#ifdef SCCS
static char *sccsid = "@(#)scan.c	1.1	4/5/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

/* Module to read self-terminating input while allowing editing of the input */

#include "war.h"

jmp_buf	scanjumpbuf;			/* jump buffer to use in scanchar */
char	scanbuffer[SCAN_SIZE+1];	/* storage for characters */
char	*scanreadptr;			/* current read pointer */
char	*scanwriteptr;			/* current write pointer */
char	(*scanroutine)();		/* routine to read characters */
static	char	rubchar;		/* erase letter character */
static	char	rubword;		/* erase word character */
static	char	rubline;		/* erase line character */
static	char	litchar;		/* literal input */


#ifndef CERASE
#define CERASE '\b'
#endif
#ifndef CKILL
#define CKILL '\025'
#endif
/*
 * Initialize for later calls to scanchar.
 */
scaninit(routine, jumpbuf)
	char (*routine)();		/* routine to get characters */
	jmp_buf	jumpbuf;		/* jump buffer to use later */
{
#ifdef __STDC__
	struct	termios	sgbuf;		/* basic tty structure */
#else
	struct	sgttyb	sgbuf;		/* basic tty structure */
#endif
#ifdef TIOCGLTC
	struct	ltchars	ltbuf;		/* local tty structure */
#endif

	scanroutine = routine;		/* init static variables */
	memcpy((char *)scanjumpbuf, (char *)jumpbuf, sizeof(scanjumpbuf));
	scanwriteptr = scanbuffer;
	scanreadptr = scanbuffer;
#ifdef __STDC__
	sgbuf.c_cc[VERASE] = CERASE;	/* set defaults in case ioctls fail */
	sgbuf.c_cc[VKILL] = CKILL;
#else
	sgbuf.sg_erase = CERASE;	/* set defaults in case ioctls fail */
	sgbuf.sg_kill = CKILL;
#endif
#ifdef TIOGLTC
	ltbuf.t_werasc = CWERASE;
	ltbuf.t_lnextc = CLNEXT;
#endif
#ifdef __STDC__
	tcgetattr(STDIN, &sgbuf);	/* get and save editing characters */
#else
	ioctl(STDIN, TIOCGETP, &sgbuf);	/* get and save editing characters */
#endif
#ifdef TIOGLTC
	ioctl(STDIN, TIOCGLTC, &ltbuf);
#endif
#ifdef __STDC__
	rubchar = sgbuf.c_cc[VERASE];
	rubline = sgbuf.c_cc[VKILL];
#else
	rubchar = sgbuf.sg_erase;
	rubline = sgbuf.sg_kill;
#endif
#ifdef TIOGLTC
	rubword = ltbuf.t_werasc;
	litchar = ltbuf.t_lnextc;
#endif
}


/*
 * Read the next input character.  If it is an editing character,
 * abort the current context and longjmp back to the last setjmp.
 * NOTE: for proper results, the caller should not alter the global
 * state until the full command has been read in.  This includes such
 * things as prompting for input or saving values.  Otherwise, improper
 * results will occur if the user edits the command.
 */
scanchar()
{
	register int	ch;			/* current character */

loop:	if (scanreadptr < scanwriteptr)		/* get saved char if have any */
		return(*scanreadptr++);
	ch = (*scanroutine)() & 0x7f;		/* get new character */
	if (ch == litchar) {			/* literal input */
		ch = (*scanroutine)() & 0x7f;
		goto store;
	}
	if (ch == rubchar) {			/* character erase */
		if (scanwriteptr <= scanbuffer) {
			write(STDERR, "\007", 1);
			goto loop;
		}
		scanwriteptr--;
		scanreadptr = scanbuffer;
		longjmp(scanjumpbuf, SCAN_EDIT);
	}
	if (ch == rubword) {			/* word erase */
		if (scanwriteptr <= scanbuffer) goto loop;
		while ((--scanwriteptr >= scanbuffer) &&
			((*scanwriteptr == ' ') || (*scanwriteptr == '\t'))) ;
		scanwriteptr++;
		while ((--scanwriteptr >= scanbuffer) &&
			((*scanwriteptr != ' ') && (*scanwriteptr != '\t'))) ;
		scanwriteptr++;
		scanreadptr = scanbuffer;
		longjmp(scanjumpbuf, SCAN_EDIT);
	}
	if (ch == rubline) {			/* line erase */
		if (scanwriteptr <= scanbuffer) goto loop;
		scanwriteptr = scanbuffer;
		scanreadptr = scanbuffer;
		longjmp(scanjumpbuf, SCAN_EDIT);
	}

store:	if (scanwriteptr >= scanbuffer + SCAN_SIZE) {
		write(STDERR, "\007", 1);
		goto loop;
	}
	*scanwriteptr++ = ch;
	return(*scanreadptr++);
}


/* Abort reading of the current command */
scanabort()
{
	scanreadptr = scanbuffer;
	scanwriteptr = scanbuffer;
	longjmp(scanjumpbuf, SCAN_ABORT);
}


/* Indicate no more characters ready yet */
scaneof()
{
	scanreadptr = scanbuffer;
	longjmp(scanjumpbuf, SCAN_EOF);
}


/* Simply reset input and output pointers without longjmping */
scanreset()
{
	scanreadptr = scanbuffer;
	scanwriteptr = scanbuffer;
}
