/*
 * astlib.c
 * misc functions, including precession
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
  "$Header: /home/Vince/cvs/net/starchart/observe/astlib.c,v 1.1 1990-03-30 16:37:43 vrs Exp $";
#endif


#include <math.h>
#include "degree.h"

/* Rigorous precession */
/* From Astronomical Ephemeris 1989, p. B18 */
/*
from t_0 to t:

A = 
sin(alpha - z_A) cos(delta) = sin(alpha_0 + zeta_A) cos(delta_0);
B =
cos(alpha - z_A) cos(delta) = cos(alpha_0 + zeta_A) cos(theta_A) cos(delta_0)
				- sin(theta_A) sin(delta_0);
C =
                 sin(delta) = cos(alpha_0 + zeta_A) sin(theta_A) cos(delta_0)
				+ cos(theta_A) sin(delta_0);

delta = asin(C);
alpha = atan2(A/B) + z_A;



from t to t_0:

A =
sin(alpha_0 + zeta_A) cos(delta_0) = sin(alpha - z_A) cos(delta);

B =
cos(alpha_0 + zeta_A) cos(delta_0) = cos(alpha - z_A) cos(theta_A) cos(delta)
				+ sin(theta_A) sin(delta);
C =
                      sin(delta_0) = -cos(alpha - z_A) sin(theta_A) cos(delta)
				+ cos(theta_A) sin(delta)

delta_0 = asin(C);
alpha_0 = atan2(A,B) - zeta_A;
*/



/* For reduction with respect to the standard epoch t_0 = J2000.0
zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T
   Z_A  = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T
theta_A = 0.5567530* T - 0.0001185* T*T + 0.0000116* T*T*T

in degrees.

T = (jd - 2451545.0)/36525.0;

alpha2000 = alpha_0;
delta2000 = delta_0;
*/


void precess(from_equinox, to_equinox,
	     alpha_in, delta_in, alpha_out, delta_out)
     double from_equinox, to_equinox,
       alpha_in, delta_in, *alpha_out, *delta_out;
{
  double zeta_A, z_A, theta_A;
  double T;
  double A, B, C;
  double alpha, delta;
  double alpha2000, delta2000;
  double into_range();


  /* From from_equinox to 2000.0 */
  if (from_equinox != 2000.0) {
    T = (from_equinox - 2000.0)/100.0;
    zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T;
    z_A     = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T;
    theta_A = 0.5567530* T - 0.0001185* T*T + 0.0000116* T*T*T;

    A = DSIN(alpha_in - z_A) * DCOS(delta_in);
    B = DCOS(alpha_in - z_A) * DCOS(theta_A) * DCOS(delta_in)
      + DSIN(theta_A) * DSIN(delta_in);
    C = -DCOS(alpha_in - z_A) * DSIN(theta_A) * DCOS(delta_in)
      + DCOS(theta_A) * DSIN(delta_in);

    alpha2000 = into_range(DATAN2(A,B) - zeta_A);
    delta2000 = DASIN(C);
  } else {
    /* should get the same answer, but this could improve accruacy */
    alpha2000 = alpha_in;
    delta2000 = delta_in;
  };


  /* From 2000.0 to to_equinox */
  if (to_equinox != 2000.0) {
    T = (to_equinox - 2000.0)/100.0;
    zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T;
    z_A     = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T;
    theta_A = 0.5567530* T - 0.0001185* T*T + 0.0000116* T*T*T;

    A = DSIN(alpha2000 + zeta_A) * DCOS(delta2000);
    B = DCOS(alpha2000 + zeta_A) * DCOS(theta_A) * DCOS(delta2000)
      - DSIN(theta_A) * DSIN(delta2000);
    C = DCOS(alpha2000 + zeta_A) * DSIN(theta_A) * DCOS(delta2000)
      + DCOS(theta_A) * DSIN(delta2000);

    alpha = into_range(DATAN2(A,B) + z_A);
    delta = DASIN(C);
  } else {
    /* should get the same answer, but this could improve accruacy */
    alpha = alpha2000;
    delta = delta2000;
  };

  *alpha_out = alpha;
  *delta_out = delta;
}


/* Obliquity epsilon 
epsilon = 23.439291 - 0.0130042* T - 0.00000016* T*T - 0.000000504* T*T*T;
*/
double obl_jd(jd)
double jd;
{
  double T = (jd - 2451545.0)/36525.0;

  return (23.439291 - 0.0130042* T - 0.00000016* T*T - 0.000000504* T*T*T);
}

double obl_year(year)
double year;
{
  double T = (year - 2000.0)/100.0;

  return (23.439291 - 0.0130042* T - 0.00000016* T*T - 0.000000504* T*T*T);
}

void anom_calc(M, e, E_p, nu_p)
     double M, e, *E_p, *nu_p;
{
  double corr, e_0, E_0, E_1;

  e_0 = e * RAD_TO_DEG;

  corr = 1;
  E_0 = M;
  while (corr > 0.000001) {
    corr = (M + e_0 * DSIN(E_0) - E_0)/(1 - e * DCOS(E_0));
    E_1 = E_0 + corr;
    if (corr < 0) corr *= -1.0;
    E_0 = E_1;
  };
	
  *E_p = E_1;

  *nu_p = 2.0 * RAD_TO_DEG * atan(sqrt((1+e)/(1-e))*DTAN(E_1/2));
}


/* 
GMST = 24110.54841 + 8640184.812866 * T_u + 0.093104 *T_u*T_u - 6.2e-6*T_u*T_u*T_u

T_u = (jd - 2451545.0)/36525.0

1.00273790934
GAST = apparent siderial time
 = GMST + (total nutation in longitude)cos epsilon
*/
double gmst0_degrees(jd)
     double jd;
{
  double T;
  double secs;
  double into_range();

  T = (jd - 2451545.0)/36525.0;
  
  secs = 24110.54841 + 8640184.812866 * T + 0.093104 *T*T - 6.2e-6*T*T*T;

  return into_range(secs/240.0);
}


double into_range(ang)
     double ang;
{
  long i;

  while (ang < 0.0) ang += 360.0;
  /* Shouldn't be more than once */

  i = (long)floor(ang/360.0);

  ang = ang - i * 360;

  return(ang);
}

void altaz(jd, lat, lon, zone, height, alpha, delta, hour, alt, azi)
     double jd;
     double lat, lon, zone, height;
     double alpha, delta;
     double hour;
     double *alt, *azi;
{
  double jd0;
  double ha;

  jd0 = ((long) (jd + 0.5001)) - .5; /* round to nearest midnight */

  ha = gmst0_degrees(jd0) + (hour*15.0 - zone*15.0) * 1.00273790934
    + lon - alpha;

  *alt = DASIN(DSIN(delta)*DSIN(lat) + DCOS(delta)*DCOS(lat)*DCOS(ha));
  *azi = DATAN2(-DCOS(delta)*DSIN(ha),
	       DSIN(delta)*DCOS(lat) - DCOS(delta)*DCOS(ha)*DSIN(lat));
  *azi = (*azi < 0.0 ? 360 + *azi: *azi);
}
