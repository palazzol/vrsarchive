/* Header for starchart 3.x */
/*
 * 
 * $Header: /home/Vince/cvs/net/starchart/starchart/star3.h,v 1.1 1990-03-30 16:39:02 vrs Exp $
 *
*/
/*
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


#define FALSE 0
#define TRUE 1

#ifdef MSDOS
#include "pcstar.h"
#endif

/* Line Styles */
/* SOLID is defined for HP's, maybe some others. */
#ifndef SOLID
#define SOLID 1
#define DOTTED 2
#define DASHED 3
#define VECSOLID 4
#define VECDOT 5
#define VECDASH 6
#define MAXSTLNS 7
#define CURNTSTY 100
#else
#define DOTTED 2
#define DASHED 3
#define VECSOLID 4
#define VECDOT 5
#define VECDASH 6
#define MAXSTLNS 7
#define CURNTSTY 100
#endif

/* Functions */
#define CHRTOUTLN 1
#define CHRTHTICK 2
#define CHRTVTICK 3
#define GRID_RA 4
#define GRID_DEC 5
#define ECLIPT 6
#define CONSTBOUND 7
#define CONSTPATTRN 8
#define CONSTNAME 9
#define CHARTFILE 10


/* Fonts */
#define CURNTFONT 100
#define TIMESROMAN 10
#define TIMESBOLD 11
#define TIMESITAL 12
#define TIMESBOLDITAL 13
#define HELV 20
#define HELVBOLD 21
#define HELVITAL 22
#define HELVBOLDITAL 23
#define COURIER 30
#define COURBOLD 31
#define COURITAL 32
#define COURITALBOLD 33

#define NAMESIZE 10

/* Chart types */
#define THREEPART 1
#define FULLPAGE 2
#define OTHERTYPE 3

/* Inverse Chart types */
static char *inv_ctypes[] = {
  "none",
  "threepart",
  "fullpage",
  "none"
  };


/* File types */
#define LINEREAD 1
#define INDEXTYPE 2
#define BINFULL 3
#define BINOBJ 4
#define BINSTAR 5
#define SAOFORMAT 6
#define GSCTYPE 7

/* inverse file types */
static char *inv_ftypes[] = {
  "none",
  "lineread",
  "indextype",
  "binfull",
  "binobj",
  "binstar",
  "saoformat",
  "gsc"
  };

/* Projection Modes */
#define SANSONS 1
#define STEREOGR 2
#define GNOMONIC 3
#define ORTHOGR 4
#define RECTANGULAR 5
#define OTHERPROJ 6

/* inverse projection mode */
static char *inv_projs[] = {
  "",
  "sansons",
  "stereogr",
  "gnomonic",
  "orthogr",
  "rectangular",
  "otherproj"
  };

/* map types */
#define MAINMAP 0
#define FULLPAGEMAP 1
#define THUMBNAIL 2

/* Layers */
#define OUTLINE 1
#define RA_GRID 2
#define DEC_GRID 3
#define ECLIPTIC 4
#define BOUNDARIES 5
#define PATTERNS 6
#define CONSTLNAMES 7
#define ALLFILES 8
#define ALLGLYPHS 9
#define ALLNAMES 10
#define ALLMAGLBLS 11
#define ALLVECTORS 12
#define ALLAREAS 13
#define LEGENDS	14
#define SKIP 0

/* inverse layers types */
static char *inv_layrs[] = {
  "skip",
  "outline",
  "ra_grid",
  "dec_grid",
  "ecliptic",
  "boundaries",
  "patterns",
  "constlnames",
  "allfiles",
  "allglyphs",
  "allnames",
  "allmaglbls",
  "allvectors",
  "allareas",
  "legends"
  };


/* Order of standard files in mapfiles */
#define YALEF 0
#define INDXF 1
#define NEBF 2
#define PLANF 3
#define NUMFS 4

#define MAXLAYRS 20

#define MAXUFILES 10	            /* Maximum number of user supplied files */
#define MAXMAPFILES (MAXUFILES+NUMFS) /* Maximum total number of files
					 user + standard */

typedef struct {
  char *name;			    /* file name */
  int type;			    /* file type */
  double maglim, lbllim, gklim;	    /* limiting mag for glyph, name,
				       lable */
  int draw_maglbl;		    /* Label with the magnitude */
  double maglmin, maglmax;	    /* Minimum and maximum mag to be labeled */
} datafile;

/* The variables in the first few lines MUST be set by driver */
typedef struct {
  int width, height, x_offset, y_offset; /* Size and position,
					    in integer device coords */
  double maglim, lbllim, gklim;	    /* default limiting mag for glyph, name,
				       and lable for files */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  int map_type;			    /* Type of map: THUMBNAIL may have
				       some restrictions */
  int tag;			    /* May be used by driver for whatever */
  char *tag_field;		    /* May be used by driver for whatever */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  int proj_mode;		    /* Projection mode for this map */
  int draw_ragrid, draw_decgrid;    /* Draw grids */
  double ra_step, dec_step;	    /* grid step size */
  double ra_strt, dec_strt;	    /* grid origin */

  int invert;			    /* Invert (flip north south) */

/* The following are set by the main routines */
  double racen, dlcen, scale;	    /* R.A. and decl. of center,
				       scale in degrees */
  double c_scale;		    /* One second of arc
				       in display units */
  int layer[MAXLAYRS];		    /* layer order */
  int nlayers;			    /* number of layers */
  datafile file[MAXMAPFILES];	    /* files in this map and their controls */
  int numfiles;			    /* number of files */
} mapwindow;
#define MAXWINDOWS 10


double modup(), moddown();
double ftod(), dtof(), htod();
long size_obj();

/* declared here for VMS and pure-ANSI systems (declared in stdlib.h there) */
double atof();


