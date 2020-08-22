/*
 *	Kludge to read dayfile[] and nightfile[] from the files of the same
 *	name.  Ideally we would compile these in, but the C-MERGE compiler
 *	won't allow more than 64K of initialized data.
*/

#include <stdio.h>
#include <ctype.h>
#include "externs.h"

extern char *malloc();

struct room dayfile[NUMOFROOMS+1];
struct room nightfile[NUMOFROOMS+1];
static FILE *fd;
static char buf[1024];

whitespace()
{ register char c;

  while (c = getc(fd), isspace(c));
  ungetc(c,fd);
}

getnum()
{ int i;

  fscanf(fd," %d",&i);
  return(i);
}

char *
getlin()
{ register char *p = buf;

  whitespace();
  while (*p = getc(fd), *p != '\n')
    p++;
  *p = '\0';
  p = malloc(p-buf+1);
  strcpy(p,buf);
  return(p);
}

char *
getstr()
{ register char *p = buf;
  register char bol;

  whitespace();
  bol = 1;
  while (*p = getc(fd)) {
    if (*p == '\n')
      if (bol)
        break;
      else
        bol = 1;
    else
      bol = 0;
    p++;
  }
  *p = '\0';
  p = malloc(p-buf+1);
  strcpy(p,buf);
  return(p);
}

error(fmt, a, b, c, d)
char *fmt;
{ printf(fmt, a, b, c, d);
  exit(1);
}

char *
unctrl(c)
register c;
{ static char buf[5];

  if (c == EOF)
    return "EOF";
  c = (unsigned char) c;
  if (c < ' ')
    sprintf(buf, "^%c", c + '@');
  else if (c < 0x7f)
    sprintf(buf, "%c", c);
  else if (c == 0x7f)
    return "^?";
  else
    sprintf(buf, "\\%03o", c);
  return buf;
}

init1(str,fil)
struct room *str;
char *fil;
{ register int i, room;

  if ((fd = fopen(fil, "r")) == 0) {
    perror(fil);
    exit(1);
  }
  while ((i = getc(fd)) != '\n' && i != EOF)
    ;	/* Eat data file's SCCS header */
  for (room = 1; room <= NUMOFROOMS; room++) {
    i = getnum();
    if (room != i)
    	error("Room number mismatch (%d should be %d).", i, room);
    if (i = getc(fd), i != ':')
      error("Format error (character '%s').", unctrl(i));
    str++;				/* Don't use str[0]	*/
    str->name = getlin();
    str->north = getnum();
    str->south = getnum();
    str->east = getnum();
    str->west = getnum();
    str->up = getnum();
    str->access = getnum();
    str->down = getnum();
    str->flyhere = getnum();
    str->desc = getstr();
  }
  fclose(fd);
  return;
}

initrms()
{ init1(dayfile,PATH(dayfile));
  init1(nightfile,PATH(nightfile));
}
