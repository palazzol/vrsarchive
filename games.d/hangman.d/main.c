# include	"hangman.h"

/*
 * die:
 *	Die properly.
 */
SIG_T
die(dummy)
{
	mvcur(0, COLS - 1, LINES - 1, 0);
	endwin();
	putchar('\n');
	exit(0);
}

/*
 * This game written by Ken Arnold.
 */
main()
{
	initscr();
	signal(SIGINT, die);
	setup();
	for (;;) {
		Wordnum++;
		playgame();
		Average = (Average * (Wordnum - 1) + Errors) / Wordnum;
	}
	/* NOTREACHED */
}
