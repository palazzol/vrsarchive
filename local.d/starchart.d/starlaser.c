/*
** Hewlett-Packard Laserjet (2686a) laser printer driver
** for starchart.
** Low-level routines (starting with 'HPLJET') written and copyrighted by
** Jyrki Yli-Nokari (jty@intrin.FI),
** Petri Launiainen (pl@intrin.FI),
** Intrinsic, Ltd.,  FINLAND.
**
** You may use this code as you wish if credit is given and this message
** is retained.
*/

/*
 ! patched December, 1987 by Alan Paeth (awpaeth@watcgl),
 !
 ! [1] "bigmaster" chart layout now added
 ! [2] different XOFF and YOFF values adopted for better margins
 !
 */

/*
** This code is intended for ALL Laserjet family printers.
** Because the base version has only 59 k raster buffer, the
** stars are not completely round, but not too ugly either.
*/

#include <stdio.h>
#include "starchart.h"

char *calloc ();

#define SCALEU	29		/* graphics scaling */
#define SCALEL	40

#ifdef ROSS
#define TSCALEU	139		/* text mode scaling */
#define TSCALEL	20
#else
#define TSCALEU	69		/* text mode scaling */
#define TSCALEL	10
#endif

#ifdef ROSS
#define XOFF	460		/* text centering offset (in decipoints) */
#define YOFF	(-80)		/* previously, XOFF=1060; YOFF=(-80) */
#else
#define XOFF	520		/* text centering offset (in decipoints) */
#define YOFF	(-50)		/* previously, XOFF=1060; YOFF=(-80) */
#endif

#define HPLJETXMAX 743		/* Number of pixels in X-axis */
#define HPLJETYMAX 557		/* Number of pixels in Y-axis */

/*
** Chart parameters (limiting magnitude and window x,y,w,h)
*/

mapblock thumbnail =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			3.2, 1.0, 2.05, 420, 35, 480, 195, 0.0 };

mapblock master =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 2.05, 20, 265, 880, 500, 0.0 };

mapblock bigmaster =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 2.05, 20,  65, 880, 700, 0.0 };

/*
** Generic functions
*/

vecopen ()
{
	HPLJETorient ();
	r_makeraster (HPLJETXMAX, HPLJETYMAX);
}

vecclose ()
{
	HPLJETdump ();
}

vecsize (points)
int points;
{
/**/
}

vecmove (x, y)
int x,y;
{
	x = (x*SCALEU)/SCALEL; /* adjust to bitmap size */
	y = (y*SCALEU)/SCALEL;

	HPLJETmove (x, y);
}

vecdraw (x, y)
int x,y;
{
	x = (x*SCALEU)/SCALEL; /* adjust to bitmap size */
	y = (y*SCALEU)/SCALEL;

	HPLJETdraw (x, y);
}

vecdrawdot(x, y)
    {
    vecdraw(x, y); /* dotted and solid the same? (any HP folks out there?) */
    }

vecdrawhyph(x, y)
    {
    vecdraw(x, y); /* ditto */
    }

/*
** Text handling is a pain because of separate text/graphics mode
** and separated axises
*/
vecsym (x, y, s)
int x,y;
char s;
{
	y = 767 - y; /* change y axis on text output */
	y -= 5; /* center character strings */
	x = (x*TSCALEU)/TSCALEL + XOFF; /* adjust to bitmap size */
	y = (y*TSCALEU)/TSCALEL + YOFF;

#ifdef ROSS
	if( x < 0 )	x = 0;
	if( y < 0 )	y = 0;
#endif
	printf ("\033&a%dh%dV%c", x, y, s);
}

vecsyms (x, y, s)
int x,y;
char *s;
{
	y = 767 - y; /* change y axis on text output */
	y -= 5; /* center character strings */
	x = (x*TSCALEU)/TSCALEL + XOFF; /* adjust to bitmap size */
	y = (y*TSCALEU)/TSCALEL + YOFF;

#ifdef ROSS
	if( x < 0 )	x = 0;
	if( y < 0 )	y = 0;
#endif
	printf ("\033&a%dh%dV%s", x, y, s);
}

vecmovedraw (x1, y1, x2, y2)
int x1, x2, y1, y2;
{
	x1 = (x1*SCALEU)/SCALEL; /* adjust to bitmap size */
	y1 = (y1*SCALEU)/SCALEL;

	x2 = (x2*SCALEU)/SCALEL; /* adjust to bitmap size */
	y2 = (y2*SCALEU)/SCALEL;

	HPLJETmove (x1, y1);
	HPLJETdraw (x2, y2);
}

drawlen (x, y, dx, dy, len)
int x, y, dx, dy, len;
{
	x = (x*SCALEU)/SCALEL; /* adjust to bitmap size */
	y = (y*SCALEU)/SCALEL;

	HPLJETmove (x + dx, y + dy);
	HPLJETdraw (x + dx + len - 1, y+dy);
}

/*
** Low Level Laserjet Plotting Routines
*/

HPLJETorient ()	/* switch to portrait text orientation mode */
{
	printf ("\033&l1O");
}

HPLJETmove (x, y)	/* move to (x,y) */
int x, y;
{
	r_move (x, y);
}

HPLJETdraw (x, y)	/* draw to (x,y) */
int x, y;
{
	r_draw (x, y);
}

#define IN(i,size)	((unsigned)i < size)
typedef char ritem;
typedef ritem *raster[];

static raster *r_p;	/* global pointer to raster */
static int r_currx, r_curry;	/* the current coordinates */
static int r_xsize, r_ysize;	/* the size of the raster */

/*
** set pixel (x,y) to value val (zero or nonzero).
*/
void
r_setpixel(x, y, val)
int x, y;
ritem val;
{
	if (IN(x, r_xsize) && IN(y, r_ysize)) {
		*(((*r_p)[y]) + x) = val;
	}
}

/*
** get pixel (x,y) value (0 or 1)
*/
int
r_getpixel(x, y)
{
	if (IN(x, r_xsize) && IN(y, r_ysize)) {
		return *(((*r_p)[y]) + x);
	}
	else
		return (0);
}

/*
** allocate the raster
*/
r_makeraster(x, y)
{
	register int j;
	
	/* allocate row pointers */
	if ((r_p = (raster *)calloc(y, sizeof(ritem *))) == NULL) {
		perror("Raster buffer allocation failure");
		exit(1);
	}
	for (j = 0; j < y; j++) {
		if (((*r_p)[j] = (ritem *)calloc(x, sizeof(ritem))) == NULL) {
			perror("Raster buffer allocation failure");
			exit(1);
		}
	}
	r_xsize = x; r_ysize = y;
	r_currx = r_curry = 0;
}
	
/*
** plot a line from (x0,y0) to (x1,y1)
*/
r_plot(x0, y0, x1, y1)
int x0, y0, x1, y1;
{
	int e, hx, hy, dx, dy, i;
	/*
	** We use Bresenham's alorithm for plotting
	** (IBM system journal 4(1):25-30, 1965)
	*/
	hx = abs(x1 - x0);
	hy = abs(y1 - y0);
	dx = (x1 > x0) ? 1 : -1;
	dy = (y1 > y0) ? 1 : -1;
	
	if (hx > hy) {
		/*
		** loop over x-axis
		*/
		e = hy + hy - hx;
		for (i = 0; i <= hx; i++) {
			r_setpixel(x0, y0, 1);
			if (e > 0) {
				y0 += dy;
				e += hy + hy - hx - hx;
			} else {
				e += hy + hy;
			}
			x0 += dx;
		}
	} else {
		/*
		** loop over y-axis
		*/
		e = hx + hx - hy;
		for (i = 0; i <= hy; i++) {
			r_setpixel(x0, y0, 1);
			if (e > 0) {
				x0 += dx;
				e += hx + hx - hy - hy;
			} else {
				e += hx + hx;
			}
			y0 += dy;
		}
	}
}

/*
** move to (x,y)
*/

r_move(x, y)
int x, y;
{
	r_currx = x;
	r_curry = y;
}

/*
** draw to (x,y)
** (move pen down)
*/

r_draw(x, y)
int x, y;
{
	r_plot(r_currx, r_curry, x, y);
	r_currx = x;
	r_curry = y;
}

/*
** free the allocated raster
*/
void
r_freeraster()
{
	int y;

	for (y = 0; y < r_ysize; y++) {
		free((char *)(*r_p)[y]);
	}
	free((char *)r_p);
}

HPLJETdump ()
{
	int x, y, i;
	unsigned v;

	printf("\033*t75R\033&a0r\033&a135C\033&a-2R\033*r1A");
	for (x = r_xsize-1; x >= 0; x--) {
		printf("\033*b%dW", r_ysize/8);
		for (y = r_ysize-8; y >= 0; y -= 8) {
			v = 0;
			for (i = 7; i >= 0; i--) {
				v = (v << 1) | r_getpixel(x, y + i);
			}
			putc(v, stdout);
		}
	}
	r_freeraster();
	printf("\033*rB\f");
}

vecsymsgk(str, x, y)
    char *str;
    {
    vecsyms(str, x, y);
    }
