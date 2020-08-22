/*
 * main.c
 * main() for combined planet, precession, ephemeris, asteroid, comet program
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
 * email: ccount@athena.mit.edu
 */

#ifndef  lint
static char rcsid[] =
  "$Header: /home/Vince/cvs/net/starchart/observe/main.c,v 1.1 1990-03-30 16:37:49 vrs Exp $";
#endif


#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>

#include <time.h>
#ifndef ATARI_ST
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <types.h>
#include <time.h>
#endif

#include "observe.h"
#include "date.h"


#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef MAXPATHLEN
#define MAXPATHLEN 1025
#endif


void parse_command();



char *progname;

/* Time */
double start_date;		/* JD of start */
int start_month, start_year;
double start_day;
double end_date;		/* JD of end */
int end_month, end_year;
double end_day;
double interval_days = 1.0;

/* Place */
double obs_lon = -71.0;
double obs_lat = 42.0;
double obs_zone = -4.0;
double obs_height = 50.0;	/* Meters */

/* files */
char *outfile_root = "planet";
FILE *o_sif, *o_star, *o_eph, *o_obs, *o_sat, *o_sat_PS, *o_altaz;
int do_sif = TRUE, do_star = TRUE, do_eph = TRUE, do_obs = TRUE,
    do_altaz = TRUE;

/* input file Formats */

int file_input = FALSE;
fformat_t tr_format();
char *infile_name = "";
FILE *infile;
fformat_t in_type;



/* objects */
int planet_list[] = {
  FALSE,			/* Mercury */
  FALSE,			/* Venus */
  FALSE,			/* Mars */
  FALSE,			/* Jupiter */
  FALSE,			/* Saturn */
  FALSE,			/* Uranus */
  FALSE				/* Neptune */
};

char *obj_name = "";

#ifndef MAXOBJECTS
#define MAXOBJECTS 1500
#endif


planet_data_t planets[7];
#ifndef ATARI_ST
wanderer_data_t bodies[MAXOBJECTS];
#else
wanderer_data_t *bodies;
#endif

int nbodies;
sun_data_t sun_data;
moon_data_t moon_data;

#ifndef ATARI_ST
obj_data_t objects[MAXOBJECTS];
#else
obj_data_t *objects;
#endif
int nobjects;


/* (7 planets + objects) each  rise rise20 rise30 transit set30 set20 set
   + sunrise sunset moonrise moonset and twilights */
#define EVENTSSIZE ((7+MAXOBJECTS)*MAXEVENTS+6)
observe_t events[EVENTSSIZE];
int nevents;

int index_events[EVENTSSIZE];
double event_times[EVENTSSIZE];

/* Controls */
int satellites = FALSE;
int invert_sats = FALSE;

double morntwil(), evetwil(), sunrise(), sunset(), moonrise(), moonset(),
  objrise(), objset(), objrise20(), objset20(), objrise30(), objset30();

main(argc, argv)
     int argc;
     char **argv;
{
  int i;
  char filename[MAXPATHLEN];
  double jd;
  char datestr[15];
  int one_day;

#ifdef ATARI_ST
  if ((bodies = (wanderer_data_t *)
       lmalloc((long)MAXOBJECTS * (long)sizeof(wanderer_data_t))
       ) == (wanderer_data_t *)0)
    {
      perror("malloc");
      exit(1);
    }
  if ((objects = (obj_data_t *)
       lmalloc((long)MAXOBJECTS * (long)sizeof(obj_data_t))
       ) == (wanderer_data_t *)0)
    {
      perror("malloc");
      exit(1);
    }
#endif


  get_time();
  obs_zone = now_zone();
  jd_to_cal(start_date, &start_day, &start_month, &start_year);
  end_date = start_date;
  end_day = start_day;
  end_month = start_month;
  end_year = start_year;

  parse_command(argc, argv);


  one_day = (start_date == end_date);

  jd_to_str(start_date, datestr);

  if (file_input) {
    if ((infile = fopen(infile_name, "r")) == NULL) {
      fprintf(stderr, "%s: could not open file %s for reading\n",
	      progname, infile_name);
      exit(1);
    };

    /* input data from infile */
    switch (in_type) {
    case emp:
    case empb:
    case aa:
    case st:
    case iau:
      read_table(infile, in_type);
      nbodies = 1;
      bodies[0].name = obj_name;
      bodies[0].orbit_type = tabulated;
      break;
    case ell_e:
      read_elliptical(infile, bodies, &nbodies, MAXOBJECTS);
      break;
    case par_e:
      read_parabolic(infile, bodies, &nbodies, MAXOBJECTS);
      break;
    case obj:
      read_objects(infile, objects, &nobjects, MAXOBJECTS);
      break;
    default:
      break;
    };
  };

  /* open output files */
  strncpy(filename, outfile_root, MAXPATHLEN-5);
#ifndef ATARI_ST
  strcat(filename, ".star");
#else
  strcat(filename, ".str");
#endif
  if (do_star)
    if ((o_star = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "%s: could not open file %s for writing\n",
	      progname, filename);
      exit(1);
    };
  strncpy(filename, outfile_root, MAXPATHLEN-5);
  strcat(filename, ".sif");
  if (do_sif)
    if ((o_sif = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "%s: could not open file %s for writing\n",
	      progname, filename);
      exit(1);
    };
  strncpy(filename, outfile_root, MAXPATHLEN-5);
  strcat(filename, ".eph");
  if (do_eph)
    if ((o_eph = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "%s: could not open file %s for writing\n",
	      progname, filename);
      exit(1);
    };
  strncpy(filename, outfile_root, MAXPATHLEN-5);
  strcat(filename, ".obs");
  if (do_obs)
    if ((o_obs = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "%s: could not open file %s for writing\n",
	      progname, filename);
      exit(1);
    };
  strncpy(filename, outfile_root, MAXPATHLEN-5);
#ifndef ATARI_ST
  strcat(filename, ".altaz");
#else
  strcat(filename, ".alt");
#endif
  if (do_altaz)
    if ((o_altaz = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "%s: could not open file %s for writing\n",
	      progname, filename);
      exit(1);
    };
  if (satellites) {
    strncpy(filename, outfile_root, MAXPATHLEN-5);
    strcat(filename, ".sat");
    if ((o_sat = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "%s: could not open file %s for writing\n",
	      progname, filename);
      exit(1);
    };
    strncpy(filename, outfile_root, MAXPATHLEN-7);
#ifdef MSDOS
    strcat(filename, ".sPS");
#else
#ifdef ATARI_ST
    strcat(filename, ".sps");
#else
    strcat(filename, ".sat_PS");
#endif /* Atari */
#endif /* msdos */

    if ((o_sat_PS = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "%s: could not open file %s for writing\n",
	      progname, filename);
      exit(1);
    };
  };


  /* for each time */
  for (jd = start_date; jd <= end_date; jd += interval_days) {
    /* Calculate sun position, moon position */
    sun_pos(jd, &sun_data);
    moon_pos(jd, sun_data, &moon_data);

    /* Calculate special events in the day */
    sun_data.rise_hour =
      events[0].hour = sunrise(jd, obs_lon, obs_lat, obs_zone, obs_height,
			       sun_data);
    events[0].object = "sun";
    events[0].event = rise;
    events[0].special = rise_special;
    sun_data.transit_hour =
      suntransit(jd, obs_lon, obs_lat, obs_zone, obs_height, sun_data);
    sun_data.set_hour =
      events[1].hour = sunset(jd, obs_lon, obs_lat, obs_zone, obs_height,
			      sun_data);
    events[1].object = "sun";
    events[1].event = set;
    events[1].special = set_special;
    events[2].hour = morntwil(jd, obs_lon, obs_lat, obs_zone, obs_height,
			      sun_data);
    events[2].object = "morning twilight";
    events[2].event = special;
    events[2].special = morning_twilight;
    events[3].hour = evetwil(jd, obs_lon, obs_lat, obs_zone, obs_height,
			     sun_data);
    events[3].object = "evening twilight";
    events[3].event = special;
    events[3].special = evening_twilight;

    moon_data.rise_hour =
      events[4].hour = moonrise(jd, obs_lon, obs_lat, obs_zone, obs_height,
				moon_data);
    events[4].object = "moon";
    events[4].event = rise;
    events[4].special = rise_special;
    moon_data.transit_hour =
      moontransit(jd, obs_lon, obs_lat, obs_zone, obs_height, moon_data);
    moon_data.set_hour =
      events[5].hour = moonset(jd, obs_lon, obs_lat, obs_zone, obs_height,
			       moon_data);
    events[5].object = "moon";
    events[5].event = set;
    events[5].special = set_special;
    nevents = 6;

    /* for each planet, calculate position and events */
    for (i = 0; i < 7; i++)
      if (planet_list[i]) {
	planet_pos(jd, sun_data, i, &planets[i]);
	calc_events(planets[i].eventlist,
		    &planets[i].rise_hour, &planets[i].transit_hour,
		    &planets[i].set_hour,
		    jd, obs_lon, obs_lat, obs_zone, obs_height,
		    planets[i].name, planets[i].alpha, planets[i].delta);
	add_events(events, &nevents, planets[i].eventlist);
      } else {
	planets[i].name = "";
      };

    /* for each object, calculate position and events */
    for (i = 0; i < nobjects; i++) {
      obj_pos(jd, &objects[i]);	/* calculates alpha and delta
				   alpha2000 and delta2000 */
      calc_events(objects[i].eventlist,
		  &objects[i].rise_hour, &objects[i].transit_hour,
		  &objects[i].set_hour,
		  jd, obs_lon, obs_lat, obs_zone, obs_height,
		  objects[i].name, objects[i].alpha, objects[i].delta);
      add_events(events, &nevents, objects[i].eventlist);
    };

    /* for each body, calculate position and events */
    for (i = 0; i < nbodies; i++) {
      if (bodies[i].orbit_type == elliptical_orbit)
	elliptical_pos(jd, sun_data, &bodies[i]);
      else if (bodies[i].orbit_type == parabolic_orbit)
	parabolic_pos(jd, sun_data, &bodies[i]);
      else if (bodies[i].orbit_type == tabulated)
	tabulated_pos(jd, sun_data, &bodies[i]);

      calc_events(bodies[i].eventlist,
		  &bodies[i].rise_hour, &bodies[i].transit_hour,
		  &bodies[i].set_hour,
		  jd, obs_lon, obs_lat, obs_zone, obs_height,
		  bodies[i].name, bodies[i].alpha, bodies[i].delta);
      add_events(events, &nevents, bodies[i].eventlist);
    };

    /* Sort event list */
    for (i = 0; i < nevents; i++)
      if (events[i].hour > 12.0)
	event_times[i] = events[i].hour;
      else {
	events[i].hour += 3.94 / 60; /* add a day */
	event_times[i] = events[i].hour + 24;
      };
    HeapSort0(event_times, index_events, nevents);

    if (do_sif || do_star)
      out_sif(o_sif, o_star, do_sif, do_star, one_day,
	      sun_data, moon_data,
	      planets, bodies, nbodies, objects, nobjects);
				/* Output .sif and .star files for object(s) */

    if (do_obs)
      out_obs(o_obs, one_day, jd, events, index_events, nevents);
				/* Output observability file for object(s) */

    if (do_eph)
      out_eph(o_eph, one_day, jd,
	      sun_data, moon_data,
	      planets, bodies, nbodies, objects, nobjects);
				/* Output ephemeris file for object(s) */

    if (do_altaz)
      out_altaz(o_altaz, one_day, jd,
		obs_lon, obs_lat, obs_zone, obs_height,
		sun_data, moon_data,
		planets, bodies, nbodies, objects, nobjects);
				/* Output altaz file for object(s) */

    if (satellites)
      if (one_day)
	out_sat(o_sat, o_sat_PS, one_day, invert_sats, jd, moon_data, planets);
				/* Output satellite, .PS file for satellites */
  };
  if (!one_day) {		/* need to calculate moving objects for each
				 object for each day */
    /* Sun */
    for (jd = start_date; jd <= end_date; jd += interval_days) {
      /* Calculate sun position, moon position */
      sun_pos(jd, &sun_data);
      if (do_sif || do_star)
	out_sif_sun(o_sif, o_star, do_sif, do_star, one_day,
		    sun_data, jd);
      /* Output .sif and .star files for sun */
    };
    /* Moon */
    for (jd = start_date; jd <= end_date; jd += interval_days) {
      /* Calculate sun position, moon position */
      sun_pos(jd, &sun_data);
      moon_pos(jd, sun_data, &moon_data);
      if (do_sif || do_star)
	out_sif_moon(o_sif, o_star, do_sif, do_star, one_day,
		     moon_data, jd);
      /* Output .sif and .star files for moon */
    };


    /* for each planet, calculate position and events */
    for (i = 0; i < 7; i++)
      if (planet_list[i]) {
	for (jd = start_date; jd <= end_date; jd += interval_days) {
	  /* Calculate sun position */
	  sun_pos(jd, &sun_data);

	  planet_pos(jd, sun_data, i, &planets[i]);
	  /* Output .sif and .star files for planet */
	  if (do_sif || do_star)
	    out_sif_planet(o_sif, o_star, do_sif, do_star, one_day,
			   planets[i], jd);
	}
      };

    /* Output satellite positions for each day */
    if (satellites) {
      /* Jupiter */
      for (jd = start_date; jd <= end_date; jd += interval_days) {
	sun_pos(jd, &sun_data);
	moon_pos(jd, sun_data, &moon_data);
	planet_pos(jd, sun_data, 3, &planets[3]);
	out_sat(o_sat, o_sat_PS, one_day, invert_sats, jd,
		moon_data, planets);
	/* Output satellite, .PS file for satellites */
      };

      /* Saturn */
      for (jd = start_date; jd <= end_date; jd += interval_days) {
	sun_pos(jd, &sun_data);
	moon_pos(jd, sun_data, &moon_data);
	planet_pos(jd, sun_data, 4, &planets[4]);
	out_sat(o_sat, o_sat_PS, one_day, invert_sats, jd,
		moon_data, planets);
	/* Output satellite, .PS file for satellites */
      };
      out_sat_end(o_sat, o_sat_PS, invert_sats, start_date, interval_days);
				/* Close the postscript (showpage) */
    };

    /* for each body, calculate position and events */
    for (i = 0; i < nbodies; i++) {
      for (jd = start_date; jd <= end_date; jd += interval_days) {
	/* Calculate sun position */
	sun_pos(jd, &sun_data);
	
	if (bodies[i].orbit_type == elliptical_orbit)
	  elliptical_pos(jd, sun_data, &bodies[i]);
	else if (bodies[i].orbit_type == parabolic_orbit)
	  parabolic_pos(jd, sun_data, &bodies[i]);
	else if (bodies[i].orbit_type == tabulated)
	  tabulated_pos(jd, sun_data, &bodies[i]);

	if (do_sif || do_star)
	  out_sif_body(o_sif, o_star, do_sif, do_star, one_day,
		       bodies[i], jd);
	/* Output .sif and .star files for planet */
      };
    };
  };
  exit(0);
}


#define GMT1970 2440587.5
/*
  Get current time
  set start_date, start_day, start_month, start_year, start_hour;
*/
void get_time()
{
  time_t timeofday;		/* time_t is usually "long" */
#ifndef ATARI_ST
  struct timeb tp;

  ftime(&tp);

  timeofday = tp.time;
#else
  time(&timeofday);
#endif

  start_date = timeofday / 86400.0 + 2440587.5; /* this is now the true JD */
}



/* rather system dependent */

/* Method 1 subtract local time from gmt */
double now_zone()
{
  time_t timeofday;
  struct timeb tp;
  struct tm *localtm, *gmttm;
  double local_hour, gmt_hour;
  ftime(&tp);

  timeofday = tp.time;

  localtm = localtime(&timeofday);
  local_hour = localtm->tm_sec/3600.0 + localtm->tm_min/60.0
    + localtm->tm_hour;
  gmttm = gmtime(&timeofday);
  gmt_hour = gmttm->tm_sec/3600.0 + gmttm->tm_min/60.0
    + gmttm->tm_hour;

  return (local_hour - gmt_hour);
}

/* Method 2, for SYSV?
double now_zone()
{
  extern long timezone;

  return (-timezone/3600.0);
}
*/
/* Method 3, for non SYSV?
double now_zone()
{
  struct timeval tp;
  struct timezone tzp;

  gettimeofday(&tp, &tzp);

  return (-tzp.tz_minuteswest/60.0 + tzp.tz_dsttime);
}
*/
/* For ATARI_ST */
/*double now_zone()
{
  extern long timezone;
  tm_t *tp;
  localtime(&tp);

  return (-timezone/3600.0);
}
*/


int now_year()
{
  time_t timeofday;		/* time_t is usually "long" */
#ifndef ATARI_ST
  struct timeb tp;
  double jd;
  double day;
  int month, year;

  ftime(&tp);

  timeofday = tp.time;

  jd = timeofday / 86400.0 + 2440587.5; /* this is now the true JD */

  jd_to_cal(jd, &day, &month, &year);
#else /* Atari */
  int year;
  tm_t *tp;
  time(&timeofday);
  tp=localtime(&timeofday);
  year=tp->tm_year;
#endif
  return year;
}


/* is not an argument switch, i.e. not "-[a-Z]" */
int notarg(s)
char *s;
{
  return (!((s[0] == '-') && (isalpha(s[1]))));
}

void usage()
{
  fprintf(stderr,
 "%s: -o[aeios] outfile_root -p [MVmJsUN] -s[i] -n name -f filename filetype\n\
      -d \"start date\" \"end date\" -z time_zone_value -l latitude -m meridian\n",
	  progname);
}

void parse_command(argc, argv)
int argc;
char **argv;
{
  int i, j;
  char *cp1;
  int mo, yr;
  double dy;

  progname = argv[0];

  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-')
      switch (argv[i][1]) {
      case 'm':
				/* Meridian */
				/* One to three arguments, floating point */
	if (((i+1) < argc) && (notarg(argv[i+1]))) {
	  obs_lon = atof(argv[i+1]);
	  i++;
	  if (((i+1) < argc) && (notarg(argv[i+1]))) {
	    if (obs_lon > 0)
	      obs_lon += atof(argv[i+1]) / 60.0;
	    else
	      obs_lon -= atof(argv[i+1]) / 60.0;
	    i++;
	    if (((i+1) < argc) && (notarg(argv[i+1]))) {
	      if (obs_lon > 0)
		obs_lon += atof(argv[i+1]) / 3600.0;
	      else
		obs_lon -= atof(argv[i+1]) / 3600.0;
	      i++;
	    };
	  };
	};
	break;
      case 'l':
				/* Latitude */
				/* One to three arguments, floating point */
	if (((i+1) < argc) && (notarg(argv[i+1]))) {
	  obs_lat = atof(argv[i+1]);
	  i++;
	  if (((i+1) < argc) && (notarg(argv[i+1]))) {
	    if (obs_lat > 0)
	      obs_lat += atof(argv[i+1]) / 60.0;
	    else
	      obs_lat -= atof(argv[i+1]) / 60.0;
	    i++;
	    if (((i+1) < argc) && (notarg(argv[i+1]))) {
	      if (obs_lat > 0)
		obs_lat += atof(argv[i+1]) / 3600.0;
	      else
		obs_lat -= atof(argv[i+1]) / 3600.0;
	      i++;
	    };
	  };
	};
	break;
      case 'z':			/* time Zone */
	if (((i+1) < argc) && (notarg(argv[i+1]))) {
	  i++;
	  obs_zone = atof(argv[i]);
	};
	break;
      case 'a':			/* altitude, meters */
	if (((i+1) < argc) && (notarg(argv[i+1]))) {
	  i++;
	  obs_height = atof(argv[i]);
	};
	break;
      case 'd':
				/* start_day [end_day [interval]] */
	if (((i+1) < argc) && (notarg(argv[i+1]))) {
	  str_to_cal(argv[i+1], &dy, &mo, &yr);
	  if (yr == 0) yr = start_year;
	  start_day = dy;
	  start_month = mo;
	  start_year = yr;
	  cal_to_jd(start_day, start_month, start_year, &start_date);
	  i++;
	  if (((i+1) < argc) && (notarg(argv[i+1]))) {
	    str_to_cal(argv[i+1], &dy, &mo, &yr);
	    if (yr == 0) yr = end_year;
	    end_day = dy;
	    end_month = mo;
	    end_year = yr;
	    cal_to_jd(end_day, end_month, end_year, &end_date);
	    i++;
	    if (((i+1) < argc) && (notarg(argv[i+1]))) {
	      interval_days = atof(argv[i+1]);
	      i++;
	    };
	  } else {
	    end_date = start_date;
	    end_day = start_day;
	    end_month = start_month;
	    end_year = start_year;
	  };
	};
	break;
      case 'o':
	cp1 = &argv[i][2];
	if (*cp1) {
	  /* Select output files */
	  do_sif = do_star = do_eph = do_obs = do_altaz = FALSE;
	  while (*cp1)
	    switch (*cp1++) {
	    case 'a':
	      do_altaz = TRUE;
	      break;
	    case 'e':
	      do_eph = TRUE;
	      break;
	    case 'i':
	      do_sif = TRUE;
	      break;
	    case 'o':
	      do_obs = TRUE;
	      break;
	    case 's':
	      do_star = TRUE;
	      break;
	    default:
	      break;
	    };
	} else
	  do_sif = do_star = do_eph = do_obs = do_altaz = TRUE;

				/* outfile_root */
	if (((i+1) < argc) && (notarg(argv[i+1]))) {
	  outfile_root = argv[i+1];
	  i++;
	};
	break;
      case 'p':
				/* [planetlist_string] */
	if (((i+1) < argc) && (notarg(argv[i+1]))) {
	  j = 0;
	  i++;
#ifdef SYSV
	  if (strchr(argv[i], 'M')) planet_list[0] = TRUE; 
	  if (strchr(argv[i], 'V')) planet_list[1] = TRUE; 
	  if (strchr(argv[i], 'm')) planet_list[2] = TRUE; 
	  if (strchr(argv[i], 'J')) planet_list[3] = TRUE; 
	  if (strchr(argv[i], 's')) planet_list[4] = TRUE; 
	  if (strchr(argv[i], 'U')) planet_list[5] = TRUE; 
	  if (strchr(argv[i], 'N')) planet_list[6] = TRUE; 
#else
	  if (index(argv[i], 'M')) planet_list[0] = TRUE; 
	  if (index(argv[i], 'V')) planet_list[1] = TRUE; 
	  if (index(argv[i], 'm')) planet_list[2] = TRUE; 
	  if (index(argv[i], 'J')) planet_list[3] = TRUE; 
	  if (index(argv[i], 's')) planet_list[4] = TRUE; 
	  if (index(argv[i], 'U')) planet_list[5] = TRUE; 
	  if (index(argv[i], 'N')) planet_list[6] = TRUE; 
#endif
	} else {
	  for (j = 0; j < 7; j++)
	    planet_list[j] = TRUE;
	};
	break;
      case 's':
				/* either -s or -si */
	satellites = TRUE;
	for (j = 0; j < 7; j++)
	  planet_list[j] = TRUE;
	if (argv[i][2] == 'i')
	  invert_sats = TRUE;
	break;
      case 'n':
	if (((i+1) < argc) && (notarg(argv[i+1]))) {
	  i++;
	  obj_name = argv[i];
	};
				/* name */
	break;
      case 'f':
				/* file format */
	if (((i+2) < argc) && (notarg(argv[i+1])) && (notarg(argv[i+2]))) {
	  infile_name = argv[i+1];
	  in_type = tr_format(argv[i+2]);
	  if (in_type == no_format) {
	    fprintf(stderr, "%s: format %s not recognized.\n", progname, 
		    argv[i+2]);
	    exit(1);
	  };
	  i += 2;
	  file_input = TRUE;
	};
	break;
      case 'h':
	usage();
	break;
      default:
	fprintf(stderr, "%s: error, unrecognized command line argument %s\n",
		progname, argv[i]);
	usage();
	break;
      };
}


/* translate string to format */
fformat_t tr_format(s)
char *s;
{
  int i = -1;
  while (s[++i]) if (isupper(s[i])) s[i] = tolower(s[i]);

  if (!strcmp(s, "emp")) return emp;
  else if (!strcmp(s, "empb")) return empb;
  else if (!strcmp(s, "aa")) return aa;
  else if (!strcmp(s, "st")) return st;
  else if (!strcmp(s, "iau")) return iau;
  else if (!strcmp(s, "ell_e")) return ell_e;
  else if (!strcmp(s, "par_e")) return par_e;
  else if (!strcmp(s, "obj")) return obj;
  else return no_format;
}

