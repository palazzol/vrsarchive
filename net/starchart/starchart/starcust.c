/*
 * Customizing functions
 */

/*
 * Produced for starchart 3.0 by Craig Counterman Jan, 1989
 *
 * Copyright (c) 1989 by Craig Counterman
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


static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starcust.c,v 1.1 1990-03-30 16:39:08 vrs Exp $";

#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif

#include "star3.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1025
#endif

/* Externs */
extern int g_argc;
extern char **g_argv;


/* title, mapwin, and numwins are the
   mostly device independent specifications for the plot */
extern char *title;	/* Title of page */

extern mapwindow *mapwin[MAXWINDOWS];
extern int numwins;

/*
mapwindow s_mapwin[MAXWINDOWS];
char s_filenames[MAXWINDOWS][MAXMAPFILES][MAXPATHLEN];
char s_tag_fields[MAXWINDOWS][81];
*/

extern int cur_function;
extern int cur_map_type;
extern int cur_map_tag;
extern char *cur_tag_field;

extern int read_mapwin_file;
extern int write_mapwin_file;
extern char mapwin_file[];

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
extern double th_smul;
extern double th_smin;
extern double th_madj;
extern double th_mmax;


#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

char *malloc();

/* Other functions */
ident_arg(argi)		/* argc and argv are already available as externs */ 
     int *argi;		/* The argument which can't be understood */
{
  /*printf("Identify '%s' '%s'\n", g_argv[*argi], g_argv[*argi+1]);*/

  return FALSE;
}
/* Return TRUE and increment argi if the argument has been interpreted.
   Return FALSE if the argument is an error */


ident_rc(var_name, var_val)
     char *var_name, *var_val;
{
  /*printf("Identify '%s' '%s'\n", var_name, var_val);*/

  return FALSE;
}
/* Return TRUE if the variable has been interpreted.
   Return FALSE if the variable is an error */


/* Do final adjustments before drawing begins */
fix_mapwin()
{
  int i, j;
  FILE *mfile;

  for (i = 0; i < numwins; i++) {
    if (mapwin[i]->map_type == THUMBNAIL) {
      /* No constellation names in thumbnail */
      for (j =0; j < mapwin[i]->nlayers; j++) {
	if (mapwin[i]->layer[j] == CONSTLNAMES)
	  mapwin[i]->layer[j] = SKIP;
	if (mapwin[i]->layer[j] == LEGENDS)
	  mapwin[i]->layer[j] = SKIP;
      }

      /* No grids, don't invert */
      mapwin[i]->draw_ragrid = FALSE;
      mapwin[i]->draw_decgrid = FALSE;
      mapwin[i]->invert = FALSE;

      if ((mapwin[i]->proj_mode == STEREOGR) ||
	  (mapwin[i]->proj_mode == GNOMONIC) ||
	  (mapwin[i]->proj_mode == ORTHOGR)) {
      /* If the parent map of this thumbnail map is stereographic,
	 gnomonic, or orthographic,
	 double the scale, for Sanson's projection */
	mapwin[i]->scale *= 2.0;

	if ((90.0 - fabs(mapwin[i]->dlcen)) < (mapwin[i]->scale/2.0)) {
	  /* pole is in map shouldn't draw this map */
	  if (i == (numwins-1)) numwins--;
	  else {
	    for (j = i; j < numwins; j++)
	      mapwin[j] = mapwin[i];
	    numwins--;
	  }
	}
      }



      /* Thumbnail must be Sanson's */
      mapwin[i]->proj_mode = SANSONS;


      /* Do scale, mag adjustments for thumbnail */
      mapwin[i]->scale = MAX(mapwin[i]->scale * th_smul, th_smin);

      for (j =0; j < mapwin[i]->numfiles; j++) {
	mapwin[i]->file[j].maglim =  MIN(th_mmax,
					 mapwin[i]->maglim - th_madj);
	mapwin[i]->file[j].lbllim = MIN(mapwin[i]->maglim,
					mapwin[i]->lbllim - th_madj);
	mapwin[i]->file[j].gklim = MIN(mapwin[i]->maglim,
				       mapwin[i]->gklim - th_madj);
	mapwin[i]->file[j].draw_maglbl = FALSE;
      }
    }
  }
  
  /* Read mapwin structures from mapwin file */
  if ((read_mapwin_file)  && ((mfile = fopen(mapwin_file, "r")) != NULL)) {
    read_mfile(mfile);
    fclose(mfile);
  }
}

char s_title[81];

read_mfile(mfile)
     FILE *mfile;
{
  int i, j;
  int n, o, p;
  char line[300];
  
  /* Check tag on file as being mapwin format */
  n = fscanf(mfile, "%[^\n]\n", line);
  if (!strcmp(line,
	      "#mapwin format 1 file: don't edit this line or change order of lines in this file\n"))
    return FALSE;

  /* set mapwin pointer to real storage
     set filename pointers and tag_field pointer to real storage */
/*
  for (i = 0; i < MAXWINDOWS; i++) {
    mapwin[i] = &s_mapwin[i];
    s_mapwin[i].tag_field = s_tag_fields[i];
    for (j = 0; j < MAXMAPFILES; j++)
      s_mapwin[i].file[j].name = s_filenames[i][j];
  };
*/

  /* read the title */
  fscanf(mfile, "title=%[^\n]\n", s_title);
  title = s_title;

  /* read number of mapwins */
  fscanf(mfile, "numwins=%d\n", &numwins);
  for (i = 0; i < numwins; i++) {
    fscanf(mfile, "mapwin=%d\n",&n);
    if (n != i) fprintf(stderr, "Error in mapwin file");
    n = i;

    /* Allocate space for this mapwin */
    mapwin[n] = (mapwindow *) malloc(sizeof(mapwindow));
    mapwin[n]->tag_field = (char *) malloc(MAXPATHLEN);
    
    fscanf(mfile, "width=%d\n", &(mapwin[n]->width));
    fscanf(mfile, "height=%d\n", &(mapwin[n]->height));
    fscanf(mfile, "x_offset=%d\n", &(mapwin[n]->x_offset));
    fscanf(mfile, "y_offset=%d\n", &(mapwin[n]->y_offset));
    
    fscanf(mfile, "maglim=%lf\n", &(mapwin[n]->maglim));
    fscanf(mfile, "lbllim=%lf\n", &(mapwin[n]->lbllim));
    fscanf(mfile, "gklim=%lf\n", &(mapwin[n]->gklim));
    
    
    fscanf(mfile, "map_type=%d\n", &(mapwin[n]->map_type));
    
    
    fscanf(mfile, "tag=%d\n", &(mapwin[n]->tag));
    
    fscanf(mfile, "tag_field=%s\n", mapwin[n]->tag_field);
    
    
    fscanf(mfile, "proj_mode=%d\n", &(mapwin[n]->proj_mode));
    
    
    fscanf(mfile, "draw_ragrid=%d\n", &(mapwin[n]->draw_ragrid));
    fscanf(mfile, "draw_decgrid=%d\n", &(mapwin[n]->draw_decgrid));
    
    fscanf(mfile, "ra_step=%lf\n", &(mapwin[n]->ra_step));
    fscanf(mfile, "dec_step=%lf\n", &(mapwin[n]->dec_step));
    
    fscanf(mfile, "ra_strt=%lf\n", &(mapwin[n]->ra_strt));
    fscanf(mfile, "dec_strt=%lf\n", &(mapwin[n]->dec_strt));
    
    
    fscanf(mfile, "invert=%d\n", &(mapwin[n]->invert));
    
    fscanf(mfile, "racen=%lf\n", &(mapwin[n]->racen));
    fscanf(mfile, "dlcen=%lf\n", &(mapwin[n]->dlcen));
    fscanf(mfile, "scale=%lf\n", &(mapwin[n]->scale));
    
    fscanf(mfile, "c_scale=%lf\n", &(mapwin[n]->c_scale));
    
    fscanf(mfile, "nlayers=%d\n", &(mapwin[n]->nlayers));
    for (j = 0; j < mapwin[n]->nlayers; j++) {
      fscanf(mfile, "layer[%d] = %d\n", &o, &p);
      mapwin[n]->layer[o] = p;
    }
    
    fscanf(mfile, "numfiles=%d\n", &(mapwin[n]->numfiles));
    for (j = 0; j < mapwin[n]->numfiles; j++) {
      fscanf(mfile, "file=%d\n", &o);
      if (o != j) fprintf(stderr, "Error in mapwin file");
      o = j;

      /* Allocate space for the file name */
      mapwin[n]->file[o].name = (char *) malloc(MAXPATHLEN);


      fscanf(mfile, "name=%s\n", (mapwin[n]->file[o].name));
      fscanf(mfile, "type=%d\n", &(mapwin[n]->file[o].type));
      fscanf(mfile, "maglim=%lf\n", &(mapwin[n]->file[o].maglim));
      fscanf(mfile, "lbllim=%lf\n", &(mapwin[n]->file[o].lbllim));
      fscanf(mfile, "gklim=%lf\n", &(mapwin[n]->file[o].gklim));
      fscanf(mfile, "draw_maglbl=%d\n", &(mapwin[n]->file[o].draw_maglbl));
      fscanf(mfile, "maglmin=%lf\n", &(mapwin[n]->file[o].maglmin));
      fscanf(mfile, "maglmax=%lf\n", &(mapwin[n]->file[o].maglmax));
    }
  }
  
  /* Check tag on file as being mapwin format */
  n = fscanf(mfile, "%[^\n]\n", line);
  if (!strcmp(line,
	      "#mapwin format 1 file: don't edit this line or change order of lines in this file\n"))
    return FALSE;
  
  return TRUE;
}

