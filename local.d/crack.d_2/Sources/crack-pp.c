#include "crack.h"

/*
 * This program is copyright (c) Alec Muffett 1991 except for certain
 * portions of code ("crack-fcrypt.c") copyright (c) Robert Baldwin, Icarus
 * Sparry and Alec Muffett.  The author(s) disclaims all responsibility or
 * liability with respect to it's usage or its effect upon hardware or
 * computer systems.  This software is in freely redistributable PROVIDED
 * that this notice remains intact.
 */

/*
 * The characters 1 and ! have the highest probability of hits of all the
 * ascii set tried. Using zero and space have a much lower probability based
 * on observation of several thousands of passwords. bct@hull.cs
 */

/*
 * Sir! I disagree! Most of the trailing character passwords I have ever seen
 * have been of a numeric type.  However I will assent to your changes.  8-)
 * aem@aber
 */

/* THIS IS THE STRING TO MODIFY TO ADD OTHER LEADING/TRAILING CHARS */

char trailing_chars[] = "123490!?";

/* These used to be enums. Oh well... non-portability rules OK */
#define ORDER_LCF	0
#define ORDER_LCFP	1
#define ORDER_MCF	2
#define ORDER_LCFT	3
#define ORDER_LCB	4
#define ORDER_LCFL	5
#define ORDER_UCF	6
#define ORDER_MCB	8
#define ORDER_UCB	7
#define NUMFILES	9

int
main (argc, argv)
    int argc;
    char *argv[];
{
    register int i;
    register char *ptr;
    int length;
    int all_print;
    int all_digits;
    int all_letters;
    int all_letters_upper;
    int all_letters_lower;
    char buffer[STRINGSIZE];

    FILE *fp;
    FILE *fps[NUMFILES];

    if (argc == 1)
    {
	printf ("Usage:\t%s dictsrc [dictsrc ...]\n", argv[0]);
	exit (1);
    }
    for (i = 0; i < NUMFILES; i++)
    {
	char scratch[255];

	fps[i] = (FILE *) 0;

#ifndef CRACK_PLURAL		/* if not defined, skip opening it */
	if (i == ORDER_LCFP)
	{
	    continue;
	}
#endif				/* CRACK_PLURAL */

#ifndef CRACK_LEADING		/* if not defined, skip opening it */
	if (i == ORDER_LCFL)
	{
	    continue;
	}
#endif				/* CRACK_LEADING */

#ifndef CRACK_TRAILING		/* if not defined, skip opening it */
	if (i == ORDER_LCFT)
	{
	    continue;
	}
#endif				/* CRACK_TRAILING */

	sprintf (scratch, "dict.%c", i + 'a');
	if (!(fps[i] = fopen (scratch, "w")))
	{
	    perror (scratch);
	    exit (3);
	}
    }

    for (i = 1; argv[i]; i++)
    {
	if (!(fp = fopen (argv[i], "r")))
	{
	    perror (argv[i]);
	    continue;
	}
	printf ("dictpp: sourcing from file '%s'\n", argv[i]);

	while (fgets (buffer, STRINGSIZE, fp))
	{
	    Trim (buffer);

	    /* skip single letters and comments */
	    if (!*buffer || !buffer[1] || buffer[0] == '#')
	    {
		continue;
	    }
	    length = strlen (buffer);

#ifndef CRACK_SHORT_PASSWDS
	    if (length < 5)
	    {
		continue;
	    }
#endif
	    all_letters_upper = 1;
	    all_letters_lower = 1;
	    all_digits = 1;
	    all_letters = 1;
	    all_print = 1;

	    for (ptr = buffer; *ptr; ptr++)
	    {
		if (!isdigit (*ptr))
		{
		    all_digits = 0;
		}
		if (!isprint (*ptr))
		{
		    all_print = 0;
		}
		if (!isalpha (*ptr))
		{
		    all_letters = 0;
		}
		if (isupper (*ptr))
		{
		    all_letters_lower = 0;
		} else if (islower (*ptr))
		{
		    all_letters_upper = 0;
		}
	    }

	    if (!all_print)
	    {
		fprintf (fps[ORDER_LCF], "%.8s\n", buffer);
		continue;	/* inputs with funnies in it */
	    }
	    if (all_letters)
	    {
		fprintf (fps[ORDER_MCF], "%.8s\n", Initcap (buffer));
		fprintf (fps[ORDER_MCB], "%.8s\n", Reverse (Initcap (buffer)));
	    }

#ifdef CRACK_PLURAL		/* slightly improved pluralisation algorithm */
	    if (all_letters && length < 8)
	    {
		/* linguistic flames to /dev/null, please */
		char pluralbuff[STRINGSIZE];

		strcpy (pluralbuff, Lowercase (buffer));

		if (!Suffix (buffer, "ch") ||
		    !Suffix (buffer, "ex") ||
		    !Suffix (buffer, "ix") ||
		    !Suffix (buffer, "sh") ||
		    !Suffix (buffer, "ss"))
		{
		    /* bench -> benches */
		    strcat (pluralbuff, "es");
		} else if (length > 2 && buffer[length - 1] == 'y')
		{
		    if (strchr ("aeiou", buffer[length - 2]))
		    {
			/* alloy -> alloys */
			strcat (pluralbuff, "s");
		    } else
		    {
			/* gully -> gullies */
			strcpy (pluralbuff + length - 1, "ies");
		    }
		} else if (buffer[length - 1] == 's')
		{
		    /* bias -> biases */
		    strcat (pluralbuff, "es");
		} else
		{
		    /* catchall */
		    strcat (pluralbuff, "s");
		}

		fprintf (fps[ORDER_LCFP], "%.8s\n", pluralbuff);
	    }
#endif				/* CRACK_PLURAL */

	    if (!all_letters_upper)
	    {
		fprintf (fps[ORDER_UCF], "%.8s\n", Uppercase (buffer));
		fprintf (fps[ORDER_UCB], "%.8s\n", Uppercase (Reverse (buffer)));
	    } else
	    {
		fprintf (fps[ORDER_UCF], "%.8s\n", buffer);
		fprintf (fps[ORDER_UCB], "%.8s\n", Reverse (buffer));
	    }

	    if (!all_letters_lower)
	    {
		fprintf (fps[ORDER_LCF], "%.8s\n", Lowercase (buffer));
		fprintf (fps[ORDER_LCB], "%.8s\n", Lowercase (Reverse (buffer)));
	    } else
	    {
		fprintf (fps[ORDER_LCF], "%.8s\n", buffer);
		fprintf (fps[ORDER_LCB], "%.8s\n", Reverse (buffer));
	    }

#if defined(CRACK_LEADING) || defined(CRACK_TRAILING)
	    if (length < 8)
	    {
		for (ptr = trailing_chars; *ptr; ptr++)
		{
#ifdef CRACK_LEADING
		    fprintf (fps[ORDER_LCFL], "%c%.7s\n", *ptr, buffer);
#endif				/* CRACK_LEADING */
#ifdef CRACK_TRAILING
		    fprintf (fps[ORDER_LCFT], "%.7s%c\n", buffer, *ptr);
#endif				/* CRACK_TRAILING */
		}
	    }
#endif
	}
    }
    fclose (fp);

    for (i = 0; i < NUMFILES; i++)
    {
	if (fps[i])
	{
	    fclose (fps[i]);
	}
    }

    exit (0);
}
