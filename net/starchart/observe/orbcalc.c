/*
 * orbcalc.c
 * calculate positions of body in elliptical or parabolic orbit
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
  "$Header: /home/Vince/cvs/net/starchart/observe/orbcalc.c,v 1.1 1990-03-30 16:37:53 vrs Exp $";
#endif


#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include "observe.h"
#include "degree.h"
#include "date.h"

extern char *malloc();


/* example 
Format 
skip  three lines, then for each asteroid
Name i Omega omega a n e M Epoch_date Equinox_year type G H
Epoch_date must be Month day year
Equinox_year, e.g. 2000.0

if type is comet, H and G are g and kappa

Vesta 7.139 104.015 150.175 2.3613 0.27163 0.0906 43.314 Oct 1 1989 2000.0 Asteroid 3.16 0.34

*/

void read_elliptical(infile, bodies, nbodies_p, maxbodies)
     FILE *infile;
     wanderer_data_t bodies[];
     int *nbodies_p;
     int maxbodies;
{
  char in_line[1000], *linep;
  char name[100], H[100], G[100], i[100], Omega[100], omega[100],
      a[100], n[100], body_type[100],
      e[100], M[100], Month[100], day[100], year[100], equinox_year[100];
  char epoch_date_str[300];
  double epoch_date;
  int j, nscanned;
  char *cp;

  fgets(in_line, 1000, infile);
  fgets(in_line, 1000, infile);
  fgets(in_line, 1000, infile);

  
  j = 0;
  while ((j < maxbodies) && (!feof(infile))) {
    linep = fgets(in_line, 1000, infile);
    if (linep != NULL)
      nscanned = sscanf(linep, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
		 name, i, Omega, omega, a, n,
		 e, M, Month, day, year, equinox_year, body_type, H, G);
    else nscanned = 0;
    if (nscanned < 15) continue;

    sprintf(epoch_date_str, "%s %s %s", Month, day, year);
    str_to_jd(epoch_date_str, 0, &epoch_date);


    cp = name;
    while (*cp) {
      if (*cp == '_') *cp = ' ';
      cp++;
    };

    bodies[j].name = (char *) malloc((unsigned) (strlen(name)+1));
    if (bodies[j].name != NULL) strcpy(bodies[j].name, name);
    else bodies[j].name = "";

    bodies[j].orbit_type = elliptical_orbit;
    bodies[j].elliptical_elements.epoch_jd = epoch_date;
    bodies[j].elliptical_elements.equinox_year = atof(equinox_year);
    bodies[j].elliptical_elements.a = atof(a);
    bodies[j].elliptical_elements.e = atof(e);
    bodies[j].elliptical_elements.i = atof(i);
    bodies[j].elliptical_elements.Omega = atof(Omega);
    bodies[j].elliptical_elements.omega = atof(omega);
    bodies[j].elliptical_elements.n = atof(n);
    bodies[j].elliptical_elements.M = atof(M);
    if ((body_type[0] == 'c') || (body_type[0] == 'C')) {
      bodies[j].body_type = comet;
      bodies[j].g = atof(G);
      bodies[j].kappa = atof(H);
    } else {
      bodies[j].body_type = asteroid;
      bodies[j].H = atof(H);
      bodies[j].G = atof(G);
    };

    j++;
  };
  *nbodies_p = j;
}


void elliptical_pos(T, sun_data, data)
     double T;			/* time, jd */
     sun_data_t sun_data;
     wanderer_data_t *data;
{
  double epsilon;
  double a_;			/* semimajor axis, A.U. */
  double e_;			/* eccentricity */
  double i_;			/* inclination (degrees) */
  double omega_;		/* argument of perihelion */
  double Omega_;		/* longitude of ascending node */
  double n_;			/* mean motion (degrees/day) */
  double M_;			/* mean anomaly at epoch */

  double a, b, c, A, B, C;
  double F, G, H, P, Q, R;
  double M, E, nu, r;
  double x, y, z, X, Y, Z;
  double Delta;
  double Psi_1, Psi_2, psi_t; /* For asteroid mag. calculation */
  double alpha, delta, alpha2000, delta2000;

  a_ = data->elliptical_elements.a;
  e_ = data->elliptical_elements.e;
  i_ = data->elliptical_elements.i;
  omega_ = data->elliptical_elements.omega;
  Omega_ = data->elliptical_elements.Omega;
  n_ = data->elliptical_elements.n;
  M_ = data->elliptical_elements.M;


  epsilon = obl_year(data->elliptical_elements.equinox_year);
  F = DCOS(Omega_);
  G = DSIN(Omega_)*DCOS(epsilon);
  H = DSIN(Omega_)*DSIN(epsilon);
  P = -DSIN(Omega_)*DCOS(i_);
  Q = DCOS(Omega_)*DCOS(i_)*DCOS(epsilon) - DSIN(i_)*DSIN(epsilon);
  R = DCOS(Omega_)*DCOS(i_)*DSIN(epsilon) + DSIN(i_)*DCOS(epsilon);

  A = atan2(F,P)*RAD_TO_DEG;
  B = atan2(G,Q)*RAD_TO_DEG;
  C = atan2(H,R)*RAD_TO_DEG;

    
  a = sqrt(F*F + P*P);
  b = sqrt(G*G + Q*Q);
  c = sqrt(H*H + R*R);

  /* Calculate M, E, nu, r */
  M = into_range(M_ + n_ * (T - data->elliptical_elements.epoch_jd));
  anom_calc(M, e_, &E, &nu);
  r = a_ * (1 - e_ * DCOS(E));

  x = r * a * DSIN(A + omega_ + nu);
  y = r * b * DSIN(B + omega_ + nu);
  z = r * c * DSIN(C + omega_ + nu);

  sun_rect(T, data->elliptical_elements.equinox_year,
	   sun_data.R, sun_data.Theta, &X, &Y, &Z);


  Delta = sqrt((X+x)*(X+x) + (Y+y)*(Y+y) + (Z+z)*(Z+z));
  R = sqrt(X*X + Y*Y + Z*Z);


  data->alpha = atan2(Y+y, X+x)*RAD_TO_DEG;
  if (data->alpha < 0) data->alpha += 360.0;
  data->delta = asin((Z+z)/Delta)*RAD_TO_DEG;
  data->r = r;
  data->Delta = Delta;

  if (data->elliptical_elements.equinox_year != 2000.0) {
    alpha = data->alpha;
    delta = data->delta;
    precess(data->elliptical_elements.equinox_year, 2000.0,
	    alpha, delta, &alpha2000, &delta2000);
    data->alpha2000 = alpha2000;
    data->delta2000 = delta2000;
  } else {
    data->alpha2000 = data->alpha;
    data->delta2000 = data->delta;
  };
/* Can't set l, b, lambda */

  data->psi = acos((R*R + Delta*Delta - r*r)/ (2*R*Delta))*RAD_TO_DEG;
  data->beta = acos((r*r + Delta*Delta - R*R)/ (2*r*Delta))*RAD_TO_DEG;

  if (data->body_type == asteroid) {
    G = data->G;
    H = data->H;
    psi_t = exp(log(DTAN(data->beta/2.0))*0.63);
    Psi_1 = exp(-3.33*psi_t);
    psi_t = exp(log(DTAN(data->beta/2.0))*1.22);
    Psi_2 = exp(-1.87*psi_t);
    data->mag = 5.0 * log10(r*Delta) + H
      - 2.5* log10((1-G)*Psi_1 + G*Psi_2);
  } else if (data->body_type == comet) {
    data->mag = data->g + 5.0 * log10(Delta)
      + 2.5 * data->kappa * log10(r);
  } else {
    data->mag = 1.0;
  }
}




/* example 
Format 
skip  three lines, then for each asteroid
Name i Omega omega q Epoch_date Equinox_year g kappa
Epoch_date must be Month day year
Equinox_year, e.g. 2000.0

Borsen-metcalf 7.139 104.015 150.175 0.4 Oct 1 1989 2000.0 3.16 0.34

*/


void read_parabolic(infile, bodies, nbodies_p, maxbodies)
     FILE *infile;
     wanderer_data_t bodies[];
     int *nbodies_p;
     int maxbodies;
{
  char in_line[1000], *linep;
  char name[100], g[100], kappa[100], i[100], Omega[100], omega[100],
      q[100], Month[100], day[100], year[100], equinox_year[100];
  char perihelion_date_str[300];
  double perihelion_date;
  int j, nscanned;

  fgets(in_line, 1000, infile);
  fgets(in_line, 1000, infile);
  fgets(in_line, 1000, infile);

  
  j = 0;
  while ((j < maxbodies) && (!feof(infile))) {
    linep = fgets(in_line, 1000, infile);
    if (linep != NULL)
      nscanned = sscanf(linep, "%s %s %s %s %s %s %s %s %s %s %s",
		 name, i, Omega, omega, q, Month, day, year, equinox_year,
			g, kappa);
    else nscanned = 0;
    if (nscanned < 11) continue;

    sprintf(perihelion_date_str, "%s %s %s", Month, day, year);
    str_to_jd(perihelion_date_str, 0, &perihelion_date);

    bodies[j].name = (char *) malloc((unsigned) (strlen(name)+1));
    if (bodies[j].name != NULL) strcpy(bodies[j].name, name);
    else bodies[j].name = "";

    bodies[j].orbit_type = parabolic_orbit;
    bodies[j].body_type = comet;
    bodies[j].parabolic_elements.perihelion_date = perihelion_date;
    bodies[j].parabolic_elements.equinox_year = atof(equinox_year);
    bodies[j].parabolic_elements.q = atof(q);
    bodies[j].parabolic_elements.i = atof(i);
    bodies[j].parabolic_elements.Omega = atof(Omega);
    bodies[j].parabolic_elements.omega = atof(omega);
    bodies[j].g = atof(g);
    bodies[j].kappa = atof(kappa);

    j++;
  };
  *nbodies_p = j;
}


void parabolic_pos(T, sun_data, data)
     double T;			/* time, jd */
     sun_data_t sun_data;
     wanderer_data_t *data;
{
  double epsilon;
  double q_;			/* perihelion distance, A.U. */
  double i_;			/* inclination (degrees) */
  double omega_;		/* argument of perihelion */
  double Omega_;		/* longitude of ascending node */
  double T_0;			/* Time of passage in perihelion */

  double W, s, gamma, gamma_1, beta;
  double nu, r;
  double A, B, C, a, b, c;
  double F, G, H, P, Q, R;
  double x, y, z, X, Y, Z;
  double Delta;
  double Psi_1, Psi_2, psi_t; /* For asteroid mag. calculation */
  double alpha, delta, alpha2000, delta2000;

  q_ = data->parabolic_elements.q;
  i_ = data->parabolic_elements.i;
  omega_ = data->parabolic_elements.omega;
  Omega_ = data->parabolic_elements.Omega;
  T_0 = data->parabolic_elements.perihelion_date;

  W = 0.0364911624 * (T - T_0)/(q_ * sqrt(q_));
  beta = atan2(2.0,W); /* note, beta in radians */
  gamma_1 = tan(beta/2.0);
  if (gamma_1 < 0) {
    gamma = atan(-exp(log(-gamma_1)/3.0));
  } else {
    gamma = atan(exp(log(gamma_1)/3.0));
  };
  s = 2.0/tan(2*gamma);

  nu = 2.0 * DATAN(s);

  r = q_ * (1 + s*s);

  epsilon = obl_year(data->parabolic_elements.equinox_year);
  F = DCOS(Omega_);
  G = DSIN(Omega_)*DCOS(epsilon);
  H = DSIN(Omega_)*DSIN(epsilon);

  P = -DSIN(Omega_)*DCOS(i_);
  Q = DCOS(Omega_)*DCOS(i_)*DCOS(epsilon) - DSIN(i_)*DSIN(epsilon);
  R = DCOS(Omega_)*DCOS(i_)*DSIN(epsilon) + DSIN(i_)*DCOS(epsilon);

  A = atan2(F,P)*RAD_TO_DEG;
  B = atan2(G,Q)*RAD_TO_DEG;
  C = atan2(H,R)*RAD_TO_DEG;

    
  a = sqrt(F*F + P*P);
  b = sqrt(G*G + Q*Q);
  c = sqrt(H*H + R*R);


  x = r * a * DSIN(A + omega_ + nu);
  y = r * b * DSIN(B + omega_ + nu);
  z = r * c * DSIN(C + omega_ + nu);

  sun_rect(T, data->parabolic_elements.equinox_year,
	   sun_data.R, sun_data.Theta, &X, &Y, &Z);

  Delta = sqrt((X+x)*(X+x) + (Y+y)*(Y+y) + (Z+z)*(Z+z));
  R = sqrt(X*X + Y*Y + Z*Z);


  data->alpha = atan2(Y+y, X+x)*RAD_TO_DEG;
  if (data->alpha < 0) data->alpha += 360.0;
  data->delta = asin((Z+z)/Delta)*RAD_TO_DEG;
  data->r = r;
  data->Delta = Delta;

  if (data->parabolic_elements.equinox_year != 2000.0) {
    alpha = data->alpha;
    delta = data->delta;
    precess(data->parabolic_elements.equinox_year, 2000.0,
	    alpha, delta, &alpha2000, &delta2000);
    data->alpha2000 = alpha2000;
    data->delta2000 = delta2000;
  } else {
    data->alpha2000 = data->alpha;
    data->delta2000 = data->delta;
  };
/* Can't set l, b, lambda */

  data->psi = acos((R*R + Delta*Delta - r*r)/ (2*R*Delta))*RAD_TO_DEG;
  data->beta = acos((r*r + Delta*Delta - R*R)/ (2*r*Delta))*RAD_TO_DEG;

  if (data->body_type == asteroid) {
    G = data->G;
    H = data->H;
    psi_t = exp(log(DTAN(data->beta/2.0))*0.63);
    Psi_1 = exp(-3.33*psi_t);
    psi_t = exp(log(DTAN(data->beta/2.0))*1.22);
    Psi_2 = exp(-1.87*psi_t);
    data->mag = 5.0 * log10(r*Delta) + H
      - 2.5* log10((1-G)*Psi_1 + G*Psi_2);
  } else if (data->body_type == comet) {
    data->mag = data->g + 5.0 * log10(Delta)
      + 2.5 * data->kappa * log10(r);
  } else {
    data->mag = 1.0;
  }

}

