/*
 *	%W% %G%
 *	Listen for connections.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>

listen(s, backlog)
int s, backlog;
{ struct socket *sock_addr;

  if ((sock_addr = (struct socket *)_get_sock_addr(s)) == NULL)
    return(-1);
  /*
   * get the socket
  */
  _enter_seg((char *)sock_addr);
  if (!(sock_addr->so_type & SOCK_STREAM)) {
    errno = EOPNOTSUPP;
    return(-1);
  }
  if ((backlog < 0) || (backlog > SOMAXCONN))
    backlog = SOMAXCONN;
  sock_addr->so_qlimit = backlog;
  _leave_seg((char *)sock_addr);
  return(0);
}
