/*
 *	These are routines to manage the virtual displays.
*/
#include <stdio.h>
#include <ctype.h>
#include <curses.h>
#include "globals.h"

extern void done();
extern char *malloc();

struct screen {
	WINDOW *display;		/* Current screen contents	*/
	FILE *trace_fd;			/* File for debug info		*/
	char *tstop;			/* Current tab stops		*/
	short cm_col;			/* Col for incomplete cursor mv	*/
	short cm_row;			/* Row for incomplete cursor mv	*/
	char vstate;			/* State of the virtual display	*/
	char xn_flag;			/* True if xn wrap in progress	*/
} screen[MAXVDISP];

#define V_NORM		0		/* Normal state			*/
#define V_ESC		1		/* Just got an escape		*/
#define V_ANSII		2		/* ANSII escape sequence	*/
#define V_CM		3		/* Expecting column		*/
#define V_FLAG		4		/* Expecting flag value		*/

char *tracefile[] = {
	"win0",
	"win1",
	"win2",
	"win3",
	"win4",
	"win5",
	"win6",
	"win7"
};

/*
 *	Loop over all virtual screens, allocating and initializing
 *	resources.  Initialize the physical display.
*/
vdisp_init()
{	int i;

	pdisp_init();
	for (curvdsp = 0; curvdsp < MAXVDISP; curvdsp++) {
		screen[curvdsp].vstate = V_NORM;
		screen[curvdsp].display = newwin(LINES, COLS, 0, 0);
		if (screen[curvdsp].display == NULL) {
			perror("vdisp_init newwin");
			done(1);
		}
		scrollok(screen[curvdsp].display, TRUE);
		werase(screen[curvdsp].display);
		screen[curvdsp].tstop = malloc((unsigned)COLS);
		if (screen[curvdsp].tstop == NULL) {
			perror("vdisp_init malloc");
			done(1);
		}
		for (i = 0; i < COLS; i++)
			screen[curvdsp].tstop[i] = (i%8 == 0);
		if (trace) {
			screen[curvdsp].trace_fd=fopen(tracefile[curvdsp],"w");
			if (screen[curvdsp].trace_fd == NULL) {
				perror("vdisp_init trace fopen");
				done(1);
			}
		}
	}
	curvdsp = 0;
}

/*
 *	Move the current logical to physical mapping up by one.
*/
next_vdisp()
{
	curvdsp++;
	if (curvdsp >= MAXVDISP)
		curvdsp = 0;
	touchwin(screen[curvdsp].display);
	wrefresh(screen[curvdsp].display);
}

/*
 *	Put a character onto a given virtual display.  We behave like a
 *	terminal here, interpreting escape sequences and worrying about
 *	the screen and cursor position.
*/
wputc(vdisp, c)
register int vdisp;
register char c;
{
	int y, x;
	WINDOW *vd;
#define wmove_xn(y, x)	(wmove(vd, y, x), screen[vdisp].xn_flag = 0)

	c &= 0177;
	if (trace)
		fputc(c, screen[vdisp].trace_fd);
	vd = screen[vdisp].display;
	getyx(vd, y, x);
	switch (screen[vdisp].vstate) {
	case V_NORM:
		switch(c) {
		default:
			if (isprint(c)) {
			    if (screen[vdisp].xn_flag) {
				screen[vdisp].xn_flag = 0;
				x = 0;
				wmove_xn(y, x);
				wputc(vdisp, '\n');
			    }
			    screen[vdisp].xn_flag = (x == COLS-1);
			    if ((y != LINES-1) || (x != COLS-1))
				waddch(vd, c);
			    if (screen[vdisp].xn_flag)
				wmove(vd, y, x);
			}
			break;
		case '\r':
			wmove_xn(y, 0);
			break;
		case '\n':
			if (y < LINES-1) {
			    wmove_xn(y+1, x);
			    break;
			}
			/*
			 *	This is ugly.  Curses will only scroll curscr
			 *	by emitting a line feed.  Other windows are
			 *	repainted even if they take the whole hardware
			 *	display.  So, we scroll curscr instead of vd.
			 *	Then we must scroll vd to match the state of
			 *	the terminal.  This may confuse curses about
			 *	the location of the hardware cursor, if curses
			 *	decides to use the position from curscr instead
			 *	of the one from vd.  So, force a cursor motion
			 *	to home and back to resynch hardware and
			 *	software.
			*/
			if (vdisp == curvdsp)
				scroll(curscr);
			scroll(vd);
			if (vdisp == curvdsp) {
				wmove(vd, 0, 0);
				wrefresh(vd);
			}
			wmove(vd, y, x);
			break;
		case '\b':
			if (x)
			    wmove_xn(y, x-1);
			break;
		case '\t':
			while (++x < COLS)
			    if (screen[vdisp].tstop[x])
				break;
			if (x >= COLS)
			    x = COLS-1;
			wmove_xn(y, x);
			break;
		case '\07':
			beep();		/* Beep even if not current	*/
			break;
		case '\033':
			screen[vdisp].vstate = V_ESC;
			break;
		}
		break;
	case V_ESC:
		/*
		 *	ESC 'H'			Set tab stop
		 *	ESC 'M'			Scroll up
		 *	ESC '['			See Below
		 *	ESC <other>		Error (ignore)
		*/
		switch (c) {
		    case '[':
			screen[vdisp].vstate = V_ANSII;
			screen[vdisp].cm_col = 0;
			break;
		    case 'H':
			screen[vdisp].vstate = V_NORM;
			screen[vdisp].tstop[x] = 1;
			break;
		    case 'M':
			screen[vdisp].vstate = V_NORM;
			if (y == 0)
				winsertln(vd);
			break;
		    default:
			screen[vdisp].vstate = V_NORM;
		}
		break;
	case V_ANSII:
		/*
		 *	ESC '[' 'A'			{ cursor_up(); }
		 *	ESC '[' 'B'			{ cursor_down(); }
		 *	ESC '[' 'C'			{ cursor_right(); }
		 *	ESC '[' 'D'			{ cursor_left(); }
		 *	ESC '[' number ';' number 'H'	{ move ($1, $2); }
		 *	ESC '[' number 'J'		{ clear_screen($1); }
		 *	ESC '[' number 'H'		{ move($1, 0); }
		 *	ESC '[' 'K'			{ clear_to_eol(); }
		 *	ESC '[' number 'g'		{ clear_tab_stops(); }
		 *	ESC '[' number 'm'		{ set_attributes($1); }
		 *	ESC '[' '?' number 'l'		;
		 *	ESC '[' '?' number 'h'		;
		*/
		screen[vdisp].vstate = V_NORM;
		switch (c) {
		    case '0':
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			screen[vdisp].vstate = V_ANSII;
			screen[vdisp].cm_col = screen[vdisp].cm_col*10 + c-'0';
			break;
		    case 'A':
			if (screen[vdisp].cm_col == 0)
			    screen[vdisp].cm_col = 1;
			y -= screen[vdisp].cm_col;
			if (y < 0)
			    y = 0;
			break;
		    case 'B':
			if (screen[vdisp].cm_col == 0)
			    screen[vdisp].cm_col = 1;
			y += screen[vdisp].cm_col;
			if (y >= LINES)
			    y = LINES-1;
			break;
		    case 'C':
			if (screen[vdisp].cm_col == 0)
			    screen[vdisp].cm_col = 1;
			x += screen[vdisp].cm_col;
			if (x >= COLS)
			    x = COLS-1;
			break;
		    case 'D':
			if (screen[vdisp].cm_col == 0)
			    screen[vdisp].cm_col = 1;
			x -= screen[vdisp].cm_col;
			if (x < 0)
			    x = 0;
			break;
		    case 'J':
			if (screen[vdisp].cm_col)
			    wclear(vd);
			else
			    wclrtobot(vd);
			wmove_xn(y, x);
			break;
		    case 'H':
			if (screen[vdisp].cm_col)
			    screen[vdisp].cm_col--;
			y = screen[vdisp].cm_col;
			x = 0;
			break;
		    case 'K':
			wclrtoeol(vd);
			break;
		    case 'g':
			{   int i;
			    for (i = 0; i < COLS; i++)
				screen[vdisp].tstop[i] = 0;
			}
			break;
		    case 'm':
			if (screen[vdisp].cm_col == 0)
			    wstandend(vd);
			else
			    wstandout(vd);
			break;
		    case ';':
			screen[vdisp].vstate = V_CM;
			screen[vdisp].cm_row = screen[vdisp].cm_col;
			screen[vdisp].cm_col = 0;
			break;
		    case '?':
			screen[vdisp].vstate = V_FLAG;
			break;
		}
		wmove_xn(y, x);
		break;
	case V_CM:
		/*
		 *	ESC '[' number ';' number 'H'	cursor move
		*/
		screen[vdisp].vstate = V_NORM;
		switch (c) {
		    case '0':
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			screen[vdisp].vstate = V_CM;
			screen[vdisp].cm_col = screen[vdisp].cm_col*10 + c-'0';
			break;
		    case 'H':
			if (screen[vdisp].cm_row)
			    screen[vdisp].cm_row--;
			if (screen[vdisp].cm_col)
			    screen[vdisp].cm_col--;
			wmove_xn(screen[vdisp].cm_row, screen[vdisp].cm_col);
			break;
		}
		break;
	case V_FLAG:
		/*
		 *	ESC '[' '?' number 'l'		no-op
		 *	ESC '[' '?' number 'h'		no-op
		*/
		screen[vdisp].vstate = V_NORM;
		switch (c) {
		    case '0':
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			screen[vdisp].vstate = V_FLAG;
			break;
		    case 'l':
		    case 'h':
			break;
		}
	}
	wrefresh(screen[curvdsp].display);
}

vdisp_wrapup()
{	int i;

	if (trace) {
		for (i = 0; i < MAXVDISP; i++)
			fclose(screen[i].trace_fd);
	}
}
