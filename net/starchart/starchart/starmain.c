/*
 * starmain.c, starchart.c without routines moved to starsupp.c
 * starchart.c -- version 2, September 1987
 *		revision 2.1 December, 1987
 *		revision 3.2 February 1990
 * contact Craig Counterman, email: ccount@athena.mit.edu
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
 * DISCLAIMER OF WARRANTY
 * ----------------------
 * The author  disclaims all warranties  with regard to  this software to
 * the   extent  permitted  by applicable   law,  including all   implied
 * warranties  of merchantability  and  fitness. In  no event shall   the
 * author be liable for any special, indirect or consequential damages or
 * any  damages whatsoever resulting from  loss of use, data or  profits,
 * whether in an action of contract, negligence or other tortious action,
 * arising  out of  or in connection with the  use or performance of this
 * software.
 *
 */

/*
 ! Version 2 modification authors:
 !
 !   [a] Petri Launiainen, pl@intrin.FI (with Jyrki Yli-Nokari, jty@intrin.FI)
 !   [b] Bob Tidd, inp@VIOLET.BERKELEY.EDU
 !   [c] Alan Paeth, awpaeth@watcgl
 !
 */

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starmain.c,v 1.1 1990-03-30 16:39:28 vrs Exp $";

#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>

#include "star3.h"
#include "parse_input.h"

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

/*
 * default datasets are local, unless defined in Makefile
 */

#ifndef STARFILE
#define STARFILE	"./yale.star"
#endif
#ifndef STARFTYPE
#define STARFTYPE LINEREAD
#endif

#ifndef INDEXFILE
#define INDEXFILE	"./index.indx"
#endif
#ifndef INDEXFTYPE
#define INDEXFTYPE INDEXTYPE
#endif

#ifndef PLANETFILE
#define PLANETFILE	"./planet.star"
#endif
#ifndef PLANETFTYPE
#define PLANETFTYPE LINEREAD
#endif

#ifndef NEBFILE
#define NEBFILE		"./neb.star"
#endif
#ifndef NEBFTYPE
#define NEBFTYPE LINEREAD
#endif

#ifndef BOUNDFILE
#define BOUNDFILE	"./boundaries.star"
#endif
#ifndef BOUNDFTYPE
#define BOUNDFTYPE LINEREAD
#endif

#ifndef PATTERNFILE
#define PATTERNFILE	"./pattern.star"
#endif
#ifndef PATTFTYPE
#define PATTFTYPE LINEREAD
#endif

#ifndef CNAMEFILE
#define CNAMEFILE	"./cnames.star"
#endif
#ifndef CNAMEFTYPE
#define CNAMEFTYPE LINEREAD
#endif

#ifndef CONSTFILE
#define CONSTFILE	"./con.locs"
#endif

#ifndef RCFILE
#define RCFILE	"./.starrc"
#endif


/* arguments */
int g_argc;
char **g_argv;

/* Globals read from command line or set by D_userinput */
char *progname;
double ra, de, sc;
char *title = "";
double all_lbllim, all_maglim, all_gklim;
int use_lbllim = FALSE, use_maglim = FALSE, use_gklim = FALSE;

int user_interact = FALSE;

int precess = FALSE;
double to_precess = 2000.0;

double all_vmin, all_vmax;
int use_vmin = FALSE;
int nomaglbls = FALSE;

double all_rstep, all_dstep;
int use_rstep = FALSE;
int no_ra_grid = FALSE;
int no_dec_grid = FALSE;
double all_rstrt = 0.0, all_dstrt = 0.0;

int all_invert = FALSE;

int chart_type = THREEPART;

int all_proj_mode = SANSONS;

char *rcfile = RCFILE;

char *constfile = CONSTFILE;
char *boundfile = BOUNDFILE;
char boundftype = BOUNDFTYPE;
char *patternfile = PATTERNFILE;
char pattftype = PATTFTYPE;
char *cnamefile = CNAMEFILE;
char cnameftype = CNAMEFTYPE;
char *mapfiles[MAXMAPFILES];
int mapftypes[MAXMAPFILES];
int nummapfiles;

mapwindow *mapwin[MAXWINDOWS];
int numwins;

int cur_function;
int cur_map_type;
int cur_map_tag;
char *cur_tag_field;
char *cur_file_name = "";
mapwindow *cur_win;

int read_mapwin_file;
int write_mapwin_file;
char mapwin_file[MAXPATHLEN];


/* read file function indirection to allow for use of alternate file readers */
int readstar();
int buf_readstar();
/* Macintosh MPW currently doesn't support buf_readstar */
#ifndef macintosh
int (*readfile)() = buf_readstar;
#else
int (*readfile)() = readstar;
#endif

/* Readstar globals */
double obj_lat, obj_lon, obj_mag;
char obj_type[] ="SS", obj_color[3], obj_label[3];
char obj_constell[3], obj_name[LINELEN];
char *obj_commnt, fileline[LINELEN];

/* xform globals */
extern double xf_west, xf_east, xf_north, xf_south, xf_bottom;
extern int xf_xcen, xf_ycen, xf_ybot;
extern int xf_w_left, xf_w_right, xf_w_top, xf_w_bot;

extern double xf_c_scale;

/* Other globals */
int all_layer[MAXLAYRS];
int numlayers;

/* storage area big enough for inputs */
char a_title[MAXPATHLEN];
char a_starfile[MAXPATHLEN];
char a_indexfile[MAXPATHLEN];
char a_planetfile[MAXPATHLEN];
char a_nebfile[MAXPATHLEN];
char a_constfile[MAXPATHLEN];
char a_boundfile[MAXPATHLEN];
char a_patternfile[MAXPATHLEN];
char a_cnamefile[MAXPATHLEN];
char a_userfile[MAXMAPFILES][MAXPATHLEN];


/* externals */
extern mapwindow fullpage;
extern mapwindow mainmap;
extern mapwindow thumbmap;
extern int htick_lim, htext_lim;
extern int htext_xoff, htext_yoff;
extern int vtick_lim, vtext_lim;
extern int vtext_xoff, vtext_yoff;

/* the code */
main(argc, argv)
     int argc;
     char *argv[];
{
  int i, j, k;
  int imap;
  int ilay;
  mapwindow *win;
#define COMMBUFLEN 1000
  char commbuf[COMMBUFLEN+2];
  FILE *mfile;

  /* Set global arguments */
  g_argc = argc;
  g_argv = argv;
  progname = g_argv[0];
  
  /* Set defaults */
  ra = 0.0;
  de = 0.0;
  sc = 15.0;

  mapfiles[YALEF] = a_starfile;
  strcpy(a_starfile, STARFILE);
  mapfiles[INDXF] = a_indexfile;
  strcpy(a_indexfile, INDEXFILE);
  mapfiles[PLANF] = a_planetfile;
  strcpy(a_planetfile, PLANETFILE);
  mapfiles[NEBF] = a_nebfile;
  strcpy(a_nebfile, NEBFILE);
  mapftypes[YALEF] = STARFTYPE;
  mapftypes[INDXF] = INDEXFTYPE;
  mapftypes[PLANF] = PLANETFTYPE;
  mapftypes[NEBF] = NEBFTYPE;
  nummapfiles = NUMFS;
  
  for (i = 0; i < MAXUFILES; i++) {
    mapfiles[NUMFS+i] = a_userfile[i];
    mapftypes[NUMFS+i] = LINEREAD;
  }
  
  all_layer[0] = OUTLINE;
  all_layer[1] = RA_GRID;
  all_layer[2] = DEC_GRID;
  all_layer[3] = ECLIPTIC;
  all_layer[4] = BOUNDARIES;
  all_layer[5] = PATTERNS;
  all_layer[6] = CONSTLNAMES;
  all_layer[7] = ALLFILES;
  all_layer[8] = LEGENDS;
  numlayers = 9;
  
  /* Read .starrc or other rc file */
  if (!rc_read(rcfile));
  /* couldn't open rcfile, but that's OK */
  
  /* Read command line: may override .starrc */
  if (!commandline());
  /* no arguments, but that's OK */
  
#ifdef INTERACTIVE_CONTROL
  while (D_userinput()) {
#endif
  
  /* set up maps */
  switch (chart_type) {
  case THREEPART:
    mapwin[0] = &mainmap;
/* it's okay to have thumbmap in stereographic mode now
    if (all_proj_mode == STEREOGR) {
      numwins = 1;
    } else {
      mapwin[1] = &thumbmap;
      numwins = 2;
    }
*/
    mapwin[1] = &thumbmap;
    numwins = 2;
    break;
  case FULLPAGE:
    mapwin[0] = &fullpage;
    numwins = 1;
    break;
  default:
    numwins = 0;
    break;
  }
  
	  
  for (i = 0; i < numwins; i++) {
    if (use_maglim) 
      mapwin[i]->maglim = all_maglim;

    if (use_lbllim) mapwin[i]->lbllim = all_lbllim;
    if (use_gklim) mapwin[i]->gklim = all_gklim;

    mapwin[i]->racen = ra;
    mapwin[i]->dlcen = de;
    
    mapwin[i]->scale = sc;

    mapwin[i]->proj_mode = all_proj_mode;
    
   
    if (use_rstep) {
      mapwin[i]->draw_ragrid = TRUE;
      mapwin[i]->draw_decgrid = TRUE;
      mapwin[i]->ra_step = all_rstep;
      mapwin[i]->dec_step = all_dstep;
      mapwin[i]->ra_strt = all_rstrt;
      mapwin[i]->dec_strt = all_dstrt;
    }
    
    if (no_ra_grid) mapwin[i]->draw_ragrid = FALSE;
    if (no_dec_grid) mapwin[i]->draw_decgrid = FALSE;

    if (all_invert) mapwin[i]->invert = TRUE;
    
    for (j =0; j < numlayers; j++)
      mapwin[i]->layer[j] = all_layer[j];
    mapwin[i]->nlayers= numlayers;

    for (j =0; j < nummapfiles; j++) {
      mapwin[i]->file[j].name = mapfiles[j];
      mapwin[i]->file[j].type = mapftypes[j];
      mapwin[i]->file[j].maglim = mapwin[i]->maglim;
      mapwin[i]->file[j].lbllim = mapwin[i]->lbllim;
      mapwin[i]->file[j].gklim = mapwin[i]->gklim;

      if (use_vmin)  {
	mapwin[i]->file[j].draw_maglbl = TRUE;
	mapwin[i]->file[j].maglmin = all_vmin;
	mapwin[i]->file[j].maglmax = all_vmax;
      }
      if (nomaglbls) mapwin[i]->file[j].draw_maglbl = FALSE;
    }
    mapwin[i]->numfiles = nummapfiles;
  }
	  
  /* Give driver a chance to change the mapwins */
  fix_mapwin();
#ifdef INTERACTIVE_CONTROL
  if (user_interact)
    D_mapwininput();
#endif

	  
  /* Begin chart */
  if (!D_open())
    die("couldn't open device", "");

  k = 0;
  commbuf[0] = '\0';
  for (i = 0; (i < argc) && (k < COMMBUFLEN); i++) {
    sprintf(&commbuf[strlen(commbuf)], "%s ", argv[i]);
  }
  D_comment(commbuf);

  /* Write the current settings to mapwin file */
  /* Must be done here to allow D_open to change settings, esp. window size */
  if ((write_mapwin_file)  && ((mfile = fopen(mapwin_file, "w")) != NULL)) {
    write_mfile(mfile);
    fclose(mfile);
  }
  
  if (precess)
    initprecess(to_precess);


  D_comment("Start");

  /* Draw the chart */
  /* For each map
       for each layer
         draw the layer */
 
  for (imap = 0; imap < numwins; imap++) {
#ifdef INTERACTIVE_CONTROL
  if (user_interact)
    if (D_break()) break;
#endif
    win = mapwin[imap];
    if (!chartparms(win)) continue;
    if (!initxform(win)) continue;	/* initialize xform before using it */

    cur_map_type = win->map_type;
    cur_map_tag = win->tag;
    cur_tag_field = win->tag_field;
    cur_win = mapwin[imap];

    for (ilay = 0; ilay < win->nlayers; ilay++) {
#ifdef INTERACTIVE_CONTROL
      if (user_interact)
	if (D_break()) break;
#endif
      switch (win->layer[ilay]) {
      case OUTLINE:
	D_comment("OUTLINE");
	chartoutline(win);
	break;
      case RA_GRID:
	D_comment("RA_GRID");
	chartragrid(win);
	break;
      case DEC_GRID:
	D_comment("DEC_GRID");
	chartdecgrid(win);
	break;
      case ECLIPTIC:
	D_comment("ECLIPTIC");
	chartecliptic(win);
	break;
      case BOUNDARIES:
	D_comment("BOUNDARIES");
	chartboundaries(win);
	break;
      case PATTERNS:
	D_comment("PATTERNS");
	chartpatterns(win);
	break;
      case CONSTLNAMES:
	D_comment("CONSTLNAMES");
	chartconstlnames(win);
	break;
      case ALLFILES:
	D_comment("ALLFILES");
	chartmain(win, ALLFILES);
	break;
      case ALLGLYPHS:
	D_comment("ALLGLYPHS");
	chartmain(win, ALLGLYPHS);
	break;
      case ALLNAMES:
	D_comment("ALLNAMES");
	chartmain(win, ALLNAMES);
	break;
      case ALLMAGLBLS:
	D_comment("ALLMAGLBLS");
	chartmain(win, ALLMAGLBLS);
	break;
      case ALLVECTORS:
	D_comment("ALLVECTORS");
	chartmain(win, ALLVECTORS);
	break;
      case ALLAREAS:
	D_comment("ALLAREAS");
	chartmain(win, ALLAREAS);
	break;
      case LEGENDS:
	D_comment("LEGENDS");
	chartlegend(win);
	break;
      }
    }
  }
  
  /* Done chart: close and exit */
  D_comment("Close");
  D_close();

#ifdef INTERACTIVE_CONTROL
  }
#endif


  exit(0);
}


commandline()
{
  int j, k;
  double varmag, varrange;
  int loc_argc;
/*
 * check command line style
 */
  if (g_argc == 1) return (FALSE);

  if  ( (g_argv[1][0] == '-') && g_argv[1][1] ) {
/*
 * new style command line (using flags)
 */
    loc_argc = g_argc;		/* in case D_control_args changes g_argc */
    for (j=1; j<loc_argc; j++) {
      if (g_argv[j][0] != '-') die("unknown argument - '%s'", g_argv[j]);
      switch (g_argv[j][1]) {

#ifdef INTERACTIVE_CONTROL
      case 'u': user_interact = TRUE; break;
#endif

      case 'r': ra = htod(g_argv[++j])*15.0; break; /* hrs -> deg */
      case 'd': de = htod(g_argv[++j]); break;
      case 's': sc = atof(g_argv[++j]); break;

      case 'm': all_maglim = (double)(atof(g_argv[++j]));
	use_maglim = TRUE; break;
      case 'l': all_lbllim = (double)(atof(g_argv[++j]));
	use_lbllim = TRUE; break;
      case 'g': all_gklim  = (double)(atof(g_argv[++j]));
	use_gklim = TRUE; break;

      case 'p':
	if (((j+1) < loc_argc) && (g_argv[j+1][0] != '-'))
	  {
	    if (k = tr_pmode(g_argv[++j])) {
	      all_proj_mode = k;
	    }
	  }
	else all_proj_mode = STEREOGR;
	break;

      case 'b': chart_type = FULLPAGE; break;

      case 'i': all_invert = TRUE; break;

      case 't': title = g_argv[++j]; break;

	/* can be filename filetype  or just  filename */
      case 'y':
	strcpy(mapfiles[YALEF], g_argv[++j]);
	if (((j+1) < loc_argc) && (g_argv[j+1][0] != '-') && 
	    tr_fty(g_argv[j+1]))
	  mapftypes[YALEF] = tr_fty(g_argv[++j]);
	break;
      case 'h':  strcpy(mapfiles[INDXF], g_argv[++j]); break;
      case 'n':
	strcpy(mapfiles[NEBF], g_argv[++j]);
	if (((j+1) < loc_argc) && (g_argv[j+1][0] != '-') && 
	    tr_fty(g_argv[j+1]))
	  mapftypes[NEBF] = tr_fty(g_argv[++j]);
	break;
      case 'w':
	strcpy(mapfiles[PLANF], g_argv[++j]);
	if (((j+1) < loc_argc) && (g_argv[j+1][0] != '-') && 
	    tr_fty(g_argv[j+1]))
	  mapftypes[PLANF] = tr_fty(g_argv[++j]);
	break;
      case 'f': 
	if (nummapfiles == MAXMAPFILES) {
	  fprintf(stderr, "Too many user files\n");
	  j++;
	} else {
	  strcpy(mapfiles[nummapfiles], g_argv[++j]);
	  if (((j+1) < loc_argc) && (g_argv[j+1][0] != '-') && 
	      tr_fty(g_argv[j+1]))
	    mapftypes[nummapfiles] = tr_fty(g_argv[++j]);
	  else
	    mapftypes[nummapfiles] = LINEREAD;
	  nummapfiles++;
	}
	break;

      case 'c':	/* could be -c con or -c con con.locs */
	if (((j+2) < loc_argc) && (g_argv[j+2][0] != '-'))
	  {
	    constfile = g_argv[j+2];
	    findconst(g_argv[++j]);
	    j++;
	  }
	else findconst(g_argv[++j]);
	break;
	
	
      case 'v':
	if (((j+2) < loc_argc) && (g_argv[j+2][0] != '-')
	    && (g_argv[j+1][0] != '-')) {
	  varmag = atof(g_argv[j+1]);
	  varrange = atof(g_argv[j+2]);
	  use_vmin = TRUE;
	  all_vmin = varmag - varrange;
	  all_vmax = varmag + varrange;
	  j += 2;
	} else die("Need varmag varrange for -v","");
	break;

      case 'x':
	if (((j+2) < loc_argc) && (g_argv[j+2][0] != '-')
	    && (g_argv[j+1][0] != '-')) {
	  use_rstep = TRUE;
	  all_rstep = htod(g_argv[j+1])*15.0;
	  all_dstep = htod(g_argv[j+2]);
	  if (((j+4) < loc_argc)
	      && (!isalpha(g_argv[j+3][1]))
	      && (!isalpha(g_argv[j+4][1]))) {
	    all_rstrt = htod(g_argv[j+3])*15.0;
	    all_dstrt = htod(g_argv[j+4]);
	    j += 4;
	  } else
	    j += 2;
	} else die("Need ra_step de_step for -x","");
	break;

      case 'e':
	if (!rc_read(g_argv[++j]))
	  fprintf(stderr,
		  "%s: error, couldn't read rc file '%s'\n",
		  progname, g_argv[j]);
	break;

      case 'a':
	D_control_arg(g_argv[++j]);
	break;
	
      default:
	if (!ident_arg(&j))
	  die("unknown switch - '%s'", g_argv[j]);
	break;
      }
      if (j == loc_argc) die("trailing command line flag - '%s'", g_argv[j-1]);
    }
  }
  else
    {
      /*
       * old style command line (position dependent)
       */
      fprintf(stderr, "Old style command line no longer supported\n");
      fprintf(stderr, "Please convert:\n");
      fprintf(stderr, "old: Ra Dcl Scale Title Maglim Labellim\n");
      fprintf(stderr, "new: -r Ra -d Dcl -s Scale -t Title\n");
    }
  return (TRUE);
}

die(a,b)
     char *a, *b;
{
  /* Currently only Gdos needs this, there appl_exit() is always necessary */
#ifdef ATARI_ST
  D_die(a,b);
#endif
  fprintf(stderr,"%s: ", progname);
  fprintf(stderr,a,b);
  fprintf(stderr,"\n");
  exit(1);
}

double htod(s)
     char *s;
{
  /*
   * htod(str) reads floating point strings of the form {+-}hh.{mm} thus
   * allowing for fractional values in base 60 (ie, degrees/minutes).
   */
  return(ftod(atof(s)));
}

double ftod(x)
     double x;
{
  int full;
  full = x;
  return(full + (x-full)*100.0/60.0);
}


/* dtof(x) undoes ftod */
double dtof(x)
     double x;
{
  int full;
  full = x;
  return(full + (x-full)*60.0/100.0);
}

/* Adapted from code from Dean Payne deanp@hplsla.hp.com */
/* Format of each line is
	variable_name variable_value
or	variable_name=variable_value

variable_value may be a string which extends to the end of line.
Lines must be 80 chars max.
Comments are anything after a #
Blank lines are allowed.
*/

rc_read(filename)
     char *filename;
{
  FILE *sfile;
  char sbuf[LINELEN], *var_name, *c_buf;
  double f_buf;	/* value parsed as double */
  int i_buf;	/* value parsed as integer */
  int l_buf;	/* value parsed as TRUE or FALSE */
  int ret_code, var_type;

  
  if ((sfile = fopen(filename, READMODE)) == OPENFAIL) return(FALSE);

  fgets(sbuf, LINELEN, sfile);
  if (!(strcmp(sbuf,
	      "#mapwin format 1 file: don't edit this line or change order of lines in this file"))) {
    /* This file is a mapwin file */
    strncpy(mapwin_file, filename, MAXPATHLEN-1);
    read_mapwin_file = TRUE;
    return TRUE;
  };
    
  for (;;) {
    /* We read the first line into  sbuf above, and will read the next line 
       at the bottom of the loop */

    if (ferror(sfile)) { fprintf(stderr, "file read error\n"); break; }
    if (feof(sfile)) break;
    
    if (!parse_line(sbuf, &var_name, &ret_code, &var_type,
		    &c_buf, &f_buf, &i_buf, &l_buf)) {
      /* Not recognized by parse routine,
	 see if ident_rc can recognize it */
      if (!(ident_rc(var_name, c_buf))) {
	fprintf(stderr, "cannot interpret '%s = %s'\n", var_name, c_buf);
	help_vars(var_name);
      }
    } else
      switch (ret_code) {
      case TINT:
	fprintf(stderr, "int = %d\n", i_buf);
	fprintf(stderr, "i_buf = %d\n", i_buf);
	fprintf(stderr, "f_buf = %f\n", f_buf);
	fprintf(stderr, "l_buf = %d\n", l_buf);
	fprintf(stderr, "c_buf = %s\n", c_buf);
	break;
      case TFLO:
	fprintf(stderr, "flo = %f\n", f_buf);
	fprintf(stderr, "i_buf = %d\n", i_buf);
	fprintf(stderr, "f_buf = %f\n", f_buf);
	fprintf(stderr, "l_buf = %d\n", l_buf);
	fprintf(stderr, "c_buf = %s\n", c_buf);
	break;
      case TBOOL:
	fprintf(stderr, "bool = %d\n", l_buf);
	fprintf(stderr, "i_buf = %d\n", i_buf);
	fprintf(stderr, "f_buf = %f\n", f_buf);
	fprintf(stderr, "l_buf = %d\n", l_buf);
	fprintf(stderr, "c_buf = %s\n", c_buf);
	break;
      case TNONE:
	fprintf(stderr, "none = %s\n", c_buf);
	fprintf(stderr, "i_buf = %d\n", i_buf);
	fprintf(stderr, "f_buf = %f\n", f_buf);
	fprintf(stderr, "l_buf = %d\n", l_buf);
	fprintf(stderr, "c_buf = %s\n", c_buf);
	break;
      case TCHAR:
	fprintf(stderr, "char = %s\n", c_buf);
	fprintf(stderr, "i_buf = %d\n", i_buf);
	fprintf(stderr, "f_buf = %f\n", f_buf);
	fprintf(stderr, "l_buf = %d\n", l_buf);
	fprintf(stderr, "c_buf = %s\n", c_buf);
	break;
      case R_NONE:
	/* blank line */
	break;
      case END_INPUT:
	return TRUE;
	break;
      case EXIT:
	exit(0);
	break;
      case HELP_ME:
	help_vars("");
	break;
      case WRITE_RC_FILE:
	{
	  FILE *tfile;

	  if ((tfile = fopen(c_buf, "w")) != NULL)
	    if (rc_write(tfile)) {
	      fclose(tfile);
	      printf("wrote %s\n", c_buf);
	    };
	};
	break;
      default:
	/* Identified but not special to this routine,
	   try to set variables */
	if (!set_varvals(var_name, ret_code, var_type,
		    c_buf, f_buf, i_buf, l_buf)) {
	  fprintf(stderr, "cannot interpret '%s = %s'\n", var_name, c_buf);
	  help_vars(var_name);
	};
	break;
      };

    /* Always call ident_rc */
    ident_rc(var_name, c_buf);


    fgets(sbuf, LINELEN, sfile);
  }
  fclose(sfile);
  return (TRUE);
}


rc_write(tfile)
FILE *tfile;
{
  int i;

  fprintf(tfile, "ra = %g\n", dtof(ra / 15.0));
  fprintf(tfile, "dec = %g\n", dtof(de));
  fprintf(tfile, "scale = %g\n", sc);
  if (use_lbllim)
    fprintf(tfile, "namelim = %g\n", all_lbllim);
  if (use_gklim)
    fprintf(tfile, "gklim = %g\n", all_gklim);
  if (use_maglim)
    fprintf(tfile, "maglim = %g\n", all_maglim);

  fprintf(tfile, "title = %s\n", title);

  for (i = 0; i < nummapfiles; i++)
    fprintf(tfile, "mapfile = %s %s\n", mapfiles[i], inv_ftypes[mapftypes[i]]);

  fprintf(tfile, "constfile = %s\n", constfile);
  fprintf(tfile, "boundfile = %s\n", boundfile);
  fprintf(tfile, "patternfile = %s\n", patternfile);
  fprintf(tfile, "constlnamefile = %s\n", cnamefile);

  for (i = 0; i < numlayers; i++)
    fprintf(tfile, "all_layer = %s\n", inv_layrs[all_layer[i]]);

  fprintf(tfile, "projection_mode = %s\n", inv_projs[all_proj_mode]);

  fprintf(tfile, "chart_type = %s\n", inv_ctypes[chart_type]);

  fprintf(tfile, "invert = %s\n", all_invert ? "true" : "false");

  if (use_vmin)
    fprintf(tfile, "vrange %g %g\n", all_vmin, all_vmax);

  if (use_rstep)
    fprintf(tfile, "grid %g %g %g %g\n", dtof(all_rstep/15.0), dtof(all_dstep),
	    dtof(all_rstrt/15.0), dtof(all_dstrt));

  if (precess)
    fprintf(tfile, "precess_to %g\n", to_precess);

  i = ferror(tfile);
  return !i;
} 
  

set_varvals(var_name, ret_code, var_type, c_buf, f_buf, i_buf, l_buf)
     char *var_name;
     int ret_code, var_type;
     char *c_buf;
     double f_buf;	/* value parsed as double */
     int i_buf;	/* value parsed as integer */
     int l_buf;	/* value parsed as TRUE or FALSE */
{
  int i;
  static int nufiles = 0;
  static int nmapfs = 0;
  static int nlays = 0;
  double varmag, varrange;
  int good_flag;
  char tbuf1[LINELEN], tbuf2[LINELEN], tbuf3[LINELEN], tbuf4[LINELEN];


  good_flag = FALSE;

  switch (ret_code) {
  case R_RA:
    ra = htod(c_buf)*15.0;
    good_flag = TRUE; 
    break;
  case R_DEC:
    de = htod(c_buf); 
    good_flag = TRUE; 
    break;
  case R_SCALE:
    sc = f_buf; 
    good_flag = TRUE; 
    break;
  case R_NAMELIM:
    all_lbllim = f_buf; 
    use_lbllim = TRUE;
    good_flag = TRUE; 
    break;
  case R_MAGLIM:
    all_maglim = f_buf; 
    use_maglim = TRUE;
    good_flag = TRUE; 
    break;
  case R_GKLIM:
    all_gklim = f_buf; 
    use_gklim = TRUE;
    good_flag = TRUE; 
    break;
  case R_TITLE:
    strcpy(a_title, c_buf); 
    title = a_title; 
    good_flag = TRUE; 
    break;
  case R_STARFILE:
    strcpy(a_starfile, c_buf);  
    mapfiles[YALEF] = a_starfile;
    good_flag = TRUE; 
    break;
  case R_INDEXFILE:
    strcpy(a_indexfile, c_buf);
    mapfiles[INDXF] = a_indexfile;
    good_flag = TRUE; 
    break;
  case R_NEBFILE:
    strcpy(a_nebfile, c_buf); 
    mapfiles[NEBF] = a_nebfile;
    good_flag = TRUE; 
    break;
  case R_PLANETFILE:
    strcpy(a_planetfile, c_buf); 
    mapfiles[PLANF] = a_planetfile;
    good_flag = TRUE; 
    break;
  case R_USERFILE:
    if (nummapfiles == MAXMAPFILES) {
      fprintf(stderr, "Too many user files\n");
    } else {
      i = sscanf(c_buf, "%s %s", a_userfile[nufiles], tbuf1);
      if ((i == 2) && tr_fty(tbuf1))	/* There's a type specified */
	mapftypes[nummapfiles] = tr_fty(tbuf1);
      else mapftypes[nummapfiles] = LINEREAD;
      
      mapfiles[nummapfiles] = a_userfile[nufiles];
      nufiles++;
      nummapfiles++;
    }
    good_flag = TRUE;
    break;
  case R_MAPFILE:
    if (nmapfs >= MAXMAPFILES)
      fprintf(stderr, "Too many user files\n");
    else {
      if (nmapfs == 0) {
	/* reset mapfiles pointers */
	for (i = 0; i < MAXMAPFILES; i++)
	  mapfiles[i] = a_userfile[i];
      };

      i = sscanf(c_buf, "%s %s", a_userfile[nmapfs], tbuf1);
      if ((i == 2) && tr_fty(tbuf1))	/* There's a type specified */
	mapftypes[nmapfs] = tr_fty(tbuf1);
      else mapftypes[nmapfs] = LINEREAD;
      
      mapfiles[nmapfs] = a_userfile[nmapfs];
      
      nummapfiles = ++nmapfs;
    }
    good_flag = TRUE;
    break;
  case R_CONSTFILE:
    strcpy(a_constfile, c_buf);
    constfile = a_constfile; 
    good_flag = TRUE; 
    break;
  case R_FINDCONST:
    findconst(c_buf); 
    good_flag = TRUE; 
    break;
  case R_BOUNDFILE:
    strcpy(a_boundfile, c_buf);
    boundfile = a_boundfile; 
    good_flag = TRUE; 
    break;
  case R_PATTERNFILE:
    strcpy(a_patternfile, c_buf);
    patternfile = a_patternfile; 
    good_flag = TRUE; 
    break;
  case R_CONSTLNAMEFILE:
    strcpy(a_cnamefile, c_buf);
    cnamefile = a_cnamefile; 
    good_flag = TRUE; 
    break;
  case R_LAYERS:
    if (nlays >= MAXLAYRS)
      fprintf(stderr, "Too many user layers\n");
    else {
      if (i = tr_layr(c_buf)) {
	all_layer[nlays] = i;
	numlayers = ++nlays;
      }
    }
    good_flag = TRUE;
    break;
  case R_POLARMODE:
    if (l_buf) all_proj_mode = STEREOGR; 
    good_flag = TRUE; 
    break;
  case R_BIGFLAG:
    if (l_buf) chart_type = FULLPAGE; 
    good_flag = TRUE; 
    break;
  case R_PROJECTION_MODE:
    if (i = tr_pmode(c_buf)) {
      all_proj_mode = i;
    }
    good_flag = TRUE;
    break;
  case R_INVERT:
    all_invert = l_buf; 
    good_flag = TRUE; 
    break;
  case R_CHART_TYPE:
    if (i = tr_ctype(c_buf)) {
      chart_type = i;
    }
    good_flag = TRUE;
    break;
  case R_VRANGE:
    i = sscanf(c_buf, "%lf %lf", &all_vmin, &all_vmax);
    use_vmin = (i == 2);
    good_flag = TRUE;
    break;
  case R_VMAGS:
    i = sscanf(c_buf, "%lf %lf", &varmag, &varrange);
    if (i == 2) {
      use_vmin = TRUE;
      all_vmin = varmag - varrange;
      all_vmax = varmag + varrange;
    }
    good_flag = TRUE;
    break;
  case R_GRID:
    i = sscanf(c_buf, "%s %s %s %s", tbuf1, tbuf2, tbuf3, tbuf4);
    all_rstep = htod(tbuf1)*15.0;
    all_dstep = htod(tbuf2);
    all_rstrt = htod(tbuf3)*15.0;
    all_dstrt = htod(tbuf4);
    use_rstep = (i > 1);
    good_flag = TRUE;
    break;
  case R_NOGRID:
    no_ra_grid = no_dec_grid = l_buf; 
    good_flag = TRUE; 
    break;
  case R_NOGRID_RA:
    no_ra_grid = l_buf; 
    good_flag = TRUE; 
    break;
  case R_NOGRID_DEC:
    no_dec_grid = l_buf; 
    good_flag = TRUE; 
    break;
  case R_NOMAGLBLS:
    nomaglbls = l_buf; 
    good_flag = TRUE; 
    break;
  case R_PRECESS_TO:
    to_precess = f_buf;
    precess = TRUE;
    good_flag = TRUE;
    break;
  case R_DRIVER_CONTROL:
    D_control_arg(c_buf);
    good_flag = TRUE;
    break;
  case R_WRITE_MAPWIN_FILE:
    write_mapwin_file = TRUE;
    strncpy(mapwin_file, c_buf, MAXPATHLEN-1);
    good_flag = TRUE;
    break;
  case R_READ_MAPWIN_FILE:
    read_mapwin_file = TRUE;
    strncpy(mapwin_file, c_buf, MAXPATHLEN-1);
    good_flag = TRUE;
    break;
  default:
    break;
  };

  return good_flag;
}


int tr_layr(s)
     char *s;
{
  int i;

  for (i = 0; s[i]; i++)
    if (isupper(s[i]))
      s[i] = tolower(s[i]);

  if(!strcmp(s, "outline")) return OUTLINE;
  else if (!strcmp(s, "ra_grid")) return RA_GRID;
  else if (!strcmp(s, "dec_grid")) return DEC_GRID;
  else if (!strcmp(s, "ecliptic")) return ECLIPTIC;
  else if (!strcmp(s, "boundaries")) return BOUNDARIES;
  else if (!strcmp(s, "patterns")) return PATTERNS;
  else if (!strcmp(s, "constlnames")) return CONSTLNAMES;
  else if (!strcmp(s, "allfiles")) return ALLFILES;
  else if (!strcmp(s, "allglyphs")) return ALLGLYPHS;
  else if (!strcmp(s, "allnames")) return ALLNAMES;
  else if (!strcmp(s, "allmaglbls")) return ALLMAGLBLS;
  else if (!strcmp(s, "allvectors")) return ALLVECTORS;
  else if (!strcmp(s, "allareas")) return ALLAREAS;
  else if (!strcmp(s, "legends")) return LEGENDS;
  else return 0;
}


int tr_pmode(s)
     char *s;
{
  int i;

  for (i = 0; s[i]; i++)
    if (isupper(s[i]))
      s[i] = tolower(s[i]);

  if(!strcmp(s, "stereogr")) return STEREOGR;
  else if(!strcmp(s, "stereographic")) return STEREOGR;
  else if (!strcmp(s, "sansons")) return SANSONS;
  else if (!strcmp(s, "gnomonic")) return GNOMONIC;
  else if (!strcmp(s, "orthogr")) return ORTHOGR;
  else if (!strcmp(s, "orthographic")) return ORTHOGR;
  else if (!strcmp(s, "rectangular")) return RECTANGULAR;
  else return 0;
}


int tr_ctype(s)
     char *s;
{
  int i;
  for (i = 0; s[i]; i++)
    if (isupper(s[i]))
      s[i] = tolower(s[i]);

  if(!strcmp(s, "fullpage")) return FULLPAGE;
  else if (!strcmp(s, "threepart")) return THREEPART;
  else return 0;
}

int tr_fty(s)
     char *s;
{
  int i;

  for (i = 0; s[i]; i++)
    if (isupper(s[i]))
      s[i] = tolower(s[i]);

  if(!strcmp(s, "lineread")) return LINEREAD;
  else if (!strcmp(s, "indextype")) return INDEXTYPE;
  else if (!strcmp(s, "binfull")) return BINFULL;
  else if (!strcmp(s, "binobj")) return BINOBJ;
  else if (!strcmp(s, "binstar")) return BINSTAR;
/*  else if (!strcmp(s, "saoformat")) return SAOFORMAT;*/
  else if (!strcmp(s, "gsc")) return GSCTYPE;
  else return 0;
}



static char title_string[LINELEN];

findconst(tag)
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
  int i;
  
  if ((cfile = fopen(constfile, "r")) == NULL)
    die("open fail on '%s'", constfile);
  taglen = strlen(tag);
  if ((taglen < 3) || (taglen > 4))
    die("constellation name must be three or four characters","");
  for (;;)
    {
      fgets(cbuf, LINELEN, cfile);
      if (ferror(cfile)) die("read error in '%s'", constfile);
      if (feof(cfile)) break;
      if (strncmp(tag, cbuf, taglen) == 0)	/* FOUND */
	{	
	  if (4!=sscanf(cbuf,"%*5s%lf%lf%lf %[^\n]",
			&ra, &de, &sc, title_string))
	    die("bogus line in constellation file: \"%s\"", cbuf);

	  ra    = ftod(ra)*15.0;
	  de    = ftod(de);
	  
	  /* remove trailing \n */
	  i = strlen(title_string);
	  if (title_string[i-1] == '\n') title_string[i-1] = '\0';
	  title = title_string;
	  return;
	}
    }
  die("Constellation '%s' not found", tag);
}

/* check validity of win structure */
chartparms(win)
     mapwindow *win;
{
  double maxdl;

  /*
   * check for bogus ra, decl, or scale
   */
  if ((win->racen >= 360.0) || (win->racen < 0.0))
    die("right ascension out of range","");
  if ((win->dlcen > 90.0) || (win->dlcen < -90.0))
    die("declination out of range","");
  if (win->scale < 0.0)
    die("scale negative","");

  /*
   * check for pole wrapping and do a potential clip
   */
  if (win->proj_mode == SANSONS) {
    maxdl = (90.0 - fabs(win->dlcen))*2.0;
    if (win->scale > maxdl) win->scale = maxdl;
    if (win->scale <= 0.0) { /* North pole is included in chart */
      return(FALSE);
    }
  };

      return(TRUE);
}



/* Dump current mapwin information to file. */
write_mfile(mfile)
     FILE *mfile;
{
  int i, j;
  
  /* Tag file as being mapwin format */
  fprintf(mfile,
	  "#mapwin format 1 file: don't edit this line or change order of lines in this file\n");
  /* Write the title */
  fprintf(mfile, "title=%s\n", title);

  /* write number of mapwins */
  fprintf(mfile, "numwins=%d\n", numwins);
  for (i = 0; i < numwins; i++) {
    fprintf(mfile, "mapwin=%d\n",i);
    
    fprintf(mfile, "width=%d\n", mapwin[i]->width);
    fprintf(mfile, "height=%d\n", mapwin[i]->height);
    fprintf(mfile, "x_offset=%d\n", mapwin[i]->x_offset);
    fprintf(mfile, "y_offset=%d\n", mapwin[i]->y_offset);
    
    fprintf(mfile, "maglim=%.10f\n", mapwin[i]->maglim);
    fprintf(mfile, "lbllim=%.10f\n", mapwin[i]->lbllim);
    fprintf(mfile, "gklim=%.10f\n", mapwin[i]->gklim);
    
    
    fprintf(mfile, "map_type=%d\n", mapwin[i]->map_type);
    
    
    fprintf(mfile, "tag=%d\n", mapwin[i]->tag);
    
    fprintf(mfile, "tag_field=%s\n", mapwin[i]->tag_field);
    
    
    fprintf(mfile, "proj_mode=%d\n", mapwin[i]->proj_mode);
    
    
    fprintf(mfile, "draw_ragrid=%d\n", mapwin[i]->draw_ragrid);
    fprintf(mfile, "draw_decgrid=%d\n", mapwin[i]->draw_decgrid);
    
    fprintf(mfile, "ra_step=%.10f\n", mapwin[i]->ra_step);
    fprintf(mfile, "dec_step=%.10f\n", mapwin[i]->dec_step);
    
    fprintf(mfile, "ra_strt=%.10f\n", mapwin[i]->ra_strt);
    fprintf(mfile, "dec_strt=%.10f\n", mapwin[i]->dec_strt);
    
    
    fprintf(mfile, "invert=%d\n", mapwin[i]->invert);
    
    fprintf(mfile, "racen=%.10f\n", mapwin[i]->racen);
    fprintf(mfile, "dlcen=%.10f\n", mapwin[i]->dlcen);
    fprintf(mfile, "scale=%.10f\n", mapwin[i]->scale);
    
    fprintf(mfile, "c_scale=%.10f\n", mapwin[i]->c_scale);
    
    fprintf(mfile, "nlayers=%d\n", mapwin[i]->nlayers);
    for (j = 0; j < mapwin[i]->nlayers; j++)
      fprintf(mfile, "layer[%d] = %d\n", j, mapwin[i]->layer[j]);  
    
    fprintf(mfile, "numfiles=%d\n", mapwin[i]->numfiles);
    for (j = 0; j < mapwin[i]->numfiles; j++) {
      fprintf(mfile, "file=%d\n", j);
      fprintf(mfile, "name=%s\n",mapwin[i]->file[j].name);
      fprintf(mfile, "type=%d\n", mapwin[i]->file[j].type);
      fprintf(mfile, "maglim=%.10f\n", mapwin[i]->file[j].maglim);
      fprintf(mfile, "lbllim=%.10f\n", mapwin[i]->file[j].lbllim);
      fprintf(mfile, "gklim=%.10f\n", mapwin[i]->file[j].gklim);
      fprintf(mfile, "draw_maglbl=%d\n", mapwin[i]->file[j].draw_maglbl);
      fprintf(mfile, "maglmin=%.10f\n", mapwin[i]->file[j].maglmin);
      fprintf(mfile, "maglmax=%.10f\n", mapwin[i]->file[j].maglmax);
    }
  }
  
  
  /* Tag file as being mapwin format */
  fprintf(mfile,
	  "#mapwin format 1 file: don't edit this line or change order of lines in this file\n");
}
