# include "def.h"

/* Routines using termcap for positioning */

# include <sgtty.h>
struct sgttyb argp;
short ospeed;

extern putchar();

char *BC;     /* Backspace if not ^H */
char *UP;     /* Up one line         */
char *HO;     /* Home                */
char *CD;     /* Clear from cursor   */
char *CM;     /* cursor motions      */
char tcbuf[1024]; /* Buffert for initialising capabilities */
char cp[256];     /* Buffert for storing capabilities      */

/* Initialising the string containing terminal depending functions */

getcap(term)
char *term;
{
	char *pc;
	char *tgetstr();

	gtty(0, &argp);
	ospeed = argp.sg_ospeed;

	switch (tgetent(tcbuf, term)) {
	case -1:
	case 0:
		return -1;
	}
	pc = cp;
	BC = tgetstr("bc", &pc);
	if (BC == 0)
		BC = "\b";
	UP = tgetstr("up", &pc);
	HO = tgetstr("ho", &pc);
	CD = tgetstr("cd", &pc);
	CM = tgetstr("cm", &pc);

	width = tgetnum("co");
	height = tgetnum("li");

	return 0;
}


/* positions on the screen. Origin in the left upper corner. */
pos(x, y) int x, y;{
	if((x > -2) && (x < 39) && (y > -2) && (y < 22)){
		x = 3 + x + x;
		y = 1 + y;
		tputs( tgoto(CM, x, y), 1, putchar);
	}
	else
		printf("Out of board %d %d", x, y);
}

/* Cursor up one line */
up(){
	tputs(UP, 1, putchar);
}

/* Clears screen from cursor */
clear(){
	tputs(CD, 1, putchar);
}

/* Cursor home */
home(){
	tputs(HO, 1, putchar);
}

