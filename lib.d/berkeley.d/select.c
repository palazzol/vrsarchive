/*
 *	%W% %G%
 *	Synchronous I/O Multiplexing
 *	Returns masks of file descriptors (selected by the masks) for
 *	which non-blocking I/O can be done.  We pretend that all files
 *	can be written without blocking, and the system has rdchk().
 *	This emulation is even harder without it.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sd.h>
#include <sys/time.h>
#include <errno.h>

extern long time();

int
select(nfds, readfds, writefds, exceptfds, timeout)
int nfds;
long *readfds, *writefds, *exceptfds;
struct timeval *timeout;
{ int fd, iready, oready, eready, emask, rmask, wmask, nfound;
  char *addr;
  struct socket *sptr;
  long tim;
  static struct timeval tv = {
    1000000L, 0			/* 1,000,000 seconds */
  };

  if (timeout == (struct timeval *)0)
    timeout = &tv;		/* Null pointer implies forever		*/
  tim = time((long *)0);
  do {
    fd = iready = oready = 0;
    emask = exceptfds? *exceptfds: 0;
    rmask = readfds ? *readfds : 0;
    wmask = writefds ? *writefds : 0;
    for (; fd < nfds; fd++, emask >>= 1, rmask >>= 1, wmask >>= 1) {
      if (fd >= NOFILE)
        break;
      if (wmask & 1) {
        oready |= 1<<fd;
        nfound++;
      }
      addr = _socktab[fd];
      if (addr != NULL) {
        _enter_seg(addr);
        sptr = (struct socket *)addr;
        if ((rmask & 1) && sptr->so_qlen) {
          iready |= 1<<fd;	/* select for read if connection avail.	*/
          nfound++;
        }
        if ((rmask & 1) && sptr->so_rcv) {
          iready |= 1<<fd;	/* select for read if data available	*/
          nfound++;
        }
        if ((emask & 1) && sptr->so_oob) {
          eready |= 1<<fd;	/* select for exception if OOB data	*/
          nfound++;
        }
        _leave_seg(addr);
        continue;		/* End of socket portion of loop	*/
      }
      if ((rmask & 1) && (rdchk(fd) >= 0)) {
        iready |= 1<<fd;
        nfound++;
      }
    }
    if (nfound) {
      if (readfds)
        *readfds = iready;
      if (writefds)
        *writefds = oready;
      if (exceptfds)
        *exceptfds = eready;
      return(nfound);
    }
    sleep(1);
  } while ((time((long *)0)-tim) < timeout->tv_sec);
  if (readfds)
    *readfds = 0;
  if (writefds)
    *writefds = 0;
  if (exceptfds)
    *exceptfds = 0;
  return(0);
}
