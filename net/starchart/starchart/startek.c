/*
 * Tektronix driver for startchart.c mainline
 */

/*
 ! patched December, 1987 by Alan Paeth (awpaeth@watcgl)
 !
 ! [1] "bigmaster" chart layout now added
 !
 */
/*
 ! Modified for 3.0 January 1989 by Craig Counterman
 */
/*
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/startek.c,v 1.1 1990-03-30 16:39:17 vrs Exp $";


#include <stdio.h>
#include <math.h>

#include "star3.h"

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
#define XSCALEI 1
#define XSCALEO 2
#define YSCALEI 1
#define YSCALEO 2
#else
#define XSCALEI 1
#define XSCALEO 1
#define YSCALEI 1
#define YSCALEO 1
#endif

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
  tekclear();

  return TRUE ;				/* open successful */
}

/* Close the device */
D_close()
{
  tekmove(0,0);
  tekalpha();
  fflush(stdout);
}


/* Move to (x, y) */
D_move(x, y)
     int x, y;
{
  tekmove(x, y);
}

/* Draw a line of style line_style from the current point to (x, y) */
/* Note, this replaces vecdraw vecdrawdot and vecdrawhyph */
D_draw(x, y, line_style)
     int x, y;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
/* all styles are the same */
  tekdraw(x, y);
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the line style drawn as desired */

/* Move to (x1, y1) then draw a line of style line_style to (x2, y2) */
D_movedraw(x1, y1, x2, y2, line_style)
     int x1, y1, x2, y2;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
/* all styles are the same */
  tekmove(x1, y1);
  tekdraw(x2, y2);
  fflush(stdout);
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


/* Display text string str at x,y, in current font and font size.
   if star_lbl is TRUE, string is a star label, use
     greek characters (if possible) */
D_text(x, y, str, star_lbl)
     int x, y;
     char *str;
     int star_lbl;
{
  tekmove(x, y-11);			/* center character strings */
  tekalpha();

  if (star_lbl) {
    /* remove leading spaces */
    while (*str == ' ') str++;
    /* can't display greek characters */
  }

  printf(str);
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
  D_movedraw((x*XSCALEI/XSCALEO+dx)*XSCALEO/XSCALEI,
	     (y*YSCALEI/YSCALEO+dy)*YSCALEO/YSCALEI,
	     (x*XSCALEI/XSCALEO+dx+len-1)*XSCALEO/XSCALEI+1,
	     (y*YSCALEI/YSCALEO+dy)*YSCALEO/YSCALEI, SOLID);
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
     int x, y;
{
  putchar(GS);
  tekdraw(x, y);
}

tekdraw(x, y)	/* draw to (x,y) */
     int x, y;
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
