# include "def.h"
# include <stdio.h>
# include <signal.h>
extern SIG_T cheat();

/* makes the program play self */
SIG_T
normal(dummy){
	signal(SIGINT, cheat);
	slf = 0;
}

/* Routin for input of moves and commands */
inmove(x, y) int *x, *y;{
	char buff[11], who;
	int i, j, xx, yy;

	while(TRUE){
		pos(1, YZIZE + 2);
		printf("Your move: ");
		scanf("%s", buff); emptyline();
		up(); up(); clear();
		if(!strcmp(buff, "board")){
			utboard(0);
			continue;
		}
		if((playcnt == 0) && !strcmp(buff, "start")){
			xx = XZIZE/2;
			yy = YZIZE/2;
			board[xx][yy] = 'o';
			update(xx, yy);
			play[xx][yy] = ++playcnt;
			pos(xx, yy);
			putchar('o');
			pos(1, YZIZE +1);
			clear();
			continue;
		}
		if(!strcmp(buff,"remove")){
			while(TRUE){
				if(playcnt == 0)
					break;
				for(xx = 0; xx < XZIZE; xx++)
					for(yy = 0; yy < YZIZE; yy++)
						if(play[xx][yy] == playcnt){
							play[xx][yy] = 0;
							who = board[xx][yy];
							board[xx][yy] = ' ';
							update(xx, yy);
							pos(xx, yy);
							printf(".\b");
						}
				playcnt--;
				if(who == 'x')
					break;
			}
			continue;
		}
		if(!strcmp(buff, "play")){
			for(xx = 0; xx < XZIZE; xx++)
				for(yy = 0; yy < YZIZE; yy++)
					if(play[xx][yy]){
						pos(xx, yy);
						printf("%2d", play[xx][yy]);
					}
			continue;
		}
		if(!strcmp(buff, "help")){
			home(); clear();
			printf("b5, c12 :Examples on moveformats\n");
			printf("remove  :Taking back moves\n");
			printf("slf     :The program plays self\n");
			printf("point   :Shows pointboard\n");
			printf("pts     :Shows way pointboard\n");
			printf("threat  :Shows threatboard\n");
			printf("tht     :Shows way threatboard\n");
			printf("play    :Shows the game\n");
			printf("board   :Rewrite board\n");
			printf("save    :Saves the game\n");
			printf("resave  :Resave the game\n");
			printf("automat :Runs pattern recognition\n");
			printf("hint    :gives you a hint\n");
			printf("\n\n\nPush <return> when you'r ready\n");
			emptyline();
			utboard(0);
			continue;
		}
		if(!strcmp(buff, "save")){
			save();
			continue;
		}
		if(!strcmp(buff, "resave")){
			resave();
			continue;
		}
		if(!strcmp(buff, "slf")){
			signal(SIGINT, normal);
			slf = TRUE;
			return;
		}
		if(!strcmp(buff, "point")){
			utboard(point);
			continue;
		}
		if(!strcmp(buff, "pts")){
			printf("Which way -|\\/(0123): ");
			scanf("%d", &i); emptyline();
			utboard(p_hvlr[i]);
			continue;
		}
		if(!strcmp(buff, "threat")){
			printf("Which player ox(01): ");
			scanf("%d", &i); emptyline();
			utboard(threat[i]);
			continue;
		}
		if(!strcmp(buff, "tht")){
			printf("Which way and player -|\\/(0123) ox(01): ");
			scanf("%d%d", &i, &j); emptyline();
			utboard(t_hvlr[i][j]);
			continue;
		}
		if(!strcmp(buff, "automat")){
			printf("What position: ");
			scanf("%s", buff); emptyline();
			up();
			printf("Which way -|\\/(0123); ");
			scanf("%d", &j); emptyline();
			if(inposition(x, y, buff)){
				catch(*x, *y);
				autom();
				pos(XZIZE + 2, 0);
				printf("    pto ptx tho thx");
				for(i = 0; i < 17; i++){
					pos(XZIZE + 2, i + 1);
					printf("%3c", hvlr[j][i]);
					printf("%4d", tmppts[j][0][i]);
					printf("%4d", tmppts[j][1][i]);
					printf("%4d", tmptht[j][0][i]);
					printf("%4d", tmptht[j][1][i]);
				}
			}
			continue;
		}
		if(!strcmp(buff, "hint")){
			if(chose('x', &xx, &yy))
				printf("My hint is: %c%d", 'a' + xx, yy);
			continue;
		}
		if(inposition(x, y, buff))
			return;
		printf("\n\nType help for help");
	}
}

/* Converts a text string type g11 to a position */

inposition(x, y, buff) int *x, *y; char buff[11];{
	*x = buff[0] - 'a';
	*y = atoi(&buff[1]);
	if((*x >= 0) && (*x < XZIZE) && (*y >= 0) && (*y < YZIZE)
	    && (buff[1] >= '0') && (buff[1] <= '9')
	    && board[*x][*y] == ' ')
		return(1);
	return(0);
}

/* Empty a line */

emptyline(){
	while(getchar() != '\n');
}
