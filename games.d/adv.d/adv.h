#include <stdio.h>
#ifndef CURSED
#include <local/window.h>

#else CURSED
#include <curses.h>
#endif CURSED
#ifndef CURSED
#define	refresh() (WRCurRow = BaseWin->w_cursor.row, \
		   WRCurCol = BaseWin->w_cursor.col, \
		   Wrefresh(0))
#else CURSED
#undef	refresh
typedef	WINDOW	Win;
#define	refresh() (wrefresh(TopWin), wrefresh(BaseWin))
#define	Wrefresh(n) (n ? wrefresh(n) : (wrefresh(TopWin), wrefresh(BaseWin)))
#define	Wclear(w, n) wclear(w)
#define	WAcursor(w, x, y) wmove(w, (x), (y))
#define Wputs(s, w) waddstr(w, s)
#define Wputc(c, w) waddch(w, c)
#define	Wcleanup() endwin()
#define	Wexit(n) (endwin(), exit(n))
#define ROWS LINES
#endif CURSED

#define BOOL		char
#ifndef TRUE
# define TRUE		1
# define FALSE		0
#endif
#define	when		break; case
#define	iconv(c)	((c) & 255)
#define	skpwht(s)	while (*(s) == ' ' || *(s) == '\t') (s)++
#define	alcz(v)		alc(v, 1)
#define	alcnz(v)	alc(v, 0)
#define	lighted(ruum)	(items[lititm].rm == -1 || items[lititm].rm == (ruum))
#define	LINELEN		256
/*#define DEBUG*/

#ifdef DEBUG
#define PDEBUG	/* Playing-debugging */
#endif

FILE	*file;			/* Adventure file for reader */
Win	*TopWin;		/* Top window - room description */
Win	*BaseWin;		/* Base window - commands, etc */
#ifdef CURSED
Win	*CurWin;		/* Current window */
#endif CURSED
int	ROWS, COLS;		/* Screen size */
int	ReadingTerminal;	/* True => in a getchar() */
int	rm;			/* Current room */
int	i_rm;			/* Initial room */
int	trsrm;			/* Treasure room */
int	tottrs;			/* Total treasures */
int	wrdsiz;			/* Significant letters in words */
int	hellrm;			/* "Hell" room # */
int	nitems;			/* # items */
int	nrooms;			/* # rooms */
int	npcts;			/* # %s */
int	nactns;			/* # actns */
int	nverbs;			/* # verbs */
int	nobjs;			/* # objs */
int	nmsgs;			/* # msgs */
int	verbv;			/* Verb wordvalue */
int	objv;			/* Object wordvalue if any */
int	lititm;			/* Lit lamp item # */
int	littim;			/* Current light time */
int	litmax;			/* Max light time */
int	warnpt;			/* When to say 'light runs out...' */
int	maxinv;			/* Max carry */

struct room {
	int dirs[6];		/* Where to - NSEWUD */
	BOOL dark;		/* Set if it's dark */
	char *desc;		/* Room description */
} *rooms, *roome;

struct item {
	int i_rm, rm;		/* Initial/Current Rooms */
	char *name;		/* Name by which to GET */
	char *desc;		/* Description */
} *items, *iteme;

struct word {
	int val;		/* Value */
	char *word;		/* Word */
};

struct word *verbs, *verbe;
struct word *objs, *obje;
char	**pct,			/* Base addr for %s */
	**actn,			/* Base addr for actions */
	**msg,			/* Base addr for msgs */
	*name,			/* The poor sap's name */
	*dirs[6],		/* Names of directions */
	*toomch,		/* "I've too much to carry" */
	*qcom,			/* Special commands/objects */
	*scom,
	*lcom,
	*gobj,
	var[256],		/* Gen. purpose vars/flags */
	fname[LINELEN+1];	/* The source adventure file */
char	linebuf[LINELEN+1],
	buf2[LINELEN+1];
BOOL	isdark,			/* If it's dark */
	isend;			/* Set when game done */

char *strsav(), *alc(), *rdr(), *nxtwrd(), *itoa();
