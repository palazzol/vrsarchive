/*
 * Driver creates X11 window and draws starchart information inside.
 * Intially window size is variable, but it stays constant once it's
 * mapped.  It handles expose events, even while it's drawing.
 * Includes glyphs for variable stars.
 *
 * Rewritten from scratch July 1989
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starXaw.c,v 1.1 1990-03-30 16:39:26 vrs Exp $";

#include <stdio.h>
#include <math.h>

#include "star3.h"
#include "starXaw.h"

#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif

#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifdef X11R4
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#define ASCII_STRING
#define XAW_BC
#include <X11/Xaw/AsciiText.h>
#else
#include <X11/Command.h>
#include <X11/Form.h>
#include <X11/Box.h>
#include <X11/AsciiText.h>
#endif

char *malloc();


char *prog="StarXaw";
#include "icon.h"


/* Externs */
extern int g_argc;
extern char **g_argv;

extern char *title;

extern int user_interact;


/* starX11 X items */
extern Display *display;	   /* connection to display server */
extern Window root, window;	   /* window to graphics in */
extern Drawable draw_into;
extern Colormap default_cmap;	   /* colormap */
extern GC default_GC;		   /* graphics context */
extern XFontStruct *default_font;  /* default font */
extern XFontStruct *greek_font;	   /* Greek font */
extern Pixmap backup;		   /* backup for expose events */
extern Pixel black, white, foreground, background;
extern Pixel *pixels;		   /* color map cells */
extern Pixel *star_pixels;         /* color map cells for super color stars */
extern int ncolors, star_colors;
extern XEvent event;		   /* event structure */
extern XGCValues values_GC;	   /* modify GC */
extern unsigned int width, height;

extern Boolean reverse;

extern Bool use_backup;
extern Bool hide_drawing;
extern Bool post_preview;
extern Bool post_landscape;
extern Bool use_x_dashes;
extern Bool is_color;		   /* Is it color? */
extern Bool is_super_color;	   /* Is it many color? */

extern int Xargc;
extern char *Xargv[];



extern mapwindow fullpage, mainmap, thumbmap;

extern int htick_lim, htext_lim, htext_xoff, htext_yoff;
extern int vtick_lim, vtext_lim, vtext_xoff, vtext_yoff;

/* externs for labels */

extern int x_nameoffset, y_nameoffset, x_lbloffset, y_lbloffset, 
  x_magoffset, y_magoffset;

/* externs for legend: variables of positioning are here */
extern int l_til;
extern int l_stil;

extern int l_lmar1;
extern int l_lmar2;
extern int l_ltext;
extern int l_rmar1;
extern int l_rmar2;
extern int l_rtext;

extern int l_line1;
extern int l_line2;
extern int l_line3;
extern int l_line4;
extern int l_line5;
extern int l_line6;

/* Point sizes for font calls */
extern int titlesize;
extern int subtlsize;
extern int namesize;
extern int lblsize;
extern int magsize;

/* Fonts for font calls */
extern int namefnt;
extern int lblfnt;
extern int magfnt;
extern int titlefnt;
extern int subtlfnt;

/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail,*/
extern double th_smul;
extern double th_smin;
extern double th_madj;
extern double th_mmax;

#ifndef MAXPATHLEN
#define MAXPATHLEN 1025
#endif
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




/* starXaw Local variables */
Widget interface, chart_shell, chart_widget;
Widget com_wid;
Screen *screen;
Bool fixed_size = FALSE;
int fixed_width = 500;
int fixed_height = 500;

Bool fixed_win_coo = False;  /* use Fixed window coordinates,
				override X window settings in D_open*/

static int draw_chart;	 /* set to TRUE when D_userinput should return TRUE */
static int quit_program; /* set to TRUE when D_userinput should return FALSE */
static int killed_window;/* set to TRUE when the user has closed the
			    chart window during drawing */
static int kill_the_window;/* set to TRUE if close_window should kill the
			      window immediately.
			      Otherwise lets D_break do it. */


/* main() parses the command line, and any input .starrc files
calls D_userinput() to finish setting the basic variables of the chart
then fix_mapwin, which may read a mapwin file, then D_mapwininput()
to give the user final input of the detailed mapwin structure.
D_open then is called to begin the display.
the mapwin structure may be modified, based on the size of the window opened
main() will then write the mapwin file if so desired

D_break is called periodically to give the user a chance to cancel input

D_close is called at the end of plotting.


This driver supplies:
D_userinput
D_mapwininput
D_open
D_close
D_break

Much other code is shared with starX11.c
D_comment is supplied to replace starX11's D_comment which writes to
standard output
*/

D_open()
{
  int x, y;
  unsigned int border, depth, map_width;
  Pixmap icon;
  XSizeHints sizehints;
  double legend_spacing;


  Window shell_window;
  Widget closebox;
  
  void expose(), close_window();
  
  Arg shell_args[10];
  Arg widget_args[1];
  Cardinal nshell_args, nwidget_args;
  static Arg close_args[] = {
    {XtNx, (XtArgVal) 2},
    {XtNy, (XtArgVal) 2},
    {XtNwidth, (XtArgVal) 10},
    {XtNheight, (XtArgVal) 10},
    {XtNbackground, (XtArgVal) NULL},
    {XtNforeground, (XtArgVal) NULL},
  };

  /* set user_interact so D_break will be called in starmain.c */
  user_interact = TRUE;

  if (post_preview) {
    if (post_landscape) {
      XtSetArg(shell_args[0], XtNwidth, 768);
      XtSetArg(shell_args[1], XtNheight, 595);
    } else {
      XtSetArg(shell_args[0], XtNwidth, 595);
      XtSetArg(shell_args[1], XtNheight, 768);
    };
  } else {
    XtSetArg(shell_args[0], XtNwidth, 550);
    XtSetArg(shell_args[1], XtNheight, 550);
  }
  XtSetArg(shell_args[2], XtNtitle, (!title[0]) ? prog : title);
  XtSetArg(shell_args[3], XtNiconName, (!title[0]) ? prog : title);
  XtSetArg(shell_args[4], XtNborderColor, foreground);
  nshell_args = 5;
  if (fixed_size) {
    XtSetArg(shell_args[0], XtNwidth, fixed_width);
    XtSetArg(shell_args[1], XtNheight, fixed_height);
  };

  XtSetArg(widget_args[0], XtNbackground, background);
  nwidget_args = 1;

  XtSetArg(close_args[4], XtNbackground, foreground);
  XtSetArg(close_args[5], XtNforeground, background);

  chart_shell =
    XtCreatePopupShell(prog, topLevelShellWidgetClass, interface,
		       shell_args, nshell_args);

  killed_window = FALSE;
  kill_the_window = FALSE;
  
  chart_widget = XtCreateManagedWidget(prog, compositeWidgetClass, chart_shell,
				       widget_args, nwidget_args);
  closebox = XtCreateManagedWidget("", simpleWidgetClass,
				   chart_widget, close_args,
				   XtNumber(close_args));

  XtPopup(chart_shell, XtGrabNonexclusive);
  window = XtWindow(chart_widget);
  shell_window = XtWindow(chart_shell);
  
  if (post_preview) {
    sizehints.flags = PSize|PMinSize|PMaxSize;
    if (post_landscape) {
      sizehints.width = sizehints.min_width = sizehints.max_width = 792;
      sizehints.height = sizehints.min_height = sizehints.max_height = 576;
    } else {
      sizehints.width = sizehints.min_width = sizehints.max_width = 576;
      sizehints.height = sizehints.min_height = sizehints.max_height = 792;
    };
  } else if (fixed_size) {
    sizehints.flags = PSize|PMinSize|PMaxSize;
    sizehints.width = sizehints.min_width = sizehints.max_width
      = fixed_width;
    sizehints.height = sizehints.min_height = sizehints.max_height
      = fixed_height;
  } else {
    sizehints.flags = PMinSize;
    sizehints.min_width = 550; sizehints.min_height = 550;
  }

  icon = XCreateBitmapFromData(display, window, icon_bits, 
                               icon_width, icon_height);
  Xargv[0] = g_argv[0];
  XSetStandardProperties(display, shell_window, prog, prog, icon,
			 Xargv, Xargc, &sizehints);
  XDefineCursor(display, XtWindow(closebox), 
		XCreateFontCursor(display, XC_top_left_arrow));
  XDefineCursor(display, window, XCreateFontCursor(display, XC_crosshair));
  while (XtPending()) {
    XtNextEvent(&event);
    XtDispatchEvent(&event);
  }
  XSelectInput(display, window, StructureNotifyMask);
  XMapWindow(display, window);
  while(!XCheckTypedWindowEvent(display, shell_window, MapNotify, &event));

  XGetGeometry(display, shell_window, &root, &x, &y, &width, &height, &border,
               &depth);
  sizehints.width = sizehints.max_width = sizehints.min_width = width;
  sizehints.height = sizehints.max_height = sizehints.min_height = height;
  XtSetArg(shell_args[0], XtNwidth, width);
  XtSetArg(shell_args[1], XtNheight, height);
  XtSetValues(chart_shell, shell_args, nshell_args);

  map_width = width - 72;

  if (!fixed_win_coo) {
    fullpage.width = map_width;
    fullpage.height = height - 72;
    fullpage.x_offset = 18;
    fullpage.y_offset = 36;

    mainmap.width = map_width;
    mainmap.height = ((height -72)*7)/10;
    mainmap.x_offset = 18;
    mainmap.y_offset = 18 + ((height - 72)*2.5)/10;

    thumbmap.width = map_width/2;
    thumbmap.height = ((height - 72)*2)/10;
    thumbmap.x_offset = 18 + map_width/2;
    thumbmap.y_offset = 18;
  };

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
  if ((!post_preview) && (!fixed_size)) {
    fullpage.height = height - fullpage.y_offset - 18;
    mainmap.height = height - mainmap.y_offset - 18;
  }

  sizehints.flags = PSize|PMinSize|PMaxSize;
  XSetNormalHints(display, shell_window, &sizehints);
  XSetNormalHints(display, window, &sizehints);

  if ((!is_color) && use_backup) {
    backup = XCreatePixmap(display, window, width, height, depth);
    XtAddEventHandler(chart_widget, ExposureMask, False,
		      expose,(caddr_t) backup);

  } else { /* Can't use backup pixmap on color: not enough memory */
    use_backup = FALSE;
    hide_drawing = FALSE;
  }
  draw_into = backup ? backup : window;
/* Should perhaps try and check for failure, and use some other
   way of selecting use of window vs. backup */

  XtAddEventHandler(closebox, ButtonPressMask|ButtonReleaseMask,
		    False, close_window, (caddr_t) chart_widget);
  

  values_GC.foreground = background; values_GC.background = foreground;
  XChangeGC(display, default_GC, GCForeground|GCBackground, &values_GC);
  XFillRectangle(display, draw_into, default_GC, 0, 0, width, height);
  XSetForeground(display, default_GC, foreground);
  XSetBackground(display, default_GC, background);

  return TRUE;				/* open successful */
}


void pointer_input();

D_close()
{
  if (is_killed()) {
    XtDestroyWidget(XtParent(chart_widget));
    if (use_backup) XFreePixmap(display, backup);
    un_help();
    D_comment("Closed");
    return;
  };

  flushlines();

  kill_the_window = TRUE;

  XtAddEventHandler(chart_widget, ButtonPressMask,False,
		    pointer_input,(caddr_t) chart_widget);

  D_comment("Waiting for close of window or mouse button 1 or 3 input");

  XtRemoveGrab(chart_shell);
  XtAddGrab(chart_shell, True, False);
  XtAddGrab(com_wid, False, False);
  pointer_help();
}

void pointer_input(widget, closure, event)
     Widget widget;
     caddr_t closure;		/* Widget */
     XEvent *event;
{
  int i;
  double lat, lon, tlat, tlon;
  char outstr[81];
  int ra_h, ra_m, ra_s;
  int de_d, de_m, de_s;
  char dsign;
  
  
  if ((i = invxform((int) event->xbutton.x,
		    (int) height - event->xbutton.y,
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
    
    
    sprintf(outstr, "RA: %2dh%2dm%2ds   dec: %c%2dd%2dm%2ds\n",
	    ra_h, ra_m, ra_s, dsign, de_d, de_m, de_s);
    buffer_comment(outstr, TRUE);
    
    if (event->xbutton.button == Button3) {
      buffer_comment("", TRUE);
      dblookup(i, lat, lon, 10);
    };
  } else {
    /*	  fprintf(stderr, "Returned -1\n");*/
  }
}



/* Open the display connection and do much of the X initialization
   Display text output window, and buttons for dialog boxes.
   process dialog box input
   draw_chart becomes true when the draw chart button gets pressed
*/
D_userinput()
{
  static int initialized = FALSE;

  if (!initialized) {
    start_X11();
    initialized = TRUE;
  }

  draw_chart = FALSE;
  quit_program = FALSE;
  while (!draw_chart) {
    XtNextEvent(&event);
    XtDispatchEvent(&event);
    if (quit_program) return FALSE;
  };

  /* User has done the input, now we are ready to begin the chart.*/
  return TRUE;
}

/*
  D_mapwininput() is in starXawDlog.c for convenience in shared variables */




/* Put non-displayed comment in output.  Allowed in postscript, but
   few other drivers will be able to support this. */
char comment_buffer[MAX(MAXPATHLEN*2, 6000)];
/* Allow for lots of space */
D_comment(str)
     char *str;
{
  Arg args[3];

  strcpy(comment_buffer, str);
  XtSetArg(args[0], XtNlabel, comment_buffer);
  XtSetArg(args[1], XtNinsertPosition, 0);
  XtSetValues(com_wid, args, (Cardinal)2);
  XtTextSetLastPos(com_wid, strlen(str));
}

/* D_comment with several lines of buffer */
buffer_comment(str, reset)
     char *str;
     int reset;
{
  Arg args[2];
  static int ncalled = 0;
  int i,j;
  int len, had_newline;

  if (reset) ncalled = 0;
  if (ncalled == 0) comment_buffer[0] = '\0';
  ncalled++;

  /* Find last character to copy */
  len = strlen(str)-1;
  if (str[len] == '\n') {
    had_newline = TRUE;
    len--;			/* skip newline */
  } else {
    had_newline = FALSE;
  }
  while (str[len] == ' ') len--;	/* skip spaces */


  i = strlen(comment_buffer);
  if ((i > 0) && (comment_buffer[i] != '\n')) {
    comment_buffer[i++] = '\n';
    comment_buffer[i] = '\0';
  }

  /* Now i is the terminating \0 */
  if ((i + strlen(str)) < sizeof(comment_buffer)) {
    /* Add string to comment_buffer up to last non space before '\n' */
    for (j = 0; j <= len;  j++)
      comment_buffer[i++] = str[j];
    if (had_newline)
      comment_buffer[i++] = '\n';
    comment_buffer[i] = '\0';
  } /* If buffer is full, ignore string */


  XtSetArg(args[0], XtNlabel, comment_buffer);
  XtSetArg(args[1], XtNinsertPosition, 0);
  XtSetValues(com_wid, args, (Cardinal)2);
  XtTextSetLastPos(com_wid, strlen(comment_buffer));

  XFlush(display);

  fprintf(stderr, "%s\n", str);
}


int D_break()
{
  if (is_killed()) {
    XtDestroyWidget(XtParent(chart_widget));
    if (use_backup) XFreePixmap(display, backup);
    un_help();
    D_comment("Closed");
  };

  return killed_window;
}

/* Widget functions */
void Destroyed(widget, closure, callData)
    Widget widget;
    caddr_t closure;		/* unused */
    caddr_t callData;		/* unused */
{
  quit_program = TRUE;
}

void Quit(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
    XtDestroyWidget((Widget)closure);
}



/* StarXaw X functions */
extern char help_buffer[];
extern Widget help_wid;

/* Make the top level window, which contains buttons to
   bring up the dialog boxes and do the chart */  
start_X11()
{
  Widget outer;
  Arg Button_arg[10], Label_arg[10], Text_arg[10];
  static XtCallbackRec callback[2];
  Cardinal NButton_args, NLabel_args, nText_args;

  static Arg shell_args[2];
  extern Button_set dlogbuttons[];
  int i;
  void Draw_chart(), Save_Load_file(), XHelp();
  Widget first_button;
  Widget new_w, old_w;
  Pixmap icon;

  XtSetArg(Button_arg[0], XtNcallback, callback);
  XtSetArg(Button_arg[1], XtNborderWidth, 1);
  XtSetArg(Button_arg[2], XtNwidth, 150);
  XtSetArg(Button_arg[3], XtNheight, 20);
  NButton_args = 4;

  /* Label_arg[0] is the text of the label */
  XtSetArg(Label_arg[1], XtNborderWidth, 0);
  XtSetArg(Label_arg[2], XtNjustify, XtJustifyLeft);
  XtSetArg(Label_arg[3], XtNheight, 20);
  NLabel_args = 4;


  Xargv[0] = g_argv[0];
  interface = XtInitialize(prog, applicationShellWidgetClass,
			  NULL, 0, &Xargc, Xargv);
  display = XtDisplay(interface);
  screen = XtScreen(interface);

  X_setdisplay();

  icon = XCreateBitmapFromData(display, XtScreen(interface)->root, icon_bits, 
                               icon_width, icon_height);

  XtSetArg(shell_args[0], XtNtitle, prog);
  XtSetArg(shell_args[1], XtNiconPixmap, icon);
  XtSetValues(interface, shell_args, (Cardinal) 2);


  set_translations();		/* Set translation tables for text widgets */

  /* We have now created the interface widget, and set up the basics
     of the display */

  /* Now we create the interface */
  /* Currently using the "form" widget, with some labels and
     command widgets (buttons) which will call the functions above */

  outer = XtCreateManagedWidget("form", formWidgetClass, interface,
				NULL, (Cardinal)0);
  XtAddCallback(outer, XtNdestroyCallback, Destroyed, NULL);

  callback[0].callback = Quit;
  callback[0].closure = (caddr_t)interface;

  XtSetArg(Button_arg[4], XtNhorizDistance, 5);
  XtSetArg(Button_arg[5], XtNvertDistance, 5);
  NButton_args = 6;
  new_w = XtCreateManagedWidget("QUIT",
			commandWidgetClass, outer, Button_arg, NButton_args);
  first_button = new_w;
  old_w = new_w;
  XtSetArg(Label_arg[0], XtNlabel, "Controls:");
  XtSetArg(Label_arg[4], XtNfromVert, old_w);
  NLabel_args = 5;
  new_w = XtCreateManagedWidget("label", labelWidgetClass,
			      outer, Label_arg, NLabel_args);

  /* Button */
  i = 0;
  while (dlogbuttons[i].blabel[0]) {
    old_w = new_w;
    XtSetArg(Button_arg[4], XtNfromVert, old_w);

    callback[0].callback = *dlogbuttons[i].func;
    new_w = XtCreateManagedWidget(dlogbuttons[i].blabel,
			  commandWidgetClass, outer, Button_arg, NButton_args);
    i++;
  };

  /* Button */
  callback[0].callback = Save_Load_file;
  old_w = new_w;
  XtSetArg(Button_arg[4], XtNfromVert, old_w);
  new_w = XtCreateManagedWidget("Save / Load File",
			commandWidgetClass, outer, Button_arg, NButton_args);

  /* Label */
  old_w = new_w;
  XtSetArg(Label_arg[0], XtNlabel, "Actions:");
  XtSetArg(Label_arg[4], XtNfromVert, old_w);
  new_w = XtCreateManagedWidget("label", labelWidgetClass,
			outer, Label_arg, NLabel_args);

  /* Button */
  callback[0].callback = Draw_chart;
  old_w = new_w;
  XtSetArg(Button_arg[4], XtNfromVert, old_w);
  new_w = XtCreateManagedWidget("Draw Chart",
			commandWidgetClass, outer, Button_arg, NButton_args);


  /* Label */
  old_w = new_w;
  XtSetArg(Label_arg[0], XtNlabel, "Output:");
  XtSetArg(Label_arg[4], XtNfromVert, old_w);
  new_w = XtCreateManagedWidget("label", labelWidgetClass, outer, 
			Label_arg, NLabel_args);

  /* Text */
  old_w = new_w;
  XtSetArg(Text_arg[0], XtNstring, comment_buffer);
  XtSetArg(Text_arg[1], XtNlength, MAXPATHLEN*2);
  XtSetArg(Text_arg[2], XtNwidth, 390);
  XtSetArg(Text_arg[3], XtNheight, 134);
  XtSetArg(Text_arg[4], XtNhorizDistance, 5);
  XtSetArg(Text_arg[5], XtNfromVert, old_w);
  XtSetArg(Text_arg[6], XtNtextOptions, wordBreak | scrollVertical);
  nText_args = 7;
  com_wid = XtCreateManagedWidget("",
			  asciiStringWidgetClass, outer,
			  Text_arg, nText_args);

  i = start_help();
  XtSetArg(Text_arg[0], XtNstring, help_buffer);
  XtSetArg(Text_arg[1], XtNlength, i);
  XtSetArg(Text_arg[2], XtNwidth, 235);
  XtSetArg(Text_arg[3], XtNheight, 365);
  XtSetArg(Text_arg[4], XtNvertDistance, 5);
  XtSetArg(Text_arg[5], XtNhorizDistance, 160);
  nText_args = 6;
  help_wid = XtCreateManagedWidget("",
			  asciiStringWidgetClass, outer,
			  Text_arg, nText_args);

  /* Button */
  callback[0].callback = XHelp;
  old_w = help_wid;
  XtSetArg(Button_arg[2], XtNwidth, 235);
  XtSetArg(Button_arg[4], XtNhorizDistance, 160);
  XtSetArg(Button_arg[5], XtNvertDistance, 377);
  new_w = XtCreateManagedWidget("Help",
			commandWidgetClass, outer, Button_arg, NButton_args);
  XtRealizeWidget(interface);
}


void Draw_chart(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  draw_chart = TRUE;
}

void expose(widget,closure,event)
     Widget widget;
     caddr_t closure;		/* Widget */
     XEvent *event;
{
  XCopyArea(display, (Drawable) closure, XtWindow(widget), default_GC,
	    event->xexpose.x, event->xexpose.y, 
	    (unsigned) event->xexpose.width, (unsigned) event->xexpose.height,
	    event->xexpose.x, event->xexpose.y);
}

void close_window(widget,closure,event)
     Widget widget;
     caddr_t closure;		/* Widget */
     XEvent *event;
{
  if (XtParent((Widget) closure) == chart_shell) {
    killed_window = TRUE;
    killlines();
  };

  if (kill_the_window) {
    XtDestroyWidget(XtParent(chart_widget));
    if (use_backup) XFreePixmap(display, backup);
    un_help();
    D_comment("Closed");
  };
}

int is_killed()
{
  XEvent event;

  while (XtPending()) {
    XtNextEvent(&event);
    XtDispatchEvent(&event);
  }
  return (killed_window);
}

/* Setting translation tables */
char num_one_line_translations[] = "\
     Ctrl<Key>F:         forward-character() \n\
     Ctrl<Key>B:         backward-character() \n\
     Ctrl<Key>D:         delete-next-character() \n\
     Ctrl<Key>A:         beginning-of-line() \n\
     Ctrl<Key>E:         end-of-line() \n\
     Ctrl<Key>H:         delete-previous-character() \n\
     Ctrl<Key>K:         kill-to-end-of-line() \n\
     Ctrl<Key>J:         redraw-display() \n\
     Ctrl<Key>L:         redraw-display() \n\
     Ctrl<Key>M:         redraw-display() \n\
     Ctrl<Key>N:         redraw-display() \n\
     Ctrl<Key>O:         redraw-display() \n\
     Ctrl<Key>P:         redraw-display() \n\
     Ctrl<Key>V:         redraw-display() \n\
     Ctrl<Key>W:         kill-selection() \n\
     Ctrl<Key>Y:         unkill() \n\
     Ctrl<Key>Z:         redraw-display() \n\
     Meta<Key>F:         forward-word() \n\
     Meta<Key>B:         backward-word() \n\
     Meta<Key>Y:         stuff() \n\
     Meta<Key>Z:         redraw-display() \n\
     Meta<Key>I:         redraw-display() \n\
     Meta<Key>K:         redraw-display() \n\
     Meta<Key>V:         redraw-display() \n\
     :Meta<Key>d:        delete-next-word() \n\
     :Meta<Key>D:        kill-word() \n\
     :Meta<Key>h:        delete-previous-word() \n\
     :Meta<Key>H:        backward-kill-word() \n\
     :Meta<Key>\<:       redraw-display() \n\
     :Meta<Key>\>:       redraw-display() \n\
     :Meta<Key>]:        redraw-display() \n\
     :Meta<Key>[:        redraw-display() \n\
     ~Shift Meta<Key>Delete:delete-previous-word() \n\
      Shift Meta<Key>Delete:backward-kill-word() \n\
     ~Shift Meta<Key>BackSpace:delete-previous-word() \n\
      Shift Meta<Key>BackSpace:backward-kill-word() \n\
     <Key>Right:         forward-character() \n\
     <Key>Left:          backward-character() \n\
     <Key>Delete:        delete-previous-character() \n\
     <Key>BackSpace:     delete-previous-character() \n\
     <Key>Return:        redraw-display() \n\
     <Key>Linefeed:      redraw-display() \n\
     <Key>Down:          redraw-display() \n\
     <Key>Up:            redraw-display() \n\
     <Key>0: 		 insert-char() \n\
     <Key>1: 		 insert-char() \n\
     <Key>2: 		 insert-char() \n\
     <Key>3: 		 insert-char() \n\
     <Key>4: 		 insert-char() \n\
     <Key>5: 		 insert-char() \n\
     <Key>6: 		 insert-char() \n\
     <Key>7: 		 insert-char() \n\
     <Key>8: 		 insert-char() \n\
     <Key>9: 		 insert-char() \n\
     <Key>-: 		 insert-char() \n\
     Shift<Key>=: 	 insert-char() \n\
     <Key>.: 		 insert-char() \n\
     <FocusIn>:          focus-in() \n\
     <FocusOut>:         focus-out() \n\
     <Btn1Down>:         select-start() \n\
     <Btn1Motion>:       extend-adjust() \n\
     <Btn1Up>:           extend-end(PRIMARY, CUT_BUFFER0) \n\
     <Btn3Down>:         extend-start() \n\
     <Btn3Motion>:       extend-adjust() \n\
     <Btn3Up>:           extend-end(PRIMARY, CUT_BUFFER0)";

char str_one_line_translations[] = "\
     Ctrl<Key>F:         forward-character() \n\
     Ctrl<Key>B:         backward-character() \n\
     Ctrl<Key>D:         delete-next-character() \n\
     Ctrl<Key>A:         beginning-of-line() \n\
     Ctrl<Key>E:         end-of-line() \n\
     Ctrl<Key>H:         delete-previous-character() \n\
     Ctrl<Key>K:         kill-to-end-of-line() \n\
     Ctrl<Key>J:         redraw-display() \n\
     Ctrl<Key>L:         redraw-display() \n\
     Ctrl<Key>M:         redraw-display() \n\
     Ctrl<Key>N:         redraw-display() \n\
     Ctrl<Key>O:         redraw-display() \n\
     Ctrl<Key>P:         redraw-display() \n\
     Ctrl<Key>V:         redraw-display() \n\
     Ctrl<Key>W:         kill-selection() \n\
     Ctrl<Key>Y:         unkill() \n\
     Ctrl<Key>Z:         redraw-display() \n\
     Meta<Key>F:         forward-word() \n\
     Meta<Key>B:         backward-word() \n\
     Meta<Key>Y:         stuff() \n\
     Meta<Key>Z:         redraw-display() \n\
     Meta<Key>I:         redraw-display() \n\
     Meta<Key>K:         redraw-display() \n\
     Meta<Key>V:         redraw-display() \n\
     :Meta<Key>d:        delete-next-word() \n\
     :Meta<Key>D:        kill-word() \n\
     :Meta<Key>h:        delete-previous-word() \n\
     :Meta<Key>H:        backward-kill-word() \n\
     :Meta<Key>\<:       redraw-display() \n\
     :Meta<Key>\>:       redraw-display() \n\
     :Meta<Key>]:        redraw-display() \n\
     :Meta<Key>[:        redraw-display() \n\
     ~Shift Meta<Key>Delete:delete-previous-word() \n\
      Shift Meta<Key>Delete:backward-kill-word() \n\
     ~Shift Meta<Key>BackSpace:delete-previous-word() \n\
      Shift Meta<Key>BackSpace:backward-kill-word() \n\
     <Key>Right:         forward-character() \n\
     <Key>Left:          backward-character() \n\
     <Key>Delete:        delete-previous-character() \n\
     <Key>BackSpace:     delete-previous-character() \n\
     <Key>Return:        redraw-display() \n\
     <Key>Linefeed:      redraw-display() \n\
     <Key>Down:          redraw-display() \n\
     <Key>Up:            redraw-display() \n\
     <Key>: 		 insert-char() \n\
     <FocusIn>:          focus-in() \n\
     <FocusOut>:         focus-out() \n\
     <Btn1Down>:         select-start() \n\
     <Btn1Motion>:       extend-adjust() \n\
     <Btn1Up>:           extend-end(PRIMARY, CUT_BUFFER0) \n\
     <Btn3Down>:         extend-start() \n\
     <Btn3Motion>:       extend-adjust() \n\
     <Btn3Up>:           extend-end(PRIMARY, CUT_BUFFER0)";

/*     <Btn2Down>:         insert-selection(PRIMARY, CUT_BUFFER0) \n\*/


XtTranslations numbers_oneline, string_oneline;

set_translations()
{
  numbers_oneline = XtParseTranslationTable(num_one_line_translations);
  string_oneline = XtParseTranslationTable(str_one_line_translations);
}
