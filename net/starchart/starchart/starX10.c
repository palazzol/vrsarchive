/*
 * X10 Driver for starchart 3.0.  Craig Counterman Jan, 1989
 *
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starX10.c,v 1.1 1990-03-30 16:39:03 vrs Exp $";

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "star3.h"

#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif

#include <X/Xlib.h>
/*#include "Xlib.h"*/

/* Cursor for window */
#define target_width 16
#define target_height 16
#define target_x_hot 8
#define target_y_hot 8
short target_bits[] = {
  0x0000, 0x0280, 0x0280, 0x0ee0,
  0x1290, 0x2288, 0x26c8, 0xfd7e,
  0x0280, 0xfd7e, 0x26c8, 0x2288,
  0x1290, 0x0ee0, 0x0280, 0x0280};
#define target_mask_width 16
#define target_mask_height 16
#define target_mask_x_hot 8
#define target_mask_y_hot 8
short target_mask_bits[] = {
  0x0000, 0x0280, 0x0380, 0x0fe0,
  0x1390, 0x2388, 0x26c8, 0xfd7e,
  0x7abc, 0xfd7e, 0x26c8, 0x2388,
  0x1390, 0x0fe0, 0x0380, 0x0280};

/* Use array of styles and fonts */
#define MAXLNSTY MAXSTLNS
#define MAXFONTS 100

/* X items */
Display *display;	/* connection to display server */
Window window;		/* window to graphics in */
int planes;		/* plane mask */
int *pixels;		/* color map cells */
Cursor cursor;		/* current cursor */
int inmask;		/* input mask for window */
int ncolors;		/* number of colors */
int is_color;		/* is color workstation */
int colr;		/* current color */
int forepix, backpix;
FontInfo *D_Fonts[MAXFONTS];
FontInfo *D_GrkFont;
FontInfo *current_font;
int D_Linesty[MAXLNSTY];

#define BLACK 0
#define WHITE 1
#define RED 2
#define ORANGE 3
#define YELLOW 4
#define GREEN 5
#define CYAN 6
#define BLUE 7
#define VIOLET 8
#define BLUE_WHITE 9


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

/* size of window */
#define WINWID 1020
#define WINHGT 800


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
int x_lbloffset = 7, y_lbloffset = 10;
int x_magoffset = 7, y_magoffset = -10;

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
double th_smul = THSMUL;
double th_smin = THSMIN;
double th_madj = THMADJ;
double th_mmax = THMMAX;

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
  char *wname;
  extern char *getenv();
  char *str, *prog = "Starchart";	/* default name string */
  char *fore_color, *back_color;
  int reverse = 1;
  Color cdef;

  wname= NULL;

  if ( !(display = XOpenDisplay(wname)))
    return FALSE;

  if ((str = XGetDefault(prog, "ReverseVideo")) && strcmp(str, "on") == 0)
    reverse = 0;
  fore_color = XGetDefault(prog, "ForeGround");
  back_color = XGetDefault(prog, "BackGround");

  cursor=XCreateCursor(target_width,target_height,
		       target_bits,target_mask_bits,
		       target_x_hot,target_y_hot,
		       BlackPixel,WhitePixel,GXcopy);


  /* create color map */
		
  if (reverse) {
    forepix = BlackPixel;
    backpix = WhitePixel;
  } 
  else {
    forepix = WhitePixel;
    backpix = BlackPixel;
  }

  is_color = (DisplayPlanes() >= 4);
  ncolors = (is_color ? 12 : 2);

  pixels = (int *) malloc((long) ncolors*sizeof(int));
  if (is_color) {
    if (!XGetColorCells(0, ncolors, 0, &planes, pixels)) {
      fprintf(stderr, "Error, can't get color cells\n");
      exit(10);
    }

    if (!fore_color || !XParseColor(fore_color, &cdef)) {
      cdef.pixel = forepix ;
      XQueryColor(&cdef) ;
    }
    cdef.pixel = pixels[0] ;
    XStoreColor(&cdef) ;
    if (!back_color || !XParseColor(back_color, &cdef)) {
      cdef.pixel = backpix ;
      XQueryColor(&cdef) ;
    }
    cdef.pixel = pixels[1] ;
    XStoreColor(&cdef) ;
    D_setcolors();
  } else {
    pixels[BLACK] = backpix ;
    pixels[WHITE] = forepix ;
  }

  colr = pixels[WHITE];

  /* open window */
  window = XCreateWindow(RootWindow,0,0,WINWID,WINHGT,2,WhitePixmap,
			 BlackPixmap);
  XMapWindow(window) ;
  XDefineCursor(window,cursor);
  inmask = ButtonPressed|KeyPressed|ExposeWindow ;
  XSelectInput(window, inmask) ;

  D_setfnts();
  D_setlinesty();

  XFlush();

  return TRUE ;				/* open successful */
} 

/* Close the device */
D_close()
{
  Window closebox;
  XEvent rep; /*isn't really used*/

  XFlush();
  /* create a close box */
  closebox=XCreateWindow(window,0,0,20,20,0,WhitePixmap,BlackPixmap);

  XMapWindow(closebox);
  XSelectInput(closebox,ButtonPressed);
  XWindowEvent(closebox,ButtonPressed,&rep);
  XUndefineCursor(window);
  XFreeCursor(cursor);
  XDestroyWindow(window) ;
  XCloseDisplay(display) ;
}


static int current_x, current_y;

/* Move to (x, y) */
D_move(x, y)
     int x, y;
{
  current_x = x;
  current_y = y;
}


/* Draw a line of style line_style from the current point to (x, y) */
/* Note, this replaces vecdraw vecdrawdot and vecdrawhyph */
D_draw(x, y, line_style)
     int x, y;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
  Vertex line[2];
  int l_style;

  line[0].x = current_x;
  line[0].y = WINHGT - current_y;
  line[1].x = x;
  line[1].y = WINHGT - y;
  line[0].flags = line[1].flags = 0;

  switch (cur_function) {
  case CHRTOUTLN:
  case CHRTHTICK:
  case CHRTVTICK:
    l_style = SOLID;
    break;
  case GRID_RA:
  case GRID_DEC:
    l_style = DASHED;
    break;
  case ECLIPT:
    l_style = DOTTED;
    break;
  case CONSTBOUND:
    l_style = DASHED;
    break;
  case CONSTPATTRN:
    l_style = DOTTED;
    break;
  case CONSTNAME:
  case CHARTFILE:
    l_style = line_style;
    break;
  default:
    l_style = line_style;
    break;
  }

  XDrawDashed(window, line, 2, 1, 1, (is_color ? colr : pixels[BLACK]),
	      D_Linesty[l_style], GXcopy, AllPlanes);

  current_x = x;
  current_y = y;
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
/* color_str is a 2 char (+ '\0') string
   containing a specification for a color,
   e.g. "G2" for the color of a star of spectral class G2, or "r7" for
   red, level seven.  The interpretation of the color string is left to
   the device driver */
D_color(color_str)
     char *color_str;
{
  if (!is_color) return;

  switch (color_str[0]) {
  case 'O':
    colr = pixels[BLUE_WHITE];
    break;
  case 'B':
    colr = pixels[BLUE];
    break;
  case 'A':
    colr = pixels[WHITE];
    break;
  case 'F':
    colr = pixels[WHITE];
    break;
  case 'G':
    colr = pixels[YELLOW];
    break;
  case 'K':
    colr = pixels[ORANGE];
    break;
  case 'M':
    colr = pixels[RED];
    break;
  case 'R':
  case 'N':
  case 'S':
    colr = pixels[RED];
    break;
  case ' ':
  default:
    colr = pixels[WHITE];
    break;
  }
}


/* Set the font and font size to be used for text. */
/* Note order of args */
D_fontsize(fsize, font)
     int fsize;		/* Size of font */
     int font;		/* e.g. TIMES, HELV, TIMES+ITALIC */
{
  switch(cur_function) {
  case CHRTOUTLN:
  case CHRTHTICK:
  case CHRTVTICK:
    current_font = D_Fonts[TIMESBOLD];
  case GRID_RA:
  case GRID_DEC:
  case ECLIPT:
  case CONSTBOUND:
  case CONSTPATTRN:
    break;
  case CONSTNAME:
    current_font = D_Fonts[TIMESROMAN];
  case CHARTFILE:
  case CURNTFONT:
  default:
    break;
  }
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
  FontInfo *tfont;

  if (star_lbl) {
    if (isgreek(str[0]) && (isdigit(str[1]) || (str[1] == ' '))) {
      tfont = D_GrkFont;
    } else { /* Star label, but not greek */
	/* remove leading spaces */
      while (*str == ' ') str++;
      tfont = current_font;
    }
  } else tfont = current_font;

  XText(window, x, WINHGT - y, str, strlen(str),
	tfont->id,
	backpix, forepix);
}

isgreek(c)
char c;
{
  char *cp;

#ifdef OLD_GREEK
  cp = "abgdezh@iklmnEoprstuOx%w";
#else
  cp = "abgdezhqiklmnxoprstujcyw";
#endif
  while (*cp && (*cp != c)) cp++;
  return (*cp != '\0'); /* True if letter was in greek string */
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
  fprintf(stderr, "%s\n", str);
}


/**
Higher level functions
**/

drawlen(x, y, dx, dy, len)
     int x, y, dx, dy, len;
{
  D_movedraw(x + dx, y + dy, x + dx + len, y + dy, SOLID);
}

D_setfnts()
{
  D_Fonts[TIMESROMAN] = XOpenFont("6x10");
  D_Fonts[TIMESBOLD] = XOpenFont("6x10");
  D_Fonts[TIMESITAL] = XOpenFont("6x10");
  D_Fonts[TIMESBOLDITAL] = XOpenFont("6x10");
  D_Fonts[HELV] = XOpenFont("6x10");
  D_Fonts[HELVBOLD] = XOpenFont("6x10");
  D_Fonts[HELVITAL] = XOpenFont("6x10");
  D_Fonts[HELVBOLDITAL] = XOpenFont("6x10");
  D_Fonts[COURIER] = XOpenFont("6x10");
  D_Fonts[COURBOLD] = XOpenFont("6x10");
  D_Fonts[COURITAL] = XOpenFont("6x10");
  D_Fonts[COURITALBOLD] = XOpenFont("6x10");
  D_GrkFont = XOpenFont("6x10");

  current_font = D_Fonts[TIMESROMAN];
}

D_setlinesty()
{
  D_Linesty[SOLID] = SolidLine;
  D_Linesty[DOTTED] = (is_color ? DottedLine : SolidLine);
  D_Linesty[DASHED] = (is_color ? DashedLine : SolidLine);
  D_Linesty[VECSOLID] = SolidLine;
  D_Linesty[VECDOT] = (is_color ? DottedLine : SolidLine);
  D_Linesty[VECDASH] = (is_color ? DashedLine : SolidLine);
}

D_setcolors()
{
  Color c ;

  c.red = 256*0;
  c.green = 256*0;
  c.blue = 256*0;
  c.pixel = pixels[BLACK];
  XStoreColor(&c);

  c.red = 256*255;
  c.green = 256*255;
  c.blue = 256*255;
  c.pixel = pixels[WHITE];
  XStoreColor(&c);

  c.red = 256*255;
  c.green = 256*0;
  c.blue = 256*0;
  c.pixel = pixels[RED];
  XStoreColor(&c);

  c.red = 256*255;
  c.green = 256*100;
  c.blue = 256*0;
  c.pixel = pixels[ORANGE];
  XStoreColor(&c);

  c.red = 256*255;
  c.green = 256*255;
  c.blue = 256*0;
  c.pixel = pixels[YELLOW];
  XStoreColor(&c);

  c.red = 256*0;
  c.green = 256*0;
  c.blue = 256*255;
  c.pixel = pixels[GREEN];
  XStoreColor(&c);

  c.red = 256*0;
  c.green = 256*255;
  c.blue = 256*255;
  c.pixel = pixels[CYAN];
  XStoreColor(&c);

  c.red = 256*0;
  c.green = 256*0;
  c.blue = 256*255;
  c.pixel = pixels[BLUE];
  XStoreColor(&c);

  c.red = 256*255;
  c.green = 256*0;
  c.blue = 256*255;
  c.pixel = pixels[VIOLET];
  XStoreColor(&c);

  c.red = 256*130;
  c.green = 256*130;
  c.blue = 256*255;
  c.pixel = pixels[BLUE_WHITE];
  XStoreColor(&c);
  XFlush();
}


