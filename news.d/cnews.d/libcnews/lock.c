/*
 * C news system locking.
 * It's compatible with B 2.10.1 news, except that locks are never
 * declared stale (blow 'em away in /etc/rc).
 * Only permit rnews to run on a file server to make this sane.
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include "news.h"

#define LOCKNAME "LOCK"
#define LOCKTEMP "LOCKTMXXXXXX"
#define INTERVAL 25		/* seconds to sleep on a busy lock */

static int debug = NO;
static int mylock = NO;

lockdebug(state)
int state;
{
	debug = state;
}

newslock()
{
	int locktries = 0;
	char tempnm[MAXFILE], lockfile[MAXFILE];
	FILE *tempfp;
	extern int errno;
	char *libfile();
	char *strcpy(), *mktemp();

	/* create temporary name for linking; store my pid in it */
	(void) strcpy(tempnm, libfile(LOCKTEMP));
	(void) mktemp(tempnm);
	tempfp = fopen(tempnm, "w");
	if (tempfp == NULL)
		error("can't create lock temporary `%s'", tempnm);
	(void) fprintf(tempfp, "%d\n", getpid());
	(void) fclose(tempfp);

	/* repeatedly try to link the temporary name to LOCKNAME */
	(void) strcpy(lockfile, libfile(LOCKNAME));
	while (link(tempnm, lockfile) < 0) {
		extern char *progname;

		if (errno != EEXIST)
			error("can't link `%s' to LOCK", tempnm);
		/*
		 * Decide here if lock is stale.
		 * If so, remove it and try again to lock.
		 */
		/* process still alive */
		if (debug && ++locktries == 1)
			(void) printf("%s: sleeping on LOCK\n", progname);
		sleep(INTERVAL);
	}
	(void) unlink(tempnm);
	mylock = YES;
}

newsunlock()
{
	char *libfile();

	if (mylock) {
		(void) unlink(libfile(LOCKNAME));
		mylock = NO;
	}
}

errunlock(fmt, s)		/* like error(3), but unlock before exit */
char *fmt, *s;
{
	warning(fmt, s);
	newsunlock();
	exit(1);
	/* NOTREACHED */
}
