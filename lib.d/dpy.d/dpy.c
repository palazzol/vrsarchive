static char *sccsid = "@(#)dpy.c	1.46	2/27/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";


/*
 * Different curses package (by David I. Bell).
 * Modified to run under System V by Alan P.W. Hewett.
 * Modified to run under System 3 Xenix by V. R. Slyngstad
 * These dpy module sources are in the public domain, and can be copied
 * or used as desired, with the following restrictions:
 * 1.	All copyright notices (and this notice) must be preserved.
 * 2.	The dpy sources (even if modified) cannot be sold for profit.
 * 3.	If any sources are modified, a sentence must exist by the
 *	copyright notice of each modified source file which says that
 *	the file has been modified.
 */


#include <stdio.h>		/* standard I/O */
#include <signal.h>		/* signals */
#include "dpy.h"		/* window definitions */

#define	DEL	0177		/* delete character */
#define	EOL	'\n'		/* end of line character */
#define	RET	'\r'		/* return character */
#define	BS	'\b'		/* backspace character */
#define	TAB	'\t'		/* tab character */
#define	SPACE	' '		/* space character */
#define	TRUE	1		/* true value */
#define	FALSE	0		/* false value */
#define	INTSIZ	(sizeof(int))	/* size of an integer */

extern	short	ospeed;		/* output speed for tputs to use */
extern	char	PC;		/* pad character for tputs to use */
struct	window	window;		/* the window */
static	int	dpytputs();	/* called by tputs to type to terminal */
static	int	dpysputs();	/* called by tputs to save characters */
int	dpystop();		/* stop routine */
char	*tgetstr();		/* return termcap string */
char	*malloc();		/* allocate memory */
char	*getenv();		/* return environment variable */


/*
 * Initialize the window structure.  Returns nonzero on failure with a
 * message already typed.  The ttytype argument is the terminal type string,
 * or NULL if the TERM environment variable is to be used.  Modes is a
 * string whose characters describe the desired state of the terminal.
 * These are:
 * 	+	turn on mode indicated by next character (default)
 *	-	turn off mode indicated by next character.
 *	c	cbreak mode (return each character as typed, without waiting
 *		for an end of line, and leave signal processing on).
 *	e	echoing of typed-in characters is enabled.
 *	r	raw mode (return each character as typed, no canonical or
 *		signal processing of any kind).
 *	<SP>	spaces are ignored
 * So the terminal modes before dpy runs are normally "e -c -r".
 * A NULL modes pointer defaults the modes to "-e c".
 */
dpyinit(ttytype, modes)
	register char	*ttytype;	/* terminal type, or NULL for default */
	char	*modes;			/* terminal modes */
{
	register struct	window	*wp;	/* window pointer */
	register char	*cp;		/* character pointer */
	register char	*sp;		/* pointer for spaces */
	register int	size;		/* size of character array */
	int on;				/* whether or not mode is on */
#ifdef BSD
	struct	ltchars	ltchars;	/* local terminal characters */
	struct	tchars	tchars;		/* terminal characters */
#endif BSD

	wp = &window;
	wp->inited = FALSE;
	wp->output = FALSE;
	if (ttytype == NULL) {
		ttytype = getenv("TERM");
		if (ttytype == NULL) {
			fprintf(stderr, "dpyinit: TERM not defined\n");
			return(1);
		}
	}
	size = tgetent(wp->tdata, ttytype);
	if (size <= 0) {
		fprintf(stderr, size ? "dpyinit: cannot open termcap file\n"
			: "dpyinit: unknown terminal type %s\n", ttytype);
		return(1);
	}
	/*
	 * Collect current tty modes, and remember editing characters
	 */
	wp->c_kill = CKILL;		/* init in case stdin is not a tty */
	wp->c_erase = CERASE;
	wp->c_werase = CWERASE;
	wp->c_lnext = CLNEXT;
	wp->c_rprnt = CRPRNT;
	wp->c_eof = CEOF;
#ifdef	BSD
	if (ioctl(STDIN, TIOCGETP, &wp->old0ttyblk) == 0) {
		wp->c_erase = wp->old0ttyblk.sg_erase;
		wp->c_kill = wp->old0ttyblk.sg_kill;
	}
	if (ioctl(STDIN, TIOCGLTC, &ltchars) == 0) {
		wp->c_werase = ltchars.t_werasc;
		wp->c_rprnt = ltchars.t_rprntc;
		wp->c_lnext = ltchars.t_lnextc;
	}
	if (ioctl(STDIN, TIOCGETC, &tchars) == 0) {
		wp->c_eof = tchars.t_eofc;
	}
	wp->old1ttyblk.sg_ospeed = 0;
	ioctl(STDOUT, TIOCGETP, &wp->old1ttyblk);
	ospeed = wp->old1ttyblk.sg_ospeed;	/* save speed for tputs */
#endif	BSD
#ifdef	USG
	if (ioctl(STDIN, TCGETA, &wp->old0ttyblk) == 0) {
		wp->c_erase = wp->old0ttyblk.c_cc[VERASE];
		wp->c_kill = wp->old0ttyblk.c_cc[VKILL];
		wp->c_eof = wp->old0ttyblk.c_cc[VEOF];
	}
	ioctl(STDOUT, TCGETA, &wp->old1ttyblk);
#endif	USG
	/*
	 * Collect terminal capability strings
	 */
	wp->nrows = tgetnum("li");
	wp->ncols = tgetnum("co");
	wp->tc_am = tgetflag("am");
	wp->tbuf[0] = '\0';
	wp->tptr = wp->tbuf;
	(void) tgetstr("pc", &wp->tptr);
	PC = wp->tbuf[0];
	wp->tptr = wp->tbuf;
	wp->tc_ho = tgetstr("ho", &wp->tptr);
	wp->tc_hocc = wp->tptr - wp->tbuf - 1;
	wp->tc_ce = tgetstr("ce", &wp->tptr);
	wp->tc_cd = tgetstr("cd", &wp->tptr);
	wp->tc_cm = tgetstr("cm", &wp->tptr);
	if ((wp->nrows <= 0) || (wp->ncols <= 0) || (wp->tc_ce == NULL)
		|| (wp->tc_cd == NULL) || (wp->tc_cm == NULL)) {
			fprintf(stderr, "dpyinit: missing termcap entry\n");
			return(1);
	}
	sp = wp->tptr;			/* apply padding to clear screen */
	tputs(wp->tc_cd, wp->nrows, dpysputs);
	wp->tc_cd = sp;
	wp->tc_cdcc = wp->tptr - sp;
	sp = wp->tptr;			/* and to clear line string */
	tputs(wp->tc_ce, 1, dpysputs);
	wp->tc_ce = sp;
	wp->tc_cecc = wp->tptr - sp;
	if (wp->tc_ho == NULL) {	/* make home up string if not defined */
		sp = wp->tptr;
		tputs(tgoto(wp->tc_cm, 0, 0), 1, dpysputs);
		wp->tc_ho = sp;
		wp->tc_hocc = wp->tptr - sp;
	}
	wp->delta = (wp->ncols + INTSIZ) &~ (INTSIZ-1);	/* round up */
	size = wp->nrows * wp->delta;
	cp = malloc(2 * (size + INTSIZ));
	if (cp == NULL) {
		fprintf(stderr, "dpyinit: failed to allocate memory\n");
		return(1);
	}
	wp->begdata = cp;
	wp->enddata = cp + size;
	wp->begwin = cp;
	wp->endwin = cp + size - wp->delta;
	wp->begrow = cp;
	wp->endrow = cp + wp->ncols;
	wp->cp = cp;
	wp->screen = cp + size + INTSIZ;
	for (sp = cp + (2 * (size + INTSIZ)) - 1; sp >= cp; sp--) *sp = SPACE;
	*((int *)(cp + size)) = 0;	/* terminate end of screens */
	*((int *)(wp->screen + size)) = 0;
	wp->currow = 0;
	wp->curcol = 0;
	wp->noctrl = 0;
	wp->nocrlf = 0;
	wp->nomove = 0;
	wp->scroll = 0;
	wp->full = 0;
	wp->tabsize = 8;
	wp->begchange = wp->enddata;
	wp->endchange = wp->begdata;
	/*
	 * Copy old tty modes to new ones, and modify them as specified
	 */
	wp->new0ttyblk = wp->old0ttyblk;
	wp->new1ttyblk = wp->old1ttyblk;
	if (modes == (char*)NULL) modes = "-e c";
	on = TRUE;
	for (cp = modes; *cp ; cp++) {		/* scan mode string */
		switch (*cp) {
		case ' ':			/* spaces (ignored) */
			continue;
		case '+':			/* turn on next mode */
			on = TRUE;
			continue;
		case '-':			/* turn off next mode */
			on = FALSE;
			continue;
#ifdef	BSD
		case 'e':			/* enable echoing */
			if (on) {
				wp->new0ttyblk.sg_flags |= ECHO;
				wp->new1ttyblk.sg_flags |= ECHO;
			} else {
				wp->new0ttyblk.sg_flags &= ~ECHO;
				wp->new1ttyblk.sg_flags &= ~ECHO;
			}
			break;

		case 'c':			/* enable character mode */
			if (on) {
				wp->new0ttyblk.sg_flags |= CBREAK;
				wp->new1ttyblk.sg_flags |= CBREAK;
			} else {
				wp->new0ttyblk.sg_flags &= ~CBREAK;
				wp->new1ttyblk.sg_flags &= ~CBREAK;
			}
			break;

		case 'r':			/* enable raw mode */
			if (on) {
				wp->new0ttyblk.sg_flags |= RAW;
				wp->new1ttyblk.sg_flags |= RAW;
			} else {
				wp->new0ttyblk.sg_flags &= ~RAW;
				wp->new1ttyblk.sg_flags &= ~RAW;
			}
			break;
#endif	BSD
#ifdef	USG
		case 'e':			/* enable echoing */
			if (on) {
				wp->new0ttyblk.c_lflag |= ECHO | ECHOE | ECHOK ;
				wp->new1ttyblk.c_lflag |= ECHO | ECHOE | ECHOK ;
			} else {
				wp->new0ttyblk.c_lflag &= ~(ECHO|ECHOE|ECHOK) ;
				wp->new1ttyblk.c_lflag &= ~(ECHO|ECHOE|ECHOK) ;
			}
			break ;

		case 'c':			/* enable character mode */
			if (on) {
				wp->new0ttyblk.c_iflag |= ISTRIP ;
				wp->new0ttyblk.c_lflag &= ~ICANON ;
				wp->new0ttyblk.c_cc[VMIN] = 1 ;
				wp->new0ttyblk.c_cc[VTIME] = 0 ;
				wp->new1ttyblk.c_iflag |= ISTRIP ;
				wp->new1ttyblk.c_lflag &= ~ICANON ;
				wp->new1ttyblk.c_cc[VMIN] = 1 ;
				wp->new1ttyblk.c_cc[VTIME] = 0 ;
			} else {
	    			wp->new0ttyblk.c_iflag |= (ICRNL|IUCLC) ;
	    			wp->new0ttyblk.c_lflag |= ICANON ;
	    			wp->new0ttyblk.c_cc[VEOF] =
					wp->old0ttyblk.c_cc[VEOF] ;
	    			wp->new0ttyblk.c_cc[VEOL] =
					wp->old0ttyblk.c_cc[VEOL] ;
	    			wp->new1ttyblk.c_iflag |= (ICRNL|IUCLC) ;
	    			wp->new1ttyblk.c_lflag |= ICANON ;
	    			wp->new1ttyblk.c_cc[VEOF] =
					wp->old1ttyblk.c_cc[VEOF] ;
	    			wp->new1ttyblk.c_cc[VEOL] =
					wp->old1ttyblk.c_cc[VEOL] ;
			}
			break ;

		case 'r':			/* enable raw mode */
			if (on) {
	    			wp->new0ttyblk.c_iflag &=
					~(BRKINT|IGNPAR|ISTRIP|IXON|IXANY) ;
	    			wp->new0ttyblk.c_oflag &= ~OPOST ;
	    			wp->new0ttyblk.c_cflag =
					(wp->new0ttyblk.c_cflag|CS8) & ~PARENB ;
	    			wp->new0ttyblk.c_lflag &= ~ICANON ;
	    			wp->new0ttyblk.c_cc[VMIN] = 1 ;
	    			wp->new0ttyblk.c_cc[VTIME] = 0 ;
	    			wp->new1ttyblk.c_iflag &=
					~(BRKINT|IGNPAR|ISTRIP|IXON|IXANY) ;
	    			wp->new1ttyblk.c_oflag &= ~OPOST ;
	    			wp->new1ttyblk.c_cflag =
					(wp->new1ttyblk.c_cflag|CS8) & ~PARENB ;
	    			wp->new1ttyblk.c_lflag &= ~ICANON ;
	    			wp->new1ttyblk.c_cc[VMIN] = 1 ;
	    			wp->new1ttyblk.c_cc[VTIME] = 0 ;
			} else {
	    			wp->new0ttyblk.c_iflag |=
					(BRKINT|IGNPAR|ISTRIP|IXON
					|IXANY|ICRNL|IUCLC) ;
	    			wp->new0ttyblk.c_oflag |= OPOST ;
	    			wp->new0ttyblk.c_cflag =
					(wp->new0ttyblk.c_cflag & ~CSIZE) |
					CS7 | PARENB ;
	    			wp->new0ttyblk.c_lflag |= ICANON | ISIG ;
	    			wp->new0ttyblk.c_cc[VEOF] = CEOF ;
	    			wp->new0ttyblk.c_cc[VEOL] = 0 ;
#ifdef VEOL2
	    			wp->new0ttyblk.c_cc[VEOL2] = 0 ;
#endif
	    			wp->new1ttyblk.c_iflag |=
					(BRKINT|IGNPAR|ISTRIP|IXON|
					IXANY|ICRNL|IUCLC) ;
	    			wp->new1ttyblk.c_oflag |= OPOST ;
	    			wp->new1ttyblk.c_cflag =
					(wp->new1ttyblk.c_cflag & ~CSIZE) |
					CS7 | PARENB ;
	    			wp->new1ttyblk.c_lflag |= ICANON | ISIG ;
	    			wp->new1ttyblk.c_cc[VEOF] = CEOF ;
	    			wp->new1ttyblk.c_cc[VEOL] = 0 ;
#ifdef VEOL2
	    			wp->new1ttyblk.c_cc[VEOL2] = 0 ;
#endif
			}
			break;
#endif	USG
		default:
			fprintf(stderr, "dpyinit: illegal flag: %c%c\n",
				(on ? '+' : '-'), *cp);
			return(1);
		}
		on = TRUE;		/* reset mode */
	}
	/*
	 * Set the new modes for real
	 */
#ifdef	BSD
	wp->new1ttyblk.sg_flags &= ~XTABS;
	signal(SIGTSTP, dpystop);
	ioctl(STDIN, TIOCSETP, &wp->new0ttyblk);
	ioctl(STDOUT, TIOCSETP, &wp->new1ttyblk);
#endif	BSD
#ifdef	USG
	wp->new1ttyblk.c_oflag &= ~TAB3 ;
	ioctl(STDIN,TCSETAW,&wp->new0ttyblk) ;
	ioctl(STDOUT,TCSETAW,&wp->new1ttyblk) ;
#endif	USG	
	wp->inited = TRUE;
	return(0);
}


/*
 * Terminate the window, home down to the bottom of the screen, and reset
 * the terminal modes to their original state.
 */
dpyclose()
{
	register struct	window	*wp;	/* window pointer */

	wp = &window;
	if (wp->inited) {
		wp->inited = FALSE;
		if (wp->output) {
			domove(wp->nrows - 1, 0, (char *)NULL);
			fwrite(wp->tc_ce, 1, wp->tc_cecc, stdout);
			fflush(stdout);
		}
		free(wp->begdata);
#ifdef	BSD
		ioctl(STDIN, TIOCSETP, &wp->old0ttyblk);
		ioctl(STDOUT, TIOCSETP, &wp->old1ttyblk);
#endif	BSD
#ifdef	USG
		ioctl(STDIN, TCSETAW, &wp->old0ttyblk);
		ioctl(STDOUT, TCSETAW, &wp->old1ttyblk);
#endif	USG
	}
	return(0);
}


/*
 * Put a given number of characters to the window at the current write location.
 * Certain control characters have effects, others print as ^X or are ignored.
 * Automatic wrapping to the next line is possible, and scrolling when the last
 * line is full. Returns nonzero if the window cannot hold the whole buffer.
 */
dpywrite(buf, count)
	register char	*buf;		/* buffer address */
	int	count;			/* number of characters */
{
	register struct	window	*wp;	/* window pointer */
	register char	*endbuf;	/* end of buffer to write */
	register char	*cp;		/* current character pointer */
	register int	ch;		/* character to store */

	wp = &window;
	if (wp->full) return(1);
	cp = wp->cp;
	if (cp < wp->begchange) wp->begchange = cp;
	for (endbuf = buf + count; buf < endbuf; buf++) {
		ch = *buf;
		if (ch < ' ') {			/* control character */
			if (ch == EOL) {	/* new line */
				clear(cp, wp->endrow);
				if (cp >= wp->endwin) {	/* window full */
					wp->endchange = wp->endrow;
					if (wp->scroll == 0) {
						wp->full = 1;
						wp->cp = wp->begrow;
						return(1);
					}
					wp->cp = cp;
					dpyscroll();
					cp = wp->begrow;
					continue;
				}
				wp->begrow += wp->delta;
				wp->endrow += wp->delta;
				cp = wp->begrow;
				continue;
			}
			if (ch == TAB) {	/* tab */
				wp->cp = cp;
				do {
					if (dpywrite(" ", 1)) return(1);
				} while ((wp->cp - wp->begrow) % wp->tabsize);
				cp = wp->cp;
				continue;
			}
			if (ch == BS) {		/* backspace */
				if (cp > wp->begrow) cp--;
				continue;
			}
			if (ch == RET) {	/* return character */
				cp = wp->begrow;
				continue;
			}
			/*
			 * Obscure control character, show as ^X
			 */
			if (wp->noctrl) continue;
			wp->cp = cp;
			if (dpywrite("^", 1) || dpychar(ch + '@')) return(1);
			cp = wp->cp;
			continue;
		}
		if (ch == DEL) {		/* delete character */
			if (wp->noctrl) continue;
			wp->cp = cp;
			if (dpywrite("^?", 2)) return(1);
			cp = wp->cp;
			continue;
		}
		/*
		 * Normal printing character
		 */
		if (cp >= wp->endrow) {		/* end of row, see if do crlf */
			wp->cp = cp;
			if (cp > wp->endchange) wp->endchange = cp;
			if (wp->nocrlf) return(1);
			if (cp >= wp->endwin) {
				if (wp->scroll == 0) return(1);
				dpyscroll();
				cp = wp->begrow;
				*cp++ = ch;
				continue;
			}
			wp->begrow += wp->delta;
			wp->endrow += wp->delta;
			cp = wp->begrow;
		}
		*cp++ = ch;
	}
	wp->cp = cp;
	if (cp > wp->endchange) wp->endchange = cp;
	return(0);
}


/*
 * Put a single character to the window.
 * Returns nonzero if full.
 */
dpychar(ch)
	char	ch;
{
	return(dpywrite(&ch, 1));
}


/*
 * Put a null-terminated string to the window.
 * Returns nonzero if full.
 */
dpystr(str)
	char	*str;
{
	return(dpywrite(str, strlen(str)));
}



/*
 * Print a formatted string to the window.  Returns nonzero if full.
 * This routine is a ripped off version of sprintf.  This routine is
 * machine-dependent!!
 */
#ifdef	BSD
dpyprintf(fmt, args)
	char	*fmt;			/* format string */
{
	FILE	_strbuf;		/* file header */
	char	buf[5000];		/* data storage */

	_strbuf._flag = _IOWRT+_IOSTRG;
	_strbuf._ptr = buf;
	_strbuf._cnt = 32767;
	_doprnt(fmt, &args, &_strbuf);
	return(dpywrite(buf, _strbuf._ptr - buf));
}
#endif	BSD
#ifdef	USG

#include <varargs.h>

dpyprintf(format, va_alist)
char *format;
va_dcl
{
	register int count;
	FILE siop;
	va_list ap;
	char	buf[5000];		/* data storage */

	siop._cnt = sizeof(buf) ;
	siop._base = siop._ptr = &buf[0] ;
	siop._flag = _IOWRT;
	siop._file = _NFILE;
	va_start(ap);
	count = _doprnt(format, ap, &siop);
	va_end(ap);
	*siop._ptr = '\0'; /* plant terminating null character */
	return(dpywrite(buf, siop._ptr - buf));
}
#endif	USG


/* Clear to the end of the current row without changing the write location */
dpyclrline()
{
	register struct	window	*wp;	/* window pointer */
	register char	*cp;		/* current character */
	register char	*endcp;		/* ending character */

	wp = &window;
	if (wp->full) return;
	cp = wp->cp;
	endcp = wp->endrow;
	if (cp < wp->begchange) wp->begchange = cp;
	if (endcp > wp->endchange) wp->endchange = cp;
	clear(cp, endcp);
}


/* Clear to the end of the window without changing the write location */
dpyclrwindow()
{
	register struct	window	*wp;	/* window pointer */
	register char	*begcp;		/* beginning character */
	register char	*cp;		/* current character */
	register char	*endcp;		/* ending character */

	wp = &window;
	if (wp->full) return;
	begcp = wp->begrow;
	endcp = wp->endrow;
	cp = wp->cp;
	if (cp < wp->begchange) wp->begchange = cp;
	while (1) {
		clear(cp, endcp);
		if (begcp >= wp->endwin) break;
		begcp += wp->delta;
		endcp += wp->delta;
		cp = begcp;
	}
	if (endcp > wp->endchange) wp->endchange = endcp;
}


/* Set the current write position to the top left corner of the window */
dpyhome()
{
	register struct	window	*wp;	/* window pointer */

	wp = &window;
	wp->endrow += wp->begwin - wp->begrow;
	wp->begrow = wp->begwin;
	wp->cp = wp->begrow;
	wp->full = 0;
}


/* Scroll the current window upwards a line to make room for more data. */
dpyscroll()
{
	register struct	window	*wp;	/* window pointer */
	register char	*currow;	/* beginning of current row */
	register char	*nextrow;	/* beginning of next row */
	register int	cols;		/* columns in window */

	wp = &window;
	cols = wp->endrow - wp->begrow;
	currow = wp->begwin;
	nextrow = currow + wp->delta;
	while (currow < wp->endwin) {		/* move each line up */
		bcopy(nextrow, currow, cols);
		currow += wp->delta;
		nextrow += wp->delta;
	}
	clear(currow, currow + cols);		/* clear last line */
	wp->begchange = wp->begwin;
	wp->endchange = wp->endwin + cols;
}


/*
 * Return the row number being written to, or -1 if out of the window.
 * The row number is relative to the beginning of the window.
 */
dpygetrow()
{
	register struct	window	*wp;	/* window pointer */

	wp = &window;
	if (wp->full) return(-1);
	return((wp->cp - wp->begwin) / wp->delta);
}


/*
 * Return the column number being written to, or -1 if out of the window.
 * The column number is relative to the current window.
 */
dpygetcol()
{
	register struct	window	*wp;	/* window pointer */

	wp = &window;
	if (wp->full) return(-1);
	if (wp->cp < wp->endrow) return(wp->cp - wp->begrow);
	if (wp->nocrlf) return(-1);
	return(0);
}


/* Make the screen match the data as previously written by the user */
dpyupdate()
{
	register struct	window	*wp;	/* window pointer */
	register char	*scp;		/* screen character pointer */
	register char	*cp;		/* current character */
	register char	*spcp;		/* cp where spaces remain in row */
	register char	*endrow;	/* end of row */
	register char	*begrow;	/* beginning of row */
	register int	row;		/* current row number */
	int	diff;

	wp = &window;
	if (wp->output == 0) {		/* first output, clear screen */
		wp->output = TRUE;
		fwrite(wp->tc_ho, 1, wp->tc_hocc, stdout);
		fwrite(wp->tc_cd, 1, wp->tc_cdcc, stdout);
	}
	cp = wp->begchange;
	scp = wp->screen + (cp - wp->begdata);
	endrow = 0;
	while (cp < wp->endchange) {	/* look for a difference */
		diff = strdif(cp, scp, wp->endchange - cp);
		cp += diff;
		scp += diff;
		if (cp >= wp->endchange) break;
		if (cp >= endrow) {
			row = (cp - wp->begdata) / wp->delta;
			begrow = wp->begdata + (row * wp->delta);
			endrow = begrow + wp->ncols;
			spcp = endrow - 1;
			while ((spcp >= begrow) && (*spcp == SPACE))
				spcp--;
			spcp++;
		}
		domove(row, cp - begrow, begrow);
		if (cp >= spcp) {		/* clear rest of line */
			fwrite(wp->tc_ce, 1, wp->tc_cecc, stdout);
			while (cp < endrow) {
				*scp++ = SPACE;
				cp++;
			}
			continue;
		}
		putchar(*cp);
		*scp++ = *cp++;
		if (++wp->curcol >= wp->ncols) {	/* fixup last column */
			wp->curcol--;
			if (wp->tc_am) {
				wp->currow++;
				wp->curcol = 0;
			}
		}
	}
	wp->begchange = wp->enddata;
	wp->endchange = wp->begdata;
	if (wp->nomove == 0) dpycursor();
	fflush(stdout);
}


/*
 * Set the terminal cursor at the current write location.
 * If the window is full, the cursor is placed at the front of the
 * last line in the window.  If lines are not being wrapped and the
 * line is full, the cursor is placed at the end of the line.
 * Otherwise, the cursor is placed at the location being written to next.
 */
static
dpycursor()
{
	register struct	window	*wp;	/* window pointer */
	register char	*cp;		/* current write location */
	register char	*begrow;	/* beginning of current row */
	register int	row;		/* row number */

	wp = &window;
	cp = wp->cp;
	if (wp->full)
		cp = wp->endwin;
	else if (cp >= wp->endrow) {
		if (wp->nocrlf || (wp->begrow >= wp->endwin))
			cp = wp->endrow - 1;
		else
			cp = wp->begrow + wp->delta;
	}
	row = (cp - wp->begdata) / wp->delta;
	begrow = wp->begdata + (row * wp->delta);
	domove(row, cp - begrow, begrow);
}


/*
 * Subroutine to move to the given location on the screen.  The third argument
 * is a pointer to beginning of the desired row in case we find it is faster
 * to type the intervening characters.  If NULL, we must use addressing.
 */
static
domove(row, col, cp)
	register int	row;		/* desired row */
	register int	col;		/* desired column */
	register char	*cp;		/* data on desired row */
{
	register struct	window	*wp;	/* window structure */

	wp = &window;
	if (cp && (row == wp->currow) && (col >= wp->curcol)
		&& (col < wp->curcol + 6)) {		/* a few ahead */
			cp += wp->curcol;
			while (wp->curcol < col) {
				putchar(*cp);
				cp++;
				wp->curcol++;
			}
			return;
	}
	if ((col == 0) && (row == wp->currow + 1)) {	/* next row */
		putchar('\n');
		wp->currow++;
		wp->curcol = 0;
		return;
	}
	tputs(tgoto(wp->tc_cm, col, row), 1, dpytputs);	/* arbitrary */
	wp->currow = row;
	wp->curcol = col;
}


/* Local routine called by tputs to print a character */
static
dpytputs(ch)
	char	ch;
{
	putchar(ch);
}


/* Local routine called by tputs to save a character */
static
dpysputs(ch)
	char	ch;
{
	*window.tptr++ = ch;
}


/* Redraw the screen to fix glitches */
dpyredraw()
{
	register struct	window	*wp;	/* window pointer */
	register char	*cp;		/* current character */
	register char	*endcp;		/* ending character */

	wp = &window;
	cp = wp->screen;
	endcp = cp + (wp->nrows * wp->delta);
	clear(cp, endcp);
	wp->currow = 0;
	wp->curcol = 0;
	wp->begchange = wp->begdata;
	wp->endchange = wp->enddata;
	fwrite(wp->tc_ho, 1, wp->tc_hocc, stdout);
	fwrite(wp->tc_cd, 1, wp->tc_cdcc, stdout);
	dpyupdate();
}


/*
 * Routine called on a terminal stop signal.  Restore the original terminal
 * state, home down to the bottom, and really stop.  If continued, restore
 * the new terminal state and redraw the screen.
 */
dpystop()
{
#ifdef	BSD
	register struct	window	*wp;	/* window pointer */

	wp = &window;
	if (wp->output) {
		domove(wp->nrows - 1, 0, (char *)NULL);
		fflush(stdout);
	}
	ioctl(STDIN, TIOCSETP, &wp->old0ttyblk);
	ioctl(STDOUT, TIOCSETP, &wp->old1ttyblk);
	kill(getpid(), SIGSTOP);	/* really stop */
	ioctl(STDIN, TIOCSETP, &wp->new0ttyblk);
	ioctl(STDOUT, TIOCSETP, &wp->new1ttyblk);
	if (wp->output) dpyredraw();
#endif	BSD
}
