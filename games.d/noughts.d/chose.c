# include "def.h"

/* Chose what move shall be done depending on threats and points */
chose(who, dox, doy) char who; int *dox, *doy;{
	short int maxpts, movbuf[2][BZIZE], movp, i, good, whomax;
	register int x, y, n;

	if(who == 'x')
		who = 1;
	else
		who = 0;

	for(x = 0; x < 2; x++){
		thtmax[x] = 0;
		for(y = 4; y > 0; y--){
			if(thtcnt[x][y]){
				thtmax[x] = y;
				break;
			}
		}
	}

	good = movp = 0;
	maxpts = -1;
	if(thtmax[who] >= thtmax[!who])
		whomax = who;
	else{
		whomax = !who;
		if(thtmax[!who] == 1)
			whomax = who;
	}
	for(x = 0; x < XZIZE; x++)
		for(y = 0; y < YZIZE; y++)
			if((board[x][y] == ' ') &&
			   (threat[whomax][x][y] == thtmax[whomax])){
				good++;
				if((threat[whomax][x][y]) || (point[x][y] != 0)){
					movbuf[0][movp] = x;
					movbuf[1][movp++] = y;
					if(point[x][y] > maxpts)
						maxpts = point[x][y];
				}
			}
	n = 1;
	for(i = 0; i < movp; i++){
		if(!maxpts || (point[movbuf[0][i]][movbuf[1][i]] * 11) / (10 * maxpts)){
			if(!((rand()>>4) % n++)){
				*dox = movbuf[0][i];
				*doy = movbuf[1][i];
			}
		}
	}
	if(!movp && good)
		for(x = 0; x < XZIZE; x++)
			for(y = 0; y < YZIZE; y++)
				if( (board[x][y] == ' ') &&
				   !((rand()>>4) % n++)){
					*dox = x;
					*doy = y;
				}
	return(good);
}
