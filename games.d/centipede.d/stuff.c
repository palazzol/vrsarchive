#include "cent.h"

extern char *def_pager, *def_pager_opts;

rnd(n)
register int n;
{
    double rn();
    register int i;

    i = n * rn();
    return(i);
}

printpic(pic,len,y,x)
char **pic;
int len, y, x;
{
    register int i;
    for (i = 0; i < len; i++)
	mvaddstr(i + y,x,pic[i]);
}

PEDE *getpede(y,x)
{
    register PEDE *pede;

    for (pede = centipede; pede != NULL; pede = pede->next)
	if (pede->pos.y == y && pede->pos.x == x)
	    return(pede);
    return(NULL);
}

addscore(n)
{
    score += n;
    mvprintw(2,67,"%s",addcommas(score));
    if (score >= nextman)
    {
	mvprintw(21,67,"%s",addcommas(nextman += FREEMAN));
	extramen++;
	displaymen();
	mvaddstr(12,66,"You've won");
	mvaddstr(13,65,"a free man!!");
	refresh();
	sleep(2);
	move(12,66);
	clrtoeol();
	move(13,65);
	clrtoeol();
    }
}

displaymen()
{
    static char men[15];
    register int i;

    for (i = 0; i < 6 && i < extramen; i++)
	men[i] = YOU;
    if (extramen > 6)
	sprintf(men + 6," (%s)",addcommas(extramen));
    else
	men[i] = 0;
    move(23,60);
    clrtoeol();
    addstr(men);
}

char *addcommas(num)
int num;
{
    static char str[14];
    register char *spot = str;
    register int p10;

    for (p10 = 1000; num / p10 > 0; p10 *= 1000);
    p10 /= 1000;
    sprintf(spot,"%d",num / p10);
    while (p10 > 1)
    {
	num -= (num / p10) * p10;
	p10 /= 1000;
	spot += strlen(spot);
	*spot++ = ',';
	sprintf(spot,"%03d",num / p10);
    }
    return(str);
}

drawpic(pic,len,wid,y,x)
char **pic;
int len,wid,y,x;
{
   register int yy,xx,start,end,pos;
	   
   start = (x < 1) ? 1 - x : 0;
   end = (x > 56 - wid) ? 56 - x : wid;
   for (yy = 0; yy < len; yy++)
       for (pos = start, xx = (x < 1) ? 1 : x; pos < end; pos ++, xx++)
	   if (pic[yy][pos] != ' ')
	       mvaddch(y + yy,xx,pic[yy][pos]);
}

erasepic(len,wid,y,x)
int len,wid,y,x;
{
    register int xx,yy,twid,newx;
     
    twid = ((x > 56 - wid) ? 56 - x : wid) - ((x < 1) ? 1 - x : 0);
    newx = (x < 1) ? 1 : x;
    for (yy = 0; yy < len; yy++)
	for (xx = newx; xx < newx + twid; xx++)
	    ERASE(y + yy,xx);
}

showla()
{
    register double la[3];
    static int ol = -1,exceeded = 0;
    register int l;

    loadav(la);
    l = la[1] * 100;
    if (l != ol)
    {
	ol = l;
	mvprintw(0,67,"%.2f",l / 100.0);
    }
#ifdef WIZARD
    if (l > 100 * maxload && !author)
#else
    if (l > 100 * maxload)
#endif
    {
	if (!exceeded)
	{
	    mvaddstr(10,60,"Load av exceeded.");
	    mvaddstr(11,60,"You have 2 minutes.");
	    exceeded = 1;
	    alarm(120);
	}
    }
    else if (exceeded)
    {
	alarm(0);
	mvaddstr(10,60,"The load av dropped.");
	mvaddstr(11,60,"You have a repreive.");
	exceeded = 0;
    }
}

toohigh()
{
    double la[3];

    loadav(la);
#ifdef WIZARD
    return(la[1] > maxload && !author);
#else
    return(la[1] > maxload);
#endif
}

addshroom(y,x)
int y,x;
{
    if (y == 22)
    {
	mvaddstr(15,60,"Mushroom on last row");
	return;
    }
    mvaddch(y,x,UNSHOTMUSHROOM);
    mushw[y][x] = UNSHOTMUSHROOM;
    if (y >= 18)
	nummushrooms++;
}

#ifdef WIZARD
setname()
{
    int i = 0;
    char ch;

    if (!author)
	return;
    mvaddstr(15,60,"New name(8 chars):");
    move(16,60);
    refresh();
    while ((ch = getch()) != '\n' && i < 9)
    {
	if (ch == '\010' || ch == '\177')
	{
	    if (i > 0)
	    {
		i--;
		printw("\010 \010");
	    }
	}
	else
	    addch(name[i++] = ch);
	refresh();
    }
    name[(i < 8) ? i : 8] = 0;
    move(16,60);
    clrtoeol();
    move(15,60);
    clrtoeol();
    addstr("Ok");
    move(3,60);
    clrtoeol();
    addstr(name);
    refresh();
}
#endif

instructions()
{
    char ch, cmd[90];

    printf("Welcome to Centipede version %.1f!\n",version);
    printf("Would you like instructions(y/n)?");
    crmode();
    if ((ch = getchar()) != 'y' && ch != 'Y')
	return;
    if (getenv("PAGER") == NULL)
	sprintf(cmd,"%s %s %s", def_pager, def_pager_opts, helpfile);
    else
	sprintf(cmd,"%s %s", getenv("PAGER"), helpfile);
    system(cmd);
    printf("[Hit return to start the game]");
    while (getchar() != '\n');
}
