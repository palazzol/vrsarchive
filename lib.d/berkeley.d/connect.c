/*
 *	%W% %G%
 *	Queue a connection to a socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>
#include <sys/sd.h>

static long l = -1;		/* For (char *)-1 casts, kernel style	*/

int
connect(s, name, namlen)
int s;				/* Pseudo-descriptor for socket		*/
struct sockaddr_un *name;	/* Name for socket to connect with	*/
int namlen;			/* Length of name actually used		*/
{ register struct socket *sptr;
  struct sockaddr_un myaddr;
  int version;
  char *addr;			/* Address of other struct socket	*/

  if ((addr = _get_sock_addr(s)) == NULL)
    return(-1);
  sptr = (struct socket *)addr;
  if ((name->sun_family != AF_UNSPEC) && (name->sun_family != AF_UNIX)) {
    errno = EADDRNOTAVAIL;
    return(-1);
  }
  /*
   *	Get the name of socket s into myaddr.sun_path.
  */
  _enter_seg(addr);
  if (sptr->so_state & (SS_ISCONNECTING|SS_ISCONNECTED)) {
    _leave_seg(addr);
    errno = EISCONN;
    return(-1);
  }
  strcpy(myaddr.sun_path, addr+sptr->so_name);
  _leave_seg(addr);
  /*
   *	Get the other socket and fuss it's connection queue.
  */
  if ((addr = _attach_seg(name->sun_path)) == NULL) {
    errno = ENOENT;		/* No such socket address		*/
    return(-1);
  }
  _enter_seg(addr);
  sptr = (struct socket *)addr;
  if (sptr->so_qlen == SOMAXCONN) {
    _leave_seg(addr);
    errno = ECONNREFUSED;
    return(-1);
  }
  strcpy(sptr->so_q[(sptr->so_first+sptr->so_qlen) % SOMAXCONN].sun_path,
         myaddr.sun_path);	/* Fill in queue element		*/
  sptr->so_qlen++;		/* One more in the queue		*/
  _leave_seg(addr);
  /*
   *	Now wait for our socket to get connected.  Since this requires
   *	co-operation from someone else (by calling accept), I don't
   *	understand the semantics of EWOULDBLOCK.  I return EWOULDBLOCK
   *	immediately if SS_NBIO is set, and hope that is what is intended.
  */
  if ((addr = _get_sock_addr(s)) == NULL)
    return(-1);
  sptr = (struct socket *)addr;
  while (! (sptr->so_state & SS_ISCONNECTED)) {
    version = sdgetv(addr) + 1;
    _leave_seg(addr);
    if (sptr->so_state & SS_NBIO) {
      errno = EWOULDBLOCK;	/* This is silly here			*/
      return(-1);
    }
    /*
     *	EINTR is being ignored below, rather than struggle to get it back
     *	to the user.
    */
    sdwaitv(addr, version);	/* Wait for someone to touch socket	*/
    _enter_seg(addr);
  }
  _leave_seg(addr);
  errno = 0;			/* Emulate syscall			*/
  return(0);			/* Return success			*/
}
