# define TH_3 1
# define TH_4 2
# define FORCED 3
# define READY 4

# define LOGIC char
# define TRUE 1
# define FALSE 0

/* ZIZEs are different from unixluff */
# define XZIZE 19
# define YZIZE 19
# define BZIZE 361 /* YZIZE * XZIZE */

int width;
int height;

char board[XZIZE][YZIZE];
short int play[XZIZE][YZIZE];
short int playcnt;

char point[XZIZE][YZIZE];
char p_hvlr[4][XZIZE][YZIZE];
char threat[2][XZIZE][YZIZE];
char t_hvlr[4][2][XZIZE][YZIZE];
short int thtcnt[2][5];
char thtmax[2];
char hvlr[4][18];

unsigned char automat[][13];
char tmppts[4][2][17];
char tmptht[4][2][17];

LOGIC slf;

