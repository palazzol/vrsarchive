/*
 * ctime time_t ... - print the ascii time of time_t(s)
 */

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define	DAY	(24L*60L*60L)

struct timeb ftnow;

char *progname;

extern int errno;
extern char *strcpy();
extern char *strncpy();
extern char *strcat();
extern char *strchr();
extern char *strtok();
extern long atol();
extern char *malloc();
extern struct tm *gmtime();
extern time_t time();

/* Forwards. */
extern void process();

/*
 - main - parse arguments and handle options
 */
main(argc, argv)
int argc;
char *argv[];
{
	register int c;
	register int errflg = 0;
	extern int optind;
	extern char *optarg;

	progname = argv[0];
	ftime(&ftnow);

	while ((c = getopt(argc, argv, "")) != EOF)
		switch (c) {
		case '?':
		default:
			errflg++;
			break;
		}
	if (errflg || optind == argc) {
		(void) fprintf(stderr, "Usage: %s ascii_time ...\n", progname);
		exit(2);
	}

	for (; optind < argc; optind++)
		process(argv[optind]);
	exit(0);
}

/*
 * process - print time_t of tm
 */
void
process(tms)
char *tms;
{
	time_t tm;
	char *ctime();
	long atol();

	tm = atol(tms);
	(void) fputs(ctime(&tm), stdout);
}
