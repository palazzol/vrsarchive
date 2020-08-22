/*
 *	%W% %G%
 *	Name a socket.
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>

int
bind(s, name, namlen)
int s;				/* Pseudo-descriptor for socket		*/
struct sockaddr_un *name;	/* Desired name				*/
int namlen;			/* Length of name actually used		*/
{ register struct socket *sptr;
  char *old;

  if ((sptr = (struct socket *)_get_sock_addr(s)) == NULL)
    return(-1);
  if ((name->sun_family != AF_UNSPEC) && (name->sun_family != AF_UNIX)) {
    errno = EADDRNOTAVAIL;
    return(-1);
  }
  _enter_seg((char *)sptr);
  if (sptr->so_state & SS_ISBOUND) {
    _leave_seg((char *)sptr);
    errno = EINVAL;
    return(-1);
  }
  old = (char *)sptr + sptr->so_name;
  /*
   *	Create the new name.  Defend against EINTR.
  */
  while (link(old, name->sun_path) < 0)
    if (errno != EINTR) {
      _leave_seg((char *)sptr);
      errno = EADDRNOTAVAIL;
      return(-1);
    }
  /*
   *	Delete old name, as it is no longer used.  More EINTR paranoia.
  */
  while (unlink(old) < 0)
    if (errno != EINTR) {
      perror("unlink in bind");
      abort();
    }
  _mbuf_free(sptr, sptr->so_name);
  sptr->so_name = _mbuf_alloc(sptr, strlen(name->sun_path)+1);
  strcpy((char *)sptr+sptr->so_name, name->sun_path);
				/* Remember socket name			*/
  _leave_seg((char *)sptr);
  return(s);			/* Return psuedo file descriptor	*/
}
