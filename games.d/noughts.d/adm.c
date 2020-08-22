# include "def.h"
# include <stdio.h>
# include <signal.h>

/* If you push BREAK you are a cheater */
SIG_T
cheat(dummy){
	signal(SIGINT, cheat);
	pos(1, YZIZE + 2);
	if(!slf)
		printf("Cheater. I win on V.O.\n");
	exit(0);
}

/* The main program which administrate who's on the move etc */

main(){
	int x, y;
	char svar[16];
	char *term, *getenv();

	term = getenv("TERM");
	if(getcap(term) == -1){
		printf("Wrong or no TERM: %s\n", term);
		exit(1);
	}
	if((height < 24) || (width < 80)){
		printf("To smal screen (%d x %d). Min is (80 x 24)\n",
			width, height);
		exit(1);
	}

	for(x = 0; x < XZIZE; x++)
		for(y = 0; y < YZIZE; y++)
			board[x][y] = ' ';
	srand( getpid() );
	utboard(0);
	signal(SIGINT, cheat);
	pos(1, YZIZE + 1);
	printf("Your move f.ex: k12. Computer start: start");

	while(TRUE){
		if(!slf)
			inmove(&x, &y);
		if(slf)
			if(!chose('x', &x, &y)){
				pos(1, YZIZE + 2);
				printf("Can't find a place for my 'x'\n");
				break;
			}
		play[x][y] = ++playcnt;
		pos(x, y); printf("x\b"); fflush(stdout);
		board[x][y] = 'x';
		if(update(x, y)){
			pos(1, YZIZE + 2);
			printf("Congratulations you won\n");
			break;
		}
		if(playcnt > 1)
			for(x = 0; x < XZIZE; x++)
				for(y = 0; y < YZIZE; y++)
					if(play[x][y] == playcnt - 1){
						pos(x, y);
						printf("o\b");
						fflush(stdout);
					}
		if(!chose('o', &x, &y)){
			pos(1, YZIZE + 2);
			printf("Can't find a place for my 'o'\n");
			break;
		}
		play[x][y] = ++playcnt;
		if(!slf){
			pos(1, YZIZE + 1);
			printf("My move:  %c%d", 'a' + x, y);
		}
		pos(x, y);
		printf("O\b");
		fflush(stdout);
		board[x][y] = 'o';
		if(update(x, y)){
			pos(1, YZIZE + 2);
			printf("I won\n");
			break;
		}
	}
	signal(SIGINT, SIG_DFL); /* Skip cheat */
	printf("Do you want to save the game? (y/n)  ");
	svar[0] = getchar(); emptyline();
	if(svar[0] == 'y'){
		save();
		exit(0);
	}
	if(svar[0] == 'n')
		exit(0);
	printf("Hmmm. Asumes you don't.\n");
	return 0;
}
