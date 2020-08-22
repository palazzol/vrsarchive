/*
 * IBM PC driver for starchart 3.0 by Tom Horsley (tahorsley@ssd.harris.com)
 *
 * Uses the Microsoft C graphics library. I do not know just how compatible
 * this is with Turbo C since I have never seen Turbo C. The 5.1 Microsoft
 * compiler is supposed to have support for Hercules graphics, but this
 * is being developed under 5.0, so there is no Hercules support.
 *
 * Development was done with a IBM PC clone, Microsoft C 5.0,
 * a QuadEGA Prosync display card, and (thank goodness) an 8087
 * numeric co-processor.
 *
 * WARNING: A quick kludge was used to get a character set to display.
 * The BIOS ROM is read to get the 8 x 8 character bit map used when
 * the PC is in graphics mode. On really really standard PCs the
 * following code sets up the segment and offset of a far pointer
 * to the table in the BIOS rom for the bit map for a particular character.
 *
 *  FP_SEG(xtab) = 0xffa6 ;
 *  FP_OFF(xtab) = 0xe + (c * 8) ;
 *
 * If your PC is not extremely standard, your character font may not
 * be located at this address. If you know (or can find out) where it
 * is, change the address. If not, just delete the body of the routine
 * and live without labels.
 *
 * The Microsoft library is pretty compatible with U**x System V, so
 * compile everything with the SYSV flag set and you will be OK.
 *
 * The main glitch is the file "con.locs". You have absolutely GOT
 * to rename this file under DOS, otherwise it tries to talk to
 * the CONsole. I use "cons.loc", which is fairly close.
 * Renaming the built in file names is done by modifying pcstar.h.
 *
 * This driver is the code I wrote for starchart 2.1 wedged into the
 * sample driver provided with 3.0, then updated for 3.1.
 *
 * Compiling with DEBUG on dummies up the data in the videoconfig struct
 * to look like an EGA and causes the low level routines to write to a
 * debug file (debug.ibm) the low level graphics commands rather than
 * actually doing graphics. This makes life simpler when trying to debug
 * the program on the same display you are doing graphics on.
 *
 * Copyright (c) 1990 by Tom Horsley and Craig Counterman. All rights reserved.
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
 */

/*
 * $Header: /home/Vince/cvs/net/starchart/starchart/staribm.c,v 1.1 1990-03-30 16:39:11 vrs Exp $
 */


#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>

#include <graph.h>
#include <dos.h>

#include "star3.h"

/* Externs */

struct videoconfig vc
#ifdef DEBUG
 = {640,350,80,48,8,4,1,0,0,0,256}
#endif
;
int yscale1 ;
long yscale2 ;
long xscale1 ;
int vmodes[] = {
   _VRES16COLOR,
   _VRES2COLOR,
   _ERESCOLOR,
   _ERESNOCOLOR,
   _HRES16COLOR,
   _HRESBW,
   _MRES16COLOR,
   _MRES4COLOR,
   _MRESNOCOLOR
} ;
#define numodes (sizeof(vmodes)/sizeof(int))
char * modenames[] = {
   "VRES16COLOR",
   "VRES2COLOR",
   "ERESCOLOR",
   "ERESNOCOLOR",
   "HRES16COLOR",
   "HRESBW",
   "MRES16COLOR",
   "MRES4COLOR",
   "MRESNOCOLOR",
   NULL
} ;
int control_mode = -1;
int yrad[7] = {7, 6, 5, 4, 3, 2, 1} ;
int xrad[7] = {7, 6, 5, 4, 3, 2, 1} ;

int draw_blue ;
int draw_white ;
int draw_yellow ;
int draw_red ;
int draw_green ;

static int xlast,ylast ;
#define scx(x) ((int)((((long)(x))*xscale1)/1000L))
#define scy(y) (yscale1-((int)((((long)(y))*yscale2)/10000L)))

extern int g_argc;
extern char **g_argv;

extern char *title;	/* Title of page */

extern mapwindow *mapwin[MAXWINDOWS];
extern int numwins;

extern int cur_function;
extern int cur_map_type;
extern int cur_map_tag;
extern char *cur_tag_field;

/* Set by initxform
   One could use elements of the mapwindow structure,
   but these should be faster for the current window */
extern int xproj_mode;
extern double xwest, xeast, xnorth, xsouth, xbot;
extern int cenx, ceny, strty;
extern double xracen, sindlcen, cosdlcen, chart_scale;
extern double yscale;
extern double xc_scale;
extern double inv_;


/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail,*/
#define THSMUL 1.2
#define THSMIN 10.0
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

/*
 * Generic Star Drawing Stuff
 */

#ifdef DEBUG
FILE * dbgfile ;
#undef _selectpalette
#undef _remappalette
#undef _setcolor
#undef _setvideomode
#undef _moveto
#undef _setlinestyle
#undef _lineto
#define _selectpalette(x)
#define _remappalette(x,y)
#define _setcolor(x)
#define _setvideomode(x)
#define _moveto(x,y) fprintf(dbgfile,"_moveto(%d,%d)\n",x,y)
#define _setlinestyle(x)
#define _lineto(x,y) fprintf(dbgfile,"_lineto(%d,%d)\n",x,y)
#endif

/* Override graphics mode */
D_control_arg(s)
char *s;
{
   int   arglen = strlen(s);
   int   i;

   for (i = 0; i < numodes; ++i) {
      if (strnicmp(s, modenames[i], arglen) == 0) {
         control_mode = i;
         fprintf(stderr, "Will attempt to use %s graphics mode.\n",
            modenames[i]);
         break;
      }
   }
}

extern int readstar();
extern int (*readfile)();

/* Open the device */
int
D_open()
{
   int i ;
   float aspect ;
   int limit ;

   readfile = readstar; /* No memory to spare on PC */
#ifndef DEBUG
   if (control_mode == -1) {
      limit = numodes ;
      i = 0 ;
   } else {
      i = control_mode;
      limit = control_mode+1;
   }
   for (; i < limit; ++i) {
      if (_setvideomode(vmodes[i]) != 0) {
         _getvideoconfig(&vc) ;
#else
         i = 2 ;
         dbgfile = fopen("debug.ibm","w") ;
#endif
         switch(vmodes[i]) {
         case _MRES4COLOR :
            _selectpalette(2) ;
            draw_blue = 3 ;
            draw_white = 3 ;
            draw_yellow = 3 ;
            draw_red = 2 ;
            draw_green = 1 ;
            break ;
         case _MRESNOCOLOR :
            _selectpalette(1) ;
            draw_blue = 3 ;
            draw_white = 3 ;
            draw_yellow = 3 ;
            draw_red = 2 ;
            draw_green = 1 ;
            break ;
         case _MRES16COLOR :
         case _HRES16COLOR :
         case _ERESCOLOR :
         case _VRES2COLOR :
         case _VRES16COLOR :
         case _MRES256COLOR :
            _remappalette(1, _GREEN) ;
            _remappalette(2, _RED) ;
            _remappalette(3, _LIGHTYELLOW) ;
            _remappalette(4, _WHITE) ;
            _remappalette(5, _BLUE) ;
            draw_blue = 5 ;
            draw_white = 4 ;
            draw_yellow = 3 ;
            draw_red = 2 ;
            draw_green = 1 ;
            break ;
         default :
            draw_blue = 1 ;
            draw_white = 1 ;
            draw_yellow = 1 ;
            draw_red = 1 ;
            draw_green = 1 ;
         }
         _setcolor(draw_green) ;
         yscale1 = vc.numypixels - 1 ;
         yscale2 = (long)(13 * vc.numypixels) ;
         xscale1 = (long)(0.5 + 0.9765625 * vc.numxpixels) ;
         xlast = 0 ;
         ylast = yscale1 ;
         aspect = 1.3333333333 * (float)(vc.numypixels) /
            (float)(vc.numxpixels) ;
         for (i = 0; i < 7; ++i) {
            yrad[i] = (int)(aspect * (float)(yrad[i]) + 0.5) ;
            if (yrad[i] < 1) {
               yrad[i] = 1 ;
            }
         }
         return TRUE ;
#ifndef DEBUG
      }
   }
   printf("No graphics available!\n") ;
   return FALSE ;
#endif
}


/* Close the device */
D_close()
{
   /* wait for keystroke */
   getch() ;
   /* reset screen */
   _setvideomode(_DEFAULTMODE) ;
}


/* Move to (x, y) */
D_move(x, y)
   int x, y;
{
   _moveto(scx(x),scy(y)) ;
}


/* Draw a line of style line_style from the current point to (x, y) */
/* Note, this replaces vecdraw vecdrawdot and vecdrawhyph */
D_draw(x, y, line_style)
     int x, y;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
   unsigned short ls ;

   switch(line_style) {
   case SOLID :
   case VECSOLID :
      ls = 0xffff ;
      break ;
   case DOTTED :
   case VECDOT :
      ls = 0xaaaa ;
      break ;
   case DASHED :
   case VECDASH :
      ls = 0xf0f0 ;
   }
   _setlinestyle(ls) ;
   _setcolor(draw_green) ;
   _lineto(scx(x),scy(y)) ;
   _setlinestyle(0xffff) ;
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the line style drawn as desired */


/* Move to (x1, y1) then draw a line of style line_style to (x2, y2) */
D_movedraw(x1, y1, x2, y2, line_style)
     int x1, y1, x2, y2;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
   D_move(x1, y1) ;
   D_draw(x2, y2, line_style) ;
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
  case 'B':
    _setcolor(draw_blue) ;
    break;
  case 'A':
  case 'F':
    _setcolor(draw_white) ;
    break;
  case 'G':
  case 'K':
    _setcolor(draw_yellow) ;
    break;
  case 'M':
    _setcolor(draw_red) ;
    break;
  default:
    _setcolor(draw_green) ;
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


/*
   Fill in one character using the 8x8 character table that BIOS
   uses to write chars in graphics mode. Advance the xlast value
   by 8 bits.
*/
one_char(c)
   int c ;
{
   char far * xtab ;
   int i,j ;
   int x,y ;

   _setcolor(draw_green) ;
   if ((c < 0) || (c > 127)) {
      xlast += 8 ;
      return ;
   }
   y = ylast - 4 ;
   FP_SEG(xtab) = 0xffa6 ;
   FP_OFF(xtab) = 0xe + (c * 8) ;
   for (i = 0; i < 8; ++i) {
      x = xlast + 7 ;
      c = *xtab++ ;
      for (j = 0; j < 8; ++j) {
         if ((c & 1) &&
             ((x > 0) && (x < vc.numxpixels)) &&
             ((y > 0) && (y < vc.numypixels))) {
            _setpixel(x, y) ;
         }
         c >>= 1 ;
         --x ;
      }
      ++y ;
   }
   xlast += 8 ;
}

/* Display text string str at x,y, in current font and font size, using
   greek characters (if possible) if gk_flag is TRUE */
D_text(x, y, str, gk_flag)
     int x, y;
     char *str;
     int gk_flag;
{
   int c ;

   xlast = scx(x) ;
   ylast = scy(y) ;
   if (! gk_flag) {
      while ((c = *str++) != '\0') {
         one_char(c) ;
      }
   }
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
{
    int x1, x2, y1 ;

    x1 = x + dx*2 ;
    y1 = y + dy*2 ;
    x2 = x1 + len*2 - 1 ;
    x1 = scx(x1) ;
    y1 = scy(y1) ;
    x2 = scx(x2) ;
    _moveto(x1, y1) ;
    _lineto(x2, y1) ;
}

