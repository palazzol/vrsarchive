/*
 * eventcalc.c
 * calculate rise, set, and transit times for event structure for observability
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
  "$Header: /home/Vince/cvs/net/starchart/observe/eventcalc.c,v 1.1 1990-03-30 16:37:48 vrs Exp $";
#endif


#include <stdio.h>
#include <math.h>
#include "observe.h"
#include "degree.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif


void calc_events(eventlist, rise_hour, transit_hour, set_hour, jd,
		 obs_lon, obs_lat, obs_zone, obs_height, name, alpha, delta)
     observe_t eventlist[];	/* events: rise, set, etc. */
     double *rise_hour, *transit_hour, *set_hour;
     double jd;
     double obs_lon, obs_lat, obs_zone, obs_height;
     char *name;
     double alpha, delta;
{
  int i;

  for (i = 0; i < MAXEVENTS; i++) {
    eventlist[i].special = not_special;
    eventlist[i].object = name;
  }
  *rise_hour = 
    eventlist[0].hour = objrise(jd, obs_lon, obs_lat, obs_zone, obs_height,
				alpha, delta);
  eventlist[0].event = rise;
  eventlist[1].hour = objrise20(jd, obs_lon, obs_lat, obs_zone, obs_height,
				alpha, delta);
  eventlist[1].event = rise_20;
  eventlist[2].hour = objrise30(jd, obs_lon, obs_lat, obs_zone, obs_height,
				alpha, delta);
  eventlist[2].event = rise_30;
  *transit_hour =
    eventlist[3].hour = objtransit(jd, obs_lon, obs_lat, obs_zone, obs_height,
				   alpha, delta);
  eventlist[3].event = transit;
  eventlist[4].hour = objset30(jd, obs_lon, obs_lat, obs_zone, obs_height,
			       alpha, delta);
  eventlist[4].event = set_30;
  eventlist[5].hour = objset20(jd, obs_lon, obs_lat, obs_zone, obs_height,
			       alpha, delta);
  eventlist[5].event = set_20;
  *set_hour =
    eventlist[6].hour = objset(jd, obs_lon, obs_lat, obs_zone, obs_height,
			     alpha, delta);
  eventlist[6].event = set;
}

void add_events(events, nevents_p, eventlist)
     observe_t events[];	/* events: rise, set, etc. */
     int *nevents_p;
     observe_t eventlist[];	/* events: rise, set, etc. */
{
  int i;

  for (i = 0;  i < MAXEVENTS; i++)
   if (eventlist[i].hour >= 0.0) {
     events[*nevents_p].hour = eventlist[i].hour;
     events[*nevents_p].object = eventlist[i].object;
     events[*nevents_p].event = eventlist[i].event;
     events[*nevents_p].special = eventlist[i].special;
     *nevents_p += 1;
   }
}
