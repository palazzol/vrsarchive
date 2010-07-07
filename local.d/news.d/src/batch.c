/*
 * Batch: program to batch a list of news articles into an unbatch script.
 * Usage: /usr/lib/news/batch listfile [bytecount]
 *  where listfile is a file containing a list, one per line, of full
 *  path names of files containing articles, e.g. as produced by the F
 *  transmission option in the sys file.
 *  bytecount is the maximum number of bytes to output before exiting
 * Output is placed on standard output.
 *
 * Intended usage:
 *
 *	With the shellfile "sendbatch", with machine names as arguments:
 * 		e.g
 *		sendbatch rlgvax seismo
 *
 * This would be invoked every hour or two from crontab.
 *
 */

#ifndef lint
static char	*SccsId = "@(#)batch.c	1.11	8/23/84";
#endif !lint

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "defs.h"
#ifdef lint
#define LIBDIR "something"
#endif lint

#if defined(USG) || defined(BSD4_2) || defined(BSD4_1C)
#include <fcntl.h>
#endif

struct stat sbuf;

main(argc,argv)
char **argv;
{
	register FILE *fd, *nfd;
	register int c;
	register long n;
	register char *cp;
	char *fdstatus;
	long maxbytes, nbytes;
	long atol();
	char fname[512];
	char workfile[512];
	char *index(), *fgets();

	if (argc < 2) {
		fprintf(stderr, "Usage: batch listfile [bytecount]\n");
		exit(1);
	}

	/*
	 * Rename real file to a work name to avoid race conditions.
	 * If workfile exsists skip the rename in order
	 * to recover from a crash w/o losing anything.
	 */
	strcpy(workfile, argv[1]);
	strcat(workfile, ".work");
	if (access(workfile, 0) < 0) {
		if (access(argv[1], 0) < 0 && errno == ENOENT)
			exit(0);	/* no news */
		if (rename(argv[1], workfile) < 0) {
			logerr("rename(%s,%s) %s", argv[1], workfile,
				sys_errlist[errno]);
			exit(1);
		}
	}
	fd = fopen(workfile, "r");
	if (fd == NULL) {
		logerr("fopen(%s,r) %s", workfile, sys_errlist[errno]);
		exit(1);
	}

	if (argc > 2)
		maxbytes = atol(argv[2]);
	else
		maxbytes = 100000000L; /* backwards compatible */
	nbytes = 0;
	while ((fdstatus = fgets(fname, sizeof fname, fd)) != NULL
	    && nbytes < maxbytes) {
		cp = index(fname, '\n');
		if (cp)
			*cp = '\0';
		nfd = fopen(fname, "r");
		if (nfd == NULL) {
			perror(fname);
			continue;
		}
		fstat(fileno(nfd), &sbuf);
		printf("#! rnews %ld\n", sbuf.st_size);
		n = 0;
		while ((c = getc(nfd)) != EOF) {
			putchar(c);
			n++;
		}
		fclose(nfd);
		nbytes += sbuf.st_size;
		if (n != sbuf.st_size) { /* paranoia */
			logerr("%s, expected %ld bytes, got %ld", fname,
				n, sbuf.st_size);
			/* breaking out of this early will end up resyncing
			   the batch files (isn't serendipity wonderful?) */
			break;
		}
	}
	if (fdstatus != NULL) {		/* exceeded maxbytes */
		char tmpfile[512];

		umask(2);
		strcpy(tmpfile, argv[1]);
		strcat(tmpfile, ".tmp");
	    	nfd = fopen(tmpfile, "w");
		if (nfd == NULL) {
			logerr("fopen(%s,w) %s", tmpfile, sys_errlist[errno]);
			exit(1);
		}
		do {
			fputs(fname, nfd);
		} while (fgets(fname, sizeof fname, fd) != NULL);
		fclose(nfd);
		fclose(fd);
		/* will pick it up next time thru */
		if (rename(tmpfile, workfile) < 0) {
			logerr("rename(%s,%s) %s", tmpfile, workfile,
				sys_errlist[errno]);
			exit(1);
		}
	}
	else
		unlink(workfile);
	exit(0);
}

/*
 * Log the given message, with printf strings and parameters allowed,
 * on the log file, if it can be written.
 */
/* VARARGS1 */
logerr(fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9)
char *fmt;
{
	FILE *logfile;
	char lfname[BUFLEN];		/* the log file */
	char bfr[BUFLEN];
	char *logtime, *ctime();
	time_t t;

	time(&t);
	logtime = ctime(&t);
	logtime[16] = 0;
	logtime += 4;

#ifdef IHCC
	sprintf(lfname, "%s/%s/errlog", logdir(HOME), LIBDIR);
#else
	sprintf(lfname, "%s/errlog", LIBDIR);
#endif

	sprintf(bfr, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	fprintf(stderr, bfr);
	if (access(lfname, 0) == 0 && (logfile = fopen(lfname, "a")) != NULL) {
#if defined(USG) || defined(BSD4_2) || defined(BSD4_1C)
		int flags;
		flags = fcntl(fileno(logfile), F_GETFL, 0);
		fcntl(fileno(logfile), F_SETFL, flags|O_APPEND);
#else v7
		lseek(fileno(logfile), 0L, 2);
#endif v7
		fprintf(logfile, "%s\batch\t%s\n", logtime, bfr);
		fclose(logfile);
	}
}

#if !defined(BSD4_2) && !defined(BSD4_1C) && !defined(__CYGWIN__)
rename(from, to)
register char *from, *to;
{
	unlink(to);
	if (link(from, to) < 0)
		return -1;

	unlink(from);
	return 0;
}
#endif !BSD4_2 && !BSD4_1C
