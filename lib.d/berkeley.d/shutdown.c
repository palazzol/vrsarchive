/*
 *	%W% %G%
 *	Shut down one or the other side of a socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>
#include <sys/sd.h>

int
shutdown(s, how)
int s;				/* Pseudo-descriptor for socket		*/
int how;			/* What to shut down			*/
{ char *addr;			/* Address of struct socket		*/
  register struct socket *sptr;
  int otherflags;		/* Flags to set on other half		*/
  struct sockaddr_un name;

  if ((addr = _get_sock_addr(s)) == NULL)
    return(-1);
  _enter_seg(addr);
  sptr = (struct socket *)addr;
  if (! (sptr->so_state & SS_ISCONNECTED)) {
    _leave_seg(addr);
    errno = ENOTCONN;
    return(-1);
  }
  switch (how) {
    case 0:
      sptr->so_state |= SS_CANTRCVMORE;
      otherflags = SS_CANTSENDMORE;
      break;
    case 1:
      sptr->so_state |= SS_CANTSENDMORE;
      otherflags = SS_CANTRCVMORE;
      break;
    case 2:
      otherflags = SS_CANTRCVMORE+SS_CANTSENDMORE;
      sptr->so_state |= otherflags;
      break;
    default:
      errno = EINVAL;
      return(-1);
  }
  strcpy(name.sun_path, addr+sptr->so_name);
  _leave_seg(addr);
  if ((addr = _attach_seg(name.sun_path)) == NULL) {
    perror("attach_seg in shutdown");
    abort();
  }
  sptr = (struct socket *)addr;
  _enter_seg(addr);
  sptr->so_state |= otherflags;
  _leave_seg(addr);
}
