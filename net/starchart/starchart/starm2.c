/*
 * starm2.c, more routines formerly in starchart.c/starmain.c
 *		revision 3.1 August, 1989 
 *
 * Portions Copyright (c) 1987 by Alan Paeth (awpaeth@watcgl)
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


static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starm2.c,v 1.1 1990-03-30 16:39:13 vrs Exp $";

#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>

#include "star3.h"

#ifndef READMODE
#define READMODE "r"
#endif
#define OPENFAIL 0
#define LINELEN 82

#ifndef MAXPATHLEN
#define MAXPATHLEN 1025
#endif

/* PI / 180 = .0174532925199 */
#define DCOS(x) (cos((x)*.0174532925199))
#define DSIN(x) (sin((x)*.0174532925199))
#define DTAN(x) (tan((x)*.0174532925199))
#define DASIN(x) (asin(x)/.0174532925199)
#define DATAN2(x,y) (atan2(x,y)/.0174532925199)
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

extern int cur_function;
extern int cur_map_type;
extern int cur_map_tag;
extern char *cur_tag_field;
extern char *cur_file_name;
extern user_interact;
extern precess;

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

/* xform globals */
extern double xf_west, xf_east, xf_north, xf_south, xf_bottom;
extern int xf_xcen, xf_ycen, xf_ybot;
extern int xf_w_left, xf_w_right, xf_w_top, xf_w_bot;

extern double xf_c_scale;


extern int htick_lim, htext_lim;
extern int htext_xoff, htext_yoff;
extern int vtick_lim, vtext_lim;
extern int vtext_xoff, vtext_yoff;


extern char *boundfile;
extern char boundftype;
extern char *patternfile;
extern char pattftype;
extern char *cnamefile;
extern char cnameftype;
extern char *mapfiles[];
extern int mapftypes[];
extern int nummapfiles;

extern char *progname;

/* read file function indirection to allow for use of alternate file readers */
int readstar();
int buf_readstar();
extern int (*readfile)();

/* Readstar globals */
extern double obj_lat, obj_lon, obj_mag;
extern char obj_type[], obj_color[], obj_label[];
extern char obj_constell[], obj_name[];
extern char *obj_commnt, fileline[];


/* Chart element routines */

/* Uses xf_north etc. from initxform */
chartoutline(win)
     mapwindow *win;
{
  double start, inc;
  int xloc, xloc2, yloc, yloc2, div, i;
  int inregion;
  int R;
  double phi;

  cur_function = CHRTOUTLN;

  if (win->proj_mode == STEREOGR) {
    R = MIN(win->height, win->width) / 2 + 1;
    D_move(xf_xcen, xf_ycen+R);
    for (phi = 5.0; phi <= 360.0; phi += 5.0)
      D_draw(xf_xcen + (int)(R*DSIN(phi) + 0.5), 
	     xf_ycen + (int)(R*DCOS(phi) + 0.5), SOLID);
  } else if (win->proj_mode == SANSONS) {
    xform(xf_south, xf_west, &xloc,  &yloc, &inregion);
    xform(xf_south, xf_east, &xloc2, &yloc2, &inregion);
    D_movedraw(xloc, yloc, xloc2, yloc2, SOLID);
    xform(xf_north, xf_west, &xloc,  &yloc, &inregion);
    xform(xf_north, xf_east, &xloc2, &yloc2, &inregion);
    D_movedraw(xloc, yloc, xloc2, yloc2, SOLID);
    
    inc = (xf_north - xf_south);
    div = (int)(inc);
    if (div < 1) div = 1;
    inc /= div;
    start = xf_south;
    xform(start, xf_west, &xloc, &yloc, &inregion);
    D_move(xloc, yloc);
    for (i=0; i < div; i++) {
      start += inc;
      xform(start, xf_west, &xloc, &yloc, &inregion);
      D_draw(xloc, yloc, SOLID);
    }
    start = xf_south;
    xform(start, xf_east, &xloc, &yloc, &inregion);
    D_move(xloc, yloc);
    for (i=0; i < div; i++) {
      start += inc;
      xform(start, xf_east, &xloc, &yloc, &inregion);
      D_draw(xloc, yloc, SOLID);
    }
    charthgrid(win, 15.0, 18);
    charthgrid(win, 5.0, 12);
    charthgrid(win, 1.0, 6);
    chartvgrid(win, 10.0, 18);
    chartvgrid(win, 5.0 , 12);
    chartvgrid(win, 1.0, 6);
  } else if (win->proj_mode == RECTANGULAR) {
    xform(xf_south, xf_west, &xloc,  &yloc, &inregion);
    xform(xf_south, xf_east, &xloc2, &yloc2, &inregion);
    D_movedraw(xloc, yloc, xloc2, yloc2, SOLID);
    xform(xf_north, xf_west, &xloc,  &yloc, &inregion);
    xform(xf_north, xf_east, &xloc2, &yloc2, &inregion);
    D_movedraw(xloc, yloc, xloc2, yloc2, SOLID);

    xform(xf_north, xf_west, &xloc,  &yloc, &inregion);
    xform(xf_south, xf_west, &xloc2, &yloc2, &inregion);
    D_movedraw(xloc, yloc, xloc2, yloc2, SOLID);
    xform(xf_north, xf_east, &xloc,  &yloc, &inregion);
    xform(xf_south, xf_east, &xloc2, &yloc2, &inregion);
    D_movedraw(xloc, yloc, xloc2, yloc2, SOLID);
    
    charthgrid(win, 15.0, 18);
    charthgrid(win, 5.0, 12);
    charthgrid(win, 1.0, 6);
    chartvgrid(win, 10.0, 18);
    chartvgrid(win, 5.0 , 12);
    chartvgrid(win, 1.0, 6);
  } else { /* GNOMONIC or ORTHOGR */
    D_movedraw(xf_w_left, xf_w_bot, xf_w_right, xf_w_bot, SOLID);
    D_movedraw(xf_w_right, xf_w_bot, xf_w_right, xf_w_top, SOLID);
    D_movedraw(xf_w_right, xf_w_top, xf_w_left, xf_w_top, SOLID);
    D_movedraw(xf_w_left, xf_w_top, xf_w_left, xf_w_bot, SOLID);
  }
}

charthgrid(win, inc, hgt)
     mapwindow *win;
     double inc;
     int hgt;
{
  double start, stop, ras;
  int xloc, xloc2, yloc, xloc3, yloc3;
  int inregion;

  cur_function = CHRTHTICK;

/* r.a. ticks always appear at the bottom, regardless of (invert) */
/* xf_bottom is the bottom, wether north or south, set in intixform */
  start = modup(xf_west, inc);
  stop = moddown(xf_east, inc);
  xform(xf_bottom, start, &xloc, &yloc, &inregion);
  xform(xf_bottom, start+inc, &xloc2, &yloc, &inregion);
  if (xloc - xloc2 > htick_lim)
    for (ras = start; ras <= stop; ras += inc)
      {
	xform(xf_bottom, ras, &xloc3, &yloc3, &inregion);
	D_movedraw(xloc3, yloc3-hgt, xloc3, yloc3, SOLID);
	if (xloc - xloc2 > htext_lim)
	  {
	    char tstr[20];
	    rastr(tstr, ras);
	    D_fontsize(10, HELV);
	    D_text(xloc3+htext_xoff, yloc-htext_yoff, tstr, FALSE);
	  }
      }
}

chartvgrid(win, inc, wid)
     mapwindow *win;
     double inc;
     int wid;
{
  double start, stop, dl;
  int xloc, yloc, yloc2, xloc3, yloc3;
  int inregion;
  
  cur_function = CHRTVTICK;

  start = modup(xf_south, inc);
  stop = moddown(xf_north, inc);
  xform(start, xf_west, &xloc, &yloc, &inregion);
  xform(start+inc, xf_west, &xloc, &yloc2, &inregion);
  if (abs(yloc2 - yloc) > vtick_lim)
    {
      for (dl = start; dl <= stop; dl += inc)
	{
	  xform(dl, xf_west, &xloc3, &yloc3, &inregion);
	  D_movedraw(xloc3, yloc3, xloc3+wid, yloc3, SOLID);
	  if (abs(yloc2 - yloc) > vtext_lim)
	    {
	      char tstr[20];
	      declstr(tstr, dl);
	      D_fontsize(10, HELV);
	      D_text(xloc3+vtext_xoff, yloc3+vtext_yoff, tstr, FALSE);
	    }
	}
    }
}

double modup(a, b)
     double a, b;
{
  double new;
  new = ((double)((int)(a/b))*b);
  if (new >= a) return(new);
  return(new += b);
}

double moddown(a, b)
     double a, b;
{
  double new;
  new = ((double)((int)(a/b))*b);
  if (new <= a) return(new);
  return (new -= b);
}

/* return TRUE if a (in degrees) is west of b */
/* west is towards lower values of RA, e.g. 60 is west of 90 */
int westof(a, b)
     double a, b;
{
  double diff;

  diff = b - a;
  if (diff > 180) diff -= 360;
  if (diff < -180) diff += 360;

  return (diff > 0);
}


/* return TRUE if a (in degrees) is east of b */
/* east is towards higher values of RA, e.g. 90 is east of 60 */
int eastof(a, b)
     double a, b;
{
  double diff;

  diff = b - a;
  if (diff > 180) diff -= 360;
  if (diff < -180) diff += 360;

  return (diff < 0);
}

rastr(str, ras)
     char *str;
     double ras;
{
  int hrs, min;
  if (ras <   0.0) ras += 360.0;
  if (ras > 360.0) ras -= 360.0;
  hrs = (int)(ras/15.0);
  min = (int)((ras - hrs * 15.0) * 4.0);
  sprintf(str, "%2dh", hrs);
  if (min) sprintf(str, "%s%02dm", str, min);
}

declstr(str, dl)
     char *str;
     double dl;
{
  int deg, min;
  if (dl == 0.0) sprintf(str, "%s", " ");
  else if (dl > 0.0) sprintf(str, "%s", "+");
  else
    {
      sprintf(str, "%s", "-");
      dl = -dl;
    }
  deg = (int)(dl);
  min = (int)((dl - deg) * 60.0);
  sprintf(str, "%s%02dd", str, deg);
  if (min) sprintf(str, "%s%02dm", str, min);
}

chartragrid(win)
     mapwindow *win;
{
  double glonc, glatc, glon;
  double olon, olat;
  int xloc, yloc, xloc2, yloc2;
  double tlat1, tlon1, tlat2, tlon2;

  if (!win->draw_ragrid) return;
  
  cur_function = GRID_RA;

  for (glonc = 0.0; glonc <= 360.0; glonc += win->ra_step) {
    glon = glonc + win->ra_strt;
    if (glon < 0.0) glon += 360.0;
    if (glon > 360.0) glon -= 360.0;
    olon = glon;
    olat = glatc = -90.0;
    for (glatc = -90.0; glatc <= 90.0; glatc += 10.0) {
      if (clipr_xform(olat, olon, glatc, glon,
		      &xloc, &yloc, &xloc2, &yloc2, TRUE,
		      &tlat1, &tlon1, &tlat2, &tlon2)) {
	drawcurveline(tlat1, tlon1, tlat2, tlon2,
		      xloc, yloc, xloc2, yloc2, DOTTED, TRUE, 0);
      }
      olat = glatc;
    }
  }
}

chartdecgrid(win)
     mapwindow *win;
{
  double glonc, glatc, glat;
  double olon, olat;
  int xloc, yloc, xloc2, yloc2;
  double tlat1, tlon1, tlat2, tlon2;  
  
  if (!win->draw_decgrid) return;
  
  cur_function = GRID_DEC;
  
  for (glatc = 0; glatc <= 90.0; glatc += win->dec_step) {
    glat = glatc + win->dec_strt;
    if (glat < 0.0) glat = 0.0;
    if (glat > 90.0) glat = 90.0;
    olat = glat;
    olon = glonc = 0.0;
    for (glonc = 0.0; glonc <= 360.0; glonc += 10.0) {
      if (clipr_xform(olat, olon, glat, glonc,
		      &xloc, &yloc, &xloc2, &yloc2, FALSE,
		      &tlat1, &tlon1, &tlat2, &tlon2)) {
	drawcurveline(tlat1, tlon1, tlat2, tlon2,
		      xloc, yloc, xloc2, yloc2, DOTTED, FALSE, 0);
      }
      olon = glonc;
    }
  }
  for (glatc = 0; glatc >= -90.0; glatc -= win->dec_step) {
    glat = glatc + win->dec_strt;
    if (glat < -90.0) glat = -90.0;
    if (glat > 0.0) glat = 0.0;
    olat = glat;
    olon = glonc = 0.0;
    for (glonc = 0.0; glonc <= 360.0; glonc += 10.0) {
      if (clipr_xform(olat, olon, glat, glonc,
		      &xloc, &yloc, &xloc2, &yloc2, FALSE,
		      &tlat1, &tlon1, &tlat2, &tlon2)) {
	drawcurveline(tlat1, tlon1, tlat2, tlon2,
		      xloc, yloc, xloc2, yloc2, DOTTED, FALSE, 0);
      }
      olon = glonc;
    }
  }
}



chartecliptic(win)
     mapwindow *win;
{
  double sineps, coseps;
  int i;
  double lat, lon, olat, olon;
  int xloc, yloc, xloc2, yloc2;
  double tlat1, tlon1, tlat2, tlon2;  

  cur_function = ECLIPT;

  sineps = DSIN(23.439);
  coseps = DCOS(23.439);
  
  olat = 0.0;
  olon = 0.0;
  for (i = 1; i <= 361; i += 10) {
    lat = DASIN(sineps*DSIN(i));
    lon = DATAN2((coseps*DSIN(i)), (DCOS(i)));
    if (lon < 0.0) lon += 360.0; /* Eliminate discontinuity at -180 */
    if (clipr_xform(olat, olon, lat, lon,
		    &xloc, &yloc, &xloc2, &yloc2, FALSE,
		    &tlat1, &tlon1, &tlat2, &tlon2)) {
	drawcurveline(tlat1, tlon1, tlat2, tlon2,
		      xloc, yloc, xloc2, yloc2, DASHED, TRUE, 0);

    }
    olat = lat;
    olon = lon;
  }
}


chartboundaries(win)
     mapwindow *win;
{
  FILE *sfile;
  if ((sfile = fopen(boundfile, READMODE)) == OPENFAIL) return(0);

  cur_function = CONSTBOUND;
  cur_file_name = boundfile;

  chartobjects(win, sfile, boundftype, 100.0, 100.0, -100.0,
				/* maglim, lbllim low, gklim high */
	       FALSE, 100.0, -100.0, ALLFILES);
				/* don't label with magnitudes */
  fclose(sfile);

  cur_file_name = "";

  return(1);
}

chartpatterns(win)
     mapwindow *win;
{
  FILE *sfile;
  if ((sfile = fopen(patternfile, READMODE)) == OPENFAIL) return(0);

  cur_function = CONSTPATTRN;
  cur_file_name = patternfile;

  chartobjects(win, sfile, pattftype, 100.0, 100.0, -100.0,
				/* maglim, lbllim low, gklim high */
	       FALSE, 100.0, -100.0, ALLFILES);
				/* don't label with magnitudes */
  fclose(sfile);

  cur_file_name = "";

  return(1);
}

chartconstlnames(win)
     mapwindow *win;
{
  FILE *sfile;
  if ((sfile = fopen(cnamefile, READMODE)) == OPENFAIL) return(0);

  cur_function = CONSTNAME;
  cur_file_name = cnamefile;

  chartobjects(win, sfile, cnameftype, 100.0, 100.0, -100.0,
				/* maglim, lbllim low, gklim high */
	       FALSE, 100.0, -100.0, ALLFILES);
				/* don't label with magnitudes */
  fclose(sfile);

  cur_file_name = "";

  return(1);
}

chartmain(win, draw_mode)
     mapwindow *win;
     int draw_mode;
{
  int i;
  char commline[132];
  
  for(i=0;i < win->numfiles; i++) {
#ifdef INTERACTIVE_CONTROL
    if (user_interact)
      if (D_break()) break;
#endif
    sprintf(commline, "file %d, type %d, '%s'\n",i,win->file[i].type,
	    win->file[i].name);
    D_comment(commline);
    if (!chartfile(win, win->file[i].name, win->file[i].type,
		   win->file[i].maglim, win->file[i].lbllim, 
		   win->file[i].gklim, win->file[i].draw_maglbl,
		   win->file[i].maglmin, win->file[i].maglmax, draw_mode))
      fprintf(stderr, "%s: error, open fail on '%s'\n",
	      progname, win->file[i].name);
  }
  D_color("  ");		/* Reset color */
}

chartfile(win, filename, filetype, maglim, lbllim, gklim,
	  draw_maglbl, maglmin, maglmax, draw_mode)
     mapwindow *win;
     char *filename;
     int filetype;
     double maglim, lbllim, gklim;
     int draw_maglbl;
     double maglmin, maglmax;
     int draw_mode;
{
  FILE *file1, *file2;
  double padding;
  double f_east, f_north, f_west, f_south;
  char ibuf[2020], fname[1030], ftypestr[32], commline[1200];
  int n;
  int xloc, yloc, inr1, inr2, inr3, inr4;
  int usethis;
  int ftype;

  if ((file1 = fopen(filename, READMODE)) == OPENFAIL) return(FALSE);

  cur_function = CHARTFILE;
  cur_file_name = filename;

  if (filetype == INDEXTYPE) {
    /* read each line of index file, parse data on the files indexed:
       region included file name
         use an indexed file
 	   if any corner is in the plot region,
	   or if the center of the plot is in the file region */
    /* file region is padded to avoid possible problems with
       corners being out of the plot but some part falling in the area
       which may occur in some clip windows shapes */
    if ((win->proj_mode == STEREOGR) || (win->proj_mode == GNOMONIC)
	|| (win->proj_mode == ORTHOGR))
      padding = win->scale;
    else padding = 1.0;

    while (!feof(file1)) {
#ifdef INTERACTIVE_CONTROL
      if (user_interact)
	if (D_break()) break;
#endif
      fgets(ibuf, 2000, file1);
      n = sscanf(ibuf, "%lf %lf %lf %lf %1000s %30s\n", &f_east, &f_north,
		 &f_west, &f_south, fname, ftypestr);
      if ((n != 6) || feof(file1)) continue;

      f_east *= 15.0;
      f_west *= 15.0;

      f_east += padding;
      f_west -= padding;
      f_north += padding;
      f_south -= padding;
      if (f_east > 360.0) f_east -= 360.0;
      if (f_west < 0.0) f_west += 360.0;
      if (f_north > 90.0) f_north = 90.0;
      if (f_south < -90.0) f_south = -90.0;

      xform(f_north, f_east, &xloc, &yloc, &inr1);
      xform(f_south, f_east, &xloc, &yloc, &inr2);
      xform(f_north, f_west, &xloc, &yloc, &inr3);
      xform(f_south, f_west, &xloc, &yloc, &inr4);

      usethis = (inr1 || inr2 || inr3 || inr4);

      /* If any corner of the file is in the region, use it. (test done above)
	 Otherwise if any corner of the region is in the file, use the file.
	 Uses xf_north, xf_south, xf_east, xf_west set by initxform()
	 Note, east is left when you look at the sky */

      if (!usethis) {
	usethis |= ((xf_north >= f_south) && (xf_north <= f_north)
		    && (eastof(xf_east,f_west)) && (westof(xf_east,f_east)));
	                                              /* Upper left is in */
	usethis |= ((xf_north >= f_south) && (xf_north <= f_north)
		    && (eastof(xf_west,f_west)) && (westof(xf_west,f_east)));
	                                              /* Upper right is in */
	usethis |= ((xf_south >= f_south) && (xf_south <= f_north)
		    && (eastof(xf_east,f_west)) && (westof(xf_east,f_east)));
	                                              /* lower left is in */
	usethis |= ((xf_south >= f_south) && (xf_south <= f_north)
		    && (eastof(xf_west,f_west)) && (westof(xf_west,f_east)));
	                                              /* lower right is in */
	usethis |= (
		    (  ((xf_north >= f_south) && (xf_north <= f_north))
		     ||((xf_south >= f_south) && (xf_south <= f_north))
		     ||((f_north >= xf_south) && (f_north <= xf_north))
		     ||((f_south >= xf_south) && (f_south <= xf_north))
		     )
		    &&
		    (  ((eastof(xf_east,f_west)) && (westof(xf_east,f_east)))
		     ||((eastof(xf_west,f_west)) && (westof(xf_west,f_east)))
		     ||((eastof(f_east,xf_west)) && (westof(f_east,xf_east)))
		     ||((eastof(f_west,xf_west)) && (westof(f_west,xf_east)))
		     )
		    );
				/* North or south of area is between
				   north and south boundaries,
				   and east or west of area is between
				   east and west boundaries. */
      }

      if (usethis) {
	if ((file2 = fopen(fname, READMODE)) == OPENFAIL) {
	  fprintf(stderr, "%s: error, couldn't open '%s' for reading.\n",
		  progname, fname);
	} else {
	  cur_file_name = fname;

	  ftype = tr_fty(ftypestr);
	  sprintf(commline, "'%s', type %d\n",fname, ftype);
	  D_comment(commline);
	  if (ftype == INDEXTYPE)  {
	    fprintf(stderr,
  "%s: error, can't use type INDEXTYPE file '%s' from another index file.\n",
		    progname, fname);
	  } else {
	    chartobjects(win, file2, ftype, maglim, lbllim, gklim,
			 draw_maglbl, maglmin, maglmax, draw_mode);
	  }
	  fclose(file2);
	  cur_file_name = "";
	}
      }
    }
  } else {
    chartobjects(win, file1, filetype, maglim, lbllim, gklim,
		 draw_maglbl, maglmin, maglmax, draw_mode);
  }
  fclose(file1);
  cur_file_name = "";
  return(TRUE);
}


double o_lat, o_lon;		/* save areas for track clipping */
char area_stat;

chartobjects(win, file, ftype, maglim, lbllim, gklim,
	     draw_maglbl, maglmin, maglmax, draw_mode)
     mapwindow *win;
     FILE *file;
     int ftype;
     double maglim, lbllim, gklim;
     int draw_mode;
     int draw_maglbl;
     double maglmin, maglmax;
{
  int xloc, yloc, xloc2, yloc2;
  int inregion;
  double tlat1, tlon1, tlat2, tlon2;
  int glyph_mode, name_mode, magl_mode, vec_mode, area_mode, all_mode;
  int use_label, use_name, use_mag;

  /* Modes:
     ALLGLYPHS		All symbols
     ALLNAMES		All names (and greek)
     ALLMAGLBLS		All magnitude labels
     ALLVECTORS		All vectors
     ALLAREAS		All areas
     ALLFILES		All of the above
  */

  glyph_mode = ((draw_mode == ALLFILES) || (draw_mode == ALLGLYPHS));
  name_mode = ((draw_mode == ALLFILES) || (draw_mode == ALLNAMES));
  magl_mode = ((draw_mode == ALLFILES) || (draw_mode == ALLMAGLBLS));
  vec_mode = ((draw_mode == ALLFILES) || (draw_mode == ALLVECTORS));
  area_mode = ((draw_mode == ALLFILES) || (draw_mode == ALLAREAS));
  all_mode = (draw_mode == ALLFILES);
  
  for(;;) {
    if ((*readfile)(file, ftype)) break;
/*    if ((obj_mag > maglim) && (obj_type[0] == 'S')) break;*/

    if (obj_type[0] == '#') continue;

#ifdef INTERACTIVE_CONTROL
    if (user_interact)
      if (D_break()) break;
#endif

    if ( (obj_mag <= maglim) )
      {
	if (precess) do_precess(&obj_lat, &obj_lon);
	xform(obj_lat, obj_lon, &xloc, &yloc, &inregion);
	if ((!inregion) && (obj_type[0] != 'V')
	    && (obj_type[0] != 'A')) continue;
	use_label = (((obj_type[0] == 'S') || (obj_type[0] == 'I')) &&
		     (obj_mag <= gklim) &&
		     (all_mode || name_mode));
	use_name =  ((obj_mag <= lbllim) && name_mode);
	use_mag =  (draw_maglbl && (obj_mag <= maglmax) &&
		    (obj_mag >= maglmin) && magl_mode);

	if (inregion)
	  drawobj(xloc, yloc, obj_mag, obj_type, obj_color, obj_label,
		  obj_constell, obj_name,
		  obj_commnt, fileline,
		  glyph_mode,
		  name_mode || magl_mode,
		  use_label,
		  use_name,
		  use_mag);
	if (obj_type[0] == 'V') {
	  D_color(obj_color);
	  if ((obj_type[1] == 'M') || (obj_type[1] == 'm')) {	 /* move */
	    o_lat = obj_lat;
	    o_lon = obj_lon;
	  } else {			 /* draw */
	    if (vec_mode && clipr_xform(o_lat, o_lon, obj_lat, obj_lon,
					&xloc, &yloc, &xloc2, &yloc2,
					(isupper(obj_type[1])),
					&tlat1, &tlon1, &tlat2, &tlon2)) {
	      switch (obj_type[1]) {
	      case 'D':
	      case 'd':
		drawcurveline(tlat1, tlon1, tlat2, tlon2, xloc, yloc,
			      xloc2, yloc2, VECDOT, (obj_type[1] == 'D'), 0);
		break;
	      case 'H':
	      case 'h':
		drawcurveline(tlat1, tlon1, tlat2, tlon2, xloc, yloc,
			      xloc2, yloc2, VECDASH, (obj_type[1] == 'H'), 0);
		break;
	      case 'S':
	      case 's':
	      default:
		drawcurveline(tlat1, tlon1, tlat2, tlon2, xloc, yloc,
			      xloc2, yloc2, VECSOLID, isupper(obj_type[1]), 0);
		break;

	      }
	    }
	    o_lat = obj_lat;
	    o_lon = obj_lon;
	  }
	  D_color("  ");
	} else if ((obj_type[0] == 'A') && area_mode) {
	  D_color(obj_color);
	  if ((obj_type[1] == 'M') || (obj_type[1] == 'm')) {
				/* move */
	    areastart(obj_lat, obj_lon, (obj_type[1] == 'M'));
	    area_stat = 'M';
	  } else if ((obj_type[1] == 'A') || (obj_type[1] == 'a')) {
				/* add segment */
	    areaadd(obj_lat, obj_lon, (obj_type[1] == 'A'));
	    area_stat = obj_type[1];
	  } else if ((obj_type[1] == 'F') || (obj_type[1] == 'f')) {
				/* fill */
	    if ((area_stat == 'A') || (area_stat == 'a'))
				/* Else, likely an error */
	      areaadd(obj_lat, obj_lon, (obj_type[1] == 'F'));
	    areafinish();
	    area_stat = obj_type[1];
	  };
	  D_color("  ");
	}
      }
  }
}
