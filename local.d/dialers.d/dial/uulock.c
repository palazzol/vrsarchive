/* uulock.c -- Make and remove uucp lock files.
 *
 * [25-Oct-84 dsh]
 *
 * USAGE:   uurmlock name
 *	    uumklock name
 *
 * Make or remove a lock as indicated.
 */


#include <stdio.h>
#ifndef M_XENIX
#  include <sys/file.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#define LCKPRE "/usr/spool/uucp/LCK."


/* isalock() -- Stolen direct from /usr/src/usr.bin/tip/uucplock.c which
 *	was stolen direct from /usr/src/usr.bin/uucp/{something}.
 *
 * Return Value:  0 if not a lock, 1 if it is.
 */
int isalock(s)
char *s;
{
	struct stat xstat;

	if (stat(s, &xstat) < 0)
		return(0);
	if (xstat.st_size != sizeof(int))
		return(0);
	return(1);
}


void usage(prgnam)
char *prgnam;
{
	fprintf(stderr, "%s: lock-name    (lock-name is like \"tty03\")\n",
		prgnam);
	exit(1);
	/*NOTREACHED*/
}



main(argc, argv)
int argc;
char **argv;
{
	char *lockname, bf[120];
	int fd, pid;

	lockname = &bf[0];
	if (argc != 2)
		usage(argv[0]);
	sprintf(lockname, "%s.%s", LCKPRE, argv[1]);

	if (strcmp(argv[0], "uurmlock") == 0) {
		if (!isalock(lockname)) {
			fprintf(stderr, "%s: lock %s invalid\n", 
				argv[0], lockname);
			exit(1);
			/*NOTREACHED*/
		}
		if (unlink(lockname) != 0)
			perror(argv[0]);
	}
	else {
		fd = creat(lockname, 0444);
		if (fd < 0)
			perror(argv[0]);
		pid = getpid();
		write(fd, (char *)&pid, sizeof(int));
		close(fd);
	}
}
