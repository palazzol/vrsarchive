/*
 * riseset.c
 * calculate rise and set times of objects, sun, moon
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
  "$Header: /home/Vince/cvs/net/starchart/observe/riseset.c,v 1.1 1990-03-30 16:38:00 vrs Exp $";
#endif

#include <math.h>

#include "observe.h"
#include "degree.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

double gmst0_degrees();

double risetime(rise, jd, alt, lat, lon, zone, height, alpha, delta)
     int rise;
     double jd;
     double alt;
     double lat, lon, zone, height;
     double alpha, delta;
{
  double H_0;
  double jd0;
  double cos_term;
  double hour;

  jd0 = ((long) (jd + 0.5001)) - .5; /* round to nearest midnight */

  cos_term = (DSIN(alt) - DSIN(lat) * DSIN(delta))/(DCOS(lat)*DCOS(delta));

  if (cos_term < -1.0)		/* Never rises or sets */
    return -1.0;
  if (cos_term > 1.0)		/* Never rises or sets */
    return -1.0;

  H_0 = RAD_TO_DEG * acos(cos_term);

  if (rise) H_0 *= -1.0;

  hour = (H_0 + alpha - gmst0_degrees(jd0) - lon) / 1.00273790934 + zone*15.0;

  hour = into_range(hour);

  return (hour/15.0);
}

double sunrise(jd, lon, lat, zone, height, sun_data)
     double jd;
     double lon, lat, zone, height;
     sun_data_t sun_data;
{
  return risetime(TRUE, jd, -0.566666667, lat, lon, zone, height,
		  sun_data.alpha, sun_data.delta);
}

double suntransit(jd, lon, lat, zone, height, sun_data)
     double jd;
     double lon, lat, zone, height;
     sun_data_t sun_data;
{
  double hour;
  double jd0;


  jd0 = ((long) (jd + 0.5001)) - .5; /* round to nearest midnight */

  hour = (sun_data.alpha - gmst0_degrees(jd0) - lon) / 1.00273790934
    + zone*15.0;
  hour = into_range(hour);

  return (hour/15.0);
}

double sunset(jd, lon, lat, zone, height, sun_data)
     double jd;
     double lon, lat, zone, height;
     sun_data_t sun_data;
{
  return risetime(FALSE, jd, -0.566666667, lat, lon, zone, height,
		  sun_data.alpha, sun_data.delta);
}


double morntwil(jd, lon, lat, zone, height, sun_data)
     double jd;
     double lon, lat, zone, height;
     sun_data_t sun_data;
{
  return risetime(TRUE, jd, -18.0, lat, lon, zone, height,
		  sun_data.alpha, sun_data.delta);
}


double evetwil(jd, lon, lat, zone, height, sun_data)
     double jd;
     double lon, lat, zone, height;
     sun_data_t sun_data;
{
  return risetime(FALSE, jd, -18.0, lat, lon, zone, height,
		  sun_data.alpha, sun_data.delta);
}


double moonrise(jd, lon, lat, zone, height, moon_data)
     double jd;
     double lon, lat, zone, height;
     moon_data_t moon_data;
{
  return risetime(TRUE, jd, -0.566666667, lat, lon, zone, height,
		  moon_data.alpha, moon_data.delta);
}

double moontransit(jd, lon, lat, zone, height, moon_data)
     double jd;
     double lon, lat, zone, height;
     moon_data_t moon_data;
{
  double hour;
  double jd0;


  jd0 = ((long) (jd + 0.5001)) - .5; /* round to nearest midnight */

  hour = (moon_data.alpha - gmst0_degrees(jd0) - lon) / 1.00273790934
    + zone*15.0;
  hour = into_range(hour);

  return (hour/15.0);
}

double moonset(jd, lon, lat, zone, height, moon_data)
     double jd;
     double lon, lat, zone, height;
     moon_data_t moon_data;
{
  return risetime(FALSE, jd, -0.566666667, lat, lon, zone, height,
		  moon_data.alpha, moon_data.delta);
}

double objrise(jd, lon, lat, zone, height, alpha, delta)
     double jd, lon, lat, zone, height, alpha, delta;
{
  return risetime(TRUE, jd, -0.566666667, lat, lon, zone, height,
		  alpha, delta);
}

double objrise20(jd, lon, lat, zone, height, alpha, delta)
     double jd, lon, lat, zone, height, alpha, delta;
{
  return risetime(TRUE, jd, 20.0, lat, lon, zone, height, alpha, delta);
}

double objrise30(jd, lon, lat, zone, height, alpha, delta)
     double jd, lon, lat, zone, height, alpha, delta;
{
  return risetime(TRUE, jd, 30.0, lat, lon, zone, height, alpha, delta);
}

double objtransit(jd, lon, lat, zone, height, alpha, delta)
     double jd, lon, lat, zone, height, alpha, delta;
{
  double hour;
  double jd0;


  jd0 = ((long) (jd + 0.5001)) - .5; /* round to nearest midnight */

  hour = (alpha - gmst0_degrees(jd0) - lon) / 1.00273790934 + zone*15.0;

  hour = into_range(hour);

  return (hour/15.0);
}

double objset30(jd, lon, lat, zone, height, alpha, delta)
     double jd, lon, lat, zone, height, alpha, delta;
{
  return risetime(FALSE, jd, 30.0, lat, lon, zone, height, alpha, delta);
}

double objset20(jd, lon, lat, zone, height, alpha, delta)
     double jd, lon, lat, zone, height, alpha, delta;
{
  return risetime(FALSE, jd, 20.0, lat, lon, zone, height, alpha, delta);
}

double objset(jd, lon, lat, zone, height, alpha, delta)
     double jd, lon, lat, zone, height, alpha, delta;
{
  return risetime(FALSE, jd, -0.566666667, lat, lon, zone, height,
		  alpha, delta);
}


