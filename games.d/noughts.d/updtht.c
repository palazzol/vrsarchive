# include "def.h"

/* Updates the threat board */
updtht(who, x, y) char who; int x, y;{
	short int t[5];
	register int i;

	for(i = 0; i < 5; i++)
		t[i] = 0;

	if(who == 'x')
		who = 1;
	else
		who = 0;

	for(i = 0; i < 4; i++) /* Count threats in the four ways */
		t[t_hvlr[i][who][x][y]]  += 1;

	thtcnt[who][threat[who][x][y]] -= 1;
	threat[who][x][y] = 0;

	if((  t[1])                       /* 3 */
	  || (t[2]))                      /* stopped 4 */
		threat[who][x][y] = 1;

	if(t[1] > 1)                      /* 3-3-combination */
		threat[who][x][y] = 2;

	if(((t[2] > 0) && (t[1] > 0))     /* 4-3-combination */
	  || (t[2] > 1)                   /* 4-4-combination */
	  || (t[3] > 0))                  /* Free 4 */
		threat[who][x][y] = 3;

	if(t[4] > 0)                      /* 5 = win */
		threat[who][x][y] = 4;

	thtcnt[who][threat[who][x][y]] += 1;
}
