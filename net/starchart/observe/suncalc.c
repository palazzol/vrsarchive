/*
 * suncalc.c
 * Sun position calculations
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
  "$Header: /home/Vince/cvs/net/starchart/observe/suncalc.c,v 1.1 1990-03-30 16:38:03 vrs Exp $";
#endif


#include <math.h>
#include "observe.h"
#include "degree.h"

void sun_pos(jd, sun_data)
     double jd;
     sun_data_t *sun_data;
{
  double L, M, Theta, R;
  double A, B, C, D, E, H;
  double e, nu;
  double T;
  double epsilon;
  double alpha, delta, alpha2000, delta2000;

  T =  (jd - 2415020.0)/36525.0;

  M = into_range(358.47583 + 35999.04975*T - 0.000150*T*T - 0.0000033*T*T*T);
  e = 0.01675104 - 0.0000418*T - 0.000000126*T*T;
  L = into_range(279.69668 + 36000.76892*T + 0.0003025*T*T);
  anom_calc(M, e, &E, &nu);
  Theta = into_range(L + nu - M);
  R = 1.0000002*(1.0 - e * DCOS(E));

  /* More accurate */
  A = into_range(153.23 + 22518.7541*T);
  B = into_range(216.57 + 45037.508184*T);
  C = into_range(312.69 + 32964.3577008*T);
  D = into_range(350.74 + 445267.1142*T - 0.00144*T*T);
  E = into_range(231.19 + 20.2*T);
  H = into_range(353.4  + 65928.715488*T);

  Theta += 0.00134*DCOS(A)+0.00154*DCOS(B)+.002*DCOS(C)
    +.00179*DSIN(D)+ .00178*DSIN(E);
  R += 5.43e-06*DSIN(A)+1.575e-05*DSIN(B)+1.627e-05*DSIN(C)+
    3.076e-05*DCOS(D)+9.27e-06*DSIN(H);

  sun_data->size = 1919.26/R;
  sun_data->R = R;
  sun_data->Theta = Theta;
  epsilon = 23.439291 - 0.0130042* T - 0.00000016* T*T - 0.000000504* T*T*T;
  alpha = sun_data->alpha
    = into_range(RAD_TO_DEG * atan2(DCOS(epsilon)*DSIN(Theta), DCOS(Theta)));
  delta = sun_data->delta = 
    RAD_TO_DEG * asin(DSIN(epsilon)*DSIN(Theta));
  precess(2000.0 - (2451545.0 - jd) / 365.25,
	  2000.0, alpha, delta, &alpha2000, &delta2000);

  sun_data->alpha2000 = alpha2000;
  sun_data->delta2000 = delta2000;
}

/*
  alpha = RAD_TO_DEG * atan2(DCOS(epsilon)*DSIN(Theta), DCOS(Theta));
  delta = RAD_TO_DEG * asin(DSIN(epsilon)*DSIN(Theta));
*/

void sun_rect(T, equinox_year, R, Theta, X_p, Y_p, Z_p)
double T, R, Theta, equinox_year;
double *X_p, *Y_p, *Z_p;
{
  double epsilon;
  double T_eq;
  double X_0, Y_0, Z_0;
  double X_x, Y_x, Z_x;
  double X_y, Y_y, Z_y;
  double X_z, Y_z, Z_z;
  double zeta, Z, theta;
  double zeta_A_1, Z_A_1, theta_A_1;
  double zeta_A_2, Z_A_2, theta_A_2;


  epsilon = obl_jd(T);
  X_0 = R * DCOS(Theta);
  Y_0 = R * DSIN(Theta) * DCOS(epsilon);
  Z_0 = R * DSIN(Theta) * DSIN(epsilon);

  /* Now precess from T to equinox */
  T_eq = (T - 2451545.0)/36525.0;
  zeta_A_1= 0.6406161* T_eq + 0.0000839* T_eq*T_eq + 0.0000050* T_eq*T_eq*T_eq;
  Z_A_1   = 0.6406161* T_eq + 0.0003041* T_eq*T_eq + 0.0000051* T_eq*T_eq*T_eq;
  theta_A_1=0.5567530* T_eq - 0.0001185* T_eq*T_eq + 0.0000116* T_eq*T_eq*T_eq;

  T_eq = (equinox_year - 2000.0)/100.0;
  zeta_A_2= 0.6406161* T_eq + 0.0000839* T_eq*T_eq + 0.0000050* T_eq*T_eq*T_eq;
  Z_A_2   = 0.6406161* T_eq + 0.0003041* T_eq*T_eq + 0.0000051* T_eq*T_eq*T_eq;
  theta_A_2=0.5567530* T_eq - 0.0001185* T_eq*T_eq + 0.0000116* T_eq*T_eq*T_eq;

  zeta = zeta_A_2 - zeta_A_1;
  Z = Z_A_2 - Z_A_1;
  theta = theta_A_2 - theta_A_1;



  X_x = DCOS(zeta) * DCOS(Z) * DCOS(theta) - DSIN(zeta) * DSIN(zeta);
  X_y = DSIN(zeta) * DCOS(Z) + DCOS(zeta) * DSIN(Z) * DCOS(theta);
  X_z = DCOS(zeta) * DSIN(theta);

  Y_x = -DCOS(zeta) * DSIN(Z) - DSIN(zeta) * DCOS(Z) * DCOS(theta);
  Y_y =  DCOS(zeta) * DCOS(Z) - DSIN(zeta) * DSIN(Z) * DCOS(theta);
  Y_z = -DSIN(zeta) * DSIN(theta);

  Z_x = -DCOS(Z) * DSIN(theta);
  Z_y = -DSIN(Z) * DSIN(theta);
  Z_z = DCOS(theta);


  *X_p = X_x*X_0 + Y_x * Y_0 + Z_x * Z_0;
  *Y_p = X_y*X_0 + Y_y * Y_0 + Z_y * Z_0;
  *Z_p = X_z*X_0 + Y_z * Y_0 + Z_z * Z_0;
}

