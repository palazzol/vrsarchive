/* Tetrix by quentin */

#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <curses.h>
#include "tet.h"

/********** Some global variable declarations ************/
int Type;		/* type specifies rotation, shape and color of blocks */
int Row;		/* Row of pivot point of block */
int Column;		/* Column of pivot point of block */
int Pause;		/* Time between movements this block */
int CurrentPause;	/* Time between movements per level */
int FallingDown;	/* True when space bar is pressed */
int Beep;
char Key;		/* holds last key polled */

#ifndef SCORE_FILE
#define SCORE_FILE	"/usr/tmp/.TetScores"
#endif
char ScoreString[10];
struct ScoreStruct {
	char Name[10];
	int Score;
} High[10];
int ThisScore,HighsChanged;
		
char *ttyname();
char combuf[2];
char Board[BOARD_WIDE][BOARD_HIGH];
char Temp[BOARD_WIDE][BOARD_HIGH];	/* temp storage for TestRows */

/* debug flash to screen */
#define FLASHMSG(x)
/*
#define FLASHMSG(x)	      { mvaddstr(23,0,"                "); \
				mvaddstr(23,0,x); \
				refresh(); }
*/
#define UPSCORE(x)    { ThisScore += x; \
			sprintf((char *)ScoreString,"%-d",ThisScore); \
			mvaddstr(1,46,ScoreString); }

#define NULL_KEY	'\0'
#define FALL_KEY	' '
#define RIGHT_KEY	'l'
#define LEFT_KEY	'j'
#define ROTATE_KEY	'k'
#define L_RIGHT_KEY	'f'		/* for south paws */
#define L_LEFT_KEY	's'
#define L_ROTATE_KEY	'd'
#define QUIT_KEY	'q'
#define BEEP_KEY	'b'
#define BOSS_KEY	'\033'
#define PLAY_KEY	'p'
#define SCORE_KEY	'h'
#define MENU_KEY	'm'

#ifdef M_XENIX
beep()
{
	write(1, "\007", 1);
}

void srand48(seed)
long seed;
{
	srand((unsigned)seed);
}

long mrand48()
{
	return((long)rand()*rand());
}
#endif /* M_XENIX */

/**************************************************MAIN*****/
main()
{
	Init();
	for ( ; ; ) {
		NewGame();
		Play();
		ScoreIt();
		DrawScore();
		}
}

/*************************************************************/
Init()
{
	register char *ttnam, *p;
	register int x,y,i,fd;
	int timein;

	time(&timein);		/* get start time */
	srand48(timein);	/* start rand randomly */

	ttnam = ttyname (0); 
	close (0); 
	open (ttnam, O_NDELAY);
	/*
	 * setup raw mode, no echo
	 */
	initscr();
	crmode();
	noecho();
	signal(SIGINT, SIG_IGN);
	
	Beep=0;
	HighsChanged = 0;
	ScoreIt();
	/* initilialize board to spaces */
	for (x=0; x<BOARD_WIDE; x++) 
		for (y=0; y<BOARD_HIGH; y++) 
			PUTCH(x,y,NO_CHAR);
	erase();
	DrawMenu();
	refresh();
}

/**************************************************************/
NewGame()
{
	register int x,y;

	CurrentPause=0;

	while (!CurrentPause) {
		GetKey();
		switch (Key) {
			case BEEP_KEY   : Beep = !Beep ;
					  if (Beep) beep(); 
					  break;
			case SCORE_KEY  : DrawScore(); break;
			case MENU_KEY	: DrawMenu(); break;
			case BOSS_KEY	: Boss(); break;
			case PLAY_KEY	: CurrentPause=100; break;
			case QUIT_KEY   : Leave();
			}
	}
	/* initilialize board to spaces */
	for (x=0; x<BOARD_WIDE; x++) 
		for (y=0; y<BOARD_HIGH; y++) 
			PUTCH(x,y,NO_CHAR);
	ThisScore=0;
	mvaddstr(1,42,"|  ......  |");
	UPSCORE(0);

}

/******************************************************************/
DrawMenu()
{
	register int y;
	erase(); 

	/* draw score border */
	mvaddstr(0,42,".----------.");
	mvaddstr(1,42,"|  ......  |");
	mvaddstr(2,42,"`----------'");
	UPSCORE(0);

	/* draw menu */
	mvaddstr( 4,35,".---------------------------.");
	mvaddstr( 5,35,"|                           |");
	mvaddstr( 6,35,"|      ..   Menu   ..       |");
	mvaddstr( 7,35,"|                           |");
	mvaddstr( 8,35,"|   h     .... high scores  |");
	mvaddstr( 9,35,"|   b     .... toggle beep  |");
	mvaddstr(10,35,"|   p     .... play         |");
	mvaddstr(11,35,"|   q     .... quit         |");
	mvaddstr(12,35,"|                           |");
	mvaddstr(13,35,"| s or j  .... move left    |");
	mvaddstr(14,35,"| d or k  .... rotate piece |");
	mvaddstr(15,35,"| f or l  .... move right   |");
	mvaddstr(16,35,"|  spc    .... fall piece   |");
	mvaddstr(17,35,"|  esc    .... pause        |");
	mvaddstr(18,35,"|                           |");
	mvaddstr(19,35,"| LATEST: allow concurrent  |");
	mvaddstr(20,35,"|    high score setting     |");
	mvaddstr(21,35,"`---------------------------'");

	/* draw game border */
	mvaddstr(0,14, ".----------.");
	mvaddstr(21,14,"`----------'");
	for (y=1; y<21; y++)
		mvaddstr(y,14,"|          |");

	/* display the title */
	mvaddstr(3,17,"TETRIX");
	refresh();
}

/**************************************************************/
Play()
{
while ((Key != QUIT_KEY) && NewPiece()) {
	FallingDown = 0;
	do {	/* do until we can't Advance the piece */
		if (FallingDown) Pause = 0;
		else Pause = CurrentPause;
		while (Pause) {		/* do this until pause runs out */
			Pause--;
			switch (Key) {
				case BOSS_KEY     : Boss(); break;
				case QUIT_KEY     : CurrentPause = 0;
				case FALL_KEY     : FallingDown = 1;
						    UPSCORE(20-Row);
						    Pause = 0; break;
				case RIGHT_KEY    :
				case L_RIGHT_KEY  : MoveRight(); break;
				case LEFT_KEY     :
				case L_LEFT_KEY   : MoveLeft(); break;
				case ROTATE_KEY   :
				case L_ROTATE_KEY : Rotate(); break;
				case NULL_KEY     : break;
				default           : if (Beep) beep();
				}
			GetKey();
			}
		} while (AdvancePiece());
	UPSCORE(5);
	TestRows();
	}
}

/*********************************************************************/
ScoreIt()
{
	register int  oldmask,fd,i,j;

	oldmask = umask(0);
	if ((fd=open(SCORE_FILE,O_CREAT|O_RDONLY,0666)) != -1) {
		read(fd,High,sizeof(High));
		close(fd);
	}
	else {
	for(i=0; i<10; i++)
		High[i].Score = 0;
	for(i=0; i<10; i++)
		strncpy("         ",High[i].Name,10);
	}
	umask(oldmask);

	for (i=0; i<10; i++)		/* place this guy */
		if (High[i].Score <= ThisScore) break;

	if (i < 10 )			/* insert this score */
	{
		HighsChanged = 1;
		for (j=9; j>i; j--)		/* move down others */
			if (High[j-1].Score)
			{
				High[j].Score = High[j-1].Score;
				strncpy(High[j].Name,High[j-1].Name,10);
			}
		cuserid((char *) High[i].Name);
		High[i].Score = ThisScore;
	}

	if (HighsChanged)
	{
		if ((fd=open(SCORE_FILE,O_RDWR)) != -1) {
			write(fd,High,sizeof(High));
			close(fd);
		}
		else mvaddstr(22,0,"Couldn't open high score file.");
	}
	
}

/***********************************************************************/
DrawScore()
{
	register int j;

	mvaddstr( 5,35,"|     Hit 'm' for menu      |");
	mvaddstr( 6,35,"|                           |");
	mvaddstr( 7,35,"|        HIGH SCORES        |");
	mvaddstr( 8,35,"| 1.                        |");
	mvaddstr( 9,35,"| 2.                        |");
	mvaddstr(10,35,"| 3.                        |");
	mvaddstr(11,35,"| 4.                        |");
	mvaddstr(12,35,"| 5.                        |");
	mvaddstr(13,35,"| 6.                        |");
	mvaddstr(14,35,"| 7.                        |");
	mvaddstr(15,35,"| 8.                        |");
	mvaddstr(16,35,"| 9.                        |");
	mvaddstr(17,35,"|10.                        |");
	mvaddstr(18,35,"|                           |");
	mvaddstr(19,35,"|                           |");
	mvaddstr(20,35,"|                           |");
	
	for (j=0; j<10; j++)
	   if (High[j].Score)
	   {
	      mvprintw(j+8,41,"%-s",(char *)High[j].Name);
	      mvprintw(j+8,54,"%d",High[j].Score);
	   }
	refresh();

}

/*********************************************************************/
Boss()
{	register int x,y;

	clear();
	refresh();
	echo();
	nocrmode();
	system("sh </dev/tty >/dev/tty");
	noecho();
	crmode();
	clear();
	DrawMenu();
	/* restore board */
	for (x=0; x<BOARD_WIDE; x++) 
		for (y=0; y<BOARD_HIGH; y++) 
			PUTCH(x,y,Board[x][y]);
	refresh();

}

/*********************************************************************/
GetKey()
{
/*	fflush(stdout); */
	noraw();	/* This accomplishes a flush */
	raw();
	Key = NULL_KEY;
	top:
	if (read (0, combuf, 1) == 0) 
		return;
	else Key = (*combuf&0177); 
	goto top;
}

/************************************************************************/
/* Could be a macro for speed but cpp runs out of tree space in CanMove */
IS_FREE(x,y)
int x,y;
{
	if ((y < 0) || (y >= BOARD_HIGH) || (x < 0) || (x >= BOARD_WIDE))
		return(0);
	if (Board[x][y] != NO_CHAR)
		return(0);
	else return(1);
}

/*********************************************************************/
TestRows()
{	register int x,y,tempy,fullrow;
	int marked[BOARD_HIGH];

for (y=0; y<BOARD_HIGH; y++) {
	marked[y] = 0;
	for (x=0; x<BOARD_WIDE; x++)
		Temp[x][y] = NO_CHAR;
	}

/* main loop to traverse Board, looking for fullrows */
/* as it looks, it copies non full ones over to Temp */
tempy=BOARD_HIGH-1;
for (y=BOARD_HIGH-1; y>=0; y--) {
	fullrow = 1;
	for (x=0; x<BOARD_WIDE; x++)		/* check for any holes at all */
		if (IS_FREE(x,y)) { fullrow = 0; break; }
	if (fullrow) {
		marked[y]++;
		CurrentPause--;			/* speed up the game */
	}
	else    {
		for (x=0; x<BOARD_WIDE; x++)
			Temp[x][tempy] = Board[x][y];
		tempy--;
		}
	}

/* flash the rows that will die */
for (tempy=1; tempy<5; tempy++)
	for (y=BOARD_HIGH-1; y>=0; y--) 
		if (marked[y]) {	
			UPSCORE(30-y);
			for (x=0; x<BOARD_WIDE; x++)	
				PUTCH(x,y,BRITE_CHAR);
			refresh();
			for (x=0; x<BOARD_WIDE; x++)	
				PUTCH(x,y,NO_CHAR);
			refresh();
			}

/* Move temp back to Board */
for (y=BOARD_HIGH-1; y>=0; y--) {
	for (x=0; x<BOARD_WIDE; x++)
		PUTCH(x,y,Temp[x][y]);
	refresh();
	}
}

/***********************************************************/
Leave()
{
	erase();
	mvaddstr(22,48,"Tetrix says Bye\n");
	mvaddstr(23,0,"");
	refresh();
	sleep(1);
	endwin();
	exit(0);
}
