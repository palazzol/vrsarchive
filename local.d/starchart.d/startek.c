/*
 * Tektronix driver for startchart.c mainline
 */

#include <stdio.h>
#include "starchart.h"

/*
 * The following rational fractions are for Tek output on limited (and
 * not 1024x768) bitmap devices, and attempt to cause graceful scaling of
 * glyphs, by defining the distance between adjacent output pixels in terms
 * of Tek coordinates. They should be fine-tuned for your Tektronix emulator.
 * Additional tuning (for rounding considerations) must take place in the
 * routine where the four values are referenced.
 *
 * Typical fractions are 5/8 (yields 640x480), 1/2, and 3/4
 *
 * For full resolution Tektronix devices (full 1024x768), all values are 1.
 */

#ifndef TEK
#define XSCALEI 5
#define XSCALEO 8
#define YSCALEI 5
#define YSCALEO 8
#else
#define XSCALEI 1
#define XSCALEO 1
#define YSCALEI 1
#define YSCALEO 1
#endif

/*
 * Chart parameters (limiting magnitude and window x,y,w,h)
 */

mapblock thumbnail =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			3.2, 1.0, 420, 35, 480, 195, 0.0 };

mapblock master =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 20, 265, 880, 500, 0.0 };

/*
 * Generic Star Drawing Stuff
 */

vecopen()
   {
   tekclear();
   }

vecclose()
    {
    tekmove(0,0);
    tekalpha();
    fflush(stdout);
    }

vecsize(points)
    int points;
    {
    }

vecmove(x, y)
    {
    tekmove(x, y);
    }

vecdraw(x, y)
    {
    tekdraw(x, y);
    }

vecdrawdot(x, y)
    {
    vecdraw(x, y);	/* dotted and solid the same */
    }

vecdrawhyph(x, y)
    {
    vecdraw(x, y);	/* dashed and solid the same */
    }


vecsym(x, y, s)
    char s;
    {
    tekmove(x, y-11); /* center character strings */
    tekalpha();
    printf("%c",s);
    }

vecsyms(x, y, s)
    char *s;
    {
    tekmove(x, y-11); /* center character strings */
    tekalpha();
    printf(s);
    }

vecmovedraw(x1, y1, x2, y2)
    {
    tekmove(x1, y1);
    tekdraw(x2, y2);
    fflush(stdout);
    }


drawlen(x, y, dx, dy, len)
    {
    vecmovedraw((x*XSCALEI/XSCALEO+dx)*XSCALEO/XSCALEI,
		(y*YSCALEI/YSCALEO+dy)*YSCALEO/YSCALEI,
		(x*XSCALEI/XSCALEO+dx+len-1)*XSCALEO/XSCALEI+1,
		(y*YSCALEI/YSCALEO+dy)*YSCALEO/YSCALEI);
    }

/*
 * Low Level Tektronix Plotting Routines
 */

#define	GS	035
#define	US	037
#define ESC	033
#define FF	014

static int oldHiY = 0, oldLoY = 0, oldHiX = 0;

tekplot()	/* switch to plot mode */
    {
    putchar(GS);
    putchar('@');
    oldHiY = oldLoY = oldHiX = 0;
    }

tekalpha()	/* switch to alpha mode */
    {
    putchar(US);
    fflush(stdout);
    }

tekclear()
    {
    putchar(ESC);
    putchar(FF);
    fflush(stdout);
    }

tekmove(x, y)	/* move to (x,y) */
    {
    putchar(GS);
    tekdraw(x, y);
    }

tekdraw(x, y)	/* draw to (x,y) */
    {
    int hiY, loY, hiX, loX;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > 1023) x = 1023;
    if (y > 767) y = 767;

    hiY = 0040 | (y>>5 & 037),
    loY = 0140 | (y    & 037),
    hiX = 0040 | (x>>5 & 037),
    loX = 0100 | (x    & 037);

    if (hiY != oldHiY) putchar(hiY);
    if (loY != oldLoY || hiX != oldHiX) putchar(loY);
    if (hiX != oldHiX) putchar(hiX);
    putchar(loX);

    oldHiY = hiY;
    oldLoY = loY;
    oldHiX = hiX;
    }

vecsymsgk(str, x, y)
    char *str;
    {
    vecsyms(str, x, y);
    }
