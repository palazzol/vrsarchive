/*
 * outsif.c
 * Output .sif and .star files for object(s)
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
  "$Header: /home/Vince/cvs/net/starchart/observe/outsif.c,v 1.1 1990-03-30 16:37:59 vrs Exp $";
#endif


#include <stdio.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include "observe.h"
#include "date.h"


void write_files(), tosizestr();


/* Output .sif and .star files for object(s) */
void out_sif(o_sif, o_star, do_sif, do_star, one_day,
	    sun_data, moon_data, planets, bodies, nbodies, objects, nobjects)
     FILE *o_sif, *o_star;
     int do_sif, do_star;	/* output sif file , .star file */
     int one_day;		/* only doing one day */
     sun_data_t sun_data;
     moon_data_t moon_data;
     planet_data_t planets[];
     wanderer_data_t bodies[];
     int nbodies;
     obj_data_t objects[];
     int nobjects;
{
  int i;
  char moon_phase[20];
  char *typefield;

  if (one_day) {
				/* print all */
    write_files(o_sif, o_star, do_sif, do_star,
		sun_data.alpha2000/15.0, sun_data.delta2000,
		-26.0,
		"PS", "G2", sun_data.size,
		"   ", "Sol", "");

    sprintf(moon_phase, "%6.1f %6.1f", moon_data.phase, moon_data.chi);
    write_files(o_sif, o_star, do_sif, do_star,
		moon_data.alpha2000/15.0, moon_data.delta2000,
		moon_data.mag,
		"PL", "G2", moon_data.size,
		"   ", "Moon", moon_phase);

    for (i = 0; i < 7; i++)
      if (planets[i].name[0])
	write_files(o_sif, o_star, do_sif, do_star,
		    planets[i].alpha2000/15.0, planets[i].delta2000,
		    planets[i].mag,
		    planets[i].type, planets[i].color, planets[i].size,
		    "   ", planets[i].name, "");

    for (i = 0; i < nbodies; i++) {
      if (bodies[i].body_type == asteroid)
	typefield = "PA";
      else if (bodies[i].body_type == comet)
	typefield = "PC";
      else
	typefield = "P?";

      write_files(o_sif, o_star, do_sif, do_star,
		  bodies[i].alpha2000/15.0, bodies[i].delta2000,
		  bodies[i].mag,
		  typefield, "  ", -1.0,
		  "   ", bodies[i].name, "");
    };
 
   for (i = 0; i < nobjects; i++)
      write_files(o_sif, o_star, do_sif, do_star,
		  objects[i].alpha2000/15.0, objects[i].delta2000,
		  objects[i].mag,
		  objects[i].type, "  ", objects[i].size,
		  "   ", objects[i].name, "");
  } else {
				/* only print object list */
    for (i = 0; i < nobjects; i++)
      write_files(o_sif, o_star, do_sif, do_star,
		  objects[i].alpha2000/15.0, objects[i].delta2000,
		  objects[i].mag,
		  objects[i].type, "  ", objects[i].size,
		  "   ", objects[i].name, "");
  }
}


/* Output .sif and .star files for a planet */
void out_sif_planet(o_sif, o_star, do_sif, do_star, one_day,
	     planet, jd)
     FILE *o_sif, *o_star;
     int do_sif, do_star;	/* output sif file , .star file */
     int one_day;		/* only doing one day */
     planet_data_t planet;
     double jd;
{
  static char *namep = "";
  char *vectfield;
  char datestr[15];
  char name_date[96];

  if (namep == planet.name) {	/* we've done this before */
    vectfield = "VS";
  } else {			/* this is a new one */
    namep = planet.name;
    vectfield = "VM";
  };

  jd_to_str(jd, datestr);
  sprintf(name_date, "%s %s", planet.name, datestr);

  write_files(o_sif, o_star, do_sif, do_star,
	      planet.alpha2000/15.0, planet.delta2000,
	      planet.mag,
	      planet.type, "  ", planet.size,
	      "   ", name_date, "");
  write_files(o_sif, o_star, do_sif, do_star,
	      planet.alpha2000/15.0, planet.delta2000,
	      planet.mag + 1.0,
	      vectfield, "  ", planet.size,
	      "   ", name_date, "");
}

/* Output .sif and .star files for a body */
void out_sif_body(o_sif, o_star, do_sif, do_star, one_day,
	     body, jd)
     FILE *o_sif, *o_star;
     int do_sif, do_star;	/* output sif file , .star file */
     int one_day;		/* only doing one day */
     wanderer_data_t body;
     double jd;
{
  static char *namep = "";
  char *vectfield, *typefield;
  char datestr[15];
  char name_date[96];

  if (namep == body.name) {	/* we've done this before */
    vectfield = "VS";
  } else {			/* this is a new one */
    namep = body.name;
    vectfield = "VM";
  };

  if (body.body_type == asteroid)
    typefield = "PA";
  else if (body.body_type == comet)
    typefield = "PC";
  else
    typefield = "P?";

  jd_to_str(jd, datestr);
  sprintf(name_date, "%s %s", body.name, datestr);

  write_files(o_sif, o_star, do_sif, do_star,
	      body.alpha2000/15.0, body.delta2000,
	      body.mag,
	      typefield, "  ", -1.0,
	      "   ", name_date, "");
  write_files(o_sif, o_star, do_sif, do_star,
	      body.alpha2000/15.0, body.delta2000,
	      body.mag + 1.0,
	      vectfield, "  ", -1.0,
	      "   ", name_date, "");
}

/* output sun */
void out_sif_sun(o_sif, o_star, do_sif, do_star, one_day,
	     sun_data, jd)
     FILE *o_sif, *o_star;
     int do_sif, do_star;	/* output sif file , .star file */
     int one_day;		/* only doing one day */
     sun_data_t sun_data;
     double jd;
{
  static char *namep = "";
  char *vectfield;
  char datestr[15];
  char name_date[96];

  if (!strcmp(namep,"Sol")) {	/* we've done this before */
    vectfield = "VS";
  } else {			/* this is a new one */
    namep = "Sol";
    vectfield = "VM";
  };

  jd_to_str(jd, datestr);
  sprintf(name_date, "%s %s", "Sol", datestr);

  write_files(o_sif, o_star, do_sif, do_star,
	      sun_data.alpha2000/15.0, sun_data.delta2000,
	      -26.0,
	      "PS", "G2", sun_data.size,
	      "   ", name_date, "");
  write_files(o_sif, o_star, do_sif, do_star,
	      sun_data.alpha2000/15.0, sun_data.delta2000,
	      -25.0,
	      vectfield, "  ", sun_data.size,
	      "   ", name_date, "");
}
/* output moon */
void out_sif_moon(o_sif, o_star, do_sif, do_star, one_day,
	     moon_data, jd)
     FILE *o_sif, *o_star;
     int do_sif, do_star;	/* output sif file , .star file */
     int one_day;		/* only doing one day */
     moon_data_t moon_data;
     double jd;
{
  static char *namep = "";
  char *vectfield;
  char datestr[15];
  char name_date[96];
  char moon_phase[20];

  if (!strcmp(namep,"moon")) {	/* we've done this before */
    vectfield = "VS";
  } else {			/* this is a new one */
    namep = "moon";
    vectfield = "VM";
  };

  jd_to_str(jd, datestr);
  sprintf(name_date, "%s %s", "Moon", datestr);
  sprintf(moon_phase, "%6.1f %6.1f", moon_data.phase, moon_data.chi);

  write_files(o_sif, o_star, do_sif, do_star,
	      moon_data.alpha2000/15.0, moon_data.delta2000,
	      moon_data.mag,
	      "PL", "G2", moon_data.size,
	      "   ", name_date, moon_phase);
  write_files(o_sif, o_star, do_sif, do_star,
	      moon_data.alpha2000/15.0, moon_data.delta2000,
	      moon_data.mag + 1.0,
	      vectfield, "  ", moon_data.size,
	      "   ", name_date, "");
}


void write_files(o_sif, o_star, do_sif, do_star, ra, dec, mag, type, color,
		 size, constell, name, comment)
     FILE *o_sif, *o_star;
     int do_sif, do_star;	/* output sif file , .star file */
     double ra, dec, mag;
     char type[3], color[3];
     double size;
     char constell[4];
     char *name;
     char *comment;
{
  int rah, ram, ras, dld, dlm, d_d, d_m, d_s, sign;
  double dec_t;
  int i;
  char magstr[4];
  char sizestr[3];

  rah = ra;
  ra -= rah;
  ram = ra*60.0;
  ra -= ram/60.0;
  ras = ra*3600.0 + 0.5;
  if (ras == 60) {
    ras = 0;
    ram++;
  }
  if (ram == 60) {
    ram = 0;
    rah++;
  }
  if (rah == 24) rah = 0;

  /* dec -> degrees and minutes */
  dec_t = dec;
  sign = (dec_t < 0.0);
  if (sign) dec_t = -dec_t;
  dld = dec_t;
  dec_t -= dld;
  dlm = dec_t * 60.0 + 0.5;
  if (dlm == 60) {
    dlm = 0;
    dld++;
  }

  /* dec -> degrees, minutes and seconds */
  dec_t = dec;
  if (sign) dec_t = -dec_t;
  d_d = (int) dec_t;
  d_m = (int)((dec_t - d_d)*60);
  d_s = (int)((dec_t - d_d - d_m/60.0)*3600 + 0.5);
  if (d_s == 60) {
    d_s = 0;
    d_m++;
  }
  if (d_m == 60) {
    d_m = 0;
    d_d++;
  }

  if (mag < 0.0) {
    if (mag < -10.0) strcpy(magstr,"-99");
    else {
      i = -mag*10 + .5;
      sprintf(magstr,"-%02d", i);
    };
  } else if (mag < 10.0) {
    i = mag*100 + .5;
    sprintf(magstr,"%03d", i);
  } else {
    i = mag*100 + .5;
    i %= 1000;
    sprintf(magstr,"%03d", i);
    magstr[0] = ((int) (mag - 10.0)) + 'A';
  };

  if (size >= 0.0)
    tosizestr(size, sizestr);
  else
    strcpy(sizestr, "  ");

  if (do_star)
    if (comment[0])
      fprintf(o_star, "%02d%02d%02d%c%02d%02d%s%s%s%s%s%s,%s\n",
	      rah, ram, ras, sign ? '-':'+', dld, dlm,
	      magstr,
	      type, color,
	      sizestr,
	      constell, name, comment);
    else
      fprintf(o_star, "%02d%02d%02d%c%02d%02d%s%s%s%s%s%s\n",
	      rah, ram, ras, sign ? '-':'+', dld, dlm,
	      magstr,
	      type, color,
	      sizestr,
	      constell, name);

  if (do_sif)
    fprintf(o_sif, "%2d %2d %2d; %c%02d %2d %d; %.3f;%s;%s;%s;%s;%s;%s\n",
	    rah, ram, ras, sign ? '-':'+', d_d, d_m, d_s,
	    mag,
	    type, color,
	    sizestr,
	    constell, name, comment);
}

void tosizestr(sze, str)
     double sze;
     char str[];
{
  if (sze < 99.5) sprintf(str, "%2d",(int) (sze + 0.5));
  else if (sze < 995.0) {
    sprintf(str, "%2d",(int) ((sze + 5.0)/10.0));
    str[0] = 'A' + str[0] - '0' - 1;
  }  else if (sze < 9950.0) {
    sprintf(str, "%2d",(int) ((sze + 50.0)/100.0));
    str[0] = 'J' + str[0] - '0' - 1;
  }  else if (sze < 89500.0) {
    sprintf(str, "%2d",(int) ((sze + 500.0)/1000.0));
    str[0] = 'S' + str[0] - '0' - 1;
  } else sprintf(str , "Z9");
}
