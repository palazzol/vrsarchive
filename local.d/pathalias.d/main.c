/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
#define MAIN
static char	*sccsid = "@(#)main.c	7.1 (down!honey) 85/08/06";
#endif lint

#include "def.h"

main(argc, argv) 
int	argc; 
char	*argv[];
{
	char	*locname = 0;

#ifdef lint
	argc = argc;
#endif lint

	ProgName = argv[0];
	while (*++argv && **argv == '-') {
		(*argv)++;
		switch(**argv) {

		case 'l':	/* local name */
			locname = *++argv;
			if (!*locname)  {
				fprintf(stderr, "%s: -l requires host name\n",
					ProgName);
				exit(1);
			}
			break;

		case 'd':	/* dead host or link */
			if (!*++argv) {
				fprintf(stderr, "%s: -d requires host name\n",
					ProgName);
				exit(1);
			}
			deadlink(*argv);
			break;

		case 'g':	/* graph output file */
			Graphout = *++argv;
			if (!*Graphout)  {
				fprintf(stderr, "%s: -g requires output file name\n",
					ProgName);
				exit(1);
			}
			break;

		case 's':	/* show cheapest links */
			Linkout = *++argv;
			if (!*Linkout)  {
				fprintf(stderr, "%s: -s requires output file name\n",
					ProgName);
				exit(1);
			}
			break;

		case 0:
			break;	/* ignore naked '-' */

		default:
			while (**argv) {
				switch (**argv) {
			
				case 'v':	/* verbose stderr, somewhat boring */
					Vflag++;
					break;

				case 'c':	/* print cost info */
					Cflag++;
					break;

				case 'i':	/* ignore case */
					Iflag++;
					break;

				default:
					fprintf(stderr, "%s: -%c: unknown flag\n", ProgName,
						**argv);
					exit(1);
				}
				(*argv)++;
			}
		}
	}

	Fcnt++;
	if (*argv) {
		Ifiles = argv;
		freopen("/dev/null", "r", stdin);
	}

	if (!locname) 
		locname = local();
	if (*locname == 0) {
		locname = "lostinspace";
		fprintf(stderr, "%s: using \"%s\" for local name\n",
				ProgName, locname);
	}

	Home = addnode(locname);	/* add home node */
	Home->n_cost = 0;		/* doesn't cost to get here */

	yyparse();			/* read in link info */

	hashanalyze();

	while (Home->n_alias) 
		Home = Home->n_alias;	/* bad practice, but conceivable */

	mapit();			/* compute shortest paths */

	exit(0);
}

badmagic(n)
{
#ifdef DEBUG
	abort();
#else !DEBUG
	exit(n);
#endif DEBUG
}
