# include "def.h"

/* Fetches patterns in the board and puts them in temporary strings */
catch(x, y) int x, y;{
	char *h, *v, *l, *r;
	register int i;

	h = hvlr[0];
	v = hvlr[1];
	l = hvlr[2];
	r = hvlr[3];

	for(i = -8; i <= 8; i++){
		if((x + i >= 0) && (x + i < XZIZE))
			*h++ = board[x + i][y];
		else
			*h++ = 'z';
		if((y + i >= 0) && (y + i < YZIZE))
			*v++ = board[x][y + i];
		else
			*v++ = 'z';
		if((x + i >= 0) && (x + i < XZIZE) && (y + i >= 0) && (y + i < YZIZE))
			*l++ = board[x + i][y + i];
		else
			*l++ = 'z';
		if((x + i >= 0) && (x + i < XZIZE) && (y - i >= 0) && (y - i < YZIZE))
			*r++ = board[x + i][y - i];
		else
			*r++ = 'z';
	}
}
