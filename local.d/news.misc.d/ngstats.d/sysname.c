/*
 *	%W% %G%
 *
 * David H. Kaufman
 *	Ala 4.1, but now only an interface to gethostname().
 * Vincent R. Slyngstad
 *	Added interface to /etc/systemid for XENIX.
*/
#include <stdio.h>

/* #define GETHOSTNAME		/* If you have gethostname()	*/

#define SYSNAMELEN 255
static char systemname[SYSNAMELEN];
extern char *strchr();

char *sysname()
#ifdef GETHOSTNAME
{ if (!*systemname)
    if (gethostname(systemname, SYSNAMELEN))
      return(NULL);
#endif
#ifdef M_XENIX
{ register int fd;

  if (!*systemname) {
    fd = open("/etc/systemid",0);
    read(fd,systemname,sizeof(systemname));
    close(fd);
    *(strchr(systemname, '\n')) = '\0';
  }
#endif

    return(systemname);
}
