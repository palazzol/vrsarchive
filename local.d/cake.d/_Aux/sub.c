/*
**	Sub - do substitutions on arguments
*/

static	char
rcs_id[] = "$Header: /home/Vince/cvs/local.d/cake.d/_Aux/sub.c,v 1.1 1987-11-16 19:35:05 vrs Exp $";

#include	"std.h"
#include	<ctype.h>

char	var_char = 'X';

main(argc, argv)
int	argc;
char	**argv;
{
	extern	bool	match();
	extern	char	*ground();
	reg	char	*old, *new;
	reg	bool	ignore;
	reg	int	unmatched, i;
	reg	char	*sep;

	ignore = FALSE;
	unmatched = 0;
	while (argc > 1 && argv[1][0] == '-')
	{
		for (i = 1; argv[1][i] != '\0'; i++)
		{
			switch (argv[1][i])
			{

		when 'i':
				ignore = TRUE;
		when 'v':
				if (argv[1][i+2] != '\0')
					usage();

				var_char = argv[1][i+1];
				goto nextword;
		otherwise:
				usage();
			}
		}

nextword:
		argc--;
		argv++;
	}

	if (argc < 2)
		usage();
	
	old = argv[1];
	new = argv[2];
	argv += 2;
	argc -= 2;

	sep = "";
	while (argc > 1)
	{
		if (! match(argv[1], old))
			unmatched++;
		else
		{
			printf("%s%s", sep, ground(new));
			sep = " ";
		}

		argc--;
		argv++;
	}

	printf("\n");
	exit(ignore? 0: unmatched);
}

/*
**	Tell the unfortunate user how to use sub.
*/

usage()
{
	printf("Usage: sub [-i] [-vX] oldpattern newpattern name ...\n");
	exit(1);
}

/*
**	Module to manipulate Cake patterns.
*/

typedef	struct	s_env
{
	char	*en_val;
	bool	en_bound;
} Env;

#define	NOVAR	 10
#define	MAXVAR	 11
#define	MAXSIZE	128

Env	env[MAXVAR];

/*
**	This function initialises the environment of domatch.
*/

bool
match(str, pat)
reg	char	*str;
reg	char	*pat;
{
	extern	bool	domatch();
	reg	int	i;
	reg	char	*s, *p;

	p = pat+strlen(pat)-1;
	if (*p != var_char && !isdigit(*p))	/* not part of a var */
	{
		s = str+strlen(str)-1;
		if (*s != *p)			/* last chars differ */
			return FALSE;
	}

	for (i = 0; i < MAXVAR; i++)
		env[i].en_bound = FALSE;

	return domatch(str, pat);
}

/*
**	Match a string against a pattern.
**	The pattern is expected to have been dereferenced.
**	To handle nondeterminism, a brute force recursion approach
**	is taken.
*/

bool
domatch(str, patstr)
reg	char	*str;
reg	char	*patstr;
{
	extern	char	*new_name();
	char		buf[MAXSIZE];
	reg	char	*follow;
	reg	char	*s, *t;
	reg	int	varno;
	reg	int	i;
	reg	bool	more;

	if (patstr[0] == var_char)
	{
		if (isdigit(patstr[1]))
		{
			varno  = patstr[1] - '0';
			follow = patstr + 2;
		}
		else
		{
			varno  = NOVAR;
			follow = patstr + 1;
		}

		if (env[varno].en_bound)
		{
			/* lifetime of buf is local */
			strcpy(buf, env[varno].en_val);
			strcat(buf, follow);
			return domatch(str, buf);
		}

		i = 0;
		buf[0] = '\0';
		env[varno].en_bound = TRUE;
		env[varno].en_val = buf;

		/* keep invariant: buf = tentative value of var  */
		/* must consider *s == \0, but do not overshoot  */
		for (s = str, more = TRUE; more; s++)
		{
			if (domatch(s, follow))
			{
				/* lifetime of buf is now global */
				env[varno].en_val = new_name(buf);
				return TRUE;
			}

			/* maintain invariant */
			buf[i++] = *s;
			buf[i]   = '\0';

			more = (*s != '\0');
		}
		
		/* no luck, match failed */
		env[varno].en_bound = FALSE;
		return FALSE;
	}

	/* here we have something other than a variable first off */
	for (s = str, t = patstr; *t != '\0' && *t != var_char; s++, t++)
	{
		if (*t == '\\')
			t++;	/* the new *t is not checked for % */
		
		if (*s != *t)
			return FALSE;
	}

	/* see if we have come to the end of the pattern */
	if (*t == '\0')
		return *s == '\0';
	
	/* if not, recurse on the next variable */
	return domatch(s, t);
}

/*
**	Ground the argument string, i.e. replace all occurrences
**	of variables in it. It is fatal error for the string to
**	contain a variable which has no value.
*/

char *
ground(str)
reg	char	*str;
{
	extern	char	*new_name();
	reg	char	*s, *t;
	reg	int	i, var;
	char		buf[MAXSIZE];

	i = 0;
	for (s = str; *s != '\0'; s++)
	{
		if (*s == var_char)
		{
			if (isdigit(s[1]))
				var = *++s - '0';
			else
				var = NOVAR;
			
			if (! env[var].en_bound)
			{
				printf("Attempt to use undefined value in %s\n", str);
				exit(1);
			}

			for (t = env[var].en_val; *t != '\0'; t++)
				buf[i++] = *t;
		}
		or (*s == '\\')
		{
			if (s[1] != '\0')
				buf[i++] = *++s;
		}
		else
			buf[i++] = *s;
	}

	if (i >= MAXSIZE)
	{
		printf("Ran out of buffer\n");
		exit(1);
	}

	buf[i] = '\0';
	return new_name(buf);
}

char *
new_name(str)
reg	char	*str;
{
	extern	char	*malloc();
	reg	char	*copy;

	copy = malloc(strlen(str) + 1);
	strcpy(copy, str);
	return copy;
}
