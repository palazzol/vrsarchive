# include	<sys/types.h>
# include	<stdio.h>
# include	<ctype.h>
# include	"strfile.h"

# define	TRUE	1
# define	FALSE	0

/*
 *	This program un-does what "strfile" makes, thereby obtaining the
 * original file again.  This can be invoked with the name of the output
 * file, the input file, or both. If invoked with only a single argument
 * ending in ".dat", it is pressumed to be the input file and the output
 * file will be the same stripped of the ".dat".  If the single argument
 * doesn't end in ".dat", then it is presumed to be the output file, and
 * the input file is that name prepended by a ".dat".  If both are given
 * they are treated literally as the input and output files.
 *
 *	Ken Arnold		Aug 13, 1978
 */

# define	DELIM_CH	'-'

char	Infile[100],			/* name of input file */
	Outfile[100],			/* name of output file */
	Delimch = '%';			/* delimiter character */

short	Oflag = FALSE;			/* use order of initial table */

FILE	*Inf, *Outf;

char	*strrchr(), *malloc(), *strcat(), *strcpy();

main(ac, av)
int	ac;
char	**av;
{
	register int	c;
	register int	nstr, delim;
	static STRFILE	tbl;		/* description table */

	getargs(ac, av);
	if ((Inf = fopen(Infile, "r")) == NULL) {
		perror(Infile);
		exit(-1);
		/* NOTREACHED */
	}
	if ((Outf = fopen(Outfile, "w")) == NULL) {
		perror(Outfile);
		exit(-1);
		/* NOTREACHED */
	}
	(void) fread((char *) &tbl, sizeof tbl, 1, Inf);
	if (Oflag) {
		order_unstr(&tbl);
		exit(0);
		/* NOTREACHED */
	}
	nstr = tbl.str_numstr;
	(void) fseek(Inf, (off_t) (sizeof (off_t) * (nstr + 1)), 1);
	delim = 0;
	nstr = 0;
	while ((c = getc(Inf)) != EOF)
		if (c != '\0')
			putc(c, Outf);
		else if (nstr != tbl.str_numstr - 1)
			if (++nstr == tbl.str_delims[delim]) {
				fprintf(Outf, "%c-\n", Delimch);
				delim++;
			}
			else
				fprintf(Outf, "%c%c\n", Delimch, Delimch);
	exit(0);
	/* NOTREACHED */
}

getargs(ac, av)
register int	ac;
register char	*av[];
{
	register int	i;
	register char	*sp;
	register int	j;
	register short	bad;

	bad = 0;
	for (i = 1; i < ac; i++)  {
		if (av[i][0] != '-') {
			(void) strcpy(Infile, av[i]);
			if (i + 1 >= ac) {
				(void) strcpy(Outfile, Infile);
				if ((sp = strrchr(av[i], '.')) &&
				    strcmp(sp, ".dat") == 0)
					Outfile[strlen(Outfile) - 4] = '\0';
				else
					(void) strcat(Infile, ".dat");
			}
			else
				(void) strcpy(Outfile, av[i + 1]);
			break;
		}
		else if (av[i][1] == '\0') {
			printf("usage: unstr [-o] [-cC] datafile[.dat] [outfile]\n");
			exit(0);
			/* NOTREACHED */
		}
		else
			for (sp = &av[i][1]; *sp != '\0'; sp++)
				switch (*sp) {
				  case 'o':	/* print out in seekptr order */
					Oflag++;
					break;
				  case 'c': /* new delimiting char */
					if ((Delimch = *++sp) == '\0') {
						--sp;
						Delimch = *av[++i];
					}
					if (!isascii(Delimch)) {
						fprintf(stderr,
							"bad delimiting character: 0x%x\n",
							Delimch);
						bad++;
					}
					break;
				  default:
					fprintf(stderr, "unknown flag: '%c'\n",
						*sp);
					bad++;
					break;
				}
	}
	if (bad) {
		printf("use \"%s -\" to get usage\n", av[0]);
		exit(-1);
	}
}

order_unstr(tbl)
STRFILE	*tbl;
{
	register int	i, c;
	register int	delim;
	register off_t	*seekpts;

	seekpts = (off_t *) malloc(sizeof *seekpts * tbl->str_numstr);	/* NOSTRICT */
	if (seekpts == NULL) {
		perror("malloc");
		exit(-1);
		/* NOTREACHED */
	}
	(void) fread((char *) seekpts, sizeof *seekpts, (int) tbl->str_numstr,
		     Inf);
	delim = 0;
	for (i = 0; i < tbl->str_numstr; i++, seekpts++) {
		if (i != 0)
			if (i == tbl->str_delims[delim]) {
				fputs("%-\n", Outf);
				delim++;
			}
			else
				fputs("%%\n", Outf);
		(void) fseek(Inf, *seekpts, 0);
		while ((c = getc(Inf)) != '\0')
			putc(c, Outf);
	}
}
