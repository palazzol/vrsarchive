/*
 * PostScript file format driver for startchart.c mainline
 */

/*
 ! patched December, 1987 by Alan Paeth (awpaeth@watcgl),
 ! based on revisions by Craig Counterman (email: ccount@athena.mit.edu)
 !
 ! [1] "bigmaster" chart layout now added
 ! [2] extensive rework and subclassing of non-stellar objects, e.g.
 !     galaxies, now have both spiral and irregular variants.
 ! [3] star sizes now extended to magnitude 10
 ! [4] white halo-ing of text overlays (by analogy to star haloing)
 !
 * Modified for starchart 3.0.  Craig Counterman Jan, 1989
 *
 * size changed:
 *   8 x 11 inch field, in units of 1/ResPT th of a point
 *   offset 1/4 inch = 18*ResPT
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


static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starpost.c,v 1.1 1990-03-30 16:39:40 vrs Exp $";
#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>
#include "star3.h"


/* Externs */
extern int g_argc;
extern char **g_argv;

extern char *title;	/* Title of page */

extern mapwindow *mapwin[MAXWINDOWS];
extern int numwins;

extern int cur_function;
extern int cur_map_type;
extern int cur_map_tag;
extern char *cur_tag_field;
extern mapwindow *cur_win;

extern int chart_type;

/* Set by initxform
   One could use elements of the mapwindow structure,
   but these should be faster for the current window */
extern double xf_west, xf_east, xf_north, xf_south, xf_bottom;
extern int xf_xcen, xf_ycen, xf_ybot;
extern int xf_w_left, xf_w_right, xf_w_top, xf_w_bot;
extern double xf_c_scale;

/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail,*/
#define THSMUL 1.2
#define THSMIN 12.0
#define THMADJ 2.5
#define THMMAX 8.0


#define ResPT 4
#define InchPT 72*ResPT

#define MAG_1_RAD 9*ResPT
#define MAG0_RAD ((int)(8*ResPT))
#define MAG1_RAD ((int)(7*ResPT))
#define MAG2_RAD ((int)(6*ResPT))
/*  if MAG3 is not an integer times ResPT,
    there is an error reported later using some (non-ansi) compilers */
#define MAG3_RAD ((int)(5*ResPT))
#define MAG4_RAD ((int)(4*ResPT))
#define MAG5_RAD ((int)(3.25*ResPT))
#define MAG6_RAD ((int)(2.5*ResPT))
#define MAG7_RAD ((int)(2.0*ResPT))
#define MAG8_RAD ((int)(1.5*ResPT))
#define MAG9_RAD ((int)(1.0*ResPT))
#define MAG10_RAD ((int)(0.5*ResPT))
#define MAG11_RAD ((int)(0.5*ResPT))
#define MAG12_RAD ((int)(0.5*ResPT))
#define MAG13_RAD ((int)(0.5*ResPT))
#define MAG14_RAD ((int)(0.5*ResPT))
#define MAG15_RAD ((int)(0.5*ResPT))
#define MAG16_RAD ((int)(0.5*ResPT))
#define MAG17_RAD ((int)(0.5*ResPT))
#define MAG18_RAD ((int)(0.5*ResPT))
#define MAG19_RAD ((int)(0.5*ResPT))
#define MAG20_RAD ((int)(0.5*ResPT))

#define MAG_NUMMAGS 40
/* Large enough to hold to mag. 40 */

/* Many things break if MAG_BRIGHT != -1; sorry, Sirius */
#define MAG_BRIGHT -1
#define MAG_DIM 20

static int magsizes[MAG_NUMMAGS]; /* Set in D_control_arg */

/* mag10: 10 refers to 1/10th magnitude binning */
#define MAG_10_NUMMAGS MAG_NUMMAGS*10 + 1
static int mag10sizes[MAG_10_NUMMAGS];

/* For Young's formula */
static double young_k = 0.11;
static double young_D0 = 0.5 * ResPT;
static double young_v0 = 10.0;
static int use_v0 = FALSE;
static int use_young = FALSE;

/* Notebook mode controls */
/* number of eyepiece/finder views */
#define NOTE_NUMVIEWS 20
static double viewrad[NOTE_NUMVIEWS];
static int numviews;
static int notebook_mode = FALSE;

/* brightest and faintest magnitudes used in first map */
static double brightest_used = 100.0, faintest_used = -1.0;
/* used in legend */

/* shift thumbscale map magnitude */
static double thumbshift = 0.0;

#ifndef MAXPATHLEN
#define MAXPATHLEN 1025
#endif

static char filname[MAXPATHLEN] = "";
static FILE *set_out_file = NULL;
static FILE *outf;


#define S_UNDEF -1
#define S_MOVE 1
#define S_LINE 2
#define S_AREA 3

/* Status variables */
static int cur_colr = -1;
static int c_font = -1, c_size = -1;
static char *c_fname = "/Times-Roman";
static int vecstate = S_UNDEF;
static int cur_x = -1, cur_y = -1;
static int cur_sty = SOLID;
static char *c_sty_str = "st";


/* Exports */

/* 7 x 10 inch, half inch on bottom + 1/4 in.  offset,
				leave 3/8 inch on right with 1/8 on left */
/* The variables in the first few lines MUST be set by driver */
mapwindow fullpage = {
  7*72*ResPT, 720*ResPT, (9+18)*ResPT, (18+36)*ResPT,
		        /* width, height, x and y offsets */
  8.0, 3.0, 2.05,	/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  FULLPAGEMAP,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  7.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};

/* 7 x 7 inch, 2.5 inch on bottom  + 18 point offset */
/* The variables in the first few lines MUST be set by driver */
mapwindow mainmap = {
  7*72*ResPT, 7*72*ResPT, (9+18)*ResPT,(18+180+18)*ResPT,
                        /* width, height, x and y offsets */
  8.0, 3.0, 2.05,	/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  MAINMAP,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  7.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};


/* 3.5 x 2 inch, in bottom right */
/* The variables in the first few lines MUST be set by driver */
mapwindow thumbmap = {
  252*ResPT, 2*72*ResPT, (9+18+252)*ResPT, (18+18)*ResPT,
                        /* width, height, x and y offsets */
  6.2+THMADJ, 1.0+THMADJ, 2.05+THMADJ,
			/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  THUMBNAIL,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  7.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};

/* h & v tick text controls */
int htick_lim = 1*ResPT;
int htext_lim = 36*ResPT;
int htext_xoff = 4*ResPT;
int htext_yoff = 10*ResPT;
int vtick_lim = 1*ResPT;
int vtext_lim = 10*ResPT;
int vtext_xoff = 14*ResPT;
int vtext_yoff = 0*ResPT;

/* Scale multiplier, minimum,
   mangitude change, maximum, for thumbnail,*/
double th_smul=THSMUL;
double th_smin=THSMIN;
double th_madj=THMADJ;
double th_mmax=THMMAX;


#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))


static int use_fine_mag = FALSE;	/* Use 10ths of magnitude */
static int color_postscript = FALSE;
static int landscape = FALSE;
/* Device control argument */
D_control_arg(s)
     char *s;
{
  int i, j, k;
  int c;
  char *cp;
  static int mags_set = FALSE;

  /* set array of magnitudes */
  if (!mags_set) {
    magsizes[0] = MAG_1_RAD;
    magsizes[1] = MAG0_RAD;
    magsizes[2] = MAG1_RAD;
    magsizes[3] = MAG2_RAD;
    magsizes[4] = MAG3_RAD;
    magsizes[5] = MAG4_RAD;
    magsizes[6] = MAG5_RAD;
    magsizes[7] = MAG6_RAD;
    magsizes[8] = MAG7_RAD;
    magsizes[9] = MAG8_RAD;
    magsizes[10] = MAG9_RAD;
    magsizes[11] = MAG10_RAD;
    magsizes[12] = MAG11_RAD;
    magsizes[13] = MAG12_RAD;
    magsizes[14] = MAG13_RAD;
    magsizes[15] = MAG14_RAD;
    magsizes[16] = MAG15_RAD;
    magsizes[17] = MAG16_RAD;
    magsizes[18] = MAG17_RAD;
    magsizes[19] = MAG18_RAD;
    magsizes[20] = MAG19_RAD;
    magsizes[21] = MAG20_RAD;
    for (i = 22; i < MAG_NUMMAGS; i++)
      magsizes[i] = MAG10_RAD;
    mags_set = TRUE;
  };

  i = 0;
  while (c = s[i++]) switch (c) {
  case 'm':
    /* m [mag-1 : mag0 : mag1 : mag2 : ...] */
    use_fine_mag = TRUE;
    if (s[i]) {
      cp = &s[i];
      /* Skip leading white space */
      while ((*cp == ' ') || (*cp == '\t')) {
	i++;
	cp++;
      }
      j = 0;
      while (*cp) {
	if (j < MAG_NUMMAGS) magsizes[j++] = atof(cp) * ResPT;
	while (isdigit(*cp) || (*cp == '.')) {
	  i++;
	  cp++;
	}
	if (*cp) cp++;
      }
      for (k = j;k < MAG_NUMMAGS; k++)
	magsizes[k] = magsizes[j];
    }
    break;
  case 'f':
    /* f [file] */
    if (s[i++] == ' ')
      if (s[i])
	strcpy(filname, &s[i]);
    while ((s[i]) && (s[i] != ' ')) i++;
    break;
  case 's':
    /* s [D0 [v0 [exp]]] */
    use_young = TRUE;
    use_fine_mag = TRUE;
    if (s[i]) {
      cp = &s[i];
      /* Skip leading white space */
      while ((*cp == ' ') || (*cp == '\t')) {
	i++;
	cp++;
      }
      j = 0;
      if (*cp) {
	if (isdigit(*cp)) {
	  young_D0 = atof(cp) * ResPT;
	  while (isdigit(*cp) || (*cp == '.')) {
	    i++;
	    cp++;
	  }
	  if (*cp) cp++;
	}
	if (*cp) {
	  if (isdigit(*cp)) {
	    young_v0 = atof(cp);
	    use_v0 = TRUE;
	    while (isdigit(*cp) || (*cp == '.')) {
	      i++;
	      cp++;
	    }
	    if (*cp) cp++;
	  }
	  if (*cp)
	    if (isdigit(*cp)) {
	      young_k = atof(cp);
	      while (isdigit(*cp) || (*cp == '.')) {
		i++;
		cp++;
	      }
	      if (*cp) cp++;
	    }
	}
      }
    }
    break;
  case 'c':
    /* c */
    color_postscript = TRUE;
    break;
  case 'l':
    /* l */
    landscape = TRUE;
    chart_type = FULLPAGE;
    break;
  case 'n':
    /* n [rad1 : rad2 : rad3 : ... ] */
    notebook_mode = TRUE;
    chart_type = THREEPART;
    j = 0;
    if (s[i]) {
      cp = &s[i];
      /* Skip leading white space */
      while ((*cp == ' ') || (*cp == '\t')) {
	i++;
	cp++;
      }
      while ((*cp) && (isdigit(*cp) || (*cp == '.'))) {
	if (j < NOTE_NUMVIEWS) viewrad[j++] = atof(cp)/2.0;
				/* It's input as diameter */
	while (isdigit(*cp) || (*cp == '.')) {
	  i++;
	  cp++;
	}
	if (*cp) cp++;
      }
    }
    numviews = j;
  default:
    break;
  }
}


/* Open the device */
D_open()
{
  int i, j;
  double young_size;

  if (filname[0] != '\0') {
    set_out_file = fopen(filname, "w");
  };

  if (set_out_file != NULL)
    outf = set_out_file;
  else
    outf = stdout;	/* WARNING: This might not work on some systems */

  /* Need to initialize all variables */
  cur_colr = -1;
  c_font = -1;
  c_size = -1;
  c_fname = "/Times-Roman";
  vecstate = S_UNDEF;
  cur_x = -1;
  cur_y = -1;
  cur_sty = SOLID;
  c_sty_str = "st";


out("%!PSAdobe-1.0");
out("%%Creator: AWPaeth@watCGL && Craig Counterman");
out("%%Title: StarChart");
out("%%Pages: 1");
out("%%DocumentFonts Times-Roman Times-Bold Times-Italic Helvetica Symbol");
out("%%BoundingBox 0 0 612 828");
out("%%EndComments");
out("%%EndProlog");
out("%%Page: 0 1");
out("%");
out("% alias moveto/drawto");
out("%");
out("/mt {moveto} def");				/* move to */
out("/lt {lineto} def");				/* line to */
out("% st, dt and ht are   x y st --");
out("/st {[] 0 setdash lineto stroke newpath} def");	/* solid to */
fprintf(outf,
	"/dt {[%d] 0 setdash lineto stroke newpath [] 0 setdash} def\n",
	1*ResPT);
				                        /* dotted to */
fprintf(outf,
	"/ht {[%d] 0 setdash lineto stroke newpath [] 0 setdash} def\n",
	3*ResPT);
							/* hyphen to */
out("%");
out("% text operations");
out("%");
/* doesn't work, since currentfont is already scaled
  fprintf(outf, "/cfsize {currentfont exch %d mul scalefont setfont} def\n", ResPT);*/
out("% key fontsz --");
fprintf(outf, "/fontsz {findfont exch %d mul scalefont setfont} def\n", ResPT);
out("% Show with extra spaces between words: string lbshw --");
out("/lbshw {5 0 8#040 4 3 roll widthshow} def");
out("% stroke white around filled letters: string obshw --");
out("/obshw {gsave dup false charpath 1 setgray 5 setlinewidth stroke");
out("            grestore true charpath fill} def");

/* Not used
out("/cshow {dup stringwidth pop 2 div neg 0 rmoveto show} def");
out("/rshow {dup stringwidth pop neg 0 rmoveto show} def");
*/
out("% use symbol font for star label: string gshow --");
out("/gshow {currentfont exch");
fprintf(outf, "        /Symbol findfont %d scalefont setfont obshw setfont} def\n",
       10*ResPT);

out("%");
out("% star/planet macros");
out("%");
out("% move relative to x1 x2 and draw to x2 y2 : x1 y1 x2 y2 movereldraw --");
out("/movereldraw {newpath 4 2 roll mt rlineto [] 0 setdash stroke} def");
out("% draw dash from -dx to +dx at x y:  x y dx starminus --");
out("/starminus {3 copy 0 movereldraw neg 0 movereldraw} def");
out("% draw plus sign size dx at x y:  x y dx starplus --");
out("/starplus {3 copy 0 movereldraw 3 copy neg 0 movereldraw");
out( "          3 copy 0 exch movereldraw 0 exch neg movereldraw} def");
out("% draw circle diameter r at x y: x y r starcirc --");
out("/starcirc {newpath 0 360 arc closepath stroke} def");
out("% draw filled circle diameter r at x y: x y r starbody --");
out("/starbody {newpath 0 360 arc closepath fill} def");
/*fprintf(outf, "/starbodyvar {3 copy %d add starcirc starbody} def\n",
                                                          (int) (.25 * ResPT));*/
out("% draw circle diameter r at x y: x y r starbodyvar --");
out("/starbodyvar {starcirc} def\n");
out("% draw dash and filled circle diameter r at x y: x y r starbodydbl --");
out("/starbodydbl {3 copy dup 3 div add starminus starbody} def");
out("%");
out("% make r larger and set white: x y r back x y r x y (r+dr)");
fprintf(outf, "/back {3 copy %d add 1 setgray} def\n", (int) (.5 * ResPT));
  if (color_postscript) {
    out("% set r g and b, and set rgbcolor");
    out("/srgb {/r exch def /g exch def /b exch def r g b setrgbcolor} def\n");
  };
  out("% set foreground drawing color");
  if (color_postscript)
    out("/fore {r g b setrgbcolor} def");
  else
    out("/fore {0 setgray} def");

out("% normal star: x y r s --");
out("/s {back starbody fore starbody} def");
out("% double star: x y r s --");
out("/d {back starbodydbl fore starbodydbl} def");
out("% variable star: x y r s --");
out("/v {back starbodyvar fore starbodyvar} def");

/* define integral magnitudes */
fprintf(outf, "/s_1 {%d s} def\n", MAG_1_RAD);
fprintf(outf, "/d_1 {%d d} def\n", MAG_1_RAD);
fprintf(outf, "/v_1 {%d v} def\n", MAG_1_RAD);
fprintf(outf, "/s0 {%d s} def\n", MAG0_RAD);
fprintf(outf, "/d0 {%d d} def\n", MAG0_RAD);
fprintf(outf, "/v0 {%d v} def\n", MAG0_RAD);
fprintf(outf, "/s1 {%d s} def\n", MAG1_RAD);
fprintf(outf, "/d1 {%d d} def\n", MAG1_RAD);
fprintf(outf, "/v1 {%d v} def\n", MAG1_RAD);
fprintf(outf, "/s2 {%d s} def\n", MAG2_RAD);
fprintf(outf, "/d2 {%d d} def\n", MAG2_RAD);
fprintf(outf, "/v2 {%d v} def\n", MAG2_RAD);
fprintf(outf, "/s3 {%d s} def\n", MAG3_RAD);
fprintf(outf, "/d3 {%d d} def\n", MAG3_RAD);
fprintf(outf, "/v3 {%d v} def\n", MAG3_RAD);
fprintf(outf, "/s4 {%d s} def\n", MAG4_RAD);
fprintf(outf, "/d4 {%d d} def\n", MAG4_RAD);
fprintf(outf, "/v4 {%d v} def\n", MAG4_RAD);
fprintf(outf, "/s5 {%d s} def\n", MAG5_RAD);
fprintf(outf, "/d5 {%d d} def\n", MAG5_RAD);
fprintf(outf, "/v5 {%d v} def\n", MAG5_RAD);
fprintf(outf, "/s6 {%d s} def\n", MAG6_RAD);
fprintf(outf, "/d6 {%d d} def\n", MAG6_RAD);
fprintf(outf, "/v6 {%d v} def\n", MAG6_RAD);
fprintf(outf, "/s7 {%d s} def\n", MAG7_RAD);
fprintf(outf, "/d7 {%d d} def\n", MAG7_RAD);
fprintf(outf, "/v7 {%d v} def\n", MAG7_RAD);
fprintf(outf, "/s8 {%d s} def\n", MAG8_RAD);
fprintf(outf, "/d8 {%d d} def\n", MAG8_RAD);
fprintf(outf, "/v8 {%d v} def\n", MAG8_RAD);
fprintf(outf, "/s9 {%d s} def\n", MAG9_RAD);
fprintf(outf, "/d9 {%d d} def\n", MAG9_RAD);
fprintf(outf, "/v9 {%d v} def\n", MAG9_RAD);
fprintf(outf, "/s10 {%d s} def\n", MAG10_RAD);
fprintf(outf, "/d10 {%d d} def\n", MAG10_RAD);
fprintf(outf, "/v10 {%d v} def\n", MAG10_RAD);
fprintf(outf, "/s11 {%d s} def\n", MAG11_RAD);
fprintf(outf, "/d11 {%d d} def\n", MAG11_RAD);
fprintf(outf, "/v11 {%d v} def\n", MAG11_RAD);
fprintf(outf, "/s12 {%d s} def\n", MAG12_RAD);
fprintf(outf, "/d12 {%d d} def\n", MAG12_RAD);
fprintf(outf, "/v12 {%d v} def\n", MAG12_RAD);
fprintf(outf, "/s13 {%d s} def\n", MAG13_RAD);
fprintf(outf, "/d13 {%d d} def\n", MAG13_RAD);
fprintf(outf, "/v13 {%d v} def\n", MAG13_RAD);
fprintf(outf, "/s14 {%d s} def\n", MAG14_RAD);
fprintf(outf, "/d14 {%d d} def\n", MAG14_RAD);
fprintf(outf, "/v14 {%d v} def\n", MAG14_RAD);
fprintf(outf, "/s15 {%d s} def\n", MAG15_RAD);
fprintf(outf, "/d15 {%d d} def\n", MAG15_RAD);
fprintf(outf, "/v15 {%d v} def\n", MAG15_RAD);
fprintf(outf, "/s16 {%d s} def\n", MAG16_RAD);
fprintf(outf, "/d16 {%d d} def\n", MAG16_RAD);
fprintf(outf, "/v16 {%d v} def\n", MAG16_RAD);
fprintf(outf, "/s17 {%d s} def\n", MAG17_RAD);
fprintf(outf, "/d17 {%d d} def\n", MAG17_RAD);
fprintf(outf, "/v17 {%d v} def\n", MAG17_RAD);
fprintf(outf, "/s18 {%d s} def\n", MAG18_RAD);
fprintf(outf, "/d18 {%d d} def\n", MAG18_RAD);
fprintf(outf, "/v18 {%d v} def\n", MAG18_RAD);
fprintf(outf, "/s19 {%d s} def\n", MAG19_RAD);
fprintf(outf, "/d19 {%d d} def\n", MAG19_RAD);
fprintf(outf, "/v19 {%d v} def\n", MAG19_RAD);
fprintf(outf, "/s20 {%d s} def\n", MAG20_RAD);
fprintf(outf, "/d20 {%d d} def\n", MAG20_RAD);
fprintf(outf, "/v20 {%d v} def\n", MAG20_RAD);

  /* Thumbshift */
  if ((numwins == 2) && (mapwin[1]->map_type == THUMBNAIL)) {
    thumbshift = mapwin[0]->maglim - MIN(th_mmax,
					 mapwin[1]->maglim - th_madj);
  }


  if (use_fine_mag) {
    /* use fine (continuous) magnitude scale */

    if (use_young) {
      /* set magsizes according to Young's formula,
	 Sky and Telescope, March 1990, p. 311 */
      if (!use_v0) young_v0 = mapwin[0]->maglim;
      for (i = 0; i < MAG_10_NUMMAGS; i++) {
	/* magsizes[i] is for magnitude (i - 1) */
	/* brightest mag10sizes is for mag -1.0 */
	young_size = young_D0 * exp(log(10.0)*young_k*(young_v0 - (i/10.0-1)));
	if (young_size <  1) young_size = 1;
	mag10sizes[i] = (int) (young_size + 0.5);
      }
    } else {
      for (i = 0; i < (MAG_NUMMAGS-1); i++)
	for (j = 0; j < 10; j++)
	  mag10sizes[i*10 + j] =
	    magsizes[i] - j*(magsizes[i] - magsizes[i+1])/10.0;
      mag10sizes[(MAG_NUMMAGS-1)*10] = magsizes[MAG_NUMMAGS-1];
    }

#ifdef USE_FINE_MACROS
    /* set macros: Only if you have lots of printer memory */
    k = 0;
    for (i = MAG_BRIGHT; i < MAG_DIM; i++)
      for (j = 0; j < 10; j++) {
	if ((i*10 + j) < 0) {
	  fprintf(outf, "/ss_%02d {%d s} def\n", - (i*10 + j), mag10sizes[k]);
	  fprintf(outf, "/sd_%02d {%d d} def\n", - (i*10 + j), mag10sizes[k]);
	  fprintf(outf, "/sv_%02d {%d v} def\n", - (i*10 + j), mag10sizes[k]);
	} else {
	  fprintf(outf, "/ss%02d {%d s} def\n", i*10 + j, mag10sizes[k]);
	  fprintf(outf, "/sd%02d {%d v} def\n", i*10 + j, mag10sizes[k]);
	  fprintf(outf, "/sv%02d {%d d} def\n", i*10 + j, mag10sizes[k]);
	}
	k++;
      }
    i = MAG_DIM;
    j = 0;
    if ((i*10 + j) < 0) {
      fprintf(outf, "/ss_%02d {%d s} def\n", - (i*10 + j), mag10sizes[k]);
      fprintf(outf, "/sd_%02d {%d d} def\n", - (i*10 + j), mag10sizes[k]);
      fprintf(outf, "/sv_%02d {%d v} def\n", - (i*10 + j), mag10sizes[k]);
    } else {
      fprintf(outf, "/ss%02d {%d s} def\n", i*10 + j, mag10sizes[k]);
      fprintf(outf, "/sd%02d {%d v} def\n", i*10 + j, mag10sizes[k]);
      fprintf(outf, "/sv%02d {%d d} def\n", i*10 + j, mag10sizes[k]);
    }
#endif

  }


out("%");
out("% non-stellar object macros (better designs most welcome)");
/* thanks to phk@kpd.dk */
out("/stdsh0 { [] 0 setdash } def");
  /* set dash zero: solid line */

  /* macros are all x y size macro - */
  /* The first three lines, and the last grestore of most of the macros
     set things up so the middle lines need only worry about a
     region of +/- 1 square (or +/- 2, for some) */
out("% all but comet and moon take x y diam,");
out("%    comet takes x y diam position-angle,");
out("%    and moon takes x y diam phase pa-of-bright-limb");
out("%    all leave nothing on the stack");
out("/planet  { 2.4 div dup /sze exch def neg /nsze exch def");
out("    2 copy stdsh0 sze starcirc sze 1.2 mul starplus} def");
out("/mercury { 4 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 0 2 0 360 arc stroke");
out("   newpath 0 1 .4 180 0 arc .5 .1 moveto");
out("   0 .1 .5 0 360 arc 0 -.4 moveto");
out("   0 -1 lineto -.3 -.7 moveto .3 -.7 lineto stroke");
out("   grestore} def");
out("/venus { 4 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 0 2 0 360 arc stroke");
out("   newpath 0 .4 .5 0 360 arc 0 .1 moveto 0 -.9 lineto -.4 -.55 moveto");
out("   .4 -.55 lineto stroke grestore } def");
out("/mars { 4 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 0 2 0 360 arc stroke");
out("   newpath -.2 -.2 .6 0 405 arc");
out("   .7 .7 lineto 0 -.5 rmoveto 0 .5 rlineto");
out("   -.5 0 rlineto stroke grestore} def");
out("/jupiter { 4 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 0 2 0 360 arc stroke");
out("   newpath .5 -.9 moveto .5 .9 lineto .9 -.5 moveto -.7 -.5 lineto");
out("   .2 -.5 .2 .4 .8 arcto pop pop pop pop .2 .4 lineto");
out("   .1 .9 -.4 .6 .4 arcto pop pop pop pop -.4 .6 lineto");
out("   -.8 .4 -.5 .2 .3 arcto pop pop pop pop");
out("   stroke grestore} def");
out("/saturn { 4 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 0 2 0 360 arc stroke");
out("   newpath -.5 -.7 moveto -.5 1 lineto -.8 1 moveto -.2 1 lineto");
out("   -.5 0 moveto -.3 .3 0 .3 .6 arcto pop pop pop pop 0 .3 lineto");
out("   .5 .3 .4 -.1 .4 arcto pop pop pop pop .4 -.1 lineto .3 -.6 lineto");
out("   .3 -1.5 .7 -.8 .2 arcto pop pop pop pop .7 -.8 lineto");
out("   stroke grestore} def");
out("/uranus { 4 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 0 2 0 360 arc stroke");
out("   newpath -1 1 moveto -1 -1 lineto  1 1 moveto 1 -1 lineto stroke");
out("   -1 0 moveto 1 0 lineto stroke");
out("   0 1 moveto 0 -1 lineto stroke");
out("   0 -1.3 .3 0 360 arc stroke");
out("   grestore } def");
out("/neptune { 4 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 0 2 0 360 arc stroke");
out("   newpath 0 -.9 moveto");
out("   0 .8 lineto -.8 .8 moveto -.7 .5 lineto -.7 0 lineto .7 0 lineto");
out("   .7 .5 lineto .8 .8 lineto -.5 -.5 moveto");
out("   .5 -.5 lineto stroke grestore} def");
out("/pluto { 4 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 0 2 0 360 arc stroke");
out("   newpath -1 1 moveto -1 -1 lineto .5 -1 lineto stroke");
out("   -1 1 moveto 0 1 lineto -1 0 moveto 0 0 lineto stroke");
out("   0 .5 .5 270 90 arc stroke");
out("   grestore} def");
out("/sun { 2 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("    newpath 0 0 1 0 360 arc stroke");
out("    0 0 .2 0 360 arc fill grestore } def");
/* Moon takes x y size phase: x y size phase moon - */
out("/moon { /chi exch def /phase exch def 2 div /sze exch def moveto");
out("   gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath [.1 .2] 0 setdash 0 0 1 0 360 arc stroke");
out("   stdsh0 gsave");
out("      chi rotate");
out("      newpath\n");
out("      0 0 1 0 180 arcn");
out("      1 phase cos scale");
out("      0 0 1 180 360 arc fill");
out("   grestore grestore } def");
out("/asteroid { 4 div dup /sze exch def neg /nsze exch def");
out("    2 copy stdsh0 newpath sze 3 div 0 360 arc closepath fill");
out("    moveto sze sze rmoveto nsze 2 mul nsze 2 mul rlineto");
out("    sze 2 mul 0 rmoveto nsze 2 mul sze 2 mul rlineto stroke } def");
/* Comet takes x y size position-angle: x y size position-angle comet - */
out("/comet { 2 div dup /sze exch def neg /nsze exch def /phase exch def");
out("    moveto gsave currentpoint translate sze dup scale");
out("    .1 setlinewidth");
out("    90 phase sub rotate stdsh0 newpath 0 0 .3 0 360 arc stroke");
out("    0 .3 moveto 1 .5 lineto 0 -.3 moveto 1 -.5 lineto");
out("    .3 0 moveto 1.2 0 lineto");
out("    stroke grestore } def");
out("/nebulad { 2 div dup /sze exch def neg /nsze exch def stdsh0");
out("    newpath moveto sze 0 rmoveto nsze nsze rlineto nsze sze rlineto");
out("    sze sze rlineto sze nsze rlineto stroke } def");
out("/nebulap { 4 div /sze exch def moveto");
out("    gsave currentpoint translate");
out("    sze dup scale .1 setlinewidth");
out("    newpath 0 0 1 0 360 arc stroke");
out("    newpath 2 0 moveto 1 0 lineto -1 0 moveto -2 0 lineto");
out("    0 2 moveto 0 1 lineto 0 -1 moveto 0 -2 lineto stroke");
out("    grestore} def");
out("/nebula {nebulad} def");
out("/galaxye { 2 div dup /sze exch def neg /nsze exch def");
out("    gsave stdsh0 newpath translate -30 rotate 1 .5 scale");
out("    0 0 sze 0 360 arc closepath fill");
out("    grestore } def");
out("/galaxys { 2 div dup /sze exch def neg /nsze exch def");
out("    gsave stdsh0 newpath translate -30 rotate 1 .5 scale");
out("    0 0 sze 0 360 arc closepath fill");
out("    nsze 1.25 mul nsze moveto nsze 0 lineto sze 1.25 mul sze moveto");
out("    sze 0 lineto stroke  grestore } def");
out("/galaxyq { 2 div dup /sze exch def neg /nsze exch def");
out("    gsave stdsh0 newpath translate");
out("    nsze 0 moveto sze 0 lineto 0 nsze moveto 0 sze lineto stroke");
out("     -30 rotate 1 .5 scale 0 0 sze 0 360 arc closepath fill");
out("    grestore } def");
out("/galaxy {galaxye} def");
out("/clustero { 2 div dup /sze exch def neg /nsze exch def");
out("    2 copy newpath 1 setgray sze 0 360 arc fill");
fprintf(outf,
	"    [%d %d] 0 setdash 0 setgray sze 0 360 arc stroke stdsh0 } def\n",
       1*ResPT, 2*ResPT);
out("/clusterg { 2 div dup /sze exch def neg /nsze exch def");
out("    2 copy 2 copy newpath 1 setgray sze 0 360 arc fill");
fprintf(outf, "    [%d %d] 0 setdash 0 setgray sze 0 360 arc stroke\n",
       1*ResPT, 2*ResPT);
out("    3 0 360 arc closepath fill stdsh0 } def");
out("/cluster {clustero} def");
out("/unknown_u { 2 div /sze exch def moveto gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   newpath 0 .5 .5 180 270 arcn stroke");
out("   0 0 moveto 0 -1 lineto stroke");
out("   grestore} def");
out("/other_o { 2 div /sze exch def moveto gsave currentpoint translate");
out("   sze dup scale .1 setlinewidth");
out("   1 1 moveto -1 -1 lineto  -1 1 moveto 1 -1 lineto stroke");
out("   0 1 moveto 1 0 lineto  0 -1 lineto -1 0 lineto 0 1 lineto stroke");
out("   grestore} def");
out("%");
out("%");

  if (notebook_mode) {
    out("%notebook mode");
    out("24 0 translate");
    numwins = 1;
  };

  if (landscape) {
    out("%landscape mode");
    out("612 0 translate");
    out("90 rotate");
    i = fullpage.width;
    fullpage.width = fullpage.height;
    fullpage.height = i;
/*    i = fullpage.x_offset;
    fullpage.x_offset = fullpage.y_offset;
    fullpage.y_offset = i;*/
  }
fprintf(outf, "1 %d div dup scale\n", ResPT);
out("% alter line drawing defaults, guarentee solid black lines");
out("1 setlinewidth 2 setlinecap");
out("[] 0 setdash 0 setgray");
out("%");
out("% boiler-plate");
out("%");
out("% Set a font");
D_fontsize(18, TIMESROMAN);
/*out("18 /Times-Roman fontsz");*/
out("%");
out("%");
out("% THE STUFF");
out("%");
return TRUE ;				/* open successful */
}

out(s)
     char *s;
{
  fprintf(outf, "%s\n", s);
}

/* Close the device */
D_close()
{
  out("showpage");
  out("%");
  out("%%Trailer");
  out("%%Pages: 1");
  fflush(outf);
  fclose(outf);
}



/*
int vecstate = S_UNDEF;
int cur_x = -1, cur_y = -1;
int cur_sty = SOLID;
char *c_sty_str = "st";
*/
/* In postscript, we want to move to the beginning of a line with moveto (mt),
        then move from point to point with lineto (lt),
	then end the line when either the style changes
		or we're moving to a new start point [This is not really
			necessary, but if too many lt's are done, it probably
			will overload the printer]
	ending the line consists of calling (st),(dt),or (ht)
		depending on the current style */

/* Move to (x, y) */
D_move(x, y)
     int x, y;
{
/*  fprintf(outf, "%%move %d %d\n", x, y);*/

  if (vecstate != S_LINE) {	/* not drawing line now */
    if ((x != cur_x) || (y != cur_y)) { /* otherwise nothing needed */
      fprintf(outf, "%d %d mt\n", x, y);
    }
  } else {			/* drawing line, finish it then move */
    fprintf(outf, "%d %d %s\n", cur_x, cur_y, c_sty_str);
    fprintf(outf, "%d %d mt\n", x, y);
  }
  cur_x = x;
  cur_y = y;
  vecstate = S_MOVE;
}


/* Draw a line of style line_style from the current point to (x, y) */
/* Note, this replaces vecdraw vecdrawdot and vecdrawhyph */
D_draw(x, y, line_style)
     int x, y;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
  int sty;

/*  fprintf(outf, "%%draw %d %d %d\n", x, y, line_style);*/

  if ((x != cur_x) || (y != cur_y) || (line_style != cur_sty)) {
    switch(cur_function) {
    case CHRTOUTLN:
    case CHRTHTICK:
    case CHRTVTICK:
      sty = SOLID;
      break;
    case GRID_RA:
    case GRID_DEC:
      sty = DOTTED;
      break;
    case ECLIPT:
      sty = DOTTED;
      break;
    case CONSTBOUND:
      sty = DASHED;
      break;
    case CONSTPATTRN:
      sty = SOLID;
      break;
    case CONSTNAME:
    case CHARTFILE:
    default:
      sty = line_style;
      break;
    }

    if (vecstate == S_UNDEF) {	/* currentpoint not defined.
				   move to start point */
      fprintf(outf, "%d %d mt\n", cur_x, cur_y);
      vecstate = S_MOVE;
    }


    if (sty == cur_sty) {	/* Continue current line, style */
      fprintf(outf, "%d %d lt\n", x, y);
      vecstate = S_LINE;
    } else {			/* Change style */
				/* Finish current line */
      fprintf(outf, "%d %d %s\n", cur_x, cur_y, c_sty_str);
				/* Move to last point */
      fprintf(outf, "%d %d mt\n", cur_x, cur_y);

				/* Switch styles */
      switch(sty) {
      case SOLID:
	c_sty_str = "st";
	break;
      case DOTTED:
	c_sty_str = "dt";
	break;
      case DASHED:
	c_sty_str = "ht";
	break;
      case VECSOLID:
	c_sty_str = "st";
	break;
      case VECDOT:
	c_sty_str = "dt";
	break;
      case VECDASH:
	c_sty_str = "ht";
	break;
      default:
	c_sty_str = "st";
	break;
      }
      cur_sty = sty;
      fprintf(outf, "%d %d lt\n", x, y);
      vecstate = S_LINE;
    }
    cur_x = x;
    cur_y = y;
  }
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the line style drawn as desired */


/* Move to (x1, y1) then draw a line of style line_style to (x2, y2) */
D_movedraw(x1, y1, x2, y2, line_style)
     int x1, y1, x2, y2;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
  D_move(x1, y1);
  D_draw(x2, y2, line_style);
}


static struct {int x,y;} areapts[1000];
static int nareapts;
/* Move to (x, y) to begin an area */
D_areamove(x, y)
     int x, y;
{
  nareapts = 0;
  areapts[nareapts].x = x;
  areapts[nareapts].y = y;
  nareapts++;
}

/* Add a segment to the area border */
D_areaadd(x, y)
     int x, y;
{
  areapts[nareapts].x = x;
  areapts[nareapts].y = y;
  nareapts++;
}

/* Fill the area, after adding the last segment */
D_areafill(x, y)
     int x, y;
{
  int i;

  areapts[nareapts].x = x;
  areapts[nareapts].y = y;
  nareapts++;


  D_move(areapts[0].x, areapts[0].y);

  for (i = 1; i < nareapts; i++)
    fprintf(outf, "%d %d lt\n", areapts[i].x, areapts[i].y);
  fprintf(outf, "fill newpath\n");

  vecstate = S_UNDEF;
}




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

/* Set the color to be used for lines and text */
/* color_str is a 2 char (+ '\0') string containing
   a specification for a color,
   e.g. "G2" for the color of a star of spectral class G2, or "r7" for
   red, level seven.  The interpretation of the color string is left to
   the device driver */
/* static int cur_colr = -1;
*/
D_color(color_str)
     char *color_str;
{
  int colr;
  int class, subclass;
  static char *table = "OBAFGKMRNSrgbycpow";
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
    /* y */ { 37, 37, 37, 38, 38, 38, 39, 39, 39, 40 },
    /* g */ { 41, 41, 41, 42, 42, 42, 43, 43, 43, 44 },
    /* c */ { 45, 45, 45, 46, 46, 46, 47, 47, 47, 48 },
    /* b */ { 49, 49, 49, 50, 50, 50, 51, 51, 51, 52 },
    /* p */ { 53, 53, 53, 54, 54, 54, 55, 55, 55, 56 },
    /* o */ { 57, 57, 57, 58, 58, 58, 59, 59, 59, 60 },
    /* w */ { 61, 61, 61, 62, 62, 62, 63, 63, 63, 64 }
	  };

  static struct {double r, g, b;} ctab[] = {
/*  0 */    { 1.0, 1.0, 1.0 },   /* BLACK */
/*  1 */    {   0,   0,   0 },   /* WHITE */
/*  2 */    { 1.0,   0,   0 },   /* RED */
/*  3 */    { 1.0, 0.4,   0 },   /* ORANGE */
/*  4 */    { 1.0, 1.0,   0 },   /* YELLOW */
/*  5 */    {   0, 1.0,   0 },   /* GREEN */
/*  6 */    {   0, 1.0, 1.0 },   /* CYAN */
/*  7 */    {   0,   0, 1.0 },   /* BLUE */
/*  8 */    { 1.0,   0, 1.0 },   /* VIOLET */
/*  9 */    { .5, .5, 1.0 },   /* BLUE_WHITE */
    /*         Red             Green           Blue          Nameby which
	                                                     type is known */
/* 10 */    { 0.38937,        0.46526,        0.79493 },    /* B0 */
/* 11 */    { 0.39501,        0.47146,        0.78847 },    /* B1 */
/* 12 */    { 0.40103,        0.47792,        0.78151 },    /* B2 */
/* 13 */    { 0.40640,        0.48355,        0.77526 },    /* B3 */
/* 14 */    { 0.41341,        0.49071,        0.76701 },    /* B5 */
/* 15 */    { 0.43251,        0.50914,        0.74412 },    /* B8 */
/* 16 */    { 0.44342,        0.51897,        0.73079 },    /* B9 */
/* 17 */    { 0.45181,        0.52618,        0.72042 },    /* A0 */
/* 18 */    { 0.46931,        0.54026,        0.69847 },    /* A2 */
/* 19 */    { 0.47958,        0.54792,        0.68541 },    /* A3 */
/* 20 */    { 0.48538,        0.55205,        0.67797 },    /* A5 */
/* 21 */    { 0.50879,        0.56731,        0.64752 },    /* F0 */
/* 22 */    { 0.51732,        0.57231,        0.63627 },    /* F2 */
/* 23 */    { 0.52348,        0.57573,        0.62810 },    /* F5 */
/* 24 */    { 0.54076,        0.58447,        0.60496 },    /* F8 */
/* 25 */    { 0.54853,        0.58799,        0.59446 },    /* G0 */
/* 26 */    { 0.56951,        0.59623,        0.56584 },    /* G5 */
/* 27 */    { 0.58992,        0.60244,        0.53765 },    /* K0 */
/* 28 */    { 0.61098,        0.60693,        0.50828 },    /* K2 */
/* 29 */    { 0.63856,        0.60977,        0.46950 },    /* K5 */
/* 30 */    { 0.68698,        0.60595,        0.40110 },    /* M0 */
/* 31 */    { 0.72528,        0.59434,        0.34744 },    /* M2 */
/* 32 */    { 0.75182,        0.58144,        0.31097 },    /* M3 */
/* 33 */    { 0.78033,        0.56272,        0.27282 },    /* M4 */
/* 34 */    { 0.81066,        0.53676,        0.23394 },    /* M5 */
/* 35 */    { 0.84247,        0.50195,        0.19570 },    /* M6 */
/* 36 */    { 0.87512,        0.45667,        0.16004 },    /* M7 */
/* 37 */    { 0.71033,        0.59983,        0.36829 },    /* N0 */
/* 38 */    { 0.78625,        0.55816,        0.26507 },    /* N3 */
/* 39 */    { 0.93792,        0.33011,        0.10649 },    /* N8 */
/* 40 */    { 0.94897,        0.29906,        0.10012 },    /* N9 */
/* 41 */    { 0.79832,        0.54811,        0.24950 },    /* S4 */
/* 42 */    { 0.38241,        0.45743,        0.80282 },    /* O  */

/* 43 */    { 0.500, 0, 0},              /* r0 */
/* 44 */    { 0.750, 0, 0},              /* r3 */
/* 45 */    { 0.875, 0, 0},              /* r6 */
/* 46 */    { 1.000, 0, 0},              /* r9 */
/* 47 */    { 0, 0.500, 0},              /* g0 */
/* 48 */    { 0, 0.750, 0},              /* g3 */
/* 49 */    { 0, 0.875, 0},              /* g6 */
/* 50 */    { 0, 1.000, 0},              /* g9 */
/* 51 */    { 0, 0, 0.500},              /* b0 */
/* 52 */    { 0, 0, 0.750},              /* b3 */
/* 53 */    { 0, 0, 0.875},              /* b6 */
/* 54 */    { 0, 0, 1.000},              /* b9 */
/* 55 */    { 0.500, 0.500, 0},          /* y0 */
/* 56 */    { 0.750, 0.750, 0},          /* y3 */
/* 57 */    { 0.875, 0.875, 0},          /* y6 */
/* 58 */    { 1.000, 1.000, 0},          /* y9 */
/* 59 */    { 0, 0.500, 0.500},          /* c0 */
/* 60 */    { 0, 0.750, 0.750},          /* c3 */
/* 61 */    { 0, 0.875, 0.875},          /* c6 */
/* 62 */    { 0, 1.000, 1.000},          /* c9 */
/* 63 */    { 0.500, 0, 0.500},          /* p0 */
/* 64 */    { 0.750, 0, 0.750},          /* p3 */
/* 65 */    { 0.875, 0, 0.875},          /* p6 */
/* 66 */    { 1.000, 0, 1.000},          /* p9 */
/* 67 */    { 0.500, 0.250, 0.000},      /* o0 */
/* 68 */    { 0.750, 0.375, 0.000},      /* o3 */
/* 69 */    { 0.875, 0.4375, 0.000},     /* o6 */
/* 70 */    { 1.000, 0.500, 0.000},      /* o9 */
/* 71 */    { 0.500, 0.500, 0.500},      /* w0 */
/* 72 */    { 0.750, 0.750, 0.750},      /* w3 */
/* 73 */    { 0.875, 0.875, 0.875},      /* w6 */
/* 74 */    { 1.000, 1.000, 1.000},      /* w9 */
  };

  if (!color_postscript) return;

  class = 0;
  while (table[class] && (table[class] != color_str[0])) class++;

  subclass = isdigit(color_str[1]) ? color_str[1] - '0' : 0;
  colr = table[class] ? 10 + super_spectra[class][subclass] : WHITE;

/*
    fprintf(outf, "%d '%s' scolor\n", colr, color_str);
*/
  if (colr != cur_colr)
    if (colr == WHITE)
      fprintf(outf, "0 0 0 srgb\n");
    else
      fprintf(outf, "%f %f %f srgb\n",
	     ctab[colr].r, ctab[colr].g, ctab[colr].b); 
/*
      fprintf(outf, "%f %f %f setrgbcolor\n%% %s\n",
	     ctab[colr].r, ctab[colr].g, ctab[colr].b, color_str); 
*/

  cur_colr = colr;
}

/*
static int c_font = -1, c_size = -1;
static char *c_fname = "/Times-Roman";
*/
/* Set the font and font size to be used for text. */
/* Note order of args */
D_fontsize(fsize, font)
     int fsize;	/* Size of font */
     int font;	/* e.g. TIMES, HELV, TIMES+ITALIC */
{

/*  fprintf(outf, "%%fontsize %d %d\n", font, fsize);*/

  /* override some requests */
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
    break;
  }

  if ((fsize == c_size) && (font == c_font)) return;

  switch(font) {
  case TIMESROMAN:
    c_fname = "/Times-Roman";
    break;
  case TIMESBOLD:
    c_fname = "/Times-Bold";
    break;
  case TIMESITAL:
    c_fname = "/Times-Italic";
    break;
  case TIMESBOLDITAL:
    c_fname = "/Times-BoldItalic";
    break;
  case HELV:
    c_fname = "/Helvetica";
    break;
  case HELVBOLD:
    c_fname = "/Helvetica-Bold";
    break;
  case HELVITAL:
    c_fname = "/Helvetica-Oblique";
    break;
  case HELVBOLDITAL:
    c_fname = "/Helvetica-BoldOblique";
    break;
  case COURIER:
    c_fname = "/Courier";
    break;
  case COURBOLD:
    c_fname = "/Courier-Bold";
    break;
  case COURITAL:
    c_fname = "/Courier-Oblique";
    break;
  case COURITALBOLD:
    c_fname = "/Courier-BoldOblique";
    break;
  default:
    font = CURNTFONT;
    break;
  };

/* This doesn't work, since currentfont is already scaled
  if (font == CURNTFONT)
    fprintf(outf, "%d cfsize\n", fsize);
  else
    fprintf(outf, "%d %s fontsz\n", fsize, c_fname);
*/

  fprintf(outf, "%d %s fontsz\n", fsize, c_fname);
  c_size = fsize;
  c_font = font;
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the font used as desired */

/*
 * Additions for Greek fonts
 */
static char  *intable = " abgdezh@iklmnEoprstu0x%w";
static char *outtable = " abgdezhqiklmnxoprstujcyw";

/* Display text string str at x,y, in current font and font size.
   if star_lbl is TRUE, string is a star label, use
      greek characters (if possible) */
D_text(x, y, str, star_lbl)
     int x, y;
     char *str;
     int star_lbl;
{
  char ch;
  int i, j, k;


  /* Note: this ends current line */
  D_move(x,y);

  /* Always issue a moveto.  D_move won't if current point is the point
     to move to.  For text which follows a previous text, this is wrong.
     e.g. "100 100 mt (asdf) lbshw (jkl) lbshw" produces "asdfjkl" instead
     of overwriting asdf with jkl. */
  fprintf(outf, "%d %d mt\n", x, y);

  if (!star_lbl) {
    fprintf(outf, "(%s) lbshw\n", str);
  } else {
    if (isgreek(str[0]) && (isdigit(str[1]) || (str[1] == ' '))) {
      /* Greek if first character is greek encoded,
	 and the second is space or a digit */
      /* Translate yaleformat greek encoding to Symbol font encoding */
#ifdef OLD_GREEK
      i = 0;
      while (ch = str[i])
	{
	  j = 0;
	  while (intable[j] && (intable[j] != ch)) j++;
	  str[i] = intable[j] ? outtable[j] : str[i];
	  /* If not in translation table, leave it alone */
	  i++;
	}
#endif
      fprintf(outf, "(%s) gshow\n", str);
    } else { /* Star label, but not greek */
	/* remove leading spaces */
	while (*str == ' ') str++;
	fprintf(outf, "(%s) obshw\n", str);
    }
  }

  vecstate = S_UNDEF;
}

isgreek(c)
char c;
{
  char *cp;

#ifdef OLD_GREEK
  cp = "abgdezh@iklmnEoprstuOx%w";
#else
  cp = "abgdezhqiklmnxoprstujcywf"; /* f and j are both phi */ 
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
D_comment(str)
     char *str;
{
  fprintf(outf, "%%%s\n", str);
}


/**
Higher level functions
**/

/* Point sizes for font calls */
#define TITLESIZE 16
#define SUBTLSIZE 12
#define NAMESIZE 10
#define LBLSIZE 8
#define MAGSIZE 8

/* Fonts for font calls */
#define NAMEFNT TIMESROMAN
#define LBLFNT HELV
#define MAGFNT COURIER
#define TITLEFNT TIMESBOLD
#define SUBTLFNT TIMESROMAN

int x_nameoffset = MAG0_RAD+ResPT, y_nameoffset = 0;
int x_lbloffset = MAG3_RAD, y_lbloffset = MAG3_RAD+ResPT/2;
int x_magoffset = MAG3_RAD+ResPT, y_magoffset = -MAG3_RAD-ResPT;

/* Interface Function */
/* Draw object at x, y.  properties set by other parameters */
drawobj(x, y, mag, type, color_str, label_field, con_str, obj_name,
	comment_str, file_line,
	draw_glyph, draw_text, use_lbl, use_name, use_mag)

     int x, y;
     double mag;	/* Magnitude of object */
     char *type;	/* 2 chars, object code and subcode */
     char *color_str;	/* 2 chars, spectral type for stars, 
			   color code otherwise */
     char *label_field;	/* 2 chars, Bayer or flamsteed for stars,
			   size in seconds for nebulae and
			   planets */
     char *con_str;	/* 3 chars, the constellation the object is in */
     char *obj_name;	/* Name of object */
     char *comment_str;	/* Comment field */
     char *file_line;	/* The full line from the file,
			   containing the above if it is in
			   standard format */
     int draw_glyph;	/* Draw object symbol */
     int draw_text;	/* Draw text */
     int use_lbl;	/* Label object with the label_field string */
     int use_name;	/* Label object with the obj_name string */
     int use_mag;	/* Label object with a 2 or 3 character string
			   containing the magnitude * 10 without
			   decimal point */
{
  char magstr[10];

/*fprintf(stderr, "%d %d %f <%s> <%s> <%s> <%s> <%s> <%s> <%s>\n", x, y, mag,
	type, color_str, label_field, con_str, obj_name, comment_str,
	file_line);*/

  if (draw_glyph)
    switch(type[0]) {
    case 'S':	drawStar(x,y,mag,type[1],color_str);
      break;
    case 'P':	drawPlan(x,y,mag,type[1],color_str, size_obj(label_field),
			 comment_str);
      break;
    case 'N':	drawNebu(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'G':	drawGalx(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'C':	drawClus(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'U':
      drawUnknown(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'O':
      drawOther(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'V':
    case 'A':
    case 'I':
      break;
    case '#':
    default:
      break;
    };


/*
 * use name or label
 */
  if (draw_text) {
    if (type[0] == 'I')
      D_color(color_str);
    else
      D_color("  ");

    if (use_name && obj_name[0]) {
      D_fontsize(NAMESIZE, NAMEFNT);
      D_text(x+x_nameoffset, y+y_nameoffset, obj_name, FALSE);
    } else if (use_lbl &&
	       ((label_field[0] != ' ') || (label_field[1] != ' '))) {
      D_fontsize(LBLSIZE, LBLFNT);
      D_text(x+x_lbloffset, y+y_lbloffset, label_field, TRUE);
    }
/* If you want to mag label other objects, change this */
    if (use_mag && (type[0] == 'S')) {
      sprintf(magstr, "%02d", (int)(mag*10.0+0.5));
      D_fontsize(MAGSIZE, MAGFNT);
      D_text(x+x_magoffset, y+y_magoffset, magstr, FALSE);
    }
  }
}

drawStar(x, y, mag, type, color)
     int x, y;
     double mag;
     char type, *color;
{
  char *code;
  double truemag = mag;

/* Not used anymore, all labels are the same size.
  switch ((int)(mag+0.5))
    {
    case -1: D_fontsize(18, CURNTFONT); break;
    case  0: D_fontsize(18, CURNTFONT); break;
    case  1: D_fontsize(16, CURNTFONT); break;
    case  2: D_fontsize(14, CURNTFONT); break;
    case  3: D_fontsize(12, CURNTFONT); break;
    case  4: D_fontsize(8, CURNTFONT); break;
    default: D_fontsize(6, CURNTFONT); break;
    }
*/

  if (cur_map_type == THUMBNAIL) mag += thumbshift;
				/* Shrink stars for thumbnail */
  else {
    if (mag < brightest_used) brightest_used = mag;
    if (mag > faintest_used) faintest_used = mag;
  }

  if (mag<MAG_BRIGHT) mag = MAG_BRIGHT;
  if (mag>MAG_DIM) mag = MAG_DIM;

  switch (type)
    {
    default:
    case 'S': code = "s"; break;
    case 'D': code = "d"; break;
    case 'V': code = "v"; break;
    }

  /* Note: this ends current line */
  D_move(x,y);

  D_color(color);
  if (use_fine_mag) {
    /* use fine magnitude scale */
#ifdef USE_FINE_MACROS
    if ((mag*10+0.5) < 0)
      fprintf(outf, "%d %d s%s_%02d\n", x, y, code, - (int) (mag*10+0.5));
    else
      fprintf(outf, "%d %d s%s%02d\n", x, y, code, (int) (mag*10+0.5));
#else
    fprintf(outf, "%d %d %d %s %% %.2f\n", x, y,
	   mag10sizes[(int) ((mag - MAG_BRIGHT)*10.0)], code, truemag);
#endif
  }else {
    if (mag < -0.5) /* rounded mag is less than 0 */
      fprintf(outf, "%d %d %s_1\n", x, y, code); /* MAG_BRIGHT == -1 */
    else
      fprintf(outf, "%d %d %s%d\n", x, y, code, (int) (mag+0.5));
  }

  vecstate = S_UNDEF;
}

drawPlan(x, y, mag, type, color, plansize, comment_str)
     int x,y;
     double mag;
     char type, *color;
     long plansize;
     char *comment_str;
{
  int diam;
  int n;
  double phase, chi;

  /* Note: this ends current line */
  D_move(x,y);

  D_color(color);
  fprintf(outf, "%%diam %.3f\n", (plansize/(3600.*xf_c_scale)+.5));
  diam = (int) (plansize/(3600.*xf_c_scale)+.5);
  if (diam < 12*ResPT)
    diam = 12*ResPT;
/*
fprintf(outf, "%%xf_c_scale %f\n", xf_c_scale);
fprintf(outf, "%%size %d\n", plansize);
fprintf(outf, "%%points %f\n", plansize/(3600.*xf_c_scale));
fprintf(outf, "%%diam %d\n", diam);*/

  switch(type)
    {
    case 'A': fprintf(outf, "%d %d %d asteroid\n", x, y, diam); break;
    case 'C':
      phase = 45.0;
      n = sscanf(comment_str, "%lf", &phase);
      fprintf(outf, "%%phase %f\n", phase);
      if (cur_win->invert) phase = 180 - phase;
      fprintf(outf, "%d %d %f %d comet\n", x, y, phase, diam);
      break;

    case 'S': fprintf(outf, "%d %d %d sun\n", x, y, diam); break;
    case 'L':
      phase = 0.0;
      chi = 90.0;
      n = sscanf(comment_str, "%lf %lf", &phase, &chi);
      fprintf(outf, "%%phase: %f chi: %f\n", phase, chi);
      if (cur_win->invert) chi = 180 - chi;
      fprintf(outf, "%d %d %d %f %f moon\n", x, y, diam, phase, chi);
      break;

    case 'M': fprintf(outf, "%d %d %d mercury\n", x, y, diam); break;
    case 'V': fprintf(outf, "%d %d %d venus\n", x, y, diam); break;
    case 'm': fprintf(outf, "%d %d %d mars\n", x, y, diam); break;
    case 'J': fprintf(outf, "%d %d %d jupiter\n", x, y, diam); break;
    case 's': fprintf(outf, "%d %d %d saturn\n", x, y, diam); break;
    case 'U': fprintf(outf, "%d %d %d uranus\n", x, y, diam); break;
    case 'N': fprintf(outf, "%d %d %d neptune\n", x, y, diam); break;
    case 'P': fprintf(outf, "%d %d %d pluto\n", x, y, diam); break;
    default:  fprintf(outf, "%d %d %d planet\n", x, y, diam);
    }

  vecstate = S_UNDEF;
  D_color("  ");
}

drawGalx(x, y, mag, type, color, nebsize)
     int x,y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  int diam;

  /* Note: this ends current line */
  D_move(x,y);
  D_color(color);

  diam = (int) (nebsize/(3600.*xf_c_scale)+.5);
  if (diam < 6*ResPT)
    diam = 6*ResPT;
/*
fprintf(outf, "%%xf_c_scale %f\n", xf_c_scale);
fprintf(outf, "%%size %d\n", nebsize);
fprintf(outf, "%%points %f\n", nebsize/(3600.*xf_c_scale));
fprintf(outf, "%%diam %d\n", diam);*/

  switch(type)
    {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'B':
    case 'S':
    case 'O':
      fprintf(outf, "%d %d %d galaxys\n", x, y, diam);
      break;
    case 'Q':
      fprintf(outf, "%d %d %d galaxyq\n", x, y, diam);
      break;
    case 'E':
    case 'I':
    case 'G':
      fprintf(outf, "%d %d %d galaxye\n", x, y, diam);
      break;
    default:
      fprintf(outf, "%d %d %d galaxy\n", x, y, diam);
    }

  vecstate = S_UNDEF;
  D_color("  ");
}

drawNebu(x, y, mag, type, color, nebsize)
     int x,y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  int diam;

  /* Note: this ends current line */
  D_move(x,y);
  D_color(color);

  diam = (int) (nebsize/(3600.*xf_c_scale)+.5);
  if (diam < 6*ResPT)
    diam = 6*ResPT;
/*
fprintf(outf, "%%xf_c_scale %f\n", xf_c_scale);
fprintf(outf, "%%size %d\n", nebsize);
fprintf(outf, "%%points %f\n", nebsize/(3600.*xf_c_scale));
fprintf(outf, "%%diam %d\n", diam);*/

  switch(type)
    {
    case 'P': fprintf(outf, "%d %d %d nebulap\n", x, y, diam); break;
    case 'D': fprintf(outf, "%d %d %d nebulad\n", x, y, diam); break;
    default:  fprintf(outf, "%d %d %d nebula\n", x, y, diam);
    }

  vecstate = S_UNDEF;
  D_color("  ");
}

drawClus(x, y, mag, type, color, nebsize)
     int x,y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  int diam;

  /* Note: this ends current line */
  D_move(x,y);
  D_color(color);

  diam = (int) (nebsize/(3600.*xf_c_scale)+.5);
  if (diam < 6*ResPT)
    diam = 6*ResPT;
/*
fprintf(outf, "%%xf_c_scale %f\n", xf_c_scale);
fprintf(outf, "%%size %d\n", nebsize);
fprintf(outf, "%%points %f\n", nebsize/(3600.*xf_c_scale));
fprintf(outf, "%%diam %d\n", diam);*/
  switch(type)
    {
    case 'G': fprintf(outf, "%d %d %d clusterg\n", x, y, diam); break;
    case 'O': fprintf(outf, "%d %d %d clustero\n", x, y, diam); break;
    default: fprintf(outf, "%d %d %d cluster\n", x, y, diam);
    }

  vecstate = S_UNDEF;
  D_color("  ");
}

drawUnknown(x, y, mag, type, color, nebsize)
     int x,y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  int diam;

  /* Note: this ends current line */
  D_move(x,y);
  D_color(color);

  diam = (int) (nebsize/(3600.*xf_c_scale)+.5);
  if (diam < 6*ResPT)
    diam = 6*ResPT;
/*
fprintf(outf, "%%xf_c_scale %f\n", xf_c_scale);
fprintf(outf, "%%size %d\n", nebsize);
fprintf(outf, "%%points %f\n", nebsize/(3600.*xf_c_scale));
fprintf(outf, "%%diam %d\n", diam);*/

  switch(type)
    {
    default:  fprintf(outf, "%d %d %d unknown_u\n", x, y, diam);
    }

  vecstate = S_UNDEF;
  D_color("  ");
}


drawOther(x, y, mag, type, color, nebsize)
     int x,y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
  int diam;

  /* Note: this ends current line */
  D_move(x,y);
  D_color(color);

  diam = (int) (nebsize/(3600.*xf_c_scale)+.5);
  if (diam < 6*ResPT)
    diam = 6*ResPT;
/*
fprintf(outf, "%%xf_c_scale %f\n", xf_c_scale);
fprintf(outf, "%%size %d\n", nebsize);
fprintf(outf, "%%points %f\n", nebsize/(3600.*xf_c_scale));
fprintf(outf, "%%diam %d\n", diam);*/
  switch(type)
    {
    default:  fprintf(outf, "%d %d %d other_o\n", x, y, diam);
    }

  vecstate = S_UNDEF;
  D_color("  ");
}


/* Want three legend columns, in 10 rows, with a title and subtitle above:
title
subtitle (ra, dec, lim)

    -1                0                1
     2                3                4
     5                6                7
     8                9               10
     double           variable
     planet           asteroid        comet
     cluster          globular        open
     nebula           planetary       diffuse
     galaxy           elliptical      spiral
     unknown          other           quasar

star columns x coordinates are at star_x_col[];
star row y coordinates are star_y_row[];
other at oth_x_col[] and oth_y_row[];

title and subtitle coordinates are defined by L_TIL_{X,Y} and L_STIL_{X,Y}
Labels are placed as in drawobj().

for fullpage map, only the title and subtitle are displayed, on one line. */

static int star_x_col[] = {
  36*ResPT,
  108*ResPT,
  180*ResPT,
  36*ResPT,
  108*ResPT,
  180*ResPT,
  36*ResPT,
  108*ResPT,
  180*ResPT,
  36*ResPT,
  108*ResPT,
  180*ResPT,
  36*ResPT,
  108*ResPT,
  180*ResPT};

static int oth_x_col[] = {
  36*ResPT,
  108*ResPT,
  180*ResPT,
  36*ResPT,
  108*ResPT,
  180*ResPT,
  36*ResPT,
  108*ResPT,
  180*ResPT,
  36*ResPT,
  108*ResPT,
  180*ResPT,
  36*ResPT,
  108*ResPT,
  180*ResPT};


#define L_TIL_X 36*ResPT
#define L_TIL_Y 180*ResPT
#define L_STIL_X 36*ResPT
#define L_STIL_Y (L_TIL_Y - 18*ResPT)

/* (let ((i 0))
  (while (< i 10)
    (princ (format "\n  %d*ResPT," (+ 36 (* (- 9 i) 12))))
    (setq i (1+ i)))
    nil) */

static int star_y_row[] = {
  144*ResPT,
  144*ResPT,
  144*ResPT,
  132*ResPT,
  132*ResPT,
  132*ResPT,
  120*ResPT,
  120*ResPT,
  120*ResPT,
  108*ResPT,
  108*ResPT,
  108*ResPT,
  96*ResPT,
  96*ResPT,
  96*ResPT
};

static int oth_y_row[] = {
  84*ResPT,
  84*ResPT,
  84*ResPT,
  72*ResPT,
  72*ResPT,
  72*ResPT,
  60*ResPT,
  60*ResPT,
  60*ResPT,
  48*ResPT,
  48*ResPT,
  48*ResPT,
  36*ResPT,
  36*ResPT,
  36*ResPT};

static int starmags[12];
static int magbright, magfaint;
#define L_X_FULLPAGE_CAPT 18*ResPT
#define L_Y_FULLPAGE_CAPT 25*ResPT

chartlegend(win)
     mapwindow *win;
{
  char ras[20], dls[20], outstr[40], magstr[20];
  int i, j, xcen, ycen;
  int nummags;

  if (!title[0]) title = "LEGEND";
  rastr(ras, win->racen);
  declstr(dls, win->dlcen);

  if (win->map_type != FULLPAGEMAP) {
    sprintf(outstr, "(%s,%s lim: %2.1f)", ras, dls, win->maglim);
    D_fontsize(TITLESIZE, TITLEFNT); D_text(L_TIL_X, L_TIL_Y, title, FALSE);
    D_fontsize(SUBTLSIZE, SUBTLFNT); D_text(L_STIL_X, L_STIL_Y, outstr, FALSE);

    D_fontsize(10, NAMEFNT);

    /* set magnitudes in legend */

    if (brightest_used > faintest_used) {
      brightest_used = 0.0;
      faintest_used = 10.0;
    }

    magbright = brightest_used - 0.5;
    magfaint = faintest_used + 0.5;
    if (magfaint > MAG_DIM) magfaint = MAG_DIM;
    if (magbright < MAG_BRIGHT) magbright = MAG_BRIGHT;
    if (magbright < (magfaint - 11)) magbright = (magfaint - 11);
    nummags = magfaint - magbright + 1;
    for (i = magbright; i <= magfaint; i++) {
      starmags[i-magbright] = i;
    }

    for (i = 0; i < nummags; i++) {
      drawStar(star_x_col[i], star_y_row[i], (double) starmags[i], 'S', "  ");
      sprintf(magstr, " %d", starmags[i]);
      D_text(star_x_col[i]+x_nameoffset, star_y_row[i]+y_nameoffset,
	     magstr, FALSE);
    }

    drawStar(star_x_col[13], star_y_row[13],
	     ((magbright+magfaint)/2.0),
	     'D', "  ");
    D_text(star_x_col[13]+x_nameoffset, star_y_row[13]+y_nameoffset,
	   "double", FALSE);

    drawStar(star_x_col[14], star_y_row[14],
	     ((magbright+magfaint)/2.0),
	     'V', "  ");
    D_text(star_x_col[14]+x_nameoffset, star_y_row[14]+y_nameoffset,
	   "variable", FALSE);



    i = 0;
    drawPlan(oth_x_col[i], oth_y_row[i], 1.0, 'u', "  ", (long) -1, "");
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Planet", FALSE);
    i++;

    drawPlan(oth_x_col[i], oth_y_row[i], 1.0, 'A', "  ", (long) -1, "");
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Asteroid", FALSE);
    i++;

    drawPlan(oth_x_col[i], oth_y_row[i], 1.0, 'C', "  ", (long) -1, "");
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Comet", FALSE);
    i++;

    drawClus(oth_x_col[i], oth_y_row[i], 1.0, 'u', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Cluster", FALSE);
    i++;

    drawClus(oth_x_col[i], oth_y_row[i], 1.0, 'G', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Globular", FALSE);
    i++;

    drawClus(oth_x_col[i], oth_y_row[i], 1.0, 'O', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Open", FALSE);
    i++;


    drawNebu(oth_x_col[i], oth_y_row[i], 1.0, 'u', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Nebula", FALSE);
    i++;

    drawNebu(oth_x_col[i], oth_y_row[i], 1.0, 'P', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Planetary", FALSE);
    i++;

    drawNebu(oth_x_col[i], oth_y_row[i], 1.0, 'D', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Diffuse", FALSE);
    i++;


    drawGalx(oth_x_col[i], oth_y_row[i], 1.0, 'u', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Galaxy", FALSE);
    i++;

    drawGalx(oth_x_col[i], oth_y_row[i], 1.0, 'E', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Elliptical", FALSE);
    i++;

    drawGalx(oth_x_col[i], oth_y_row[i], 1.0, 'S', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Spiral", FALSE);
    i++;


    drawUnknown(oth_x_col[i], oth_y_row[i], 1.0, 'u', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Unknown", FALSE);
    i++;

    drawOther(oth_x_col[i], oth_y_row[i], 1.0, 'o', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Other", FALSE);
    i++;

    drawGalx(oth_x_col[i], oth_y_row[i], 1.0, 'Q', "  ", (long) -1);
    D_text(oth_x_col[i]+x_nameoffset, oth_y_row[i]+y_nameoffset,
	   "Quasar", FALSE);
    i++;

    /* Notebook mode */
#define NOTE_LEFT_COL 306
#define NOTE_RIGHT_COL 486
#define NOTE_TOP_ROW 144
#define NOTE_ROW_SPACE 14*ResPT
    i = 0;
    if (notebook_mode) {
      /* draw views */
      out("gsave");
      out("1 setlinewidth 2 setlinecap");
      out("[] 0 setdash 0 setgray");
      xcen = win->width/2 + win->x_offset;
      ycen = win->height/2 + win->y_offset;
      for (j = 0; j< numviews; j++) {
	fprintf(outf, "    newpath %d %d %f 0 360 arc stroke\n",
	       xcen, ycen, viewrad[j]/win->c_scale);
      };
out("grestore");
      out("12 /Helvetica fontsz");
      out("/rjust { dup stringwidth pop neg 0 rmoveto show} def");
      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Date:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Time:) rjust");
      i++;

      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Object Altitude:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("() rjust");
      i++;

      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Instrument:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("() rjust");
      i++;

      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Eyepiece:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Filter:) rjust");
      i++;

      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Site:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("() rjust");
      i++;

      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Seeing:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Transparency:) rjust");
      i++;

      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Darkness:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Magnitude limit:) rjust");
      i++;

      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Wind:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("() rjust");
      i++;

      fprintf(outf, "%d %d mt", NOTE_LEFT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Temperature:) rjust");
      fprintf(outf, "%d %d mt", NOTE_RIGHT_COL*ResPT,
	     NOTE_TOP_ROW*ResPT - i*NOTE_ROW_SPACE);
      out("(Dew:) rjust");
      i++;
    };

  } else {
    D_fontsize(8, NAMEFNT);
    sprintf(outstr, "%s: %s,%s lim: %2.1f", title, ras, dls, win->maglim);
    D_text(L_X_FULLPAGE_CAPT, L_Y_FULLPAGE_CAPT, outstr, FALSE);
  }
}



