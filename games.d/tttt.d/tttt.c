/*
	Eric Lechner		Tic-Tac-Toc-Toe, Version 2
	March 25, 1990		CE 126 Final Project Development

	a 4x4 version of tic-tac-toe with "wraparound edges"

	Originally written Feb. 27, 1990

	revised for curses output: march 6, 1990

	user interface enhanced, help features added: march 22-25, 1990

	Everything should be reasonably self documenting...
*/

#include <stdio.h>
#include <curses.h>
#include <signal.h>

#define EMPTY	0
#define X	1
#define O	2

#define MORE	0
#define TIE	-1		/* for ending when there are no more moves */
#define QUIT	-3

#define UP	1		/* definitions for board scrolling routine */
#define DOWN	2
#define LEFT	3
#define RIGHT	4
#define WIN	100		/* for position rank scoring */

#define ACROSS		1	/* types of wins for win hiliting... */
#define VERTICAL	2
#define DIAG1		3
#define DIAG2		4

#ifndef TRUE
#define TRUE	1		/* cause they aren't included elsewhere */
#define FALSE	0
#endif

#ifdef BSD
# define rand	random
# define srand	srandom
#else
/* Current versions of System V */
# define rand	lrand48		/* comment this out if you only have rand() */
# define srand	srand48		/* comment this out if you only have srand() */
#endif

char board [4][4];		/* this is the playing board */
int pieces;
int randomness;			/* whether the computer uses randomness */

/* this is the help screen part for the command list */
char *commands[] = {	" ---------------------- ",
			"|     Command Help     |",
			" ---------------------- ",
			"| Adding pieces :      |",
			"|  Type the letter and |",
			"|  number of your move |",
			"|----------------------|",
			"| Scroll the Board :   |",
			"|      h : left        |",
			"|      j : down        |",
			"|      k : up          |",
			"|      l : right       |",
			"|----------------------|",
			"|     ^L : redraw      |",
			"|      q : quit        |",
			" ---------------------- ",
			0 };

/* and this is the "objective of the game" help stuff */
char *objective[] = {	" --------------------- ",
			"|      Objective      |",
			" --------------------- ",
			"| Try to connect four |",
			"|  of your pieces in  |",
			"|  a row before your  |",
			"|   opponent does!    |",
			"|---------------------|",
			"|  Wins can be :      |",
			"|        vertical,    |",
			"|        horizontal,  |",
			"|    and diagonal.    |",
			"|---------------------|",
			"| The board edges are |",
			"| `wraparound' edges. |",
			" --------------------- ",
			0 };

void	displayhelp(),		/* display on-screen help */
	getrank(),		/* move ranking */
	hilite_win(),		/* for hiliting wins */
	initboard(),		/* initialize the board */
	print_help(),		/* print the command flag help list */
	printboard(),		/* print the board */
	shift(),		/* board shifter */
	updaterank();		/* update rank status */

int	wins(),			/* see if any wins are still possible */
	check_win(),		/* check for a win */
	human_move(),		/* "our" move */
	computer_move();	/* the computer player's move */

SIG_T	quit();			/* control-c signal exit */

main(argc,argv)
int argc;
char *argv[];
{
	int	done,
		i,
		win,
		players = 1,
		startx = TRUE,
		xmove;

	randomness = TRUE;	/* default player uses some randomness */

	for (i=1;i<argc;i++) {
		if ((*argv[i] == '-') || (*argv[i] == '+')) {
			while (*++argv[i]) {
				switch (*argv[i]) {
					case 'o' :
					case 'O' :
						startx = FALSE;
						break;
					case '2' :
						players = 2;
						break;
					case '0' :
						players = 0;
						break;
					case 'r' :
					case 'R' :
						randomness = FALSE;
						break;
					case 'h' :
					case 'H' :
					case '?' :
						print_help();
						exit(0);
					default	:
						break;
				}
			}
		}
	}

	initscr();
	savetty();
#ifdef CRMODE
	crmode();
#else
	cbreak();
#endif
	noecho();

	signal(SIGINT,quit);
	srand(getpid());

	clear();

StartGame:
	done = FALSE;
	xmove = startx;		/* reset who moves first */
	initboard();		/* reset the board to all empty */

	printboard(1);
	while (!done) {
		if (xmove) {
			if (players > 0) {
				done = human_move(X);
			} else {
				done = computer_move(X);
			}
		} else {
			if (players < 2) {
				done = computer_move(O);
			} else {
				done = human_move(O);
			}
		}
		if ((++pieces == 16) && !done) done = TIE;
		if (!done) done = wins();

		xmove = !xmove;
	}

	move(15,0);
	clrtoeol();

	win = done;			/* save type of win */

DisplayEnd:
	switch (win) {
		case TIE :
			move(17,28);
			printw("Tie game! Nobody can win.\n");
			break;
		case QUIT :
			goto EndGame;
			break;
		default :
			move(17,33);
			printw("Player `%c' Wins!\n",(win == X) ? 'X' : 'O');
	}

	move(19,31);
	printw("Play again (y/n) ? ");
	refresh();

	done = FALSE;
	while (!done) {
		i = getch();
		switch (i) {
			case 'y' :
			case 'Y' :
				move(19,0);
				clrtoeol();
				move(17,0);
				clrtoeol();
				goto StartGame;
				break;
			case 'q' :
			case 'Q' :
			case 'x' :
			case 'X' :
			case 'n' :
			case 'N' :
				done = TRUE;
				break;
			case 0x12 :
			case 0x0c :
				clear();
				printboard(0);
				if ((win == X) || (win == O))
					hilite_win(-1,-1,-1,-1);
				goto DisplayEnd;
		}
	}

EndGame:
	move(19,31);
	printw("Thanks for playing!");
	refresh();
	move(21,0);
	refresh();
	resetty();
	endwin();
}

/*
	initialize the board to all empty
*/
void initboard()
{
	int i,j;

	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			board [i][j] = EMPTY;

	pieces = 0;
}

/*
	print the current board
*/
void printboard(do_refresh)
int do_refresh;
{
	int i;

	move(2,24);
	printw("Tic-Tac-Toc-Toe - by Eric Lechner");

	move(5,30);
	printw("    A   B   C   D \n");
	move(6,30);
	printw("   --- --- --- ---\n");
	for (i=0; i<4; i++) {
		move(7 + (i * 2), 30);
		printw("%d | %c | %c | %c | %c |\n", i,
			(!board[i][0]) ? ' ' : (board[i][0] == X) ? 'X' : 'O',
			(!board[i][1]) ? ' ' : (board[i][1] == X) ? 'X' : 'O',
			(!board[i][2]) ? ' ' : (board[i][2] == X) ? 'X' : 'O',
			(!board[i][3]) ? ' ' : (board[i][3] == X) ? 'X' : 'O');
		move(8 + (i * 2), 30);
		printw("   --- --- --- ---\n");
	}

	if (do_refresh) refresh();
}

/*
	the human player's move
*/
int human_move(type)
int type;
{
	int row, col, tmp, mess = 0;
Move:	row = col = -1;

	move(15,32);
	printw("Player `%c' : ",(type == X) ? 'X' : 'O');
	clrtoeol();
	refresh();
	while ((row == -1) || (col == -1)) {
		tmp = getch();
		switch (tmp) {
			case 'A' :
			case 'a' :
			case 'B' :
			case 'b' :
			case 'C' :
			case 'c' :
			case 'D' :
			case 'd' :
				move(15,45);
				printw("%c ",tmp);
				if ((tmp >= 'a') && (tmp <= 'd'))
					col = tmp - 'a';
				else
					col = tmp - 'A';
				break;
			case '0' :
			case '1' :
			case '2' :
			case '3' :
				move(15,47);
				printw("%c",tmp);
				row = tmp - '0';
				break;
			case 'q' :
			case 'Q' :
				return(QUIT);
				break;
			case 'j' :
			case 'J' :
				shift(DOWN);
				printboard(1);
				goto Move;
			case 'k' :
			case 'K' :
				shift(UP);
				printboard(1);
				goto Move;
			case 'l' :
			case 'L' :
				shift(RIGHT);
				printboard(1);
				goto Move;
			case 'h' :
			case 'H' :
				shift(LEFT);
				printboard(1);
				goto Move;
			case 0x12 :
			case 0x0c :
				clear();
				printboard(0);
				goto Move;
			case '?' :
				displayhelp();
				move(15,45);
				goto Move;
			default:
				move(17,34);
				printw("`?' for help");
				move(15,45);
				mess = 1;
				break;
		}
		refresh();
	}
	if (board[row][col] == EMPTY) board[row][col] = type;
	else {
		move(17,34);
		printw("Illegal move!\n");
		mess = 1;	/* whether an error message has been printed */
		goto Move;
	}
	
	if (mess) {		/* if there was an error message, clear it */
		move(17,1);
		clrtoeol();
	}

	move(7 + (row * 2), 34 + (col * 4));	/* print the move */
	printw("%c",(type == X) ? 'X' : 'O');

	refresh();

	return (check_win (type,row,col));
}

/*
	the computer player
*/
int computer_move(type)
int type;
{
	int bestrow, bestcol, i, j;
	int bestrank, numbest, rank, numrank;
		/* rank == the rank score */
		/* num == the number of times the score happened */

	bestrow = bestcol = bestrank = numbest = 0;

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			getrank(type,i,j,&rank,&numrank);
			if (rank == 0) continue;
			if (rank > bestrank) {
				bestrow = i;
				bestcol = j;
				bestrank = rank;
				numbest = numrank;
			} else if (rank == bestrank) {
				if (numrank > numbest) {
					bestrow = i;
					bestcol = j;
					bestrank = rank;
					numbest = numrank;
				} else if (numrank == numbest) {
					if ((rand() % 2) && randomness) {
						bestrow = i;
						bestcol = j;
						bestrank = rank;
						numbest = numrank;
					}
				}
			}
		}
	}

	board [bestrow][bestcol] = type;

	move(7 + (bestrow * 2), 34 + (bestcol * 4));
	printw("%c",(type == O) ? 'O' : 'X');
	refresh();

	return (check_win (type,bestrow,bestcol));
}

/*
	this ranks a move location in order of "preference".

	the strategy is to not let the opponent get lots in a row.

	ever.

	rows, columns, and diagonals get treated independently, and
	the "best" rank of them all is considered.
*/
void getrank(type,row,col,rank,numrank)
int type, row, col, *rank, *numrank;
{
	int i, j, countx = 0, counto = 0;
	
	*rank = 0;
	*numrank = 0;

	/* if already taken, this isn't a good spot */
	if (board[row][col] != EMPTY) return;

	/* check across */
	countx = counto = 0;
	for (i=0; i<4; i++) {
		if (board[row][i] == X) countx++;
		else if (board[row][i] == O) counto++;
	}

	updaterank(type,countx,counto,rank,numrank);

	/* check vertically */
	countx = counto = 0;
	for (i=0; i<4; i++) {
		if (board[i][col] == X) countx++;
		else if (board[i][col] == O) counto++;
	}

	updaterank(type,countx,counto,rank,numrank);

	/* check \ diagonal */
	countx = counto = 0;
	for (i=0; i<4; i++) {
		if (board[(row + i) % 4][(col + i) % 4] == X) countx++;
		else if (board[(row + i) % 4][(col + i) % 4] == O) counto++;
	}

	updaterank(type,countx,counto,rank,numrank);

	/* check / diagonal */
	countx = counto = 0;
	for (i=0; i<4; i++) {
		if (board[(row + 4 - i) % 4][(col + i) % 4] == X) countx++;
		else if (board[(row + 4 - i) % 4][(col + i) % 4] == O) counto++;
	}

	updaterank(type,countx,counto,rank,numrank);

	/* add one, so that even no blocks still shows as a valid move	*/
	/* and return the rank for this move				*/
	++(*rank);
}

/*
	checks for a win at a specific location, and returns the
	type of win, or 0, if there wasn't a win.
*/
int check_win(type,row,col)
int type, row, col;
{
	int r[4],c[4], i;	/* temp row, col, and counter vars */

	/* check across */
	if ((board [row][0] == type) && (board [row][1] == type) &&
		(board [row][2] == type) && (board [row][3] == type)) {
		hilite_win(row,col,ACROSS,type);
		return (type);
	}

	/* check vertically */
	if ((board [0][col] == type) && (board [1][col] == type) &&
		(board [2][col] == type) && (board [3][col] == type)) {
		hilite_win(row,col,VERTICAL,type);
		return (type);
	}

	/* check \ diagonal */
	for (i=0; i<4; i++) {
		r[i] = (row + i) % 4;
		c[i] = (col + i) % 4;
	}
	if ((board [r[0]][c[0]] == type) && (board [r[1]][c[1]] == type) &&
		(board [r[2]][c[2]] == type) && (board [r[3]][c[3]] == type)) {
		hilite_win(row,col,DIAG1,type);
		return (type);
	}

	/* check / diagonal */
	for (i=0; i<4; i++) {
		r[i] = (row + i) % 4;
		c[i] = (col + (4 - i)) % 4;
	}
	if ((board [r[0]][c[0]] == type) && (board [r[1]][c[1]] == type) &&
		(board [r[2]][c[2]] == type) && (board [r[3]][c[3]] == type)) {
		hilite_win(row,col,DIAG2,type);
		return (type);
	}

	/* if it got here, there aren't any wins at that spot */
	return (0);
}

/*
	this is the board shifter.  it's so you can move the
	board around to look at wraparound edges better...
*/
void shift(type)
int type;
{
	int i, j, tmp;

	for (i=0; i<4; i++) {
		switch (type) {
			case UP:
				tmp = board[0][i];
				for (j=0; j<3; j++)
					board[j][i] = board[j+1][i];
				board[3][i] = tmp;
				break;
			case DOWN:
				tmp = board[3][i];
				for (j=3; j>0; j--)
					board[j][i] = board[j-1][i];
				board[0][i] = tmp;
				break;
			case LEFT:
				tmp = board[i][0];
				for (j=0; j<3; j++)
					board[i][j] = board[i][j+1];
				board[i][3] = tmp;
				break;
			case RIGHT:
				tmp = board[i][3];
				for (j=3; j>0; j--)
					board[i][j] = board[i][j-1];
				board[i][0] = tmp;
				break;
		}
	}
}

/*
	the nice control-C handler
*/
/* ARGSUSED */
SIG_T quit(dummy)
int dummy;
{
	move(21,0);
	refresh();
	resetty();
	endwin();

	exit(0);
}

/*
	this checks the board to see if any wins are still possible
	and returns TIE if no more wins, or MORE if there are.
*/
int wins()
{
	int i, j, rank = 0, countx = 0, counto = 0;

	/* check across */
	for (j=0; j<4; j++) {
		countx = counto = 0;
		for (i=0; i<4; i++) {
			if (board[j][i] == X) countx++;
			else if (board[j][i] == O) counto++;
		}
		if ((countx == 0) || (counto == 0)) return (MORE);
	}

	/* check vertically */
	for (j=0; j<4; j++) {
		countx = counto = 0;
		for (i=0; i<4; i++) {
			if (board[i][j] == X) countx++;
			else if (board[i][j] == O) counto++;
		}
		if ((countx == 0) || (counto == 0)) return (MORE);
	}

	/* check \ diagonal */
	for (j=0; j<4; j++) {
		countx = counto = 0;
		for (i=0; i<4; i++) {
			if (board[(j + i) % 4][i] == X) countx++;
			else if (board[(j + i) % 4][i] == O) counto++;
		}
		if ((countx == 0) || (counto == 0)) return (MORE);
	}

	/* check / diagonal */
	for (j=0; j<4; j++) {
		countx = counto = 0;
		for (i=0; i<4; i++) {
			if (board[(j + 4 - i) % 4][i] == X) countx++;
			else if (board[(j + 4 - i) % 4][i] == O) counto++;
		}
		if ((countx == 0) || (counto == 0)) return (MORE);
	}

	/* if there are more moves, then we've already returned */
	return (TIE);
}

/*
	print the command line help stuff
*/
void print_help()
{
	fprintf(stderr,"Tic Tac Toc Toe - By Eric Lechner\n\n");
	fprintf(stderr,"Command line options:\n");
	fprintf(stderr,"\t-h - print this help screen\n");
	fprintf(stderr,"\t-o - Have player `O' move first\n");
	fprintf(stderr,"\t-r - disallow randomness for the computer player\n");
	fprintf(stderr,"\t-0 - Have the computer play against itself\n");
	fprintf(stderr,"\t-2 - Play with two human players\n");
	fflush(stderr);
}

void hilite_win(row,col,wintype,playertype)
int row, col, wintype, playertype;
{
	int i, row_step, col_step, tmprow, tmpcol;
	static int oldrow, oldcol, oldwintype, oldplayertype;

	/* in order to redraw the hilited win, we need to save the values */
	/* to redisplay the hilite, call hilite_win with row == -1 */

	if (row != -1) {
		oldrow = row;
		oldcol = col;
		oldwintype = wintype;
		oldplayertype = playertype;
	} else {
		row =oldrow;
		col = oldcol;
		wintype = oldwintype;
		playertype = oldplayertype;
	}

	switch (wintype) {
		case ACROSS :
			row_step = 0;
			col_step = 1;
			break;
		case VERTICAL :
			row_step = 1;
			col_step = 0;
			break;
		case DIAG1 :
			row_step = 1;
			col_step = 1;
			break;
		case DIAG2 :
			row_step = 3;
			col_step = 1;
			break;
	}

	for (i=0; i<4; i++) {
		tmprow = (row + (row_step * i)) % 4;
		tmpcol = (col + (col_step * i)) % 4;
		move(7 + (tmprow * 2), 33 + (tmpcol * 4));

		printw("*%c*", (board[tmprow][tmpcol] == X) ? 'X' : 'O');
	}
}

/*
	this is the on-screen help stuff
	(^L will redraw the screen)
*/
void displayhelp()
{
	int i = 0, j =0, tmp;

DisplayHelp:
	i = j = 0;

	move(15,32);
	printw("                ");

	while (commands[i]) {
		move(4 + i,3);
		printw("%s",commands[i++]);
	}

	while (objective[j]) {
		move(4 + j,52);
		printw("%s",objective[j++]);
	}

	move(17,33);
	printw("Hit any hey to");
	move(18,33);
	printw("  continue. ");
	refresh();

	
	tmp = getch();
	if ((tmp == 0x12) || (tmp == 0x0c)) {
		clear();
		printboard(0);
		goto DisplayHelp;
	}

	move(17,33);
	printw("              ");
	move(18,33);
	printw("            ");

	while (j--) {
		move(4 + j,52);
		printw("                        ");
	}

	while (i--) {
		move(4 + i,3);
		printw("                        ");
	}
}

void updaterank(type,countx,counto,rank,numrank)
int type, countx, counto, *rank, *numrank;
{
	if (type == X) {
		if (countx >= 3) {
			if (*rank < countx) {
				*rank = WIN;
				*numrank = 1;
			} else if (*rank == countx) *numrank++;
		} else if (!countx && (counto > *rank)) {
			if (*rank < counto) {
				*rank = counto;
				*numrank = 1;
			} else if (*rank == counto) *numrank++;
		}
	} else {
		if (counto >= 3) {
			if (*rank < counto) {
				*rank = WIN;
				*numrank = 1;
			} else if (*rank == counto) *numrank++;
		} else if (!counto && (countx > *rank)) {
			if (*rank < countx) {
				*rank = countx;
				*numrank = 1;
			} else if (*rank == countx) *numrank++;
		}
	}
}
