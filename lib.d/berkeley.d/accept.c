/*
 *	%W% %G%
 *	Accept a connection to a socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>
#include <sys/sd.h>

static long l = -1;		/* For (char *)-1 casts, kernel style	*/

int
accept(s, name, namlen)
int s;				/* Pseudo-descriptor for socket		*/
struct sockaddr_un *name;	/* Name for socket to connect with	*/
int *namlen;			/* Length of name actually used		*/
{ register struct socket *sptr;
  int version;
  struct sockaddr_un myaddr, hisaddr;
  int ns;
  char *addr;			/* Address of other struct socket	*/
  short msgoff;			/* Used during queue flushes		*/

  if ((addr = _get_sock_addr(s)) == NULL)
    return(-1);
  sptr = (struct socket *)addr;
  _enter_seg(addr);
  if (sptr->so_type != SOCK_STREAM) {
    errno = EOPNOTSUPP;
    _leave_seg(addr);
    return(-1);
  }
  /*
   *	Sleep if there are no pending connections (unless set up for
   *	non-blocking I/O).
  */
  while (sptr->so_qlen == 0) {
    version = sdgetv(addr) + 1;
    _leave_seg(addr);
    if (sptr->so_state & SS_NBIO) {
      errno = EWOULDBLOCK;
      return(-1);
    }
    /*
     *	Wait for someone to touch the socket.  EINTR can be ignored here
     *	unless we really want to pass it back out to the user.
    */
    sdwaitv(addr, version);	/* Wait for someone to touch socket	*/
    _enter_seg(addr);
  }
  /*
   *	Get the first connection off my queue.  That is all we need socket
   *	s for, so release it.
  */
  bcopy((char *)&sptr->so_q[sptr->so_first++],
        (char *)&hisaddr, sizeof(hisaddr));
  sptr->so_qlen--;
  _leave_seg(addr);
  /*
   *	Get a new socket and connect him up from this side.
  */
  if ((ns = socket(AF_UNIX, SOCK_STREAM, PF_UNIX)) < 0) {
    perror("socket in accept");
    return(-1);
  }
  if ((addr = _get_sock_addr(ns)) == NULL)
    return(-1);
  sptr = (struct socket *)addr;
  strcpy(myaddr.sun_path, addr+sptr->so_name);
  sptr->so_state |= SS_ISCONNECTED;
  _mbuf_free(sptr, sptr->so_name);
  sptr->so_name = _mbuf_alloc(sptr, strlen(hisaddr.sun_path)+1);
  strcpy(addr+sptr->so_name, hisaddr.sun_path);
  _leave_seg(addr);
  /*
   *	Get the other socket and mark it connected.  
  */
  if ((addr = _attach_seg(hisaddr.sun_path)) == NULL) {
    errno = EADDRNOTAVAIL;	/* No such socket address		*/
    return(-1);
  }
  _enter_seg(addr);
  sptr = (struct socket *)addr;
  sptr->so_state |= SS_ISCONNECTED;
  sptr->so_state &= ~SS_ISCONNECTING;
  _mbuf_free(sptr, sptr->so_name);
  sptr->so_name = _mbuf_alloc(sptr, strlen(myaddr.sun_path)+1);
  strcpy(addr+sptr->so_name, myaddr.sun_path);
  /*
   *	Free all messages on so_oob and so_rcv queues.  This guarantees all
   *	messages recieved will have to come from the connected partner.
  */
  for (msgoff = sptr->so_oob; msgoff; ) {
    _mbuf_free(sptr, msgoff);
    msgoff = ((struct sock_msg *)(addr+msgoff))->sm_next;
  }
  for (msgoff = sptr->so_rcv; msgoff; ) {
    _mbuf_free(sptr, msgoff);
    msgoff = ((struct sock_msg *)(addr+msgoff))->sm_next;
  }
  _leave_seg(addr);
  return(ns);			/* Return the new socket (whew!)	*/
}
