/*
 *	@(#)talk.h	1.3 12/3/84
*/

#include <stdio.h>
#include <curses.h>
#include <utmp.h>

#define	forever		for(;;)

#define	BUF_SIZE		512
#define NAME_SIZE		9	/* Size of a login name		*/
#define SYS_SIZE		11	/* Size of a system name	*/
#define TTY_SIZE		16	/* Size of a tty name		*/

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

#ifdef MAIN
#  define _extern
#else
#  define _extern extern
#endif

_extern int curses_initialized;
_extern char *state;
_extern int line;

_extern xwin_t my_win;
_extern xwin_t other_win;
_extern WINDOW *line_win;

_extern char my_tty[TTY_SIZE];		/* My tty name			*/
_extern char my_name[NAME_SIZE];	/* My user name			*/
_extern char my_system[SYS_SIZE];	/* My system name		*/
_extern int mine;			/* My pipe fd			*/

_extern char other_tty[TTY_SIZE];	/* Other tty name		*/
_extern char other_name[NAME_SIZE];	/* Other user name		*/
_extern char other_system[SYS_SIZE];	/* Other system name		*/
_extern int other;			/* Other pipe fd		*/
