# include "def.h"

/* Administrate the updating of point and threat boards */
update(x, y) int x, y;{
    register int i, way;
    short int xx, yy, step, j;
    char who;
    LOGIC xflag, oflag, blankflag, stopflag, win;

    win = FALSE;
    who = board[x][y];
    catch(x, y);
    autom();
    if(who != ' '){
	if((who == 'o') && (threat[0][x][y] == 4))
	    win = TRUE;
	if((who == 'x') && (threat[1][x][y] == 4))
	    win = TRUE;
	thtcnt[0][threat[0][x][y]] -= 1;
	threat[0][x][y] = 0;
	thtcnt[1][threat[1][x][y]] -= 1;
	threat[1][x][y] = 0;
    }
    else{
	for(way = 0; way < 4; way++){
	    p_hvlr[way][x][y] = tmppts[way][0][8] + tmppts[way][1][8];
	    t_hvlr[way][0][x][y] = tmptht[way][0][8];
	    t_hvlr[way][1][x][y] = tmptht[way][1][8];
	}
	point[x][y] = p_hvlr[0][x][y] + p_hvlr[1][x][y] +
		      p_hvlr[2][x][y] + p_hvlr[3][x][y];
	if( t_hvlr[0][0][x][y] | t_hvlr[1][0][x][y] |
	    t_hvlr[2][0][x][y] | t_hvlr[3][0][x][y] )
	    updtht('o', x, y);
	else{
	    thtcnt[0][threat[0][x][y]] -= 1;
	    threat[0][x][y] = 0;
	}
	if( t_hvlr[0][1][x][y] | t_hvlr[1][1][x][y] |
	    t_hvlr[2][1][x][y] | t_hvlr[3][1][x][y] )
	    updtht('x', x, y);
	else{
	    thtcnt[1][threat[1][x][y]] -= 1;
	    threat[1][x][y] = 0;
	}
    }
    for(way = 0; way < 4; way++){
	for(step = -1; step <= 1; step += 2){
	    xflag = oflag = blankflag = stopflag = FALSE;
	    i = 8;
	    for(j = 0; j < 4; j++){
		i += step;
		if(blankflag == 2)
		    break;
		switch(hvlr[way][i]){
		    case ' ':
			blankflag++;
			break;
		    case 'o':
			oflag = TRUE;
			blankflag = FALSE;
			break;
		    case 'x':
			xflag = TRUE;
			blankflag = FALSE;
			break;
		    case 'z':
			stopflag = TRUE;
			break;
		}
		if(stopflag)
		    break;
		if(!blankflag)
		    continue;
		switch(way){
		    case 0:
			xx = x + i - 8;
			yy = y;
			break;
		    case 1:
			xx = x;
			yy = y + i - 8;
			break;
		    case 2:
			xx = x + i - 8;
			yy = y + i - 8;
			break;
		    case 3:
			xx = x + i - 8;
			yy = y - i + 8;
			break;
		}
		p_hvlr[way][xx][yy] = tmppts[way][0][i] + tmppts[way][1][i];
		point[xx][yy] = p_hvlr[0][xx][yy] + p_hvlr[1][xx][yy] +
				p_hvlr[2][xx][yy] + p_hvlr[3][xx][yy];
		if(!xflag){
		    t_hvlr[way][0][xx][yy] = tmptht[way][0][i];
		    if( t_hvlr[0][0][xx][yy] | t_hvlr[1][0][xx][yy] |
			t_hvlr[2][0][xx][yy] | t_hvlr[3][0][xx][yy] )
			updtht('o', xx, yy);
		    else{
			thtcnt[0][threat[0][xx][yy]] -= 1;
			threat[0][xx][yy] = 0;
		    }
		}
		if(!oflag){
		    t_hvlr[way][1][xx][yy] = tmptht[way][1][i];
		    if( t_hvlr[0][1][xx][yy] | t_hvlr[1][1][xx][yy] |
			t_hvlr[2][1][xx][yy] | t_hvlr[3][1][xx][yy] )
			updtht('x', xx, yy);
		    else{
			thtcnt[1][threat[1][xx][yy]] -= 1;
			threat[1][xx][yy] = 0;
		    }
		}
	    }
	}
    }
    return(win);
}

