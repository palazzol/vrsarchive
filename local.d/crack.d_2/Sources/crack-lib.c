#include "crack.h"

/*
 * This program is copyright (c) Alec Muffett 1991 except for certain
 * portions of code ("crack-fcrypt.c") copyright (c) Robert Baldwin, Icarus
 * Sparry and Alec Muffett.  The author(s) disclaims all responsibility or
 * liability with respect to it's usage or its effect upon hardware or
 * computer systems.  This software is in freely redistributable PROVIDED
 * that this notice remains intact.
 */

void
Trim (string)			/* remove trailing whitespace from a string */
    register char *string;
{
    register char *ptr;

    for (ptr = string; *ptr; ptr++);
    while ((--ptr >= string) && isspace (*ptr));
    *(++ptr) = '\0';
}

char *
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

char *
Uppercase (str)			/* return a pointer to an uppercase */
    register char *str;
{
    register char *ptr;
    static char area[STRINGSIZE];

    ptr = area;
    while (*str)
    {
	*(ptr++) = islower (*str) ? toupper (*str) : *str;
	str++;
    }
    *ptr = '\0';

    return (area);
}

char *
Lowercase (str)			/* return a pointer to an lowercase */
    register char *str;
{
    register char *ptr;
    static char area[STRINGSIZE];

    ptr = area;
    while (*str)
    {
	*(ptr++) = isupper (*str) ? tolower (*str) : *str;
	str++;
    }
    *ptr = '\0';

    return (area);
}

char *
Initcap (str)			/* return a pointer to an capitalised */
    register char *str;
{
    register char *ptr;
    static char area[STRINGSIZE];

    ptr = area;

    while (*str)
    {
	*(ptr++) = isupper (*str) ? tolower (*str) : *str;
	str++;
    }

    *ptr = '\0';

    if (islower (area[0]))
    {
	area[0] = toupper (area[0]);
    }
    return (area);
}

char *
Clone (string)
    char *string;
{
    int len;
    char *retval;

    retval = (char *) malloc (strlen (string) + 1);
    strcpy (retval, string);
    return (retval);
}

int
Suffix (word, suffix)
    char *word;
    char *suffix;
{
    register int i;
    register int j;

    i = strlen (word);
    j = strlen (suffix);

    if (i > j)
    {
	return (strcmp ((word + i - j), suffix));
    } else
    {
	return (-1);
    }
}
