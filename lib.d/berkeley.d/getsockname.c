/*
 *	%W% %G%
 *	Get socket name.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sd.h>
#include <errno.h>

getsockname(s, name, namelen)
int s;
struct sockaddr_un *name;
int *namelen;
{

  if (_get_sock_addr(s) == NULL)
    return(-1);
  /*
   *	An unbound socket has no name.  A connected socket does not refer
   *	to it's own name.  Instead, the name of the socket to which it is
   *	connected is given.  We would have to attach that socket, extract
   *	our own name, and return it.  For now we implement the Berkeley
   *	UNIX domain semantics and always return a null name.
  */
  name->sun_path[0] = '\0';
  *namelen = 0;
  return(0);
}
