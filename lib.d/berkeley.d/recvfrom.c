/*
 *	%W% %G%
 *	Receive a message on a socket, informing user of the sender's
 *	identity.  May be used on a disconnected socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>
#include <sys/sd.h>

int
recvfrom(s, buf, len, flags, from, fromlen)
int s;				/* Pseudo-descriptor for socket		*/
char *buf;			/* Buffer to receive data into		*/
int len;			/* Length of buffer provided		*/
int flags;			/* Flags for the operation		*/
struct sockaddr_un *from;	/* Fill with address of sender		*/
int *fromlen;			/* Fill with size of address		*/
{ char *addr;			/* Address of struct socket		*/
  register struct socket *sptr;
  int version;
  int *qptr;
  struct sock_msg *mptr;
  int cc;

  addr = _get_sock_addr(s);
  if (addr == NULL)
    return(-1);
  sptr = (struct socket *)addr;
  _enter_seg(addr);
  if (sptr->so_state & SS_CANTRCVMORE) {
    _leave_seg(addr);
    errno = ESHUTDOWN;
    return(-1);
  }
  if (flags & MSG_OOB)
    qptr = &sptr->so_oob;
  else
    qptr = &sptr->so_rcv;
  while (*qptr == 0) {
    /*
     * No data available.  Return EWOULDBLOCK if SS_NBIO.
    */
    if (sptr->so_state & SS_NBIO) {
      _leave_seg(addr);
      errno = EWOULDBLOCK;
      return(-1);
    }
    if (sptr->so_state & SS_CANTRCVMORE) {
      _leave_seg(addr);
      errno = ESHUTDOWN;
      return(-1);
    }
    /*
     *	Sleep until some data arrives.
    */
    version = sdgetv(addr)+1;
    _leave_seg(addr);
    /*
     *	EINTR causes extra loop and is not seen by user code.
    */
    sdwaitv(addr, version);	/* Wait for someone to touch socket	*/
    _enter_seg(addr);
  }
  /*
   *	There is data on the queue.  Copy it out to the user and delete it
   *	from the socket.  Delete the packet if all the data is gone.
  */
  mptr = (struct sock_msg *) (addr+*qptr);
  *fromlen = strlen(mptr->sm_from.sun_path)+sizeof(mptr->sm_from.sun_family);
  bcopy((char *)&mptr->sm_from, (char *)from, *fromlen);
  if (len < mptr->sm_len) {
    cc = len;				/* User gets full buffer	*/
    bcopy((char *)(mptr+1), buf, len);	/* Copy data to user		*/
    mptr->sm_len -= cc;			/* Less data now		*/
    bcopy((char *)(mptr+1)+cc, (char *)(mptr+1), mptr->sm_len);
  } else {
    cc = mptr->sm_len;			/* User gets a packet		*/
    bcopy((char *)(mptr+1), buf, cc);	/* Copy out packet data		*/
    _mbuf_free(sptr, *qptr);		/* Free the packet		*/
    *qptr = mptr->sm_next;		/* Remove from message queue	*/
  }
  _leave_seg(addr);
  return(cc);
}
