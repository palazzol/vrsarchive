/*
 * outobs.c
 * Output observability file for object(s)
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
  "$Header: /home/Vince/cvs/net/starchart/observe/outobs.c,v 1.1 1990-03-30 16:37:56 vrs Exp $";
#endif


#include <stdio.h>
#include "observe.h"
#include "date.h"

static char *ret_timeformat();
static char *ret_eventformat();

/* Output observability file for object(s) */
void out_obs(outf, one_day, jd, events, index, nevents)
     FILE *outf;
     int one_day;		/* only doing one day */
     double jd;
     observe_t events[];	/* The events */
     int index[];		/* sorted into order */
     int nevents;
{
  int i;
  char datestr[15];
  char *special_cp;

  jd_to_str(jd, datestr);
  if (!one_day) fprintf(outf, "%s %.3f\n", datestr, jd);
  for (i = 0; i < nevents; i++) {
    switch (events[index[i]].special) {
    case rise_special:
      special_cp = "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
      break;
    case morning_twilight:
      special_cp = "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
      break;
    case set_special:
      special_cp = "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv";
      break;
    case evening_twilight:
      special_cp = "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv";
      break;
    default:
      special_cp = "";
      break;
    };

    fprintf(outf,
	    "%-20.20s %8s %8s %s\n",
	    events[index[i]].object, ret_timeformat(events[index[i]].hour),
	    ret_eventformat(events[index[i]].event), special_cp);
  };
  if (!one_day) fprintf(outf, "\f\n");
}

static char *ret_timeformat(time)
     double time;		/* Hours */
{
  static char timestring[8];
  int hrs, mins;

  if (time < 0.0) return ("-----");

  hrs = time;
  mins = (time-hrs)*60.0 + 0.5;
  if (mins == 60)
    {
      hrs = (hrs == 23) ? 0 :(hrs+1);
      mins = 0;
    }
  sprintf(timestring, "%02d:%02d", hrs, mins);

  return timestring;
}

static char *ret_eventformat(event)
     event_t event;
{
  switch (event) {
  case rise:
    return "rise    ";
    break;

  case rise_20:
    return "above 20";
    break;
    
  case rise_30:
    return "above 30";
    break;
    
  case set:
    return "set     ";
    break;

  case set_20:
    return "below 20";
    break;

  case set_30:
    return "below 30";
    break;

  case transit:
    return "transit ";
    break;

  default:
    return "        ";
    break;
  };
}

