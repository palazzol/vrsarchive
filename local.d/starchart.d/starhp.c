/*
** Hewlett-Packard 2623 (and equivalent) graphics terminal
** driver for starchart.
** Low-level routines (starting with 'HP') written and copyrighted by
** Jyrki Yli-Nokari (jty@intrin.FI),
** Petri Launiainen (pl@intrin.FI),
** Intrinsic, Ltd.,  FINLAND.
**
** You may use this code as you wish if credit is given and this message
** is retained.
*/

/*
** Star scaling is changed to happen only for the placement,
** therefore the stars look always nice
*/

#include <stdio.h>
#include "starchart.h"

#define SCALEU	1
#define SCALEL	2

/*
** Chart parameters (limiting magnitude and window x,y,w,h)
*/

mapblock thumbnail =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			3.2, 1.0, 420, 35, 480, 195, 0.0 };

mapblock master =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 20, 265, 880, 500, 0.0 };

/*
** Generic functions
*/

vecopen ()
{
	HPplot ();
	HPclear ();
}

vecclose ()
{
	fflush (stdout);
}

vecsize (points)
int points;
{
}

vecmove (x, y)
int x,y;
{
	x = SCALEU*x/SCALEL; /* adjust to screen size */
	y = SCALEU*y/SCALEL;

	HPmove (x, y);
}

vecdraw (x, y)
int x,y;
{
	x = SCALEU*x/SCALEL; /* adjust to screen size */
	y = SCALEU*y/SCALEL;

	HPdraw (x, y);
}

vecdrawdot(x, y)
    {
    vecdraw(x, y);	/* solid and dotted currently the same */
    }

vecdrawhyph(x, y)
    {
    vecdraw(x, y);	/* solid and dashed currently the same */
    }
    
xvecsym (x, y, s)
int x,y;
char s;
{
	y -= 11; /* center character strings */
	x = SCALEU*x/SCALEL; /* adjust to screen size */
	y = SCALEU*y/SCALEL;

	HPmove (x, y-11); /* center character strings */
	HPalpha_on ();
	putchar (s);
	HPalpha_off ();
}

vecsyms (x, y, s)
int x,y;
char *s;
{
	y -= 11; /* center character strings */
	x = SCALEU*x/SCALEL; /* adjust to screen size */
	y = SCALEU*y/SCALEL;

	HPmove (x, y);
	HPalpha_on ();
	fputs (s, stdout);
	HPalpha_off ();
}

vecmovedraw (x1, y1, x2, y2)
int x1, x2, y1, y2;
{
	x1 = SCALEU*x1/SCALEL; /* adjust to screen size */
	y1 = SCALEU*y1/SCALEL;

	x2 = SCALEU*x2/SCALEL; /* adjust to screen size */
	y2 = SCALEU*y2/SCALEL;

	HPmove (x1, y1);
	HPdraw (x2, y2);
}

drawlen (x, y, dx, dy, len)
int x, y, dx, dy, len;
{
	x = SCALEU*x/SCALEL; /* adjust to screen size */
	y = SCALEU*y/SCALEL;

	HPmove (x + dx, y + dy);
	HPdraw (x + dx + len - 1, y+dy);
}

/*
** Low Level HP Plotting Routines
*/

HPplot ()	/* switch to plot mode */
{
	fputs ("\033*dcl0,0P\033&k1O\033*m1g", stdout);
/*
**	1. Enable graphics display.
**	2. Disable cursor.
**	3. Locate cursor to origin.
**	4. Enable Graphics pad.
**	5. Solid area fill
*/
}

HPalpha_on ()	/* switch to alpha mode */
{
	fputs ("\033*dS", stdout);
}

HPalpha_off ()	/* switch back to plot mode */
{
	fputs ("\033*dT", stdout);
}

HPclear ()
{
	fputs ("\033*dA\033*m2a1B", stdout);
/*
**	1. Clear graphics screen.
**	2. Draw in "set pixel" mode.
**	3. Select solid line.
*/
	fflush (stdout);
}

HPmove (x, y)	/* move to (x,y) */
int x, y;
{
	printf ("\033*pa%d,%dZ", x, y);
}

HPdraw (x, y)	/* draw to (x,y) */
int x, y;
{
	printf ("\033*pb%d,%dZ", x, y);
}

vecsymsgk(str, x, y)
    char *str;
    {
    vecsyms(str, x, y);
    }
