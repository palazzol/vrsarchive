/*
 * observe.h
 * Types etc. needed for observe program
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
/*
 * $Header: /home/Vince/cvs/net/starchart/observe/observe.h,v 1.1 1990-03-30 16:37:52 vrs Exp $
 */


typedef enum {
  rise, rise_20, rise_30, transit, set_30, set_20, set, special, nothing
} event_t;
#define MAXEVENTS 7
/* order is RISE = 0 to SET = 6 with TRANSIT = 3 */

typedef enum {
  rise_special, set_special, morning_twilight, evening_twilight, not_special
} special_event_t;

typedef struct {
  double hour;			/* of event */
  char *object;			/* event is happening to */
  event_t event;
  special_event_t special;
				/* sun or moon rise or set,
				   twilight */
} observe_t;

typedef enum {
  planet_orbit,			/* Planet */
  elliptical_orbit,		/* Elliptical Orbit */
  parabolic_orbit,		/* Parabolic Orbit */
  tabulated,			/* Tabulated */
  no_orbit
} orbtype_t;

typedef enum {
  asteroid,
  comet,
  no_type
} bodytype_t;

typedef struct {
  double epoch_jd;
  double equinox_year;
  double a;			/* semimajor axis, A.U. */
  double e;			/* eccentricity */
  double i;			/* inclination (degrees) */
  double omega;			/* argument of perihelion */
  double Omega;			/* longitude of ascending node */
  double n;			/* mean motion (degrees/day) */
  double M;			/* Mean anomaly at epoch */
} elliptical_elements_t;

typedef struct {
  double perihelion_date;	/* Time of passage in perihelion */
  double equinox_year;
  double q;			/* perihelion distance, A.U. */
  double i;			/* inclination (degrees) */
  double omega;			/* argument of perihelion */
  double Omega;			/* longitude of ascending node */
} parabolic_elements_t;


typedef struct {
  double beta_e;		/* planetocentric declination of Earth */
  double p_n;			/* Position angle of the axis,
				   measured east from north */
  double lambda_e;		/* planetographic longitude of the central
				   meridian, measured in the direction
				   opposite to the dir. of rotation */
} rotation_elements_t;

typedef struct {
  char *name;
  char *type;
  char *color;
  double alpha, delta;		/* position in equinox of date */
  double alpha2000, delta2000;	/* position in equinox 2000.0 */
  double l, b;			/* ecliptical longitude and latitude */
  double lambda, beta;		/* geocentric longitude and latitude */
  double Cen;			/* Center */
  double psi;			/* elongation */
  double r, Delta;		/* Distance to sun, and earth */
  double mag, phase, size;	/* magnitude, phase (degrees) size (arcsec) */
  double illum_frac;		/* illuminated fraction of disk */
  double chi;			/* position angle of bright limb */
  rotation_elements_t rotation_elements;
				/* beta_e, p_n, lambda_e */
  observe_t eventlist[MAXEVENTS]; /* events: rise, set, etc. */
  double rise_hour, set_hour, transit_hour;
				/* times of these events */
} planet_data_t;

typedef struct {
  char *name;
  double alpha, delta;		/* position in equinox of date */
  double alpha2000, delta2000;	/* position in equinox 2000.0 */
  double beta;			/* Phase angle */
  double psi;			/* Elongation */
  double r, Delta;		/* Distance to sun, and earth */
  double mag;			/* magnitude */
  orbtype_t orbit_type;		/* Orbit type */
  elliptical_elements_t elliptical_elements;
  parabolic_elements_t parabolic_elements;
  bodytype_t body_type;		/* Body type: asteroid or comet */
  double H, G;			/* Magnitude of asteroid */
  double g, kappa;		/* Magnitude of a comet */
  observe_t eventlist[MAXEVENTS]; /* events: rise, set, etc. */
  double rise_hour, set_hour, transit_hour;
				/* times of these events */
} wanderer_data_t;

typedef struct {
  char *name;
  double alpha, delta;		/* position in equinox of date */
  double alpha2000, delta2000;	/* position in equinox 2000.0 */
  double R, Theta;		/* Distance to earth, Theta equinox of date */
  double size;			/* size (arcsec) */
  double rise_hour, set_hour, transit_hour;
				/* times of these events */
} sun_data_t;

typedef struct {
  char *name;
  double alpha, delta;		/* position in equinox of date */
  double alpha2000, delta2000;	/* position in equinox 2000.0 */
  double lambda, beta, moon_pi;	/* geocentric longitude and latitude
				   and horizontal parallax */
  double Delta;			/* Distance to earth (km) */
  double mag, phase, size;	/* magnitude, phase (degrees) size (arcsec) */
  double illum_frac;		/* illuminated fraction */
  double chi;			/* position angle of bright limb */
  double rise_hour, set_hour, transit_hour;
				/* times of these events */
} moon_data_t;


typedef struct {
  char *name;
  char type[3];
  double alpha, delta;		/* position in original equinox */
  double equinox;
  double alpha2000, delta2000;	/* position in equinox 2000.0 */
  double mag;
  double size;
  observe_t eventlist[MAXEVENTS]; /* events: rise, set, etc. */
  double rise_hour, set_hour, transit_hour;
				/* times of these events */
} obj_data_t;


typedef enum {
  emp,				/* Eph. Minor Planet */
  empb,				/* Eph. Minor Planet, bright */
  aa,				/* Astro. Alman */
  st,				/* Sky and Telescope */
  iau,				/* IAU circular */
  ell_e,			/* Elliptical Orbital elements:
				   Calculate ephemeris */
  par_e,			/* Parabolic Orbital elements:
				   Calculate ephemeris */
  obj,				/* Fixed object */
  no_format
} fformat_t;


typedef struct {
  double dx, dy, dz;		/* relative to planet,
				   units of equatorial radius */
  double dalpha, ddelta;	/* displacements in RA and dec. */
  double mag;
  char *name;
} sat_t;


/* functions */
void precess();
void anom_calc();
double gmst0_degrees();
void read_elliptical(), elliptical_pos();
void read_parabolic(), parabolic_pos();
void read_table(), tabulated_pos();
void read_objects(), obj_pos();
void calc_events(), add_events();
void get_time();
double now_zone();
int now_year();
void planet_pos(), moon_pos(), sun_pos();
void out_obs(), out_eph(), out_sat(), out_sat_end();
void out_sif(), out_sif_planet(), out_sif_body(),
  out_sif_sun(), out_sif_moon();
void HeapSort(), HeapSort0();
double obl_jd(), obl_year();
void sun_rect();
double into_range();

double sunrise(), suntransit(), sunset(), moonrise(), moontransit(), moonset(),
  morntwil(), evetwil(),
  objrise(), objrise20(), objrise30(), objtransit(),
  objset30(), objset20(), objset();
  

/* put here for VMS and pure-ANSI systems */
double atof();
