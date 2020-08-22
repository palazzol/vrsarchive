/*
 * moonpos.c
 * moon position calculations
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
 * DISCLAIMER OF WARRANTY
 * ----------------------
 * The author  disclaims all warranties  with regard to  this software to
 * the   extent  permitted  by applicable   law,  including all   implied
 * warranties  of merchantability  and  fitness. In  no event shall   the
 * author be liable for any special, indirect or consequential damages or
 * any  damages whatsoever resulting from  loss of use, data or  profits,
 * whether in an action of contract, negligence or other tortious action,
 * arising  out of  or in connection with the  use or performance of this
 * software.
 *
 */


#ifndef  lint
static char rcsid[] =
  "$Header: /home/Vince/cvs/net/starchart/observe/mooncalc.c,v 1.1 1990-03-30 16:37:50 vrs Exp $";
#endif


#include <math.h>
#include "observe.h"
#include "degree.h"

void moon_pos(jd, sun_data, moon_data)
     double jd;
     sun_data_t sun_data;
     moon_data_t *moon_data;
{
  double T;
  double L_prime, M, M_prime, delta, f, Omega, e;
  double lambda, beta, moon_pi;
  double tmp;
  double venus_term;
  double omega1, omega2;
  double alpha;			/* R.A. and dec.(delta above) both degrees */
  double alpha2000, delta2000;	/* R.A. and dec. both degrees equin 2000.0 */
  double epsilon;		/* obliquity */

  T = (jd - 2415020.0)/36525.0;

  /* Compute the mean values for the terms. */
  L_prime = into_range(270.434164 + 481267.8831 * T
		       - 0.001133 * T*T + 0.0000019 * T*T*T);
  M = into_range(358.475833 + 35999.0498 * T
		 - 0.000150 * T*T - 0.0000033 * T*T*T);
  M_prime = into_range(296.104608 + 477198.8491 * T
		       + 0.009192 * T*T + 0.0000144 * T*T*T);
  delta = into_range(350.737486 + 445267.1142 * T
		     - 0.001436 * T*T + 0.0000019 * T*T*T);
  f = into_range(11.250889 + 483202.0251 * T
		 - 0.003211 * T*T - 0.0000003 * T*T*T);
  Omega = into_range(259.183275 - 1934.1420 * T
		     + 0.002078 * T*T + 0.0000022 * T*T*T);

  /* Additive terms. */
  tmp = DSIN(51.2 + 20.2 * T);
  L_prime += 0.000233 * tmp;
  M += -0.001778 * tmp;
  M_prime += 0.000817 * tmp;
  delta += 0.002011 * tmp;

  venus_term = 0.003964 * DSIN(346.560 + 132.870 * T - 0.0091731 * T*T );
  L_prime += venus_term;
  M_prime += venus_term;
  delta += venus_term;
  f += venus_term;

  tmp = DSIN(Omega);
  L_prime += 0.001964 * tmp;
  M_prime += 0.002541 * tmp;
  delta += 0.001964 * tmp;
  f += -0.024691 * tmp;

  f += -0.004328 * DSIN(Omega + 275.05 - 2.30 * T);

  e = 1 - 0.002495 * T - 0.00000752 * T*T;

  /* Bring these angles within 0 to 360 degrees. */
  M = into_range(M);
  M_prime = into_range(M_prime);
  delta = into_range(delta);
  f = into_range(f);
  Omega = into_range(Omega);


  /* Calculate lambda, the ecliptical longitude of the Moon's center. */
  lambda = L_prime + 6.288750 * DSIN(M_prime)
    + 1.274018 * DSIN(2*delta - M_prime)
      + 0.658309 * DSIN(2*delta)
	+ 0.213616 * DSIN(2 * M_prime)
	  - e * 0.185596 * DSIN(M)
	    - 0.114336 * DSIN(2*f)
	      + 0.058793 * DSIN(2*delta - 2*M_prime)
		+ e * 0.057212 * DSIN(2*delta - M - M_prime)
		  + 0.053320 * DSIN(2*delta + M_prime)
		    + e * 0.045874 * DSIN(2*delta - M)
		      + e * 0.041024 * DSIN(M_prime - M)
			- 0.034718 * DSIN(delta)
			  - e * 0.030465 * DSIN(M + M_prime)
			    + 0.015326 * DSIN(2*delta - 2*f)
			      - 0.012528 * DSIN(2*f + M_prime)
				- 0.010980 * DSIN(2*f - M_prime)
				  + 0.010674 * DSIN(4*delta - M_prime)
				    + 0.010034 * DSIN(3*M_prime)
				      + 0.008548 * DSIN(4*delta - 2*M_prime);
  lambda +=
    - e * 0.007910 * DSIN(M - M_prime + 2*delta)
      - e * 0.006783 * DSIN(2*delta + M)
	+ 0.005162 * DSIN(M_prime - delta)
	  + e * 0.005000 * DSIN(M + delta)
	    + e * 0.004049 * DSIN(M_prime - M + 2*delta)
	      + 0.003996 * DSIN(2*M_prime + 2*delta)
		+ 0.003862 * DSIN(4*delta)
		  + 0.003665 * DSIN(2*delta - 3*M_prime)
		    + e * 0.002695 * DSIN(2*M_prime - M)
		      + 0.002602 * DSIN(M_prime - 2*f - 2*delta) 
			+ e * 0.002396 * DSIN(2*delta - M - 2*M_prime)
			  - 0.002349 * DSIN(M_prime + delta)
			    + e*e * 0.002249 * DSIN(2*delta - 2*M)
			      - e * 0.002125 * DSIN(2*M_prime + M)
				- e*e * 0.002079 * DSIN(2*M)
				  + e*e * 0.002059 * DSIN(2*delta - M_prime - 2*M)
				    - 0.001773 * DSIN(M_prime + 2*delta - 2*f)
				      - 0.001595 * DSIN(2*f + 2*delta)
					+ e * 0.001220 * DSIN(4*delta - M - M_prime);
  lambda +=
    - 0.001110 * DSIN(2*M_prime + 2*f)
      + 0.000892 * DSIN(M_prime - 3*delta)
	- e * 0.000811 * DSIN(M + M_prime + 2*delta)
	  + e * 0.000761 * DSIN(4*delta - M - 2*M_prime)
	    + e*e * 0.000717 * DSIN(M_prime - 2*M)
	      + e*e * 0.000704 * DSIN(M_prime - 2*M -2*delta)
		+ e * 0.000693 * DSIN(M - 2*M_prime + 2*delta)
		  + e * 0.000598 * DSIN(2*delta - M - 2*f)
		    + 0.000550 * DSIN(M_prime + 4*delta)
		      + 0.000538 * DSIN(4*M_prime)
			+ e * 0.000521 * DSIN(4*delta - M)
			  + 0.000486 * DSIN(2*M_prime - delta);
  lambda = into_range(lambda);
  
 /* Calculate beta, the ecliptical latitude of the Moon's center. */
  beta = 5.128189 * DSIN(f)
    + 0.280606 * DSIN(M_prime + f)
      + 0.277693 * DSIN(M_prime - f)
	+ 0.173238 * DSIN(2*delta - f)
	  + 0.055413 * DSIN(2*delta + f - M_prime)
	    + 0.046272 * DSIN(2*delta - f - M_prime)
	      + 0.032573 * DSIN(2*delta + f)
		+ 0.017198 * DSIN(2*M_prime + f)
		  + 0.009267 * DSIN(2*delta + M_prime - f)
		    + 0.008823 * DSIN(2*M_prime - f)
		      + e * 0.008247 * DSIN(2*delta - M - f)
			+ 0.004323 * DSIN(2*delta - f - 2*M_prime)
			  + 0.004200 * DSIN(2*delta + f + M_prime)
			    + e * 0.003372 * DSIN(f - M - 2*delta)
			      + e * 0.002472 * DSIN(2*delta + f - M - M_prime)
				+ e * 0.002222 * DSIN(2*delta + f - M)
				  + e * 0.002072 * DSIN(2*delta - f - M - M_prime)
				    + e * 0.001877 * DSIN(f - M + M_prime)
				      + 0.001828 * DSIN(4*delta - f - M_prime);
  beta +=
    - e * 0.001803 * DSIN(f + M)
      - 0.001750 * DSIN(3*f)
	+ e * 0.001570 * DSIN(M_prime - M - f)
	  - 0.001487 * DSIN(f + delta)
	    - e * 0.001481 * DSIN(f + M + M_prime)
	      + e * 0.001417 * DSIN(f - M - M_prime)
		+ e * 0.001350 * DSIN(f - M)
		  + 0.001330 * DSIN(f - delta)
		    + 0.001106 * DSIN(f + 3*M_prime)
		      + 0.001020 * DSIN(4*delta - f)
			+ 0.000833 * DSIN(f + 4*delta - M_prime)
			  + 0.000781 * DSIN(M_prime - 3*f)
			    + 0.000670 * DSIN(f + 4*delta - 2*M_prime)
			      + 0.000606 * DSIN(2*delta - 3*f)
				+ 0.000597 * DSIN(2*delta + 2*M_prime - f)
				  + e * 0.000492 * DSIN(2*delta + M_prime - M - f);
  beta +=
    0.000450 * DSIN(2*M_prime - f - 2*delta)
      + 0.000439 * DSIN(3*M_prime - f)
	+ 0.000423 * DSIN(f + 2*delta + 2*M_prime)
	  + 0.000422 * DSIN(2*delta - f - 3*M_prime)
	    - e * 0.000367 * DSIN(M + f + 2*delta - M_prime)
	      - e * 0.000353 * DSIN(M + f + 2*delta)
		+ 0.000331 * DSIN(f + 4*delta)
		  + e * 0.000317 * DSIN(2*delta + f - M + M_prime)
		    + e*e * 0.000306 * DSIN(2*delta - 2*M - f)
		      - 0.000283 * DSIN(M_prime + 3*f);
  
  omega1 = 0.0004664 * DCOS(Omega);
  omega2 = 0.0000754 * DCOS(Omega + 275.05 - 2.30 * T);
  
  beta *= (1 - omega1 - omega2);
  
  moon_pi = .950724+.051818*DCOS(M_prime)+.009531*DCOS(2*delta-M_prime)
    +.007843*DCOS(2*delta)+ .002824*DCOS(2*M_prime)
      +.000857*DCOS(2*delta+M_prime)+e*.000533*DCOS(2*delta-M)
	+ e*.000401*DCOS(2*delta-M_prime-M)+e*.00032*DCOS(M_prime-M)
	  -.000271*DCOS(delta) -e*.000264*DCOS(M+M_prime)
	    -.000198*DCOS(2*f-M_prime);

  moon_pi += .000173*DCOS(3*M_prime)+.000167*DCOS(4*delta-M_prime)
    -e*.000111*DCOS(M)+.000103*DCOS(4*delta-2*M_prime)
      -.000084*DCOS(2*M_prime-2*delta)-e*.000083*DCOS(2*delta+M)
	+.000079*DCOS(2*delta+2*M_prime)+.000072*DCOS(4*delta)+
	  e*.000064*DCOS(2*delta-M+M_prime)-e*.000063*DCOS(2*delta+M-M_prime)+
	    e*.000041*DCOS(M+delta);

  moon_pi += e*.000035*DCOS(2*M_prime-M)-.000033*DCOS(3*M_prime-2*delta)-
    .00003*DCOS(M_prime+delta)-.000029*DCOS(2*(f-delta))
      -e*.000029*DCOS(2*M_prime+M)+e*e*.000026*DCOS(2*(delta-M))
	-.000023*DCOS(2*(f-delta)+M_prime)+ e*.000019*DCOS(4*delta-M-M_prime);

  moon_pi = into_range(moon_pi);


  epsilon = obl_jd(jd);


  moon_data->lambda = lambda;
  moon_data->beta = beta;
  moon_data->moon_pi = moon_pi;

  moon_data->Delta = 6378.14 / DSIN(moon_pi); /* in km */
  moon_data->size = 2*(358482800.0 / moon_data->Delta);
  alpha = RAD_TO_DEG * atan2(DSIN(lambda)*DCOS(epsilon)
			     - DTAN(beta) * DSIN(epsilon),
			     DCOS(lambda));
  delta = RAD_TO_DEG * asin(DSIN(beta)*DCOS(epsilon)
			    + DCOS(beta)*DSIN(epsilon)*DSIN(lambda));
  alpha = into_range(alpha);
  moon_data->alpha = alpha;
  moon_data->delta = delta;
  precess(2000.0 - (2451545.0 - jd) / 365.25,
	  2000.0, alpha, delta, &alpha2000, &delta2000);
  moon_data->alpha2000 = alpha2000;
  moon_data->delta2000 = delta2000;

  tmp = RAD_TO_DEG * acos(DCOS(lambda - sun_data.Theta) * DCOS(beta));
  
  moon_data->phase = 180 - tmp
    - 0.1468 * DSIN(tmp) * (1 - 0.0549 * DSIN(M_prime))/(1 - DSIN(M));
  moon_data->illum_frac = (1 + DCOS(moon_data->phase))/2.0;

  moon_data->chi = /* position angle of bright limb */
    DATAN2(DCOS(sun_data.delta) * DSIN(sun_data.alpha - alpha),
	  DCOS(delta) * DSIN(sun_data.delta)
	  - DSIN(delta) * DCOS(sun_data.delta) * DCOS(sun_data.alpha - alpha));


  moon_data->mag = -12.74 - 2.5 * log10(moon_data->illum_frac);
  /* Doesn't allow for opposition surge */
}

