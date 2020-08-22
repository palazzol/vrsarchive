/*
 * IRIS driver for startchart.c mainline
 *
 * Tim Pointing, DCIEM
 * <decvax!utzoo!dciem!tim>
 */

#include <stdio.h>
#include <gl.h>
#include "starchart.h"

/*
 * Chart parameters (limiting magnitude and window x,y,w,h)
 */


mapblock master =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			 8.0, 3.0, 2.05, 20, 265, 880, 500, 0.0 };

mapblock bigmaster =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			 8.0, 3.0, 2.05, 20,  65, 880, 700, 0.0 };

mapblock thumbnail =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			 3.2, 1.0, 2.05, 420, 35, 480, 195, 0.0 };
/*
 * Generic Star Drawing Stuff
 */

vecopen()
   {
   keepaspect(1,1);
   getport();
   ortho(0., 1023., 0., 1023.);
   reshapeviewport();
   color(BLACK);
   clear();
   makeobj(1);
   color(BLACK);
   clear();
   color(WHITE);
   }

vecclose()
    {
    int x;
    closeobj(1);
    callobj(1);
    while(qread(&x))
	{
	ortho(0., 1023., 0., 1023.);
	reshapeviewport();
	callobj(1);
	}
    }

vecsize(points)
    int points;
    {
    }

vecmove(x, y)
    {
    move2i(x, y);
    }

vecdraw(x, y)
    {
    draw2i(x, y);
    }

vecdrawhyph(x, y)
    {
    draw2i(x, y);
    }

vecdrawdot(x, y)
    {
    draw2i(x, y);
    }

vecsyms(x, y, s)
    char *s;
    {
    color(WHITE);
    cmov2i(x, y-5); /* center character strings */
    charstr(s);
    color(WHITE);
    }

vecsymsgk(x, y, s)
    char *s;
    {
    vecsyms(x, y, s);
    }

vecmovedraw(x1, y1, x2, y2)
    {
    move2i(x1, y1);
    draw2i(x2, y2);
    }

drawlen(x, y, dx, dy, len)
    {
    vecmovedraw(x+dx, y+dy, x+dx+len, y+dy);
    }
/*
 * presently in conflict with stariamges.c
 */
 
#ifdef FIXED

drawPlan(x, y, mag, type, col)
    char type, *col;
    {
    color(YELLOW);
    switch(type)
	{
	case ('S'<<8)|('o'):	/* Sol */
		pnt2i(x, y);
		circi(x, y, 5);
		break;
	case ('M'<<8)|('e'):	/* Mercury */
		circi(x, y+1, 3);
		move2i(x, y-2);
		draw2i(x, y-5);
		move2i(x-2, y-4);
		draw2i(x+2, y-4);
		move2i(x-1, y+3);
		draw2i(x-2, y+5);
		move2i(x+1, y+3);
		draw2i(x+2, y+5);
		break;
	case ('V'<<8)|('e'):	/* Venus */
		circi(x, y+1, 3);
		move2i(x, y-2);
		draw2i(x, y-5);
		move2i(x-2, y-4);
		draw2i(x+2, y-4);
		break;
	case ('M'<<8)|('a'):	/* Mars */
		circi(x-2, y-2, 3);
		move2i(x, y);
		draw2i(x+3, y+3);
		draw2i(x+1, y+3);
		move2i(x+3, y+3);
		draw2i(x+3, y+1);
		break;
	case ('J'<<8)|('u'):	/* Jupiter */
		arci(x-2, y+2, 2, 3150, 1350);
		move2(x-.6, y+.6);
		draw2i(x-3, y-1);
		draw2i(x+1, y-1);
		move2i(x, y-2);
		draw2i(x, y+4);
		break;
	case ('S'<<8)|('a'):	/* Saturn */
		move2i(x-2, y+3);
		draw2i(x, y+3);
		move2i(x-1, y+3);
		draw2i(x-1, y-3);
		move2i(x-1, y+1);
		draw2i(x, y+1);
		draw2i(x+2, y);
		draw2i(x+2, y-1);
		draw2i(x+1, y-2);
		draw2i(x+1,y-3);
		break;
	case ('U'<<8)|('r'):	/* Uranus */
		circi(x, y-3, 3);
		pnt2i(x, y-3);
		move2i(x, y);
		draw2i(x, y+3);
		draw2i(x-1, y+2);
		move2i(x, y+3);
		draw2i(x+1, y+2);
		break;
	case ('N'<<8)|('e'):	/* Neptune */
		arci(x, y+3, 3, 1800, 3600);
		move2i(x, y+3);
		draw2i(x, y-6);
		move2i(x-2, y-5);
		draw2i(x+2, y-5);
		break;
	default:
		circi(x, y, 4);
		move2i(x, y-5);
		draw2i(x, y+5);
		move2i(x-5, y);
		draw2i(x+5, y);
		break;
	}
    color(WHITE);
    }

drawStar(x, y, mag, type, col)
char type, *col;
    {
    if (mag > 9) mag = 9;
    if (mag < 4)
	{
	color(WHITE);
	circfi(x, y, (int)((4-mag)/2));
	}
    else
	{
	color(31 - (mag-4)*3);
	pnt2i(x,y);
	}
    color(WHITE);
    }

drawNebu(x, y, mag, type, col)
    char type, *col;
    {
    }

drawGalx(x, y, mag, type, col)
    char type, *col;
    {
    }

drawClus(x, y, mag, type, col)
    char type, *col;
    {
    }

#endif
