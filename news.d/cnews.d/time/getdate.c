/*
 * getdate ascii_time ... - print the time_t of ascii_time(s)
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

extern time_t getdate();

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
process(tm)
char *tm;
{
	time_t it;

	it = getdate(tm, &ftnow);
	if (it < 0)
		error("`%s' not valid date", tm);
	else
		(void) printf("%ld\n", it);
}
