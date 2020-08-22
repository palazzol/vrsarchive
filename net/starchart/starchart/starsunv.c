/*
 * Sunview driver for starchart.c 3.0 6/15/89
 *
 * Driver creates sunview window and draws starchart information inside.
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


static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starsunv.c,v 1.1 1990-03-30 16:39:25 vrs Exp $";

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "star3.h"

#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif

#include <suntool/sunview.h>
#include <suntool/canvas.h>

char *malloc();



#ifndef INTERACTIVE_CONTROL
char prog[] = "Starsunv";
#else
extern char prog[];
#endif

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

extern char *title;

extern mapwindow *mapwin[MAXWINDOWS];
extern int numwins;

extern int cur_function;
extern int cur_map_type;
extern int cur_map_tag;
extern char *cur_tag_field;


/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail */
#define THSMUL 1.2
#define THSMIN 12.0
#define THMADJ 2.5
#define THMMAX 8.0

mapwindow fullpage = {920, 700, 20, 48,
                      8.0, 2.0, 2.05, FULLPAGEMAP, 0, "String",
                      SANSONS, FALSE, FALSE, 0.5, 5.0, 0.0, 0.0, FALSE,};
mapwindow mainmap = {920, 480, 20, 268,
                     8.0, 3.0, 2.05, MAINMAP, 0, "String", 
                     SANSONS, FALSE, FALSE, 0.5, 5.0, 0.0, 0.0, FALSE,};
mapwindow thumbmap = {480, 195, 420, 35, 6.0+THMADJ, 1.0+THMADJ, 2.05+THMADJ,
                     THUMBNAIL, 0, "String", SANSONS, FALSE, FALSE, 0.5, 5.0,
                     0.0, 0.0, FALSE,};

int htick_lim=2;
int htext_lim = 80;
int htext_xoff = 2;
int htext_yoff = 17;
int vtick_lim = 2;
int vtext_lim = 20;
int vtext_xoff = 24;
int vtext_yoff = 0;

/* externs for labels */
int x_nameoffset = 10, y_nameoffset = 0;
int x_lbloffset = 7, y_lbloffset = 0;
int x_magoffset = 7, y_magoffset = -10;

/* externs for legend: variables of positioning are here */
int l_til=105;
int l_stil=85;

int l_lmar1=25;
int l_lmar2=50;
int l_ltext=75;
int l_rmar1=150;
int l_rmar2=175;
int l_rtext=200;

int l_line1=72;
int l_line2=60;
int l_line3=47;
int l_line4=35;
int l_line5=22;
int l_line6=10;

/* Point sizes for font calls */
int titlesize=18;
int subtlsize=12;
int namesize=10;
int lblsize=10;
int magsize=8;

/* Fonts for font calls */
int namefnt=TIMESROMAN;
int lblfnt=TIMESROMAN;
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


/* Font names */
static char *default_font_name = "/usr/lib/fonts/fixedwidthfonts/screen.r.12";
static char *greek_font_name = "/usr/lib/fonts/fixedwidthfonts/apl.r.10";
static char *timesroman_font_name = "/usr/lib/fonts/fixedwidthfonts/serif.r.10";
static char *timesbold_font_name = "/usr/lib/fonts/fixedwidthfonts/serif.r.10";
static char *timesital_font_name = "/usr/lib/fonts/fixedwidthfonts/serif.r.10";
static char *timesboldital_font_name = "/usr/lib/fonts/fixedwidthfonts/serif.r.10";
static char *helv_font_name = "/usr/lib/fonts/fixedwidthfonts/gacha.r.8";
static char *helvbold_font_name = "/usr/lib/fonts/fixedwidthfonts/gacha.b.8";
static char *helvital_font_name = "/usr/lib/fonts/fixedwidthfonts/gacha.r.8";
static char *helvboldital_font_name = "/usr/lib/fonts/fixedwidthfonts/gachs.b.8";
static char *courier_font_name = "/usr/lib/fonts/fixedwidthfonts/cour.r.10";
static char *courital_font_name = "/usr/lib/fonts/fixedwidthfonts/cour.r.10";
static char *courbold_font_name = "/usr/lib/fonts/fixedwidthfonts/cour.b.10";
static char *couritalbold_font_name = "/usr/lib/fonts/fixedwidthfonts/cour.b.10";


/* Sunview things */
Frame frame;
Canvas canvas;
Pixwin *pw;
Pr_brush pr_br = { 0 };
Pr_texture dash_tex;
short dash_patts[][3] = {
  {1, 1, 0},
  {2, 2, 0},
  {3, 3, 0},
  {4, 4, 0},
  {5, 5, 0},
  {6, 6, 0},
  {7, 7, 0},
  {8, 8, 0},
  {9, 9, 0},
  {10, 10, 0}
};
static Pixfont *greek_pfont;

int is_color = TRUE;		/* Is it color? */
int is_super_color = TRUE;	/* Is it many color? */
int ncolors, star_colors;
unsigned int width, height;
int draw_color = 1;

/* Device control argument */
D_control_arg(s)
char *s;
{
  int i = 0;
  int c;

  while (c = s[i++]) switch (c) {
  case 'm': /* monochrome display */
    is_color = FALSE;
  default:
    break;
  }
}


#ifndef INTERACTIVE_CONTROL

void
mouse_event_proc(window, event, arg)
Window window;
Event *event;
caddr_t arg;
{
  int x, y;
  int i;
  double lat, lon, tlat, tlon;
  char outstr[81];
  int ra_h, ra_m, ra_s;
  int de_d, de_m, de_s;
  char dsign;

  if (event_is_up(event))
    return;

  x = event_x(event);
  y = event_y(event);

  switch(event_action(event)) {
  case MS_LEFT:
  case MS_RIGHT:
    if ((i = invxform(x, height - y, &lat, &lon)) >= 0) {
      if (lon < 0.0) lon += 360.0;
      tlon = lon / 15.0;

      ra_h = tlon;
      ra_m = (tlon - ra_h) * 60 + (0.5 / 60);
      ra_s = (((tlon - ra_h) * 60) - ra_m) * 60 + 0.5;

      if (ra_s >= 60) {ra_s -= 60; ra_m++;};
      if (ra_m >= 60) {ra_m -= 60; ra_h++;};


      if (lat < 0.0) {
	tlat = -lat;
	dsign = '-';
      } else {
	tlat = lat;
	dsign = '+';
      }

      de_d = tlat;
      de_m = (tlat - de_d) * 60 + (0.5 / 60);
      de_s = (((tlat - de_d) * 60) - de_m) * 60 + 0.5;

      if (de_s >= 60) {de_s -= 60; de_m++;};
      if (de_m >= 60) {de_m -= 60; de_d++;};


      sprintf(outstr, "RA: %2dh%2dm%2ds   dec: %c%2dd%2dm%2ds",
	      ra_h, ra_m, ra_s, dsign, de_d, de_m, de_s);
      D_comment(outstr);
      if (event_action(event) == MS_RIGHT)
	dblookup(i, lat, lon, 10);
    }
    break;
  default:
    break;
  };
}


/* Open the device */
D_open()
{
  unsigned int map_width;
  double legend_spacing;

  width = 650; height = 768;

  /* Create frame and canvas */
  frame = window_create(NULL, FRAME, FRAME_LABEL, "Hello there",
			WIN_HEIGHT, height + 30,
			WIN_WIDTH, width + 20,
			0);
  if (!(frame))
    return FALSE;

  canvas = window_create(frame, CANVAS,
			 WIN_HEIGHT, height,
			 WIN_WIDTH, width,
			 WIN_EVENT_PROC, mouse_event_proc,
			 0);


  window_set(canvas,
	     CANVAS_AUTO_SHRINK, FALSE,
	     CANVAS_AUTO_EXPAND, FALSE,
	     0);

  /* get the canvas pixwin to draw into */
  pw = canvas_pixwin(canvas);

  is_color &= (pw->pw_pixrect->pr_depth >= 4);
  is_super_color = is_color & (pw->pw_pixrect->pr_depth >= 6);
  ncolors = (is_color ? 12 : 2);
  star_colors = (is_super_color ? 65: 0);

  D_setcolors();

  map_width = width - 75;

  fullpage.width = map_width;
  fullpage.height = height - 50;
  fullpage.x_offset = 25;
  fullpage.y_offset = 25;

  mainmap.width = map_width;
  mainmap.height = ((height -25 - 25 - 25)*3)/4;
  mainmap.x_offset = 25;
  mainmap.y_offset = 25 + (height -25 - 25 - 25)/4 + 25;

  thumbmap.width = map_width/2;
  thumbmap.height = (height -25 - 25 - 25)/4;
  thumbmap.x_offset = 25 + map_width/2;
  thumbmap.y_offset = 25;

  /* Need to adjust label here */
  /* Want top of l_til to be at top of thumbmap */
  l_til = thumbmap.height + thumbmap.y_offset - titlesize;
  /* Space text in legends to fill space.  Leave 10 on bottom.
     Text totals 6*namesize+subtlsize. */

  legend_spacing = (l_til - 10 - 6*namesize - subtlsize)/7;
  if (legend_spacing < 2) legend_spacing = 2;

  l_stil = l_til - subtlsize - legend_spacing;
  l_line1 = l_stil - namesize - legend_spacing;
  l_line2 = l_line1 - namesize - legend_spacing;
  l_line3 = l_line2 - namesize - legend_spacing;
  l_line4 = l_line3 - namesize - legend_spacing;
  l_line5 = l_line4 - namesize - legend_spacing;
  l_line6 = l_line5 - namesize - legend_spacing;

  greek_pfont = pf_open(greek_font_name);

  
  /* Clear to black */
  {
    int i;
    /* I don't underatanmd pw_polygon_2, so won't use it */
/*
    for (i = 0; i < width; i++) {
      pw_vector(pw, i, 0, i, 30, PIX_SRC, 0);
      pw_vector(pw, i, 30, i, 60, PIX_SRC, 1);
      pw_vector(pw, i, 60, i, 90, PIX_SRC, 2);
      pw_vector(pw, i, 90, i, 120, PIX_SRC, 3);
      pw_vector(pw, i, 120, i, 150, PIX_SRC, 4);
      pw_vector(pw, i, 150, i, 180, PIX_SRC, 5);
      pw_vector(pw, i, 180, i, 210, PIX_SRC, 6);
      pw_vector(pw, i, 210, i, 240, PIX_SRC, 7);
      pw_vector(pw, i, 240, i, 270, PIX_SRC, 8);
      pw_vector(pw, i, 270, i, 300, PIX_SRC, 9);
      pw_vector(pw, i, 300, i, 330, PIX_SRC, 10);
      pw_vector(pw, i, 330, i, 360, PIX_SRC, 11);
      pw_vector(pw, i, 360, i, 390, PIX_SRC, 12);
      pw_vector(pw, i, 390, i, 420, PIX_SRC, 13);
      pw_vector(pw, i, 420, i, 450, PIX_SRC, 14);
      pw_vector(pw, i, 450, i, 480, PIX_SRC, 15);
    }
*/
  }

  return TRUE;				/* open successful */
}

/* Close the device */
D_close()
{
  window_main_loop(frame);
  exit(1);   
}

#endif /* not INTERACTIVE_CONTROL */


/* Readstar globals */
extern double obj_lat, obj_lon, obj_mag;
extern char obj_type[3], obj_color[3], obj_label[3];
extern char obj_constell[3], obj_name[];
extern char *obj_commnt, fileline[];

/* needed by readstar */
int (*readfile)();
extern char *cur_file_name;

dblookup(winnum, lat, lon, dpix)
     int winnum;
     double lat, lon;
     int dpix;
{
  int i;
  double delta;
  double f_east, f_north, f_west, f_south;
  FILE *file1, *file2;
  char ibuf[2020], fname[1030], ftypestr[32];
  int n;
  int ftype;

  D_comment("Looking for object");
  
  delta = mapwin[winnum]->c_scale * dpix;

  for (i = 0; i < mapwin[winnum]->numfiles; i++) {
    if ((file1 = fopen(mapwin[winnum]->file[i].name, "r")) == NULL) continue;

    cur_file_name = mapwin[winnum]->file[i].name;
    ftype = mapwin[winnum]->file[i].type;
    if (mapwin[winnum]->file[i].type == INDEXTYPE) {
      /* read each line of index file, parse data on the files indexed:
	 region_included file_name
         use an indexed file if the point is in the region */

      while (!feof(file1)) {
	fgets(ibuf, 2000, file1);
	n = sscanf(ibuf, "%lf %lf %lf %lf %1000s %30s\n", &f_east, &f_north,
		   &f_west, &f_south, fname, ftypestr);
	if ((n != 6) || feof(file1)) continue;

	f_east *= 15.0;
	f_west *= 15.0;

	if (westof(lon, f_east) && eastof(lon, f_west)
	    && (lat >= f_south) && (lat <= f_north)) {
	  if ((file2 = fopen(fname, "r")) == NULL) {
	    fprintf(stderr, "%s: error, couldn't open '%s' for reading.\n",
		    prog, fname);
	  } else {
	    cur_file_name = fname;

	    ftype = tr_fty(ftypestr);

	    if (ftype == INDEXTYPE)  {
	      fprintf(stderr,
  "%s: error, can't use type INDEXTYPE file '%s' from another index file.\n",
		      prog, fname);
	    } else {
	      do_look(file2, ftype, lat, lon, delta);
	    }
	    fclose(file2);
	    cur_file_name = "";
	  }
	}
      }
    } else {
      do_look(file1, ftype, lat, lon, delta);
    }

    fclose(file1);
  };

  D_comment("Done search");
}

do_look(file, ftype, lat, lon, delta)
FILE *file;
int ftype;
double lat, lon, delta;
{
  double dlat, dlon;
  double tlat, tlon;
  int ra_h, ra_m, ra_s;
  int de_d, de_m, de_s;
  char dsign;

  char outstr[81];


  while (!(*readfile)(file, ftype)) {
    dlat = fabs(obj_lat - lat);
    dlon = fabs(obj_lon - lon);
    if ((dlat <= delta) && (dlon <= delta)) {
      tlon = obj_lon/15.0;
      ra_h = tlon;
      ra_m = (tlon - ra_h) * 60 + (0.5 / 60);
      ra_s = (((tlon - ra_h) * 60) - ra_m) * 60 + 0.5;

      if (ra_s >= 60) {ra_s -= 60; ra_m++;};
      if (ra_m >= 60) {ra_m -= 60; ra_h++;};


      if (obj_lat < 0.0) {
	tlat = -obj_lat;
	dsign = '-';
      } else {
	tlat = obj_lat;
	dsign = '+';
      }

      de_d = tlat;
      de_m = (tlat - de_d) * 60 + (0.5 / 60);
      de_s = (((tlat - de_d) * 60) - de_m) * 60 + 0.5;

      if (de_s >= 60) {de_s -= 60; de_m++;};
      if (de_m >= 60) {de_m -= 60; de_d++;};



      sprintf(outstr,
      "%2dh%2dm%2ds %c%2dd%2dm%2ds %5.2f %2s %2s %2s %3s %-25.25s %-14.14s",
	      ra_h, ra_m, ra_s, dsign, de_d, de_m, de_s, obj_mag,
	      obj_type,  obj_color, obj_label, obj_constell,
	      obj_name, obj_commnt);
      D_comment(outstr);
    }
  }
}


static int current_x, current_y;

/* Move to (x, y) */
D_move(x, y)
     int x, y;
{
  current_x = x; current_y = y;
}


static int curr_lsty = -1;
/* Draw a line of style line_style from the current point to (x, y) */
D_draw(x, y, line_style)
     int x, y, line_style;
{
  int xline_sty;
  int dashes = 1;
  Pr_texture *texture;

  switch (cur_function) {
  case CHRTOUTLN:
  case CHRTHTICK:
  case CHRTVTICK:
    xline_sty = SOLID;
    dashes = 1;
    break;
  case GRID_RA:
  case GRID_DEC:
    xline_sty = DASHED;
    dashes = 4;
    break;
  case ECLIPT:
    xline_sty = DASHED;
    dashes = 1;
    break;
  case CONSTBOUND:
    xline_sty = DASHED;
    dashes = 6;
    break;
  case CONSTPATTRN:
    xline_sty = DASHED;
    dashes = 8;
    break;
  case CONSTNAME:
  case CHARTFILE:
  default:
    switch (line_style) {
    case SOLID:
    case VECSOLID:
      xline_sty = SOLID;
      dashes = 1;
      break;
    case DASHED:
    case VECDASH:
      xline_sty = DASHED;
      dashes = 5;
      break;
    case DOTTED:
    case VECDOT:
      xline_sty = DASHED;
      dashes = 1;
      break;
    default:
      xline_sty = SOLID;
      dashes = 1;
      break;
    };
    break;
  }

  if (xline_sty == SOLID)  {
    texture = NULL;
  } else {
    texture = &dash_tex;

    texture->pattern = dash_patts[dashes];
  }

  pw_line(pw, current_x, (int)(height - current_y),
	  x, (int)(height - y), &pr_br, texture,
	  PIX_SRC | PIX_COLOR(draw_color));


  curr_lsty = xline_sty;

  current_x = x; current_y = y;
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the line style drawn as desired */


/* Move to (x1, y1) then draw a line of style line_style to (x2, y2) */
D_movedraw(x1, y1, x2, y2, line_style)
     int x1, y1, x2, y2, line_style;
{
  D_move(x1, y1);
  D_draw(x2, y2, line_style);
}


static struct pr_pos areapts[1000];
static int nareapts;
/* Move to (x, y) to begin an area */
D_areamove(x, y)
     int x, y;
{
  nareapts = 0;
  areapts[nareapts].x = x;
  areapts[nareapts].y = height - y;
  nareapts++;
}

/* Add a segment to the area border */
D_areaadd(x, y)
     int x, y;
{
  areapts[nareapts].x = x;
  areapts[nareapts].y = height - y;
  nareapts++;
}

/* Fill the area, after adding the last segment */
D_areafill(x, y)
     int x, y;
{
  areapts[nareapts].x = x;
  areapts[nareapts].y = height - y;
  nareapts++;

  pw_polygon_2(pw, 0, 0, 1, &nareapts, areapts, PIX_SRC|PIX_COLOR(draw_color),
	  NULL, 0, 0);

  nareapts = 0;
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
  int colr;
  int class, subclass;
  static char *table = "OBAFGKMRNSrgbycpow";
  static int spectra[10] = {
    BLUE_WHITE, BLUE, WHITE, WHITE, YELLOW, ORANGE, RED, RED, RED};

  static int super_spectra[][10] = {
    /*         0   1   2   3   4   5   6   7   8   9   */
    /* O */ { 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
    /* B */ {  0,  1,  2,  3,  3,  4,  4,  4,  5,  6 },
    /* A */ {  7,  7,  8,  9,  9, 10, 10, 10, 10, 10 },
    /* F */ { 11, 11, 12, 12, 12, 13, 13, 13, 14, 14 },
    /* G */ { 15, 15, 15, 15, 15, 16, 16, 16, 16, 16 },
    /* K */ { 17, 17, 18, 18, 18, 19, 19, 19, 19, 19 },
    /* M */ { 20, 20, 21, 22, 23, 24, 25, 26, 26, 26 },
    /* R */ { 27, 27, 27, 28, 28, 28, 28, 28, 29, 30 },
    /* N */ { 27, 27, 27, 28, 28, 28, 28, 28, 29, 30 },
    /* S */ { 30, 30, 30, 30, 31, 31, 31, 31, 31, 31 },
    /* r */ { 33, 33, 33, 34, 34, 34, 35, 35, 35, 36 },
    /* g */ { 37, 37, 37, 38, 38, 38, 39, 39, 39, 40 },
    /* b */ { 41, 41, 41, 42, 42, 42, 43, 43, 43, 44 },
    /* y */ { 45, 45, 45, 46, 46, 46, 47, 47, 47, 48 },
    /* c */ { 49, 49, 49, 50, 50, 50, 51, 51, 51, 52 },
    /* p */ { 53, 53, 53, 54, 54, 54, 55, 55, 55, 56 },
    /* o */ { 57, 57, 57, 58, 58, 58, 59, 59, 59, 60 },
    /* w */ { 61, 61, 61, 62, 62, 62, 63, 63, 63, 64 }
	  };

  if (!is_color) return;

  class = 0;
  while (table[class] && (table[class] != color_str[0])) class++;

  if (is_super_color) {
    subclass = isdigit(color_str[1]) ? color_str[1] - '0' : 0;
    colr = table[class] ? (10+super_spectra[class][subclass]) : WHITE;
  } else
    colr = table[class] ? spectra[class] : WHITE;

  draw_color = colr;
}


static int current_font = -1, current_fsize = -1;
static Pixfont *curr_pfont;

/* Set the font and font size to be used for text. */
/* Note order of args */
D_fontsize(fsize, font)
     int font, fsize;
{
  char *fontname, readname[50];

  switch(cur_function) {
  case CHRTOUTLN:
    break;
  case CHRTHTICK:
  case CHRTVTICK:
    font = TIMESBOLD;
    break;
  case GRID_RA:
  case GRID_DEC:
  case ECLIPT:
    break;
  case CONSTBOUND:
  case CONSTPATTRN:
    break;
  case CONSTNAME:
    font = HELVITAL;
    break;
  case CHARTFILE:
  case CURNTFONT:
  default:
    break;
  }

  if ((font == current_font) && (fsize == current_fsize)) return;

  switch(font)
    {
    case TIMESROMAN: fontname = timesroman_font_name; break;
    case TIMESBOLD: fontname = timesbold_font_name; break;
    case TIMESITAL: fontname = timesital_font_name; break;
    case TIMESBOLDITAL: fontname = timesboldital_font_name; break;
    case HELV: fontname = helv_font_name; break;
    case HELVBOLD: fontname = helvbold_font_name; break;
    case HELVITAL: fontname = helvital_font_name; break;
    case HELVBOLDITAL: fontname = helvboldital_font_name; break;
    case COURIER: fontname = courier_font_name; break;
    case COURITAL: fontname = courital_font_name; break;
    case COURBOLD: fontname = courbold_font_name; break;
    case COURITALBOLD: fontname = couritalbold_font_name; break;
    }

  current_font = font;
  current_fsize = fsize;

  curr_pfont = pf_open(fontname);
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the font used as desired */


static char  *intable = " 0123456789abgdezh@iklmnEoprstuOx%w";
static char *outtable = " 0123456789abgdezhqiklmnxoprstujcyw";
/* Display text string str at x,y, in current font and font size.
   if star_lbl is TRUE, string is a star label, use
      greek characters (if possible) */
D_text(x, y, s, star_lbl)
     int x, y;
     char *s;
     int star_lbl;
{
  char ch;
  int i, j;
  int use_greek = FALSE;

  if (star_lbl) {
    if (isgreek(s[0]) && (isdigit(s[1]) || (s[1] == ' '))) {
      /* Greek if first character is greek encoded,
	 and the second is space or a digit */
      use_greek = TRUE;
#ifdef OLD_GREEK
      /* Translate yaleformat greek encoding to Symbol font encoding */
      i = 0;
      while (ch = s[i])
	{
	  j = 0;
	  while (intable[j] && (intable[j] != ch)) j++;
	  s[i] = intable[j] ? outtable[j] : s[i];
	  /* If not in translation table, leave it alone */
	  i++;
	}
#endif
    } else { /* Star label, but not greek */
	/* remove leading spaces */
	while (*s == ' ') s++;
    }
  }

  pw_ttext(pw, x, height - y, PIX_SRC|PIX_COLOR(draw_color),
	  use_greek ? greek_pfont : curr_pfont, s);
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
#ifndef INTERACTIVE_CONTROL
D_comment(str)
     char *str;
{
  fprintf(stdout, "%s\r\n", str);
}
#endif



/**
Higher level functions
**/

drawlen(x, y, dx, dy, len)
     int x,y, dx, dy, len;
{
  buildlines(x + dx, (int) (height - (y+dy)),
	     x+dx+len, (int)(height - (y+dy)));
}


D_setcolors()
{
  static int data[][3] = {
    {   0,   0,   0 },   /* BLACK */
    { 255, 255, 255 },   /* WHITE */
    { 255,   0,   0 },   /* RED */
    { 255, 100,   0 },   /* ORANGE */
    { 255, 255,   0 },   /* YELLOW */
    {   0, 255,   0 },   /* GREEN */
    {   0, 255, 255 },   /* CYAN */
    {   0,   0, 255 },   /* BLUE */
    { 255,   0, 255 },   /* VIOLET */
    { 130, 130, 255 },   /* BLUE_WHITE */
  };
  static double super_data[][3] = {
    /*  Red             Green          Blue    Name by which type is known */
    { 0.38937,        0.46526,        0.79493 },    /* B0 */
    { 0.39501,        0.47146,        0.78847 },    /* B1 */
    { 0.40103,        0.47792,        0.78151 },    /* B2 */
    { 0.40640,        0.48355,        0.77526 },    /* B3 */
    { 0.41341,        0.49071,        0.76701 },    /* B5 */
    { 0.43251,        0.50914,        0.74412 },    /* B8 */
    { 0.44342,        0.51897,        0.73079 },    /* B9 */
    { 0.45181,        0.52618,        0.72042 },    /* A0 */
    { 0.46931,        0.54026,        0.69847 },    /* A2 */
    { 0.47958,        0.54792,        0.68541 },    /* A3 */
    { 0.48538,        0.55205,        0.67797 },    /* A5 */
    { 0.50879,        0.56731,        0.64752 },    /* F0 */
    { 0.51732,        0.57231,        0.63627 },    /* F2 */
    { 0.52348,        0.57573,        0.62810 },    /* F5 */
    { 0.54076,        0.58447,        0.60496 },    /* F8 */
    { 0.54853,        0.58799,        0.59446 },    /* G0 */
    { 0.56951,        0.59623,        0.56584 },    /* G5 */
    { 0.58992,        0.60244,        0.53765 },    /* K0 */
    { 0.61098,        0.60693,        0.50828 },    /* K2 */
    { 0.63856,        0.60977,        0.46950 },    /* K5 */
    { 0.68698,        0.60595,        0.40110 },    /* M0 */
    { 0.72528,        0.59434,        0.34744 },    /* M2 */
    { 0.75182,        0.58144,        0.31097 },    /* M3 */
    { 0.78033,        0.56272,        0.27282 },    /* M4 */
    { 0.81066,        0.53676,        0.23394 },    /* M5 */
    { 0.84247,        0.50195,        0.19570 },    /* M6 */
    { 0.87512,        0.45667,        0.16004 },    /* M7 */
    { 0.71033,        0.59983,        0.36829 },    /* N0 */
    { 0.78625,        0.55816,        0.26507 },    /* N3 */
    { 0.93792,        0.33011,        0.10649 },    /* N8 */
    { 0.94897,        0.29906,        0.10012 },    /* N9 */
    { 0.79832,        0.54811,        0.24950 },    /* S4 */
    { 0.38241,        0.45743,        0.80282 },    /* O  */
  };
  static struct {u_char r, g, b} other_colrs[] = {
    { 0x80, 0, 0},
    { 0xc0, 0, 0},
    { 0xe0, 0, 0},
    { 0xff, 0, 0},
    { 0x0, 0x80, 0},
    { 0x0, 0xc0, 0},
    { 0x0, 0xe0, 0},
    { 0x0, 0xff, 0},
    { 0x0, 0x0, 0x80},
    { 0x0, 0x0, 0xc0},
    { 0x0, 0x0, 0xe0},
    { 0x0, 0x0, 0xff},
    { 0x80, 0x80, 0x0},
    { 0xc0, 0xc0, 0x0},
    { 0xe0, 0xe0, 0x0},
    { 0xff, 0xff, 0x0},
    { 0x0, 0x80, 0x80},
    { 0x0, 0xc0, 0xc0},
    { 0x0, 0xe0, 0xe0},
    { 0x0, 0xff, 0xff},
    { 0x80, 0x0, 0x80},
    { 0xc0, 0x0, 0xc0},
    { 0xe0, 0x0, 0xe0},
    { 0xff, 0x0, 0xff},
    { 0x80, 0x40, 0x00},
    { 0xc0, 0x60, 0x00},
    { 0xe0, 0x70, 0x00},
    { 0xff, 0x80, 0x00},
    { 0x80, 0x80, 0x80},
    { 0xc0, 0xc0, 0xc0},
    { 0xe0, 0xe0, 0xe0},
    { 0xff, 0xff, 0xff}
  };
  u_char red[255], green[255], blue[255];
  double max_colr;
  int i;
  static int plane_colors[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256};


  for (i=0; i< 256; i++) {
    red[i] = 0;
    green[i] = 0;
    blue[i] = 0;
  };

  for (i=0; i<=9; i++) {
    red[i] = data[i][0];
    green[i] = data[i][1];
    blue[i] = data[i][2];
  };

  if (is_super_color) {
    for (i=0; i<=32; i++) {
      max_colr = super_data[i][0];
      max_colr = super_data[i][1] > max_colr ? super_data[i][1] : max_colr;
      max_colr = super_data[i][2] > max_colr ? super_data[i][2] : max_colr;
      red[i+10] = (int) 255*super_data[i][0] / max_colr;
      green[i+10] = (int) 255*super_data[i][1] /max_colr;
      blue[i+10] = (int) 255*super_data[i][2] /max_colr;
    }

    for (i = 33; i <= 64; i++) {
      red[i+10] = other_colrs[i-33].r;
      green[i+10] = other_colrs[i-33].g;
      blue[i+10] = other_colrs[i-33].b;
    };
  }

  i = plane_colors[pw->pw_pixrect->pr_depth];
  red[i-1] = blue[i-1] = green[i-1] = 255;

  pw_setcmsname(pw, "starcolors");
  pw_putcolormap(pw, 0, plane_colors[pw->pw_pixrect->pr_depth],
		 red, green, blue);
}



#ifndef INTERACTIVE_CONTROL
buildlines(x1, y1, x2, y2)
     int x1, y1, x2, y2;
{
  pw_line(pw, x1, y1, x2, y2, &pr_br, NULL,
	  PIX_SRC | PIX_COLOR(draw_color));
}


#endif /* not INTERACTIVE_CONTROL */


