/*
 * outeph.c
 * output ephemeris file
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
  "$Header: /home/Vince/cvs/net/starchart/observe/outeph.c,v 1.1 1990-03-30 16:37:55 vrs Exp $";
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


void raformat(), decformat(), timeformat();
char *malloc();


/* Output ephemeris file for object(s) */
void out_eph(o_eph, one_day, jd,
	     sun_data, moon_data, planets, bodies, nbodies, objects, nobjects)
     FILE *o_eph;
     int one_day;		/* only doing one day */
     double jd;
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
  char rastr1[15], rastr2[15];
  char decstr1[15], decstr2[15];
  char risestr[6], setstr[6], transitstr[6];
  char Delta_str[6], r_str[6];


  jd_to_str(jd, datestr);
  if (!one_day) fprintf(o_eph, "%s  %.3f\n", datestr, jd);


  fprintf(o_eph,
"name   RA2000.0  Dec2000.0  RA epoch  Dec epoch  rise  trans set   mag    size   elong. phase Delta r\n");

  raformat(sun_data.alpha2000/15.0, rastr1);
  raformat(sun_data.alpha/15.0, rastr2);
  decformat(sun_data.delta2000, decstr1);
  decformat(sun_data.delta, decstr2);
  timeformat(sun_data.rise_hour, risestr);
  timeformat(sun_data.transit_hour, transitstr);
  timeformat(sun_data.set_hour, setstr);
  fprintf(o_eph,
	  "%-7.7s %s %s %s %s %s %s %s %5.1f %6.1f              %5.2f\n",
	  "Sun",
	  rastr1, decstr1,
	  rastr2, decstr2,
	  risestr, transitstr, setstr,
	  -26.0, sun_data.size, sun_data.R);

  raformat(moon_data.alpha2000/15.0, rastr1);
  raformat(moon_data.alpha/15.0, rastr2);
  decformat(moon_data.delta2000, decstr1);
  decformat(moon_data.delta, decstr2);
  timeformat(moon_data.rise_hour, risestr);
  timeformat(moon_data.transit_hour, transitstr);
  timeformat(moon_data.set_hour, setstr);
  fprintf(o_eph,
	 "%-7.7s %s %s %s %s %s %s %s %5.1f %6.1f        %5.3f %5.2f\n",
	  "Moon",
	  rastr1, decstr1,
	  rastr2, decstr2,
	  risestr, transitstr, setstr,
	  moon_data.mag, moon_data.size, moon_data.illum_frac,
	  moon_data.Delta/10000.0);

 

  for (i = 0; i < 7; i++)
    if (planets[i].name[0]) {
      raformat(planets[i].alpha2000/15.0, rastr1);
      raformat(planets[i].alpha/15.0, rastr2);
      decformat(planets[i].delta2000, decstr1);
      decformat(planets[i].delta, decstr2);
      timeformat(planets[i].eventlist[0].hour, risestr);
      timeformat(planets[i].eventlist[3].hour, transitstr);
      timeformat(planets[i].eventlist[6].hour, setstr);
      fprintf(o_eph,
        "%-7.7s %s %s %s %s %s %s %s %5.2f %6.1f %6.1f %5.3f %5.2f %5.2f\n",
	      planets[i].name,
	      rastr1, decstr1,
	      rastr2, decstr2,
	      risestr, transitstr, setstr,
	      planets[i].mag, planets[i].size,
	      planets[i].psi, planets[i].illum_frac,
	      planets[i].Delta, planets[i].r);
    };


  for (i = 0; i < nbodies; i++) {
    raformat(bodies[i].alpha2000/15.0, rastr1);
    raformat(bodies[i].alpha/15.0, rastr2);
    decformat(bodies[i].delta2000, decstr1);
    decformat(bodies[i].delta, decstr2);
    timeformat(bodies[i].eventlist[0].hour, risestr);
    timeformat(bodies[i].eventlist[3].hour, transitstr);
    timeformat(bodies[i].eventlist[6].hour, setstr);
    if (bodies[i].Delta < 0.0) {
      strcpy(Delta_str, "-----");
    } else {
      sprintf(Delta_str, "%5.2f", bodies[i].Delta);
    };
    if (bodies[i].r < 0.0) {
      strcpy(r_str, "-----");
    } else {
      sprintf(r_str, "%5.2f", bodies[i].r);
    };

    fprintf(o_eph,
      "%-7.7s %s %s %s %s %s %s %s %5.2f                     %s %s\n",
	    bodies[i].name,
	    rastr1, decstr1,
	    rastr2, decstr2,
	    risestr, transitstr, setstr,
	    bodies[i].mag, Delta_str, r_str);
  };

  for (i = 0; i < nobjects; i++) {
    raformat(objects[i].alpha2000/15.0, rastr1);
    raformat(objects[i].alpha/15.0, rastr2);
    decformat(objects[i].delta2000, decstr1);
    decformat(objects[i].delta, decstr2);
    timeformat(objects[i].rise_hour, risestr);
    timeformat(objects[i].transit_hour, transitstr);
    timeformat(objects[i].set_hour, setstr);
    fprintf(o_eph,
      "%-7.7s %s %s %s %s %s %s %s %5.2f %6.1f\n",
	    objects[i].name,
	    rastr1, decstr1,
	    rastr2, decstr2,
	    risestr, transitstr, setstr, objects[i].mag, objects[i].size);
  };
  if (!one_day) fprintf(o_eph, "\f\n");
}

void raformat(ra, str)
     double ra;
     char str[];
{
  int h, m, s;

  h = ra;
  m = (ra-h)*60;
  s = ((ra-h)*60 - m)*60 + .5;
  if (s == 60) {
    s = 0;
    m++;
  };
  if (m == 60) {
    m = 0;
    h++;
  }
  if (h == 24) h = 0;
  sprintf(str, "%02dh%02dm%02ds", h, m, s);
}

void decformat(dec, str)
     double dec;
     char str[];
{
  int h, m, s;
  char sign_char = ' ';

  if (dec < 0) {
    sign_char = '-';
    dec = -dec;
  };

  h = dec;
  m = (dec-h)*60;
  s = ((dec-h)*60 - m)*60 + .5;
  if (s == 60) {
    s = 0;
    m++;
  };
  if (m == 60) {
    m = 0;
    h++;
  }

  sprintf(str, "%c%02dd%02dm%02ds", sign_char, h, m, s);
}

void timeformat(time, str)
     double time;		/* Hours */
     char str[];
{
  int hrs, mins;

  if (time < 0.0) strcpy(str, "-----");
  else {
    hrs = time;
    mins = (time-hrs)*60.0 + 0.5;
    if (mins == 60)
      {
   	hrs = (hrs == 23) ? 0 :(hrs+1);
   	mins = 0;
      }
    sprintf(str, "%02d:%02d", hrs, mins);
  };
}
