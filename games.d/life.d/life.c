/*----------------------------------------------------------------------*/
/*                                                                      */
/* The game of Life.                                                    */
/*                                                                      */
/*      cc life.c -o life -lcurses -ltermlib                            */
/*                                                                      */
/*                          Miles Murdocca                              */
/*			    4G-538					*/
/*			    AT&T Bell Laboratories			*/
/*			    Crawfords Corner Rd				*/
/*			    Holmdel, NJ, 07733				*/
/*			    (201) 949-2504				*/
/*			    ...{ihnp4}!vax135!miles			*/
/*                          January, 1984                               */
/*                                                                      */
/*----------------------------------------------------------------------*/
#include <stdio.h>
#include <curses.h>
#include <signal.h>

#define XMAX 88         /* Max expected number of columns */
#define YMAX 72         /* Max expected number of row */
#define SET   1
#define RESET 0
unsigned char universe[XMAX][YMAX];     /* Max expected size of screen */
int singlestep; /* Boolean - if TRUE, single step (wait for char to continue) */

SIG_T
die(dummy) {
    char str[80];
    int fp1, i, j;

    signal(SIGINT, SIG_IGN);            /* Ignore rubouts */
    mvcur(0, COLS-1, LINES-1, 0);       /* Go to bottom of screen */
    endwin();                           /* Set terminal to initial state */
    printf("Save game? ");
    fflush(stdout);
    scanf("%s", str);
    if (str[0] == 'y' || str[0] == 'Y') {
	printf("File name: ");
	fflush(stdout);
	scanf("%s", str);
	if ((fp1 = creat(str, 0644)) < 0) {
	    printf("Cannot open %s.  You get one more chance.\n",str);
	    printf("File name: ");
	    scanf("%s", str);
	    if ((fp1 = creat(str, 0)) < 0) {
		printf("Cannot open %s.  Tough luck.\n",str);
		exit(0);
		}
	    }
    for (j = 1; j < LINES - 1; j++)
	for (i = 1; i < COLS - 1; i++) {
	    move(j, i);
	    if (inch() == 'X') universe[i][j] = SET;
	    }
	for (i = 0; i < COLS; i++)
	    write(fp1, universe[i], LINES);
	close(fp1);
	}
    exit(0);
    }

main(argc, argv)
int argc;
char *argv[];
{
    int i, j;

    initscr();          /* Initialize screen package */
    if (evalargs(argc, argv) == 0)    /* Evaluate arguments */
	for (i = 0; i < XMAX; i++)
	    for (j = 0; j < YMAX; j++) universe[i][j] = RESET;

    signal(SIGINT, die);        /* Set to restore tty stats */
    crmode();           /* set for char-by-char */
    noecho();           /*      input */
    nonl();             /* For optimization */

    getstart();         /* Get starting position */
    for (;;) {
	prboard();      /* Print out current board */
	update();       /* Update board position */
	if (singlestep) getchar();
	}
    }

getstart() {
    reg char c;
    reg int  x, y;
    char buf[100];

    prboard();
    move(11, 36);               /* Move to middle */

    refresh();
    c = getch();
    while (c != 'q') {
	switch(c) {
	    case 'h': if (stdscr->_curx == 1) break;
		      move(stdscr->_cury, stdscr->_curx-1); break;
	    case 'j': if (stdscr->_cury >= LINES - 2) break;
		      move(stdscr->_cury+1, stdscr->_curx); break;
	    case 'k': if (stdscr->_cury == 1) break;
		      move(stdscr->_cury-1, stdscr->_curx); break;
	    case 'l': if (stdscr->_curx >= COLS - 2) break;
		      move(stdscr->_cury, stdscr->_curx+1); break;
	    case '?': help(); break;
	    case 'x': if (stdscr->_curx >= COLS - 1) break;
		      addch('X'); break;
	    case ' ': if (stdscr->_curx >= COLS - 1) break;
		      addch(' '); break;
	    default: break;
	    }
	refresh();              /* Print current position */
	c = getch();
	}

    for (y = 1; y < LINES - 1; y++)
	for (x = 1; x < COLS - 1; x++) {
	    move(y, x);
	    if (inch() == 'X') universe[x][y] = SET;
	    }
    }

prboard() {
    int x, y;

    erase();            /* Clear out last position */
    box(stdscr, '|', '_');  /* Box in the screen */
    for (y = 1; y < LINES - 1; y++)
	for (x = 1; x < COLS - 1; x++)
	    if (universe[x][y] == SET) {
		move(y, x);
		addch('X');
		}
    refresh();
    }

evalargs(argc, argv)
int argc;
char *argv[];
{
    int i, j, k, fp;
    unsigned char c;

    singlestep = FALSE;
    if (argc == 1) return(0);
    if (argc > 3) {
	printf("Usage:  clo [filename] [-s]\n");
	exit(0);
	}
    for (k = 1; k < argc; k++) {
	if (argv[k][0] == '-') {
	    switch(argv[k][1]) {
		case 's': singlestep = TRUE; break;
		default: printf("Bad option: %s\n",argv[k]); break;
		}
	    continue;
	    }
	if ((fp = open(argv[1], 0)) < 0) {
	    printf("Cannot open %s\n",argv[1]);
	    exit(0);
	    }
	for (i = 0; i < COLS; i++)
	    read(fp, universe[i], LINES);
	close(fp);
	}
    return(1);
    }

/*-------------------------------------------------------------------*/
/*                                                                   */
/* Rules: if less than 2 neighbors are set, then die.  If exactly    */
/* 2 neighbors set then stay the same.  If 3 neighbors are set       */
/* then birth. If more than 3 neighbors are set then die.            */
/*                                                                   */
/*-------------------------------------------------------------------*/
update()
{
    int i, j, c;
    unsigned char tmpuniverse[XMAX][YMAX];     /* Max expected size of screen */

    for (i = 0; i < COLS; i++)
	for (j = 0; j < LINES; j++) tmpuniverse[i][j] = universe[i][j];
    for (i = 1; i < COLS - 1; i++) {
	for (j = 1; j < LINES - 1; j++) {
	    c = count(i, j);
	    switch(c) {
		case 0: tmpuniverse[i][j] = RESET; break;
		case 1: tmpuniverse[i][j] = RESET; break;
		case 2: break;
		case 3: tmpuniverse[i][j] = SET; break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8: tmpuniverse[i][j] = RESET; break;
		default: printf("Bad count\n"); break;
		}
	    }
	}
    for (i = 0; i < COLS; i++)
	for (j = 0; j < LINES; j++) universe[i][j] = tmpuniverse[i][j];
    }

help()
{
    int x, y, savex, savey;

    savex = stdscr->_curx;
    savey = stdscr->_cury;
    for (y = 1; y < LINES - 1; y++)
	for (x = 1; x < COLS - 1; x++) {
	    move(y, x);
	    if (inch() == 'X') universe[x][y] = SET;
	    }
    erase();
    move(3, 15); printw("The Game of Life");
    move(5, 15); printw("h - left\n");
    move(6, 15); printw("j - down\n");
    move(7, 15); printw("k - up\n");
    move(8, 15); printw("l - right\n");
    move(9, 15); printw("x - set point\n");
    move(10, 15); printw("<space> - reset point\n");
    move(11, 15); printw("q - start\n");
    move(12, 15); printw("[DEL] - exit (with save)\n");
    move(14, 15); printw("Hit any character to continue: ");
    refresh();
    getch();
    prboard();
    stdscr->_curx = savex;
    stdscr->_cury = savey;
    move(stdscr->_cury, stdscr->_curx);
    refresh();
    }

count(x, y)
int x, y;
{
    int c;

    c = 0;
    if (universe[x-1][y-1] == SET) c++;
    if (universe[x][y-1] == SET) c++;
    if (universe[x+1][y-1] == SET) c++;
    if (universe[x-1][y] == SET) c++;
    if (universe[x+1][y] == SET) c++;
    if (universe[x-1][y+1] == SET) c++;
    if (universe[x][y+1] == SET) c++;
    if (universe[x+1][y+1] == SET) c++;
    return(c);
    }
