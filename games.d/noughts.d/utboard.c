# include "def.h"

/* Prints out an image of th board with or without points */
utboard(p) char p[XZIZE][YZIZE];{
	register int x, y;

	home(); clear();
	printf("  ");
	for(x = 0; x < XZIZE; x++)
		printf(" %c", 'a' + x);
	putchar('\n');
	for(y = 0; y < YZIZE; y++){
		printf("%2d", y);
		for(x = 0; x < XZIZE; x++){
			if(board[x][y] == ' ')
				if(p && p[x][y])
					if(p[x][y] > 077)
						printf(" >");
					else
						printf("%2o", p[x][y]);
				else{
					putchar(' ');
					putchar('.');
				}
			else{
				putchar(' ');
				putchar(board[x][y]);
			}
		}
		printf(" %2d\n", y);
	}
	printf("  ");
	for(x = 0; x < XZIZE; x++)
		printf(" %c", 'a' + x);
	pos(XZIZE + 2, 0); printf("Luff7.1");
	pos(XZIZE + 2, 1); printf("6/4 1982");
}
