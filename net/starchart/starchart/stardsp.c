/*
 * TTY Display driver for starchart.c mainline
 */

/*
 ! patched December, 1987 by Alan Paeth (awpaeth@watcgl),
 !
 ! [1] formal/actual parmaters for drawNebu, drawPlan.. now agree
 ! [2] "bigmaster" chart layout now added
 !
 * Modified for starchart 3.0.  Craig Counterman Jan, 1989
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/stardsp.c,v 1.1 1990-03-30 16:39:09 vrs Exp $";

#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>
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

/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail,*/
double th_smul=THSMUL;
double th_smin=THSMIN;
double th_madj=THMADJ;
double th_mmax=THMMAX;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define ROWS 32L
#define COLS 79L

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

int oldx, oldy, scrx, scry; /* Must be 32 bits */
char **linevec;
char *calloc();


/* Open the device */
D_open()
{
  int i;

  linevec = (char**)(calloc((unsigned)ROWS, sizeof(char*)));
  if (linevec == NULL) return FALSE;
  for (i=0; i<ROWS; i++) {
    linevec[i] = (char*)(calloc((unsigned)COLS, sizeof(char)));
    if (linevec[i] == NULL) return FALSE;
  }

  return TRUE ;				/* open successful */
}


/* Close the device */
D_close()
{
  int i, j, k;
  char c;

  for (i=0; i<ROWS; i++)
    {
      for (j=COLS-1; j>0; j--) if (linevec[i][j]) break;
      for (k=0; k<=j; k++) putchar((c=linevec[i][k]) ? c : ' ');
      putchar('\n');
      free((char *)linevec[i]);
    }
  fflush(stdout);
  free((char *)linevec);
}


/* Move to (x, y) */
D_move(x, y)
     int x, y;
{
  oldx = x;
  oldy = y;
  scrx = (x*COLS+512L)/1024L;
  scry = (((768L-y)*ROWS)+384L)/768L;
}


/* Draw a line of style line_style from the current point to (x, y) */
/* Note, this replaces vecdraw vecdrawdot and vecdrawhyph */
D_draw(x, y, line_style)
     int x, y;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
  int dx, dy, savex, savey, i, steps;
  char c;

  /* all styles are the same */
  savex = oldx;
  savey = oldy;
  dx = x-oldx;
  dy = y-oldy;
  c = (abs(dx) > abs(dy)) ? '-' : '|';
  steps = MAX(MAX(abs(dx),abs(dy))/12, 1);
  for(i=0; i<=steps; i++)
    {
      D_move(savex+(i*dx+steps/2)/steps,savey+(i*dy+steps/2)/steps);
      sym(c);
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
  case 'r':
  case 'g':
  case 'b':
  case 'y':
  case 'c':
  case 'p':
  case 'o':
  case 'w':
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
  char c;

  D_move(x, y);

  if (star_lbl) {
    /* remove leading spaces */
    while (*str == ' ') str++;
    /* can't display greek characters */
  }

  while(c = *str++)
    {
      sym(c);
      scrx++;
    }
}


char char_overwrite();

sym(c)
     char c;
{
  if ( (scrx >= 0) && (scrx < COLS) &&
      (scry >= 0) && (scry < ROWS) )
    linevec[scry][scrx] = char_overwrite(linevec[scry][scrx], c );
}


char char_overwrite(under, over)
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

/* Point sizes for font calls */
#define TITLESIZE 16
#define SUBTLSIZE 12
#define NAMESIZE 10
#define LBLSIZE 8
#define MAGSIZE 8

/* Fonts for font calls */
#define NAMEFNT TIMESROMAN
#define LBLFNT HELV
#define MAGFNT COURIER
#define TITLEFNT TIMESBOLD
#define SUBTLFNT TIMESROMAN

int x_nameoffset = 10, y_nameoffset = 0;
int x_lbloffset = 0, y_lbloffset = 10;
int x_magoffset = 7, y_magoffset = -15;


/* Interface Function */
/* Draw object at x, y.  properties set by other parameters */
drawobj(x, y, mag, type, color_str, label_field, con_str, obj_name,
	comment_str, file_line,
	draw_glyph, draw_text, use_lbl, use_name, use_mag)

     int x, y;
     double mag;	/* Magnitude of object */
     char *type;	/* 2 chars, object code and subcode */
     char *color_str;	/* 2 chars, spectral type for stars, 
			   color code otherwise */
     char *label_field;	/* 2 chars, Bayer or flamsteed for stars,
			   size in seconds for nebulae and
			   planets */
     char *con_str;	/* 3 chars, the constellation the object is in */
     char *obj_name;	/* Name of object */
     char *comment_str;	/* Comment field */
     char *file_line;	/* The full line from the file,
			   containing the above if it is in
			   standard format */
     int draw_glyph;	/* Draw object symbol */
     int draw_text;	/* Draw text */
     int use_lbl;	/* Label object with the label_field string */
     int use_name;	/* Label object with the obj_name string */
     int use_mag;	/* Label object with a 2 or 3 character string
			   containing the magnitude * 10 without
			   decimal point */
{
  char magstr[10];

/*fprintf(stderr, "%d %d %f <%s> <%s> <%s> <%s> <%s> <%s> <%s>\n", x, y, mag,
	type, color_str, label_field, con_str, obj_name, comment_str,
	file_line);*/

  if (draw_glyph)
    switch(type[0]) {
    case 'S':	drawStar(x,y,mag,type[1],color_str);
      break;
    case 'P':	drawPlan(x,y,mag,type[1],color_str, size_obj(label_field),
			 comment_str);
      break;
    case 'N':	drawNebu(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'G':	drawGalx(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'C':	drawClus(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'U':
      drawUnknown(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'O':
      drawOther(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'V':
    case 'A':
    case 'I':
      break;
    case '#':
    default:
      break;
    };


/*
 * use name or label
 */
  if (draw_text) {
    if (type[0] == 'I')
      D_color(color_str);
    else
      D_color("  ");

    if (use_name && obj_name[0]) {
      D_fontsize(NAMESIZE, NAMEFNT);
      D_text(x+x_nameoffset, y+y_nameoffset, obj_name, FALSE);
    } else if (use_lbl &&
	       ((label_field[0] != ' ') || (label_field[1] != ' '))) {
      D_fontsize(LBLSIZE, LBLFNT);
      D_text(x+x_lbloffset, y+y_lbloffset, label_field, TRUE);
    }
/* If you want to mag label other objects, change this */
    if (use_mag && (type[0] == 'S')) {
      sprintf(magstr, "%02d", (int)(mag*10.0+0.5));
      D_fontsize(MAGSIZE, MAGFNT);
      D_text(x+x_magoffset, y+y_magoffset, magstr, FALSE);
    }
  }
}

drawStar(x, y, mag, type, color)
     int x, y;
     double mag;
     char type, *color;
{
  switch ((int)(mag+0.5))
    {
    case -1:
    case  0:
      D_text(x, y, "O", FALSE);
      break;
    case  1:
      D_text(x, y, "*", FALSE);
      break;
    case  2:
      D_text(x, y, "o", FALSE);
      break;
    case  3:
      D_text(x, y, "-", FALSE);
      break;
    case  4:
      D_text(x, y, ",", FALSE);
      break;
    default:
      D_text(x, y, ".", FALSE);
      break;
    }
}

drawPlan(x, y, mag, pcode, color, plansize)
     int x,y;
     double mag;
     char pcode, *color;
     long plansize;

{
  D_text(x, y, "+", FALSE);
}

drawGalx(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  D_text(x, y, "@", FALSE);
}

drawClus(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  D_text(x, y, "%", FALSE);
}

drawNebu(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  D_text(x, y, "~", FALSE);
}

drawUnknown(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  D_text(x, y, "?", FALSE);
}

drawOther(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  D_text(x, y, "#", FALSE);
}

/* Functions for areas, drawn as lines for now */
/* Move to (x, y) to begin an area */
D_areamove(x, y)
     int x, y;
{
  D_move(x, y);
}

/* Add a segment to the area border */
D_areaadd(x, y)
     int x, y;
{
  D_draw(x, y, SOLID);
}

/* Fill the area, after adding the last segment */
D_areafill(x, y)
     int x, y;
{
  D_draw(x, y, SOLID);
}


/* Variables of positioning are here */
#define L_TIL 220
#define L_STIL 185

#define L_LMAR1 40
#define L_LMAR2 65
#define L_LTEXT 95
#define L_RMAR1 205
#define L_RMAR2 230
#define L_RTEXT 260

#define L_LINE1 150
#define L_LINE2 125
#define L_LINE3 100
#define L_LINE4 75
#define L_LINE5 50
#define L_LINE6 25


chartlegend(win)
     mapwindow *win;
{
  char ras[20], dls[20], outstr[40];
  if (!title[0]) title = "LEGEND";
  rastr(ras, win->racen);
  declstr(dls, win->dlcen);

  if (win->map_type != FULLPAGEMAP) {
    sprintf(outstr, "(%s,%s lim: %2.1f)", ras, dls, win->maglim);
    D_fontsize(TITLESIZE, TITLEFNT); D_text(L_LMAR1, L_TIL, title, FALSE);
    D_fontsize(SUBTLSIZE, SUBTLFNT); D_text(L_LMAR1, L_STIL, outstr, FALSE);
    
    drawStar(L_LMAR2, L_LINE1, 0.0, 'S', "  ");
    D_fontsize(12, NAMEFNT);
    D_text(L_LTEXT, L_LINE1,"<0.5", FALSE);
    if (win->maglim >= 0.5)
      {
	drawStar(L_RMAR2, L_LINE1, 1.0, 'S', "  ");
	D_fontsize(10, NAMEFNT);
	D_text( L_RTEXT, L_LINE1,"<1.5", FALSE);
      }
    if (win->maglim >= 1.5)
      {
	drawStar(L_LMAR2, L_LINE2, 2.0, 'S', "  ");
	D_fontsize( 9, NAMEFNT);
	D_text(L_LTEXT, L_LINE2,"<2.5", FALSE);
      }
    if (win->maglim >= 2.5)
      {
	drawStar(L_RMAR2, L_LINE2, 3.0, 'S', "  ");
	D_fontsize( 8, NAMEFNT);
	D_text(L_RTEXT, L_LINE2,"<3.5", FALSE);
      }
    if (win->maglim >= 3.5)
      {
	drawStar(L_LMAR2, L_LINE3, 4.0, 'S', "  ");
	D_fontsize( 7, NAMEFNT);
	D_text(L_LTEXT, L_LINE3,"<4.5", FALSE);
      }
    if (win->maglim > 4.5)
      {
	drawStar(L_RMAR2, L_LINE3, 5.0, 'S', "  ");
	D_fontsize( 6, NAMEFNT);
	D_text(L_RTEXT, L_LINE3,">4.5", FALSE);
      }
    
    D_fontsize(10, NAMEFNT);
    D_text(L_LTEXT,L_LINE4,"double", FALSE);
    drawStar(L_LMAR2,L_LINE4,2.0, 'D', "  ");
    D_fontsize(10, NAMEFNT);
    D_text(L_RTEXT,L_LINE4,"variable",FALSE);
    drawStar(L_RMAR2,L_LINE4,2.0, 'V', "  ");

    D_fontsize(10, NAMEFNT);
    D_text(L_LTEXT,L_LINE5,"planet", FALSE);
    drawPlan(L_LMAR2,L_LINE5,1.0, ' ', "  ", (long) -1);

    D_fontsize(10, NAMEFNT);
    D_text(L_RTEXT,L_LINE5,"galaxy", FALSE);
    drawGalx(L_RMAR2,L_LINE5,1.0, 'E', "  ", (long) -1);
    drawGalx(L_RMAR1,L_LINE5,1.0, 'S', "  ", (long) -1);

    D_fontsize(10, NAMEFNT);
    D_text(L_LTEXT,L_LINE6,"nebula", FALSE);
    drawNebu(L_LMAR2,L_LINE6,1.0, 'D', "  ", (long) -1);
    drawNebu( L_LMAR1,L_LINE6,1.0, 'P', "  ", (long) -1);

    D_fontsize(10, NAMEFNT);
    D_text(L_RTEXT,L_LINE6,"cluster", FALSE);
    drawClus(L_RMAR2,L_LINE6,1.0, 'O', "  ", (long) -1);
    drawClus(L_RMAR1,L_LINE6,1.0, 'G', "  ", (long) -1);
  } else {
    D_fontsize(8, NAMEFNT);

    sprintf(outstr, "%s: %s,%s lim: %2.1f", title, ras, dls, win->maglim);
    D_text(15, 15, outstr, FALSE);
  }
}



