#include "cent.h"

dofire()                   /* move the guy's shot */
{
    register int i;
    register char thing;

    checkhit();            /* something walked into the shot */
    if (!fired)
	return;
    if (!COMPSPOTS(shot,guy))
	ERASE(shot.y,shot.x);
    for (i = 0; i < 4 && fired; i++)
    {
	if (!shot.y--)          /* Shot went off the screen */
	{
	    fired = 0;
	    return;
	}
	checkhit();
    }
    if (fired)
    {
       mvaddch(shot.y,shot.x,SHOT);
       refresh();
    }
}

checkhit()
{
    register char thing;

    if ((thing = mvinch(shot.y,shot.x)) != ' ' && inch() != SHOT
      && inch() != YOU)
    {                                             /* he hit something */
	fired = 0;
	mvaddch(shot.y,shot.x,SHOT);
	refresh();
	switch (thing)
	{
	    case UNSHOTMUSHROOM:
		mvaddch(shot.y,shot.x,ONCESHOTMUSHROOM);
		mushw[shot.y][shot.x] = ONCESHOTMUSHROOM;
		break;
	    case ONCESHOTMUSHROOM:
		mvaddch(shot.y,shot.x,TWICESHOTMUSHROOM);
		mushw[shot.y][shot.x] = TWICESHOTMUSHROOM;
		break;
	    case TWICESHOTMUSHROOM:
	    case TWICESHOTPOISON:
		mvaddch(shot.y,shot.x,' ');
		mushw[shot.y][shot.x] = ' ';
		addscore(1);
		if (shot.y >= 18)
		    nummushrooms--;
		break;
	    case UNSHOTPOISON:
		mvaddch(shot.y,shot.x,ONCESHOTPOISON);
		mushw[shot.y][shot.x] = ONCESHOTPOISON;
		break;
	    case ONCESHOTPOISON:
		mvaddch(shot.y,shot.x,TWICESHOTPOISON);
		mushw[shot.y][shot.x] = TWICESHOTPOISON;
		break;
	    default:
		if (getpede(shot.y,shot.x) != NULL)
		    shootpede();
		else if (fleahere && COMPSPOTS(shot,flea))
		    shootflea();
		else if (spiderhere &&
		  (shot.y == spider.y || shot.y == spider.y + 1)
		  && spider.x <= shot.x && shot.x <= spider.x + 5)
		    shootspider();
		else if (scorphere && shot.y == scorp.y &&
		  scorp.x <= shot.x && shot.x <= scorp.x + 6)
		{
		    scorphere = 0;
		    erasepic(1,7,scorp.y,scorp.x);
		    addscore(1000);
		}
		else
		{
		    mvprintw(15,60,"Unknown char: %c",thing);
		    ERASE(shot.y,shot.x);
		    refresh();
		}
	}
    }
}

shootpede()
{
    register PEDE *piece;

    piece = getpede(shot.y,shot.x);
    if (piece->type == HEAD)
	addscore(100);
    else
	addscore(10);
    if (piece != centipede)
	piece->prev->next = piece->next;
    else if ((centipede = centipede->next) == NULL)
    {
	finished = 1;
	breeding = 0;
	move(1,67);
	clrtoeol();
	printw("%s",addcommas(++board));
    }
    if (piece->next != NULL)
    {
	PEDE *pp = piece->next;

	pp->type = HEAD;
	pp->prev = piece->prev;
	if (piece->poisoned)
	    do
		pp->poisoned = WASPOISONED;
	    while ((pp = pp->next) != NULL && pp->type != HEAD);
    }
    else
	lastpede = piece->prev;
    addshroom(shot.y,shot.x);
    free(piece);
    numpedes--;
}

shootflea()
{
    if (!fleashot)
    {
	fleashot = 1;
	mvaddch(flea.y,flea.x,FLEA);
	return;
    }
    fleahere = 0;
    ERASE(flea.y,flea.x);
    addscore(200);
    startflea();
}

shootspider()
{
    spiderhere = 0;
    erasepic(2,6,spider.y,spider.x);
    spidcount = 0;
    if (guy.y - spider.y == 2)
	addscore(900);
    else if (guy.y - spider.y < 4)
	addscore(600);
    else
	addscore(300);
}
