/*
 * satcalc.c
 * jupiter and saturn satellites positions
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
  "$Header: /home/Vince/cvs/net/starchart/observe/satcalc.c,v 1.1 1990-03-30 16:38:01 vrs Exp $";
#endif

#include <math.h>

#include "observe.h"
#include "degree.h"

/* given jd, return sat_t list of gallilean satellites */
void jupsat(jd, jupiter, sats)
     double jd;
     planet_data_t jupiter;
     sat_t sats[4];
{
  double d;			/* modified julian date */
  double V;			/* long period term of Jupiter */
  double M;			/* mean anomaly of earth */
  double N;			/* mean anomaly of Jupiter */
  double J;			/* differance in heliocentric longitude */
  double A;			/* Center of earth */
  double B;			/* Center of jupiter */
  double K;
  double R;			/* Radius vector of earth */
  double r;			/* Radius vector of jupiter */
  double Delta;			/* Distance from earth to jupiter */
  double psi;			/* Phase angle */
  double alpha, delta;		/* Position of jupiter at equinox of date */
  double D_e;			/* == beta_e, planetocentric declination of earth */
  double u_1, u_2, u_3, u_4;	/* angle */
  double G, H;			/* correction terms */
  double cor_u_1, cor_u_2, cor_u_3, cor_u_4; /* Corrections to u_ */
  double r_1, r_2, r_3, r_4;	/* distance to center of Jupiter (radii) */
  double X_1, X_2, X_3, X_4;	/* relative Positions (radii) */
  double Y_1, Y_2, Y_3, Y_4;	/* relative Positions (radii) */
  double Z_1, Z_2, Z_3, Z_4;	/* relative Positions (radii) */
  double X, Y;
  double dmag;

  d = jd - 2415020.0;

  alpha = jupiter.alpha;
  delta = jupiter.delta;

  Delta = jupiter.Delta;
  psi = jupiter.phase;
  B = jupiter.Cen;

  D_e = jupiter.rotation_elements.beta_e;

  u_1 = 84.5506 + 203.4058630 * (d - Delta * 0.00577167643528) + psi - B;
  u_2 = 41.5015 + 101.2916323 * (d - Delta * 0.00577167643528) + psi - B;
  u_3 = 109.9770 + 50.2345169 * (d - Delta * 0.00577167643528) + psi - B;
  u_4 = 176.3586 + 21.4879802 * (d - Delta * 0.00577167643528) + psi - B;

  u_1 = into_range(u_1);
  u_2 = into_range(u_2);
  u_3 = into_range(u_3);
  u_4 = into_range(u_4);


  G = 187.3 + 50.310674 * (d - Delta * 0.00577167643528);
  H = 311.1 + 21.569229 * (d - Delta * 0.00577167643528);
  
  cor_u_1 =  0.472 * DSIN(2*(u_1 - u_2));
  cor_u_2 =  1.073 * DSIN(2*(u_2 - u_3));
  cor_u_3 =  0.174 * DSIN(G);
  cor_u_4 =  0.845 * DSIN(H);
  
  r_1 = 5.9061 - 0.0244 * DCOS(2*(u_1 - u_2));
  r_2 = 9.3972 - 0.0889 * DCOS(2*(u_2 - u_3));
  r_3 = 14.9894 - 0.0227 * DCOS(G);
  r_4 = 26.3649 - 0.1944 * DCOS(H);
  
  X_1 = r_1 * DSIN(u_1 + cor_u_1);
  X_2 = r_2 * DSIN(u_2 + cor_u_2);
  X_3 = r_3 * DSIN(u_3 + cor_u_3);
  X_4 = r_4 * DSIN(u_4 + cor_u_4);

  Z_1 = r_1 * DCOS(u_1 + cor_u_1);
  Z_2 = r_2 * DCOS(u_2 + cor_u_2);
  Z_3 = r_3 * DCOS(u_3 + cor_u_3);
  Z_4 = r_4 * DCOS(u_4 + cor_u_4);
  
  Y_1 = - r_1 * DCOS(u_1 + cor_u_1) * DSIN(D_e);
  Y_2 = - r_2 * DCOS(u_2 + cor_u_2) * DSIN(D_e);
  Y_3 = - r_3 * DCOS(u_3 + cor_u_3) * DSIN(D_e);
  Y_4 = - r_4 * DCOS(u_4 + cor_u_4) * DSIN(D_e);

  dmag = 5.0 * log10(jupiter.r*jupiter.Delta)
    - 2.5 * log10(jupiter.illum_frac);

  sats[0].dx = X_1;
  sats[0].dy = Y_1;
  sats[0].dz = Z_1;
  X = -X_1*jupiter.size/2.0;
  Y = Y_1*jupiter.size/2.0;
  sats[0].dalpha = X * DCOS(jupiter.rotation_elements.p_n)
      + Y * DSIN(jupiter.rotation_elements.p_n);
  sats[0].dalpha /= DCOS(jupiter.delta);
  sats[0].ddelta = -X * DSIN(jupiter.rotation_elements.p_n)
    + Y * DCOS(jupiter.rotation_elements.p_n);
  sats[0].name = "Io";
  sats[0].mag = -1.68 + dmag;
  
  sats[1].dx = X_2;
  sats[1].dy = Y_2;
  sats[1].dz = Z_2;
  X = -X_2*jupiter.size/2.0;
  Y = Y_2*jupiter.size/2.0;
  sats[1].dalpha = X * DCOS(jupiter.rotation_elements.p_n)
      + Y * DSIN(jupiter.rotation_elements.p_n);
  sats[1].dalpha /= DCOS(jupiter.delta);
  sats[1].ddelta = -X * DSIN(jupiter.rotation_elements.p_n)
    + Y * DCOS(jupiter.rotation_elements.p_n);
  sats[1].name = "Europa";
  sats[1].mag = -1.41 + dmag;
  
  sats[2].dx = X_3;
  sats[2].dy = Y_3;
  sats[2].dz = Z_3;
  X = -X_3*jupiter.size/2.0;
  Y = Y_3*jupiter.size/2.0;
  sats[2].dalpha = X * DCOS(jupiter.rotation_elements.p_n)
      + Y * DSIN(jupiter.rotation_elements.p_n);
  sats[2].dalpha /= DCOS(jupiter.delta);
  sats[2].ddelta = -X * DSIN(jupiter.rotation_elements.p_n)
    + Y * DCOS(jupiter.rotation_elements.p_n);
  sats[2].name = "Ganymede";
  sats[2].mag = -2.09 + dmag;
  
  sats[3].dx = X_4;
  sats[3].dy = Y_4;
  sats[3].dz = Z_4;
  X = -X_4*jupiter.size/2.0;
  Y = Y_4*jupiter.size/2.0;
  sats[3].dalpha = X * DCOS(jupiter.rotation_elements.p_n)
      + Y * DSIN(jupiter.rotation_elements.p_n);
  sats[3].dalpha /= DCOS(jupiter.delta);
  sats[3].ddelta = -X * DSIN(jupiter.rotation_elements.p_n)
    + Y * DCOS(jupiter.rotation_elements.p_n);
  sats[3].name = "Callisto";
  sats[3].mag = -1.05 + dmag;
}
/* siderial Periods of satellites:
 0.942421813
 1.370217855
 1.887802160
 2.736914742
 4.517500436
15.94542068
21.2766088
79.3301825

synodic (approx)
 0.94250436287643326
 1.3703923657888438
 1.8881334260185879
 2.7376110810451278
 4.519397869256954
 15.969085530060568
 21.318764097751611
 79.919403771981642

semimajor axes
185.52
238.02
294.66
377.40
527.04
1221.83
1481.1
3561.3
*/

static struct {
  double off; /* angle of satellite at jd 2415020.0 */
  double angvel; /* anbular velocity, degrees per day */
  double r; /* Distance to saturn in saturn radii */
  double mag; /* V(1,0) */
  char *name; /* Name */
} satsat_data[] = {
  {49.0, 381.96109660576467, 3.092, 3.3, "Mimas"},
  {98.7, 262.69848620527883, 3.967, 2.1, "Enceladus"},
  {263.0, 190.66449173515979, 4.911, 0.6, "Tethys"},
  {101.3, 131.50151330574105, 6.290, 0.8, "Dione"},
  {11.2, 79.656629138338852, 8.784, 0.1, "Rhea"},
  {183.7, 22.543557633424146, 20.364, -1.28, "Titan"},
  {95.0, 16.886532368823739, 24.685, 4.63, "Hyperion"},
  {338.4, 4.5045381097576426, 59.355, 1.5, "Iapetus"}
};


/* given jd, return sat_t list of major satellites of Saturn */
/* Ignore many corrections to Saturn's orbit,
   assume moons in circular orbits in Saturn's equatorial plane */
void satsat(jd, saturn, sats)
     double jd;
     planet_data_t saturn;
     sat_t sats[8];
{
  double d;			/* modified julian date */
  double M;			/* mean anomaly of earth */
  double N;			/* mean anomaly of Saturn */
  double J;			/* differance in heliocentric longitude */
  double A;			/* Center of earth */
  double B;			/* Center of saturn */
  double K;
  double R;			/* Radius vector of earth */
  double r;			/* Radius vector of Saturn */
  double Delta;			/* Distance from earth to Saturn */
  double psi;			/* Phase angle */
  double alpha, delta;		/* Position of Saturn at equinox of date */
  double D_e;			/* == beta_e, planetocentric declination of earth */
  double u[8];			/* angle */
  double X[8];			/* relative Positions (radii) */
  double Y[8];			/* relative Positions (radii) */
  double Z[8];			/* relative Positions (radii) */
  double dmag;
  int i;
  double x, y;

  d = jd - 2415020.0;

  alpha = saturn.alpha;
  delta = saturn.delta;

  Delta = saturn.Delta;
  psi = saturn.phase;
  B = saturn.Cen;

  /* Position of Saturn at equinox of date */
  D_e = saturn.rotation_elements.beta_e;

  dmag = 5.0 * log10(saturn.r*saturn.Delta)
    - 2.5 * log10(saturn.illum_frac);

  for (i = 0; i < 8; i++) {
    u[i] = satsat_data[i].off
      + satsat_data[i].angvel * (d - Delta * 0.00577167643528) + psi - B;

    u[i] = into_range(u[i]);

    X[i] = satsat_data[i].r * DSIN(u[i]);
    Z[i] = satsat_data[i].r * DCOS(u[i]);
    Y[i] = - satsat_data[i].r * DCOS(u[i]) * DSIN(D_e);

    sats[i].dx = X[i];
    sats[i].dy = Y[i];
    sats[i].dz = Z[i];
    x = -X[i]*saturn.size/2.0;
    y = Y[i]*saturn.size/2.0;

    sats[i].dalpha = x * DCOS(saturn.rotation_elements.p_n)
      + y * DSIN(saturn.rotation_elements.p_n);
    sats[i].dalpha /= DCOS(saturn.delta);
    sats[i].ddelta = -x * DSIN(saturn.rotation_elements.p_n)
      + y * DCOS(saturn.rotation_elements.p_n);
    sats[i].name = satsat_data[i].name;
    sats[i].mag = satsat_data[i].mag + dmag;
/*printf("%d: %f %f\n", i, u[i], into_range(270 - u[i]));*/
  };
}
  
  
  
  
