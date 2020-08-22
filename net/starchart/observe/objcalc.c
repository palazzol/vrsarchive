/*
 * objcalc.c
 * read objects data file, calculate position in equinox 2000.0
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
  "$Header: /home/Vince/cvs/net/starchart/observe/objcalc.c,v 1.1 1990-03-30 16:37:51 vrs Exp $";
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

extern char *malloc();

/* Format
Name
R.A
dec.
mag type size epoch
*/

void read_objects(infile, objects, nobjects_p, maxobjects)
     FILE *infile;
     obj_data_t objects[];
     int *nobjects_p;
     int maxobjects;
{
  char in_line[1000], *linep;
  int i, j, nscanned;
  double ra1, ra2, ra3, ra;
  double dec1, dec2, dec3, dec;
  double alpha, delta, alpha2000, delta2000;
  char typestr[10];

  j = 0;
  while (!feof(infile)) {
    linep = fgets(in_line, 1000, infile);
    if (linep != NULL) {
      /* remove trailing '\n' */
      linep[strlen(linep)-1] = '\0';
      objects[j].name = (char *) malloc((unsigned) strlen(linep)+1);
      if (objects[j].name != NULL)
	strcpy(objects[j].name, linep);
      else objects[j].name = "object";
      nscanned = 1;
    } else nscanned = 0;
    if (nscanned < 1) continue;

    linep = fgets(in_line, 1000, infile);
    if (linep != NULL)
      nscanned = sscanf(linep, "%lf %lf %lf",
		 &ra1, &ra2, &ra3);
    else nscanned = 0;
    if (nscanned < 1) continue;

    ra = ra1 + ra2/60.0 + ra3/3600.0;
    objects[j].alpha = ra*15.0;

    linep = fgets(in_line, 1000, infile);
    if (linep != NULL)
      nscanned = sscanf(linep, "%lf %lf %lf",
		 &dec1, &dec2, &dec3);
    else nscanned = 0;
    if (nscanned < 1) continue;

    dec1 = fabs(dec1);
    dec = dec1 + dec2/60.0 + dec3/3600.0;
    for (i=0; linep[i]; i++)
      if (linep[i] == '-') dec = -dec;
    objects[j].delta = dec;

    linep = fgets(in_line, 1000, infile);
    objects[j].size = 0.0;
    objects[j].equinox = 2000.0;
    if (linep != NULL)
      nscanned = sscanf(linep, "%lf %2s %lf %lf",
			&objects[j].mag, typestr, &objects[j].size,
			&objects[j].equinox);
    else nscanned = 0;
    if (nscanned < 1) continue;

    if (typestr[0]) {
      objects[j].type[0] = typestr[0];
      if (typestr[1])
	objects[j].type[1] = typestr[1];
      else
	objects[j].type[1] = ' ';
    } else {
      objects[j].type[0] = ' ';
      objects[j].type[1] = ' ';
    };
    objects[j].type[2] = '\0';

    if (objects[j].equinox != 2000.0) {
      alpha = objects[j].alpha;
      delta = objects[j].delta;

      precess(objects[j].equinox, 2000.0,
	      alpha, delta, &alpha2000, &delta2000);

      objects[j].alpha2000 = alpha2000;
      objects[j].delta2000 = delta2000;
    } else {
      objects[j].alpha2000 = objects[j].alpha;
      objects[j].delta2000 = objects[j].delta;
    };

    j++;
  };
  *nobjects_p = j;
}

void obj_pos(jd, object)
     double jd;
     obj_data_t *object;
{
}

