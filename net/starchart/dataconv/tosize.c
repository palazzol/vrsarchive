/*
 * tosize -- convert object size to two character size code
 *           for use in starchart databases.
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/dataconv/tosize.c,v 1.1 1990-03-30 16:37:17 vrs Exp $";



#include <stdio.h>
#include <math.h>
#include <ctype.h>

char *usage =
"%s: [-[d][m][s]] size [size ...]\n";

main(argc, argv)
     int argc;
     char *argv[];
{
  int i, j, nszs;
  double sze, sdbl;
  double cfact[3];
  int scan_ret;
  char *tosizestr();

  cfact[0] = 1.0;
  cfact[1] = 1.0;
  cfact[2] = 1.0;

  j = 1;
  nszs = 1;

  if (argv[1][0] == '-') {
    i = 1;
    j++;
    while (argv[1][i]) {
      switch (argv[1][i]) {
      case 'd':
	cfact[i-1] = 3600.0;
	break;
      case 'm':
	cfact[i-1] = 60.0;
	break;
      case 's':
	break;
      default:
	fprintf(stderr, usage, argv[0]);
	exit(1);
	break;
      }
      i++;
    }
    nszs = i - 1;
  }

  while (j < argc) {
    for (i = 0, sze = 0.0; i < nszs; i++)
      sze += atof(argv[j + i])*cfact[i];
    printf("%s\n", tosizestr(sze));
    j += nszs;
  }

  scan_ret = 0;
  while (scan_ret != EOF) {
    for (i = 0, sze = 0.0; i < nszs; i++) {
      scan_ret = scanf("%lf", &sdbl);
      sze += sdbl*cfact[i];
    }
    if (scan_ret != EOF)
      printf("%s\n", tosizestr(sze));
  }
}

char retsize[3];

char *tosizestr(sze)
double sze;
{
  if (sze < 99.5) sprintf(retsize, "%2d",(int) (sze + 0.5));
  else if (sze < 995.0) {
    sprintf(retsize, "%2d",(int) ((sze + 5.0)/10.0));
    retsize[0] = 'A' + retsize[0] - '0' - 1;
  }  else if (sze < 9950.0) {
    sprintf(retsize, "%2d",(int) ((sze + 50.0)/100.0));
    retsize[0] = 'J' + retsize[0] - '0' - 1;
  }  else if (sze < 89500.0) {
    sprintf(retsize, "%2d",(int) ((sze + 500.0)/1000.0));
    retsize[0] = 'S' + retsize[0] - '0' - 1;
  } else sprintf(retsize , "Z9");

  return retsize;
}
