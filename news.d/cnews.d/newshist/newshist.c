/*
 * newshist msgids - print history lines corresponding to msgids
 */

#include <stdio.h>
#include "history.h"

char *progname;
int debug;
static char *history;		/* unused */
int remote;			/* to satisfy rnews code */

/*
 * main - parse arguments and handle options
 */
main(argc, argv)
int argc;
char *argv[];
{
	int c;
	int errflg = 0;
	extern int optind;
	extern char *optarg;

	progname = argv[0];
	while ((c = getopt(argc, argv, "df:")) != EOF)
		switch (c) {
		case 'd':
			++debug;
			break;
		case 'f':
			history = optarg;
			break;
		default:
			errflg++;
			break;
		}
	if (optind == argc || errflg) {
		fprintf(stderr, "usage: %s [-df file]\n", progname);
		exit(2);
	}

	for (; optind < argc; optind++)
		process(argv[optind]);
	exit(0);
}

/*
 * process - message-id argument
 */
process(msgid)
char *msgid;
{
	char *histent;

	if (msgid == NULL)
		return;		
	histent = gethistory(msgid);
	if (histent == NULL) {
		char newmsgid[1000];
		extern char *strcpy(), *strcat();

		(void) strcpy(newmsgid, "<");
		(void) strcat(newmsgid, msgid);
		(void) strcat(newmsgid, ">");
		histent = gethistory(newmsgid);
	}
	if (histent == NULL)
		fprintf(stderr, "%s: no history entry for %s nor <%s>\n",
			progname, msgid, msgid);
	else
		fputs(histent, stdout);
	(void) fflush(stdout);
}

unprivileged()
{
}
