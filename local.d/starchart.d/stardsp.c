/*
 * TTY Display driver for starchart.c mainline
 */

/*
 ! patched December, 1987 by Alan Paeth (awpaeth@watcgl),
 !
 ! [1] formal/actual parmaters for drawNebu, drawPlan.. now agree
 ! [2] "bigmaster" chart layout now added
 !
 */

#include <stdio.h>
#include <ctype.h>
#include "starchart.h"

#define MAX(a,b) ((a)>(b)?(a):(b))
#define ROWS 32
#define COLS 79

/*
 * Chart parameters (limiting magnitude and window x,y,w,h)
 */

mapblock thumbnail =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			3.0, 1.0, 2.05, 420, 35, 480, 195, 0.0 };

mapblock master =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			5.9, 2.0, 2.05, 20, 265, 880, 500, 0.0 };

mapblock bigmaster =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			5.9, 2.0, 2.05, 20, 65, 880, 700, 0.0 };

/*
 * Generic Star Drawing Stuff
 */

static int oldx, oldy, scrx, scry;
static char **linevec;

vecopen()
   {
   int i;
   linevec = (char**)(calloc(ROWS, sizeof(char*)));
   for (i=0; i<ROWS; i++) linevec[i] = (char*)(calloc(COLS, sizeof(char)));
   }

vecclose()
    {
    int i, j, k;
    char c;
    for (i=0; i<ROWS; i++)
	{
	for (j=COLS-1; j>0; j--) if (linevec[i][j]) break;
	for (k=0; k<=j; k++) putchar((c=linevec[i][k]) ? c : ' ');
	putchar('\n');
	free(linevec[i]);
	}
    fflush(stdout);
    free(linevec);
    }

vecsize(points)
    int points;
    {
    }

vecmove(x, y)
    {
    oldx = x;
    oldy = y;
    scrx = (x*COLS+512)/1024;
    scry = (((768-y)*ROWS)+384)/768;
    }

vecdrawdot(x, y)
    {
    vecdraw(x, y); /* dotted and solid the same */
    }

vecdrawhyph(x, y)
    {
    vecdraw(x, y); /* dashed (hyphen) and solid the same */
    }

vecdraw(x, y)
    {
    int dx, dy, savex, savey, i, steps;
    char c;
    savex = oldx;
    savey = oldy;
    dx = x-oldx;
    dy = y-oldy;
    c = (abs(dx) > abs(dy)) ? '-' : '|';
    steps = MAX(MAX(abs(dx),abs(dy))/12, 1);
    for(i=0; i<=steps; i++)
	{
	vecmove(savex+(i*dx+steps/2)/steps,savey+(i*dy+steps/2)/steps);
	sym(c);
	}
    }

vecsyms(x, y, s)
    char *s;
    {
    char c;
    vecmove(x, y);
    while(c = *s++)
	{
	sym(c);
	scrx++;
	}
    }

vecmovedraw(x1, y1, x2, y2)
    {
    vecmove(x1, y1);
    vecdraw(x2, y2);
    }

drawPlan(x, y, mag, type, color)
    char type, *color;
    {
    vecsyms(x, y, "+");
    }

drawStar(x, y, mag, type, color)
    char type, *color;
    {
    switch (mag)
	{
	case -1: draw0(x, y); break;
	case  0: draw0(x, y); break;
	case  1: draw1(x, y); break;
	case  2: draw2(x, y); break;
	case  3: draw3(x, y); break;
	case  4: draw4(x, y); break;
	default: draw5(x, y); break;
	}
    }

draw0(x, y)
    {
    vecsyms(x, y, "O");
    }

draw1(x, y)
    {
    vecsyms(x, y, "*");
    }

draw2(x, y)
    {
    vecsyms(x, y, "o");
    }

draw3(x, y)
    {
    vecsyms(x, y, "-");
    }

draw4(x, y)
    {
    vecsyms(x, y, ",");
    }

draw5(x, y)
    {
    vecsyms(x, y, ".");
    }

drawGalx(x, y, mag, type, color)
    char type, *color;
    {
    vecsyms(x, y, "@");
    }

drawNebu(x, y, mag, type, color)
    char type, *color;
    {
    vecsyms(x, y, "~");
    }

drawClus(x, y, mag, type, color)
    char type, *color;
    {
    vecsyms(x, y, "%");
    }

char overwrite(under, over)
    char under, over;
    {
    if (over == under) return(over);
    if (!under) return(over);
    if (!over) return(under);
    if (isspace(under)) return(over);
    if (isspace(over)) return(under);
    if (over == '%') return(over);
    if (over == '~') return(over);
    if (over == '@') return(over);
    if (under == '%') return(under);
    if (under == '~') return(under);
    if (under == '@') return(under);
    if ( isalnum(under) &&  isalnum(over)) return(over);
    if (!isalnum(under) &&  isalnum(over)) return(over);
    if ( isalnum(under) && !isalnum(over)) return(under);
    if ((under == '-') && (over == '|')) return('+');
    if ((under == '|') && (over == '-')) return('+');
    if ((under == '+') && (over == '-')) return('+');
    if ((under == '+') && (over == '|')) return('+');
    if ((under == ',') && (over == '.')) return(';');
    if ((under == '.') && (over == ',')) return(';');
    if ((under == '.') && (over == '.')) return(':');
    if ((under == '|') && (over == '.')) return('!');
    if ((under == '|') && (over == ',')) return('!');
    if ((under == '.') && (over == '|')) return('!');
    if ((under == ',') && (over == '|')) return('!');
    if ((under == '.') && (over == '.')) return(':');
    return(under);
    }

sym(c)
    char c;
    {
    if ( (scrx >= 0) && (scrx < COLS) &&
	 (scry >= 0) && (scry < ROWS) )
	linevec[scry][scrx] = overwrite(linevec[scry][scrx], c );
    }

vecsymsgk(str, x, y)
    char *str;
    {
    vecsyms(str, x, y);
    }
