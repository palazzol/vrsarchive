/*
	Eric Lechner				Feb. 27, 1990

	revised for curses output: march 6, 1990

		CE 126 Final Project Development

	a high level simulation of tic-tac-toc-toe, a 4x4
	version of tic-tac-toe with "wraparound edges"

	this takes user input for one player, and has a
	computer player for the other.
*/

#include <stdio.h>
#include <curses.h>

#define EMPTY	0
#define X	1
#define O	2

#define TIE	-1		/* for ending when the board is full */
#define QUIT	-2

#define WIN	100		/* for position rank scoring */

#ifndef TRUE
#define TRUE	1		/* cause they aren't included elsewhere */
#define FALSE	0
#endif

char board [4][4];		/* this is the playing board */
int pieces;

void	initboard(),
	printboard();

int	check_win(),
	getrank(),
	x_move(),		/* "our" move */
	o_move();		/* the computer player's move */

main()
{
	int done = FALSE;
	int xmove = TRUE;

	pieces = 0;

	initscr();
	savetty();
	cbreak();
	noecho();

	initboard();

	clear();
	printboard();
	while (!done) {
		if (xmove)
			done = x_move();
		else {
			done = o_move();
		}
		if ((++pieces == 16) && !done) done = TIE;

		xmove = !xmove;
	}

	switch (done) {
		case TIE :
			move(17,23);
			printw("The board is full, and nobody won.\n");
			break;
		case QUIT :
			move(17,23);
			printw("Hope to beat you again someday!\n");
			break;
		default :
			move(17,33);
			printw("Player `%c' Wins!\n",(done == X) ? 'X' : 'O');
	}
	move(21,0);
	refresh();
	sleep(2);
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
}

/*
	print the current board
*/
void printboard()
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
	refresh();
}

/*
	player "x", the human player
*/
int x_move()
{
	int row, col, tmp, mess = 0;
GetX:	row = col = -1;

	move(15,33);
	printw("Your move : ");
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
			case 0x12 :
			case 0x0c :
				clear();
				printboard();
				goto GetX;
			default:
				break;
		}
		refresh();
	}
	if (board[row][col] == EMPTY) board[row][col] = X;
	else {
		move(17,34);
		printw("Illegal move!\n");
		mess = 1;	/* whether an error message has been printed */
		refresh();
		goto GetX;
	}
	
	if (mess) {		/* if there was an error message, clear it */
		move(17,1);
		clrtoeol();
	}

	move(7 + (row * 2), 34 + (col * 4));	/* print the move */
	printw("X");

	refresh();

	return (check_win (X,row,col));
}

/*
	player "o", the computer player
*/
int o_move()
{
	int bestrank = 0, bestrow = 0, bestcol = 0, i, j, rank;

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			rank = getrank(O,i,j);
			if (rank > bestrank) {
				bestrow = i;
				bestcol = j;
				bestrank = rank;
			}
		}
	}

	board [bestrow][bestcol] = O;

	move(7 + (bestrow * 2), 34 + (bestcol * 4));
	printw("O");
	refresh();

	return (check_win (O,bestrow,bestcol));
}

/*
	this ranks a move location in order of "preference".

	the strategy is to not let the opponent get lots in a row.
	ever.


	rows, columns, and diagonals get treated independently, and
	the "best" rank of them all is considered.
*/
int getrank(type,row,col)
int type,row,col;
{
	int i, j, rank = 0, countx = 0, counto = 0;

	/* if already taken, this isn't a good spot */
	if (board[row][col]) return (rank);

	/* check across */
	countx = counto = 0;
	for (i=0; i<4; i++) {
		if (board[row][i] == X) countx++;
		else if (board[row][i] == O) counto++;
	}
	if (type == X) {
		if (countx >= 3) return (WIN);
		if (!countx && (counto > rank)) rank = counto;
	} else {
		if (counto >= 3) return (WIN);
		if (!counto && (countx > rank)) rank = countx;
	}

	/* check vertically */
	countx = counto = 0;
	for (i=0; i<4; i++) {
		if (board[i][col] == X) countx++;
		else if (board[i][col] == O) counto++;
	}
	if (type == X) {
		if (countx >= 3) return (WIN);
		if (!countx && (counto > rank)) rank = counto;
	} else {
		if (counto >= 3) return (WIN);
		if (!counto && (countx > rank)) rank = countx;
	}

	/* check \ diagonal */
	countx = counto = 0;
	for (i=0; i<4; i++) {
		if (board[(row + i) % 4][(col + i) % 4] == X) countx++;
		else if (board[(row + i) % 4][(col + i) % 4] == O) counto++;
	}
	if (type == X) {
		if (countx >= 3) return (WIN);
		if (!countx && (counto > rank)) rank = counto;
	} else {
		if (counto >= 3) return (WIN);
		if (!counto && (countx > rank)) rank = countx;
	}

	/* check / diagonal */
	countx = counto = 0;
	for (i=0; i<4; i++) {
		if (board[(row + 4 - i) % 4][(col + i) % 4] == X) countx++;
		else if (board[(row + 4 - i) % 4][(col + i) % 4] == O) counto++;
	}
	if (type == X) {
		if (countx >= 3) return (WIN);
		if (!countx && (counto > rank)) rank = counto;
	} else {
		if (counto >= 3) return (WIN);
		if (!counto && (countx > rank)) rank = countx;
	}

	/* add one, so that even no blocks still shows as a valid move	*/
	/* and return the rank for this move				*/
	return (++rank);
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
		(board [row][2] == type) && (board [row][3] == type))
		return (type);

	/* check vertically */
	if ((board [0][col] == type) && (board [1][col] == type) &&
		(board [2][col] == type) && (board [3][col] == type))
		return (type);

	/* check \ diagonal */
	for (i=0; i<4; i++) {
		r[i] = (row + i) % 4;
		c[i] = (col + i) % 4;
	}
	if ((board [r[0]][c[0]] == type) && (board [r[1]][c[1]] == type) &&
		(board [r[2]][c[2]] == type) && (board [r[3]][c[3]] == type))
		return (type);

	/* check / diagonal */
	for (i=0; i<4; i++) {
		r[i] = (row + i) % 4;
		c[i] = (col + (4 - i)) % 4;
	}
	if ((board [r[0]][c[0]] == type) && (board [r[1]][c[1]] == type) &&
		(board [r[2]][c[2]] == type) && (board [r[3]][c[3]] == type))
		return (type);

	/* if it got here, there aren't any wins at that spot */
	return (0);
}

