/*
 * stdfdopen - ensure that the standard i/o descriptors are open,
 *	to avoid mayhem.
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

stdfdopen()			/* ensure standard descriptors are open */
{
	register int fd;

	for (fd = 0; fd < 3; fd++) {		/* 3 is NSYSFILE on V8 */
		register int newfd;
		extern int errno;

		/* TODO: use fstat instead? */
		newfd = dup(fd);		/* just to check whether it's open */
		if (newfd < 0 && errno == EBADF)	/* fd is closed */
			if (open("/dev/null", 2) != fd)	/* open read/write */
				exit(1);	/* bad news */
		if (newfd >= 0)			/* fd was open */
			(void) close(newfd);
	}
}
