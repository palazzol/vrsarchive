/*
 * @(#)dpy.h	1.12	2/27/85
 * @(#)Copyright (C) 1985 by D Bell
 * Modified for USG by Alan P.W. Hewett
 */

#ifdef	BSD
#include <sys/ioctl.h>
#endif	BSD

#ifdef	USG
#ifdef  M_XENIX
#  include <sys/types.h>
#endif
#include <sys/termio.h>
#ifndef  TCGETA
#  include <sys/ioctl.h>
#endif
#define	CTRL(c)	('c'&037)
#undef	CERASE
#define	CERASE	CTRL(h)
#define	CRPRNT	CTRL(r)
#define	CWERASE	CTRL(w)
#define	CLNEXT	CTRL(v)
#ifndef M_XENIX
#define	bcopy(from,to,size)	memcpy(to,from,size)
#endif
#endif	USG

#define	STDIN	0		/* standard descriptors */
#define	STDOUT	1
#define	STDERR	2

/*
 * The window structure contains data describing the state of the screen.
 * The following picture shows a typical screen and window, and shows the
 * relationship of each of the character pointers in the structure.
 *
 *	A----------------------|
 *	|                      |
 *	|    C--------|        |
 *	|    |        |        |
 *	|    E     G  |F       |
 *	|    |        |        |
 *	|    D--------|        |
 *	|                      |
 *	|----------------------|
 *	B
 */
struct	window	{
	char	*begdata;		/* A: beginning of new screen image */
	char	*enddata;		/* B: end of screen image */
	char	*begwin;		/* C: beginning of current window */
	char	*endwin;		/* D: last row of current window */
	char	*begrow;		/* E: beginning of current row */
	char	*endrow;		/* F: end of current row */
	char	*cp;			/* G: current character pointer */
	char	*begchange;		/* beginning of changed region */
	char	*endchange;		/* end of changed region */
	char	*screen;		/* the actual screen image */
	int	nrows;			/* number of rows on screen */
	int	ncols;			/* number of columns on screen */
	int	delta;			/* distance between rows */
	int	currow;			/* current row column */
	int	curcol;			/* cursor column column */
	int	tabsize;		/* current tab size */
	char	nocrlf;			/* don't do automatic crlfs */
	char	noctrl;			/* don't show control characters */
	char	nomove;			/* don't move cursor when done */
	char	scroll;			/* scroll screen when end reached */
	char	inited;			/* we are initialized */
	char	output;			/* we have output to screen */
	char	full;			/* window is full */
	char	c_kill;			/* line erase character */
	char	c_erase;		/* character erase character */
	char	c_werase;		/* word erase character */
	char	c_rprnt;		/* retype line character */
	char	c_eof;			/* end of file character */
	char	c_lnext;		/* literal input character */
	char	tc_am;			/* cursor wraps on last column */
	char	tc_hocc;		/* character count for tc_ho */
	char	tc_cecc;		/* character count for tc_ce */
	char	tc_cdcc;		/* character count for tc_cd */
	char	*tc_ho;			/* home capability */
	char	*tc_ce;			/* clear to end of line */
	char	*tc_cd;			/* clear to end of display */
	char	*tc_cm;			/* move cursor */
	char	*tptr;			/* pointer into termcap strings */
#ifdef	BSD
	struct	sgttyb	old0ttyblk;	/* original tty parameters for stdin */
	struct	sgttyb	new0ttyblk;	/* new terminal parameters for stdin */
	struct	sgttyb	old1ttyblk;	/* original tty parameters for stdout */
	struct	sgttyb	new1ttyblk;	/* new terminal parameters for stdout */
#endif	BSD
#ifdef	USG
	struct	termio	old0ttyblk;	/* original tty parameters for stdin */
	struct	termio	new0ttyblk;	/* new terminal parameters for stdin */
	struct	termio	old1ttyblk;	/* original tty parameters for stdout */
	struct	termio	new1ttyblk;	/* new terminal parameters for stdout */
#endif	USG
	char	tdata[1024];		/* buffer for termcap data */
	char	tbuf[200];		/* strings for termcap */
};

extern	struct	window	window;		/* window structure */
