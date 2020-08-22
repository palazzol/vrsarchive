/*
 * Plotter Display driver for starchart.c mainline (UNIX Plot(5) style output)
 *
 * Sjoerd Mullender <sjoerd@cs.vu.nl>
 * Free University, Amsterdam
 */

/*
 * Produced for starchart 3.0 by Craig Counterman Jan, 1989
 *
 * Copyright (c) 1990 by Craig Counterman. All rights reserved.
 *
 * This software may be redistributed freely, not sold.
 * This copyright notice and disclaimer of warranty must remain
 *    unchanged. 
 *
 * No representation is made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty, to the extent permitted by applicable law.
 *
 */


static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/staruplot.c,v 1.1 1990-03-30 16:39:21 vrs Exp $";

#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif

#include "star3.h"

/* Externs */
extern int g_argc;
extern char **g_argv;

extern char *title;	/* Title of page */

extern mapwindow *mapwin[MAXWINDOWS];
extern int numwins;

extern int cur_function;
extern int cur_map_type;
extern int cur_map_tag;
extern char *cur_tag_field;



/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail,*/
#define THSMUL 1.2
#define THSMIN 12.0
#define THMADJ 2.5
#define THMMAX 5.0

/* Exports */

/* The variables in the first few lines MUST be set by driver */
mapwindow fullpage = {
  880, 700, 20, 65,	/* width, height, x and y offsets */
  5.9, 2.0, 2.05,	/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  FULLPAGEMAP,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */
  
/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  0.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};

/* The variables in the first few lines MUST be set by driver */
mapwindow mainmap = {
  880, 500, 20, 265,	/* width, height, x and y offsets */
  5.9, 2.0, 2.05,	/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  MAINMAP,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  0.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};


/* The variables in the first few lines MUST be set by driver */
mapwindow thumbmap = {
  480, 195, 420, 35,	/* width, height, x and y offsets */
  3.0+THMADJ, 1.0+THMADJ, 2.05+THMADJ,
			/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  THUMBNAIL,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  0.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};

/* h & v tick text controls */
int htick_lim = 2;
int htext_lim = 80;
int htext_xoff = 2;
int htext_yoff = 17;
int vtick_lim = 2;
int vtext_lim = 20;
int vtext_xoff = 24;
int vtext_yoff = 0;


/* externs for labels */
int x_nameoffset = 10, y_nameoffset = 0;
int x_lbloffset = 0, y_lbloffset = 10;
int x_magoffset = 7, y_magoffset = -15;

/* externs for legend: variables of positioning are here */
int l_til=220;
int l_stil=185;

int l_lmar1=40;
int l_lmar2=65;
int l_ltext=95;
int l_rmar1=205;
int l_rmar2=230;
int l_rtext=260;

int l_line1=150;
int l_line2=125;
int l_line3=100;
int l_line4=75;
int l_line5=50;
int l_line6=25;

/* Point sizes for font calls */
int titlesize=16;
int subtlsize=12;
int namesize=10;
int lblsize=8;
int magsize=8;

/* Fonts for font calls */
int namefnt=TIMESROMAN;
int lblfnt=HELV;
int magfnt=COURIER;
int titlefnt=TIMESBOLD;
int subtlfnt=TIMESROMAN;

/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail,*/
double th_smul=THSMUL;
double th_smin=THSMIN;
double th_madj=THMADJ;
double th_mmax=THMMAX;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))


/* Device control argument */
D_control_arg(s)
char *s;
{
  int i = 0;
  int c;

  while (c = s[i++]) switch (c) {
  default:
    break;
  }
}

/* Open the device */
D_open()
{
  openpl();
  space(0, 0, 1024, 1024);
  erase();

  return TRUE ;				/* open successful */
}


/* Close the device */
D_close()
{
  closepl();
}


/* Move to (x, y) */
D_move(x, y)
     int x, y;
{
    move(x, y);
}

int cur_sty;
/* Draw a line of style line_style from the current point to (x, y) */
/* Note, this replaces vecdraw vecdrawdot and vecdrawhyph */
D_draw(x, y, line_style)
     int x, y;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
  int sty;

  switch(cur_function) {
  case CHRTOUTLN:
  case CHRTHTICK:
  case CHRTVTICK:
    sty = SOLID;
    break;
  case GRID_RA:
  case GRID_DEC:
    sty = DOTTED;
    break;
  case ECLIPT:
    sty = DOTTED;
    break;
  case CONSTBOUND:
    sty = DASHED;
    break;
  case CONSTPATTRN:
    sty = SOLID;
    break;
  case CONSTNAME:
  case CHARTFILE:
  default:
    sty = line_style;
    break;
  }

  if (sty == cur_sty) {	/* Continue current line, style */
    cont(x,y);
  } else {			/* Change style */
    switch(sty) {
    case SOLID:
      linemod("solid");
      break;
    case DOTTED:
      linemod("dotted");
      break;
    case DASHED:
      linemod("shortdashed");
      break;
    case VECSOLID:
      linemod("solid");
      break;
    case VECDOT:
      linemod("dotted");
      break;
    case VECDASH:
      linemod("shortdashed");
      break;
    default:
      linemod("solid");
      break;
    }
    cur_sty = sty;
    cont(x,y);
  }
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the line style drawn as desired */


/* Move to (x1, y1) then draw a line of style line_style to (x2, y2) */
D_movedraw(x1, y1, x2, y2, line_style)
     int x1, y1, x2, y2;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
  D_move(x1, y1);
  D_draw(x2, y2, line_style);
}


/* Set the color to be used for lines and text */
/* color_str is a 2 char (+ '\0') string containing
   a specification for a color,
   e.g. "G2" for the color of a star of spectral class G2, or "r7" for
   red, level seven.  The interpretation of the color string is left to
   the device driver */
D_color(color_str)
     char *color_str;
{
  switch (color_str[0]) {
  case 'O':
    break;
  case 'B':
    break;
  case 'A':
    break;
  case 'F':
    break;
  case 'G':
    break;
  case 'K':
    break;
  case 'M':
    break;
  case 'R':
  case 'N':
  case 'S':
    break;
  case ' ':
  default:
    break;
  }
}


/* Set the font and font size to be used for text. */
/* Note order of args */
D_fontsize(fsize, font)
     int fsize;	/* Size of font */
     int font;	/* e.g. TIMES, HELV, TIMES+ITALIC */
{
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the font used as desired */


/* Display text string str at x,y, in current font and font size.
   if star_lbl is TRUE, string is a star label, use
     greek characters (if possible) */
D_text(x, y, str, star_lbl)
     int x, y;
     char *str;
     int star_lbl;
{
  move(x, y);
  if (star_lbl) {
    /* remove leading spaces */
    while (*str == ' ') str++;
    /* can't display greek characters */
  }
  label(str);
}


/* Return input coordinate in device coords where there are pointing devices */
D_inxy(x, y)
     int *x, *y;
{
}


/* Put non-displayed comment in output.  Allowed in postscript, but
   few other drivers will be able to support this. */ 
D_comment(str)
     char *str;
{
/*
  fprintf(stderr, "%s\n", str);
*/
}


drawlen(x, y, dx, dy, len)
     int x, y, dx, dy, len;
{
  D_movedraw(x+dx, y+dy,
	     x+dx+len,
	     y+dy, SOLID);
}






