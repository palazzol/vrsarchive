/*
 * Unix Pic file format driver for startchart.c mainline
 */

#include <stdio.h>
#include "starchart.h"

#define PICFRAG 8	/* split long "move,draw,...,draw" chains for pic */
#define DNONE 0		/* track current line style for chains */
#define DSOLID 1
#define DDOT 2
#define DHYPH 3

static int style;
static float xold, yold;

/*
 * Chart parameters (limiting magnitude and window x,y,w,h)
 */

mapblock thumbnail =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			3.2, 1.0, 480, 0, 480, 240, 0.0 };

mapblock master =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 0, 370, 960, 960, 0.0 };

/*
 * Generic Star Drawing Stuff
 */

static int oldps, dcount;

#define PICSCALE (1.0/160.0) /* roughly 6.5/1024.0 */

float conv(i)
    {
    return(i*PICSCALE);
    }

vecsize(newps)
    int newps;
    {
    if (newps != oldps) printf("\n.ps %d", newps);
    oldps = newps;
    }

 vecopen()
   {
   printf(".nf\n.ll 6.75i\n.po 0.75i\n.RT\n.PS");
   vecsize(10);
   printf("\n.tl'Database: \\fIYale Star Catalog\\fP'\\s18\\fBStarChart\\fP\\s0'Software: \\fIAWPaeth@watCGL\\fP'");
   printf("\n.ll 7.25i");
   }

vecclose()
    {
    printf("\n.PE\n");
    fflush(stdout);
    }

vecmove(x, y)
    {
    dcount = 0;
    printf("\nline from %.3fi,%.3fi", xold = conv(x), yold = conv(y));
    style = DNONE;
    }

vecbreak()
    {		/* repost current location when changing line attributes */
    dcount = 0;
    printf("\nline from %.3fi,%.3fi", xold, yold);
    style = DNONE;
    }

vecdraw(x, y)
    {
    if ((style != DNONE) && (style != DSOLID)) vecbreak();
    printf(" to %.3fi,%.3fi", xold = conv(x), yold = conv(y));
    dcount++;
    if (dcount > PICFRAG) vecmove(x,y);	/* must fragment long pic commands */
    style = DSOLID;
    }

vecdrawdot(x, y)
    {
    if ((style != DNONE) && (style != DDOT)) vecbreak();
    printf(" to %.3fi,%.3fi dotted", xold = conv(x), yold = conv(y));
    dcount++;
    if (dcount > PICFRAG) vecmove(x,y);	/* must fragment long pic commands */
    style = DDOT;
    }

vecdrawhyph(x, y)
    {
    if ((style != DNONE) && (style != DHYPH)) vecbreak();
    printf(" to %.3fi,%.3fi dashed", xold = conv(x), yold = conv(y));
    dcount++;
    if (dcount > PICFRAG) vecmove(x,y);	/* must fragment long pic commands */
    style = DHYPH;
    }

vecsyms(x, y, s)
    char *s;
    {
    printf("\n\"\\ %s\" ljust at %.3fi,%.3fi", s, conv(x), conv(y));
    }

vecmovedraw(x1, y1, x2, y2)
    {
    vecmove(x1, y1);
    vecdraw(x2, y2);
    }

drawGalx(x, y)
    {
    vecsize(10);
    vecsymcen(x, y, "@");
    }

drawNebu(x, y)
    {
    vecsize(10);
    vecsymcen(x, y, "\\v'3p'~\\v'-3p'"); /* vertical motion to lower '~' */
    }

drawClus(x, y)
    {
    vecsize(10);
    vecsymcen(x, y, "%");
    }

drawPlan(x, y)
    {
    vecsize(10);
    vecsymcen(x, y, "+");
    }

vecsymcen(x, y, s)
    char *s;
    {
    printf("\n\"%s\" at %.3fi,%.3fi", s, conv(x), conv(y));
    }

drawStar(x, y, mag, type, color)
    char type, *color;
    {
    switch (mag)
	{
	case -1: vecsize(18); break;
	case  0: vecsize(18); break;
	case  1: vecsize(16); break;
	case  2: vecsize(12); break;
	case  3: vecsize(10); break;
	case  4: vecsize(8);  break;
	default: vecsize(6);  break;
	}
    switch (type)
	{
	default:
/*
 * unadulterated overstrikes to form star symbols
 */

	case 'S': vecsymcen(x, y, "\\(bu"); break;
	case 'D': vecsymcen(x, y, "\\o'\\(em\\(bu'"); break;
	case 'V': vecsymcen(x, y, "\\o'O\\(bu'"); break;

/*
 * an attempt to raise (with troff local motion commands) the bullet by
 * two printer's points, as it rides a touch low in the Waterloo fonts.
 */
/*
 *	case 'S': vecsymcen(x, y, "\\v'2p'\\(bu\\v'-2p'"); break;
 *	case 'D': vecsymcen(x, y, "\\o'\\(em\\v'2p'\\(bu\\v'-2p''"); break;
 *	case 'V': vecsymcen(x, y, "\\o'O\\v'2p'\\(bu\\v'-2p''"); break;
 */
	}
    }

/*
 * Additions for Greek fonts
 */

char *intable = " 0123456789abgdezh@iklmnEoprstuOx%w";
char *outtable[] = {
    " ", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "\\(*a", "\\(*b", "\\(*g", "\\(*d", "\\(*e", "\\(*z", "\\(*y", "\\(*h",
    "\\(*i", "\\(*k", "\\(*l", "\\(*m", "\\(*n", "\\(*c", "\\(*o", "\\(*p",
    "\\(*r", "\\(*s", "\\(*t", "\\(*u", "\\(*f", "\\(*x", "\\(*q", "\\(*w"
    };

vecsymsgk(x, y, s)
    char *s;
    {
    char ch, line[200];
    int i;
    line[0] = '\0';
    while (ch = *s++)
	{
	i = 0;
	while (intable[i] && (intable[i] != ch)) i++;
	strcat(line, intable[i] ? outtable[i] : " ");
	}
    printf("\n\"\\ %s\" ljust at %.3fi,%.3fi", line, conv(x), conv(y));
    }
