/*
 *	%W% %G%
 *	Close a socket or file.
*/
#include <errno.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sd.h>

static long l = -1;

close(fd)
int fd;
{ char *addr;
  struct socket *sptr;
  struct sockaddr_un hisaddr;
  int options;
  int version;

  addr = _socktab[fd];
  if (addr == NULL)
    return(_close(fd));
  sptr = (struct socket *)addr;
  /*
   *	Close of unconnected socket?  If not, remember if SO_LINGER
   *	and the name of the connected socket.  EINTR paranoia again.
  */
  _enter_seg(addr);
  if ((sptr->so_state & SS_ISCONNECTED) == 0) {
    _socktab[fd] = NULL;		/* Not a socket anymore		*/
    while (_close(fd) < 0)
      if (errno != EINTR) {
        perror("_close in sock_close");
        abort();
      }
    while (unlink(addr+sptr->so_name) < 0)
      if (errno != EINTR) {
        perror("unlink in sock_close");
        abort();
      }
    _leave_seg(addr);
    return(0);
  }
  strcpy(hisaddr.sun_path, addr+sptr->so_name);
  options = sptr->so_options;
  _leave_seg(addr);
  if (options & SO_LINGER) {
    /*
     *	SHOULD IMPLEMENT THE SO_LINGER TIMEOUT HERE.
    */
  }
  if ((addr = _attach_seg(hisaddr.sun_path)) == NULL) {
    perror("_attach_seg in sock_close");
    abort();
  }
  _enter_seg(addr);
  sptr = (struct socket *)addr;
  while (sptr->so_oob || sptr->so_rcv) {
    version = sdgetv(addr) + 1;
    _leave_seg(addr);
    /*
     *	Wait for someone to touch the socket.  EINTR is harmless
     *	here, though it really ought to be returned to the user.
    */
    sdwaitv(addr, version);	/* Wait for someone to touch socket	*/
    _enter_seg(addr);
  }
  strcpy(hisaddr.sun_path, addr+sptr->so_name);
  _leave_seg(addr);
  /*
   *	Shut down the socket, then close the pseudo-fd.
  */
  if (shutdown(fd, 2) < 0)		/* No more sends or recieves	*/
    return(-1);				/* errno set by shutdown()	*/
  _socktab[fd] = NULL;			/* Not a socket anymore		*/
  while (_close(fd) < 0)
    if (errno != EINTR) {
      perror("_close in sock_close");
      abort();
    }
  /*
   *	Calling sdfree() would botch memory management, so we just unlink
   *	the namespace binding.
  */
  while (unlink(hisaddr.sun_path) < 0)
    if (errno != EINTR) {
      perror("unlink in sock_close");
      abort();
    }
  return(0);
}
