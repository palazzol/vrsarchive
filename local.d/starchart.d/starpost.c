/*
 * PostScript file format driver for startchart.c mainline
 */

#include <stdio.h>
#include "starchart.h"

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

#define PICSCALE (72.0/160.0) /* roughly 6.5 inches/1024.0 units */

static int oldps, vecstate;

out(s)
    char *s;
    {
    printf("%s\n", s);
    }

float conv(i)
    {
    return(i*PICSCALE+80);	/* 1.1" margins left and bottom */
    }

vecsize(newps)
    int newps;
    {
    if (newps != oldps) printf("%d fsize\n", newps);
    oldps = newps;
    }

vecopen()
    {
out("%!PSAdobe-1.0");
out("%%Creator: AWPaeth@watCGL");
out("%%Title: StarChart");
out("%%Pages: 1");
out("%%DocumentFonts Times-Bold Symbol");
out("%%BoundingBox 0 0 612 792");
out("%%EndComments");
out("%%EndProlog");
out("%%Page: 0 1");
out("%");
out("% alias moveto/drawto");
out("%");
out("/mt {moveto} def");				/* move to */
out("/st {lineto} def");				/* solid to */
out("/dt {[1] 0 setdash lineto [] 0 setdash} def");	/* dotted to */
out("/ht {[3] 0 setdash lineto [] 0 setdash} def");	/* hyphen to */
out("%");
out("% show: right, center, left adjust");
out("%");
out("/fsize {/Times-Bold findfont exch scalefont setfont} def");
out("/lshow {5 0 8#040 4 3 roll widthshow} def");
out("/cshow {dup stringwidth pop 2 div neg 0 rmoveto show} def");
out("/rshow {dup stringwidth pop neg 0 rmoveto show} def");
out("/gshow {currentfont exch");
out("        /Symbol findfont 10 scalefont setfont lshow setfont} def");
out("%");
out("% star/planet macros");
out("%");
out("/movereldraw {newpath 4 2 roll mt rlineto stroke} def");
out("/starminus {3 copy 0 movereldraw neg 0 movereldraw} def");
out("/starplus {3 copy 0 movereldraw 3 copy neg 0 movereldraw");
out( "          3 copy 0 exch movereldraw 0 exch neg movereldraw} def");
out("/starcirc {newpath 0 360 arc closepath stroke} def");
out("/starbody {newpath 0 360 arc closepath fill} def");
out("/starbodyvar {3 copy 1 add starcirc starbody} def");
out("/starbodydbl {3 copy dup 3 div add starminus starbody} def");
out("% non-stellar object macros (better designs most welcome)");
out("/planet  {2 copy 2.5 starcirc 3.5 starplus} def");
out("/galaxy  {2 copy 2.5 starcirc 3.5 starminus} def");
out("/nebula  {2 copy 1 setgray 2.5 starcirc 0 setgray [2] 0 setdash");
out("          2.5 starcirc [] 0 setdash} def");
out("/cluster {2.5 starcirc} def");
out("%");
out("/back {3 copy 0.5 add 1 setgray} def");
out("/fore {0 setgray} def");
out("/s {back starbody fore starbody} def");
out("/d {back starbodydbl fore starbodydbl} def");
out("/v {back starbodyvar fore starbodyvar} def");
out("/s0 {4.5 s} def");
out("/d0 {4.5 d} def");
out("/v0 {4.5 v} def");
out("/s1 {3.8 s} def");
out("/d1 {3.8 d} def");
out("/v1 {3.8 v} def");
out("/s2 {3.1 s} def");
out("/d2 {3.1 d} def");
out("/v2 {3.1 v} def");
out("/s3 {2.4 s} def");
out("/d3 {2.4 d} def");
out("/v3 {2.4 v} def");
out("/s4 {1.7 s} def");
out("/d4 {1.7 d} def");
out("/v4 {1.7 v} def");
out("/s5 {1.0 s} def");
out("/d5 {1.0 d} def");
out("/v5 {1.0 v} def");
out("%");
out("% alter line drawing defaults, guarentee solid black lines");
out("0.5 setlinewidth 2 setlinecap");
out("[] 0 setdash 0 setgray");
out("%");
out("% boiler-plate");
out("%");
vecsize(10);
out(" 76 685 mt (Database: Yale Star Catalog) show");
out("548 685 mt (Software: AWPaeth@watCGL) rshow");
vecsize(18);
out("314 685 mt (StarChart) cshow");
vecsize(10);
out("%");
out("% THE STUFF");
out("%");
    }

vecclose()
    {
    out("showpage");
    out("%");
    out("%%Trailer");
    out("%%Pages: 1");
    fflush(stdout);
    }

vecmove(x, y)
    {
    if (vecstate==2) printf("stroke\n");
    if (vecstate==2 || (vecstate == 0)) printf("newpath\n");
    printf("%.1f %.1f mt\n", conv(x), conv(y));
    vecstate = 1;
    }

vecdraw(x, y)
    {
    printf("%.1f %.1f st\n", conv(x), conv(y));
    vecstate = 2;
    }

vecdrawdot(x, y)
    {
    printf("%.1f %.1f dt\n", conv(x), conv(y));
    vecstate = 2;
    }

vecdrawhyph(x, y)
    {
    printf("%.1f %.1f ht\n", conv(x), conv(y));
    vecstate = 2;
    }

vecsyms(x, y, s)
    char *s;
    {
    vecmove(x,y-4);
    printf("(%s) lshow\n", s);
    }

vecmovedraw(x1, y1, x2, y2)
    {
    vecmove(x1, y1);
    vecdraw(x2, y2);
    }

drawPlan(x, y)
    {
    printf("%.1f %.1f planet\n", conv(x), conv(y));
    }

drawGalx(x, y)
    {
    printf("%.1f %.1f galaxy\n", conv(x), conv(y));
    }

drawNebu(x, y)
    {
    printf("%.1f %.1f nebula\n", conv(x), conv(y));
    }

drawClus(x, y)
    {
    printf("%.1f %.1f cluster\n", conv(x), conv(y));
    }

drawStar(x, y, mag, type, color)
    char type, *color;
    {
    char *code;
    switch (mag)
	{
	case -1: vecsize(18); break;
	case  0: vecsize(18); break;
	case  1: vecsize(16); break;
	case  2: vecsize(14); break;
	case  3: vecsize(12); break;
	case  4: vecsize(8); break;
	default: vecsize(6); break;
	}
    if (mag<0) mag = 0;
    if (mag>5) mag = 5;
    switch (type)
	{
	default:
	case 'S': code = "s"; break;
	case 'D': code = "d"; break;
	case 'V': code = "v"; break;
	}
    printf("%.1f %.1f %s%1d\n", conv(x), conv(y), code, mag);
    }

/*
 * Additions for Greek fonts
 */

char  *intable = " 0123456789abgdezh@iklmnEoprstuOx%w";
char *outtable = " 0123456789abgdezhqiklmnxoprstujcyw";

vecsymsgk(x, y, s)
    char *s;
    {
    char ch;
    int i, j;
    i = 0;
    while (ch = s[i])
	{
	j = 0;
	while (intable[j] && (intable[j] != ch)) j++;
	s[i] = intable[j] ? outtable[j] : ' ';
	i++;
	}
    vecmove(x,y-4);
    printf("(%s) gshow\n", s);
    }
