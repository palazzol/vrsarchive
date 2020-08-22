/*
 *	Operators on boards and pieces
*/
#include <assert.h>
#include <curses.h>
#include "board.h"

#define ROWS	24
#define COLS	80
extern char screen[ROWS][COLS];
extern char *malloc();
extern char *realloc();
extern void free();
extern int debug;

#define max(x, y)	(x>y? x : y)
#define min(x, y)	(x<y? x : y)
#define sgn(x)	(x>0? 1 : x<0? -1 : 0)
#define closer(oldpos, mypos)	(oldpos + sgn(mypos-oldpos))

/*
 *	Create a new board big enough for <count> pieces, containing just the
 *	player at the specified co-ordinates.
*/
board *
cleanboard(row, col, count)
int row, col, count;
{	board *bd;

	bd = (board *)malloc(sizeof(*bd));
	bd->pieces = (piece *)malloc(count*sizeof(piece));
	bd->allocated = count;
	bd->pieces[0].row = row;
	bd->pieces[0].col = col;
	bd->pieces[0].obj = PLAYER;
	bd->count = 1;
	bd->ranking = 0;
	return(bd);
}

/*
 *	Forget a board whose usefullness is at an end
*/
void
forget(bd)
board *bd;
{
	free(bd->pieces);
	free(bd);
}

/*
 *	Convert the screen array into a board
*/
board *
parse()
{	int row, col;
	board *nboard;
	piece p;

	nboard = cleanboard(0, 0, 10);
	for (row = 0; row < ROWS; row++) {
		for (col = 0; col < COLS; col++) {
			switch (screen[row][col]) {
			case '=':
				p.obj = ROBOT;
				goto insert;
			case '@':
				p.obj = HEAP;
			insert:
				p.row = row;
				p.col = col;
				if (nboard->count == nboard->allocated) {
					nboard->allocated += 10;
					nboard->pieces = (piece *)realloc(nboard->pieces,
											nboard->allocated*sizeof(piece));
				}
				nboard->pieces[nboard->count] = p;
				nboard->count++;
				break;
			case 'I':
				nboard->pieces->row = row;
				nboard->pieces->col = col;
			}
		}
	}
	return(nboard);
}

/*
 *	Compute his new position, given a pointer to the new board.
 *	Insert his new position in the new board.
 *	N.B.  Must have player in first cell of each board!
*/
piece
newpos(him, bd)
piece him;
board *bd;
{	int my_x, my_y, his_x, his_y;
	register int i;
	register piece *p;

	p = bd->pieces;
	my_x = p->col;
	my_y = p->row;
	his_x = him.col;
	his_y = him.row;
	his_x = closer(his_x, my_x);
	his_y = closer(his_y, my_y);
	him.col = his_x;
	him.row = his_y;
	for (i = 0; i < bd->count; i++, p++) {
		if ((his_x == p->col) && (his_y == p->row)) {
			him.obj = HEAP;
			if (p->obj != HEAP) {
				p->obj = HEAP;
				bd->ranking--;
			}
			return(him);
		}
	}
	bd->count++;
	assert(bd->allocated >= bd->count);
	*p = him;
	bd->ranking++;
	return(him);
}

/*
 *	Given a board and a proposed new position for the player, compute the 
 *	result.
*/
board *
newboard(row, col, oboard)
int row, col;
board *oboard;
{	board *nboard;
	/*register*/ piece *p;
	/*register*/ int i;

	nboard = cleanboard(row, col, oboard->count);
	p = oboard->pieces+1;
	for (i = 1; i < oboard->count; i++, p++) {
		newpos(*p, nboard);
	}
	nboard->pieces = (piece *)realloc(nboard->pieces,
										nboard->count*sizeof(piece));
	nboard->allocated = nboard->count;
	return(nboard);
}

/*
 *	Given a board, compute the best successor (if any)
*/
board *
lookahead(oboard)
board *oboard;
{	int row, lrow, hrow;
	int col, lcol, hcol;
	board *nboard, *tboard;
	board *bboard = 0;
	int best, savechar;

	if (oboard->pieces[0].obj != PLAYER)
		return(0);	/* No player after previous move */ 
	lrow = max(1, oboard->pieces->row-1);
	hrow = min(ROWS-2, oboard->pieces->row+1);
	lcol = max(1, oboard->pieces->col-1);
	hcol = min(COLS-2, oboard->pieces->col+1);
	best = oboard->ranking;
	for (row = lrow; row <= hrow; row++) {
		for (col = lcol; col <= hcol; col++) {
			if (debug) {
				savechar = screen[row][col];
				mvaddch(row, col, '.');
				screen[row][col] = '.';
			}
			nboard = newboard(row, col, oboard);
			tboard = lookahead(nboard);
			if (!tboard)
				tboard = nboard;
			if (!bboard || (best > tboard->ranking)) {
				/* New one is best, forget old one (if any) */
				if (bboard)
					forget(bboard);
				bboard = nboard;
				best = tboard->ranking;
			} else
				forget(nboard);
			if (tboard != nboard)
				forget(tboard);
			if (debug) {
				mvaddch(row, col, savechar);
				screen[row][col] = savechar;
			}
		}
	}
	/* Getting here with bboard == 0 implies there are no legal moves */
	if (!bboard)
		bboard = oboard;	/* Current state is as good as it gets */
	if (debug)
		refresh();
	return(bboard);			/* Return best successor */
}

/*
 *	Make the best move
*/
char
strategy()
{	static board *oboard = 0;
	board *nboard;
	char cmd = 0;

	if (!oboard)
		oboard = parse();
	nboard = lookahead(oboard);
	if (nboard != oboard) {
		cmd = makemove(nboard->pieces[0].row, nboard->pieces[0].col);
		forget(oboard);
		oboard = nboard;
	}
	if (cmd == 0)
		cmd = 't';	/* Can't get better */
	forget(oboard);
	oboard = 0;
	return(cmd);
}
