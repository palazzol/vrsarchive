/*
 *	%W% %G%
 *	Read a file or a socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

read(fd, buf, nbytes)
int fd;
char *buf;
unsigned nbytes;
{ if (_socktab[fd] == NULL)
    return(_read(fd, buf, nbytes));
  return(recv(fd, buf, nbytes, 0));
}
