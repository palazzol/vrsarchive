/*
 *	%W% %G%
 *	Write on a file or a socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

write(fd, buf, nbytes)
int fd;
char *buf;
unsigned nbytes;
{ if (_socktab[fd] == NULL)
    return(_write(fd, buf, nbytes));
  return(send(fd, buf, nbytes, 0));
}
