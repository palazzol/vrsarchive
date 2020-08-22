/*
 * readline - like fgets, but newslock at first EOF
 */

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include "news.h"

void complain();

/* How to get an unsigned char. */
#ifdef CHARBITS
#define	UCHARAT(p)	(*(p)&CHARBITS)
#else
#define	UCHARAT(p)	((int)*(unsigned char *)(p))
#endif

int nlocked = 0;		/* newslock() done? read by the caller */

/* Buffer etc. for readline and friends. */
static char buf[BUFSIZ];
static int nleft = 0;
static char *rest;

/*
 - readline - read a line (sans newline), with locking when we hit EOF
 *
 * Minor flaw:  will lose a last line which lacks a newline.
 */
int				/* 0 success, -1 final EOF */
readline(s, n, fd)
char *s;
int n;
int fd;				/* Note descriptor, not FILE *. */
{
	register char *dst;
	register int ndst;
	register int c;
	extern void refill();
	extern int errno;

	dst = s;
	for (ndst = n-1; ndst > 0; ndst--) {	/* -1 for NUL */
		if (nleft <= 0) {
			refill(fd);
			if (nleft <= 0)	/* refill gave up. */
				return -1;
		}
		c = UCHARAT(rest);
		rest++;
		nleft--;

		if (c == '\n') {
			*dst++ = '\0';
			return 0;
		} else
			*dst++ = c;
	}

	*dst++ = '\0';
	errno = 0;
	complain("over-long history line `%s'", s);
	return 0;
}

/*
 - refill - refill readline's buffer, with locking on EOF
 */
void
refill(fd)
int fd;
{
	register int ret;

	/* Just in case... */
	if (nleft > 0)
		return;

	/* Try ordinary read. */
	ret = read(fd, buf, (int)sizeof(buf));
	if (ret < 0) {
		complain("read error in history", "");
		return;
	}
	if (ret > 0) {
		nleft = ret;
		rest = buf;
		return;
	}

	/* EOF. */
	if (nlocked)
		return;		/* We're really done. */

	/* EOF but we haven't locked yet.  Lock and try again. */
	(void) signal(SIGINT, (sigarg_t)SIG_IGN);
	(void) signal(SIGQUIT, (sigarg_t)SIG_IGN);
	(void) signal(SIGHUP, (sigarg_t)SIG_IGN);
	(void) signal(SIGTERM, (sigarg_t)SIG_IGN);
	newslock();
	nlocked = 1;
	refill(fd);
}
