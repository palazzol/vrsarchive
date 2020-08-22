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
 ! Modified for 3.0 January 1989 by Craig Counterman
 !                                  (email: ccount@royal.mit.edu)
 */

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starlaser.c,v 1.1 1990-03-30 16:39:12 vrs Exp $";

/*
** This code is intended for ALL Laserjet family printers.
** Because the base version has only 59 k raster buffer, the
** stars are not completely round, but not too ugly either.
*/

#include <stdio.h>
#include <math.h>

#include "star3.h"

char *calloc ();

#define SCALEU	29L		/* graphics scaling */
#define SCALEL	40L

#define TSCALEU	69L		/* text mode scaling */
#define TSCALEL	10L

#define XOFF	520		/* text centering offset (in decipoints) */
#define YOFF	(-50)		/* previously, XOFF=1060; YOFF=(-80) */

#define HPLJETXMAX 743		/* Number of pixels in X-axis */
#define HPLJETYMAX 557		/* Number of pixels in Y-axis */


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
  8.0, 2.0, 2.05,	/* default limiting mags for glyph, name, label */

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
  8.0, 2.0, 2.05,	/* default limiting mags for glyph, name, label */

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
  5.5+THMADJ, 1.0+THMADJ, 2.05+THMADJ,
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
  HPLJETorient ();
  r_makeraster (HPLJETXMAX, HPLJETYMAX);

  return TRUE ;				/* open successful */
}

/* Close the device */
D_close()
{
  HPLJETdump ();
}


/* Move to (x, y) */
D_move(x, y)
     int x, y;
{
  x = (x*SCALEU)/SCALEL;		/* adjust to bitmap size */
  y = (y*SCALEU)/SCALEL;

  HPLJETmove (x, y);
}

/* Draw a line of style line_style from the current point to (x, y) */
/* Note, this replaces vecdraw vecdrawdot and vecdrawhyph */
D_draw(x, y, line_style)
     int x, y;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
  x = (x*SCALEU)/SCALEL;		/* adjust to bitmap size */
  y = (y*SCALEU)/SCALEL;

/* all styles are the same */
  HPLJETdraw (x, y);
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the line style drawn as desired */


/* Move to (x1, y1) then draw a line of style line_style to (x2, y2) */
D_movedraw(x1, y1, x2, y2, line_style)
     int x1, y1, x2, y2;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
/* all styles are the same */
  x1 = (x1*SCALEU)/SCALEL;		/* adjust to bitmap size */
  y1 = (y1*SCALEU)/SCALEL;

  x2 = (x2*SCALEU)/SCALEL;		/* adjust to bitmap size */
  y2 = (y2*SCALEU)/SCALEL;

  HPLJETmove (x1, y1);
  HPLJETdraw (x2, y2);
}


/* Set the color to be used for lines and text */
/* color_str is a 2 char (+ '\0') string
   containing a specification for a color,
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
     int fsize;		/* Size of font */
     int font;		/* e.g. TIMES, HELV, TIMES+ITALIC */
{
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the font used as desired */

/*
** Text handling is a pain because of separate text/graphics mode
** and separated axises
*/

/* Display text string str at x,y, in current font and font size.
   if star_lbl is TRUE, string is a star label, use
     greek characters (if possible) */
D_text(x, y, str, star_lbl)
     int x, y;
     char *str;
     int star_lbl;
{
  y = 767 - y;				/* change y axis on text output */
  y -= 5;				/* center character strings */
  x = (x*TSCALEU)/TSCALEL + XOFF;	/* adjust to bitmap size */
  y = (y*TSCALEU)/TSCALEL + YOFF;

  if (star_lbl) {
    /* remove leading spaces */
    while (*str == ' ') str++;
    /* can't display greek characters */
  }

  printf ("\033&a%dh%dV%s", x, y, str);
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


/**
Higher level functions
**/

drawlen(x, y, dx, dy, len)
     int x, y, dx, dy, len;
{
  x = (x*SCALEU)/SCALEL;		 /* adjust to bitmap size */
  y = (y*SCALEU)/SCALEL;

  HPLJETmove (x + dx, y + dy);
  HPLJETdraw (x + dx + len - 1, y+dy);
}


/*
** Low Level Laserjet Plotting Routines
*/

HPLJETorient ()	/* switch to portrait text orientation mode */
{
  printf ("\033E");	/* Restore defaults */
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
void r_setpixel(x, y, val)
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
int r_getpixel(x, y)
     int x, y;
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
     unsigned x, y;
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
      putc((char) v, stdout);
    }
  }
  r_freeraster();
  printf("\033*rB\033&l0O");
	/* Change of orientation seems to produce form feed */
}

