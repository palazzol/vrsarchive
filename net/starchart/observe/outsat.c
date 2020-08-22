/*
 * outsat.c
 * Output satellite, PostScript file for satellites
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

#ifndef  lint
static char rcsid[] =
  "$Header: /home/Vince/cvs/net/starchart/observe/outsat.c,v 1.1 1990-03-30 16:37:57 vrs Exp $";
#endif


#include <stdio.h>
#include "observe.h"
#include "date.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

void outjs(), outjs_PS();
void jupsat(), satsat();

/* Output satellite, .PS file for satellites */
void out_sat(o_sat, o_sat_PS, one_day, invert_sats, jd, moon_data, planets)
     FILE *o_sat, *o_sat_PS;
     int one_day, invert_sats;
     double jd;
     moon_data_t moon_data;
     planet_data_t planets[];
{
  sat_t jovesats[4], saturnsats[8];
  static int last_jd = 0;
  static int doing_saturn = FALSE;

  jupsat(jd, planets[3], jovesats);
  satsat(jd, planets[4], saturnsats);


  if (jd < last_jd) {
    doing_saturn = TRUE;
  };
  last_jd = jd;
  if (!doing_saturn)
    outjs(o_sat, one_day, jd, jovesats, saturnsats, planets);
  outjs_PS(o_sat_PS, one_day, invert_sats, jd,
	   jovesats, saturnsats, moon_data, planets);
}


void outjs(outf, one_day, jd, jovesats, saturnsats)
     FILE *outf;
     int one_day;
     double jd;
     sat_t jovesats[], saturnsats[];
{
  int i;
  char datestr[15];


  jd_to_str(jd, datestr);
  fprintf(outf, "%s\n", datestr);

  fprintf(outf,
  "Relative position in planet radii, +x west +y north, RA seconds east\n");
  fprintf(outf,
"Satellite     Relative position: x  y  z      R.A.      Dec.       mag\n");
  for (i = 0; i < 4; i++)
    fprintf(outf, "%-10.10s %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f\n",
	    jovesats[i].name, jovesats[i].dx, jovesats[i].dy, jovesats[i].dz,
	    jovesats[i].dalpha/15.0, jovesats[i].ddelta,
	    jovesats[i].mag);
  fprintf(outf, "\n");
  for (i = 0; i < 8; i++)
    fprintf(outf, "%-10.10s %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f\n",
	    saturnsats[i].name, saturnsats[i].dx, saturnsats[i].dy,
	    saturnsats[i].dz,
	    saturnsats[i].dalpha/15.0, saturnsats[i].ddelta,
	    saturnsats[i].mag);


  if (!one_day) fprintf(outf, "\f\n");
}

#define CenterLine 288
#define DateY 720
#define JupY 580
#define SatY 396
#define JupOneRad 6
#define SatOneRad 3
#define JOrderOff 36
#define SOrderOff 144

#define JupManyRad 4.5
#define SatManyRad 2.25
#define JupColumn 144
#define SatColumn 144
#define OrderOffset 72
#define DateOffset 192

#define Fontname "/Times-Roman"
#define Bigsize 10
#define JSFontname "/Times-Roman"
#define JSmlsz 10
#define SSFontname "/Times-Roman"
#define SSmlsz 8

int n_called = 0;

/* output postscript picture of jupiter and saturn */
void outjs_PS(outf, one_day, invert_sats, jd,
	      jovesats, saturnsats, moon_data, planets)
     FILE *outf;
     int one_day, invert_sats;
     double jd;
     sat_t jovesats[], saturnsats[];
     moon_data_t moon_data;
     planet_data_t planets[];
{
  int i, j;
  static int printed_header = FALSE;
  static int top = 720;
  static int last_jd = 0;
  static int doing_saturn = FALSE;
  char datestr[15];
  int zindex[9];
  double z[9];
  int yindex[9];
  double y[9];
  int xindex[9];
  double x[9];
  double beta_e_saturn;
  int psx, psy;

  jd_to_str(jd, datestr);

  n_called++;

  beta_e_saturn = planets[4].rotation_elements.beta_e;

  /* Start postscript */
  if (!printed_header) {
    fprintf(outf, "%%!\n");
    fprintf(outf, "%%Remove this header and the later footer\n");
    fprintf(outf,
     "%%    if you are adding the code to a starchart postscript file.\n");
    fprintf(outf,
"%%Place the function definitions after the chart header in that file.\n");
    if (!one_day) {
      fprintf(outf, "%%This file was produced for a multiple day display,\n");
      fprintf(outf,
"%% and is not suitable for use within a starpost output file.\n");
      fprintf(outf,
"%% Prepare charts for individual days\n");
    };
    fprintf(outf, "%s findfont %d scalefont setfont\n", Fontname, Bigsize);
    fprintf(outf, "\n");
    fprintf(outf, "1 setlinewidth 2 setlinecap\n");
    fprintf(outf, "[] 0 setdash 0 setgray\n");
    fprintf(outf, "/s {newpath 0 360 arc closepath fill} def\n");
    fprintf(outf, "/s0 {8 s} def\n");
    fprintf(outf, "/s1 {7 s} def\n");
    fprintf(outf, "/s2 {6 s} def\n");
    fprintf(outf, "/s3 {5 s} def\n");
    fprintf(outf, "/s4 {4 s} def\n");
    fprintf(outf, "/s5 {3.25 s} def\n");
    fprintf(outf, "/s6 {2.5 s} def\n");
    fprintf(outf, "/s7 {2.0 s} def\n");
    fprintf(outf, "/s8 {1.5 s} def\n");
    fprintf(outf, "/s9 {1.0 s} def\n");
    fprintf(outf, "/s10 {0.5 s} def\n");
    fprintf(outf, "%%\n%%\n%%\n%%End of header\n%%\n%%\n");
    fprintf(outf, "%%\n%%\n%%\n%%Begin definitions\n%%\n%%\n");
    fprintf(outf,
"/centershow { dup stringwidth pop 2 div neg 0 rmoveto show } def\n");
    fprintf(outf,
"/atshow10pt { dup stringwidth pop 2 div neg 3.5 neg rmoveto show } def\n");


    fprintf(outf,
 "/array-centershow { /str-arr exch def /widthskip exch def\n");
    fprintf(outf, "  str-arr length widthskip mul 2 div neg 0 rmoveto\n");
    fprintf(outf, "  str-arr {\n");
    fprintf(outf, "    gsave show grestore\n");
    fprintf(outf, "    widthskip 0 rmoveto\n");
    fprintf(outf, "  } forall\n");
    fprintf(outf, "} def\n");
    fprintf(outf, "/array-vertshow { /str-arr exch def /lineskip exch def\n");
    fprintf(outf, "  0 str-arr length lineskip mul 2 div neg rmoveto\n");
    fprintf(outf, "  str-arr {\n");
    fprintf(outf, "    gsave\n");
    fprintf(outf, "      dup stringwidth pop 2 div neg 0 rmoveto show\n");
    fprintf(outf, "    grestore\n");
    fprintf(outf, "    0 lineskip rmoveto\n");
    fprintf(outf, "  } forall\n");
    fprintf(outf, "} def\n");

    fprintf(outf, "/Textx 2 def\n");
    fprintf(outf, "/Texty 2 def\n");


    fprintf(outf, "%% x y r phase p_n beta_e chi\n");
    fprintf(outf, "/planet-with-phase {\n");
    fprintf(outf, "  /chi exch def /beta_e exch def /p_n exch def\n");
    fprintf(outf, "  /phase exch def\n");
    fprintf(outf, "  /r exch def /y exch def /x exch def gsave\n");
    fprintf(outf, "    0 setgray .01 setlinewidth\n");
    fprintf(outf, "    x y translate r r scale\n");
    if (invert_sats) {
      if (one_day)
	fprintf(outf, "    1 -1 scale\n");
      else
	fprintf(outf, "    -1 1 scale\n");
      };
    fprintf(outf, "    gsave p_n rotate\n");
    fprintf(outf, "      1 setgray\n");
    fprintf(outf, "      0 0 1.05 0 360 arc fill\n");
    fprintf(outf, "      0 setgray\n");
    fprintf(outf, "      0 0 1 0 360 arc stroke\n");
    fprintf(outf, "      gsave\n");
    fprintf(outf, "        1 beta_e cos scale\n");
    fprintf(outf, "        0 1 moveto 0 1.25 lineto stroke\n");
    fprintf(outf, "      grestore\n");
    fprintf(outf, "      gsave\n");
    fprintf(outf, "        1 beta_e sin scale\n");
    fprintf(outf, "        0 0 1 180 360 arc stroke\n");
    fprintf(outf, "      grestore\n");
    fprintf(outf, "    grestore %% restore p_n rotation\n");
    fprintf(outf, "    gsave\n");
    fprintf(outf, "      chi rotate\n");
    fprintf(outf, "      newpath\n");
    fprintf(outf, "      0 0 1 0 180 arcn\n");
    fprintf(outf, "      1 phase cos scale\n");
    fprintf(outf, "      0 0 1 180 360 arc fill\n");
    fprintf(outf, "    grestore\n");
    fprintf(outf, "  grestore\n} def\n");

  /* Saturn with rings, no satellites, with north pole and equator */
    fprintf(outf, "/saturn-with-phase  {\n");
    fprintf(outf, "  /chi exch def /beta_e exch def /p_n exch def\n");
    fprintf(outf, "  /phase exch def\n");
    fprintf(outf, "  /r exch def /y exch def /x exch def gsave\n");
    fprintf(outf, "    0 setgray .01 setlinewidth\n");
    fprintf(outf, "    x y translate r r scale\n");
    if (invert_sats) {
      if (one_day)
	fprintf(outf, "    1 -1 scale\n");
      else
	fprintf(outf, "    -1 1 scale\n");
      };
    fprintf(outf, "    gsave p_n rotate\n");
    fprintf(outf, "      gsave 1 beta_e sin scale newpath\n"); /* back ring */
    fprintf(outf, "        0 0 1.51 0 180 arc stroke\n");
    fprintf(outf, "        0 0 1.94 0 180 arc stroke\n");
    fprintf(outf, "        0 0 2.00 0 180 arc stroke\n");
    fprintf(outf, "        0 0 2.27 0 180 arc stroke\n");
    fprintf(outf, "      grestore\n");
    fprintf(outf, "      1 setgray\n");
    fprintf(outf, "      0 0 1.05 0 360 arc fill\n");
    fprintf(outf, "      0 setgray\n");
    fprintf(outf, "      0 0 1 0 360 arc stroke\n");
    fprintf(outf, "      gsave\n");
    fprintf(outf, "        1 beta_e cos scale\n");
    fprintf(outf, "        0 1 moveto 0 1.25 lineto stroke\n");
    fprintf(outf, "      grestore\n");
    fprintf(outf, "      gsave\n");
    fprintf(outf, "        1 beta_e sin scale\n");
    fprintf(outf, "        0 0 1 180 360 arc stroke\n");
    fprintf(outf, "      grestore\n");
    fprintf(outf, "    grestore %% restore p_n rotation\n");
    fprintf(outf, "    gsave\n");
    fprintf(outf, "      chi rotate\n");
    fprintf(outf, "      newpath\n");
    fprintf(outf, "      0 0 1 0 180 arcn\n");
    fprintf(outf, "      1 phase cos scale\n");
    fprintf(outf, "      0 0 1 180 360 arc fill\n");
    fprintf(outf, "    grestore\n");
    fprintf(outf, "    gsave p_n rotate\n");
    fprintf(outf, "      gsave 1 beta_e sin scale newpath\n"); /* front ring */
    fprintf(outf, "        0 0 1.51 180 360 arc stroke\n");
    fprintf(outf, "        0 0 1.94 180 360 arc stroke\n");
    fprintf(outf, "        0 0 2.00 180 360 arc stroke\n");
    fprintf(outf, "        0 0 2.27 180 360 arc stroke\n");
    fprintf(outf, "      grestore\n");
    fprintf(outf, "    grestore\n");
    fprintf(outf, "  grestore\n} def\n");


    fprintf(outf, "/Luna-with-phase {\n");
    fprintf(outf, "  /chi exch def\n");
    fprintf(outf, "  /phase exch def\n");
    fprintf(outf, "  /r exch def /y exch def /x exch def gsave\n");
    fprintf(outf, "    0 setgray .01 setlinewidth\n");
    fprintf(outf, "    x y translate r r scale\n");
    if (invert_sats) {
      if (one_day)
	fprintf(outf, "    1 -1 scale\n");
      else
	fprintf(outf, "    -1 1 scale\n");
      };
    fprintf(outf, "    1 setgray\n");
    fprintf(outf, "    0 0 1.05 0 360 arc fill\n");
    fprintf(outf, "    0 setgray\n");
    fprintf(outf, "    0 0 1 0 360 arc stroke\n");
    fprintf(outf, "    gsave\n");
    fprintf(outf, "      chi rotate\n");
    fprintf(outf, "      newpath\n");
    fprintf(outf, "      0 0 1 0 180 arcn\n");
    fprintf(outf, "      1 phase cos scale\n");
    fprintf(outf, "      0 0 1 180 360 arc fill\n");
    fprintf(outf, "    grestore\n");
    fprintf(outf, "  grestore\n} def\n");


    /* Define satellites */
    for (i = 0; i < 4; i++) {
/*
      if (one_day) {
	fprintf(outf, "/%s {2 copy s%d\n", jovesats[i].name,
		(int) (jovesats[i].mag+0.5));
	fprintf(outf,
		"     Texty add exch Textx add exch moveto (%c%c) show\n",
		jovesats[i].name[0], jovesats[i].name[1]);
      } else {
	fprintf(outf, "/%s {s%d\n", jovesats[i].name,
		(int) (jovesats[i].mag+0.5));
      };
*/
      fprintf(outf, "/%s {s%d\n", jovesats[i].name,
	      (int) (jovesats[i].mag+0.5));
      fprintf(outf, "     } def\n");
    };

    fprintf(outf,
"%% Saturn satellites are shown 4 magnitudes brighter then they are\n");
    for (i = 0; i < 8; i++) {
      j = saturnsats[i].mag - 4 +0.5;
      if (j > 10) j = 10;
      fprintf(outf, "/%s {s%d\n", saturnsats[i].name, j);
      fprintf(outf, "     } def\n");
    };

    /* Define Jupiter and saturn without satellites */
    fprintf(outf, "\n");
    fprintf(outf, "/Jupiter-body {3 copy newpath 0 360 arc 1 setgray fill\n");
    fprintf(outf, "    0 360 arc 0 setgray stroke} def\n");


    fprintf(outf,
	  "/back-ring {/beta exch def /r exch def /y exch def /x exch def\n");
    fprintf(outf, "    newpath gsave x y translate 1 beta sin scale\n");
    fprintf(outf, "    0 0 r 1.51 mul 0 180 arc stroke\n");
    fprintf(outf, "    0 0 r 2.27 mul 0 180 arc stroke\n");
    fprintf(outf, "    grestore} def\n");
    fprintf(outf,
	  "/front-ring {/beta exch def /r exch def /y exch def /x exch def\n");
    fprintf(outf, "    newpath gsave x y translate 1 beta sin scale\n");
    fprintf(outf, "    0 0 r 1.51 mul 180 360 arc stroke\n");
    fprintf(outf, "    0 0 r 2.27 mul 180 360 arc stroke\n");
    fprintf(outf, "    grestore} def\n");
    fprintf(outf, "\n");
    fprintf(outf, "/Saturn-body {/r exch def /y exch def /x exch def\n");
    fprintf(outf, "    /beta %f def\n", beta_e_saturn);
    fprintf(outf, "    x y r beta back-ring\n");
    fprintf(outf, "    x y r 0 360 arc 1 setgray fill\n");
    fprintf(outf, "    x y r 0 360 arc 0 setgray stroke\n");
    fprintf(outf, "    x y r beta front-ring\n");
    fprintf(outf, "} def\n");


    fprintf(outf, "\n");
    fprintf(outf, "\n");

    printed_header = TRUE;
  };



  /* Define Jupiter and saturn with satellites */
  /* Jupiter */
  /* Find drawing order */
  for (i = 0; i < 4; i++)
    z[i] = jovesats[i].dz;
  z[4] = 0.0;			/* Jupiter's body */
  HeapSort0(z, zindex, 5);

  fprintf(outf, "/Jupiter ");
  fprintf(outf, " { /r exch def gsave translate\n");
  if (invert_sats) {
    if (one_day)
      fprintf(outf, "    1 -1 scale\n");
    else
      fprintf(outf, "    -1 1 scale\n");
  };
  fprintf(outf, "    %f rotate\n",
	  planets[3].rotation_elements.p_n);
  for (i = 0; i < 5; i++) {
    if (zindex[i] < 4) {
      fprintf(outf, "    r %f mul r %f mul %s\n",
	      jovesats[zindex[i]].dx,
	      jovesats[zindex[i]].dy, jovesats[zindex[i]].name);
    } else {
      fprintf(outf, "    0 0 r Jupiter-body\n");
    };
  };
  fprintf(outf, "grestore } def\n");
  /* Define order string */
  for (i = 0; i < 4; i++)
    x[i] = -jovesats[i].dalpha;

  if (invert_sats)
    if (!one_day)
      for (i = 0; i < 4; i++)
	x[i] = jovesats[i].dalpha;
      
  x[4] = 0.0;			/* Jupiter's body */
  HeapSort0(x, xindex, 5);
  for (i = 0; i < 5; i++)
    if (xindex[i] == 4) j = i;

  fprintf(outf, "/Jup-order-array [");
  for (i = 0; i < (4 - 2*j); i++) fprintf(outf, " (  )");
  for (i = 0; i < 5; i++) {
    if (xindex[i] < 4) {
      fprintf(outf, " (%c%c)",
	      jovesats[xindex[i]].name[0], jovesats[xindex[i]].name[1]);
    } else {
      fprintf(outf, " (J)");
    }
  };
  for (i = 0; i < (2*(j-2)); i++) fprintf(outf, " (  )");
  fprintf(outf, "] def\n\n");
  fprintf(outf, "\n");

  /* Saturn */
  /* Find drawing order */
  for (i = 0; i < 8; i++)
    z[i] = saturnsats[i].dz;
  z[8] = 0.0;			/* Saturn's body */
  HeapSort0(z, zindex, 9);

  fprintf(outf, "/Saturn ");
  fprintf(outf, " { /r exch def gsave translate\n");
  if (invert_sats) {
    if (one_day)
      fprintf(outf, "    1 -1 scale\n");
    else
      fprintf(outf, "    -1 1 scale\n");
  };
  fprintf(outf, "    %f rotate\n",
	  planets[4].rotation_elements.p_n);
  for (i = 0; i < 9; i++) {
    if (zindex[i] < 8) {
      fprintf(outf, "    r %f mul r %f mul %s\n",
	      saturnsats[zindex[i]].dx,
	      saturnsats[zindex[i]].dy, saturnsats[zindex[i]].name);
    } else {
      fprintf(outf, "    0 0 r Saturn-body\n");
    };
  };
  fprintf(outf, "grestore } def\n");

  /* Define order string */
  for (i = 0; i < 8; i++)
    x[i] = -saturnsats[i].dalpha;

  if (invert_sats)
    if (!one_day)
      for (i = 0; i < 8; i++)
	x[i] = saturnsats[i].dalpha;
      
  x[8] = 0.0;			/* Saturn's body */
  HeapSort0(x, xindex, 9);
  for (i = 0; i < 9; i++)
    if (xindex[i] == 8) j = i;
  fprintf(outf, "/Sat-order-array [");
  for (i = 0; i < (8 - 2*j); i++) fprintf(outf, " (  )");
  for (i = 0; i < 9; i++) {
    if (xindex[i] < 8) {
      fprintf(outf, " (%c%c)",
	      saturnsats[xindex[i]].name[0], saturnsats[xindex[i]].name[1]);
    } else {
      fprintf(outf, " (S)");
    }
  };
  for (i = 0; i < (2*(j-4)); i++) fprintf(outf, " (  )");
  fprintf(outf, "] def\n\n");
  fprintf(outf, "\n");
  fprintf(outf, "\n");

  /* Vertical for saturn */
  for (i = 0; i < 8; i++)
    y[i] = saturnsats[i].ddelta;

  if (invert_sats)
    if (one_day)
      for (i = 0; i < 8; i++)
	y[i] = -saturnsats[i].ddelta;

  y[8] = 0.0;			/* Saturn's body */
  HeapSort0(y, yindex, 9);
  for (i = 0; i < 9; i++)
    if (yindex[i] == 8) j = i;
  fprintf(outf, "/Sat-yorder-array [");
  for (i = 0; i < (8 - 2*j); i++) fprintf(outf, " (  )");
  for (i = 0; i < 9; i++) {
    if (yindex[i] < 8) {
      fprintf(outf, " (%c%c)",
	      saturnsats[yindex[i]].name[0], saturnsats[yindex[i]].name[1]);
    } else {
      fprintf(outf, " (S)");
    }
  };
  for (i = 0; i < (2*(j-4)); i++) fprintf(outf, " (  )");
  fprintf(outf, "] def\n\n");
  fprintf(outf, "\n");
  fprintf(outf, "\n");

  /* Draw them */
  if (one_day) {
    fprintf(outf, "/jupiter {2 div Jupiter} def\n");
    fprintf(outf, "/saturn {2 div Saturn} def\n");
    fprintf(outf,
"%% The above \"jupiter\" and \"saturn\" can be called from starchart postscript\n");
    fprintf(outf, "%%End definitions\n");
    fprintf(outf,
	  "%%Begin footer: Remove if inserted in starchart postscript file\n");
    for (i = 0; i < 5; i++) {
      psx = 72 + i*72;
      psy = 180;
      if (i != 4)
	fprintf(outf,"%d %d %f %f %f %f %f planet-with-phase\n", psx, psy,
		planets[i].size* 0.70866141732283465, /* 1 cm = 20 s */
		planets[i].phase,
		planets[i].rotation_elements.p_n,
		planets[i].rotation_elements.beta_e,
		planets[i].chi);
      else
	fprintf(outf,"%d %d %f %f %f %f %f saturn-with-phase\n", psx, psy,
		planets[i].size* 0.70866141732283465, /* 1 cm = 20 s */
		planets[i].phase,
		planets[i].rotation_elements.p_n,
		planets[i].rotation_elements.beta_e,
		planets[i].chi);
      fprintf(outf, "%d %d moveto (%s) centershow\n", psx, psy - 72,
	      planets[i].name);
      fprintf(outf, "%d %d moveto (Meridian) centershow\n",
	      psx, psy - 90);
      fprintf(outf, "%d %d moveto (%.0f\\312) centershow\n",
	      psx, psy - 102,
	      planets[i].rotation_elements.lambda_e);
    };

    fprintf(outf, "504 180 36 %f %f Luna-with-phase\n",
	    moon_data.phase, moon_data.chi);
    fprintf(outf, "504 108 moveto (Moon) centershow\n");
    fprintf(outf, "504 90 moveto (%.2f arcmin) centershow\n",
	    moon_data.size/60.0);

    psy = 48;
    psx = 72;
    fprintf(outf,
 "%d %d moveto %f %d lineto stroke %f %d moveto (  1 cm = 20 arcsec) show\n",
	    psx, psy, psx + 20*0.70866141732283465, /* 1 cm = 20 s */ psy,
	    psx + 20*0.70866141732283465, psy);
	    
    fprintf(outf, "%d %d moveto (%s) centershow\n",
	    CenterLine, DateY, datestr);

    psx = CenterLine;
    psy = DateY - 72;
    if (invert_sats) {
      fprintf(outf, "%d %d moveto %d %d lineto %d %d lineto stroke\n",
	      psx, psy + 12, psx, psy - 12, psx - 4, psy - 12 + 4);
      fprintf(outf, "%d %d moveto (N) atshow10pt\n", psx, psy - 12 - 9);
      fprintf(outf, "%d %d moveto (S) atshow10pt\n", psx, psy + 12 + 9);
    } else {
      fprintf(outf, "%d %d moveto %d %d lineto %d %d lineto stroke\n",
	      psx, psy - 12, psx, psy + 12, psx + 4, psy + 12 - 4);
      fprintf(outf, "%d %d moveto (N) atshow10pt\n", psx, psy + 12 + 9);
      fprintf(outf, "%d %d moveto (S) atshow10pt\n", psx, psy - 12 - 9);
    };
    fprintf(outf, "%d %d moveto %d %d lineto stroke\n",
	    psx-12, psy, psx+12, psy);
    fprintf(outf, "%d %d moveto (E) atshow10pt\n", psx - 12 - 9, psy);
    fprintf(outf, "%d %d moveto (W) atshow10pt\n", psx + 12 + 9, psy);


    fprintf(outf, "%d %d %d Jupiter\n", CenterLine, JupY, JupOneRad);
    fprintf(outf, "/MMwidth (MM) stringwidth pop def\n");
    fprintf(outf,
 "%d MMwidth 2 div add %d moveto MMwidth Jup-order-array array-centershow\n",
	    CenterLine, JupY-JOrderOff);
    fprintf(outf, "\n");

    fprintf(outf, "%d %d %d Saturn\n", CenterLine, SatY, SatOneRad);
    fprintf(outf,
 "%d MMwidth 2 div add %d moveto MMwidth Sat-order-array array-centershow\n",
	    CenterLine, SatY-SOrderOff);
    fprintf(outf, "%d %d moveto 12 Sat-yorder-array array-vertshow\n",
	    CenterLine + SOrderOff, SatY);
    fprintf(outf, "\n");
  } else {
    if (jd < last_jd) {
      doing_saturn = TRUE;
      fprintf(outf, "%d %d moveto (N) show\n", CenterLine, 756);
      fprintf(outf, "%d %d moveto (S) show\n", CenterLine, 36);
      if (invert_sats) {
	fprintf(outf, "%d %d moveto (E) show\n", 576, 360);
	fprintf(outf, "%d %d moveto (W) show\n", 36, 360);
      } else {
	fprintf(outf, "%d %d moveto (E) show\n", 36, 360);
	fprintf(outf, "%d %d moveto (W) show\n", 576, 360);
      };
      fprintf(outf, "showpage\n");

      top = 720;
    };
    last_jd = jd;
    if (!doing_saturn) {
      fprintf(outf, "%d %d %f Jupiter\n", JupColumn, top, JupManyRad);
      fprintf(outf, "%s findfont %d scalefont setfont\n", JSFontname, JSmlsz);
      fprintf(outf,
 "%f %d moveto (MM) stringwidth pop Jup-order-array array-centershow\n",
	      JupColumn + 36*JupManyRad + OrderOffset, top);
      fprintf(outf, "%s findfont %d scalefont setfont\n", Fontname, Bigsize);
      fprintf(outf, "%f %d moveto (%s) show\n",
	      JupColumn + 36*JupManyRad + DateOffset,
	      top, datestr);

      fprintf(outf, "\n");
      top -= 18;
      if (top <= 71) {
	fprintf(outf, "%d %d moveto (N) show\n", CenterLine, 756);
	fprintf(outf, "%d %d moveto (S) show\n", CenterLine, 36);
	if (invert_sats) {
	  fprintf(outf, "%d %d moveto (E) show\n", 576, 360);
	  fprintf(outf, "%d %d moveto (W) show\n", 36, 360);
	} else {
	  fprintf(outf, "%d %d moveto (E) show\n", 36, 360);
	  fprintf(outf, "%d %d moveto (W) show\n", 576, 360);
	};
	fprintf(outf, "showpage\n");
	top = 720;
      };
    } else {
      fprintf(outf, "%d %d %f Saturn\n", SatColumn, top, SatManyRad);
      fprintf(outf, "%s findfont %d scalefont setfont\n", SSFontname, SSmlsz);
      fprintf(outf,
 "%f %d moveto (MM) stringwidth pop Sat-order-array array-centershow\n",
	      SatColumn + 72*SatManyRad + OrderOffset, top);
      fprintf(outf, "%s findfont %d scalefont setfont\n", Fontname, Bigsize);
      fprintf(outf, "%f %d moveto (%s) show\n",
	      SatColumn + 72*SatManyRad + DateOffset,
	      top, datestr);

      fprintf(outf, "\n");
      top -= 72;
      if (top <= 71) {
	fprintf(outf, "%d %d moveto (N) show\n", CenterLine, 756);
	fprintf(outf, "%d %d moveto (S) show\n", CenterLine, 36);
	if (invert_sats) {
	  fprintf(outf, "%d %d moveto (E) show\n", 576, 360);
	  fprintf(outf, "%d %d moveto (W) show\n", 36, 360);
	} else {
	  fprintf(outf, "%d %d moveto (E) show\n", 36, 360);
	  fprintf(outf, "%d %d moveto (W) show\n", 576, 360);
	};
	fprintf(outf, "showpage\n");
	top = 720;
      };
    };
  };

  if (one_day)
    fprintf(outf, "showpage\n");
}

/* End: called if multiple day */
void out_sat_end(o_sat, outf, invert_sats, start_date, interval_days)
     FILE *o_sat, *outf;
     int invert_sats;
     double start_date, interval_days;
{

  fprintf(outf, "%s findfont %d scalefont setfont\n", Fontname, Bigsize);
  fprintf(outf, "%d %d moveto (N) show\n", CenterLine, 756);
  fprintf(outf, "%d %d moveto (S) show\n", CenterLine, 36);
  if (invert_sats) {
    fprintf(outf, "%d %d moveto (E) show\n", 576, 360);
    fprintf(outf, "%d %d moveto (W) show\n", 36, 360);
  } else {
    fprintf(outf, "%d %d moveto (E) show\n", 36, 360);
    fprintf(outf, "%d %d moveto (W) show\n", 576, 360);
  };
  fprintf(outf, "showpage\n");
}
