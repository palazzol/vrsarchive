/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
static char	*sccsid = "@(#)makedb.c	7.1 (down!honey) 85/08/06";
#endif lint

#include <stdio.h>
#include "config.h"

typedef struct {
	char *dptr;
	int dsize;
} datum;

char	*Ofile = ALIASDB, *ProgName, *Fname;
int	Nets, Paths, Edges;

char	*strany();

#define USAGE "makedb [-o dbname] [file ...]"

main(argc, argv)
	char	*argv[];
{	int	verbose = 0, append = 0;
	char	*ofptr;
	FILE	*dbstream;

	ProgName = argv[0];
	--argc;
	for ( ; *++argv && **argv == '-'; --argc) {
		(*argv)++;
		switch(**argv) {

		case 'o':	/* dbm output file */
			Ofile = *++argv;
			--argc;
			if (*Ofile == 0) {
				usage();
				exit(1);
			}
			if ((ofptr = rindex(Ofile, '/')) != 0)
				ofptr++;
			else
				ofptr = Ofile;
			if (strlen(ofptr) > 10) {
				ofptr[10] = 0;
				fprintf(stderr, "%s: using %s for db output\n",
					ProgName, Ofile);
			}
			break;

		case 'v':	/* chatty */
			verbose++;
			break;

		case 'a':	/* append mode */
			append++;
			break;

		default:
			fprintf(stderr, "%s: -%s: unknown flag\n", ProgName, *argv);
			usage();
			exit(1);
			break;

		}
	}

	if (append == 0 && dbmfile(Ofile) < 0) {
		perror_(Ofile);
		exit(1);
	}

	if (dbminit(Ofile) < 0) {
		perror_(Ofile);
		exit(1);
	}

	if (argc == 0) {
		makedb(stdin);
	} else {
		while (argc > 0) {
			if ((dbstream = fopen(*argv, "r")) == NULL) {
				perror_(*argv);
			} else {
				Fname = *argv;
				makedb(dbstream);
				fclose(dbstream);
			}
			--argc;
			argv++;
		}
	}
	if (verbose)
		fprintf(stderr, "%d paths, %d edges, %d nets\n", Paths, Edges, Nets);
}

dbmfile(dbmf)
	char	*dbmf;
{	int	fd;
	char	buf[BUFSIZ];

	sprintf(buf, "%s.dir", dbmf);
	fd = creat(buf, 0666);
	if (fd < 0)
		return(-1);
	close(fd);

	sprintf(buf, "%s.pag", dbmf);
	fd = creat(buf, 0666);
	if (fd < 0)
		return(-1);
	close(fd);

	return(0);
}

makedb(stream)
	FILE	*stream;
{	char	*op, line[BUFSIZ], *end;
	datum	key, val;

	/*
	 * keys and values are 0 terminated.  this wastes time and
	 * (disk) space, and is generally stupid.  it does buy
	 * simplicity and backwards compatibility.
	 */
	key.dptr = line;
	while (fgets(line, sizeof(line), stream) != NULL) {
		end = line + strlen(line);
		end[-1] = 0;			/* kill newline */
		op = index(line, '\t');
		if (op != 0) {
			*op++ = 0;
			key.dsize = op - line;		/* 0 terminated */
			val.dptr = op;
			val.dsize = end - op;		/* 0 terminated */
		} else {
			key.dsize = end - line;		/* 0 terminated */
			val.dptr = "\0";		/* why must i do this? */
			val.dsize = 1;
		}
		if (store(key, val) < 0)
			perror_(Ofile);
	}
}

perror_(str)
	char	*str;
{
	fprintf(stderr, "%s: ", ProgName);
	perror(str);
}

usage()
{
	fputs(USAGE, stderr);
}
