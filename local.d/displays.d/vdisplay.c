/*
 *	These are routines to manage the virtual displays.
*/
#include <stdio.h>
#include <ctype.h>
#include <curses.h>
#include "globals.h"

extern char *malloc();

struct screen {
	short vstate;			/* State of the virtual display	*/
	short cm_row;			/* Row for incomplete cursor mv	*/
	WINDOW *display;		/* Current screen contents	*/
} screen[MAXVDISP];

#define V_NORM		0		/* Normal state			*/
#define V_ESC		1		/* Just got an escape		*/
#define V_CM1		2		/* Expecting column		*/
#define V_CM2		3		/* Expecting row		*/

/*
 *	Loop over all virtual screens, allocating and initializing
 *	resources.  Initialize the physical display.
*/
vdisp_init()
{
	pdisp_init();
	for (curvdsp = 0; curvdsp < MAXVDISP; curvdsp++) {
		screen[curvdsp].vstate = V_NORM;
		screen[curvdsp].display = newwin(LINES, COLS, 0, 0);
		if (screen[curvdsp].display == NULL) {
			perror("newwin");
			done(1);
		}
		scrollok(screen[curvdsp].display, TRUE);
		werase(screen[curvdsp].display);
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

	c &= 0177;
	vd = screen[vdisp].display;
	getyx(vd, y, x);
	switch (screen[vdisp].vstate) {
	case V_NORM:
		switch(c) {
		default:
			if (isprint(c))
				waddch(vd, c);
			break;
		case '\n':
			waddch(vd, c);
			break;
		case '\b':
			if (x)
				wmove(vd, y, x-1);
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
		screen[vdisp].vstate = V_NORM;
		switch(c) {
		case 'A': if (y) wmove(vd, y-1, x); break;
		case 'B': if (y+1 < LINES) wmove(vd, y+1, x); break;
		case 'C': if (x+1 < COLS)  wmove(vd, y, x+1); break;
		case 'D': if (y) wmove(vd, y, x-1); break;
		case 'Y': screen[vdisp].vstate = V_CM1; break;
		case 'J': werase(vd); break;
		case 'K': wclrtoeol(vd); break;
		case 'L': wclrtobot(vd); break;
		case 'N': wdeleteln(vd); break;
		case 'O': winsch(vd, ' '); break;
		case 'P': winsertln(vd); break;
		case 'a': wstandout(vd); break;
		case 'b': wstandend(vd); break;
		}
		break;
	case V_CM1:
		screen[vdisp].vstate = V_CM2;
		screen[vdisp].cm_row = c-' ';
		break;
	case V_CM2:
		screen[vdisp].vstate = V_NORM;
		wmove(vd, screen[vdisp].cm_row, c-' ');
		break;
	}
	wrefresh(screen[curvdsp].display);
}
