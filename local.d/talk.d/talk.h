/*
 *	@(#)talk.h	1.2 12/2/84
*/

#include <stdio.h>
#include <curses.h>
#include <utmp.h>

#define	forever		for(;;)

#define	BUF_SIZE		512
#define NAME_SIZE		9
#define TTY_SIZE		16

/*
 *	Values for answers
*/
#define SUCCESS			0
#define NOT_HERE		1
#define FAILED			2
#define MACHINE_UNKNOWN		3
#define PERMISSION_DENIED	4
#define UNKNOWN_REQUEST		5

extern FILE *popen();
extern int  quit();

extern int curses_initialized;
extern char *state;
extern int line;

typedef struct xwin {
	WINDOW *x_win;
	int x_nlines;
	int x_ncols;
	int x_line;
	int x_col;
	char kill;
	char cerase;
	char werase;
} xwin_t;

extern xwin_t my_win;
extern xwin_t other_win;
extern WINDOW *line_win;

extern char my_tty[];			/* My tty name			*/
extern char my_name[];			/* My user name			*/
extern int mine;			/* My pipe fd			*/

extern char other_tty[];		/* Other tty name		*/
extern char other_name[];		/* Other user name		*/
extern int other;			/* Other pipe fd		*/
