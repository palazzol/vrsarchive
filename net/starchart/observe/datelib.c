/*
 * datelib.c
 * date routines
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
  "$Header: /home/Vince/cvs/net/starchart/observe/datelib.c,v 1.1 1990-03-30 16:37:45 vrs Exp $";
#endif


#include <stdio.h>
#include <math.h>
#include <ctype.h>

#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif /* SYSV */

#include "date.h"

/* Calendar = month, day, year, where day is type double
   jd = julian date, type double
   str = string format.  Year is optional, default must be specified.

functions:
cal_to_jd(day, month, year, &jd);
cal_to_str(day, month, year, string);  String must be char string[15];
jd_to_cal(jd, &day, &month, &year);
jd_to_str(jd, string);  String must be char string[15];
str_to_jd(string, default_year, &jd);
                              default_year is default year,
                              used if not specified in string
str_to_cal(string, &day, &month, &year);
*/

void cal_to_jd(day, month, year, jd_p)
     int month, year;
     double day;
     double *jd_p;
{
  int b, d, m, y;
  long c;

  m = month;
  y = (year < 0) ? year + 1 : year;
  if (month < 3) {
    m += 12;
    y -= 1;
  }

  if ((year < 1582) ||
      (year == 1582 && ((month < 10)
			|| ((month == 10) && (day < 15)))))
    b = 0;
  else {
    int a;
    a = y/100;
    b = 2 - a + a/4;
  }

  if (y < 0)
    c = (long)((365.25*y) - 0.75) + 1720995L;
  else
    c = (long)(365.25*y) + 1720995L;

  d = 30.6001*(m+1);

  *jd_p = (b + c + d + day - 0.5);
}

static char *mname[] = {
  "",  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void cal_to_str(day, month, year, str)
     int month, year;
     double day;
     char str[];
{
  sprintf(str, "%.2f %s %d", day, mname[month], year);
}


void jd_to_cal(jd, day, month, year)
     double jd;
     double *day;
     int *month, *year;
{
  double d, f;
  double i, a, b, ce, g;

  d = jd + 0.5 - 2415020L;
  i = floor(d);
  f = d-i;
  if (f == 1) {
    f = 0;
    i += 1;
  }

  if (jd > 2299160L) {		/* After the date change in 1582 */
    a = floor((i/36524.25)+.9983573)+14;
    i += 1 + a - floor(a/4.0);
  }

  b = floor((i/365.25)+.802601);
  ce = i - floor((365.25*b)+.750001)+416;
  g = floor(ce/30.6001);
  *month = g - 1;
  *day = ce - floor(30.6001*g)+f;
  *year = b + 1899;

  if (g > 13.5)
    *month = g - 13;
  if (*month < 2.5)
    *year = b + 1900;
  if (*year < 1)
    *year -= 1;
}


void jd_to_str(jd, str)
     double jd;
     char str[];
{
  int year, month;
  double day;

  jd_to_cal(jd, &day, &month, &year);

  sprintf(str, "%.2f %s %d", day, mname[month], year);
}





/* Given date string and the year to be zero, give the day number:
E.g.
"100.3"             1988 => 100.3
"1/1/1988"          1988 => 1.0
"2/1/1989"          1988 => 398.0
"October 3"         1988 => 
*/
void str_to_jd(s, year, jd_p)
     char *s;
     int year;
     double *jd_p;
{
  int mo, yr;
  double dy;
  double daynum;

  /* see if it's just a number already */
  if ((index(s, ' ')) == NULL) {
    *jd_p = atof(s);
    return;
  };

  str_to_cal(s, &dy, &mo, &yr);
  if (yr == 0) yr = year;

  daynum = dy;
  cal_to_jd(daynum, mo, yr, jd_p);
}

/* interpret string as month, day and perhaps year.
year 0 if no year in string, there is no year 0 in the calender.
E.g.:
Oct. 1                  => 1 10 0
IX 23                   => 23 9 0
2/12/1987               => 12 2 1987
5-2-88                  => 2 5 1988
January 23 1988         => 23 1 1988
*/

void str_to_cal(s, dy, mo, yr)
     char *s;
     double *dy;
     int *mo, *yr;
{
  int i, n;
  char string1[100], string2[100];

  /* Is there a dash? */
  if ((index(s, '-')) != NULL) { /* YES */
    n = sscanf(s, "%lf-%d-%d", dy, mo, yr);
    if (n != 3) *yr = 0;
    return;
  }

  /* Is there a slash? */
  if ((index(s, '/')) != NULL) { /* YES */
    n = sscanf(s, "%lf/%d/%d", dy, mo, yr);
    if (n != 3) *yr = 0;
    return;
  }

  /* Try Month day year or Month day, year
     or day Month year */
  /* first eliminate commas */
  i = 0;
  while (s[i]) {
    if (s[i] == ',') s[i] = ' ';
    i++;
  }

  n = sscanf(s, "%s %s %d", string1, string2, yr);
  if (n < 2) {
    fprintf(stderr, "Can't understand date %s\n", s);
    exit(1);
  } else if (n == 2) *yr = 0;

  *mo = tr_mname(string1);
  if (*mo < 1) {
    *mo = tr_mname(string2);
    *dy = atof(string1);
  } else
    *dy = atof(string2);

  if (*mo < 1) {
    fprintf(stderr, "Can't understand date %s\n", s);
    exit(1);
  }
}

/* translate date string to month number */
int tr_mname(s)
     char *s;
{
  int i = -1;
  while (s[++i]) if (isupper(s[i])) s[i] = tolower(s[i]);

  if (!strcmp(s,"jan")
      || !strcmp(s,"jan.")
      || !strcmp(s,"i")
      || !strcmp(s,"january"))
    return 1;

  if (!strcmp(s,"feb")
      || !strcmp(s,"feb.")
      || !strcmp(s,"ii")
      || !strcmp(s,"febuary"))
    return 2;

  if (!strcmp(s,"mar")
      || !strcmp(s,"mar.")
      || !strcmp(s,"iii")
      || !strcmp(s,"march"))
    return 3;

  if (!strcmp(s,"apr")
      || !strcmp(s,"apr.")
      || !strcmp(s,"iv")
      || !strcmp(s,"april"))
    return 4;

  if (!strcmp(s,"may")
      || !strcmp(s,"v"))
    return 5;

  if (!strcmp(s,"jun")
      || !strcmp(s,"jun.")
      || !strcmp(s,"vi")
      || !strcmp(s,"june"))
    return 6;

  if (!strcmp(s,"jul")
      || !strcmp(s,"jul.")
      || !strcmp(s,"vii")
      || !strcmp(s,"july"))
    return 7;

  if (!strcmp(s,"aug")
      || !strcmp(s,"aug.")
      || !strcmp(s,"viii")
      || !strcmp(s,"august"))
    return 8;

  if (!strcmp(s,"sep")
      || !strcmp(s,"sep.")
      || !strcmp(s,"ix")
      || !strcmp(s,"september"))
    return 9;

  if (!strcmp(s,"oct")
      || !strcmp(s,"oct.")
      || !strcmp(s,"x")
      || !strcmp(s,"october"))
    return 10;

  if (!strcmp(s,"nov")
      || !strcmp(s,"nov.")
      || !strcmp(s,"xi")
      || !strcmp(s,"november"))
    return 11;

  if (!strcmp(s,"dec")
      || !strcmp(s,"dec.")
      || !strcmp(s,"xii")
      || !strcmp(s,"december"))
    return 12;

  return atoi(s);
}

