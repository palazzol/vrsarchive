# include "def.h"
# include "stdio.h"

/* Saves and resaves a position. File for saving is luff.out */

save(){
	int x, y;
	FILE *fd;

	fd = fopen("luff.out", "w");
	for(y = 0; y < YZIZE; y++){
		for(x = 0; x < XZIZE; x++)
			fprintf(fd, "%3d%c", play[x][y], board[x][y]);
		fprintf(fd, "\n");
	}
}

resave(){
	int x, y;
	char temp;
	FILE *fd;

	if((fd = fopen("luff.out", "r")) == 0){
		printf("Can't open the file luff.out\n");
		return;
	}
	playcnt = 0;
	for(y = 0; y < YZIZE; y++){
		for(x = 0; x < XZIZE; x++){
			fscanf(fd, "%3d%c", &play[x][y], &temp);
			if(play[x][y] > playcnt)
				playcnt = play[x][y];
			if(temp != board[x][y]){ /* [ndring */
				board[x][y] = temp;
				update(x, y);
				pos(x, y);
				if(board[x][y] == ' ')
					putchar('.');
				else
					putchar(board[x][y]);
			}
		}
		fscanf(fd, "\n");
	}
}
