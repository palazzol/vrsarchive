#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>

/*
 * goodpass.c : A simple yes/no password sensibility function to be wired
 * into "passwd" & "yppasswd", etc.  (c) ADE Muffett, 1991 (aem@aber.ac.uk).
 * This module is freely redistributable for use in software so long as this
 * copyright notice remains intact.  Distributed as part of the 'Crack' suite
 * of password testing programs.
 */

/* Usage:- */
/* char *result = GoodPass(char *password); */
/* where password is a text string to be tested for suitability */
/* GoodPass returns NULL if the password is OK */
/* GoodPass returns a diagnostic string if the password is NOT ok */

#define PWLENGTH	8	/* significant length of text */
#define MINDIFF		5	/* minimum number of different characters */
#define MAXSTEP		3	/* max number of steps up/down in char set */
#define MINLENGTH	6	/* minimum length of a password */
#define STRINGSIZE	255	/* a standard buffer size */
#define STRIDE		4	/* word skipping length */
#undef DEBUG

static char *dikshunarys[] =
{
    "/usr/dict/words",
    "/usr/local/lib/bad_pws.dat",
    NULL
};

static int
Pmatch (control, string)
    char *control;
    char *string;
{
    while (*control)
    {
	if (!*string)
	{
	    return (0);
	}
	switch (*control)
	{
	case 'u':
	    if (!isupper (*string))
	    {
		return (0);
	    }
	    break;
	case 'l':
	    if (!islower (*string))
	    {
		return (0);
	    }
	    break;
	case 'd':
	    if (!isdigit (*string))
	    {
		return (0);
	    }
	    break;
	case 'c':
	    if (!isalpha (*string))
	    {
		return (0);
	    }
	    break;
	case '.':
	default:
	    if (!isalnum (*string))
	    {
		return (0);
	    }
	    break;
	}
	control++;
	string++;
    }
    return (1);
}

static void
Trim (string)			/* remove trailing whitespace from a string */
    register char *string;
{
    register char *ptr;

    for (ptr = string; *ptr; ptr++);
    while ((--ptr >= string) && isspace (*ptr));
    *(++ptr) = '\0';
}

static char *
Reverse (str)			/* return a pointer to a reversal */
    register char *str;
{
    register int i;
    register int j;
    register char *ptr;
    static char area[STRINGSIZE];

    j = i = strlen (str);
    while (*str)
    {
	area[--i] = *str++;
    }
    area[j] = '\0';
    return (area);
}
/******* THE TEST FUNCTION *******/

static int
Try (input, guess)
    register char *input;
    register char *guess;
{
    if (!strncasecmp (input, guess, PWLENGTH) ||
	!strncasecmp (input, Reverse (guess), PWLENGTH))
    {
	return (-1);
    }
    return (0);
}
/******* DICTIONARY SEARCHING *******/

static int
GetWord (fp, buff)
    FILE *fp;
    char *buff;
{
    register int c;

    for (;;)
    {
	c = getc (fp);
	if (c == EOF)
	{
	    return (-1);
	}
	if (c == '\n')
	{
	    break;
	}
	*(buff++) = (char) c;
    }
    *buff = 0;
    return (0);
}

static int
DictSearch (input)
    char *input;
{
    int i;
    int loops;
    register long top;
    register long bot;
    register long mid;
    long scratch;
    char word[STRINGSIZE];
    FILE *fp;
    char **dictionary;

    for (dictionary = dikshunarys; *dictionary; dictionary++)
    {
	if (!(fp = fopen (*dictionary, "r")))
	{
	    perror (*dictionary);
	    continue;
	}
	bot = 0L;		/* start of file */
	fseek (fp, 0L, 2);	/* to end of file */
	top = ftell (fp);

	for (loops = 0; loops < 1000; loops++)
	{
	    mid = (top + bot) / 2;	/* calculate the middle */
	    scratch = mid;

	  stride_loop:

	    scratch -= STRIDE;	/* calculate a bit beforehand */
	    if (scratch < 0)	/* error fixing */
	    {
		mid = scratch = 0L;
		fseek (fp, 0L, 0);
	    } else
	    {			/* find the start of the current word */
		fseek (fp, scratch, 0);	/* go read the scratch buffer */
		fread (word, 1, STRIDE, fp);

		for (i = STRIDE - 1; i >= 0; i--)
		{
		    if (word[i] == '\n')	/* where 'mid' is is start of
						 * word */
		    {
			break;
		    } else
		    {		/* mid is between words */
			mid--;
		    }
		}
		if (i < 0)
		{
		    goto stride_loop;
		}
	    }

	    fseek (fp, mid, 0);
	    GetWord (fp, word);

	    i = strncasecmp (input, word, PWLENGTH);

	    if (i > 0)
	    {
		bot = mid + strlen (word) + 1;	/* why retest this word ever */
	    } else if (i < 0)
	    {
		if (mid >= top)
		{
		    break;
		}
		top = mid;
	    } else
	    {
		fclose (fp);
		return (-1);	/* found it */
	    }
	}
	fclose (fp);
    }

    return (0);
}
/******* THE EXTERNAL CALL *******/

char *
GoodPass (input)
    char *input;
{
    register int i;
    register char *ptr;
    register char *ptr2;
    struct passwd *pwd;
    char junk[STRINGSIZE];
    char password[STRINGSIZE];

    /* back it up. */
    strcpy (password, input);
    Trim (password);

    /* who is it ? */
    pwd = getpwuid (getuid ());
    if (!pwd)
    {
	perror ("getpwuid");
	return ("Error - no password entry found to verify against.");
    }
    /* size */
    if (strlen (password) < MINLENGTH)
    {
	return ("it is too short - use more characters.");
    }
    /* username */
    if (Try (password, pwd -> pw_name))
    {
	return ("it is your username");
    }
    /* usernameusername */
    strcpy (junk, pwd -> pw_name);
    strcat (junk, pwd -> pw_name);
    if (Try (password, junk))
    {
	return ("it is your username, doubled");
    }
    /* Gecos information field */
    strcpy (junk, pwd -> pw_gecos);
    ptr = junk;
    if (*ptr == '-')		/* never seen this, but... */
    {
	ptr++;
    }
    if (ptr2 = strchr (ptr, ';'))	/* trim off junk */
    {
	*ptr2 = '\0';
    }
    if (ptr2 = strchr (ptr, ','))	/* trim off more junk */
    {
	*ptr2 = '\0';
    }
    for (;;)
    {
	if (ptr2 = strchr (ptr, ' '))
	{
	    *(ptr2++) = '\0';
	}
	if (Try (password, ptr))
	{
	    return ("it is part of your name. Use something less obvious.");
	}
	if (ptr2)
	{
	    ptr = ptr2;
	    while (*ptr && isspace (*ptr))
	    {
		ptr++;
	    }
	} else
	{
	    break;
	}
    }

    /* check for repeated characters */
    bzero (junk, sizeof (junk));
    for (i = 0; i < PWLENGTH && password[i]; i++)
    {
	if (!strchr (junk, password[i]))
	{
	    strncat (junk, password + i, 1);
	}
    }
    if (strlen (junk) < MINDIFF)
    {
	return ("it does not contain enough different characters.\nUse more different characters.");
    }
    /* check for over simplicity */
    i = 0;
    ptr = password;
    while (ptr[0] && ptr[1])
    {
	if ((ptr[1] == (ptr[0] + 1)) ||
	    (ptr[0] == (ptr[1] + 1)))
	{
	    i++;
	}
	ptr++;
    }
    if (i > MAXSTEP)
    {
	return ("it is too simplistic. Try something more random.");
    }
    /* lets get a little silly... */
    if (Pmatch ("cdddccc", password))
    {
	return ("it looks like a new style car registration.");
    }
    if (Pmatch ("cccdddc", password))
    {
	return ("it looks like a old style car registration.");
    }
    if (Pmatch ("cccddd", password) || Pmatch ("dddccc", password))
    {
	return ("it looks like an old-style car registration.");
    }
    if (Pmatch ("ccddddddc", password))
    {
	return ("it looks like a National Insurance number.");
    }
    strcpy (junk, input);

    /* do a dictionary search here */
    if (DictSearch (junk))
    {
	return ("it is a guessable dictionary word.");
    }
    if (DictSearch (Reverse (junk)))
    {
	return ("it is a guessable reversed dictionary word.");
    }
    /* strip off possible initial number and do a dictionary search here */
    if (isdigit (junk[0]))
    {
	if (DictSearch (junk + 1))
	{
	    return ("it is a digit + guessable dictionary word.");
	}
	if (DictSearch (Reverse (junk + 1)))
	{
	    return ("it is a digit + guessable reversed dictionary word.");
	}
    }
    i = strlen (junk) - 1;
    if (isdigit (junk[i]))
    {
	junk[i] = '\0';
	if (DictSearch (junk))
	{
	    return ("it is a guessable dictionary word + digit.");
	}
	if (DictSearch (Reverse (junk)))
	{
	    return ("it is a guessable reversed dictionary word + digit.");
	}
    } else if (junk[i] == 's')
    {
	junk[i] = '\0';
	if (DictSearch (junk))
	{
	    return ("it is a pluralised dictionary word.");
	}
    }
    return ((char *) NULL);
}
#ifdef DEBUG
main (argc, argv)
    int argc;
    char *argv[];
{
    int i;
    char *p;

    for (i = 1; i < argc; i++)
    {
	printf ("'%s'\n", argv[i]);

	if (p = GoodPass (argv[i]))
	{
	    printf ("\t%s\n", p);
	} else
	{
	    printf ("\tok\n");
	}
    }
}
#endif
