/*
 * Plotter Display driver for starchart.c mainline (UNIX Plot(5) style output)
 *
 * Sjoerd Mullender <sjoerd@cs.vu.nl>
 * Free University, Amsterdam
 */

#include <stdio.h>
#include <ctype.h>
#include "starchart.h"

#define SOLID		1
#define DOTTED		2
#define DASHED		3

static int mode = 0;

/*
 * Chart parameters (limiting magnitude and window x,y,w,h)
 */

mapblock thumbnail =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			3.2, 1.0, 2.05, 420, 35, 480, 195, 0.0 };

mapblock master =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 2.05, 20, 265, 880, 500, 0.0 };

mapblock bigmaster=	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 2.05, 20,  65, 880, 700, 0.0 };

vecopen()
    {
    openpl();
    space(0, 0, 1024, 768);
    erase();
    }

vecclose()
    {
    closepl();
    }

vecsize(points)
    int points;
    {
    }

vecmove(x, y)
    {
    move(x, y);
    }

vecdrawdot(x, y)
    {
    if (mode != DOTTED) linemod("dotted");
    mode = DOTTED;
    cont(x, y);
    }

vecdrawhyph(x, y)
    {
    if (mode != DASHED) linemod("shortdashed");
    mode = DASHED;
    cont(x, y);
    }

vecdraw(x, y)
    {
    if (mode != SOLID) linemod("solid");
    mode = SOLID;
    cont(x, y);
    }

vecsyms(x, y, s)
    char *s;
    {
    move(x, y);
    label(s);
    }

vecmovedraw(x1, y1, x2, y2)
    {
    vecmove(x1, y1);
    vecdraw(x2, y2);
    }

vecsymsgk(x, y, str)
    char *str;
    {
    vecsyms(x, y, str);
    }

drawlen(x, y, dx, dy, len)
    {
    move(x + dx, y + dy);
    cont(x + dx + len - 1, y + dy);
    }
