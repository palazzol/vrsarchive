/*
 * Dialogs for starXaw
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starXawDlog.c,v 1.1 1990-03-30 16:39:37 vrs Exp $";

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

#include "starXawDlog.h"

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

extern int precess;
extern double to_precess;

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

/* local similar buffers */
char a_constname[MAXPATHLEN];
char a_rcfile[MAXPATHLEN];
char write_rcfile[MAXPATHLEN];

#define READ_MAPWIN 1
#define WRITE_MAPWIN 2
#define NO_MAPWIN 0
int save_load_state;
char *save_load_str[] = {
  "   No mapwin file",
  " Read mapwin file",
  "Write mapwin file"
};

/* Shared with starXawMwin.c */
Bool edit_mapwins = False;

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

/* From starXaw */
extern Bool fixed_size;
extern int fixed_width;
extern int fixed_height;


int pp_status;

void Chart(), Type(), Magnitudes(), Layers(), Map_files(),
  Projection_mode(), Magnitude_labels(), Grid(), Edit_mapwins(), X_parms();

Button_set dlogbuttons[] = {
  {"Chart", Chart},
  {"Type", Type},
  {"Magnitudes", Magnitudes},
  {"Layers", Layers},
  {"Data Files", Map_files},
  {"Projection Mode", Projection_mode},
  {"Magnitude Labels", Magnitude_labels},
  {"Grid", Grid},
  {"Additional Control", Edit_mapwins},
  {"X Parameters", X_parms},
  {"", NULL},
};


/****/
/* The dialog boxes */
/* There must be a better way, but for now, use this variable to indicate
   which dialog is being cancelled */
DlogType current_dialog = NoDialog;

ChartD_S ChartD;
TypeD_S TypeD;
MagnitudesD_S MagnitudesD;
LayersD_S LayersD;
Map_filesD_S Map_filesD;
Projection_modeD_S Projection_modeD;
Magnitude_labelsD_S Magnitude_labelsD;
GridD_S GridD;
Edit_mapwinsD_S Edit_mapwinsD;
X_parmsD_S X_parmsD;




/* It's OK */
void do_dismiss(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  un_help();
  XtPopdown((Widget) closure);
}

/* It's not OK */
void do_cancel(widget,closure,callData)
    Widget widget;
    caddr_t closure;
    caddr_t callData;
{
  int i;

  switch (current_dialog) {
  case ChartDialog:
    fprintf(stderr,"Cancelling the chart dialog\n");
    ra = ChartD.ra;
    de = ChartD.de;
    sc = ChartD.sc;
    title = ChartD.title;
    constfile = ChartD.constfile;
    all_invert = ChartD.all_invert;
    precess = ChartD.precess;
    to_precess = ChartD.to_precess;
    break;
  case TypeDialog:
    fprintf(stderr,"Cancelling the type dialog\n");
    chart_type = TypeD.chart_type;
    break;
  case MagnitudesDialog:
    fprintf(stderr,"Cancelling the magnitudes dialog\n");
    all_lbllim = MagnitudesD.all_lbllim;
    all_maglim = MagnitudesD.all_maglim;
    all_gklim = MagnitudesD.all_gklim;
    break;
  case LayersDialog:
    fprintf(stderr,"Cancelling the layers dialog\n");
    for (i = 0; i < MAXLAYRS; i++)
      all_layer[i] = LayersD.all_layer[i];
    numlayers = LayersD.numlayers;
    break;
  case Map_filesDialog:
    fprintf(stderr,"Cancelling the files dialog\n");
    for (i = 0; i < MAXMAPFILES; i++) {
      strcpy(mapfiles[i], Map_filesD.mapfiles[i]);
      mapftypes[i] = Map_filesD.mapftypes[i];
    }
    nummapfiles = Map_filesD.nummapfiles;
    break;
  case Projection_modeDialog:
    fprintf(stderr,"Cancelling the projection mode dialog\n");
    all_proj_mode = Projection_modeD.all_proj_mode;
    break;
  case Magnitude_labelsDialog:
    fprintf(stderr,"Cancelling the magnitude labels dialog\n");
    all_vmin = Magnitude_labelsD.all_vmin;
    all_vmax = Magnitude_labelsD.all_vmax;
    break;
  case GridDialog:
    fprintf(stderr,"Cancelling the grid dialog\n");
    all_rstep = GridD.all_rstep;
    all_dstep = GridD.all_dstep;
    all_rstrt = GridD.all_rstrt;
    all_dstrt = GridD.all_dstrt;
    break;
  case Edit_mapwinsDialog:
    break;
  case X_parmsDialog:
    fprintf(stderr,"Cancelling the X parameters dialog\n");
    post_preview = X_parmsD.post_preview;
    post_landscape = X_parmsD.post_landscape;
    use_backup = X_parmsD.use_backup;
    is_color = X_parmsD.is_color;
    fixed_size = X_parmsD.fixed_size;
    fixed_width = X_parmsD.fixed_width;
    fixed_height = X_parmsD.fixed_height;
    break;
  default:
    fprintf(stderr,"No dialog to cancel\n");
    break;
  }

  un_help();
  XtPopdown((Widget) closure);
}

/* Utility widget functions */
extern XtTranslations numbers_oneline, string_oneline;

Widget Get_float(prompt, bottom_widge, form, flonum, flostr, flostr_len)
     char *prompt;
     Widget bottom_widge;
     Widget form;
     double flonum;
     char *flostr;
     int flostr_len;
{
  Arg Label_arg[10], Float_arg[10];
  Cardinal nLabel_args, nFloat_args;
  Widget t1, retw;

  /* Labels should be left justified, and not have borders */
  XtSetArg(Label_arg[0], XtNhorizDistance,  4);
  XtSetArg(Label_arg[1], XtNfromVert,  bottom_widge);
  XtSetArg(Label_arg[2], XtNborderWidth, 0);
  XtSetArg(Label_arg[3], XtNjustify, XtJustifyLeft);
  nLabel_args = 4;
  t1 = 
    XtCreateManagedWidget(prompt,
			  labelWidgetClass, form,
			  Label_arg, nLabel_args);

  sprintf(flostr, "%.4f", flonum);
  XtSetArg(Float_arg[0], XtNfromHoriz,  t1);
  XtSetArg(Float_arg[1], XtNfromVert,  bottom_widge);
  XtSetArg(Float_arg[2], XtNeditType, XttextEdit);
  XtSetArg(Float_arg[3], XtNinsertPosition, 0);
  XtSetArg(Float_arg[4], XtNlength, flostr_len);
  XtSetArg(Float_arg[5], XtNstring, flostr);
  XtSetArg(Float_arg[6], XtNtranslations, numbers_oneline);
	   
  nFloat_args = 7;
  retw = 
    XtCreateManagedWidget("",
			  asciiStringWidgetClass, form,
			  Float_arg, nFloat_args);
  return retw;
}



Widget Get_string(prompt, bottom_widge, form, strstr, str_len, fixed_len,
		  min_len)
     char *prompt;
     Widget bottom_widge;
     Widget form;
     char *strstr;
     int str_len;
     int fixed_len, min_len;
{
  Arg Label_arg[10], Str_arg[10];
  Cardinal nLabel_args, nStr_args;
  Widget t1, retw;

  /* Labels should be left justified, and not have borders */
  XtSetArg(Label_arg[0], XtNhorizDistance,  4);
  XtSetArg(Label_arg[1], XtNfromVert,  bottom_widge);
  XtSetArg(Label_arg[2], XtNborderWidth, 0);
  XtSetArg(Label_arg[3], XtNjustify, XtJustifyLeft);
  nLabel_args = 4;
  t1 = 
    XtCreateManagedWidget(prompt,
			  labelWidgetClass, form,
			  Label_arg, nLabel_args);

  XtSetArg(Str_arg[0], XtNfromHoriz,  t1);
  XtSetArg(Str_arg[1], XtNfromVert,  bottom_widge);
  XtSetArg(Str_arg[2], XtNeditType, XttextEdit);
  XtSetArg(Str_arg[3], XtNinsertPosition, strlen(strstr));
  XtSetArg(Str_arg[4], XtNlength, str_len);
  XtSetArg(Str_arg[5], XtNstring, strstr);
  XtSetArg(Str_arg[6], XtNtranslations, string_oneline);
  if (fixed_len != 0)
    XtSetArg(Str_arg[7], XtNwidth, fixed_len);
  else
    XtSetArg(Str_arg[7], XtNwidth, MAX(min_len, strlen(strstr)*6.25));
  nStr_args = 8;
  retw = 
    XtCreateManagedWidget("",
			  asciiStringWidgetClass, form,
			  Str_arg, nStr_args);
  return retw;
}

Widget Get_int(prompt, bottom_widge, form, intnum, intstr, intstr_len)
     char *prompt;
     Widget bottom_widge;
     Widget form;
     int intnum;
     char *intstr;
     int intstr_len;
{
  Arg Label_arg[10], Int_arg[10];
  Cardinal nLabel_args, nInt_args;
  Widget t1, retw;

  /* Labels should be left justified, and not have borders */
  XtSetArg(Label_arg[0], XtNhorizDistance, 4);
  XtSetArg(Label_arg[1], XtNfromVert,  bottom_widge);
  XtSetArg(Label_arg[2], XtNborderWidth, 0);
  XtSetArg(Label_arg[3], XtNjustify, XtJustifyLeft);
  nLabel_args = 4;
  t1 = 
    XtCreateManagedWidget(prompt,
			  labelWidgetClass, form,
			  Label_arg, nLabel_args);

  sprintf(intstr, "%d", intnum);
  XtSetArg(Int_arg[0], XtNfromHoriz,  t1);
  XtSetArg(Int_arg[1], XtNfromVert,  bottom_widge);
  XtSetArg(Int_arg[2], XtNeditType, XttextEdit);
  XtSetArg(Int_arg[3], XtNinsertPosition, 0);
  XtSetArg(Int_arg[4], XtNlength, intstr_len);
  XtSetArg(Int_arg[5], XtNstring, intstr);
  XtSetArg(Int_arg[6], XtNtranslations, numbers_oneline);
	   
  nInt_args = 7;
  retw = 
    XtCreateManagedWidget("",
			  asciiStringWidgetClass, form,
			  Int_arg, nInt_args);
  return retw;
}



int local_conslookup(tag)
char *tag;
{
/*
 * lookup "con.locs" for a matching tag, and then substitute initial values
 * for ra, decl, scale, and label. File layout follows:
 *
 * com  13    25   10   Coma Bereneces
 * cor  15.45 28   10   Corona Borealis
 * 0....+....1....+....2....+....3
 */
  FILE *cfile;
  int taglen;
  char cbuf[LINELEN+1];
  char tbuf[MAXPATHLEN];
  int i;
  
  if ((cfile = fopen(constfile, "r")) == NULL) {
    sprintf(tbuf,"open fail on %s", constfile);
    XBell(display, 50);
    D_comment(tbuf);
    return FALSE;
  }
  taglen = strlen(tag);

  for (;;)
    {
      fgets(cbuf, LINELEN, cfile);
      if (ferror(cfile)) {
	sprintf(tbuf,"read error in '%s'", constfile);
	XBell(display, 50);
	D_comment(tbuf);
	return FALSE;
      };
      if (feof(cfile)) break;
      if (strncmp(tag, cbuf, taglen) == 0)	/* FOUND */
	{	
	  if (4!=sscanf(cbuf,"%*5s%lf%lf%lf %[^\n]",
			&ra, &de, &sc, a_title)) {
	    sprintf(tbuf,"bogus line in constellation file: %s", cbuf);
	    XBell(display, 50);
	    D_comment(tbuf);
	    return FALSE;
	  };
	  ra    = ftod(ra)*15.0;
	  de    = ftod(de);
	  
	  /* remove trailing \n */
	  i = strlen(a_title);
	  if (a_title[i-1] == '\n') a_title[i-1] = '\0';
	  title = a_title;
	  return TRUE;
	}
    }
  sprintf(tbuf,"Constellation '%s' not found", tag);
  XBell(display, 50);
  D_comment(tbuf);
  return FALSE;
}

update_string(wid, str)
Widget wid;
char *str;
{
  Arg args[2];

  XtSetArg(args[0], XtNlabel, str);
  XtSetArg(args[1], XtNinsertPosition, strlen(str));
  XtSetValues(wid, args, (Cardinal)2);
  XtTextSetLastPos(wid, strlen(str));
}

void chart_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  ra = htod(ChartD.ra_str)*15.0;
  de = htod(ChartD.de_str);
  sc = atof(ChartD.sc_str);
  title = a_title;
  constfile = a_constfile;
  if (a_constname[0])
    if (local_conslookup(a_constname)) {
      sprintf(ChartD.ra_str, "%.4f", dtof(ra/15.0));
      sprintf(ChartD.de_str, "%.4f", dtof(de));
      sprintf(ChartD.sc_str, "%.4f", sc);
      update_string(ChartD.ra_wid, ChartD.ra_str);
      update_string(ChartD.de_wid, ChartD.de_str);
      update_string(ChartD.sc_wid, ChartD.sc_str);
      update_string(ChartD.title_wid, title);
    }
  to_precess = atof(ChartD.to_p_str);
  precess = ((to_precess < 1999.99) || (to_precess > 2000.01));
}

void mag_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  all_lbllim = atof(MagnitudesD.all_lbllim_str);
  all_maglim = atof(MagnitudesD.all_maglim_str);
  all_gklim = atof(MagnitudesD.all_gklim_str);
  use_lbllim = use_maglim = use_gklim = TRUE;
}



void layers_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  int i, nlayers;
  Arg args[1];
  char *cp;

  for (i = 0, nlayers = 0; i < MAXLAYRS; i++)
    if (all_layer[i] != 0) all_layer[nlayers++] = all_layer[i];
  for (i = nlayers;i < MAXLAYRS; i++) all_layer[i] = 0;
  numlayers = nlayers;

  for (i = 0; i < MAXLAYRS; i++) {
    cp = lay_strings[all_layer[i]];
    XtSetArg(args[0], XtNlabel, cp);

    XtSetValues(LayersD.wid[i], args, (Cardinal)1);
  }
}


void mapf_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  int i, nfiles;
  Arg args[2];
  char *cp;

  for (i = 0, nfiles = 0; i < MAXMAPFILES; i++)
    if (mapfiles[i][0]) {
      strcpy(Map_filesD.mapfiles[nfiles], Map_filesD.mapfiles[i]);
      mapftypes[nfiles] = mapftypes[i];
      nfiles++;
    };

  for (i = nfiles;i < MAXMAPFILES; i++) {
    strcpy(mapfiles[i], "");
    mapftypes[i] = LINEREAD;
  }
  nummapfiles = nfiles;

  for (i = 0, nfiles = 0; i < MAXMAPFILES; i++)
    if (mapfiles[i][0]) {
      strcpy(Map_filesD.mapfiles[nfiles], Map_filesD.mapfiles[i]);
      mapftypes[nfiles] = mapftypes[i];
      nfiles++;
    };

  for (i = nfiles;i < MAXMAPFILES; i++) {
    strcpy(mapfiles[i], "");
    mapftypes[i] = LINEREAD;
  }
  nummapfiles = nfiles;

  for (i = 0; i < MAXMAPFILES; i++) {
    mapfiles[i] = Map_filesD.mapfiles[i];
    update_string(Map_filesD.file_wid[i], mapfiles[i]);


    cp = filetype_strings[mapftypes[i]];
    XtSetArg(args[0], XtNlabel, cp);

    XtSetValues(Map_filesD.ftype_wid[i], args, (Cardinal)1);
  }
}



void maglbl_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  all_vmin = atof(Magnitude_labelsD.all_vmin_str);
  all_vmax = atof(Magnitude_labelsD.all_vmax_str);
  use_vmin = TRUE;
}

void grid_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  all_rstep = htod(GridD.all_rstep_str)*15.0;
  all_dstep = htod(GridD.all_dstep_str);
  all_rstrt = htod(GridD.all_rstrt_str)*15.0;
  all_dstrt = htod(GridD.all_dstrt_str);
  use_rstep = TRUE;
}

void xparm_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  fixed_width = atoi(X_parmsD.fixed_width_str);
  fixed_height = atoi(X_parmsD.fixed_height_str);
}


void save_load_apply(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  FILE *tfile;

  if (write_rcfile[0])
    if ((tfile = fopen(write_rcfile, "w")) != NULL)
      if (rc_write(tfile)) {
	fclose(tfile);
	D_comment("Wrote .starrc file");
      }

  switch (save_load_state) {
  case NO_MAPWIN:
    break;
  case READ_MAPWIN:
    read_mapwin_file = TRUE;
    write_mapwin_file = FALSE;
    break;
  case WRITE_MAPWIN:
    write_mapwin_file = TRUE;
    read_mapwin_file = FALSE;
    break;
  default:
    break;
  }
}





/* Specific dialogs */

/********** Dialog ***************/
/* Chart widget
   Get center ra and declination and scale,
   title
   constellation name to look up
   constellation lookup file
   invert or not
*/

void Chart(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  void invert_toggle();
  char *dlog_name = "chart";
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, apply_widge, dismiss_widge, cancel_widge;

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
    XtCreateManagedWidget("Specify Center and scale of chart",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);



  /* get RA */
  ChartD.ra_wid =
    twidge2 = Get_float("Right Asc.:   ", twidge1, pform, dtof(ra/15.0),
		      ChartD.ra_str,
		      sizeof(ChartD.ra_str));
  twidge1 = twidge2;

  /* get DEC */
  ChartD.de_wid =
    twidge2 = Get_float("Declination:  ", twidge1, pform, dtof(de),
		      ChartD.de_str,
		      sizeof(ChartD.de_str));
  twidge1 = twidge2;

  /* Get Scale */
  ChartD.sc_wid =
    twidge2 = Get_float("Scale:        ", twidge1, pform, sc,
		      ChartD.sc_str,
		      sizeof(ChartD.sc_str));
  twidge1 = twidge2;


  /* Get Title */
  strcpy(a_title, title);
  ChartD.title_wid =
    twidge2 = Get_string("Title:        ", twidge1, pform,
		      a_title,
		      MAXPATHLEN, 0, 200);
  twidge1 = twidge2;


  /* Get Constellation to look up */
  strcpy(a_constname, "");
  twidge2 = Get_string("Constellation:", twidge1, pform,
		      a_constname,
		      4, 0, 50);
  twidge1 = twidge2;

  /* Get Constellation data file */
  strcpy(a_constfile, constfile);
  twidge2 = Get_string("Constellation data file:", twidge1, pform,
		      a_constfile,
		      MAXPATHLEN, 0, 100);
  twidge1 = twidge2;


  /* Toggle invert */
  callback[0].callback = invert_toggle;
  callback[0].closure = (caddr_t) "Invert";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget( all_invert ? " Inverted ": "  Normal  ",
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;

  /* precess */
  ChartD.to_p_wid =
    twidge2 = Get_float("Precess to:   ", twidge1, pform, to_precess,
		      ChartD.to_p_str,
		      sizeof(ChartD.to_p_str));
  twidge1 = twidge2;

  bottom_widge = twidge1;	/* The bottommost widget so far */


  


  callback[0].callback = chart_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  apply_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  ChartD.ra = ra;
  ChartD.de = de;
  ChartD.sc = sc;
  ChartD.title = title;
  ChartD.constfile = constfile;
  ChartD.all_invert = all_invert;
  ChartD.precess = precess;
  ChartD.to_precess = to_precess;

  current_dialog = ChartDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  chart_help();
}

/********** Dialog ***************/
/* Type widget
   Set chart type to be either Main + thumbnail
                        or full page
*/
void Type(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "type";
  void type_select();
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, dismiss_widge, cancel_widge;

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
    XtCreateManagedWidget("Specify Type of chart (press button)",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);

  /* set type */
  callback[0].callback = type_select;
  callback[0].closure = (caddr_t) "chart_type";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(
			  ((chart_type == THREEPART) ?
			   "Main Map plus Thumbnail" :
			   "Full Page              "),
			  commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;

  bottom_widge = twidge1;	/* The bottommost widget so far */


  


  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  TypeD.chart_type = chart_type;

  current_dialog = TypeDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  type_help();
}

/********** Dialog ***************/
/* Magnitudes dialog
   Set lbllim, maglim, gklim
*/

void Magnitudes(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "magnitudes";
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, apply_widge, dismiss_widge, cancel_widge;

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
    XtCreateManagedWidget("Specify Magnitude limits for",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);

  /* Get name limit (lbllim) */
  MagnitudesD.all_lbllim_wid =
    twidge2 = Get_float("Object names:  ", twidge1, pform, all_lbllim,
		      MagnitudesD.all_lbllim_str,
		      sizeof(MagnitudesD.all_lbllim_str));
  twidge1 = twidge2;

  /* Get label limit (gklim) */
  MagnitudesD.all_gklim_wid =
    twidge2 = Get_float("Bayer labels:  ", twidge1, pform, all_gklim,
		      MagnitudesD.all_gklim_str,
		      sizeof(MagnitudesD.all_gklim_str));
  twidge1 = twidge2;

  /* Get visibility limit (maglim) */
  MagnitudesD.all_maglim_wid =
    twidge2 = Get_float("Object symbol: ", twidge1, pform, all_maglim,
		      MagnitudesD.all_maglim_str,
		      sizeof(MagnitudesD.all_maglim_str));
  twidge1 = twidge2;


  bottom_widge = twidge1;	/* The bottommost widget so far */


  


  callback[0].callback = mag_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);
  bottom_widge = apply_widge;

  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  MagnitudesD.all_lbllim = all_lbllim;
  MagnitudesD.all_maglim = all_maglim;
  MagnitudesD.all_gklim = all_gklim;

  current_dialog = MagnitudesDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  magnitudes_help();
}

/********** Dialog ***************/
/* Layers dialog
   Set all_layer
*/

void Layers(widget,closure,callData)
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
  Widget bottom_widge, apply_widge, dismiss_widge, cancel_widge;
  int i;
  char *cp;


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
    LayersD.laynum[i] = i;
    
    callback[0].callback = layer_select;
    callback[0].closure = (caddr_t) ((int *) &(LayersD.laynum[i]));
    XtSetArg(Button_arg[0], XtNcallback, callback);
    XtSetArg(Button_arg[1], XtNhorizDistance,  4);
    XtSetArg(Button_arg[2], XtNfromVert,  twidge1);

    NButton_args = 3;
    cp = lay_strings[all_layer[i]];
    LayersD.wid[i] = 
      twidge2 =
	XtCreateManagedWidget(cp,
			      commandWidgetClass, pform,
			      Button_arg, NButton_args);

    twidge1 = twidge2;
  }

  bottom_widge = twidge1;	/* The bottommost widget so far */


  
  callback[0].callback = layers_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);
  bottom_widge = apply_widge;

  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[0], XtNcallback, callback);
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  for (i = 0; i < MAXLAYRS; i++)
    LayersD.all_layer[i] = all_layer[i];
  LayersD.numlayers = numlayers;

  current_dialog = LayersDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  layers_help();
}

/* mapfiles widget
   Set mapfiles array
*/
void Map_files(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "mapfiles";
  void ftype_select();
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2, higher_widge;
  Widget bottom_widge, apply_widge, dismiss_widge, cancel_widge;
  int i;
  char *cp;

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
    XtCreateManagedWidget("Specify data files and types to be used",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);

  higher_widge = twidge1;
  for (i = 0; i < MAXMAPFILES; i++) {
    Map_filesD.mapnum[i] = i;
    strcpy(Map_filesD.mapfiles[i], mapfiles[i]);
    Map_filesD.mapftypes[i] = mapftypes[i];

    Map_filesD.file_wid[i] =
      twidge2 = Get_string("File:", twidge1, pform,
			   Map_filesD.mapfiles[i],
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

    callback[0].callback = ftype_select;
    callback[0].closure = (caddr_t) ((int *) &(Map_filesD.mapnum[i]));
    XtSetArg(Button_arg[0], XtNcallback, callback);
    XtSetArg(Button_arg[1], XtNfromHoriz,  twidge1);
    XtSetArg(Button_arg[2], XtNfromVert,  higher_widge);

    NButton_args = 3;
    cp = filetype_strings[mapftypes[i]];
    Map_filesD.ftype_wid[i] =
      twidge2 =
	XtCreateManagedWidget(cp,
			      commandWidgetClass, pform,
			      Button_arg, NButton_args);
    twidge1 = twidge2;
    higher_widge = twidge1;
  }

  bottom_widge = twidge1;	/* The bottommost widget so far */

  callback[0].callback = mapf_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);
  bottom_widge = apply_widge;

  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  /* names and types set above */
  Map_filesD.nummapfiles = nummapfiles;

  current_dialog = Map_filesDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  dfiles_help();
}

/* projection_mode widget
   Set projection mode of chart to be
   SANSONS, STEREOGR, GNOMONIC, ORTHOGR, or RECTANGULAR 
*/
void Projection_mode(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "projection_mode";
  void mode_select();
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, dismiss_widge, cancel_widge;
  char *tcp;


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
    XtCreateManagedWidget("Specify Projection mode (press button)",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);

  /* set mode */
  callback[0].callback = mode_select;
  callback[0].closure = (caddr_t) "projection_mode";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;

  tcp = proj_mode_strings[all_proj_mode];

  twidge2 =
    XtCreateManagedWidget(tcp,
			  commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;

  bottom_widge = twidge1;	/* The bottommost widget so far */


  


  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  Projection_modeD.all_proj_mode = all_proj_mode;

  current_dialog = Projection_modeDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  pmode_help();
}

/********** Dialog ***************/
/* Magnitude label dialog
   set all_vmin, all_vmax (use_vmin)
   nomaglbls
*/

void Magnitude_labels(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  void nomaglbls_toggle();
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "maglbls";
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, apply_widge, dismiss_widge, cancel_widge;

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
    XtCreateManagedWidget("Specify star magnitude labeling",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);

  /* Get minimum */
  Magnitude_labelsD.all_vmin_wid =
    twidge2 = Get_float("Brightest:  ", twidge1, pform, all_vmin,
		      Magnitude_labelsD.all_vmin_str,
		      sizeof(Magnitude_labelsD.all_vmin_str));
  twidge1 = twidge2;

  /* Get maximum */
  Magnitude_labelsD.all_vmax_wid =
    twidge2 = Get_float("Faintest:   ", twidge1, pform, all_vmax,
		      Magnitude_labelsD.all_vmax_str,
		      sizeof(Magnitude_labelsD.all_vmax_str));
  twidge1 = twidge2;


  /* Toggle nomaglabls */
  callback[0].callback = nomaglbls_toggle;
  callback[0].closure = (caddr_t) "nomablbls";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(nomaglbls ?
			  "Don't Label stars with their magnitudes":
			  "Label stars with their magnitudes      ",
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;


  bottom_widge = twidge1;	/* The bottommost widget so far */


  


  callback[0].callback = maglbl_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);
  bottom_widge = apply_widge;

  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  Magnitude_labelsD.all_vmin = all_vmin;
  Magnitude_labelsD.all_vmax = all_vmax;

  current_dialog = Magnitude_labelsDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  maglabels_help();
}

/********** Dialog ***************/
/* Grid dialog
   set all_rstep, all_dstep, all_rstrt, all_dstrt,
   no_ra_grid, no_dec_grid
*/

void Grid(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  void no_ra_grid_toggle();
  void no_dec_grid_toggle();
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "grid";
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, apply_widge, dismiss_widge, cancel_widge;

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
    XtCreateManagedWidget("Specify Grid",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);


  if (all_rstep == 0) all_rstep = 15.0;
  if (all_dstep == 0) all_dstep = 5.0;
  /* Get ra_step */
  GridD.all_rstep_wid =
    twidge2 = Get_float("R.A. Step:   ", twidge1, pform, dtof(all_rstep/15.0),
		      GridD.all_rstep_str,
		      sizeof(GridD.all_rstep_str));
  twidge1 = twidge2;

  /* Get dec_step */
  GridD.all_dstep_wid =
    twidge2 = Get_float("Dec. Step:   ", twidge1, pform, all_dstep,
		      GridD.all_dstep_str,
		      sizeof(GridD.all_dstep_str));
  twidge1 = twidge2;


  /* Get ra_strt */
  GridD.all_rstrt_wid =
    twidge2 = Get_float("R.A. Start:  ", twidge1, pform, dtof(all_rstrt/15.0),
		      GridD.all_rstrt_str,
		      sizeof(GridD.all_rstrt_str));
  twidge1 = twidge2;

  /* Get dec_strt */
  GridD.all_dstrt_wid =
    twidge2 = Get_float("Dec. Start:  ", twidge1, pform, all_dstrt,
		      GridD.all_dstrt_str,
		      sizeof(GridD.all_dstrt_str));
  twidge1 = twidge2;



  /* Toggle ra */
  callback[0].callback = no_ra_grid_toggle;
  callback[0].closure = (caddr_t) "no_ra_grid";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(no_ra_grid ?
			  "  No R.A. grid":
			  "Show R.A. grid",
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;

  /* Toggle dec */
  callback[0].callback = no_dec_grid_toggle;
  callback[0].closure = (caddr_t) "no_dec_grid";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(no_dec_grid ?
			  "  No Dec. grid":
			  "Show Dec. grid",
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;


  bottom_widge = twidge1;	/* The bottommost widget so far */


  


  callback[0].callback = grid_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);
  bottom_widge = apply_widge;

  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  GridD.all_rstep = all_rstep;
  GridD.all_dstep = all_dstep;
  GridD.all_rstrt = all_rstrt;
  GridD.all_dstrt = all_dstrt;
  GridD.no_ra_grid = no_ra_grid;
  GridD.no_dec_grid = no_dec_grid;

  current_dialog = GridDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  grid_help();
}

void Edit_mapwins(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  void e_mapwins_toggle();
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "x parms";
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, dismiss_widge;

  XtSetArg( Button_arg[0], XtNcallback, callback );
  NButton_args = 1;

  /* Labels should be left justified, and not have borders */
  XtSetArg(Label_arg[0], XtNborderWidth, 0);
  XtSetArg(Label_arg[1], XtNjustify, XtJustifyLeft);
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
    XtCreateManagedWidget("Edit map window structure",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);

  XtSetArg(Label_arg[2], XtNhorizDistance,  4);
  XtSetArg(Label_arg[3], XtNfromVert,  twidge1);
  NLabel_args = 4;
  twidge2 = 
    XtCreateManagedWidget("(Editing will be performed",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);
  twidge1 = twidge2;

  XtSetArg(Label_arg[3], XtNfromVert,  twidge1);
  twidge2 = 
    XtCreateManagedWidget("when \"Draw Chart\" is selected)",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);
  twidge1 = twidge2;

  /* Toggle edit_mapwins */
  callback[0].callback = e_mapwins_toggle;
  callback[0].closure = (caddr_t) "edit mapwins";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(edit_mapwins ?
			  "   Edit   ":
			  "Don't Edit",
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;
  bottom_widge = twidge1;	/* The bottommost widget so far */


  


  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  /* Save the current values */
  current_dialog = Edit_mapwinsDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  emapwins_help();
}

/********** Dialog ***************/
/* 
  Set X specific parameters
  post_preview, post_landscape, use_backup, is_color
*/

void X_parms(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  void pp_select();
  void use_back_toggle();
  void is_color_toggle();
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  char *dlog_name = "x parms";
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, apply_widge, dismiss_widge, cancel_widge;
  char *cp;

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
    XtCreateManagedWidget("X11 specific controls",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);

  /* Toggle post_preview */
  if (fixed_size) {
    cp = " Use Fixed window dimensions  ";
    pp_status = 3;
  } else if (post_preview) {
    if (post_landscape) {
      cp = "Use PostScript landscape page ";
      pp_status = 1;
    } else {
      cp = "Use PostScript page dimensions";
      pp_status = 2;
    };
  } else {
    cp = "Use flexible window dimensions";
    pp_status = 0;
  };
  callback[0].callback = pp_select;
  callback[0].closure = (caddr_t) "post preview";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(cp,
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;


  X_parmsD.fixed_width_wid =
    twidge2 =
      Get_int("Fixed width: ",
	      twidge1, pform, fixed_width, X_parmsD.fixed_width_str,
	      sizeof(X_parmsD.fixed_width_str));
  twidge1 = twidge2;

  X_parmsD.fixed_height_wid =
    twidge2 =
      Get_int("Fixed height:",
	      twidge1, pform, fixed_height, X_parmsD.fixed_height_str,
	      sizeof(X_parmsD.fixed_height_str));
  twidge1 = twidge2;

  /* Toggle use_backup */
  callback[0].callback = use_back_toggle;
  callback[0].closure = (caddr_t) "use back";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(use_backup ?
			  "Use X Backing store":
			  "No X Backing store ",
			  commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;


  /* Toggle is_color */
  callback[0].callback = is_color_toggle;
  callback[0].closure = (caddr_t) "is color";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(is_color ?
			  "Color Display      ":
			  "Monochrome Display ",
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;


  bottom_widge = twidge1;	/* The bottommost widget so far */


  
  callback[0].callback = xparm_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);
  bottom_widge = apply_widge;


  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_cancel;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNfromHoriz,  dismiss_widge);
  cancel_widge = 
    XtCreateManagedWidget( "Cancel ",
			commandWidgetClass, pform, Button_arg, NButton_args);


  /* Save the current values */
  X_parmsD.post_preview = post_preview;
  X_parmsD.post_landscape = post_landscape;
  X_parmsD.use_backup = use_backup;
  X_parmsD.is_color = is_color;
  X_parmsD.fixed_size = fixed_size;
  X_parmsD.fixed_width = fixed_width;
  X_parmsD.fixed_height = fixed_height;

  current_dialog = X_parmsDialog;
  XtPopup(pwidg, XtGrabNonexclusive);
  xparms_help();
}



/********** Dialog ***************/
/* Save / Load file
   set read_mapwin_file, write_mapwin_file, mapwin_file
*/

void Save_Load_file(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
  Widget pshell, pwidg, pform;
  static Arg shell_args[] = {
    { XtNwidth, (XtArgVal) 100},
  };
  void saveload_select();
  char *dlog_name = "saveload";
  Arg Button_arg[10], Label_arg[10];
  Cardinal NButton_args, NLabel_args;
  static XtCallbackRec callback[2];
  Widget twidge1, twidge2;
  Widget bottom_widge, apply_widge, dismiss_widge;

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
    XtCreateManagedWidget(
		      "Save or Load mapwin file, or write .starrc format file",
			  labelWidgetClass, pform, 
			  Label_arg, NLabel_args);



  /* Get Mapwin file name */
  twidge2 = Get_string("Mapwin file:       ", twidge1, pform,
		      mapwin_file,
		      MAXPATHLEN, 0, 100);
  twidge1 = twidge2;


  if (read_mapwin_file)
    save_load_state = READ_MAPWIN;
  else if (write_mapwin_file)
    save_load_state = WRITE_MAPWIN;
  else
    save_load_state = NO_MAPWIN;

  callback[0].callback = saveload_select;
  callback[0].closure = (caddr_t) "saveload";
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  twidge1);
  NButton_args = 3;
  twidge2 =
    XtCreateManagedWidget(save_load_str[save_load_state],
			commandWidgetClass, pform, Button_arg, NButton_args);

  twidge1 = twidge2;

  /* Get .starrc file name */
  twidge2 = Get_string("Write .starrc file:", twidge1, pform,
		      write_rcfile,
		      MAXPATHLEN, 0, 100);
  twidge1 = twidge2;


  bottom_widge = twidge1;	/* The bottommost widget so far */


  
  callback[0].callback = save_load_apply;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  bottom_widge);
  NButton_args = 3;
  apply_widge = 
    XtCreateManagedWidget( " Apply ",
			commandWidgetClass, pform, Button_arg, NButton_args);

  callback[0].callback = do_dismiss;
  callback[0].closure = (caddr_t)pwidg;
  XtSetArg(Button_arg[1], XtNhorizDistance,  4);
  XtSetArg(Button_arg[2], XtNfromVert,  apply_widge);
  dismiss_widge = 
    XtCreateManagedWidget( "Dismiss",
			commandWidgetClass, pform, Button_arg, NButton_args);

  XtPopup(pwidg, XtGrabNonexclusive);
  svld_help();
}



/* Functions called by dialogs */

void invert_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  all_invert = !all_invert;

  XtSetArg( args[0], XtNlabel, all_invert ? " Inverted ": "  Normal  ");

  XtSetValues(widget, args, (Cardinal)1);
}


void nomaglbls_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  nomaglbls = !nomaglbls;

  XtSetArg( args[0], XtNlabel,nomaglbls ?
			  "Don't Label stars with their magnitudes":
			  "Label stars with their magnitudes      ");

  XtSetValues(widget, args, (Cardinal)1);
}


void no_ra_grid_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  no_ra_grid = !no_ra_grid;

  XtSetArg( args[0], XtNlabel, no_ra_grid ? "  No R.A. grid":"Show R.A. grid");

  XtSetValues(widget, args, (Cardinal)1);
}

void no_dec_grid_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  no_dec_grid = !no_dec_grid;

  XtSetArg( args[0], XtNlabel, no_dec_grid ?
	   "  No Dec. grid":"Show Dec. grid");

  XtSetValues(widget, args, (Cardinal)1);
}


void type_select(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  if (chart_type == THREEPART) {
    XtSetArg( args[0], XtNlabel, "Full Page              ");
    chart_type = FULLPAGE;
  } else {
    XtSetArg( args[0], XtNlabel, "Main Map plus Thumbnail");
    chart_type = THREEPART;
  }
  XtSetValues(widget, args, (Cardinal)1);
}


void mode_select(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];
  char *cp;

  all_proj_mode++;
  all_proj_mode %= (MAXPMODE+1);
  if (all_proj_mode == 0) all_proj_mode = 1;

  cp = proj_mode_strings[all_proj_mode];
  XtSetArg( args[0], XtNlabel, cp);

  XtSetValues(widget, args, (Cardinal)1);
}


void layer_select(widget, laynump, callData)
    Widget widget;
    caddr_t laynump;
    caddr_t callData;
{
  Arg args[1];
  char *cp;
  int i;

  i = *((int* ) laynump);

  all_layer[i]++;
  all_layer[i] %= (MAXLAYNUM+1);

  cp = lay_strings[all_layer[i]];
  XtSetArg( args[0], XtNlabel, cp);

  XtSetValues(widget, args, (Cardinal)1);
}



void ftype_select(widget, mapnump, callData)
    Widget widget;
    caddr_t mapnump;
    caddr_t callData;
{
  Arg args[1];
  char *cp;
  int i;

  i = *((int* ) mapnump);

  mapftypes[i]++;
  mapftypes[i] %= (MAXFTYPES+1);
  if (mapftypes[i] == 0) mapftypes[i] = 1;

  cp = filetype_strings[mapftypes[i]];
  XtSetArg( args[0], XtNlabel, cp);

  XtSetValues(widget, args, (Cardinal)1);
}


void saveload_select(widget, namestr, callData)
     Widget widget;
     caddr_t namestr;
     caddr_t callData;
{
  Arg args[1];
  char *cp;

  save_load_state++;
  save_load_state %= 3;


  cp = save_load_str[save_load_state];
  XtSetArg(args[0], XtNlabel, cp);

  XtSetValues(widget, args, (Cardinal)1);
}



void pp_select(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];
  char *cp;

  pp_status++;
  pp_status %= 4;

  /* pp_status:
     0: flexible -> post_preview = post_landscape = fixed_size = FALSE;
     1: postscript size ->
                post_preview = TRUE; post_landscape = fixed_size = FALSE;
     2: postscript landscape size ->
                post_preview = TRUE; post_landscape = TRUE; fixed_size = FALSE;
     3: fixed size -> post_preview = post_landscape = FALSE; fixed_size = TRUE;
     */
  switch (pp_status) {
  case 0:
    post_preview = post_landscape = fixed_size = FALSE;
    cp = "Use flexible window dimensions";
    break;
  case 1:
    post_preview = TRUE; post_landscape = fixed_size = FALSE;
    cp = "Use PostScript page dimensions";
    break;
  case 2:
    post_preview = post_landscape = TRUE; fixed_size = FALSE;
    cp = "Use PostScript landscape page ";
    break;
  case 3:
    post_preview = FALSE; fixed_size = TRUE;
    cp = " Use Fixed window dimensions  ";
    break;
  default:
    break;
  }

  XtSetArg( args[0], XtNlabel, cp);
  XtSetValues(widget, args, (Cardinal)1);
}


void use_back_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  use_backup = !use_backup;

  XtSetArg( args[0], XtNlabel, use_backup ?
			  "Use X Backing store":
			  " No X Backing store");

  XtSetValues(widget, args, (Cardinal)1);
}

void is_color_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  is_color = !is_color;

  XtSetArg( args[0], XtNlabel, is_color ?
	   "     Color Display":
	   "Monochrome Display");

  XtSetValues(widget, args, (Cardinal)1);
}



void e_mapwins_toggle(widget,namestr,callData)
    Widget widget;
    caddr_t namestr;
    caddr_t callData;
{
  Arg args[1];

  edit_mapwins = !edit_mapwins;

  XtSetArg( args[0], XtNlabel, edit_mapwins ?
	   "   Edit   ":
	   "Don't Edit");

  XtSetValues(widget, args, (Cardinal)1);
}

