/*
 *	%W% %G%
*/
#include <stdio.h>

extern FILE *_pfile;

_doprnt(fmt,args,junk)
char *fmt;
int ***args;
FILE *junk;
{ _pfile = junk;
  _print(fmt, &args);
}
