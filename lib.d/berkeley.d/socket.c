/*
 *	%W% %G%
 *	Create a socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>
#include <sys/brk.h>
#include <sys/sd.h>

extern char *tmpnam();

static long l = -1;		/* for (char *)-1 casts, kernel style	*/

int
socket(af, type, pf)
int af;				/* AF_UNIX, or AF_UNSPEC		*/
int type;			/* SOCK_STREAM				*/
int pf;				/* PF_UNIX, or PF_UNSPEC		*/
{ register struct socket *sptr;
  int s;
  char *addr;
  char *nam;

  if (af == AF_UNSPEC)
    af = AF_UNIX;
  if (pf == PF_UNSPEC)
    pf = PF_UNIX;
  if (af != AF_UNIX) {
    errno = EAFNOSUPPORT;
    return(-1);
  }
  if (type != SOCK_STREAM) {
    errno = ESOCKTNOSUPPORT;
    return(-1);
  }
  if (pf != PF_UNIX) {
    errno = EPROTONOSUPPORT;
    return(-1);
  }
  s = open("/dev/tty", 1);	/* Make read/write botches obvious	*/
  if (s < 0)
    return(-1);			/* errno already set by open		*/
  if (_socktab[s] != NULL) {
    perror("User closed socket via syscall");
    abort();
  }
  /*
   *	BUG -- can't bind() accross filesystems.  tmpnam() currently gives
   *	names of the form /usr/tmp/XXXXXXX, so beware.
  */
  nam = tmpnam((char *)NULL);
  while ((addr = sdget(nam, SD_CREAT|SD_WRITE, SDSIZE, SDMODE)) == (char *)l)
    if ((errno != EAGAIN) && (errno != EINTR)) {
      perror("sdget in socket");
      abort();
    }
#ifdef M_LDATA
  brkctl(BR_NEWSEG, 0L, (char *)0); /* GROT -- let malloc work		*/
#endif M_LDATA
  sptr = (struct socket *)addr;
  _enter_seg(addr);
  _mbuf_init(sptr);		/* Init socket memory allocator		*/
  sptr->so_name = _mbuf_alloc(sptr, strlen(nam)+1);
  strcpy(addr+sptr->so_name, nam); /* Remember socket name		*/
  sptr->so_type = type;		/* Remember socket type			*/
  sptr->so_options = 0;		/* No funny options yet			*/
  sptr->so_linger = 0;		/* Don't linger on close		*/
  sptr->so_state = 0;		/* Not connected, etc			*/
  sptr->so_error = 0;		/* No error yet (need this?)		*/
  sptr->so_pgrp = getpgrp();	/* Signal Process group (need this?)	*/
  sptr->so_first = 0;		/* Start connection queue here		*/
  sptr->so_qlen = 0;		/* No connections queued yet		*/
  sptr->so_qlimit = 0;		/* Limit of zero connection requests	*/
  sptr->so_timeo = 0;		/* No Connection timeout		*/
  sptr->so_oob = 0;		/* No OOB data yet			*/
  sptr->so_rcv = 0;		/* No receive data yet			*/
  _socktab[s] = addr;		/* This fd is a socket			*/
  _leave_seg(addr);
  errno = 0;			/* No error (a la syscall)		*/
  return(s);			/* Return psuedo file descriptor	*/
}
