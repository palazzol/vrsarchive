/*
 * planetcalc.c
 * planetary positions calculations
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
  "$Header: /home/Vince/cvs/net/starchart/observe/planetcalc.c,v 1.1 1990-03-30 16:38:05 vrs Exp $";
#endif


#include <stdio.h>
#include <math.h>
#include "observe.h"
#include "degree.h"

static double M_sun(), M_1(), M_2(), M_4(), M_5(), M_6();

/* Approximate M for each planet 
   from Meeus, chapter 25 */

static double M_sun(jd)
double jd;
{
  double T;

  T = (jd - 2415020.0)/36525.0;

  return into_range(358.47583 + 35999.04975*T - 0.000150*T*T -0.0000033*T*T*T);
}

static double M_1(jd)
double jd;
{
  double T;

  T = (jd - 2415020.0)/36525.0;

  return into_range(102.27938 + 149472.51529*T + 0.000007*T*T);
}

static double M_2(jd)
double jd;
{
  double T;

  T = (jd - 2415020.0)/36525.0;

  return into_range(212.60322 + 58517.80387*T +0.001286*T*T);
}

static double M_4(jd)
double jd;
{
  double T;

  T = (jd - 2415020.0)/36525.0;

  return into_range(319.51913 + 19139.85475*T + 0.000181*T*T);
}

static double M_5(jd)
double jd;
{
  double T;

  T = (jd - 2415020.0)/36525.0;

  return into_range(225.32833 + 3034.69202*T - 0.000722*T*T);
}

static double M_6(jd)
double jd;
{
  double T;

  T = (jd - 2415020.0)/36525.0;

  return into_range(175.46622 +1221.55147*T - 0.000502*T*T);
}

static char *planet_name[] = {
  "Mercury",
  "Venus",
  "Mars",
  "Jupiter",
  "Saturn",
  "Uranus",
  "Neptune"
};

static char *planet_type[] = {
  "PM",
  "PV",
  "Pm",
  "PJ",
  "Ps",
  "PU",
  "PN"
};

static char *planet_colr[] = {
  "r9",
  "w9",
  "r9",
  "w9",
  "y9",
  "g9",
  "c9"
};

typedef struct {
  double L[4];
  double a;
  double e[4];
  double i[4];
  double omega[4];
  double Omega[4];
  double size_1au;
  double mag0;
} pelements;

pelements peles[] = {
  {   /* Mercury */
    {178.179078, 149474.07078, 3.011e-4, 0.0},
    0.3870986,
    {.20561421, 2.046e-5, 3e-8,  0.0},
    {7.002881, 1.8608e-3, -1.83e-5, 0.0},
    {28.753753, 0.3702806, 0.0001208, 0.0},
    {47.145944, 1.1852083, 1.739e-4, 0.0},
    6.74,
    -0.42
    },
  {   /* Venus */
    {342.767053, 58519.21191, 0.0003097, 0.0},
    0.7233316,
    {0.00682069, -0.00004774, 0.000000091, 0.0},
    {3.393631, 0.0010058, -0.0000010, 0.0},
    {54.384186, 0.5081861, -0.0013864, 0.0},
    {75.779647, 0.8998500, 0.0004100, 0.0},
    16.92,
    -4.4
    },
  {   /* Mars */
    {293.737334, 19141.69551, 0.0003107, 0.0},
    1.5236883,
    {0.09331290, 0.000092064, -0.000000077, 0.0},
    {1.850333, -0.0006750, 0.0000126, 0.0},
    {285.431761, 1.0697667, 0.0001313, 0.00000414},
    {48.786442, 0.7709917, -0.0000014, -0.00000533},
    9.36,
    -1.52
    },
  {   /* Jupiter */
    {238.049257, 3036.301986, 0.0003347, -0.00000165},
    5.202561,
    {0.04833475, 0.000164180, -0.0000004676, -0.0000000017},
    {1.308736, -0.0056961, 0.0000039, 0.0},
    {273.277558, 0.5994317, 0.00070405, 0.00000508},
    {99.443414, 1.0105300, 0.00035222, -0.00000851},
    196.74,
    -9.4
    },
  {   /* Saturn */
    {266.564377, 1223.509884, 0.0003245, -0.0000058},
    9.554747,
    {0.05589232, -0.00034550, -0.000000728, 0.00000000074},
    {2.492519, -0.0039189, -0.00001549, 0.00000004},
    {338.307800, 1.0852207, 0.00097854, 0.00000992},
    {112.790414, 0.8731951, -0.00015218, -0.00000531},
    165.6,
    -8.88
    },
  {   /* Uranus */
    {244.197470, 429.863546, 0.0003160, -0.00000060},
    19.21814,
    {0.0463444, -0.00002658, 0.000000077, 0.0},
    {0.772464, 0.0006253, 0.0000395, 0.0},
    {98.071581, 0.9857650, -0.0010745, -0.00000061},
    {73.477111, 0.4986678, 0.0013117, 0.0},
    65.8,
    -7.19
    },
  {   /* Neptune */
    {84.457994, 219.885914, 0.0003205, -0.00000060},
    30.10957,
    {0.00899704, 0.000006330, -0.000000002, 0.0},
    {1.779242, -0.0095436, -0.0000091, 0.0},
    {276.045975, 0.3256394, 0.00014095, 0.000004113},
    {130.681389, 1.0989350, 0.00024987, -0.000004718},
    62.2,
    -6.87
    }
};

typedef struct {
  double alpha_1, delta_1;
  double W_0, W_dot;
} rot_els_t;
rot_els_t rot_els[] = {
  {				/* Mercury */
    280.98, 61.44,
    142.14, 6.13850
    },
  {				/* Venus */
    272.78, 67.21,
    353.00, -1.48142
    },
  {				/* Mars */
    317.61, 52.85,
    237.21, 350.89198
    },
  {				/* Jupiter III */
    268.04, 64.49,
    156.03, 870.53600
    },
  {				/* Saturn III */
    40.09, 83.49,
    223.60, 810.79390
    },
  {				/* Uranus */
    257.27, -15.09,
    287.17, -499.42197
    },
  {				/* Neptune */
    295.24, 40.62,
    315.33, 468.75000
    }
};

static double polynom(jd, a)
     double jd;
     double a[4];
{
  double T;

  T = (jd - 2415020.0)/36525.0;

  return (a[0] + a[1]*T + a[2]*T*T + a[3]*T*T*T);
}

static void mercury(), venus(), mars(),
  jupiter(), saturn(), uranus(), neptune();


/* Calculate alpha and delta
   from lambda and beta (and epsilon)
   which are from r, Delta, psi, b, l, and Theta (in sun_data)
   which are from u, i (given), and Omega (given)
   u is from L (given), nu, and M
   nu and M are calculated.
   r is from E, a (given) and e (given)
   E is calculated

   calculate mag from Delta, size form Delta, phase (== beta).
 */

void planet_pos(jd, sun_data, nplanet, data)
     double jd;			/* time, jd */
     sun_data_t sun_data;
     int nplanet;		/* Planet number, 0 = mercury */
     planet_data_t *data;
{
  double L_, a_, e_, i_, omega_, Omega_, M_;
  double r;			/* radius distance to sun */
  double l, b;			/* ecliptical longitude and latitude */
  double Delta;			/* Distance to earth */
  double lambda, beta;		/* geocentric longitude and latitude */
  double alpha, delta;		/* R.A. and dec. both degrees */
  double alpha2000, delta2000;	/* R.A. and dec. both degrees equin 2000.0 */
  double psi;			/* elongation */
  double N, D;			/* temporary variables */
  double Theta;			/* Theta of the sun */
  double epsilon;		/* obliquity */
  double Cen;			/* center */
  double A, B;			/* used in calculating p_n */
  double W_1, K;		/* Used in calculating lambda_e */

  L_ = into_range(polynom(jd, peles[nplanet].L));
  a_ = peles[nplanet].a;
  e_ = polynom(jd, peles[nplanet].e);
  i_ = polynom(jd, peles[nplanet].i);
  omega_ = into_range(polynom(jd, peles[nplanet].omega));
  Omega_ = into_range(polynom(jd, peles[nplanet].Omega));
  M_ = into_range(L_ - omega_ - Omega_);

  /* Perturb */
  switch (nplanet) {
  case 0:
    mercury(jd, L_, a_, e_, i_, omega_, Omega_, M_, &r, &l, &b, &Cen);
    break;
  case 1:
    venus(jd, L_, a_, e_, i_, omega_, Omega_, M_, &r, &l, &b, &Cen);
    break;
  case 2:
    mars(jd, L_, a_, e_, i_, omega_, Omega_, M_, &r, &l, &b, &Cen);
    break;
  case 3:
    jupiter(jd, L_, a_, e_, i_, omega_, Omega_, M_, &r, &l, &b, &Cen);
    break;
  case 4:
    saturn(jd, L_, a_, e_, i_, omega_, Omega_, M_, &r, &l, &b, &Cen);
    break;
  case 5:
    uranus(jd, L_, a_, e_, i_, omega_, Omega_, M_, &r, &l, &b, &Cen);
    break;
  case 6:
    neptune(jd, L_, a_, e_, i_, omega_, Omega_, M_, &r, &l, &b, &Cen);
    break;
  default:
    break;
  };

  Theta = sun_data.Theta;
  N = r * DCOS(b) * DSIN(l - Theta);
  D = r * DCOS(b) * DCOS(l - Theta) + sun_data.R;

  epsilon = obl_jd(jd);

  lambda = into_range(RAD_TO_DEG * atan2(N, D)) + Theta;
  Delta = sqrt(N*N + D*D + (r * DSIN(b))*(r * DSIN(b)));
  beta = RAD_TO_DEG * asin(r * DSIN(b) / Delta);
  psi = RAD_TO_DEG * acos(DCOS(beta) * DCOS(lambda - Theta));
  if (into_range(lambda - Theta) > 180.0)
    psi = -psi;
  alpha = RAD_TO_DEG * atan2(DSIN(lambda)*DCOS(epsilon)
			     - DTAN(beta) * DSIN(epsilon),
			     DCOS(lambda));
  delta = RAD_TO_DEG * asin(DSIN(beta)*DCOS(epsilon)
			    + DCOS(beta)*DSIN(epsilon)*DSIN(lambda));
  alpha = into_range(alpha);

/* should correct for nutation and aberration */
  data->name = planet_name[nplanet];
  data->type = planet_type[nplanet];
  data->color = planet_colr[nplanet];
  data->alpha = alpha;
  data->delta = delta;
  precess(2000.0 - (2451545.0 - jd) / 365.25,
	  2000.0, alpha, delta, &alpha2000, &delta2000);
  data->alpha2000 = alpha2000;
  data->delta2000 = delta2000;
  data->l = l;
  data->b = b;
  data->lambda = lambda;
  data->beta = beta;
  data->psi = psi;
  data->phase =
    DACOS((r*r + Delta*Delta - sun_data.R*sun_data.R) / (2*r*Delta));
  if (psi < 0) data->phase = -data->phase;
  data->r = r;
  data->Delta = Delta;
  data->illum_frac = ((r+Delta)*(r+Delta) - sun_data.R*sun_data.R)/(4*r*Delta);
  data->chi = /* position angle of bright limb */
    DATAN2(DCOS(sun_data.delta) * DSIN(sun_data.alpha - alpha),
	  DCOS(delta) * DSIN(sun_data.delta)
	  - DSIN(delta) * DCOS(sun_data.delta) * DCOS(sun_data.alpha - alpha));
  data->Cen = Cen;

  data->mag = 5.0 * log10(r*Delta)
    - 2.5 * log10(data->illum_frac)
    + peles[nplanet].mag0;

  data->size = peles[nplanet].size_1au / Delta;

  data->rotation_elements.beta_e =
    DASIN(-DSIN(rot_els[nplanet].delta_1)*DSIN(delta)
	  - DCOS(rot_els[nplanet].delta_1) * DCOS(delta)
	      * DCOS(rot_els[nplanet].alpha_1 - alpha));

  A = DCOS(rot_els[nplanet].delta_1) * DSIN(rot_els[nplanet].alpha_1 - alpha);
  A /= DCOS(data->rotation_elements.beta_e);
  B = DSIN(rot_els[nplanet].delta_1) * DCOS(delta)
    - DCOS(rot_els[nplanet].delta_1) * DSIN(delta)
      * DCOS(rot_els[nplanet].alpha_1 - alpha);
  B /= DCOS(data->rotation_elements.beta_e);
  data->rotation_elements.p_n = DATAN2(A, B);

  W_1 = into_range(rot_els[nplanet].W_0
		   + rot_els[nplanet].W_dot
		     * (jd - 2447526.5 - 0.0057755 * Delta));

  A = -DCOS(rot_els[nplanet].delta_1) * DSIN(delta)
    + DSIN(rot_els[nplanet].delta_1) * DCOS(delta)
      * DCOS(rot_els[nplanet].alpha_1 - alpha);
  A /= DCOS(data->rotation_elements.beta_e);
  B = DCOS(delta) * DSIN(rot_els[nplanet].alpha_1 - alpha);
  B /= DCOS(data->rotation_elements.beta_e);
  K = DATAN2(A, B);
  if (rot_els[nplanet].W_dot < 0.0)
    data->rotation_elements.lambda_e = into_range(K - W_1);
  else
    data->rotation_elements.lambda_e = into_range(W_1 - K);
}


static void mercury(jd, L_, a_, e_, i_, omega_, Omega_, M_, r_p, l_p, b_p, C_p)
double jd, L_, a_, e_, i_, omega_, Omega_, M_;
double *r_p, *l_p, *b_p, *C_p;
{
  double E, nu;
  double M1, M2, M4, M5, M6;
  double r, l, b;

  double u;			/* argument of latitude */
  double r_pert, l_pert;

  M1 = M_1(jd);
  M2 = M_2(jd);
  M4 = M_4(jd);
  M5 = M_5(jd);
  M6 = M_6(jd);


  /* Calculate E and nu */
  anom_calc(M_, e_, &E, &nu);
  r = a_ * (1 - e_ * DCOS(E));

  u = L_ + nu - M_ - Omega_;
  *C_p = nu - M_;
  l = into_range(RAD_TO_DEG * atan2(DCOS(i_) * DSIN(u), DCOS(u)) + Omega_);
  b = RAD_TO_DEG * asin(DSIN(u)*DSIN(i_));

  /* Perturbations */
  l_pert = 0.00204*DCOS(5*M2-2*M1+12.220)
    +0.00103*DCOS(2*M2-M1-160.692)
      +0.00091*DCOS(2*M5-M1-37.003)
	+0.00078*DCOS(5*M2-3*M1+10.137);
  r_pert = 0.000007525*DCOS(2*M5-M1+53.013)
    +0.000006802*DCOS(5*M2-3*M1-259.918)
      +0.000005457*DCOS(2*M2-2*M1-71.188)
	+0.000003569*DCOS(5*M2-M1-77.75);

  *r_p = r + r_pert;
  *l_p = l + l_pert;
  *b_p = b;
}

static void venus(jd, L_, a_, e_, i_, omega_, Omega_, M_, r_p, l_p, b_p, C_p)
double jd, L_, a_, e_, i_, omega_, Omega_, M_;
double *r_p, *l_p, *b_p, *C_p;
{
  double E, nu;
  double M, M1, M2, M4, M5, M6;
  double r, l, b;

  double u;			/* argument of latitude */
  double T;
  double r_pert, l_pert;

  M = M_sun(jd);
  M1 = M_1(jd);
  M2 = M_2(jd);
  M4 = M_4(jd);
  M5 = M_5(jd);
  M6 = M_6(jd);


  /* Long term perturbation */
  T = (jd - 2415020.0)/36525.0;
  l_pert = 0.00077 * DSIN(237.24 + 150.27*T);
  M_ += l_pert;
  L_ += l_pert;

  /* Calculate E and nu */
  anom_calc(M_, e_, &E, &nu);
  r = a_ * (1 - e_ * DCOS(E));


  u = L_ + nu - M_ - Omega_;
  *C_p = nu - M_;
  l = into_range(RAD_TO_DEG * atan2(DCOS(i_) * DSIN(u), DCOS(u)) + Omega_);
  b = RAD_TO_DEG * asin(DSIN(u)*DSIN(i_));

  /* Perturbations */
  l_pert = 0.00313*DCOS(2*M-2*M2 -148.225)
    +0.00198*DCOS(3*M-3*M2 +2.565)
      +0.00136*DCOS(M-M2-119.107)
	+0.00096*DCOS(3*M-2*M2-135.912)
	  +0.00082*DCOS(M5-M2-208.087);
  r_pert = 0.000022501 * DCOS(2*M-2*M2-58.208)
    +0.000019045 * DCOS(3*M-3*M2+92.577)
      +0.000006887 * DCOS(M5-M2-118.090)
	+0.000005172 * DCOS(M-M2-29.110)
	  +0.000003620 * DCOS(5*M-4*M2-104.208)
	    +0.000003283 * DCOS(4*M-4*M2+63.513)
	      +0.000003074 * DCOS(2*M5-2*M2-55.167);

  *r_p = r + r_pert;
  *l_p = l + l_pert;
  *b_p = b;
}

static void mars(jd, L_, a_, e_, i_, omega_, Omega_, M_, r_p, l_p, b_p, C_p)
double jd, L_, a_, e_, i_, omega_, Omega_, M_;
double *r_p, *l_p, *b_p, *C_p;
{
  double E, nu;
  double M, M1, M2, M4, M5, M6;
  double r, l, b;

  double u;			/* argument of latitude */
  double T;
  double r_pert, l_pert;

  M = M_sun(jd);
  M1 = M_1(jd);
  M2 = M_2(jd);
  M4 = M_4(jd);
  M5 = M_5(jd);
  M6 = M_6(jd);


  /* Long term perturbation */
  T = (jd - 2415020.0)/36525.0;
  l_pert = -0.01133*DSIN(3*M5-8*M4 +4*M) - 0.00933*DCOS(3*M5-8*M4 +4*M);

  M_ += l_pert;
  L_ += l_pert;

  /* Calculate E and nu */
  anom_calc(M_, e_, &E, &nu);
  r = a_ * (1 - e_ * DCOS(E));


  u = L_ + nu - M_ - Omega_;
  *C_p = nu - M_;
  l = into_range(RAD_TO_DEG * atan2(DCOS(i_) * DSIN(u), DCOS(u)) + Omega_);
  b = RAD_TO_DEG * asin(DSIN(u)*DSIN(i_));

  /* Perturbations */
  l_pert = 0.00705*DCOS(M5-M4-48.958)
    +0.00607*DCOS(2*M5-M4-188.350)
      +0.00445*DCOS(2*M5-2*M4-191.897)
	+0.00388*DCOS(M-2*M4+20.495)
	  +0.00238*DCOS(M-M4+35.097)
	    +0.00204*DCOS(2*M-3*M4+158.638)
	      +0.00177*DCOS(3*M4-M2-57.602)
		+0.00136*DCOS(2*M-4*M4+154.093)
		  +0.00104*DCOS(M5+17.618);
  r_pert = 0.000053227*DCOS(M5-M4+41.1306)
    +0.000050989*DCOS(2*M5-2*M4-101.9847)
      +0.000038278*DCOS(2*M5-M4-98.3292)
	+0.000015996*DCOS(M-M4-55.555)
	  +0.000014764*DCOS(2*M-3*M4+68.622)
	    +0.000008966*DCOS(M5-2*M4+43.615);
  r_pert += 0.000007914*DCOS(3*M5-2*M4-139.737)
    +0.000007004*DCOS(2*M5-3*M4-102.888)
      +0.000006620*DCOS(M-2*M4+113.202)
	+0.000004930*DCOS(3*M5-3*M4-76.243)
	  +0.000004693*DCOS(3*M-5*M4+190.603)
	    +0.000004571*DCOS(2*M-4*M4+244.702)
	      +0.000004409*DCOS(3*M5-M4-115.828);

  *r_p = r + r_pert;
  *l_p = l + l_pert;
  *b_p = b;
}


static void jupiter(jd, L_, a_, e_, i_, omega_, Omega_, M_, r_p, l_p, b_p, C_p)
double jd, L_, a_, e_, i_, omega_, Omega_, M_;
double *r_p, *l_p, *b_p, *C_p;
{
  double E, nu;
  double M, M1, M2, M4, M5, M6;
  double r, l, b;

  double u;			/* argument of latitude */
  double T;
  double A, B, e_pert, a_pert, v, zeta;
  double P, Q, S, V, W;

  M = M_sun(jd);
  M1 = M_1(jd);
  M2 = M_2(jd);
  M4 = M_4(jd);
  M5 = M_5(jd);
  M6 = M_6(jd);

  /* for perturbations */
  T = (jd - 2415020.0)/36525.0;

  v = T/5.0 + 0.1;
  P = 237.47555 +3034.9061*T;
  Q = 265.91650 + 1222.1139*T;
  S = 243.51721 + 428.4677*T;
  V = 5.0*Q -2.0*P;
  W = 2.0*P - 6.0*Q +3.0*S;
  zeta = Q - P;


  A = (0.331364 - 0.010281*v - 0.004692*v*v)*DSIN(V)
    +(0.003228 - 0.064436*v + 0.002075*v*v)*DCOS(V)
      -(0.003083 + 0.000275*v - 0.000489*v*v)*DSIN(2*V)
	+0.002472*DSIN(W)
	  +0.013619*DSIN(zeta)
	    +0.018472*DSIN(2*zeta)
	      +0.006717*DSIN(3*zeta)
		+0.002775*DSIN(4*zeta)
		  +(0.007275 - 0.001253*v)*DSIN(zeta)*DSIN(Q)
		    +0.006417*DSIN(2*zeta)*DSIN(Q)
		      +0.002439*DSIN(3*zeta)*DSIN(Q);
  A += -(0.033839 + 0.001125*v)*DCOS(zeta)*DSIN(Q)
    -0.003767*DCOS(2*zeta)*DSIN(Q)
      -(0.035681 + 0.001208*v)*DSIN(zeta)*DCOS(Q)
	-0.004261*DSIN(2*zeta)*DCOS(Q)
	  +0.002178*DCOS(Q)
	    +(-0.006333 + 0.001161*v)*DCOS(zeta)*DCOS(Q)
	      -0.006675*DCOS(2*zeta)*DCOS(Q)
		-0.002664*DCOS(3*zeta)*DCOS(Q)
		  -0.002572*DSIN(zeta)*DSIN(2*Q)
		    -0.003567*DSIN(2*zeta)*DSIN(2*Q)
		      +0.002094*DCOS(zeta)*DCOS(2*Q)
			+0.003342*DCOS(2*zeta)*DCOS(2*Q);

  e_pert = (.0003606 + .0000130*v - .0000043*v*v)*DSIN(V)
    +(.0001289 - .0000580*v)*DCOS(V)
      -.0006764*DSIN(zeta)*DSIN(Q)
	-.0001110*DSIN(2*zeta)*DSIN(Q)
	  -.0000224*DSIN(3*zeta)*DSIN(Q)
	    -.0000204*DSIN(Q)
	      +(.0001284 + .0000116*v)*DCOS(zeta)*DSIN(Q)
		+.0000188*DCOS(2*zeta)*DSIN(Q)
		  +(.0001460 + .0000130*v)*DSIN(zeta)*DCOS(Q)
		    +.0000224*DSIN(2*zeta)*DCOS(Q)
		      -.0000817*DCOS(Q);

  e_pert += .0006074*DCOS(zeta)*DCOS(Q)
    +.0000992*DCOS(2*zeta)*DCOS(Q)
      +.0000508*DCOS(3*zeta)*DCOS(Q)
	+.0000230*DCOS(4*zeta)*DCOS(Q)
	  +.0000108*DCOS(5*zeta)*DCOS(Q);

  e_pert += -(.0000956 + .0000073*v)*DSIN(zeta)*DSIN(2*Q)
    +.0000448*DSIN(2*zeta)*DSIN(2*Q)
      +.0000137*DSIN(3*zeta)*DSIN(2*Q)
	+(-.0000997 + .0000108*v)*DCOS(zeta)*DSIN(2*Q)
	  +.0000480*DCOS(2*zeta)*DSIN(2*Q);

  e_pert += .0000148*DCOS(3*zeta)*DSIN(2*Q)
    +(-.0000956 +.0000099*v)*DSIN(zeta)*DCOS(2*Q)
      +.0000490*DSIN(2*zeta)*DCOS(2*Q)
	+.0000158*DSIN(3*zeta)*DCOS(2*Q);

  e_pert += .0000179*DCOS(2*Q)
    +(.0001024 + .0000075*v)*DCOS(zeta)*DCOS(2*Q)
      -.0000437*DCOS(2*zeta)*DCOS(2*Q)
	-.0000132*DCOS(3*zeta)*DCOS(2*Q);

  B = (0.007192 - 0.003147*v)*DSIN(V)
    +(-0.020428 - 0.000675*v + 0.000197*v*v)*DCOS(V)
      +(0.007269 + 0.000672*v)*DSIN(zeta)*DSIN(Q)
	-0.004344*DSIN(Q)
	  +0.034036*DCOS(zeta)*DSIN(Q)
	    +0.005614*DCOS(2*zeta)*DSIN(Q)
	      +0.002964*DCOS(3*zeta)*DSIN(Q)
		+0.037761*DSIN(zeta)*DCOS(Q);

  B += 0.006158*DSIN(2*zeta)*DCOS(Q)
      -0.006603*DCOS(zeta)*DCOS(Q)
	-0.005356*DSIN(zeta)*DSIN(2*Q)
	  +0.002722*DSIN(2*zeta)*DSIN(2*Q)
	    +0.004483*DCOS(zeta)*DSIN(2*Q);

  B += -0.002642*DCOS(2*zeta)*DSIN(2*Q)
    +0.004403*DSIN(zeta)*DCOS(2*Q)
      -0.002536*DSIN(2*zeta)*DCOS(2*Q)
	+0.005547*DCOS(zeta)*DCOS(2*Q)
	  -0.002689*DCOS(2*zeta)*DCOS(2*Q);

  a_pert = -.000263*DCOS(V)
    +.000205*DCOS(zeta)
      +.000693*DCOS(2*zeta)
	+.000312*DCOS(3*zeta)
	  +.000147*DCOS(4*zeta)
	    +.000299*DSIN(zeta)*DSIN(Q)
	      +.000181*DCOS(2*zeta)*DSIN(Q)
		+.000204*DSIN(2*zeta)*DCOS(Q)
		  +.000111*DSIN(3*zeta)*DCOS(Q)
		    -.000337*DCOS(zeta)*DCOS(Q)
		      -.000111*DCOS(2*zeta)*DCOS(Q);

  L_ += A;
  M_ += A - B / e_;
  e_ += e_pert;
  a_ += a_pert;
  omega_ += B;

  /* Calculate E and nu */
  anom_calc(M_, e_, &E, &nu);
  r = a_ * (1 - e_ * DCOS(E));


  u = L_ + nu - M_ - Omega_;
  *C_p = nu - M_;
  l = into_range(RAD_TO_DEG * atan2(DCOS(i_) * DSIN(u), DCOS(u)) + Omega_);
  b = RAD_TO_DEG * asin(DSIN(u)*DSIN(i_));

  *r_p = r;
  *l_p = l;
  *b_p = b;
}


static void saturn(jd, L_, a_, e_, i_, omega_, Omega_, M_, r_p, l_p, b_p, C_p)
double jd, L_, a_, e_, i_, omega_, Omega_, M_;
double *r_p, *l_p, *b_p, *C_p;
{
  double E, nu;
  double M, M1, M2, M4, M5, M6;
  double r, l, b;

  double u;			/* argument of latitude */
  double T;
  double A, B, e_pert, a_pert, b_pert, v, zeta, psi;
  double P, Q, S, V, W;

  M = M_sun(jd);
  M1 = M_1(jd);
  M2 = M_2(jd);
  M4 = M_4(jd);
  M5 = M_5(jd);
  M6 = M_6(jd);

  /* for perturbations */
  T = (jd - 2415020.0)/36525.0;

  v = T/5.0 + 0.1;
  P = 237.47555 +3034.9061*T;
  Q = 265.91650 + 1222.1139*T;
  S = 243.51721 + 428.4677*T;
  V = 5.0*Q -2.0*P;
  W = 2.0*P - 6.0*Q +3.0*S;
  zeta = Q - P;
  psi = S - Q;

  A = (-0.814181 + 0.018150*v + 0.016714*v*v)*DSIN(V)
    +(-0.010497 + 0.160906*v - 0.004100*v*v)*DCOS(V)
      +0.007581*DSIN(2*V)
	-0.007986*DSIN(W)
	  -0.148811*DSIN(zeta)
	    -0.040786*DSIN(2*zeta)
	      -0.015208*DSIN(3*zeta)
		-0.006339*DSIN(4*zeta)
		  -0.006244*DSIN(Q);
  A += (0.008931 + 0.002728*v)*DSIN(zeta)*DSIN(Q)
      -0.016500*DSIN(2*zeta)*DSIN(Q)
	-0.005775*DSIN(3*zeta)*DSIN(Q)
	  +(0.081344 + 0.003206*v)*DCOS(zeta)*DSIN(Q)
	    +0.015019*DCOS(2*zeta)*DSIN(Q)
	      +(0.085581 + 0.002494*v)*DSIN(zeta)*DCOS(Q)
		+(0.025328 - 0.003117*v)*DCOS(zeta)*DCOS(Q);
  A += 0.014394*DCOS(2*zeta)*DCOS(Q)
      +0.006319*DCOS(3*zeta)*DCOS(Q)
	+0.006369*DSIN(zeta)*DSIN(2*Q)
	  +0.009156*DSIN(2*zeta)*DSIN(2*Q)
	    +0.007525*DSIN(3*psi)*DSIN(2*Q)
	      -0.005236*DCOS(zeta)*DCOS(2*Q)
		-0.007736*DCOS(2*zeta)*DCOS(2*Q)
		  -0.007528*DCOS(3*psi)*DCOS(2*Q);

  e_pert = (-.0007927 + .0002548*v +.0000091*v*v)*DSIN(V)
    +(.0013381 + .0001226*v -.0000253*v*v)*DCOS(V)
      +(.0000248 - .0000121*v)*DSIN(2*V)
	-(.0000305 + .0000091*v)*DCOS(2*V)
	  +.0000412*DSIN(2*zeta)
	    +.0012415*DSIN(Q)
	      +(.0000390 -.0000617*v)*DSIN(zeta)*DSIN(Q)
		+(.0000165 - .0000204*v)*DSIN(2*zeta)*DSIN(Q)
		  +.0026599*DCOS(zeta)*DSIN(Q)
		    -.0004687*DCOS(2*zeta)*DSIN(Q);
  e_pert += -.0001870*DCOS(3*zeta)*DSIN(Q)
      -.0000821*DCOS(4*zeta)*DSIN(Q)
	-.0000377*DCOS(5*zeta)*DSIN(Q)
	  +.0000497*DCOS(2*psi)*DSIN(Q)
	    +(.0000163 - .0000611*v)*DCOS(Q)
	      -.0012696*DSIN(zeta)*DCOS(Q)
		-.0004200*DSIN(2*zeta)*DCOS(Q)
		  -.0001503*DSIN(3*zeta)*DCOS(Q)
		    -.0000619*DSIN(4*zeta)*DCOS(Q)
		      -.0000268*DSIN(5*zeta)*DCOS(Q);
  e_pert += -(.0000282 + .0001306*v)*DCOS(zeta)*DCOS(Q)
      +(-.0000086 + .0000230*v)*DCOS(2*zeta)*DCOS(Q)
	+.0000461*DSIN(2*psi)*DCOS(Q)
	  -.0000350*DSIN(2*Q)
	    +(.0002211 - .0000286*v)*DSIN(zeta)*DSIN(2*Q)
	      -.0002208*DSIN(2*zeta)*DSIN(2*Q)
		-.0000568*DSIN(3*zeta)*DSIN(2*Q)
		  -.0000346*DSIN(4*zeta)*DSIN(2*Q)
		    -(.0002780 + .0000222*v)*DCOS(zeta)*DSIN(2*Q)
		      +(.0002022 + .0000263*v)*DCOS(2*zeta)*DSIN(2*Q);
  e_pert += .0000248*DCOS(3*zeta)*DSIN(2*Q)
      +.0000242*DSIN(3*psi)*DSIN(2*Q)
	+.0000467*DCOS(3*psi)*DSIN(2*Q)
	  -.0000490*DCOS(2*Q)
	    -(.0002842 + .0000279*v)*DSIN(zeta)*DCOS(2*Q)
	      +(.0000128 + .0000226*v)*DSIN(2*zeta)*DCOS(2*Q)
		+.0000224*DSIN(3*zeta)*DCOS(2*Q)
		  +(-.0001594 + .0000282*v)*DCOS(zeta)*DCOS(2*Q)
		    +(.0002162 - .0000207*v)*DCOS(2*zeta)*DCOS(2*Q)
		      +.0000561*DCOS(3*zeta)*DCOS(2*Q);
  e_pert += .0000343*DCOS(4*zeta)*DCOS(2*Q)
      +.0000469*DSIN(3*psi)*DCOS(2*Q)
	-.0000242*DCOS(3*psi)*DCOS(2*Q)
	  -.0000205*DSIN(zeta)*DSIN(3*Q)
	    +.0000262*DSIN(3*zeta)*DSIN(3*Q)
	      +.0000208*DCOS(zeta)*DCOS(3*Q)
		-.0000271*DCOS(3*zeta)*DCOS(3*Q)
		  -.0000382*DCOS(3*zeta)*DSIN(4*Q)
		    -.0000376*DSIN(3*zeta)*DCOS(4*Q);
  B = (0.077108 + 0.007186*v - 0.001533*v*v)*DSIN(V)
    +(0.045803 - 0.014766*v - 0.000536*v*v)*DCOS(V)
      -0.007075*DSIN(zeta)
	-0.075825*DSIN(zeta)*DSIN(Q)
	  -0.024839*DSIN(2*zeta)*DSIN(Q)
	    -0.008631*DSIN(3*zeta)*DSIN(Q)
	      -0.072586*DCOS(Q)
		-0.150383*DCOS(zeta)*DCOS(Q)
		  +0.026897*DCOS(2*zeta)*DCOS(Q)
		    +0.010053*DCOS(3*zeta)*DCOS(Q);
  B += -(0.013597 +0.001719*v)*DSIN(zeta)*DSIN(2*Q)
      +(-0.007742 + 0.001517*v)*DCOS(zeta)*DSIN(2*Q)
	+(0.013586 - 0.001375*v)*DCOS(2*zeta)*DSIN(2*Q)
	  +(-0.013667 + 0.001239*v)*DSIN(zeta)*DCOS(2*Q)
	    +0.011981*DSIN(2*zeta)*DCOS(2*Q)
	      +(0.014861 + 0.001136*v)*DCOS(zeta)*DCOS(2*Q)
		-(0.013064 + 0.001628*v)*DCOS(2*zeta)*DCOS(2*Q);

  a_pert = .000572*DSIN(V) -.001590*DSIN(2*zeta)*DCOS(Q)
    +.002933*DCOS(V) -.000647*DSIN(3*zeta)*DCOS(Q)
      +.033629*DCOS(zeta) -.000344*DSIN(4*zeta)*DCOS(Q)
	-.003081*DCOS(2*zeta) +.002885*DCOS(zeta)*DCOS(Q)
	  -.001423*DCOS(3*zeta) +(.002172 + .000102*v)*DCOS(2*zeta)*DCOS(Q)
	    -.000671*DCOS(4*zeta) +.000296*DCOS(3*zeta)*DCOS(Q)
	      -.000320*DCOS(5*zeta) -.000267*DSIN(2*zeta)*DSIN(2*Q);
  a_pert += .001098*DSIN(Q) -.000778*DCOS(zeta)*DSIN(2*Q)
      -.002812*DSIN(zeta)*DSIN(Q) +.000495*DCOS(2*zeta)*DSIN(2*Q)
	+.000688*DSIN(2*zeta)*DSIN(Q) +.000250*DCOS(3*zeta)*DSIN(2*Q);
  a_pert += -.000393*DSIN(3*zeta)*DSIN(Q)
      -.000228*DSIN(4*zeta)*DSIN(Q)
	+.002138*DCOS(zeta)*DSIN(Q)
	  -.000999*DCOS(2*zeta)*DSIN(Q)
	    -.000642*DCOS(3*zeta)*DSIN(Q)
	      -.000325*DCOS(4*zeta)*DSIN(Q)
		-.000890*DCOS(Q)
		  +.002206*DSIN(zeta)*DCOS(Q);
  a_pert += -.000856*DSIN(zeta)*DCOS(2*Q)
      +.000441*DSIN(2*zeta)*DCOS(2*Q)
	+.000296*DCOS(2*zeta)*DCOS(2*Q)
	  +.000211*DCOS(3*zeta)*DCOS(2*Q)
	    -.000427*DSIN(zeta)*DSIN(3*Q)
	      +.000398*DSIN(3*zeta)*DSIN(3*Q)
		+.000344*DCOS(zeta)*DCOS(3*Q)
		  -.000427*DCOS(3*zeta)*DCOS(3*Q);

  L_ += A;
  M_ += A - B / e_;
  e_ += e_pert;
  a_ += a_pert;
  omega_ += B;

  /* Calculate E and nu */
  anom_calc(M_, e_, &E, &nu);
  r = a_ * (1 - e_ * DCOS(E));


  u = L_ + nu - M_ - Omega_;
  *C_p = nu - M_;
  l = into_range(RAD_TO_DEG * atan2(DCOS(i_) * DSIN(u), DCOS(u)) + Omega_);
  b = RAD_TO_DEG * asin(DSIN(u)*DSIN(i_));

  b_pert = 0.000747*DCOS(zeta)*DSIN(Q)
      +0.001069*DCOS(zeta)*DCOS(Q)
	+0.002108*DSIN(2*zeta)*DSIN(2*Q)
	  +0.001261*DCOS(2*zeta)*DSIN(2*Q)
	    +0.001236*DSIN(2*zeta)*DCOS(2*Q)
	      -0.002075*DCOS(2*zeta)*DCOS(2*Q);


  *r_p = r;
  *l_p = l;
  *b_p = b + b_pert;
}

static void uranus(jd, L_, a_, e_, i_, omega_, Omega_, M_, r_p, l_p, b_p, C_p)
double jd, L_, a_, e_, i_, omega_, Omega_, M_;
double *r_p, *l_p, *b_p, *C_p;
{
  double E, nu;
  double M, M1, M2, M4, M5, M6;
  double r, l, b;

  double u;			/* argument of latitude */
  double T;
  double A, B, e_pert, a_pert, b_pert, v, zeta, eta, theta;
  double P, Q, S, V, W, G, H;
  double r_pert, l_pert;

  M = M_sun(jd);
  M1 = M_1(jd);
  M2 = M_2(jd);
  M4 = M_4(jd);
  M5 = M_5(jd);
  M6 = M_6(jd);

  /* for perturbations */
  T = (jd - 2415020.0)/36525.0;

  v = T/5.0 + 0.1;
  P = 237.47555 +3034.9061*T;
  Q = 265.91650 + 1222.1139*T;
  S = 243.51721 + 428.4677*T;
  V = 5.0*Q -2.0*P;
  W = 2.0*P - 6.0*Q +3.0*S;
  G = 83.76922 + 218.4901*T;
  H = 2.0*G - S;
  zeta = S - P;
  eta = S - Q;
  theta = G - S;

  A = (0.864319 - 0.001583*v)*DSIN(H)
    +(0.082222 - 0.006833*v)*DCOS(H)
      +0.036017*DSIN(2*H)
	-0.003019*DCOS(2*H)
	  +0.008122*DSIN(W);

  e_pert = (-.0003349 + .0000163*v)*DSIN(H)
    +.0020981*DCOS(H)
      +.0001311*DCOS(H);

  B = 0.120303*DSIN(H)
    +(0.019472 - 0.000947*v)*DCOS(H)
      +0.006197*DSIN(2*H);

  a_pert = - 0.003825*DCOS(H);

  L_ += A;
  M_ += A - B / e_;
  e_ += e_pert;
  a_ += a_pert;
  omega_ += B;

  /* Calculate E and nu */
  anom_calc(M_, e_, &E, &nu);
  r = a_ * (1 - e_ * DCOS(E));


  u = L_ + nu - M_ - Omega_;
  *C_p = nu - M_;
  l = into_range(RAD_TO_DEG * atan2(DCOS(i_) * DSIN(u), DCOS(u)) + Omega_);
  b = RAD_TO_DEG * asin(DSIN(u)*DSIN(i_));

  l_pert = (0.010122 - 0.000988*v)*DSIN(S+eta)
      +(-0.038581 + 0.002031*v - 0.001910*v*v)*DCOS(S+eta)
	+(0.034964 - 0.001038*v + 0.000868*v*v)*DCOS(2*S+eta)
	  +0.005594*DSIN(S +3*theta);
  l_pert += -0.014808*DSIN(zeta)
      -0.005794*DSIN(eta)
	+0.002347*DCOS(eta)
	  +0.009872*DSIN(theta)
	    +0.008803*DSIN(2*theta)
	      -0.004308*DSIN(3*theta);
  b_pert = (0.000458*DSIN(eta) - 0.000642*DCOS(eta) - 0.000517*DCOS(4*theta))
      *DSIN(S)
	-(0.000347*DSIN(eta) + 0.000853*DCOS(eta) + 0.000517*DSIN(4*eta))
	  *DCOS(S)
	    +0.000403*(DCOS(2*theta)*DSIN(2*S) + DSIN(2*theta)*DCOS(2*S));
  r_pert = -.025948
      +.004985*DCOS(zeta)
	-.001230*DCOS(S)
	  +.003354*DCOS(eta)
	    +(.005795*DCOS(S) - .001165*DSIN(S) + .001388*DCOS(2*S))*DSIN(eta)
	      +(.001351*DCOS(S) + .005702*DSIN(S) + .001388*DSIN(2*S))*DCOS(eta)
		+.000904*DCOS(2*theta)
		  +.000894*(DCOS(theta) - DCOS(3*theta));

  *r_p = r + r_pert;
  *l_p = l + l_pert;
  *b_p = b + b_pert;
}

static void neptune(jd, L_, a_, e_, i_, omega_, Omega_, M_, r_p, l_p, b_p, C_p)
double jd, L_, a_, e_, i_, omega_, Omega_, M_;
double *r_p, *l_p, *b_p, *C_p;
{
  double E, nu;
  double M, M1, M2, M4, M5, M6;
  double r, l, b;

  double u;			/* argument of latitude */
  double T;
  double A, B, e_pert, a_pert, b_pert, v, zeta, eta, theta;
  double P, Q, S, V, W, G, H;
  double r_pert, l_pert;

  M = M_sun(jd);
  M1 = M_1(jd);
  M2 = M_2(jd);
  M4 = M_4(jd);
  M5 = M_5(jd);
  M6 = M_6(jd);

  /* for perturbations */
  T = (jd - 2415020.0)/36525.0;

  v = T/5.0 + 0.1;
  P = 237.47555 +3034.9061*T;
  Q = 265.91650 + 1222.1139*T;
  S = 243.51721 + 428.4677*T;
  V = 5.0*Q -2.0*P;
  W = 2.0*P - 6.0*Q +3.0*S;
  G = 83.76922 + 218.4901*T;
  H = 2.0*G - S;
  zeta = S - P;
  eta = S - Q;
  theta = G - S;

  A = (-0.589833 + 0.001089*v)*DSIN(H)
    +(-0.056094 + 0.004658*v)*DCOS(H)
      -0.024286*DSIN(2*H);

  e_pert = .0004389*DSIN(H)
    +.0004262*DCOS(H)
      +.0001129*DSIN(2*H)
	+.0001089*DCOS(2*H);

  B = 0.024039*DSIN(H)
    -0.025303*DCOS(H)
      +0.006206*DSIN(2*H)
	-0.005992*DCOS(2*H);

  a_pert = -0.000817*DSIN(H)
    +0.008189*DCOS(H)
      +0.000781*DCOS(2*H);

  L_ += A;
  M_ += A - B / e_;
  e_ += e_pert;
  a_ += a_pert;
  omega_ += B;

  /* Calculate E and nu */
  anom_calc(M_, e_, &E, &nu);
  r = a_ * (1 - e_ * DCOS(E));


  u = L_ + nu - M_ - Omega_;
  *C_p = nu - M_;
  l = into_range(RAD_TO_DEG * atan2(DCOS(i_) * DSIN(u), DCOS(u)) + Omega_);
  b = RAD_TO_DEG * asin(DSIN(u)*DSIN(i_));

  l_pert = -0.009556*DSIN(zeta)
      -0.005178*DSIN(eta)
	+0.002572*DSIN(2*theta)
	  -0.002972*DCOS(2*theta)*DSIN(G)
	    -0.002833*DSIN(2*theta)*DCOS(G);

  b_pert = 0.000336*DCOS(2*theta)*DSIN(G)
      +0.000364*DSIN(2*theta)*DCOS(G);

  r_pert = -.040596
      +.004992*DCOS(zeta)
	+.002744*DCOS(eta)
	  +.002044*DCOS(theta)
	    +.001051*DCOS(2*theta);

  *r_p = r + r_pert;
  *l_p = l + l_pert;
  *b_p = b + b_pert;
}

