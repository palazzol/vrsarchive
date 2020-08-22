/*
 * screen_control.c
 *
 * I/O functions.  This needs to be linked with termcap library (termlib on
 * some systems).  The only parameter needed is for cursor motion.  ("cm")
 */

#include <stdio.h>
#include "wator.h"

extern char *getenv();
extern char *tgetstr();
extern char *tgoto();
extern int tputs();
int    putch();
static char dspbuf[100];	/* buffer for movement string */

/*
 * screen_control()
 *
 * Find out what type of terminal, and read attribute for cursor motion.
 * Check lines and colum if possible.
 */

screen_control()
{
	register int i;
	int t_line, t_col;
	char term[80];
	char trmbuf[1024];
	char *p;

	term[0] = 0;
	strcpy(term, getenv("TERM"));
	if (!term[0]) {
		printf("Unable to read terminal type from 'getenv' call.\n");
		printf("Check setting of environment variable TERM.\n");
		exit(1);
	}
	switch(i = tgetent(trmbuf, term)) {
	case -1:
		printf("'tgetent' unable to open termcap file.\n");
		exit(1);
	case 0:
		printf("Cannot find terminal type '%s' in termcap file.\n",
		  term);
		exit(1);
	case 1:
		break;	/* ok */
	default:
		printf("Unexpected value of %d returned from 'tgetent'.\n", i);
		exit(1);
	}
	p = &dspbuf[0];
	if (!(C_move = tgetstr("cm", &p))) {
		printf("Cannot find cursor movement attribute 'cm'.\n");
		exit(1);
	}
	if ((t_line = tgetnum("li")) == -1) {
		printf("No terminal entry for number of lines.\n");
		printf("Using value of %d\n", lines);
	} else {
		if (lines > t_line) {
			printf("Terminal entry for lines is %d\n", t_line);
			printf("Altering specified line count.\n");
			lines = t_line;
		}
	}
	if ((t_col = tgetnum("co")) == -1) {
		printf("No terminal entry for columns.\n");
		printf("Using value of %d\n", wide);
	} else {
		if (wide > t_col) {
			printf("Terminal entry for columns is %d\n", t_col);
			printf("Altering specified column count.\n");
			wide = t_col;
		}
	}
}

/*
 * movexy(c, l)
 *    int c, l;
 *
 * This outputs the string for cursor motion.
 */

movexy(column, line)
	int column, line;
{
	if (column < 0 || column > wide-1
	    || line < 0 || line > lines-1) {
		printf("\nMove call botched, x = %d, y = %d\n", column, line);
		exit(1);
	}
	tputs(tgoto(C_move, column, line), 1, putch);
}

/*
 * putch(ch)
 *   char ch;
 *
 * This gets characters out on the terminal.  Required by termcap routines.
 */

putch(ch)
	char ch;
{
	putchar(ch);
}

/*
 * draw_screen()
 *
 * First draw of a screen.  Gets initial information out, not used
 * after that.
 */

draw_screen()
{
	register int i, j;

	/*
	 * First loop erases screen.  No need to rely on home and
	 * erase for a one time use.  Note that after a stopped job
	 * is started we might have the cursor near the top of the
	 * screen -- so do two screen-fulls of new lines.
	 */

	for (i=0; i < L_MAX*2; i++)
		putchar('\n');

	movexy(0, 0);			/* Home cursor */

	for (j=0; j < lines-1; j++) {
		for (i=0; i < wide; i++)
			switch (screen[i][j].item_type) {
			case EMPTY:
				putempty();
				break;
			case SHARK:
				putshark();
				break;
			case FISH:
				putfish();
				break;
			}
		putchar('\n');
	}
}

/*
 * Only symbol definition of symbols is here.  Change next four routines
 * to display something different.
 */

putempty()
{
	putchar(' ');
}

putshark()
{
	putchar('O');
}

putfish()
{
	putchar('.');
}

put_stat()
{
	movexy(0, lines-1);
	printf("gen %4d; sharks(O): %4d; fish(.) %4d; breed %d,%d, starve %d  ",
	  generation, num_shark, num_fish, s_breed, f_breed, starve);
	fflush(stdout);
}
