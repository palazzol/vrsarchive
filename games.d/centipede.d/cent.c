/* Centipede
   Copyright 1987 by Nathan Glasser
   Do not redistribute this source code without including this
   copyright notice.
*/


#include "cent.h"

main(argc,argv)
int argc;
char **argv;
{
    int rest = 0;

    if (argc > 2)
    {
	printf("Usage: cent [-s] [savefile]\n");
	exit(0);
    }
#ifndef SYS5
    signal(SIGTSTP,SIG_IGN);
#endif
    signal(SIGQUIT,catchint);
    signal(SIGINT,SIG_IGN);
    if (argc == 2 && !strcmp(argv[1],"-s"))
	showscores();
    dooptions();
    strcpy(name,getlogin());
#ifdef WIZARD
    author = !strcmp(name,"keith");
    if (author && getenv("CENTNAME") != NULL)
	strcpy(name,getenv("CENTNAME"));
#endif
    if (toohigh())
    {
	printf("Sorry, the load average is too high to play now.\n");
	exit(0);
    }
    ioctl(0,TIOCGETP,&origterm);
    initscr();
    if (LINES < 24 || COLS < 80)
    {
	printf("Screen size too small. Size must be at least 24 X 80.\n");
	exit(0);
    }
    rninit();
    if (argc > 1)
    {
	dorest(argv[1]);
	showla();
	gamestarted = rest = 1;
    }
    else
	instructions();
    signal(SIGINT,catchint);
#ifndef SYS5
    signal(SIGTSTP,catchstop);
#endif
    signal(SIGALRM,catchalarm);
    noecho();
    crmode();
    nonl();
    if (rest)
    {
	waitboard();
	if (dead)
	{
	    contdeath();
	    rest = 0;
	}
    }
    else
	make_screen();
    gamestarted = 1;
    nice(niceness);
    while (1)
    {
	fleafreq = 4 - ((board + 2) % 4);
	if (rest)
	    rest = 0;
	else
	{
	    make_cent((board - 1) % CENTLENGTH);
	    put_pede();
	    refresh();
	}
	movestuff();
	if (dead)
	   death();
    }
}

make_screen()
{
    register int i,y,x;

    clear();
    for (y = 0; y <= 22; y++)
	for (x = 1; x <= 55; x++)
	    mushw[y][x] = ' ';
    for (i = 0; i <= 22; i++)
    {
	mvaddch(i,0,'|');
	mushw[i][0] = '|';
	mvaddch(i,56,'|');
	mushw[i][56] = '|';
    }
    mvaddch(17,0,'-');
    mushw[17][0] = '-';
    mvaddch(17,56,'-');
    mushw[17][56] = '-';
    move(23,0);
    for (i = 0; i <= 56; i++)
    {
	addch('-');
	mushw[23][i] = '-';
    }
    nummushrooms = 0;
    for (i = 45 + rnd(15); i; i--)
    {
	do
	{
	    y = rnd(22);        /* not on bottom row */
	    x = rnd(55) + 1;
	}
	while (mvinch(y,x) != ' ');
	addshroom(y,x);
    }
    mvaddstr(0,60,"La:");
    showla();
    mvprintw(1,60,"Board: %d",board);
    mvprintw(2,60,"Score: %d",score);
    mvaddstr(20,60,"Next free man:");
    mvprintw(21,67,"%s",addcommas(nextman));
    displaymen();
    if (strcmp(name,getlogin()))
	mvaddstr(3,60,name);
    waitboard();
    extramen--;
    displaymen();
}

make_cent(num_free)
int num_free;
{
    register int i;
    int vel = 2 * rnd(2) - 1;
    register PEDE **piece = &centipede, *prev = NULL;

    for (i = 0; i < CENTLENGTH; i++)
    {
	*piece = (PEDE *)malloc(sizeof(PEDE));
	(*piece)->prev = prev;
	(*piece)->pos.y = 0;
	(*piece)->speed.y = 1;
	(*piece)->overlap = 0;
	(*piece)->poisoned = 0;
	(*piece)->speed.x = (i < CENTLENGTH - num_free) ? vel : 2 * rnd(2) - 1;
	(*piece)->type = (i == 0 || i >= CENTLENGTH - num_free) ? HEAD : BODY;
	prev = *piece;
	piece = &(*piece)->next;
    }
    lastpede = prev;
    *piece = NULL;
}

put_pede()
{
    register PEDE *piece = centipede;
    register int x;

    finished = breeding = scorpthisboard = 0;
    numpedes = CENTLENGTH;
    mvaddch(guy.y,guy.x,YOU);
    piece->pos.x = 22 + rnd(12);
    ADDPIECE(piece);
    while ((piece = piece->next) != NULL)
    {
	if (piece->type == BODY)
	    piece->pos.x = piece->prev->pos.x - piece->prev->speed.x;
	else
	{
	    while (mvinch(0,x = rnd(55) + 1) == HEAD || inch() == BODY);
	    piece->pos.x = x;
	}
	ADDPIECE(piece);
    }
}

death()
{
    static char *ouch[] = {
	"\\|/",
	"-*-",
	"/|\\"};

    mvaddch(guy.y,guy.x,'*');
    refresh();
    mvaddstr(guy.y,guy.x - 1,"-*-");
    refresh();
    printpic(ouch,3,guy.y - 1,guy.x - 1);
    refresh();
    countmushrooms();
    if (!extramen--)
	endgame();
    waitboard();
    contdeath();
}

contdeath()
{
    register PEDE *piece;
    register int y,x;

    if (fleahere)       /* erase flea */
    {
	ERASE(flea.y,flea.x);
	fleahere = 0;
    }
    if (fired)          /* erase shot */
    {
	ERASE(shot.y,shot.x);
	fired = 0;
    }
    if (scorphere)      /* erase scorpion */
    {
	erasepic(1,7,scorp.y,scorp.x);
	scorphere = 0;
    }
    spidcount = 0;
    if (spiderhere)     /* erase spider */
    {
	erasepic(2,6,spider.y,spider.x);
	spiderhere = 0;
    }
    for (piece = centipede; piece != NULL; piece = piece->next)
    {
	ERASE(piece->pos.y,piece->pos.x);
	free(piece);
    }
    for (y = guy.y - 1; y <= guy.y + 1; y++)
	for (x = guy.x - 1; x <= guy.x + 1; x++)
	    ERASE(y,x);
    guy.y = 22;
    guy.x = 28;
    displaymen();
    dead = 0;
}

countmushrooms()
{
    register int y,x,y2,x2,flag,i,j;
    char cu,cd,cl,cr,cm;        /* Characters on screen being overwritten */

    for (x = 1; x <= 55; x++)
	for (y = 22; y >= 0; y--)
	    if (mushw[y][x] != ' ' && mushw[y][x] != UNSHOTMUSHROOM)
	    {
		flag = mushw[y][x] != (cm = mvinch(y,x));
		mushw[y][x] = UNSHOTMUSHROOM;
		if (y > 0)
		{
		    cu = mvinch(y - 1,x);
		    mvaddch(y - 1,x,'|');
		}
		cd = mvinch(y + 1,x);
		mvaddch(y + 1,x,'|');
		cl = mvinch(y,x - 1);
		mvaddch(y,x - 1,'-');
		cr = mvinch(y,x + 1);
		mvaddch(y,x + 1,'-');
		refresh();
		if (y > 0)
		    mvaddch(y - 1,x,cu);
		mvaddch(y + 1,x,cd);
		mvaddch(y,x - 1,cl);
		mvaddch(y,x + 1,cr);
		mvaddch(y,x,((flag) ? cm : UNSHOTMUSHROOM));
		addscore(5);
		refresh();
	    }
}
