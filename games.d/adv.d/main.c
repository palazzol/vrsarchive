/*
 * Adventure Driver Program
 *
 * Version 1.0, 29 December 1981
 * Chris Torek
 * advent [-name] [adventure file]
 *
 * Lots of hacks sometime in '85
 *
 * Amendments to use "curses" library (#ifdef CURSED) by Chris Miller,
 *  August '85.
 */

#include "adv.h"
#ifndef	CURSED
#undef TRUE
#undef FALSE
#endif
#undef LL
#include <pwd.h>
#include <ctype.h>
#include <signal.h>

int IntCatch();
char *getlogin();
struct passwd *getpwuid();

/*
 * Start up.  Handle the single option ("-username"), get the
 * name of the adventure file, and run it.
 */
main(argc, argv)
	register argc;
	register char **argv;
{
	register int c;

	++argv;
	if (argc > 1 && **argv == '-') {
		--argc;
		name = (*argv++) + 1;
	}
	else {
		name = getlogin();
		if (name == 0) {
			struct passwd *p = getpwuid(getuid());

			if (p)
				name = p->pw_name;
			else {
				fprintf(stderr, "Who are you?\n");
				exit(1);
			}
		}
	}
	if (--argc)
		strcpy(fname, *argv);
	else {
		printf("Enter the adventure filename: ");
		fgets(fname, sizeof fname, stdin);
		fname[strlen(fname) - 1] = 0;
	}
#ifdef DEBUG
	printf ("Starting adventure %s for %s\n", fname, name);
#endif
	srand(getpid() + time((long *)0));
	file = fopen(fname, "r");
	if (file == NULL) {
		printf("There's no such adventure!\n");
		exit(1);
	}

	/*
	 * Fire up windows.
	 */
#ifndef CURSED
	if (Winit(0, 0)) {
		printf ("Sorry, this program won't run on your terminal.\n");
		exit(1);
	}
#endif
	printf("Hi %s, just a second while I create the world...\r\n", name);
	fflush(stdout);

#ifndef CURSED
	/*
	 * Get some windows and set them up our way (no cursor, nice
	 * newline mode); we'll use the real screen cursor for input.
	 * N.B.: it is important that TopWin is initially as large as
	 * the real screen.
	 */
	Wscreensize(&ROWS, &COLS);
	BaseWin = Wopen(1, 0, 0, COLS, ROWS, 0, 0);
	TopWin = Wopen(0, 0, 0, COLS, ROWS, 0, 0);
	Woncursor(BaseWin, 0);
	Woncursor(TopWin, 0);
	Wnewline(BaseWin, 1);
	Wnewline(TopWin, 1);
	WSetRealCursor++;
#else
	initscr();
	crmode();
	noecho();
	{
		int	i;
		int	boundary;

		boundary = ROWS/2;		
		TopWin = newwin(boundary, COLS, 0, 0);
		CurWin = BaseWin = newwin(ROWS-boundary-1, COLS,
				boundary+1, 0);
		if (TopWin == NULL || BaseWin == NULL) {
		    Wcleanup();
		    fprintf(stderr, "adv: cannot set up windows\n");
		    exit(100);
		}
		move(boundary, 0);
		for (i = 0; i < COLS; i++) {
		    addch('-');
		}
		wrefresh(stdscr);
	}
	scrollok(BaseWin, TRUE);
#endif

	/*
	 * Read in the adventure file.
	 */
	readin();

	/*
	 * All set.
	 */
#ifdef BSD41				/* 4.1 */
	sigset(SIGINT, IntCatch);
	sigset(SIGQUIT, IntCatch);
#else
	signal(SIGINT, IntCatch);
	signal(SIGQUIT, IntCatch);
#endif
	do {
		init();
		play();
		prt("Do you wish to play again (Y/N)? ");
		prt(0);
		refresh();
		do {
			ReadingTerminal++;
			c = getchar();
			ReadingTerminal = 0;
			if (isupper(c))
				c = tolower(c);
		} while (c != 'y' && c != 'n');
		prt(c == 'n' ? "No\n" : "Yes\n");
		Wrefresh(0);
	} while (c != 'n');
	prt("Goodbye!\n");
	refresh();
	Wexit(0);
}

/*
 * Handle interrupts - just print a message about how to quit.
 */
IntCatch()
{
	static char msg[] = " *** Use the QUIT command to quit.";

	if (ReadingTerminal == 2) {	/* print our msg in TopWin */
		WAcursor(TopWin, 1, 0);
		Wputs(msg, TopWin);
	}
	else {
#ifndef CURSED
		if (BaseWin->w_cursor.col)
#endif
			Wputc('\n', BaseWin);
		Wputs(msg, BaseWin);
		Wputc('\n', BaseWin);
	}
	if (ReadingTerminal)
		refresh();
}
