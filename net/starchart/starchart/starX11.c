/*
 * X11 driver for starchart.c 3.0 mainline 4/17/88
 *
 * Driver creates X11 window and draws starchart information inside.
 * Intially window size is variable, but it stays constant once it's
 * mapped.  It handles expose events, even while it's drawing.
 * Includes glyphs for variable stars.
 *
 *
 * Portions Copyright (c) 1989 by Joe Wang (joe@athena.mit.edu)
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starX11.c,v 1.1 1990-03-30 16:39:33 vrs Exp $";

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "star3.h"

#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif

#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

char *malloc();

/* X items */
Display *display;            /* connection to display server */
int screen_num;
Screen screen;
Window root, window;         /* window to graphics in */
Drawable draw_into;
Colormap default_cmap;       /* colormap */
GC default_GC;               /* graphics context */
XFontStruct *default_font;   /* default font */
XFontStruct *greek_font;     /* Greek font */
Pixmap backup;               /* backup for expose events */
Pixel black, white, foreground, background;
Pixel *pixels;		     /* color map cells */
Pixel *star_pixels;	     /* color map cells for super color stars */
int ncolors, star_colors;
XEvent event;		     /* event structure */
XGCValues values_GC;	     /* modify GC */
unsigned int width, height;

Boolean reverse;

Bool use_backup = FALSE;
Bool hide_drawing = FALSE;
Bool post_preview = FALSE;
Bool post_landscape = FALSE;
#ifdef USE_X_DASHES
Bool use_x_dashes = TRUE;
#else
Bool use_x_dashes = FALSE;
#endif
Bool is_color = TRUE;		/* Is it color? */
Bool is_super_color = TRUE;	/* Is it many color? */


int Xargc = 1;
char *Xargv[40];



#ifdef STARX11
char prog[] = "StarX11";
#else
extern char prog[];
#endif

#include "icon.h"

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
                      8.0, 3.0, 2.05, FULLPAGEMAP, 0, "String",
                      SANSONS, FALSE, FALSE, 7.5, 5.0, 0.0, 0.0, FALSE,};
mapwindow mainmap = {920, 480, 20, 268,
                     8.0, 3.0, 2.05, MAINMAP, 0, "String", 
                     SANSONS, FALSE, FALSE, 7.5, 5.0, 0.0, 0.0, FALSE,};
mapwindow thumbmap = {480, 195, 420, 35, 6.2+THMADJ, 1.0+THMADJ, 2.05+THMADJ,
                     THUMBNAIL, 0, "String", SANSONS, FALSE, FALSE, 7.5, 5.0,
                     0.0, 0.0, FALSE,};

int htick_lim = 2;
int htext_lim = 37;
int htext_xoff = 2;
int htext_yoff = 12;
int vtick_lim = 2;
int vtext_lim = 12;
int vtext_xoff = 16;
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
#ifdef RELEASE3_FONTS
static char *default_font_name = "*helvetica-medium-r-normal--10*";
static char *greek_font_name = "*symbol-medium-r-normal--10*";
static char *timesroman_font_name = "*times-medium-r-normal--";
static char *timesbold_font_name = "*times-bold-r-normal--";
static char *timesital_font_name = "*times-roman-i-normal--";
static char *timesboldital_font_name = "*times-bold-i-normal--";
static char *helv_font_name = "*helvetica-medium-r-normal--";
static char *helvbold_font_name = "*helvetica-bold-r-normal--";
static char *helvital_font_name = "*helvetica-medium-o-normal--";
static char *helvboldital_font_name = "*helvetica-bold-o-normal--";
static char *courier_font_name = "*courier-medium-r-normal--";
static char *courital_font_name = "*courier-medium-i-normal--";
static char *courbold_font_name = "*courier-bold-r-normal--";
static char *couritalbold_font_name = "*courier-bold-o-normal--";
#else
static char *default_font_name = "helvetica-medium10";
static char *greek_font_name = "symbol-medium10";
static char *timesroman_font_name = "times-roman";
static char *timesbold_font_name = "times-bold";
static char *timesital_font_name = "times-italic";
static char *timesboldital_font_name = "times-bold-italic";
static char *helv_font_name = "helvetica-medium";
static char *helvbold_font_name = "helvetica-bold";
static char *helvital_font_name = "helvetica-oblique";
static char *helvboldital_font_name = "helvetica-boldoblique";
static char *courier_font_name = "courier-medium";
static char *courital_font_name = "courier-medium";
static char *courbold_font_name = "courier-bold";
static char *couritalbold_font_name = "courier-bold";
#endif /* RELEASE3_FONTS */



/* Device control argument */
D_control_arg(s)
char *s;
{
  int i = 0;
  int c;
  int j;

  while (c = s[i++])
    switch (c) {
    case 'p': /* Postscript preview */
      post_preview = TRUE;
      break;
    case 'l': /* Postscript landscape */
      post_preview = TRUE;
      post_landscape = TRUE;
      break;
    case 'h': /* Hide */
      hide_drawing = use_backup = TRUE;
      break;
    case 'b': /* Use backup */
      use_backup = TRUE;
      break;
    case 'm': /* monochrome display */
      is_color = FALSE;
      break;
#ifdef STARXAW
    case 't': /* Toolkit */
      /* skip current argument */
      j = i-1;
      while (s[j]) {
	if (s[j] == ' ') {
	  s[j] = '\0';
	  if (Xargc < 39)
	    Xargv[Xargc++] = &s[j+1];
	};
	j++;
      };
      i = j-1;
      break;
#endif
    default:
      break;
    }
}


#ifdef STARX11
/* Open the device */
D_open()
{
  int x, y;
  unsigned int border, depth, map_width;
  Pixmap icon;
  XSizeHints sizehints;
  double legend_spacing;

  if (!(display = XOpenDisplay(NULL)))
    return FALSE;
   
  X_setdisplay();

  /* open window */
  window = XCreateSimpleWindow(display, RootWindow(display,screen_num), 12, 12,
                                 720, 540, 1, foreground, background);

  if (post_preview) {
    sizehints.flags = PSize|PMinSize|PMaxSize;
    if (post_landscape) {
      sizehints.width = sizehints.min_width = sizehints.max_width = 792;
      sizehints.height = sizehints.min_height = sizehints.max_height = 576;
    } else {
      sizehints.width = sizehints.min_width = sizehints.max_width = 576;
      sizehints.height = sizehints.min_height = sizehints.max_height = 792;
    };
  } else {
    sizehints.flags = PMinSize;
    sizehints.min_width = 550; sizehints.min_height = 550;
  }

  icon = XCreateBitmapFromData(display, window, icon_bits, 
                               icon_width, icon_height);
  Xargv[0] = g_argv[0];
  XSetStandardProperties(display, window, prog, prog, icon, Xargv, Xargc,
                         &sizehints);
  XDefineCursor(display, window, XCreateFontCursor(display, XC_crosshair));
  XSelectInput(display, window, StructureNotifyMask);
  XMapWindow(display, window);
  while(!XCheckTypedWindowEvent(display, window, MapNotify, &event));
  XSelectInput(display, window, ExposureMask);

  XGetGeometry(display, window, &root, &x, &y, &width, 
               &height, &border, &depth);
  sizehints.max_width = sizehints.min_width = width;
  sizehints.max_height = sizehints.min_height = height;


  map_width = width - 72;

  fullpage.width = map_width;
  fullpage.height = height - 72;
  fullpage.x_offset = 18;
  fullpage.y_offset = 36;

  /* main is 7/10 the printing space 
   */
  mainmap.width = map_width;
  mainmap.height = ((height - 72)*7)/10;
  mainmap.x_offset = 18;
  mainmap.y_offset = 18 + ((height - 72)*2.5)/10;

  /* Thumbscale is 2/10 the printing space */
  thumbmap.width = map_width/2;
  thumbmap.height = ((height - 72)*2)/10;
  thumbmap.x_offset = 18 + map_width/2;
  thumbmap.y_offset = 18;

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


  /* Actually, we left too much space above,
     for proper postscript simulation.
     We can make the maps higher */
  if (!post_preview) {
    fullpage.height = height - fullpage.y_offset - 18;
    mainmap.height = height - mainmap.y_offset - 18;
  }


  sizehints.flags = PSize|PMinSize|PMaxSize;
  XSetNormalHints(display, window, &sizehints);

  if ((!is_color) && use_backup)
    backup = XCreatePixmap(display, window, width, height, depth);
  else { /* Can't use backup pixmap on color: not enough memory */
    use_backup = FALSE;
    hide_drawing = FALSE;
  }
  draw_into = backup ? backup : window;
/* Should perhaps try and check for failure, and use some other
   way of selecting use of window vs. backup */

    
  values_GC.foreground = background; values_GC.background = foreground;
  XChangeGC(display, default_GC, GCForeground|GCBackground, &values_GC);
  XFillRectangle(display, draw_into, default_GC, 0, 0, width, height);
  XSetForeground(display, default_GC, foreground);
  XSetBackground(display, default_GC, background);

  return TRUE;				/* open successful */
}

/* Close the device */
D_close()
{
  Window closebox;         /* window to do close in */
  XEvent closebox_event, window_event;
  int i;
  double lat, lon, tlat, tlon;
  char outstr[81];
  int ra_h, ra_m, ra_s;
  int de_d, de_m, de_s;
  char dsign;


  flushlines();
  if (use_backup)
    XCopyArea(display, backup, window, default_GC, 0, 0, width, height,
	      0, 0);

  closebox =XCreateSimpleWindow(display,window, 0, 0, 10, 10, 1, background,
				foreground);
  XDefineCursor(display, closebox, 
                XCreateFontCursor(display, XC_top_left_arrow));
  XMapWindow(display,closebox);
  XSelectInput(display, closebox, ButtonPressMask);
  XSelectInput(display, window, ButtonPressMask|ExposureMask);

  D_comment("Waiting for close of window or mouse button 1 or 3 input");

  while (!XCheckWindowEvent(display, closebox, ButtonPressMask,
			     &closebox_event))
    {
      if (XCheckWindowEvent(display, window, ExposureMask, &window_event)
	  && (window_event.type == Expose))
	if (use_backup)
	  XCopyArea(display, backup, window, default_GC,
		    window_event.xexpose.x, window_event.xexpose.y,
		    window_event.xexpose.width, window_event.xexpose.height,
		    window_event.xexpose.x, window_event.xexpose.y);

      if (XCheckWindowEvent(display, window, ButtonPressMask, &window_event)
	  && (window_event.type == ButtonPress)) {
	if ((i = invxform((int) window_event.xbutton.x,
			 (int) height - window_event.xbutton.y,
			 &lat, &lon)) >= 0) {
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
	  if (window_event.xbutton.button == Button3)
	    dblookup(i, lat, lon, 10);
	} else {
/*	  fprintf(stderr, "Returned -1\n");*/
	}
      }
    }
  
  XCloseDisplay(display);
}

#endif /* STARX11 */

#ifndef STARXTK
X_setdisplay()
{
  char *str;
  char *fore_color, *back_color;
  XColor cdef;
  Pixel *planes;

  screen_num = DefaultScreen(display);
  black = BlackPixel(display, screen_num);
  white = WhitePixel(display, screen_num);

  if ((str = XGetDefault(display, prog, "ReverseVideo")) 
     && strcmp(str, "on") == 0)
     reverse = 1;
  fore_color = XGetDefault(display, prog, "ForeGround");
  back_color = XGetDefault(display, prog, "BackGround");

  /* get global defaults from .Xdefaults */
  get_defaults();

  default_GC = DefaultGC(display, screen_num);
  default_cmap = DefaultColormap(display, screen_num);
  default_font = XLoadQueryFont(display, default_font_name);

  greek_font = XLoadQueryFont(display, greek_font_name);

  /* create color map */

  if (reverse) {
    foreground = black;
    background = white;
  }
  else {
    foreground = white;
    background = black;
  }

  is_color &= (DisplayPlanes(display, screen_num) >= 4);
  is_super_color = is_color & (DisplayPlanes(display, screen_num) >= 6);
  ncolors = (is_color ? 12 : 2);
  star_colors = (is_super_color ? 65: 0);

  if (is_color) {
    pixels = (Pixel *)
      malloc((unsigned int) (star_colors+ncolors)*sizeof(Pixel));
    if (!XAllocColorCells(display, default_cmap, 0, planes,
			  0, pixels, ncolors+star_colors))  {
      fprintf(stderr, "Error, can't get color cells\n");
      exit(10);
    }

    cdef.flags = DoRed|DoBlue|DoGreen;
    if (!fore_color || 
        !XParseColor(display, default_cmap, fore_color, &cdef)) {
      cdef.pixel = foreground ;
      XQueryColor(display, default_cmap, &cdef) ;
    }
    cdef.pixel = pixels[0] ;
    XStoreColor(display, default_cmap, &cdef) ;

    if (!back_color || 
        !XParseColor(display, default_cmap, back_color, &cdef)) {
      cdef.pixel = background ;
      XQueryColor(display, default_cmap, &cdef) ;
    }
    cdef.pixel = pixels[1] ;
    XStoreColor(display, default_cmap, &cdef) ;
    D_setcolors();
  }
}
#endif /* not STARXTK */

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

#ifdef STARXAW
  buffer_comment("Looking for object", FALSE);
#else
  D_comment("Looking for object");
#endif

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

#ifdef STARXAW
  buffer_comment("Done search", FALSE);
#else
  D_comment("Done search");
#endif
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
#ifdef STARXAW
      buffer_comment(outstr, FALSE);
#else
      D_comment(outstr);
#endif
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

#ifdef STARXAW
  if (is_killed()) return; /* Return if process_events returns TRUE,
				    i.e. if the close box has been hit */
#endif

  switch (cur_function) {
  case CHRTOUTLN:
  case CHRTHTICK:
  case CHRTVTICK:
    xline_sty = LineSolid;
    dashes = 1;
    break;
  case GRID_RA:
  case GRID_DEC:
    xline_sty = LineDoubleDash;
    dashes = 4;
    break;
  case ECLIPT:
    xline_sty = LineDoubleDash;
    dashes = 1;
    break;
  case CONSTBOUND:
    xline_sty = LineDoubleDash;
    dashes = 6;
    break;
  case CONSTPATTRN:
    xline_sty = LineDoubleDash;
    dashes = 8;
    break;
  case CONSTNAME:
  case CHARTFILE:
  default:
    switch (line_style) {
    case SOLID:
    case VECSOLID:
      xline_sty = LineSolid;
      dashes = 1;
      break;
    case DASHED:
    case VECDASH:
      xline_sty = LineDoubleDash;
      dashes = 5;
      break;
    case DOTTED:
    case VECDOT:
      xline_sty = LineDoubleDash;
      dashes = 1;
      break;
    default:
      xline_sty = LineSolid;
      dashes = 1;
      break;
    };
    break;
  }

/* Some servers can't do dashes */
  if (!use_x_dashes)
    xline_sty = LineSolid;


  if (xline_sty != curr_lsty) {
    values_GC.line_style = xline_sty;
    values_GC.dashes = dashes;
    XChangeGC(display, default_GC, GCLineStyle | GCDashList, &values_GC);
  }

  XDrawLine(display, draw_into, default_GC,
	    current_x, height - current_y, x, height - y);


  curr_lsty = xline_sty;

  if ((use_backup) && !hide_drawing)
    XCopyArea(display, backup, window, default_GC, 0, 0, width, height,
	      0, 0);

/*  XFlush(display);*/

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


static XPoint areapts[1000];
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

#ifdef STARXAW
  if (is_killed()) return; /* Return if process_events returns TRUE,
				    i.e. if the close box has been hit */
#endif

  XFillPolygon(display, draw_into, default_GC,
	       areapts, nareapts, Complex, CoordModeOrigin);
  nareapts = 0;
}




static int curr_colr;
/* Set the color to be used for lines, areas and text */
/* color_str is a 2 char (+ '\0') string
   containing a specification for a color,
   e.g. "G2" for the color of a star of spectral class G2, or "r7" for
   red, level seven.  The interpretation of the color string is left to
   the device driver */
D_color(color_str)
     char *color_str;
{
  Pixel colr;
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
#ifdef STARXAW
  if (is_killed()) return; /* Return if process_events returns TRUE,
				    i.e. if the close box has been hit */
#endif


  class = 0;
  while (table[class] && (table[class] != color_str[0])) class++;

  if (is_super_color) {
    subclass = isdigit(color_str[1]) ? color_str[1] - '0' : 0;
    colr = table[class] ? star_pixels[super_spectra[class][subclass]] : 
      pixels[WHITE];
  } else
    colr = table[class] ? pixels[spectra[class]] : pixels[WHITE];

  if (colr != curr_colr) {
    flushlines();
    XSetForeground(display, default_GC, colr);
  };
}


static int current_font = -1, current_fsize = -1;
static XFontStruct *curr_xfont;

/* Set the font and font size to be used for text. */
/* Note order of args */
D_fontsize(fsize, font)
     int font, fsize;
{
  char *fontname, readname[50];

#ifdef STARXAW
  if (is_killed()) return; /* Return if process_events returns TRUE,
				    i.e. if the close box has been hit */
#endif

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

#ifdef RELEASE3_FONTS
  sprintf(readname,"%s%d*",fontname,fsize);
#else
  sprintf(readname,"%s%d",fontname,fsize);
#endif

  if (!(curr_xfont = XLoadQueryFont(display, readname)))
    curr_xfont = default_font;
  else {
    current_font = font;
    current_fsize = fsize;
  }

  if (curr_xfont)
    XSetFont(display, default_GC, curr_xfont->fid);
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

#ifdef STARXAW
  if (is_killed()) return; /* Return if process_events returns TRUE,
				    i.e. if the close box has been hit */
#endif

  if (star_lbl) {
    if (isgreek(s[0]) && (isdigit(s[1]) || (s[1] == ' '))) {
      /* Greek if first character is greek encoded,
	 and the second is space or a digit */
      use_greek = TRUE;
      if (greek_font)
	XSetFont(display, default_GC, greek_font->fid);
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

  XDrawString(display, draw_into, default_GC, x,
	      height - y, s, strlen(s));

  if ((use_backup) && !hide_drawing)
    XCopyArea(display, backup, window, default_GC, 0, 0, width, height,
	      0, 0);

/*  XFlush(display);*/

  if (use_greek && curr_xfont) XSetFont(display, default_GC, curr_xfont->fid);
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
#ifdef STARX11
D_comment(str)
     char *str;
{
  fprintf(stderr, "%s\n", str);
}
#endif /* STARX11 */



/**
Higher level functions
**/

drawlen(x, y, dx, dy, len)
     int x,y, dx, dy, len;
{
  buildlines(x + dx, (int)(height - (y+dy)), x+dx+len, (int)(height - (y+dy)));
}


D_setcolors()
{
  Cardinal i;

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

  double max_colr;

  static double super_data[][3] = {
    /*  Red             Green           Blue    Name by which type is known */
/*  0 */    { 0.38937,        0.46526,        0.79493 },    /* B0 */
/*  1 */    { 0.39501,        0.47146,        0.78847 },    /* B1 */
/*  2 */    { 0.40103,        0.47792,        0.78151 },    /* B2 */
/*  3 */    { 0.40640,        0.48355,        0.77526 },    /* B3 */
/*  4 */    { 0.41341,        0.49071,        0.76701 },    /* B5 */
/*  5 */    { 0.43251,        0.50914,        0.74412 },    /* B8 */
/*  6 */    { 0.44342,        0.51897,        0.73079 },    /* B9 */
/*  7 */    { 0.45181,        0.52618,        0.72042 },    /* A0 */
/*  8 */    { 0.46931,        0.54026,        0.69847 },    /* A2 */
/*  9 */    { 0.47958,        0.54792,        0.68541 },    /* A3 */
/* 10 */    { 0.48538,        0.55205,        0.67797 },    /* A5 */
/* 11 */    { 0.50879,        0.56731,        0.64752 },    /* F0 */
/* 12 */    { 0.51732,        0.57231,        0.63627 },    /* F2 */
/* 13 */    { 0.52348,        0.57573,        0.62810 },    /* F5 */
/* 14 */    { 0.54076,        0.58447,        0.60496 },    /* F8 */
/* 15 */    { 0.54853,        0.58799,        0.59446 },    /* G0 */
/* 16 */    { 0.56951,        0.59623,        0.56584 },    /* G5 */
/* 17 */    { 0.58992,        0.60244,        0.53765 },    /* K0 */
/* 18 */    { 0.61098,        0.60693,        0.50828 },    /* K2 */
/* 19 */    { 0.63856,        0.60977,        0.46950 },    /* K5 */
/* 20 */    { 0.68698,        0.60595,        0.40110 },    /* M0 */
/* 21 */    { 0.72528,        0.59434,        0.34744 },    /* M2 */
/* 22 */    { 0.75182,        0.58144,        0.31097 },    /* M3 */
/* 23 */    { 0.78033,        0.56272,        0.27282 },    /* M4 */
/* 24 */    { 0.81066,        0.53676,        0.23394 },    /* M5 */
/* 25 */    { 0.84247,        0.50195,        0.19570 },    /* M6 */
/* 26 */    { 0.87512,        0.45667,        0.16004 },    /* M7 */
/* 27 */    { 0.71033,        0.59983,        0.36829 },    /* N0 */
/* 28 */    { 0.78625,        0.55816,        0.26507 },    /* N3 */
/* 29 */    { 0.93792,        0.33011,        0.10649 },    /* N8 */
/* 30 */    { 0.94897,        0.29906,        0.10012 },    /* N9 */
/* 31 */    { 0.79832,        0.54811,        0.24950 },    /* S4 */
/* 32 */    { 0.38241,        0.45743,        0.80282 },    /* O  */
  };

  XColor c;

  static struct {unsigned char red, green, blue;} other_colrs[] = {
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


  c.flags = DoRed|DoGreen|DoBlue;
  for (i=0; i<=9; i++) {
    c.red = 256*data[i][0];
    c.green = 256*data[i][1];
    c.blue = 256*data[i][2];
    c.pixel = pixels[i];
    XStoreColor(display, default_cmap, &c);
  }
  
  if (is_super_color) {
    star_pixels = &(pixels[i]);
    for (i=0; i<=32; i++) {
      max_colr = super_data[i][0];
      max_colr = super_data[i][1] > max_colr ? super_data[i][1] : max_colr;
      max_colr = super_data[i][2] > max_colr ? super_data[i][2] : max_colr;
      c.red = (int) 256*255*super_data[i][0] / max_colr;
      c.green = (int) 256*255*super_data[i][1] /max_colr;
      c.blue = (int) 256*255*super_data[i][2] /max_colr;
      c.pixel = star_pixels[i];
      XStoreColor(display, default_cmap, &c);
    }
    for (i = 33; i <= 64; i++) {
      c.red = other_colrs[i-33].red*256;
      c.green = other_colrs[i-33].green*256;
      c.blue = other_colrs[i-33].blue*256;
      c.pixel = star_pixels[i];
      XStoreColor(display, default_cmap, &c);
    };
  }
}


#ifndef STARXTK
/* Differ in starXtk */
#define MAXDRAW 250
static XSegment seg[MAXDRAW];
static int npoints = 0;

buildlines(x1, y1, x2, y2)
     int x1, y1, x2, y2;
{
#ifdef STARXAW
  if (is_killed()) return; /* Return if process_events returns TRUE,
				    i.e. if the close box has been hit */
#endif

  if (npoints == MAXDRAW) flushlines();
  seg[npoints].x1 = x1;
  seg[npoints].y1 = y1;
  seg[npoints].x2 = x2;
  seg[npoints].y2 = y2;
  npoints++;
}

flushlines()
{
#ifdef STARXAW
  if (is_killed()) {
    npoints = 0;
    return; /* Return if process_events returns TRUE,
	       i.e. if the close box has been hit */
  };
#endif

  curr_lsty = values_GC.line_style = LineSolid;
  values_GC.dashes = 1;
  XChangeGC(display, default_GC, GCLineStyle | GCDashList, &values_GC);

  XDrawSegments(display, draw_into, default_GC, seg, npoints);
  npoints = 0;

  if ((use_backup) && !hide_drawing)
    XCopyArea(display, backup, window, default_GC, 0, 0, width, height,
	      0, 0);
  XFlush(display);
}

killlines()
{
  npoints = 0;
}
#endif /* not STARXtk */


/* get global defaults from .Xdefaults */
get_defaults()
{
  char *str;
  int i;

  /* Allow fonts to be overriden */
  if (str = XGetDefault(display, prog,"default_font_name"))
    default_font_name = str;
  if (str =  XGetDefault(display, prog, "greek_font_name"))
    greek_font_name = str;
  if (str =  XGetDefault(display, prog, "timesroman_font_name"))
    timesroman_font_name = str;
  if (str =  XGetDefault(display, prog, "timesbold_font_name"))
    timesbold_font_name = str;
  if (str =  XGetDefault(display, prog, "timesital_font_name"))
    timesital_font_name = str;
  if (str =  XGetDefault(display, prog, "timesboldital_font_name"))
    timesboldital_font_name = str;
  if (str =  XGetDefault(display, prog, "helv_font_name"))
    helv_font_name = str;
  if (str =  XGetDefault(display, prog, "helvbold_font_name"))
    helvbold_font_name = str;
  if (str =  XGetDefault(display, prog, "helvital_font_name"))
    helvital_font_name = str;
  if (str =  XGetDefault(display, prog, "helvboldital_font_name"))
    helvboldital_font_name = str;
  if (str =  XGetDefault(display, prog, "courier_font_name"))
    courier_font_name = str;
  if (str =  XGetDefault(display, prog, "courital_font_name"))
    courital_font_name = str;
  if (str =  XGetDefault(display, prog, "courbold_font_name"))
    courbold_font_name = str;
  if (str =  XGetDefault(display, prog, "couritalbold_font_name"))
    couritalbold_font_name = str;

  /* set post_preview, hide_drawing, use_backup from .Xdefaults */
  if ((str = XGetDefault(display, prog, "PostscriptPreview"))
     && (!strcmp(str, "on") || !strcmp(str, "true")))
    post_preview = TRUE;
  if ((str = XGetDefault(display, prog, "PostscriptLandscape"))
     && (!strcmp(str, "on") || !strcmp(str, "true")))
    post_landscape = TRUE;
  if ((str = XGetDefault(display, prog, "HideDrawing"))
     && (!strcmp(str, "on") || !strcmp(str, "true")))
    hide_drawing = TRUE;
  if ((str = XGetDefault(display, prog, "UseBackup"))
     && (!strcmp(str, "on") || !strcmp(str, "true")))
    use_backup = TRUE;

  if ((str = XGetDefault(display, prog, "UseXDashes"))
     && (!strcmp(str, "on") || !strcmp(str, "true")))
     use_x_dashes = TRUE;

  if (str = XGetDefault(display, prog, "htick_lim"))
    htick_lim = atoi(str);
  if (str = XGetDefault(display, prog, "htext_lim"))
    htext_lim = atoi(str);
  if (str = XGetDefault(display, prog, "htext_xoff"))
    htext_xoff = atoi(str);
  if (str = XGetDefault(display, prog, "htext_yoff"))
    htext_yoff = atoi(str);
  if (str = XGetDefault(display, prog, "vtick_lim"))
    vtick_lim = atoi(str);
  if (str = XGetDefault(display, prog, "vtext_lim"))
    vtext_lim = atoi(str);
  if (str = XGetDefault(display, prog, "vtext_xoff"))
    vtext_xoff = atoi(str);
  if (str = XGetDefault(display, prog, "vtext_yoff"))
    vtext_yoff = atoi(str);


/* externs for labels */
  if (str = XGetDefault(display, prog, "x_nameoffset"))
    x_nameoffset = atoi(str);
  if (str = XGetDefault(display, prog, "y_nameoffset"))
    y_nameoffset = atoi(str);
  if (str = XGetDefault(display, prog, "x_lbloffset"))
    x_lbloffset = atoi(str);
  if (str = XGetDefault(display, prog, "y_lbloffset"))
    y_lbloffset = atoi(str);
  if (str = XGetDefault(display, prog, "x_magoffset"))
    x_magoffset = atoi(str);
  if (str = XGetDefault(display, prog, "y_magoffset"))
    y_magoffset = atoi(str);

/* externs for legend: variables of positioning are here */
  if (str = XGetDefault(display, prog, "l_til"))
    l_til = atoi(str);
  if (str = XGetDefault(display, prog, "l_stil"))
    l_stil = atoi(str);

  if (str = XGetDefault(display, prog, "l_lmar1"))
    l_lmar1 = atoi(str);
  if (str = XGetDefault(display, prog, "l_lmar2"))
    l_lmar2 = atoi(str);
  if (str = XGetDefault(display, prog, "l_ltext"))
    l_ltext = atoi(str);
  if (str = XGetDefault(display, prog, "l_rmar1"))
    l_rmar1 = atoi(str);
  if (str = XGetDefault(display, prog, "l_rmar2"))
    l_rmar2 = atoi(str);
  if (str = XGetDefault(display, prog, "l_rtext"))
    l_rtext = atoi(str);

  if (str = XGetDefault(display, prog, "l_line1"))
    l_line1 = atoi(str);
  if (str = XGetDefault(display, prog, "l_line2"))
    l_line2 = atoi(str);
  if (str = XGetDefault(display, prog, "l_line3"))
    l_line3 = atoi(str);
  if (str = XGetDefault(display, prog, "l_line4"))
    l_line4 = atoi(str);
  if (str = XGetDefault(display, prog, "l_line5"))
    l_line5 = atoi(str);
  if (str = XGetDefault(display, prog, "l_line6"))
    l_line6 = atoi(str);

/* Point sizes for font calls */
  if (str = XGetDefault(display, prog, "titlesize"))
    titlesize = atoi(str);
  if (str = XGetDefault(display, prog, "subtlsize"))
    subtlsize = atoi(str);
  if (str = XGetDefault(display, prog, "namesize"))
    namesize = atoi(str);
  if (str = XGetDefault(display, prog, "lblsize"))
    lblsize = atoi(str);
  if (str = XGetDefault(display, prog, "magsize"))
    magsize = atoi(str);

/* Fonts for font calls */
  if ((str = XGetDefault(display, prog, "namefnt")) && (i = tr_fname(str)))
    namefnt = i;
  if ((str = XGetDefault(display, prog, "lblfnt")) && (i = tr_fname(str)))
    lblfnt = i;
  if ((str = XGetDefault(display, prog, "magfnt")) && (i = tr_fname(str)))
    magfnt = i;
  if ((str = XGetDefault(display, prog, "titlefnt")) && (i = tr_fname(str)))
    titlefnt = i;
  if ((str = XGetDefault(display, prog, "subtlfnt")) && (i = tr_fname(str)))
    subtlfnt = i;

/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail,*/
  if (str = XGetDefault(display, prog, "th_smul"))
    th_smul = atoi(str);
  if (str = XGetDefault(display, prog, "th_smin"))
    th_smin = atoi(str);
  if (str = XGetDefault(display, prog, "th_madj"))
    th_madj = atoi(str);
  if (str = XGetDefault(display, prog, "th_mmax"))
    th_mmax = atoi(str);
}



int tr_fname(s)
char *s;
{
  int i;

  for (i = 0; s[i]; i++)
    if (isupper(s[i]))
      s[i] = tolower(s[i]);

  if(!strcmp(s, "timesroman")) return TIMESROMAN;
  else if (!strcmp(s, "timesbold")) return TIMESBOLD;
  else if (!strcmp(s, "timesital")) return TIMESITAL;
  else if (!strcmp(s, "timesboldital")) return TIMESBOLDITAL;
  else if (!strcmp(s, "helv")) return HELV;
  else if (!strcmp(s, "helvbold")) return HELVBOLD;
  else if (!strcmp(s, "helvital")) return HELVITAL;
  else if (!strcmp(s, "helvboldital")) return HELVBOLDITAL;
  else if (!strcmp(s, "courier")) return COURIER;
  else if (!strcmp(s, "courbold")) return COURBOLD;
  else if (!strcmp(s, "courital")) return COURITAL;
  else if (!strcmp(s, "couritalbold")) return COURITALBOLD;
  else return 0;
}
