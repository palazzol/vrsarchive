/*
 * dial.c -- handle switching of dial in and dial out lines.
 *
 *
 * Dial checks 4 cases:
 * 1) Creating a dial out line, lock file doesn't exist.
 *    This is fine, disable the getty process.
 * 2) Creating a dial out line, lock file does exist.
 *    If there is a process up for this line, the lock file is there to
 *    stop tip/uucp/kermit from trying to use the line.
 *    We can remove the lock file and disable getty.
 *    On the other hand, if there is NOT a process up, somebody else is using 
 *    the line, so complain.
 * 3) Creating a dial in line, lock file doesn't exist.
 *    This is fine.  Start up a getty process and create a lock file
 *    (so that tip/uucp/kermit won't try to use the line).
 * 4) Creating a dial in line, lock file does exist.
 *    Nothing to do, just complain.
 *
 * In all cases, if the lock file exists and is very old (over 8 hours) then
 * dial goes ahead and removes it.
 *
 * This program originally came from Gertjan Vinkesteyn (gertjan@txsil.UUCP),
 * but has been heavily modified by David Herron (david@ukma.UUCP).  I have
 * took out all the localized code, and made it do the 4 cases above.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

char	lck[40];
char	devname[20];

#define ROOTID	(0)
#define UUCPID	(66)
#define DAEMGRP	(1)

#define TTYSFILE	("/etc/ttys")
#define NEWTTYS		("/etc/newttys")

#define SLCKTIME (28800) /* Time to allow lock files to live in seconds */

main(argc, argv)
int	argc;
char	**argv;
{
	register char	*p;
	char	buf[10];
	FILE	*Ti, *To;
	struct stat	statbuf;
	char *tty;
	time_t ptime;
	char *direction;

	if (argc != 3)
		usage();

	tty = argv[2];
	sprintf(lck, "/usr/spool/uucp/LCK..%s", tty);
	sprintf(devname, "/dev/%s", tty);

	direction = argv[1];
	
	if ((strcmp(direction, "in") == 0) || (strcmp(direction,"out") == 0)) {
		if (access(lck) == 0) {
			/*
			 * Check to see if it matters whether or not
			 * we can use it.
			 */
			if (stat(lck, &statbuf) < 0) {
				fprintf(stderr,"stat failed on %s\n", lck);
				Abort();
			}
			time(&ptime);
			if ((ptime - statbuf.st_ctime) >= SLCKTIME)
				goto out;
			if (strcmp(direction, "in") == 0) {
				/*
				 * Making an incoming line.  Must be an
				 * outgoing line currently, right?  If there
				 * is a lock file, then someone is using the
				 * port, and we can't do anything with it.
				 */
				fprintf(stderr, "%s exists, program abort\n",
					 lck);
				Abort();
			}
			/* else, making it outgoing */

			if ((Ti = fopen(TTYSFILE, "r")) == NULL) {
				fprintf(stderr, "Cannot open `%s'\n", TTYSFILE);
				Abort();
			}
			/*
			 * If we are making an outgoing line, and
			 * there is already a process up for it, then
			 * that lock file is a fake meant to cause
			 * tip/uucp/kermit to not try to open the
			 * device.
			 * Otherwise someone is using the line,
			 * so we must respect the lock file.
			 */
			while ((p = fgets(buf, sizeof buf, Ti))) {
				if (strncmp(&buf[2], tty, 5) == 0) {
					/* check if a process up */
					if (*p == '1')
						break;
				}
			}
			fclose(Ti);
			/*
			 * If p == NULL, we saw EOF, indicating that
			 * there is not a process up on that line.
			 */
			if (p == NULL) {
				fprintf(stderr, "%s exists, program abort\n",
					 lck);
				Abort();
			}
		}
out:
		/* Get the status for the device file. */
		if (stat(devname, &statbuf) < 0) {
			fprintf(stderr,"stat failed\n");
			Abort();
		}
		/* check ownership */
		if (statbuf.st_uid != ROOTID && statbuf.st_uid != UUCPID) {
			fprintf(stderr,
	"root or uucp does not own `%s', somebody might be logged on\n", tty);
			Abort();
		}
		if ((Ti = fopen(TTYSFILE, "r")) == NULL) {
			fprintf(stderr, "Cannot open `%s'\n", TTYSFILE);
			usage();
		}
		if ((To = fopen(NEWTTYS, "w")) == NULL) {
			fprintf(stderr, "Cannot creat `%s'\n", NEWTTYS);
			usage();
		}
		/* Read through and modify TTYSFILE */
		while ((p = fgets(buf, sizeof buf, Ti))) {
			if (strncmp(&buf[2], tty, 5) == 0) {
				if (strcmp(direction,"in") == 0) {
					*p = '1';
					mklock(lck);
				} else {
					*p = '0';
					if (chmod(devname, 0666) != 0)
						fprintf(stderr,
						"Cannot chmod `%s'\n", devname);
					else 
						chown(devname, UUCPID, DAEMGRP);
					if (access(lck) == 0)
						rmlock(lck);
				}
			}
			fputs(buf, To);
		}
		fclose(To);
		fclose(Ti);
		if (unlink(TTYSFILE) == -1) {
			fprintf(stderr, "Cannot unlink `%s'\n", TTYSFILE);
			Abort();
		}
		if (link(NEWTTYS, TTYSFILE) == -1) {
			fprintf(stderr, "Cannot link `%s' `%s'\n",
				NEWTTYS, TTYSFILE);
			fprintf(stderr,
			   "FATAL. Now we don't have a `%s' file anymore!!\n",
			    TTYSFILE);
			Abort();
		}
		if (unlink(NEWTTYS) == -1) {
			fprintf(stderr, "Cannot unlink `%s'\n", NEWTTYS);
			Abort();
		}
	} else
		usage();
	if (kill (1, 1) != 0)
		fprintf(stderr, "Cannot kill -1 1\n");
}

usage()
{
	fprintf(stderr, "Usage: `dial in ttyxx' or `dial out ttyxx'\n");
	exit();
}

Abort()
{
	fprintf(stderr, "dial abort\n");
	exit(-1);
}


/*
 * isalock() -- Stolen direct from /usr/src/usr.bin/tip/uucplock.c which
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

/*
 * rmlock() -- remove a lock file, checking that it actually IS a lock file.
 */
rmlock(s)
char *s;
{
	if (!isalock(s)) {
		fprintf(stderr, "dial: lock %s invalid\n", s);
		return(1);
	}
	if (unlink(s) != 0)
		perror("dial");
}

/*
 * mklock() -- make a proper lockfile.  They contain a process id.
 */
mklock(s)
char *s;
{
	int fd, pid;

	fd = creat(s, 0444);
	if (fd < 0)
		perror("dial");
	pid = getpid();
	write(fd, (char *)&pid, sizeof(int));
	close(fd);
}
