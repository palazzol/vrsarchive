/*
 * outaltaz.c
 * output altaz file
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
  "$Header: /home/Vince/cvs/net/starchart/observe/outaltaz.c,v 1.1 1990-03-30 16:37:54 vrs Exp $";
#endif


#include <stdio.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include "observe.h"
#include "date.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif


char *malloc();

void altaz();


/* Output altaz file for object(s) */
void out_altaz(o_altaz, one_day, jd,
	       lon, lat, zone, height,
	       sun_data, moon_data, planets, bodies, nbodies, objects, nobjects)
     FILE *o_altaz;
     int one_day;		/* only doing one day */
     double jd;
     double lon, lat, zone, height;
     sun_data_t sun_data;
     moon_data_t moon_data;
     planet_data_t planets[];
     wanderer_data_t bodies[];
     int nbodies;
     obj_data_t objects[];
     int nobjects;
{
  int i;
  char datestr[15];
  double rise_hr, set_hr, eve_twil, morn_twil;
  double alt1, az1, alt2, az2, alt3, az3, alt4, az4;

  jd_to_str(jd, datestr);
  if (!one_day) fprintf(o_altaz, "%s  %.3f\n", datestr, jd);

  rise_hr = sun_data.rise_hour;
  set_hr = sun_data.set_hour;
  morn_twil = morntwil(jd, lon, lat, zone, height, sun_data);
  eve_twil = evetwil(jd, lon, lat, zone, height, sun_data);
  fprintf(o_altaz,
"name       at sun set    eve. twil.   morn. twil.   at sun rise\n");


  altaz(jd, lat, lon, zone, height, sun_data.alpha, sun_data.delta,
	set_hr, &alt1, &az1);
  altaz(jd, lat, lon, zone, height, sun_data.alpha, sun_data.delta,
	eve_twil, &alt2, &az2);
  altaz(jd, lat, lon, zone, height, sun_data.alpha, sun_data.delta,
	morn_twil, &alt3, &az3);
  altaz(jd, lat, lon, zone, height, sun_data.alpha, sun_data.delta,
	rise_hr, &alt4, &az4);
  
  fprintf(o_altaz,
	  "%-7.7s %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f\n",
	  "Sun",
	  alt1, az1,
	  alt2, az2,
	  alt3, az3,
	  alt4, az4
	  );

  for (i = 0; i < 7; i++)
    if (planets[i].name[0]) {
      altaz(jd, lat, lon, zone, height, planets[i].alpha, planets[i].delta,
	    set_hr, &alt1, &az1);
      altaz(jd, lat, lon, zone, height, planets[i].alpha, planets[i].delta,
	    eve_twil, &alt2, &az2);
      altaz(jd, lat, lon, zone, height, planets[i].alpha, planets[i].delta,
	    morn_twil, &alt3, &az3);
      altaz(jd, lat, lon, zone, height, planets[i].alpha, planets[i].delta,
	    rise_hr, &alt4, &az4);
      fprintf(o_altaz,
	      "%-7.7s %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f\n",
	      planets[i].name,
	      alt1, az1,
	      alt2, az2,
	      alt3, az3,
	      alt4, az4
	      );
    };


  for (i = 0; i < nbodies; i++) {
      altaz(jd, lat, lon, zone, height, bodies[i].alpha, bodies[i].delta,
	    set_hr, &alt1, &az1);
      altaz(jd, lat, lon, zone, height, bodies[i].alpha, bodies[i].delta,
	    eve_twil, &alt2, &az2);
      altaz(jd, lat, lon, zone, height, bodies[i].alpha, bodies[i].delta,
	    morn_twil, &alt3, &az3);
      altaz(jd, lat, lon, zone, height, bodies[i].alpha, bodies[i].delta,
	    rise_hr, &alt4, &az4);
      fprintf(o_altaz,
	      "%-7.7s %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f\n",
	      bodies[i].name,
	      alt1, az1,
	      alt2, az2,
	      alt3, az3,
	      alt4, az4
	      );
  };

  for (i = 0; i < nobjects; i++) {
      altaz(jd, lat, lon, zone, height, objects[i].alpha, objects[i].delta,
	    set_hr, &alt1, &az1);
      altaz(jd, lat, lon, zone, height, objects[i].alpha, objects[i].delta,
	    eve_twil, &alt2, &az2);
      altaz(jd, lat, lon, zone, height, objects[i].alpha, objects[i].delta,
	    morn_twil, &alt3, &az3);
      altaz(jd, lat, lon, zone, height, objects[i].alpha, objects[i].delta,
	    rise_hr, &alt4, &az4);
      fprintf(o_altaz,
	      "%-7.7s %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f\n",
	      objects[i].name,
	      alt1, az1,
	      alt2, az2,
	      alt3, az3,
	      alt4, az4
	      );
  };

  if (!one_day) fprintf(o_altaz, "\f\n");
}
