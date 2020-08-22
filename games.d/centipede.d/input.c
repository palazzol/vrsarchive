#include "cent.h"
#include "sys_dep.h"

#ifdef SYS5
#    undef getchar
#    define getchar cget
#endif

bool cready();

move_guy()
{
    register int y, x, changed = 0;
    int count,repeat = 0;
    char ch;

#ifndef SYS5
    ioctl(0,FIONREAD,&count);
#else
    if (cready()) count = 1;
    else count = 0;
#endif
    while (repeat || count--)
    {
	if (repeat)
	    repeat--;
	else
	    ch = in_map[getchar()];
	if (dead)
	   continue;
	y = guy.y;
	x = guy.x;
	switch(ch)
	{
	    case '\014':		/* KDW */
		redrawscr(); refresh();
		break;
	    case LEFT:
	        x--;
		break;
	    case RIGHT:
		x++;
		break;
	    case UPWARD:
		y--;
		break;
	    case DOWN:
		y++;
		break;
	    case UPRIGHT:
		x++;
		y--;
		break;
	    case UPLEFT:
		x--;
		y--;
		break;
	    case DOWNRIGHT:
		x++;
		y++;
		break;
	    case DOWNLEFT:
		x--;
		y++;
		break;
	    case FIRE:
		if (!fired)
		{
		    fired = 1;
		    shot.y = guy.y - 1;
		    shot.x = guy.x;
		    checkhit();
		    changed = 1;
		    if (fired)
			mvaddch(shot.y,shot.x,SHOT);
		}
		continue;
	    case PAUSEKEY:
		waitboard();
		continue;
	    case FASTLEFT:
		repeat = 8;
		ch = LEFT;
		continue;
	    case FASTRIGHT:
		repeat = 8;
		ch = RIGHT;
		continue;
	    default:
		continue;
	}

	if (ch == UPRIGHT || ch == UPLEFT || ch == DOWNRIGHT || ch == DOWNLEFT)
	{
	    if (y < 18)
		y = 18;
	    else if (y > 22)
		y = 22;
	    if (x < 1)
		x = 1;
	    else if (x > 55)
		x = 55;
	    if (y == guy.y && x == guy.x)
		continue;
	}
	if (x >= 1 && x <= 55 && y >= 18 && y <= 22)
	{
	    if (getpede(y,x) != NULL || fleahere && y == flea.y && x == flea.x
	      || spiderhere && (y == spider.y || y == spider.y + 1)
	      && spider.x <= x && x <= spider.x + 5)
	    {
		dead = 1;
		mvaddch(y,x,YOU);
		ERASE(guy.y,guy.x);
		guy.y = y;
		guy.x = x;
		changed = 1;
	    }
	    else if (mvinch(y,x) == ' ')
	    {
		addch(YOU);
		ERASE(guy.y,guy.x);
		guy.y = y;
		guy.x = x;
		changed = 1;
	    }
	}
    }
    if (changed) {
	refresh();
	raw();
	noraw();
	crmode();
    }
}

/*
 * KDW: setblock, cready and cget from Rochkind.
 */

#define EMPTY '\0'

static char cbuf = EMPTY;

setblock (fd, on)
    int fd;
    bool on;
{
    static int blockf, nonblockf;
    static bool first = TRUE;
    int flags;

    if (first)
    {
	first = FALSE;
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
	    perror("fcntl");
	blockf = flags & ~O_NDELAY;
	nonblockf = flags | O_NDELAY;
    }
    if (fcntl(fd, F_SETFL, on ? blockf : nonblockf) == -1)
	perror("fcntl2");
}

bool cready ()
{
    if (cbuf != EMPTY)
	return TRUE;
    setblock(0, FALSE);
    switch (read(0, &cbuf, 1))
    {
    case -1: perror("read"); break;
    case 0:  return FALSE;
    default: return TRUE;
    }
}

int cget ()
{
    char c;

    if (cbuf != EMPTY)
    {
	c = cbuf;
	cbuf = EMPTY;
	return c & 0377;
    }
    setblock(0, TRUE);
    switch (read(0, &c, 1))
    {
    case -1: perror("read"); break;
    case 0:  return -1;
    default: return c & 0377;
    }
}
