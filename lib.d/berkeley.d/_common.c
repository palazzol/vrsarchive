/*
 *	%W%
 *	Common internal operations on sockets.
*/
#define GLOBAL
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/stat.h>
#include <sys/brk.h>
#include <sys/sd.h>
#include <errno.h>

static long l = -1;		/* For (char *)-1 casts, kernel style	*/

extern char *malloc();
extern char *realloc();

char *
_get_sock_addr(s)
int s;
{
	char *addr;		/* Address of struct socket	*/
	struct stat sbuf;

	addr = _socktab[s];
	if (addr == NULL) {
		if (fstat(s, &sbuf) == 0)
			errno = ENOTSOCK;
	}
	return(addr);
}

/*
 *	Kludge around the kernel's tendency to blow up with ENOMEM when
 *	lots of sdget()/sdfree() pairs are done.  Basically, rather than
 *	get and free segments, we get them once and keep them around for
 *	the life of the process.  [The current implementation of sdget()
 *	will always frow the ldt (even if there are free ldt slots in it).
 *	Eventually the ldt cannot be grown any larger and the user sees
 *	ENOMEM -- the process has hundreds of free ldt slots, but the sdget()
 *	implementation doesn't know it.  We avoid the problem by simply
 *	never calling sdfree.]
*/
char *
_attach_seg(name)
char *name;
{ struct sdcache *cptr;		/* Pointer into the cache		*/
  char *addr;			/* Address of shared data segment	*/
  int csize;			/* Current size of cache (used entries)	*/

  /*
   *	Look for name in _sd_cache.  Return it if found.
  */
  if (_sd_cache != NULL) {
    for (cptr = _sd_cache; cptr->sd_addr != NULL; cptr++)
      if (strcmp(cptr->sd_name.sun_path, name) == 0)
        return(cptr->sd_addr);
    csize = cptr - _sd_cache;
  } else {
    /*
     *	Create cache, since it doesn't exist yet.  Last entry in cache
     *	always has NULL address so cache search will terminate.  This
     *	sets things up as if cache search had failed in a zero entry
     *	cache.
    */
    cptr = _sd_cache = (struct sdcache *)malloc(sizeof(*_sd_cache));
    csize = 0;
  }
  /*
   *	Cache search failed.  The local 'csize' gives the subscript of
   *	the first unused entry.  First, get the segment.
  */
  while ((addr = sdget(name, SD_WRITE)) == (char *)l)
    if ((errno != EAGAIN) && (errno != EINTR))
      return(NULL);		/* Return failure			*/
#ifdef M_LDATA
  brkctl(BR_NEWSEG, 0L, (char *)NULL); /* GROT -- Let malloc() work	*/
#endif M_LDATA
  /*
   *	Now grow the cache and insert the new element.  The current size
   *	if csize+1, so new size is csize+2 elements.
  */
  cptr = (struct sdcache *)realloc((char *)_sd_cache,
                                   (csize+2)*sizeof(struct sdcache));
  if (cptr == NULL) {
    perror("No Room to grow _sd_cache in socket code");
    abort();
  }
  _sd_cache = cptr;
  strcpy(_sd_cache[csize].sd_name.sun_path, name);
  _sd_cache[csize++].sd_addr = addr;
  _sd_cache[csize].sd_addr = NULL;
  return(addr);
}

/*
 *	Kludge around the EAGAIN flakiness.  Also defend against EINTR,
 *	which can happen during sleeps waiting for the segment to become
 *	free.  EINTR should really make it back out to the user's code,
 *	but that is much more difficult.
*/
void
_enter_seg(addr)
char *addr;
{
	while (sdenter(addr, SD_WRITE) < 0)
		if ((errno != EAGAIN) && (errno != EINTR)) {
			perror("sdenter");
			abort();
		}
}

void
_leave_seg(addr)
char *addr;
{
		if (sdleave(addr) < 0) {
			perror("sdleave");
			abort();
		}
}
