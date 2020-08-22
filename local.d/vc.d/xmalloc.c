/*
 * A safer saner malloc, for careless programmers
 */

#include <stdio.h>
#include <curses.h>
#include "sc.h"

extern char *malloc();
#ifdef SYSV3
extern void free();
extern void exit();
#endif

char *
xmalloc(n)
unsigned n;
{
register char *ptr;

if ((ptr = malloc(n + sizeof(int))) == NULL)
    fatal("xmalloc: no memory");
*((int *) ptr) = 12345;		/* magic number */
return(ptr + sizeof(int));
}

xfree(p)
char *p;
{
if (p == NULL)
    fatal("xfree: NULL");
p -= sizeof(int);
if (*((int *) p) != 12345)
    fatal("xfree: storage not malloc'ed");
free(p);
}

fatal(str)
char *str;
{
    deraw();
    (void) fprintf(stderr,"%s\n", str);
    exit(1);
}
