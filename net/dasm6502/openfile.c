/* home-brew file extensions         */
/* author:   Mark R. Rinfret         */
/* date:     04/19/84                */
/* filename: openfile.c              */

#include <stdio.h>
#include <string.h>

FILE *
openfile(name,how)
char *name, *how;

{
  char *xname;
  char c;
  unsigned dvc;

  xname = name; /* copy pointer */
  dvc = 8;  /* default is device 8 */
  if ((c=*xname) == '#') {
    xname++;
    if (((c=*xname) == '8') || ((c=*xname) == '9')) {
      xname++;
      dvc = (8 + (c - '8'));
      if (*xname==':') xname++;
      else {
        printf("missing ':' in device specification\n%s\n",xname);
        return NULL;
        }
      }
    else {
      printf("illegal device number:\n%s\n",xname);
      return NULL;
      }
    }

#ifndef M_XENIX
  device(dvc);
#endif M_XENIX
  return (fopen(xname,how));
}
