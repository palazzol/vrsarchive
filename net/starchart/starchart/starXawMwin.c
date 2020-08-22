/*
 * Mapwin edit dialogs for starXaw
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


static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starXawMwin.c,v 1.1 1990-03-30 16:39:35 vrs Exp $";

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

#define LINELEN 82

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

extern char *prog;

/* Externs */
extern int g_argc;
extern char **g_argv;

extern char *title;

/* From starchart.c */
extern double ra, de, sc;
extern double all_lbllim, all_maglim, all_gklim;
extern int use_lbllim, use_maglim, use_gklim;

extern double all_vmin, all_vmax;
extern int use_vmin;
extern int nomaglbls;

extern double all_rstep, all_dstep;
extern int use_rstep;
extern double all_rstrt, all_dstrt;
extern int no_ra_grid;
extern int no_dec_grid;

extern int all_invert;

extern int chart_type;

extern int all_proj_mode;

extern char *rcfile;

extern char *constfile;
extern char *boundfile;
extern char boundftype;
extern char *patternfile;
extern char pattftype;
extern char *cnamefile;
extern char cnameftype;
extern char *mapfiles[];
extern int mapftypes[];
extern int nummapfiles;

extern mapwindow *mapwin[];
extern int numwins;

extern int cur_function;
extern int cur_map_type;
extern int cur_map_tag;
extern char *cur_tag_field;
extern char *cur_file_name;

extern int read_mapwin_file;
extern int write_mapwin_file;
extern char mapwin_file[];


extern int all_layer[MAXLAYRS];
extern int numlayers;

/* storage area big enough for inputs */
#ifndef MAXPATHLEN
#define MAXPATHLEN 1025
#endif
extern char a_title[];
extern char a_starfile[];
extern char a_indexfile[];
extern char a_planetfile[];
extern char a_nebfile[];
extern char a_constfile[];
extern char a_boundfile[];
extern char a_patternfile[];
extern char a_cnamefile[];
extern char a_userfile[][MAXPATHLEN];


#define READ_MAPWIN 1
#define WRITE_MAPWIN 2
#define NO_MAPWIN 0

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
extern Bool use_x_dashes;
extern Bool is_color;		   /* Is it color? */
extern Bool is_super_color;	   /* Is it many color? */



extern Bool edit_mapwins;


/* From starXaw */
extern Widget interface;
extern XtTranslations numbers_oneline, string_oneline;
extern Bool fixed_win_coo;


/* From starXawDlog */
Widget Get_float();
Widget Get_string();
Widget Get_int();
void do_dismiss();
void update_string();
extern char *lay_strings[], *filetype_strings[];
#define MAXLAYNUM 14
#define MAXFTYPES 6

/* Local */
static Bool done_mapwin = False;
static int editting_mapwin = 0;
static int num_initted_mapwins;	/* Number of mapwins which have 
				   been initialized */

static char *loc_proj_mode_strings[] = {
  "Error",
  "Sansons",
  "Stereographic",
  "Gnomonic",
  "Orthographic",
  "Rectangular",
};
#define MAXPMODE 5


static int curr_file;

static char numwin_str[6];
static Widget numwin_wid;
/* edit_struct */
static char width_str[6], height_str[6], x_off_str[6], y_off_str[6];
static char ra_str[12], de_str[12], sc_str[12];
static char ra_step_str[12], de_step_str[12], ra_strt_str[12], de_strt_str[12];

static char stru_num_str[80];
static Widget stru_num_wid, next_map_wid, prev_map_wid;
static char file_num_str[80];
static Widget file_num_wid, next_file_wid, prev_file_wid;
static Widget width_wid, height_wid, x_off_wid, y_off_wid;
static Widget ra_wid, de_wid, sc_wid;
static Widget ra_step_wid, de_step_wid, ra_strt_wid, de_strt_wid;
static Widget fix_win_wid, inv_wid, pmode_wid, draw_ra_wid, draw_dec_wid;

static Widget layers_wid[MAXLAYRS];
static int laynum[MAXLAYRS];

static char numfiles_str[6];
static Widget numfiles_wid;

static char f_maglim_str[12], f_lbllim_str[12], f_gklim_str[12];
static char f_vmin_str[12], f_vmax_str[12];
static char file_name_str[MAXPATHLEN];
static Widget file_name_wid, f_type_wid;
static Widget f_maglim_wid, f_lbllim_wid, f_gklim_wid;
static Widget f_vmin_wid, f_vmax_wid, f_dmaglbl_wid;


void done_in(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  done_mapwin = True;

  XtPopdown((Widget) closure);
  un_help();
}

void do_win_dismiss(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  win_apply();

  XtPopdown((Widget) closure);
/*  un_help();*/
  mwinin_help();
}


void do_file_dismiss(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  file_apply();

  XtPopdown((Widget) closure);
/*  un_help();*/
  estru_help();
}

void do_lay_dismiss(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{

  XtPopdown((Widget) closure);
/*  un_help();*/
  estru_help();
}


void next_map(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  win_apply();
  
  editting_mapwin++;

  win_reset(closure);
/*  XtPopdown((Widget) closure);*/
}

void prev_map(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  win_apply();

  editting_mapwin--;

  win_reset(closure);
/*  XtPopdown((Widget) closure);*/
}

void nothing_map(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
}


void next_file(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  file_apply();
  curr_file++;
  file_reset(closure);
/*  XtPopdown((Widget) closure);*/
}

void prev_file(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  file_apply();
  curr_file--;
  file_reset(closure);
/*  XtPopdown((Widget) closure);*/
}

void nothing_file(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
}


void fixed_coo_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  fixed_win_coo = !fixed_win_coo;

  XtSetArg( args[0], XtNlabel, fixed_win_coo ?
			  "Use these settings":
			  "Allow X to set size");
  XtSetValues(widget, args, (Cardinal)1);
}

void invert_win_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  mapwin[editting_mapwin]->invert =
    !mapwin[editting_mapwin]->invert;

  XtSetArg( args[0], XtNlabel, 
	   mapwin[editting_mapwin]->invert?
	   "Inverted": "Normal");

  XtSetValues(widget, args, (Cardinal)1);
}

void win_pmode_select(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];
  char *cp;

  mapwin[editting_mapwin]->proj_mode++;
  mapwin[editting_mapwin]->proj_mode %= (MAXPMODE+1);
  if (mapwin[editting_mapwin]->proj_mode == 0)
    mapwin[editting_mapwin]->proj_mode = 1;

  cp = loc_proj_mode_strings[mapwin[editting_mapwin]->proj_mode];
  XtSetArg( args[0], XtNlabel, cp);

  XtSetValues(widget, args, (Cardinal)1);
}

void draw_ra_grid_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  mapwin[editting_mapwin]->draw_ragrid = !mapwin[editting_mapwin]->draw_ragrid;

  XtSetArg( args[0], XtNlabel, mapwin[editting_mapwin]->draw_ragrid ?
	   "Show R.A. grid":
	   "No R.A. grid");

  XtSetValues(widget, args, (Cardinal)1);
}

void draw_dec_grid_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  mapwin[editting_mapwin]->draw_decgrid
    = !mapwin[editting_mapwin]->draw_decgrid;

  XtSetArg( args[0], XtNlabel, mapwin[editting_mapwin]->draw_decgrid ?
	   "Show Dec. grid":
	   "No Dec. grid");

  no_dec_grid = !no_dec_grid;

  XtSetValues(widget, args, (Cardinal)1);
}

void draw_maglbl_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  mapwin[editting_mapwin]->file[curr_file].draw_maglbl = 
    !mapwin[editting_mapwin]->file[curr_file].draw_maglbl;

  XtSetArg(args[0], XtNlabel,
	   mapwin[editting_mapwin]->file[curr_file].draw_maglbl ?
	   "Label stars with their magnitudes      ":
	   "Don't Label stars with their magnitudes");

  XtSetValues(widget, args, (Cardinal)1);
}


D_mapwininput()
{
  int i;
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "mapwininput";
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, dismiss_widge;
  void edit_structs();

  /* First do final customization of the mapwin */
  /* This is necessary because fix_mapwin() in starcust.c,
     and the code in main() is not quite appropriate for undoing things set 
     interactively */
  for (i = 0; i < numwins; i++) {
    if (!all_invert && mapwin[i]->invert) mapwin[i]->invert = FALSE;
  }

  if (edit_mapwins) {
    num_initted_mapwins = numwins;

    XtSetArg( Button_arg[0], XtNcallback, callback );
    NButton_args = 1;

    /* Labels should be left justified, and not have borders */
    XtSetArg( Label_arg[0], XtNborderWidth, 0);
    XtSetArg( Label_arg[1], XtNjustify, XtJustifyLeft);
    NLabel_args = 2;


    /* Create shell and shell widget */
    pshell = XtCreatePopupShell(dlog_name,
				topLevelShellWidgetClass,
				interface, shell_args, XtNumber(shell_args));
    pwidg = 
      XtCreateManagedWidget(dlog_name, shellWidgetClass, pshell, NULL, 0);

    /* Create this dialog box */
    pform = 
      XtCreateManagedWidget(dlog_name, formWidgetClass, pwidg, NULL, 0);


    /* Create widgets in the box
       Each has coordinates specified by either XtNfromHoriz and XtNfromVert
       or XtNhorizDistance and XtNvertDistance
       arg[1] sets horizontal position, arg[2] sets vertical
       Each has a callback if appropriate
       */


    twidge1 = 
      XtCreateManagedWidget("Edit mapwindow structures",
			    labelWidgetClass, pform,
			    Label_arg, NLabel_args);

    numwin_wid = 
      twidge2 =
	Get_int("Number of window structures:",
		twidge1, pform, numwins, numwin_str, sizeof(numwin_str));
    twidge1 = twidge2;

    callback[0].callback = edit_structs;
    callback[0].closure = (caddr_t)pwidg;
    XtSetArg(Button_arg[1], XtNhorizDistance,  4);
    XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
    NButton_args = 3;
    twidge2 = 
      XtCreateManagedWidget( "edit struct",
			  commandWidgetClass, pform, Button_arg, NButton_args);
    twidge1 = twidge2;


    bottom_widge = twidge1;	/* The bottommost widget so far */

    callback[0].callback = done_in;
    callback[0].closure = (caddr_t)pwidg;
    XtSetArg(Button_arg[1], XtNhorizDistance,  4);
    XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
    NButton_args = 3;
    dismiss_widge = 
      XtCreateManagedWidget( "Dismiss",
			  commandWidgetClass, pform, Button_arg, NButton_args);

    /* Save the current values */
    XtPopup(pwidg, XtGrabNonexclusive);
    mwinin_help();
    done_mapwin = FALSE;
    while (!done_mapwin) {
      XtNextEvent(&event);
      XtDispatchEvent(&event);
    };
  };
}


/* edit structures */
void edit_structs(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "edit structs";
  Arg Button_arg[10], Text_arg[10], Label_arg[10];
  Cardinal NButton_args, NText_args, NLabel_args;
  static XtCallbackRec callback[2];
  static XtCallbackRec next_callback[3], prev_callback[3];
  Widget twidge1, twidge2;
  Widget bottom_widge, dismiss_widge;
  int i;
  char *cp;
  void edit_layers();
  void edit_files();

  XtSetArg( Button_arg[0], XtNcallback, callback );
  NButton_args = 1;

  /* Labels should be left justified, and not have borders */
  XtSetArg( Label_arg[0], XtNborderWidth, 0);
  XtSetArg( Label_arg[1], XtNjustify, XtJustifyLeft);
  NLabel_args = 2;

  pshell = XtCreatePopupShell(dlog_name,
			      topLevelShellWidgetClass,
			      widget, shell_args, XtNumber(shell_args));
  pwidg = 
    XtCreateManagedWidget(dlog_name, shellWidgetClass, pshell, NULL, 0);

  /* Create this dialog box */
  pform = 
    XtCreateManagedWidget(dlog_name, formWidgetClass, pwidg, NULL, 0);


  /* Create widgets in the box
     Each has coordinates specified by either XtNfromHoriz and XtNfromVert
     or XtNhorizDistance and XtNvertDistance
     arg[1] sets horizontal position, arg[2] sets vertical
     Each has a callback if appropriate
     */


  /* convert numwin_str to numwins */
  numwins = atoi(numwin_str);
  if (numwins > MAXWINDOWS) {
    numwins = MAXWINDOWS;
    sprintf(numwin_str, "%d", numwins);
    update_string(numwin_wid, numwin_str);
  }

  /* Set curr_file to zero for each new mapwindow */
  curr_file = 0;


  /* Create shell and shell widget */
  if (editting_mapwin >= num_initted_mapwins)
    init_mapwin(editting_mapwin);

  /* Must make sure that there is a file name */
  for (i = 0; i < MAXMAPFILES; i++) {
     if (mapwin[editting_mapwin]->file[i].name == NULL)
       mapwin[editting_mapwin]->file[i].name = "";
     if (mapwin[editting_mapwin]->file[i].type == 0)
       mapwin[editting_mapwin]->file[i].type = LINEREAD;
   }

  XtSetArg(Text_arg[0], XtNlength, sizeof(stru_num_str));
  XtSetArg(Text_arg[1], XtNstring, stru_num_str);
  XtSetArg(Text_arg[2], XtNwidth, 208);
  XtSetArg(Text_arg[3], XtNborderWidth, 0);
  NText_args = 4;
  sprintf(stru_num_str, "Structure number %d", editting_mapwin+1);
  stru_num_wid =
    twidge2 = 
      XtCreateManagedWidget(stru_num_str,
			    asciiStringWidgetClass, pform, 
			    Text_arg, NText_args);
  twidge1 = twidge2;


  width_wid = 
    twidge2 =
      Get_int("Width:          ",
	      twidge1, pform,
	      mapwin[editting_mapwin]->width, width_str, sizeof(width_str));
  twidge1 = twidge2;

  height_wid =
    twidge2 =
      Get_int("Height:         ",
	      twidge1, pform,
	      mapwin[editting_mapwin]->height, height_str, sizeof(height_str));
  twidge1 = twidge2;

  x_off_wid = 
    twidge2 =
      Get_int("X offset:       ",
	      twidge1, pform,
	      mapwin[editting_mapwin]->x_offset, x_off_str, sizeof(x_off_str));
  twidge1 = twidge2;

  y_off_wid = 
    twidge2 =
      Get_int("Y offset:       ",
	      twidge1, pform,
	      mapwin[editting_mapwin]->y_offset, y_off_str, sizeof(y_off_str));
  twidge1 = twidge2;

  /* Fixed window coordinates */
  callback[0].callback = fixed_coo_toggle;
  callback[0].closure = (caddr_t) "fixwin";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  XtSetArg(Button_arg[3], XtNwidth, 208);
  NButton_args = 4;

  fix_win_wid =
    twidge2 =
      XtCreateManagedWidget(fixed_win_coo ?
			    "Use these settings":
			    "Allow X to set size",
			  commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;


  /* get RA */
  ra_wid =
    twidge2 = Get_float("Right Asc.:     ", twidge1, pform,
			dtof(mapwin[editting_mapwin]->racen/15.0),
			ra_str,
			sizeof(ra_str));
  twidge1 = twidge2;

  /* get DEC */
  de_wid =
    twidge2 = Get_float("Declination:    ", twidge1, pform,
			dtof(mapwin[editting_mapwin]->dlcen),
			de_str,
			sizeof(de_str));
  twidge1 = twidge2;

  /* Get Scale */
  sc_wid =
    twidge2 = Get_float("Scale:          ", twidge1, pform,
			mapwin[editting_mapwin]->scale,
			sc_str,
			sizeof(sc_str));
  twidge1 = twidge2;

  /* Toggle invert */
  callback[0].callback = invert_win_toggle;
  callback[0].closure = (caddr_t) "Invert";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  XtSetArg(Button_arg[3], XtNwidth, 208);
  NButton_args = 4;
  inv_wid =
    twidge2 =
      XtCreateManagedWidget(mapwin[editting_mapwin]->invert?
			    "Inverted": "Normal",
			  commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;

  /* Projection mode */
  callback[0].callback = win_pmode_select;
  callback[0].closure = (caddr_t) "projection_mode";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);

  cp = loc_proj_mode_strings[mapwin[editting_mapwin]->proj_mode];

  pmode_wid =
    twidge2 =
      XtCreateManagedWidget(cp,
			  commandWidgetClass, pform, Button_arg, NButton_args);
  twidge1 = twidge2;

/*  twidge1 = 
    XtCreateManagedWidget("Specify Grid",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);
*/

  if (mapwin[editting_mapwin]->ra_step == 0)
    mapwin[editting_mapwin]->ra_step = 15.0;
  if (mapwin[editting_mapwin]->dec_step == 0)
    mapwin[editting_mapwin]->dec_step = 5.0;

  /* Get ra_step */
  ra_step_wid =
    twidge2 = Get_float("R.A. Step:      ", twidge1, pform,
			dtof(mapwin[editting_mapwin]->ra_step/15.0),
			ra_step_str,
			sizeof(ra_step_str));
  twidge1 = twidge2;

  /* Get dec_step */
  de_step_wid =
    twidge2 = Get_float("Dec. Step:      ", twidge1, pform,
			dtof(mapwin[editting_mapwin]->dec_step),
			de_step_str,
			sizeof(de_step_str));
  twidge1 = twidge2;


  /* Get ra_strt */
  ra_strt_wid =
    twidge2 = Get_float("R.A. Start:     ", twidge1, pform,
			dtof(mapwin[editting_mapwin]->ra_strt/15.0),
			ra_strt_str,
			sizeof(ra_strt_str));
  twidge1 = twidge2;

  /* Get dec_strt */
  de_strt_wid =
    twidge2 = Get_float("Dec. Start:     ", twidge1, pform,
			dtof(mapwin[editting_mapwin]->dec_strt),
			de_strt_str,
			sizeof(de_strt_str));
  twidge1 = twidge2;


  /* Toggle ra */
  callback[0].callback = draw_ra_grid_toggle;
  callback[0].closure = (caddr_t) "draw_ra_grid";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  draw_ra_wid =
    twidge2 =
      XtCreateManagedWidget(mapwin[editting_mapwin]->draw_ragrid ?
			    "Show R.A. grid":
			    "No R.A. grid",
			  commandWidgetClass, pform, Button_arg, NButton_args);
  twidge1 = twidge2;

  /* Toggle dec */
  callback[0].callback = draw_dec_grid_toggle;
  callback[0].closure = (caddr_t) "draw_dec_grid";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  draw_dec_wid =
    twidge2 =
      XtCreateManagedWidget(mapwin[editting_mapwin]->draw_decgrid ?
			    "Show Dec. grid":
			    "No Dec. grid",
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;



  callback[0].callback = edit_layers;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[0], XtNcallback, callback);
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  XtSetArg(Button_arg[3], XtNwidth, 208);
  NButton_args = 4;
  twidge2 = 
    XtCreateManagedWidget( "Edit Layers",
			  commandWidgetClass, pform, Button_arg, NButton_args);
  twidge1 = twidge2;

  numfiles_wid = 
    twidge2 =
      Get_int("Number of files:",
	      twidge1, pform, mapwin[editting_mapwin]->numfiles,
	      numfiles_str, sizeof(numfiles_str));
  twidge1 = twidge2;

  callback[0].callback = edit_files;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[0], XtNcallback, callback);
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  twidge2 = 
    XtCreateManagedWidget( "Edit Files",
			  commandWidgetClass, pform, Button_arg, NButton_args);
  twidge1 = twidge2;


  bottom_widge = twidge1;	/* The bottommost widget so far */

  if (editting_mapwin < (numwins-1)) {
    next_callback[0].callback = next_map;
    next_callback[0].closure = (caddr_t)pwidg;
/*
    next_callback[1].callback = edit_structs;
    next_callback[1].closure = closure;
*/
    next_callback[1].callback = NULL;
    next_callback[1].closure = NULL;
    cp = "Next";
  } else {
    next_callback[0].callback = nothing_map;
    next_callback[0].closure = (caddr_t)pwidg;
    next_callback[1].callback = NULL;
    next_callback[1].closure = NULL;
    cp = "No Next";
  }
  XtSetArg(Button_arg[0], XtNcallback, next_callback);
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert, bottom_widge);
  XtSetArg(Button_arg[3], XtNwidth, 101);
  next_map_wid =
    twidge2 = 
      XtCreateManagedWidget(cp,
			  commandWidgetClass, pform, Button_arg, NButton_args);
  twidge1 = twidge2;


  if (editting_mapwin > 0) {
    prev_callback[0].callback = prev_map;
    prev_callback[0].closure = (caddr_t)pwidg;
/*
    prev_callback[1].callback = edit_structs;
    prev_callback[1].closure = closure;
*/
    prev_callback[1].callback = NULL;
    prev_callback[1].closure = NULL;
    cp = "Prev";
  } else {
    prev_callback[0].callback = nothing_map;
    prev_callback[0].closure = (caddr_t)pwidg;
    prev_callback[1].callback = NULL;
    prev_callback[1].closure = NULL;
    cp = "No Prev";
  }
  XtSetArg(Button_arg[0], XtNcallback, prev_callback);
  XtSetArg(Button_arg[1], XtNfromHoriz, twidge1);
  XtSetArg(Button_arg[2], XtNfromVert, bottom_widge);
  prev_map_wid =
    twidge2 = 
      XtCreateManagedWidget( cp,
			  commandWidgetClass, pform, Button_arg, NButton_args);
  twidge1 = twidge2;

  bottom_widge = twidge1;	/* The bottommost widget so far */


  callback[0].callback = do_win_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[0], XtNcallback, callback);
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert, bottom_widge);
  XtSetArg(Button_arg[3], XtNwidth, 208);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  XtPopup(pwidg, XtGrabExclusive);
  estru_help();
}

init_mapwin(win_no)
int win_no;
{
  int i;

  mapwin[win_no] = (mapwindow *) malloc((unsigned) sizeof(mapwindow));
  /* Copy the values from mapwin[0] to this mapwin */

  mapwin[win_no]->width = mapwin[0]->width;
  mapwin[win_no]->height = mapwin[0]->height;
  mapwin[win_no]->x_offset = mapwin[0]->x_offset;
  mapwin[win_no]->y_offset = mapwin[0]->y_offset;
		
  mapwin[win_no]->maglim = mapwin[0]->maglim;
  mapwin[win_no]->lbllim = mapwin[0]->lbllim;
  mapwin[win_no]->gklim = mapwin[0]->gklim;


  mapwin[win_no]->map_type = mapwin[0]->map_type;
  mapwin[win_no]->tag = mapwin[0]->tag;
  mapwin[win_no]->tag_field = mapwin[0]->tag_field;

  mapwin[win_no]->proj_mode = mapwin[0]->proj_mode;
  mapwin[win_no]->draw_ragrid = mapwin[0]->draw_ragrid;
  mapwin[win_no]->draw_decgrid = mapwin[0]->draw_decgrid;
  mapwin[win_no]->ra_step = mapwin[0]->ra_step;
  mapwin[win_no]->dec_step = mapwin[0]->dec_step;
  mapwin[win_no]->ra_strt = mapwin[0]->ra_strt;
  mapwin[win_no]->dec_strt = mapwin[0]->dec_strt;

  mapwin[win_no]->invert = mapwin[0]->invert;

  mapwin[win_no]->racen = mapwin[0]->racen;
  mapwin[win_no]->dlcen = mapwin[0]->dlcen;
  mapwin[win_no]->scale = mapwin[0]->scale;

  mapwin[win_no]->c_scale = mapwin[0]->c_scale;
   for (i = 0; i < MAXLAYRS; i++)
     mapwin[win_no]->layer[i] = mapwin[0]->layer[i];
  mapwin[win_no]->nlayers = mapwin[0]->nlayers;

   for (i = 0; i < MAXMAPFILES; i++) {
     if (mapwin[0]->file[i].name != NULL)
       mapwin[win_no]->file[i].name = mapwin[0]->file[i].name;
     else
       mapwin[win_no]->file[i].name = "";
     mapwin[win_no]->file[i].type = mapwin[0]->file[i].type;
     mapwin[win_no]->file[i].maglim = mapwin[0]->file[i].maglim;
     mapwin[win_no]->file[i].lbllim = mapwin[0]->file[i].lbllim;
     mapwin[win_no]->file[i].gklim = mapwin[0]->file[i].gklim;

     mapwin[win_no]->file[i].draw_maglbl = mapwin[0]->file[i].draw_maglbl;
     mapwin[win_no]->file[i].maglmin = mapwin[0]->file[i].maglmin;
     mapwin[win_no]->file[i].maglmax = mapwin[0]->file[i].maglmax;
   }

  mapwin[win_no]->numfiles = mapwin[0]->numfiles;

  num_initted_mapwins = win_no+1;
}


/********** Dialog ***************/
/* Layers dialog
   Set mapwin[editting_mapwin].layer
*/

void edit_layers(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "layers";
  void layer_select();
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, apply_widge, dismiss_widge;
  int i;
  char *cp;
  void mlayer_select();
  void mlayers_apply();

  XtSetArg( Button_arg[0], XtNcallback, callback );
  NButton_args = 1;

  /* Labels should be left justified, and not have borders */
  XtSetArg( Label_arg[0], XtNborderWidth, 0);
  XtSetArg( Label_arg[1], XtNjustify, XtJustifyLeft);
  NLabel_args = 2;


  /* Create shell and shell widget */
  pshell = XtCreatePopupShell(dlog_name,
			      topLevelShellWidgetClass,
			      widget, shell_args, XtNumber(shell_args));
  pwidg = 
    XtCreateManagedWidget(dlog_name, shellWidgetClass, pshell, NULL, 0);

  /* Create this dialog box */
  pform = 
    XtCreateManagedWidget(dlog_name, formWidgetClass, pwidg, NULL, 0);


  /* Create widgets in the box
     Each has coordinates specified by either XtNfromHoriz and XtNfromVert
     or XtNhorizDistance and XtNvertDistance
     arg[1] sets horizontal position, arg[2] sets vertical
     Each has a callback if appropriate
     */


  twidge1 = 
    XtCreateManagedWidget("Specify Layer drawing order",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);

  for (i = 0; i < MAXLAYRS; i++) {
    laynum[i] = i;
    
    callback[0].callback = mlayer_select;
    callback[0].closure = (caddr_t) ((int *) &(laynum[i]));
    XtSetArg(Button_arg[0], XtNcallback, callback);
    XtSetArg(Button_arg[1], XtNhorizDistance,  4);
    XtSetArg(Button_arg[2], XtNfromVert,  twidge1);

    NButton_args = 3;
    cp = lay_strings[mapwin[editting_mapwin]->layer[i]];
    layers_wid[i] = 
      twidge2 =
	XtCreateManagedWidget(cp,
			      commandWidgetClass, pform,
			      Button_arg, NButton_args);

    twidge1 = twidge2;
  }

  bottom_widge = twidge1;	/* The bottommost widget so far */


  
  callback[0].callback = mlayers_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);
  bottom_widge = apply_widge;

  callback[0].callback = do_lay_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[0], XtNcallback, callback);
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  /* Save the current values */
  XtPopup(pwidg, XtGrabExclusive);
  elay_help();
}

void mlayer_select(widget, laynump, callData)
    Widget widget;
    caddr_t laynump;
    caddr_t callData;
{
  Arg args[1];
  char *cp;
  int i;

  i = *((int* ) laynump);

  mapwin[editting_mapwin]->layer[i]++;
  mapwin[editting_mapwin]->layer[i] %= (MAXLAYNUM+1);

  cp = lay_strings[mapwin[editting_mapwin]->layer[i]];
  XtSetArg( args[0], XtNlabel, cp);

  XtSetValues(widget, args, (Cardinal)1);
}

void mlayers_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  int i, nlayers;
  Arg args[1];
  char *cp;

  for (i = 0, nlayers = 0; i < MAXLAYRS; i++)
    if (mapwin[editting_mapwin]->layer[i] != 0)
      mapwin[editting_mapwin]->layer[nlayers++]
	= mapwin[editting_mapwin]->layer[i];
  for (i = nlayers;i < MAXLAYRS; i++) mapwin[editting_mapwin]->layer[i] = 0;
  mapwin[editting_mapwin]->nlayers = nlayers;

  for (i = 0; i < MAXLAYRS; i++) {
    cp = lay_strings[mapwin[editting_mapwin]->layer[i]];
    XtSetArg(args[0], XtNlabel, cp);

    XtSetValues(layers_wid[i], args, (Cardinal)1);
  }
}

void ftype_win_select(widget, mapnump, callData)
    Widget widget;
    caddr_t mapnump;
    caddr_t callData;
{
  Arg args[1];
  char *cp;

  mapwin[editting_mapwin]->file[curr_file].type++;
  mapwin[editting_mapwin]->file[curr_file].type %= (MAXFTYPES+1);
  if (mapwin[editting_mapwin]->file[curr_file].type == 0)
    mapwin[editting_mapwin]->file[curr_file].type = 1;

  cp = filetype_strings[mapwin[editting_mapwin]->file[curr_file].type];
  XtSetArg( args[0], XtNlabel, cp);

  XtSetValues(widget, args, (Cardinal)1);
}



/* edit file structures */
void edit_files(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "edit files";
  Arg Button_arg[10], Text_arg[10], Label_arg[10];
  Cardinal NButton_args, NText_args, NLabel_args;
  static XtCallbackRec callback[2];
  static XtCallbackRec next_callback[3], prev_callback[3];
  Widget twidge1, twidge2;
  Widget bottom_widge, higher_widge, dismiss_widge;
  char *cp;
  int i;

  XtSetArg( Button_arg[0], XtNcallback, callback );
  NButton_args = 1;

  /* Labels should be left justified, and not have borders */
  XtSetArg( Label_arg[0], XtNborderWidth, 0);
  XtSetArg( Label_arg[1], XtNjustify, XtJustifyLeft);
  NLabel_args = 2;

  /* Create shell and shell widget */
  pshell = XtCreatePopupShell(dlog_name,
			      topLevelShellWidgetClass,
			      widget, shell_args, XtNumber(shell_args));
  pwidg = 
    XtCreateManagedWidget(dlog_name, shellWidgetClass, pshell, NULL, 0);

  /* Create this dialog box */
  pform = 
    XtCreateManagedWidget(dlog_name, formWidgetClass, pwidg, NULL, 0);


  /* Create widgets in the box
     Each has coordinates specified by either XtNfromHoriz and XtNfromVert
     or XtNhorizDistance and XtNvertDistance
     arg[1] sets horizontal position, arg[2] sets vertical
     Each has a callback if appropriate
     */


  /* convert numfiles_str to numfiles */
  i = atoi(numfiles_str);
  if (i > MAXMAPFILES) {
    i = MAXMAPFILES;
    sprintf(numfiles_str, "%d", i);
    update_string(numfiles_wid, numfiles_str);
  }
  mapwin[editting_mapwin]->numfiles = i;


  XtSetArg(Text_arg[0], XtNlength, sizeof(file_num_str));
  XtSetArg(Text_arg[1], XtNstring, file_num_str);
  XtSetArg(Text_arg[2], XtNwidth, 200);
  XtSetArg(Text_arg[3], XtNborderWidth, 0);
  NText_args = 4;
  sprintf(file_num_str, "File number %d", curr_file+1);
  file_num_wid =
    twidge2 = 
      XtCreateManagedWidget(file_num_str,
			    asciiStringWidgetClass, pform, 
			    Text_arg, NText_args);

  twidge1 = twidge2;

  higher_widge = twidge1;
  /* File name and type */
  strcpy(file_name_str, mapwin[editting_mapwin]->file[curr_file].name);


  file_name_wid =
    twidge2 = Get_string("File:", twidge1, pform,
			 file_name_str,
			 MAXPATHLEN, 300, 300);
  twidge1 = twidge2;


  XtSetArg(Label_arg[2], XtNfromHoriz, twidge1);
  XtSetArg(Label_arg[3], XtNfromVert, higher_widge);
  NLabel_args = 4;

  twidge2 = 
    XtCreateManagedWidget(" Type:",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);
  twidge1 = twidge2;

  callback[0].callback = ftype_win_select;
  callback[0].closure = (caddr_t) "fwin";
  XtSetArg(Button_arg[0], XtNcallback, callback);
  XtSetArg(Button_arg[1], XtNfromHoriz, twidge1);
  XtSetArg(Button_arg[2], XtNfromVert,  higher_widge);

  NButton_args = 3;
  cp = filetype_strings[mapwin[editting_mapwin]->file[curr_file].type];
  f_type_wid =
    twidge2 =
      XtCreateManagedWidget(cp,
			    commandWidgetClass, pform,
			    Button_arg, NButton_args);
  twidge1 = twidge2;

  /* Magnitudes */
  /* Get name limit (lbllim) */
  XtSetArg(Label_arg[2], XtNfromVert, twidge1);
  NLabel_args = 3;

  twidge2 = 
    XtCreateManagedWidget("Magnitude limits:",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);
  twidge1 = twidge2;

  f_lbllim_wid =
    twidge2 = Get_float("Object names:  ", twidge1, pform,
			mapwin[editting_mapwin]->file[curr_file].lbllim,
			f_lbllim_str,
			sizeof(f_lbllim_str));
  twidge1 = twidge2;

  /* Get label limit (gklim) */
  f_gklim_wid =
    twidge2 = Get_float("Bayer labels:  ", twidge1, pform,
			mapwin[editting_mapwin]->file[curr_file].gklim,
			f_gklim_str,
			sizeof(f_gklim_str));
  twidge1 = twidge2;

  /* Get visibility limit (maglim) */
  f_maglim_wid =
    twidge2 = Get_float("Object symbol: ", twidge1, pform,
			mapwin[editting_mapwin]->file[curr_file].maglim,
			f_maglim_str,
			sizeof(f_maglim_str));
  twidge1 = twidge2;


  XtSetArg(Label_arg[2], XtNfromVert, twidge1);
  NLabel_args = 3;
  twidge2 = 
    XtCreateManagedWidget("Star magnitude labeling:",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);
  twidge1 = twidge2;

  f_vmin_wid =
    twidge2 = Get_float("Brightest:     ", twidge1, pform,
			mapwin[editting_mapwin]->file[curr_file].maglmin,
			f_vmin_str,
			sizeof(f_vmin_str));
  twidge1 = twidge2;

  /* Get maximum */
  f_vmax_wid =
    twidge2 = Get_float("Faintest:      ", twidge1, pform,
			mapwin[editting_mapwin]->file[curr_file].maglmax,
			f_vmax_str,
			sizeof(f_vmax_str));
  twidge1 = twidge2;


  /* Toggle nomaglabls */
  callback[0].callback = draw_maglbl_toggle;
  callback[0].closure = (caddr_t) "draw mablbls";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;

  f_dmaglbl_wid =
    twidge2 =
      XtCreateManagedWidget(
			 mapwin[editting_mapwin]->file[curr_file].draw_maglbl ?
			 "Label stars with their magnitudes      ":
			 "Don't Label stars with their magnitudes",
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;



  bottom_widge = twidge1;	/* The bottommost widget so far */

  if (curr_file < (mapwin[editting_mapwin]->numfiles-1)) {
    next_callback[0].callback = next_file;
    next_callback[0].closure = (caddr_t)pwidg;
    next_callback[1].callback = NULL;
    next_callback[1].closure = NULL;
/*
    next_callback[1].callback = edit_files;
    next_callback[1].closure = closure;
*/
    cp = "   Next";
  } else {
    next_callback[0].callback = nothing_file;
    next_callback[0].closure = (caddr_t)pwidg;
    next_callback[1].callback = NULL;
    next_callback[1].closure = NULL;
    cp = "No Next";
  }
  XtSetArg(Button_arg[0], XtNcallback, next_callback);
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert, bottom_widge);
  NButton_args = 3;
  next_file_wid =
    twidge2 = 
      XtCreateManagedWidget(cp,
			  commandWidgetClass, pform, Button_arg, NButton_args);
  twidge1 = twidge2;


  if (curr_file > 0) {
    prev_callback[0].callback = prev_file;
    prev_callback[0].closure = (caddr_t)pwidg;
    prev_callback[1].callback = NULL;
    prev_callback[1].closure = NULL;
/*
    prev_callback[1].callback = edit_files;
    prev_callback[1].closure = closure;
*/
    cp = "   Prev";
  } else {
    prev_callback[0].callback = nothing_map;
    prev_callback[0].closure = (caddr_t)pwidg;
    prev_callback[1].callback = NULL;
    prev_callback[1].closure = NULL;
    cp = "No Prev";
  }
  XtSetArg(Button_arg[0], XtNcallback, prev_callback);
  XtSetArg(Button_arg[1], XtNfromHoriz, twidge1);
  XtSetArg(Button_arg[2], XtNfromVert, bottom_widge);
  NButton_args = 3;
  prev_file_wid =
    twidge2 = 
      XtCreateManagedWidget( cp,
			  commandWidgetClass, pform, Button_arg, NButton_args);
  twidge1 = twidge2;

  bottom_widge = twidge1;	/* The bottommost widget so far */


  callback[0].callback = do_file_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[0], XtNcallback, callback);
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert, bottom_widge);
  NButton_args = 3;
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  XtPopup(pwidg, XtGrabExclusive);
  efiles_help();
}

file_apply()
{
  char *cp;

  if (strcmp(file_name_str, mapwin[editting_mapwin]->file[curr_file].name))
    if (cp = (char *) malloc(strlen(file_name_str))) {
      mapwin[editting_mapwin]->file[curr_file].name = cp;
      strcpy(cp, file_name_str);
    }
  mapwin[editting_mapwin]->file[curr_file].maglim = atof(f_maglim_str);
  mapwin[editting_mapwin]->file[curr_file].lbllim = atof(f_lbllim_str);
  mapwin[editting_mapwin]->file[curr_file].gklim = atof(f_gklim_str);
  mapwin[editting_mapwin]->file[curr_file].maglmin = atof(f_vmin_str);
  mapwin[editting_mapwin]->file[curr_file].maglmax = atof(f_vmax_str);
}



win_apply()
{

  mapwin[editting_mapwin]->width = atoi(width_str);
  mapwin[editting_mapwin]->height = atoi(height_str);
  mapwin[editting_mapwin]->x_offset = atoi(x_off_str);
  mapwin[editting_mapwin]->y_offset = atoi(y_off_str);
  mapwin[editting_mapwin]->racen = htod(ra_str)*15.0;
  mapwin[editting_mapwin]->dlcen = htod(de_str);
  mapwin[editting_mapwin]->scale = atof(sc_str);

  mapwin[editting_mapwin]->ra_step = htod(ra_step_str)*15.0;
  mapwin[editting_mapwin]->dec_step = htod(de_step_str);
  mapwin[editting_mapwin]->ra_strt = htod(ra_strt_str)*15.0;
  mapwin[editting_mapwin]->dec_strt = htod(de_strt_str);
}


/* win_reset must do everything edit_structs does on entry */
win_reset(closure)
    caddr_t closure;		/* Widget */
{
  Arg Label_arg[10];
  int i;
  char *cp;
  static XtCallbackRec next_callback[3], prev_callback[3];

  /* convert numwin_str to numwins */
  numwins = atoi(numwin_str);
  if (numwins > MAXWINDOWS) {
    numwins = MAXWINDOWS;
    sprintf(numwin_str, "%d", numwins);
    update_string(numwin_wid, numwin_str);
  }

  /* Set curr_file to zero for each new mapwindow */
  curr_file = 0;


  /* Create shell and shell widget */
  if (editting_mapwin >= num_initted_mapwins)
    init_mapwin(editting_mapwin);

  /* Must make sure that there is a file name */
  for (i = 0; i < MAXMAPFILES; i++) {
     if (mapwin[editting_mapwin]->file[i].name == NULL)
       mapwin[editting_mapwin]->file[i].name = "";
     if (mapwin[editting_mapwin]->file[i].type == 0)
       mapwin[editting_mapwin]->file[i].type = LINEREAD;
   }

  sprintf(stru_num_str, "Structure number %d", editting_mapwin+1);
  XtSetArg(Label_arg[0], XtNlabel, stru_num_str);
  XtSetValues(stru_num_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(stru_num_wid, strlen(stru_num_str));

  if (editting_mapwin < (numwins-1)) {
    next_callback[0].callback = next_map;
    next_callback[0].closure = closure;
    next_callback[1].callback = NULL;
    next_callback[1].closure = NULL;
    cp = "Next";
  } else {
    next_callback[0].callback = nothing_map;
    next_callback[0].closure = closure;
    next_callback[1].callback = NULL;
    next_callback[1].closure = NULL;
    cp = "No Next";
  }
  XtSetArg(Label_arg[0], XtNcallback, next_callback);
  XtSetArg(Label_arg[1], XtNlabel, cp);
  XtSetValues(next_map_wid, Label_arg, (Cardinal)2);


  if (editting_mapwin > 0) {
    prev_callback[0].callback = prev_map;
    prev_callback[0].closure = closure;
    prev_callback[1].callback = NULL;
    prev_callback[1].closure = NULL;
    cp = "Prev";
  } else {
    prev_callback[0].callback = nothing_map;
    prev_callback[0].closure = closure;
    prev_callback[1].callback = NULL;
    prev_callback[1].closure = NULL;
    cp = "No Prev";
  }
  XtSetArg(Label_arg[0], XtNcallback, prev_callback);
  XtSetArg(Label_arg[1], XtNlabel, cp);
  XtSetValues(prev_map_wid, Label_arg, (Cardinal)2);



  sprintf(width_str,"%d", mapwin[editting_mapwin]->width);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(width_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(width_wid, strlen(width_str));

  sprintf(height_str,"%d", mapwin[editting_mapwin]->height);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(height_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(height_wid, strlen(height_str));

  sprintf(x_off_str,"%d", mapwin[editting_mapwin]->x_offset);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(x_off_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(x_off_wid, strlen(x_off_str));

  sprintf(y_off_str,"%d", mapwin[editting_mapwin]->y_offset);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(y_off_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(y_off_wid, strlen(y_off_str));

  XtSetArg(Label_arg[0], XtNlabel, fixed_win_coo ?
	   "Use these settings":
	   "Allow X to set size");
  XtSetValues(fix_win_wid, Label_arg, (Cardinal)1);

  sprintf(ra_str, "%.4f", dtof(mapwin[editting_mapwin]->racen/15.0));
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(ra_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(ra_wid, strlen(ra_str));

  sprintf(de_str, "%.4f", dtof(mapwin[editting_mapwin]->dlcen));
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(de_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(de_wid, strlen(de_str));

  sprintf(sc_str, "%.4f", mapwin[editting_mapwin]->scale);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(sc_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(sc_wid, strlen(sc_str));


  XtSetArg(Label_arg[0], XtNlabel,mapwin[editting_mapwin]->invert?
			    "Inverted": "Normal");
  XtSetValues(inv_wid, Label_arg, (Cardinal)1);

  XtSetArg(Label_arg[0], XtNlabel,
	   loc_proj_mode_strings[mapwin[editting_mapwin]->proj_mode]);
  XtSetValues(pmode_wid, Label_arg, (Cardinal)1);

  sprintf(ra_step_str, "%.4f", dtof(mapwin[editting_mapwin]->ra_step/15.0));
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(ra_step_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(ra_step_wid, strlen(ra_step_str));

  sprintf(de_step_str, "%.4f", dtof(mapwin[editting_mapwin]->dec_step));
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(de_step_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(de_step_wid, strlen(de_step_str));

  sprintf(ra_strt_str, "%.4f", dtof(mapwin[editting_mapwin]->ra_strt/15.0));
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(ra_strt_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(ra_strt_wid, strlen(ra_strt_str));

  sprintf(de_strt_str, "%.4f", dtof(mapwin[editting_mapwin]->dec_strt));
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(de_strt_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(de_strt_wid, strlen(de_strt_str));


  XtSetArg(Label_arg[0], XtNlabel, mapwin[editting_mapwin]->draw_ragrid ?
			    "Show R.A. grid":
			    "No R.A. grid");
  XtSetValues(draw_ra_wid, Label_arg, (Cardinal)1);

  XtSetArg(Label_arg[0], XtNlabel, mapwin[editting_mapwin]->draw_decgrid ?
			    "Show Dec. grid":
			    "No Dec. grid");
  XtSetValues(draw_dec_wid, Label_arg, (Cardinal)1);


  sprintf(numfiles_str,"%d", mapwin[editting_mapwin]->numfiles);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(numfiles_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(numfiles_wid, strlen(numfiles_str));
}



file_reset(closure)
    caddr_t closure;		/* Widget */
{
  Arg Label_arg[10];
  int i;
  char *cp;
  static XtCallbackRec next_callback[3], prev_callback[3];


  /* convert numfiles_str to numfiles */
  i = atoi(numfiles_str);
  if (i > MAXMAPFILES) {
    i = MAXMAPFILES;
    sprintf(numfiles_str, "%d", i);
    update_string(numfiles_wid, numfiles_str);
  }
  mapwin[editting_mapwin]->numfiles = i;


  sprintf(file_num_str, "File number %d", curr_file+1);
  XtSetArg(Label_arg[0], XtNlabel, file_num_str);
  XtSetValues(file_num_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(file_num_wid, strlen(file_num_str));

  if (curr_file < (mapwin[editting_mapwin]->numfiles-1)) {
    next_callback[0].callback = next_file;
    next_callback[0].closure = closure;
    next_callback[1].callback = NULL;
    next_callback[1].closure = NULL;
    cp = "   Next";
  } else {
    next_callback[0].callback = nothing_file;
    next_callback[0].closure = closure;
    next_callback[1].callback = NULL;
    next_callback[1].closure = NULL;
    cp = "No Next";
  }
  XtSetArg(Label_arg[0], XtNcallback, next_callback);
  XtSetArg(Label_arg[1], XtNlabel, cp);
  XtSetValues(next_file_wid, Label_arg, (Cardinal)2);


  if (curr_file > 0) {
    prev_callback[0].callback = prev_file;
    prev_callback[0].closure = closure;
    prev_callback[1].callback = NULL;
    prev_callback[1].closure = NULL;
    cp = "   Prev";
  } else {
    prev_callback[0].callback = nothing_file;
    prev_callback[0].closure = closure;
    prev_callback[1].callback = NULL;
    prev_callback[1].closure = NULL;
    cp = "No Prev";
  }
  XtSetArg(Label_arg[0], XtNcallback, prev_callback);
  XtSetArg(Label_arg[1], XtNlabel, cp);
  XtSetValues(prev_file_wid, Label_arg, (Cardinal)2);

  /* file name string */
  strcpy(file_name_str, mapwin[editting_mapwin]->file[curr_file].name);
  update_string(file_name_wid, file_name_str);
  
  /* file type toggle */
  cp = filetype_strings[mapwin[editting_mapwin]->file[curr_file].type];
  XtSetArg(Label_arg[0], XtNlabel, cp);
  XtSetValues(f_type_wid, Label_arg, (Cardinal)1);

  /* mags (floats) */ 
  sprintf(f_lbllim_str, "%.4f",
	  mapwin[editting_mapwin]->file[curr_file].lbllim);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(f_lbllim_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(f_lbllim_wid, strlen(f_lbllim_str));

  sprintf(f_gklim_str, "%.4f",
	  mapwin[editting_mapwin]->file[curr_file].gklim);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(f_gklim_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(f_gklim_wid, strlen(f_gklim_str));

  sprintf(f_maglim_str, "%.4f",
	  mapwin[editting_mapwin]->file[curr_file].maglim);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(f_maglim_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(f_maglim_wid, strlen(f_maglim_str));

  /* min and max */
  sprintf(f_vmin_str, "%.4f",
	  mapwin[editting_mapwin]->file[curr_file].maglmin);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(f_vmin_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(f_vmin_wid, strlen(f_vmin_str));

  sprintf(f_vmax_str, "%.4f",
	  mapwin[editting_mapwin]->file[curr_file].maglmax);
  XtSetArg(Label_arg[0], XtNinsertPosition, 0);
  XtSetValues(f_vmax_wid, Label_arg, (Cardinal)1);
  XtTextSetLastPos(f_vmax_wid, strlen(f_vmax_str));

  /* labl toggle */
  XtSetArg(Label_arg[0], XtNlabel,
	   mapwin[editting_mapwin]->file[curr_file].draw_maglbl ?
	   "Label stars with their magnitudes      ":
	   "Don't Label stars with their magnitudes");
  XtSetValues(f_dmaglbl_wid, Label_arg, (Cardinal)1);
}
