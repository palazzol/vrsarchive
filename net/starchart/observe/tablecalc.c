/*
 * tablecalc.c
 * calculate positions by interpolation of a table
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
  "$Header: /home/Vince/cvs/net/starchart/observe/tablecalc.c,v 1.1 1990-03-30 16:38:04 vrs Exp $";
#endif


#include <stdio.h>
#include <math.h>
#include <ctype.h>

#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif /* SYSV */
#include "observe.h"
#include "date.h"


#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

static double polint();
static void readfile();

#define MAXPTS 300

static double dates[MAXPTS], ra[MAXPTS], dec[MAXPTS],
  mag[MAXPTS], r[MAXPTS], Delta[MAXPTS];
static int Nread;
static double from_equinox;

void tabulated_pos(jd, sun_data, body)
     double jd;
     sun_data_t sun_data;
     wanderer_data_t *body;
{
  double alpha, delta;
  double alpha2000, delta2000;
  double error;

  alpha = 15.0 * polint(dates, ra, Nread, jd, &error);
  delta = polint(dates, dec, Nread, jd, &error);
  precess(from_equinox, 2000.0, alpha, delta, &alpha2000, &delta2000);

  body->alpha = alpha;
  body->delta = delta;
  body->alpha2000 = alpha2000;
  body->delta2000 = delta2000;
  body->mag = polint(dates, mag, Nread, jd, &error);
  body->r = polint(dates, r, Nread, jd, &error);
  body->Delta = polint(dates, Delta, Nread, jd, &error);
  body->body_type = no_type;

  /* Unknown */
  body->beta = -999.0;
  body->psi = -999.0;
}

void read_table(infile, in_type)
     FILE *infile;
     fformat_t in_type;
{
  double ra_1, ra_2;
  int i;
  int yr;
  int adjust_ra;



  /* Prepare for epoch change */
  switch (in_type) {
  case emp:
    from_equinox = 1950.0;
    break;
  case empb:
    from_equinox = 1950.0;
    break;
  case aa:
    from_equinox = 2000.0;
    break;
  case st:
    from_equinox = 2000.0;
    break;
  case iau:
    from_equinox = 1950.0;
    break;
  default:
    break;
  }

  yr = now_year();
  readfile(infile, in_type, yr);


  /* Adjust ra to be continuous */
  /* If the difference between adjacent numbers is greater than
     the difference if you subtract 24 from the larger,
     then shift all by 24 hrs */
  adjust_ra = FALSE;
  for (i = 0; i < Nread-1; i++) {
    ra_2 = MAX(ra[i],ra[i+1]);
    ra_1 = MIN(ra[i],ra[i+1]);
    if (fabs(ra_2-ra_1) > fabs(ra_2-24-ra_1))
      adjust_ra = TRUE;
  }
  if (adjust_ra)
    for (i = 0; i < Nread; i++)
      if (ra[i] < 12.0) ra[i] += 24.0;
}


static void readfile(fptr, fform, year)
     FILE *fptr;
     fformat_t fform;
     int year;
{
  char lbuf[1000], *bufp;
  char s1[100], s2[100], s3[100], s4[100], s5[100], s6[100], s7[100], s8[100],
       s9[100], s10[100], s11[100];
  int i, n;
  int mo, last_mo, yr;
  double dy;

  mo = -1;
  last_mo = -2;
  dy = -1;
  yr = year;

  fgets(lbuf, 1000, fptr);
  n = sscanf(lbuf, "year %d", &i);
  if (n == 1) yr = i;
  fgets(lbuf, 1000, fptr);
  n = sscanf(lbuf, "year %d", &i);
  if (n == 1) yr = i;
  fgets(lbuf, 1000, fptr);
  n = sscanf(lbuf, "year %d", &i);
  if (n == 1) yr = i;


  switch (fform) {
  case emp:
/*
month day ra ra     dec dec
1     2    3 4       5  6
VIII  17   0 23.7   -13 48
      27   0 19.1   -14 51

*/
    i = 0;
    while (!feof(fptr)) {
      bufp = fgets(lbuf, 1000, fptr);
      if (bufp != NULL)
	n = sscanf(bufp, "%s %s %s %s %s %s",
		   s1, s2, s3, s4, s5, s6);
      else n = 0;
      if (n < 5) continue;
      if ((lbuf[0] == ' ') || (lbuf[0] == '\t')) {
	/* use current month */
	dy = atof(s1);

	ra[i] = atof(s2) + atof(s3)/60.0;
	dec[i] = atof(s4);
	dec[i] += (atof(s5)/60.0)*(s4[0] == '-' ? -1 : 1);
				/* Beware of the sign */

	mag[i] = 0.0;		/* don't know it */
	r[i] = -1.0;		/* don't know it */
	Delta[i] = -1.0;	/* don't know it */
      } else {
	mo = tr_mname(s1);
	if (mo < last_mo) yr++;
	last_mo = mo;
	dy = atof(s2);

	ra[i] = atof(s3) + atof(s4)/60.0;
	dec[i] = atof(s5);
	dec[i] += (atof(s6)/60.0)*(s5[0] == '-' ? -1 : 1);
				/* Beware of the sign */

	mag[i] = 0.0;		/* don't know it */
	r[i] = -1.0;		/* don't know it */
	Delta[i] = -1.0;	/* don't know it */
      }

      cal_to_jd(dy, mo, yr, &dates[i]);
      i++;
    };
    Nread = i;
    break;
  case empb:
/*
month day ra ra     dec dec r    Delta   V    beta
1     2   3  4     5    6    7    8       9   10
X      6  23 48.36 -18  5.5 2.958 2.033   7.8 8.8
      16  23 41.45 -18 13.1 2.955 2.095   8.0 11.6
*/
    i = 0;
    while (!feof(fptr)) {
      bufp = fgets(lbuf, 1000, fptr);
      if (bufp != NULL)
	n = sscanf(bufp, "%s %s %s %s %s %s %s %s %s %s",
		   s1, s2, s3, s4, s5, s6, s7, s8, s9, s10);
      else n = 0;
      if (n < 5) continue;
      if ((lbuf[0] == ' ') || (lbuf[0] == '\t')) {
	/* use current month */
	dy = atof(s1);

	ra[i] = atof(s2) + atof(s3)/60.0;
	dec[i] = atof(s4);
	dec[i] += (atof(s5)/60.0)*(s4[0] == '-' ? -1 : 1);
				/* Beware of the sign */

	r[i] = atof(s6);
	Delta[i] = atof(s7);
	mag[i] = atof(s8);
/*	beta[i] = atof(s9);*/
      } else {
	mo = tr_mname(s1);
	if (mo < last_mo) yr++;
	last_mo = mo;
	dy = atof(s2);

	ra[i] = atof(s3) + atof(s4)/60.0;
	dec[i] = atof(s5);
	dec[i] += (atof(s6)/60.0)*(s5[0] == '-' ? -1 : 1);
				/* Beware of the sign */

	r[i] = atof(s7);
	Delta[i] = atof(s8);
	mag[i] = atof(s9);
/*	beta[i] = atof(s10);*/
      }

      cal_to_jd(dy, mo, yr, &dates[i]);

      i++;
    };
    Nread = i;
    break;
  case aa:
/*
date     RA          DEC       r    Ephemeris Transit
1     2  3  4  5     6  7  8  9     10  11
Oct.  1  23 54 52.1 -17 37 21 2.011 23 10.0
      3  23 53 16.1 -17 42 32 2.019 23 00.5
*/
    i = 0;
    while (!feof(fptr)) {
      bufp = fgets(lbuf, 1000, fptr);
      if (bufp != NULL)
	n = sscanf(bufp, "%s %s %s %s %s %s %s %s %s %s %s",
		   s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11);
      else n = 0;
      if (n < 7) continue;
      if ((lbuf[0] == ' ') || (lbuf[0] == '\t')) {
	/* use current month */
	dy = atof(s1);

	ra[i] = atof(s2) + atof(s3)/60.0 + atof(s4)/3600.0;
	dec[i] = atof(s5);
	dec[i] += (atof(s6)/60.0 + atof(s7)/3600.0)*(s5[0] == '-' ? -1 : 1);
				/* Beware of the sign */

	r[i] = atof(s8);

	mag[i] = 0.0;		/* don't know it */
	Delta[i] = -1.0;	/* don't know it */
      } else {
	mo = tr_mname(s1);
	if (mo < last_mo) yr++;
	last_mo = mo;
	dy = atof(s2);

	ra[i] = atof(s3) + atof(s4)/60.0 + atof(s5)/3600.0;
	dec[i] = atof(s6);
	dec[i] += (atof(s7)/60.0 + atof(s8)/3600.0)*(s6[0] == '-' ? -1 : 1);
				/* Beware of the sign */
	r[i] = atof(s9);
	
	mag[i] = 0.0;		/* don't know it */
	Delta[i] = -1.0;	/* don't know it */
      }

      cal_to_jd(dy, mo, yr, &dates[i]);

      i++;
    };
    Nread = i;
    break;
  case st:
/*
Date    RA       DEC   Elongation Mag.
1    2  3  4     5 6   7   8
Oct  1  14 07.6  4 06  25  5.4
     6  14 52.3  3 28  32  6.5
*/
    i = 0;
    while (!feof(fptr)) {
      bufp = fgets(lbuf, 1000, fptr);
      if (bufp != NULL)
	n = sscanf(bufp, "%s %s %s %s %s %s %s %s",
		   s1, s2, s3, s4, s5, s6, s7, s8);
      else n = 0;
      if (n < 5) continue;
      if ((lbuf[0] == ' ') || (lbuf[0] == '\t')) {
	/* use current month */
	dy = atof(s1);

	ra[i] = atof(s2) + atof(s3)/60.0;
	dec[i] = atof(s4);
	dec[i] += (atof(s5)/60.0)*(s4[0] == '-' ? -1 : 1);
				/* Beware of the sign */
	mag[i] = atof(s7);

	r[i] = -1.0;		/* don't know it */
	Delta[i] = -1.0;	/* don't know it */
      } else {
	mo = tr_mname(s1);
	if (mo < last_mo) yr++;
	last_mo = mo;
	dy = atof(s2);

	ra[i] = atof(s3) + atof(s4)/60.0;
	dec[i] = atof(s5);
	dec[i] += (atof(s6)/60.0)*(s5[0] == '-' ? -1 : 1);
				/* Beware of the sign */
	mag[i] = atof(s8);

	r[i] = -1.0;		/* don't know it */
	Delta[i] = -1.0;	/* don't know it */
      }

      cal_to_jd(dy, mo, yr, &dates[i]);

      i++;
    };
    Nread = i;
    break;
  case iau:
/*
date       RA          DEC      \Delta     r     (V or m1)
     *Only gives \Delta r and V or m1  for every other date*

1      2   3  4        5  6     7        8       9
Oct.   6   14 50.24    3 41.0   1.204    0.643   12.5
      11   15 29.62    2 48.8
      16   16 04.01    1 54.1   1.339    0.889   14.1
      21   16 33.96    1 02.5
*/
    i = 0;
    while (!feof(fptr)) {
      bufp = fgets(lbuf, 1000, fptr);
      if (bufp != NULL)
	n = sscanf(bufp, "%s %s %s %s %s %s %s %s %s",
		   s1, s2, s3, s4, s5, s6, s7, s8, s9);
      else n = 0;
      if (n < 5) continue;
      if ((lbuf[0] == ' ') || (lbuf[0] == '\t')) {
	/* use current month */
	dy = atof(s1);

	ra[i] = atof(s2) + atof(s3)/60.0;
	dec[i] = atof(s4);
	dec[i] += (atof(s5)/60.0)*(s4[0] == '-' ? -1 : 1);
				/* Beware of the sign */
	if (n > 5) {
	  Delta[i] = atof(s6);
	  r[i] = atof(s7);
	  mag[i] = atof(s8);
	} else {
	  Delta[i] =
	  r[i] =
	  mag[i] =
	    -9999.0;
	}
      } else {
	mo = tr_mname(s1);
	if (mo < last_mo) yr++;
	last_mo = mo;
	dy = atof(s2);

	ra[i] = atof(s3) + atof(s4)/60.0;
	dec[i] = atof(s5);
	dec[i] += (atof(s6)/60.0)*(s5[0] == '-' ? -1 : 1);
				/* Beware of the sign */
	if (n > 5) {
	  Delta[i] = atof(s7);
	  r[i] = atof(s8);
	  mag[i] = atof(s9);
	} else {
	  Delta[i] =
	  r[i] =
	  mag[i] =
	    -9999.0;
	}
      }

      cal_to_jd(dy, mo, yr, &dates[i]);

      i++;
    };
    Nread = i;

    /* now fix mag, r, and Delta */
    for (i = 0; i < Nread; i++) {
      if (mag[i] < -1000.0)
	mag[i] = (mag[i-1] + mag[i-1])/2.0;
      if (r[i] < -1000.0)
	r[i] = (r[i-1] + r[i-1])/2.0;
      if (Delta[i] < -1000.0)
	Delta[i] = (Delta[i-1] + Delta[i-1])/2.0;
    };
    break;
  default:
    break;
  }
}



/* Maximum number of points */
#define NMAX MAXPTS

/* Polymomial interpolation (or extrapolation) function
Adopted from Numerical Recipies
Given arrays xa and ya, each of length N, and given a value x, this routine 
returns a value y and an error estimate dy.  If P(x) is the polynomial of
degree N-1 such that P(xa[i]) = ya[i], i = 1, ..., N then the returned
value y = P(x);  */
static double polint(xa, ya, N, x, dy)
     double xa[], ya[];		/* value arrays */
     int N;			/* Order */
     double x;			/* x point desired */
     double *dy;		/* error estimate */
{
  int i, NS, M;
  double dif, dift, c[NMAX], d[NMAX], ho, hp, w, den, y;

  NS = 0;
  dif = fabs(x-xa[0]);
  /* Find the index NS of closest table entry */
  for (i = 0; i < N; i++) {
    dift = fabs(x - xa[i]);
    if (dift < dif) {
      NS = i;
      dif = dift;
    }
    /* initialize the 'tableau' of c's and d's */
    c[i] =
    d[i] = ya[i];
  }
  y = ya[NS];			/* Initial approximation to Y. */
  NS--;
  /* For each column of the tableay, we loop over the current c's and d's
     and update them */
  for (M = 1; M <= N-1; M++) {
    for (i = 0; i < N-M; i++) {
      ho = xa[i] - x;
      hp = xa[i+M] - x;
      w = c[i+1] - d[i];
      den = ho - hp;
      if (den == 0.0) {
	/* This can occur only if two input xa's are identical */
	fprintf(stderr, "Error in routine polint\n");
	exit(1);
      }
      den = w/den;
      d[i] = hp*den;
      c[i] = ho*den;
    }

    /* After each column in the tableau is completed, we decide
       which correction, c or d, we want to add to our accumulating value of y,
       i.e. which path to take throught the tableau -- forking up or down.
       We do this in such a way as to take the most "straight line" route
       through the tableau to its apex, updating NS accordingly to keep track
       of where we are.  This route keeps the apartial approximations centered
       (insofar as possible) on the target x.  The last dy added is thus
       the error indication. */
    if ((NS+1)*2 < N-M)
      *dy = c[NS+1];
    else {
      *dy = d[NS];
      NS--;
    }
    y = y + *dy;
  }

  return y;
}
