/* battleships by Judah S. Kaminetsky */
/*cc -obs bs.c -lcurses*/
#include <curses.h>

#define MAX_X 12
#define MAX_Y 12
#define NO 1
#define YES 0

#ifndef A_REVERSE
#define wattron(win,notused)	wstandout(win)
#define wattroff(win,notused)	wstandend(win)
#define cbreak()		crmode()
#define flash()
#endif

long r;/* for random number generator*/
int myscore = 0;
int hisscore = 0;
int hismoves = 1;
int mymoves = 1;
int nextx, nexty;/* next square to attack if adjacent to previous hit*/

main()
{
	WINDOW *mywin;/*player's screen*/
	WINDOW *myscrwin;/*players's score window*/
	WINDOW *hisscrwin;/*computers's score window*/
	WINDOW *hidwin;/* computers hidden screen */
	WINDOW *showwin;/*computer's displayed screen*/
	WINDOW *recwin;/*computers record of my screen */
	int t;
	extern long time();
	extern void srand();

	(void) time(&r);/*Seed the random number generator*/
	srand((unsigned)(r&0177777L));

	initscr();
	nonl();
	noecho();
	cbreak();

#ifdef EMAIL
	err("Battleships System Test - comments to hounx!juda");
#endif

	mywin = newwin(MAX_Y, MAX_X, 5, 15);
	recwin = newwin(MAX_Y, MAX_X, 5, 0);
	overlay(mywin, stdscr);
	overlay(recwin, stdscr);
	werase(mywin);
	werase(recwin);

	wattron(mywin,A_REVERSE);
	label_x(mywin);
	label_y(mywin);
	wattroff(mywin,A_REVERSE);
	wrefresh(mywin);

	wattron(recwin,A_REVERSE);
	label_x(recwin);
	label_y(recwin);
	wattroff(recwin,A_REVERSE);

	set(mywin,'A',5);
	wrefresh(mywin);
	set(mywin,'B',4);
	wrefresh(mywin);
	set(mywin,'S',3);
	wrefresh(mywin);
	set(mywin,'D',3);
	wrefresh(mywin);
	set(mywin,'P',2);
	wrefresh(mywin);
	clrtop();

	hidwin = newwin(MAX_Y, MAX_X, 5, 55);
	overlay(hidwin, stdscr);
	showwin = newwin(MAX_Y, MAX_X, 5, 55);
	overlay(showwin, stdscr);
	myscrwin = newwin(4, MAX_X+3, 17, 55);
	overlay(myscrwin, stdscr);
	hisscrwin = newwin(4, MAX_X+3, 17, 15);
	overlay(hisscrwin, stdscr);
	werase(hidwin);
	werase(showwin);

	wattron(hidwin,A_REVERSE);
	label_x(hidwin);
	label_y(hidwin);
	wattroff(hidwin,A_REVERSE);
	wrefresh(hidwin);

	wattron(showwin,A_REVERSE);
	label_x(showwin);
	label_y(showwin);
	wattroff(showwin,A_REVERSE);

	setup(hidwin,'A',5);
	setup(hidwin,'B',4);
	setup(hidwin,'S',3);
	setup(hidwin,'D',3);
	setup(hidwin,'P',2);

	clrtop();
	for(t=1;t<101;t++)
	{
		myttack(hidwin,showwin);
		myrecord(myscrwin);
		if(myscore>16)
		{
			msg("YOU WIN!!");
			sleep(3);
			move(LINES-1,0);
			refresh();
			endwin();
			(void) exit(1);
		}
		hisattack(mywin,recwin);
		hisrecord(hisscrwin);
		if(hisscore>16)
		{
			msg("YOU LOSE!!");
			sleep(3);
			move(LINES-1,0);
			refresh();
			endwin();
			(void) exit(1);
		}
	}
	endwin();
	delwin(mywin);
	delwin(recwin);
	delwin(hidwin);
	delwin(showwin);
	delwin(myscrwin);
	delwin(hisscrwin);
}
/*************************************************************/
/* Draw x axis labels  */
label_x(win)
WINDOW *win;
{
	int x,y;
	int ch;
	for(y=0;y<MAX_Y;y+=MAX_Y-1)
	{
		for(x=1,ch=0;x<MAX_X-1;x++,ch++)
		{
			wmove(win,y,x);
			waddch(win,ch+'0');
		}
	}
	touchwin(win);
}
/*****************************************************************/
/* Draw y axis labels */
label_y(win)
WINDOW *win;
{
	int x,y;
	/* in columns 1 and MAX_X */
	for(x=0;x<MAX_X;x+=MAX_X-1)
	{
		for(y=0;y<MAX_Y;y++)
		{
			wmove(win,y,x);
			waddch(win,y+'/');/*start with char 0*/
		}
	}
	touchwin(win);
}
/************************************************************************/
/* Place ship of length len, represented by character ch at coordinates
y, x */
/* direction up, down, right, left from start */
place(win,len,ch,init_y,init_x,dir)
WINDOW *win;
int len;/*length*/
char ch;
int init_y,init_x;
int dir;/*direction*/
{
	int i;
	char c;
	int dir_y, dir_x;
	int x, y;

	dir_x=xdir(dir);
	dir_y=ydir(dir);

	/* avoid collisions with existing characters*/
	for(i=0,x=init_x,y=init_y;i<len;i++,x=x+dir_x,y=y+dir_y)
	{
		wmove(win,y,x);
		c=winch(win);
		if(c!=' ')
		{
			return(1);
		}
	}

	/* place characters if no collision */
	for(i=0,x=init_x,y=init_y;i<len;i++,x=x+dir_x,y=y+dir_y)
	{
		wmove(win,y,x);
		waddch(win,ch);
	}
	return(0);
}
/*********************************************************************/
/* Determine x direction increment or decrement */
xdir(dir)
int dir;
{
	int xdir1;

	switch(dir){
	case 0 :/*up*/
		xdir1=0;
		break;
	case 1 :/*down*/
		xdir1=0;
		break;
	case 2 :/*right*/
		xdir1=1;
		break;
	case 3 :/*left*/
		xdir1=(-1);
		break;
	default:
		xdir1=1;
		break;
	}
	return(xdir1);
}
/*********************************************************************/
/* Determine y direction increment or decrement */
ydir(dir)
int dir;
{
	int ydir1;

	switch(dir){
	case 0 :/*up*/
		ydir1=(-1);
		break;
	case 1 :/*down*/
		ydir1=1;
		break;
	case 2 :/*right*/
		ydir1=0;
		break;
	case 3 :/*left*/
		ydir1=0;
		break;
	default:
		ydir1=0;
		break;
	}
	return(ydir1);
}
/***********************************************************************/
/* generate random number between 0 and high (arg1), retrun random number */
random(high)
int high;
{
	return(rand() % high+1);
}
/********************************************************************/
/* place ship for computer*/

setup(win,ship,length)
WINDOW *win;
char ship;
int length;
{
	int y , x;
	int dir;
	while(1)
	{
		msg("The computer is now placing its ships in a random manner");
		x=random(11);
		y=random(11);
		dir=random(3);
		if(place(win,length,ship,y,x,dir)!=1)
		{
			return;
		}
	}
}
/********************************************************************/
/* get x coordinate from user*/
get_x()
{
	WINDOW *xwin;
	int c;

	xwin = newwin(1, COLS, 1, 0);/*line 1*/
	overlay(xwin, stdscr);
	werase(xwin);
	wprintw(xwin,"Enter x (horizontal) coordinate [0-9] (q to exit):\t");
	touchwin(stdscr);
	wrefresh(xwin);
	c=wgetch(xwin);
	out(c);
	waddch(xwin,c);
	touchwin(stdscr);
	wrefresh(xwin);
	delwin(xwin);
	return(c-'0'+1);
}
/********************************************************************/
/* get y coordinate from user*/
get_y()
{
	WINDOW *ywin;
	int c;

	ywin = newwin(1, COLS, 2, 0);/*line 2*/
	overlay(ywin, stdscr);
	werase(ywin);
	wprintw(ywin,"Enter y (vertical) coordinate [0-9] (q to exit):\t");
	touchwin(stdscr);
	wrefresh(ywin);
	c=wgetch(ywin);
	out(c);
	waddch(ywin,c);
	touchwin(stdscr);
	wrefresh(ywin);
	delwin(ywin);
	return(c-'0'+1);
}
/*********************************************************************/
/* get dir  from user*/
get_dir()
{
	WINDOW *dirwin;
	int c;
	dirwin = newwin(1, COLS, 3, 0);/*line 3*/
	overlay(dirwin, stdscr);
	werase(dirwin);
	wprintw(dirwin,"Enter Direction, 0 is up, 1 is down, 2 is right, 3 is left: (q to exit)");
	touchwin(stdscr);
	wrefresh(dirwin);
	c=wgetch(dirwin);
	out(c);
	waddch(dirwin,c);
	touchwin(stdscr);
	wrefresh(dirwin);
	delwin(dirwin);
	return(c-'0');
}
/*********************************************************************/
/* place ship for player*/
set(win,ship,length)
WINDOW *win;
char ship;
int length;
{
	int y , x;
	int dir;
	char msg_str[80];
	while(1)
	{
		(void) sprintf(msg_str,"PLACE SHIP %c, length %d avoiding collisions",ship,length);
		msg(msg_str);
		if(verify(x=get_x(),0,10)==1)
		{
			continue;
		}
		if(verify(y=get_y(),0,10)==1)
		{
			continue;
		}
		if(verify(dir=get_dir(),0,3)==1)
		{
			continue;
		}
		if(place(win,length,ship,y,x,dir)!=1)
		{
			touchwin(win);
			return;
		}
	}
}
/***********************************************************************/
msg(str)
char *str;
{
	WINDOW *msgwin;

	msgwin=newwin(1,COLS,0,0);
	overlay(msgwin, stdscr);
	werase(msgwin);
	wattron(msgwin,A_REVERSE);
	wprintw(msgwin,"%s",str);
	wattroff(msgwin,A_REVERSE);
	touchwin(stdscr);
	wrefresh(msgwin);
	delwin(msgwin);
	return;
}
/***********************************************************************/
verify(c,low,high)
int c;
int low;
int high;
{
	char msgstr[80];
	if(c<low||c>high)
	{
		(void) sprintf(msgstr,"%c is out of legal range %c to %c",c,low,high);
		return(1);
	}
	else
	{
		return(0);
	}
}
/*********************************************************************/
/*player attack computer - goes to attack coordinates on hidwin*/
/*and copies ch (+attributes?) to showwin - touchwin and refresh showwin*/
/* allows duplicate moves but does not score duplicaate hits*/
myttack(hidwin,showwin)
WINDOW *hidwin;
WINDOW *showwin;
{
	int y , x;
	int hit;
	char c;
	char d;

	msg("ATTACK!!");
	while(verify(x=get_x(),0,10)==1)
	{
	}
	while(verify(y=get_y(),0,10)==1)
	{
	}
	wmove(hidwin,y,x);
	c=winch(hidwin);
	if(c!=' ')
	{
		hit=YES;
	}
	wmove(showwin,y,x);
	d=winch(showwin);
	if(hit==YES&&d==' ')/* first hit - not repeat */
	{
		flash();
		myscore++;
	}
	wattron(showwin,A_REVERSE);
	waddch(showwin,(c != ' ')? '*' : '.');
	wattroff(showwin,A_REVERSE);
	touchwin(showwin);
	wrefresh(showwin);
}
/***************************************************************************/
/* clear top of screen */
clrtop()
{
	WINDOW *topwin;

	topwin = newwin(5, COLS, 0, 0);/*top 5 lines*/
	overlay(topwin, stdscr);
	werase(topwin);
	touchwin(topwin);
	wrefresh(topwin);
	delwin(topwin);
}
/*********************************************************************/
/*computer attack player - goes to attack coordinates on recwin*/
/* checks recwin - blank means haven't attacked here yet */
/* if so go to coord on mywin - attack & record result on recwin */
hisattack(mywin,recwin)
WINDOW *mywin;
WINDOW *recwin;
{
	int y , x;
	char c;
	char mark='+';

	clrtop();
	while(1)
	{
		if(nextshot(recwin)==YES)
		{
			x=nextx;
			y=nexty;
		}
		else if(nextshot(recwin)==NO)
		{
			x=random(11);
			y=random(11);
		}


		wmove(recwin,y,x);/*check for repeat move*/
		c=winch(recwin);

		if(c!=' ')/*repeat move */
		{
			continue;
		}
		else
		{
			wmove(mywin,y,x);
			c=winch(mywin);
			if(c!=' ')/*hit*/
			{
				flash();
				mark=c;/*mark recwin with ship character*/
				hisscore++;
			}
			wattron(mywin,A_REVERSE);
			waddch(mywin,(c != ' ')? '*' : '.');
			wattroff(mywin,A_REVERSE);
			touchwin(mywin);
			wrefresh(mywin);

			waddch(recwin,mark);
			/*mark square as tried already and result + for blank or char for ship */
			touchwin(recwin);
			return;
		}
	}
}
/***********************************************************************/
err(str)
char *str;
{
	WINDOW *errwin;

	errwin=newwin(1,COLS,23,0);
	overlay(errwin, stdscr);
	werase(errwin);
	wattron(errwin,A_REVERSE);
	wprintw(errwin,"%s",str);
	wattroff(errwin,A_REVERSE);
	touchwin(stdscr);
	wrefresh(errwin);
	delwin(errwin);
	return;
}
/***********************************************************************/
out(c)
char c;
{
	if(c=='q')
	{
		endwin();
		(void) exit(1);
	}
}
/*********************************************************************/
myrecord(win)
WINDOW *win;
{

	werase(win);
	wprintw(win,"hit %d ",myscore);
	wprintw(win,"move %d\n",mymoves++);
	touchwin(win);
	wrefresh(win);
}
/*********************************************************************/
hisrecord(win)
WINDOW *win;
{

	werase(win);
	wprintw(win,"hit %d ",hisscore);
	wprintw(win,"move %d\n",hismoves++);
	touchwin(win);
	wrefresh(win);
}
/************************************************************************/
/* check win for previous hit and choose next guess at adjacent square */

nextshot(win)
WINDOW *win;
{
	int y;
	int x;
	int ax;
	int by;
	char c;

	for(by=1;by<MAX_Y-1;by++)
	{
		for(ax=1;ax<MAX_X-1;ax++)
		{
			wmove(win,by,ax);
			c=winch(win);
			if(c!=' '&&c!='+')
			/* space was hit previously */
			{
				x=ax+1;/*right*/
				y=by;
				if(check(win,y,x)==YES)
				{
					return(YES);
				}
				x=ax-1;/*left*/
				y=by;
				if(check(win,y,x)==YES)
				{
					return(YES);
				}
				x=ax;
				y=by+1;/*1 square down */
				if(check(win,y,x)==YES)
				{
					return(YES);
				}
				x=ax;
				y=by-1;/*down*/
				if(check(win,y,x)==YES)
				{
					return(YES);
				}
			}
		}
	}
	return(NO);/* no untried squares adjacent to hits */
}
/*******************************************************************/
/* check y,x on win, return YES if blank - not shot at yet
return NO if not blank */

check(win,y,x)
WINDOW *win;
int y;
int x;
{

	char c;

	wmove(win,y,x);
	c=winch(win);
	if(c==' ')
	/* adjacent to previous hit & blank */
	{
		nextx=x;
		nexty=y;
		return(YES);
	}
	else
	{
		return(NO);
	}
}
