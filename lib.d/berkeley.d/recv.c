/*
 *	%W% %G%
 *	Receive a message on a connected socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>
#include <sys/sd.h>

int
recv(s, buf, len, flags)
int s;				/* Pseudo-descriptor for socket		*/
char *buf;			/* Buffer to receive data into		*/
int len;			/* Length of buffer provided		*/
int flags;			/* Flags for the operation		*/
{ char *addr;			/* Address of struct socket		*/
  register struct socket *sptr;
  struct sockaddr_un s_addr;
  int s_len;
  int ret;

  addr = _get_sock_addr(s);
  if (addr == NULL)
    return(-1);
  _enter_seg(addr);
  sptr = (struct socket *)addr;
  if (! (sptr->so_state & SS_ISCONNECTED)) {
    _leave_seg(addr);
    errno = ENOTCONN;
    return(-1);
  }
  _leave_seg(addr);
  return(recvfrom(s, buf, len, flags, &s_addr, &s_len));
}
