/*
 * PostScript file format driver for startchart.c mainline
 */

/*
 ! patched December, 1987 by Alan Paeth (awpaeth@watcgl),
 ! based on revisions by Craig Counterman (ccount@royal.mit.edu)
 !
 ! [1] "bigmaster" chart layout now added
 ! [2] extensive rework and subclassing of non-stellar objects, e.g.
 !     galaxies, now have both spiral and irregular variants.
 ! [3] star sizes now extended to magnitude 10
 ! [4] white halo-ing of text overlays (by analogy to star haloing)
 !
 */

#include <stdio.h>
#include "starchart.h"

/*
 * Chart parameters (limiting magnitude and window x,y,w,h)
 */

mapblock thumbnail =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			3.2, 1.0, 2.05, 480, 0, 480, 240, 0.0 };

mapblock master =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 2.05, 0, 370, 960, 960, 0.0 };

mapblock bigmaster =	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			8.0, 3.0, 2.05, 0, 30, 960, 1400, 0.0 };

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

double conv(i)
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
out("% The default lshow halos glyphs, but is slow. It is especially");
out("% useful when both the -a and -o switches are in effect (see man page)");
out("%/lshow {5 0 8#040 4 3 roll widthshow} def %fast version");
out("/lshow {true charpath gsave 1 setgray stroke grestore 0 setgray fill} def");
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
out("%");
out("/back {3 copy 0.5 add 1 setgray} def");
out("/fore {0 setgray} def");
out("/s {back starbody fore starbody} def");
out("/d {back starbodydbl fore starbodydbl} def");
out("/v {back starbodyvar fore starbodyvar} def");
out("/s0 {4.5 s} def");
out("/d0 {4.5 d} def");
out("/v0 {4.5 v} def");
out("/s1 {4.0 s} def");
out("/d1 {4.0 d} def");
out("/v1 {4.0 v} def");
out("/s2 {3.5 s} def");
out("/d2 {3.5 d} def");
out("/v2 {3.5 v} def");
out("/s3 {3.0 s} def");
out("/d3 {3.0 d} def");
out("/v3 {3.0 v} def");
out("/s4 {2.5 s} def");
out("/d4 {2.5 d} def");
out("/v4 {2.5 v} def");
out("/s5 {2.0 s} def");
out("/d5 {2.0 d} def");
out("/v5 {2.0 v} def");
out("/s6 {1.7 s} def");
out("/d6 {1.7 d} def");
out("/v6 {1.7 v} def");
out("/s7 {1.5 s} def");
out("/d7 {1.5 d} def");
out("/v7 {1.5 v} def");
out("/s8 {1.3 s} def");
out("/d8 {1.3 d} def");
out("/v8 {1.3 v} def");
out("/s9 {1.1 s} def");
out("/d9 {1.1 d} def");
out("/v9 {1.1 v} def");
out("/s10 {1.0 s} def");
out("/d10 {1.0 d} def");
out("/v10 {1.0 v} def");
out("%");
out("% non-stellar object macros (better designs most welcome)");
out("/planet  {2 copy 2.5 starcirc 3.5 starplus} def");
out("/asteroid { 2 copy newpath 1.5 0 360 arc closepath fill moveto");
out("        3 3 rmoveto -6 -6 rlineto 6 0 rmoveto -6 6 rlineto stroke } def");
out("/comet { 2 copy newpath 3 0 360 arc moveto 45 rotate");
out("  6 3 rmoveto -6 0 rlineto 3 -3 rmoveto 3 0 rlineto 0 -3 rmoveto");
out("  -6 0 rlineto   closepath stroke -45 rotate } def");
out("/nebulad  { newpath moveto 3 0 rmoveto -3 -3 rlineto -3 3 rlineto");
out("  3 3 rlineto 3 -3 rlineto stroke } def");
out("/nebulap { 2 copy newpath translate 30 rotate 1 .5 scale");
out("    0 0 3 0 360 arc closepath stroke");
out("    1 2 scale -30 rotate neg exch neg exch translate } def");
out("/galaxye { 2 copy newpath translate -30 rotate 1 .5 scale");
out("    0 0 3 0 360 arc closepath fill");
out("    1 2 scale 30 rotate neg exch neg exch translate } def");
out("/galaxys { 2 copy 2 copy newpath translate -30 rotate 1 .5 scale");
out("    0 0 3 0 360 arc closepath fill");
out("    -4 -3 moveto -3 0 lineto 4 3 moveto 3 0 lineto ");
out("    stroke  1 2 scale 30 rotate ");
out("    neg exch neg exch translate } def");
out("/galaxyq { 2 copy 2 copy newpath translate -30 rotate 1 .5 scale");
out("    0 0 3 0 360 arc closepath fill");
out("    1 2 scale 30 rotate ");
out("    -3 0 moveto 3 0 lineto 0 -3 moveto 0 3 lineto stroke");
out("    neg exch neg exch translate } def");
out("/clustero { 2 copy newpath 1 setgray 3 0 360 arc fill");
out("    [1 2] 0 setdash 0 setgray 3 0 360 arc stroke [] 0 setdash } def");
out("/clusterg { 2 copy 2 copy newpath 1 setgray 3 0 360 arc fill");
out("    [1 2] 0 setdash 0 setgray 3 0 360 arc stroke [] 0 setdash");
out("    1.5 0 360 arc closepath fill } def");
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

drawPlan(x, y, mag, type, color)
    char type, *color;
    {
    switch(type)
	{
	case 'A': printf("%.1f %.1f asteroid\n", conv(x), conv(y)); break;
	case 'C': printf("%.1f %.1f comet\n", conv(x), conv(y)); break;
	case 'S':
	case 'M':
	case 'V':
	case 'm':
	case 'J':
	case 's':
	case 'U':
	case 'N':
	default:  printf("%.1f %.1f planet\n", conv(x), conv(y));
	}
    }

drawGalx(x, y, mag, type, color)
    char type, *color;
    {
    switch(type)
	{
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'B':
	case 'S':
	case 'O': printf("%.1f %.1f galaxys\n", conv(x), conv(y)); break;
	case 'Q': printf("%.1f %.1f galaxyq\n", conv(x), conv(y)); break;
	case 'E':
	case 'I':
	case 'G':
	default:  printf("%.1f %.1f galaxye\n", conv(x), conv(y));
	}
    }

drawNebu(x, y, mag, type, color)
    char type, *color;
    {
    switch(type)
	{
	case 'P': printf("%.1f %.1f nebulap\n", conv(x), conv(y)); break;
	case 'D':
	default:  printf("%.1f %.1f nebulad\n", conv(x), conv(y));
	}
    }

drawClus(x, y, mag, type, color)
    char type, *color;
    {
    switch(type)
	{
	case 'G': printf("%.1f %.1f clusterg\n", conv(x), conv(y)); break;
	case 'O':
	default:  printf("%.1f %.1f clustero\n", conv(x), conv(y));
	}
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
    if (mag>10) mag = 10;
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
