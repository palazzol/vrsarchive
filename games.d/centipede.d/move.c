#include "cent.h"

movestuff()
{
    register int y,x;
    int count;

    while (!dead && finished < 40)
    {
	if (spidcount % 20 == 0)
	    showla();
	if (inter)
	    quit();
	if (stopped)
	    stopawhile();
	if (gameover)
	    ladie();
	if (finished)
	{
	    putnull(24);
	    finished++;
	}
	else
	{
	    if (numpedes < 6 && !spiderhere)
		putnull(4 * (6 - numpedes));
	    movepedes();
	    if (breeding)
		dobreed();
	}
	if (fleahere)
	    moveflea();
	if (board >= 2 && !fleahere && !scorphere &&
	  (rnd(3400) < (board - 1) * fleafreq ||
	  nummushrooms < 5 && rnd(600) < (board - 1) * fleafreq))
	    startflea();
	if (scorphere)
	    movescorp();
	if (board >= 3 && !scorphere && !fleahere &&
	  (!scorpthisboard  && rnd(2000) < board - 1 ||
	  scorpthisboard && rnd(4000) < board - 1))
	    startscorp();
	if (spiderhere)
	    movespider();
	spidcount++;
	if (!spiderhere && spidcount % 90 == 0 && rnd (7) < 6)
	    startspider();
	if (fired)
	    dofire();
	getyx(stdscr,y,x);                      /* make sure he's not */
	if (mvinch(guy.y,guy.x) != YOU)         /* invisible, but don't */
	    addch(YOU);                         /* waste cursor movement */
	move(y,x);
	refresh();
	fflush(stdout);
	do
	    ioctl(1,TIOCOUTQ,&count);
	while (count > spiderhere * 40);
	move_guy();
    }
}

movepedes()
{
    register PEDE *piece;
    register int x;
    register char thing;

    for (piece = lastpede; piece != NULL; piece = piece->prev)
    {                                     /* move the 'pedes (rev order) */
	piece->oldpos.x = piece->pos.x;
	piece->oldpos.y = piece->pos.y;
	if (piece->type == BODY && (piece == lastpede ||
	  piece->next->type == HEAD) && piece->pos.y == 22 &&
	  piece->prev->pos.y == 21 && piece->poisoned == UNPOISONED)
	{
	    piece->type = HEAD;
	    piece->speed.x = -piece->speed.x;
	    if (1 <= piece->pos.x + piece->speed.x &&
	      piece->pos.x + piece->speed.x <= 55)
	    {
		piece->pos.x += piece->speed.x;
		if (COMPSPOTS(piece->pos,guy))
		    dead = 1;
	    }
	}
	if (piece->type == BODY)
	{
	    piece->pos.x = piece->prev->pos.x;
	    piece->pos.y = piece->prev->pos.y;
	    piece->speed.x = piece->prev->speed.x;
	    piece->speed.y = piece->prev->speed.y;
	    if (piece->poisoned == WASPOISONED &&
	      piece->prev->poisoned == UNPOISONED)
		piece->poisoned = UNPOISONED;
	}
	else
	{
	    x = piece->pos.x + piece->speed.x;
	    if (x < 1 || x > 55 || (thing = mvinch(piece->pos.y,x)) != ' ' &&
	      thing != YOU && thing != SHOT || piece->overlap ||
	      piece->poisoned != UNPOISONED)
	    {
		if (1 <= x && x <= 55 && (thing == UNSHOTPOISON ||
		  thing == ONCESHOTPOISON || thing == TWICESHOTPOISON))
		    poison(piece);
		piece->speed.x = -piece->speed.x;
		if (piece->pos.y == 22 || piece->pos.y == 18
		  && piece->speed.y == -1)
		    piece->speed.y = -piece->speed.y;
		piece->pos.y += piece->speed.y;
		piece->overlap = 0;
		if (piece->poisoned == WASPOISONED)
		    piece->poisoned = UNPOISONED;
	    }
	    else
		piece->pos.x = x;
	    if (piece->pos.y == 22 && piece->poisoned == UNPOISONED
	      && !breeding)
	    {
		breeding = 1;
		if (breedtime > 25)
		    breedtime--;
	    }
	}
	if (COMPSPOTS(piece->pos,guy))
	    dead = 1;
	if (piece->poisoned == POISONED && piece->oldpos.y == 22)
	    piece->poisoned = UNPOISONED;
    }
    for (piece = centipede; piece != NULL; piece = piece->next)
	ERASE(piece->oldpos.y,piece->oldpos.x);
    for (piece = centipede; piece != NULL; piece = piece->next)
    {
	if (mvinch(piece->pos.y,piece->pos.x) == HEAD && piece->type == HEAD)
	    piece->overlap = 1;
	if ((thing = mushw[piece->pos.y][piece->pos.x]) == UNSHOTPOISON
	  || thing == ONCESHOTPOISON || thing == TWICESHOTPOISON)
	    poison(piece);
	ADDPIECE(piece);
    }
}

dobreed()                      /* bring on the reinforcements! */
{
    if (++moves >= breedtime && rnd(10) < 9)
    {
	if (breedtime > 125)
	    breedtime -= 10;
	else if (breedtime > 55)
	    breedtime -= 5;
	if (breedtime > 25)
	    breedtime--;
	moves = 0;
	lastpede->next = (PEDE *)malloc(sizeof(PEDE));
	lastpede->next->prev = lastpede;
	lastpede = lastpede->next;
	lastpede->next = NULL;
	lastpede->overlap = 0;
	lastpede->poisoned = UNPOISONED;
	lastpede->type = HEAD;
	lastpede->pos.y = 18;
	lastpede->speed.y = 1;
	if (rnd(2) == 0)
	{
	    lastpede->pos.x = 1;
	    lastpede->speed.x = 1;
	}
	else
	{
	    lastpede->pos.x = 55;
	    lastpede->speed.x = -1;
	}
	ADDPIECE(lastpede);
	numpedes++;
	if (COMPSPOTS(lastpede->pos,guy))
	    dead = 1;
    }
}

startflea()
{
    fleahere = 1;
    fleashot = 0;
    flea.y = 0;
    flea.x = rnd(55) + 1;
    mvaddch(flea.y,flea.x,FLEA);
}

moveflea()                      /* move a flea */
{
    ERASE(flea.y,flea.x);
    if (flea.y == 22)
	fleahere = 0;
    else
    {
	if (mushw[flea.y][flea.x] == ' ' && rnd(5) < 2)
	    addshroom(flea.y,flea.x);
	flea.y++;
	mvaddch(flea.y,flea.x,FLEA);
	if (COMPSPOTS(flea,guy))
	    dead = 1;

    }
}

startscorp()        /* start a scorpion */
{
    if ((scorpvel = rnd(6) - 2) < 1)
	scorpvel--;
    scorppic = (scorpvel > 0) ? rscorpion : lscorpion;
    scorp.x = (scorpvel > 0) ? -5 : 55;
    scorp.y = rnd(12) + 2;
    scorpthisboard = 1;
    scorphere = 1;
    drawpic(&scorppic,1,7,scorp.y,scorp.x);
}

movescorp()                     /* move a scorpion */
{
    register int dir,i;

    if (scorpvel > 0)
    {
	dir = 1;
	i = scorpvel;
    }
    else
    {
	dir = -1;
	i = -scorpvel;
    }
    erasepic(1,7,scorp.y,scorp.x);
    while (i-- && scorphere)
    {
	if (1 <= scorp.x && scorp.x <= 55)
	    switch (mushw[scorp.y][scorp.x])        /* poison a mushroom */
	    {
		case UNSHOTMUSHROOM:
		    mvaddch(scorp.y,scorp.x,UNSHOTPOISON);
		    mushw[scorp.y][scorp.x] = UNSHOTPOISON;
		    break;
		case ONCESHOTMUSHROOM:
		    mvaddch(scorp.y,scorp.x,ONCESHOTPOISON);
		    mushw[scorp.y][scorp.x] = ONCESHOTPOISON;
		    break;
		case TWICESHOTMUSHROOM:
		    mvaddch(scorp.y,scorp.x,TWICESHOTPOISON);
		    mushw[scorp.y][scorp.x] = TWICESHOTPOISON;
		    break;
	    }
	scorp.x += dir;
	if (scorp.x < -5 || scorp.x > 55)
	    scorphere = 0;
    }
    if (scorphere)
	drawpic(&scorppic,1,7,scorp.y,scorp.x);
}

poison(piece)
PEDE *piece;
{
    do
	piece->poisoned = POISONED;
    while ((piece = piece->next) != NULL && piece->type != HEAD);
}

putnull(n)
int n;
{
    while (n--)
	putchar(0);
}

startspider()
{
    spiderhere = 1;
    spiderdir = 2 * rnd(2) - 1;
    spider.y = 14;
    spidervel.y = 1;
    if (spiderdir > 0)
    {
	spider.x = -4;
	spidervel.x = 1;
    }
    else
    {
	spider.x = 55;
	spidervel.x = -1;
    }
    drawpic(spiderpic,2,6,spider.y,spider.x);
}

movespider()
{
    register int y,x,count = 0,dx,dy;

    erasepic(2,6,spider.y,spider.x);
    while (count++ < 10)
    {
	y = spider.y + spidervel.y;
	x = spider.x + spidervel.x;
	if (!spidcango(y,x))
	{
	    spidervel.y = -spidervel.y;
	    if (rnd(4) < 1)
		spidervel.x = spiderdir - spidervel.x;
	    continue;
	}
	else
	{
	    dy = spider.y + 1 - guy.y;
	    dx = spider.x - guy.x;
	    if (dx / spiderdir < 0 && dy && 0 <= (dx + dy) * spiderdir &&
		(dx + dy) * spiderdir <= 5 && rnd(3) < 2)
	    {
		spidervel.x = spiderdir;
		spidervel.y = (dy < 0) ? 1 : -1;
		continue;
	    }
	    if (dx / spiderdir < 0 && rnd(6) < 1)
	    {
		spidervel.x = spiderdir;
		continue;
	    }
	    else if (spidervel.x && rnd(8) < 1 || !spidervel.x && rnd(12) < 1)
	    {
		spidervel.x = spiderdir - spidervel.x;
		continue;
	    }
	    else if (rnd(12) < 1)
	    {
		spidervel.y = -spidervel.y;
		continue;
	    }
	}
	break;
    }
    if (count != 11)
    {
	spider.y = y;
	spider.x = x;
    }
    else
    {
	if (spidcango(spider.y,spider.x + spiderdir) && rnd(12) < 11)
	    spider.x += spiderdir;
	x = spider.x;
	y = spider.y;
    }
    if (x < -4 || x > 55)
	spiderhere = spidcount = 0;
    else
    {
	int xx;
     
	for (xx = x + 2; xx <= x + 3; xx++)
	    if (1 <= xx && xx <= 55)
		if (mushw[y + 1][xx] != ' ')
		{
		    mushw[y + 1][xx] = ' ';
		    if (y >= 17)
			nummushrooms--;
		 }
	drawpic(spiderpic,2,6,spider.y,spider.x);
	if ((guy.y == spider.y || guy.y == spider.y + 1) &&
	  spider.x <= guy.x && guy.x <= spider.x + 5)
	    dead = 1;
    }
}

spidcango(y,x)
int y,x;
{
    int yy,xx;

    if (y == 13 || y ==22)
	return(0);
    for (yy = y; yy < y + 2; yy++)
	for (xx = x; xx < x + 6; xx++)
	    if (1 <= xx && xx <= 55 && (mvinch(yy,xx) == HEAD ||
	      inch() == BODY))
		return(0);
    return(1);
}
