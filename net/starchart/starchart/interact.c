/*
 * User Interaction subroutines
 * 
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/interact.c,v 1.1 1990-03-30 16:38:53 vrs Exp $";

#include <stdio.h>
#include <math.h>

#include "star3.h"
#include "parse_input.h"
#include "patchlevel.h"

#define OPENFAIL 0
#define LINELEN 82


/* Externs */
extern int g_argc;
extern char **g_argv;

extern char *title;

/* From starchart.c */
extern int user_interact;

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

/* local similar buffers */
char a_constname[MAXPATHLEN];
char a_rcfile[MAXPATHLEN];
char *a_mapfile[MAXMAPFILES];
char a_mapftypes[MAXMAPFILES][MAXPATHLEN];



/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail */
#define THSMUL 1.2
#define THSMIN 12.0
#define THMADJ 2.5
#define THMMAX 8.0

extern mapwindow fullpage, mainmap, thumbmap;


/* Adapted from code from Dean Payne deanp@hplsla.hp.com */
/* Format of each line is
	variable_name variable_value
or	variable_name=variable_value

variable_value may be a string which extends to the end of line.
Lines must be 80 chars max.
Comments are anything after a #
Blank lines are allowed.
*/
D_userinput()
{
  char sbuf[LINELEN], *var_name, *c_buf;
  double f_buf;	/* value parsed as double */
  int i_buf;	/* value parsed as integer */
  int l_buf;	/* value parsed as TRUE or FALSE */
  int ret_code, var_type;
  static int first_time = TRUE; 
  static int printed_notice = FALSE;

    if (!user_interact)
      if (first_time) {
	first_time = FALSE;
	return TRUE;
      } else return FALSE;	/* If we don't really want user interaction,
				   i.e. -u wasn't used,
				   only do the main loop once */

  if (!printed_notice) {
    printf("Starchart Version %s, patchlevel %s\n", VERSION_STRING,
	   PATCHLEVEL_STRING);
    printf(
    "This is a copyrighted program, however the source is freely available\n");
    printf("and freely redistributable for noncommercial use.\n");
    printf("Distributed 'as is', with no warranty.\n\n\n");
    printed_notice = TRUE;
  };


  printf("Please type your commands:\n");
  for (;;) {
    fgets(sbuf, LINELEN, stdin);
    if (ferror(stdin)) { fprintf(stderr, "file read error \n"); break; }
    if (feof(stdin)) return TRUE;


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
      case END_INPUT:
	return TRUE;
	break;
      case EXIT:
	exit(0);
	break;
      case HELP_ME:
	help_vars("");
	break;
      case SHOW_ME:
	rc_write(stderr);
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
  }
  
  
  return TRUE;
}

D_mapwininput()
{
  return TRUE;
}

D_break()
{
  return FALSE;
}

