/* $Header: /home/Vince/cvs/games.d/fortune.d/fortune.c,v 1.3 1990-04-04 21:18:03 vrs Exp $ */

# include	<sys/types.h>
# include	<stdio.h>
# include	<sys/file.h>
# include	<sys/stat.h>
# include	"strfile.h"

#ifndef NO_REGEX
# include	<ctype.h>
#endif

# define	TRUE	1
# define	FALSE	0
# define	bool	short

# define	MINW	6		/* minimum wait if desired */
# define	CPERS	20		/* # of chars for each sec */
# define	SLEN	160		/* # of chars in short fortune */

#ifndef FORTFILE
# define	FORTFILE	"/usr/games/lib/fortunes.dat"
#endif

bool	Wflag		= FALSE;	/* wait desired after fortune */
bool	Sflag		= FALSE;	/* short fortune desired */
bool	Lflag		= FALSE;	/* long fortune desired */
bool	Oflag		= FALSE;	/* offensive fortunes only */
bool	Aflag		= FALSE;	/* any fortune allowed */
#ifndef NO_REGEX
bool	Mflag		= FALSE;	/* dump fortunes matching a pattern */
#endif

char	*Fortfile	= FORTFILE,	/* fortune database */
	*Usage[]	= {
# ifdef	NO_REGEX
       "usage:  fortune [ - ] [ -wsloa ] [ file ]",
# else
       "usage:  fortune [ - ] [ -wsloai ] [ -m pattern ] [ file ]",
# endif
       "	- - give this summary of usage",
       "	w - have program wait after printing message in order",
       "	    to give time to read",
       "	s - short fortunes only",
       "	l - long fortunes only",
       "	o - offensive fortunes only",
       "	a - any fortune, regular or offensive",
# ifndef NO_REGEX
       "	m - print fortunes which match a pattern",
       "	i - ignore case in matching patterns",
# endif
       "		Mail suggested fortunes to \"fortune@ucbvax.berkeley.edu\"",
	NULL
	};

off_t	Seekpts[2];			/* seek pointers to fortunes */

FILE	*Inf;				/* input file */

STRFILE	Tbl;				/* input table */

char	*malloc();

#ifndef NO_REGEX
char	*conv_pat();
#endif

#ifndef NO_REGEX
#ifdef REGCMP
# define	RE_COMP(p)	(Re_pat = regcmp(p, NULL))
# define	BAD_COMP(f)	((f) == NULL)
# define	RE_EXEC(p)	regex(Re_pat, (p))

char	*Re_pat;

char	*regcmp(), *regex();
#else
# define	RE_COMP(p)	(p = re_comp(p))
# define	BAD_COMP(f)	((f) != NULL)
# define	RE_EXEC(p)	re_exec(p)

char	*re_comp(), *re_exec();
#endif
#endif

time_t	time();

main(ac, av)
int	ac;
char	*av[];
{
	register char	c;
	register int	nchar = 0;

	getargs(ac, av);
	if ((Inf = fopen(Fortfile, "r+")) == NULL) {
		perror(Fortfile);
		exit(-1);
	}
	if (fread((char *) &Tbl, sizeof Tbl, 1, Inf) != 1) {	/* NOSTRICT */
		fprintf(stderr, "fortune file is truncated\n");
		exit(-1);
	}
	if (Tbl.str_longlen <= SLEN && Lflag) {
		fprintf(stderr, "Sorry, no long strings in this file\n");
		exit(0);
	}
	if (Tbl.str_shortlen > SLEN && Sflag) {
		fprintf(stderr, "Sorry, no short strings in this file\n");
		exit(0);
	}

	/*
	 * initialize the pointer to the first -o fortune if need be.
	 */
	if (Tbl.str_delims[2] == 0)
		Tbl.str_delims[2] = Tbl.str_delims[0];

#ifndef NO_REGEX
	if (Mflag) {
		find_matches();
		/* NOTREACHED */
	}
#endif

	do {
		getfort();
	} while ((Sflag && !is_short()) || (Lflag && !is_long()));

	(void) fseek(Inf, Seekpts[0], 0);
	while (c = getc(Inf)) {
		nchar++;
		putchar(c);
	}
	(void) fflush(stdout);
	(void) fseek(Inf, 0L, 0);
#ifdef	LOCK_EX
	/*
	 * if we can, we exclusive lock, but since it isn't very
	 * important, we just punt if we don't have easy locking
	 * available.
	 */
	(void) flock(fileno(Inf), LOCK_EX);
#endif
	if (fwrite((char *) &Tbl, 1, sizeof Tbl, Inf) != sizeof Tbl)
		fprintf(stderr, "can't update fortune data file\n");
#ifdef	LOCK_EX
	(void) flock(fileno(Inf), LOCK_UN);
#endif
	if (Wflag)
		sleep((unsigned int) max((int) nchar / CPERS, MINW));
	exit(0);
}

/*
 * is_short:
 *	Return TRUE if fortune is "short".
 */
is_short()
{
	register int	nchar;

	if (!(Tbl.str_flags & (STR_RANDOM | STR_ORDERED)))
		return (Seekpts[1] - Seekpts[0] <= SLEN);
	(void) fseek(Inf, Seekpts[0], 0);
	nchar = 0;
	while (getc(Inf))
		nchar++;
	return (nchar <= SLEN);
}

/*
 * is_long:
 *	Return TRUE if fortune is "long".
 */
is_long()
{
	register int	nchar;

	if (!(Tbl.str_flags & (STR_RANDOM | STR_ORDERED)))
		return (Seekpts[1] - Seekpts[0] > SLEN);
	(void) fseek(Inf, Seekpts[0], 0);
	nchar = 0;
	while (getc(Inf))
		nchar++;
	return (nchar > SLEN);
}

/*
 *	This routine evaluates the arguments on the command line
 */
getargs(ac, av)
register int	ac;
register char	*av[];
{
	register int	i;
	register char	*sp;
	register int	j;
	register short	bad;
	register int	ignore_case;
# ifndef NO_REGEX
	register char	*pat;
# endif

	ignore_case = FALSE;
	bad = 0;
	pat = NULL;
	for (i = 1; i < ac; i++)  {
		if (av[i][0] != '-') {
			(void) setuid(getuid());
			(void) setgid(getgid());
			Fortfile = av[i];
		}
		else if (av[i][1] == '\0') {
			j = 0;
			while (Usage[j] != NULL)
				puts(Usage[j++]);
			exit(0);
			/* NOTREACHED */
		}
		else
			for (sp = &av[i][1]; *sp != '\0'; sp++)
				switch (*sp) {
				  case 'w':	/* give time to read */
					Wflag++;
					break;
				  case 's':	/* short ones only */
					Sflag++;
					Lflag = 0;
					break;
				  case 'l':	/* long ones only */
					Lflag++;
					Sflag = 0;
					break;
				  case 'o':	/* offensive ones only */
					Oflag++;
					break;
				  case 'a':	/* any fortune */
					Aflag++;
					/*
					 * initialize the random number
					 * generator; throw away the first
					 * few numbers to avoid any non-
					 * randomness in startup
					 */
					srnd(time((time_t *) NULL) + getpid());
					for (j = 0; j < 20; j++)
						(void) rnd(100);
					break;
				  case 'm':	/* dump out the fortunes */
# ifdef	NO_REGEX
				  case 'i':
					fprintf(stderr,
						"can't match fortunes on this system (Sorry)\n");
					bad++;
# else
					Mflag++;
					if (sp[1]) {
						pat = ++sp;
						while (*sp)
							sp++;
					}
					else if (i + 1 < ac)
						pat = av[++i];
					else {
						fprintf(stderr,
							"must give pattern\n");
						bad++;
						break;
					}
					break;
				  case 'i':
					ignore_case++;
					break;
# endif
				  default:
					fprintf(stderr, "unknown flag: '%c'\n",
						*sp);
					bad++;
					break;
				}
	}

# ifndef NO_REGEX
	if (pat != NULL) {
		if (ignore_case)
			pat = conv_pat(pat);
		if (BAD_COMP(RE_COMP(pat))) {
#ifndef REGCMP
			fprintf(stderr, "%s\n", pat);
#else
			fprintf(stderr, "bad pattern: %s\n", pat);
#endif
			bad++;
		}
	}
# endif

	if (bad) {
		printf("use \"%s -\" to get usage\n", av[0]);
		exit(-1);
	}
}

/*
 * getfort:
 *	Get the fortune data file's seek pointer for the next fortune.
 */
getfort()
{
	register int	fortune;

	/*
	 * Make sure all values are in range.
	 */

	if (Tbl.str_delims[1] >= Tbl.str_delims[0])
		Tbl.str_delims[1] %= Tbl.str_delims[0];
	if (Tbl.str_delims[2] >= Tbl.str_numstr) {
		Tbl.str_delims[2] -= Tbl.str_delims[0];
		Tbl.str_delims[2] %= Tbl.str_numstr - Tbl.str_delims[0];
		Tbl.str_delims[2] += Tbl.str_delims[0];
	}

	if (Aflag) {
		if (rnd((int) Tbl.str_numstr) < Tbl.str_delims[0])
			fortune = Tbl.str_delims[1]++;
		else
			fortune = Tbl.str_delims[2]++;
	}
	else if (Oflag)
		fortune = Tbl.str_delims[2]++;
	else
		fortune = Tbl.str_delims[1]++;

	(void) fseek(Inf, (off_t) (sizeof Seekpts[0]) * fortune + sizeof Tbl,
		     0);
	if ((int)fread((char *) Seekpts, (sizeof Seekpts[0]), 2, Inf) < 2) {
		fprintf(stderr, "fortune data file corrupted\n");
		exit(-2);
	}
}

max(i, j)
register int	i, j;
{
	return (i >= j ? i : j);
}

#ifndef	NO_REGEX
/*
 * conv_pat:
 *	Convert the pattern to an ignore-case equivalent.
 */
char *
conv_pat(orig)
register char	*orig;
{
	register char	*sp;
	register int	cnt;
	register char	*new;

	cnt = 1;	/* allow for '\0' */
	for (sp = orig; *sp != '\0'; sp++)
		if (isalpha(*sp))
			cnt += 4;
		else
			cnt++;
	if ((new = malloc(cnt)) == NULL) {
		fprintf(stderr, "pattern too long for ignoring case\n");
		exit(1);
	}

	for (sp = new; *orig != '\0'; orig++) {
		if (islower(*orig)) {
			*sp++ = '[';
			*sp++ = *orig;
			*sp++ = toupper(*orig);
			*sp++ = ']';
		}
		else if (isupper(*orig)) {
			*sp++ = '[';
			*sp++ = *orig;
			*sp++ = tolower(*orig);
			*sp++ = ']';
		}
		else
			*sp++ = *orig;
	}
	*sp = '\0';
	return new;
}

/*
 * find_matches:
 *	Find all the fortunes which match the pattern we've been given.
 */
find_matches()
{
	register char		*sp;
	register char		*fortune;
	register int		found_one;
	register int		i;
	register int		start, end;

	if (Oflag || Aflag)
		end = Tbl.str_numstr;
	else
		end = Tbl.str_delims[0];
	if (Oflag) {
		start = Tbl.str_delims[0];
		(void) fseek(Inf, Tbl.str_dpos[0], 0);
	}
	else {
		start = 0;
		(void) fseek(Inf,
			     (off_t) (sizeof Tbl +
				      sizeof Seekpts[0] * (Tbl.str_numstr + 1)),
			     0);
	}

	if ((fortune = malloc(Tbl.str_longlen + 1)) == NULL) {
		perror("malloc");
		exit(1);
	}
	found_one = FALSE;
	for (i = start; i < end; i++) {
		sp = fortune;
		while ((*sp++ = getc(Inf)) != '\0')
			continue;
		if (RE_EXEC(fortune)) {
			if (found_one)
				printf("%%%%\n");
			(void) fwrite(fortune, 1, sp - fortune, stdout);
			found_one = TRUE;
		}
	}
	if (found_one)
		exit(0);
	else
		exit(1);
	/* NOTREACHED */
}
# endif
