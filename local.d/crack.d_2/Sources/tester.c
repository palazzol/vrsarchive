#include <stdio.h>
#include <time.h>

#define EEK 8192
#define PROFx

static char *strings[] =
{
    "fredfred",
    "brian",
    "ab",
    "qwerty",
    "marginallytoolong",
    (char *) 0
};

main ()
{
    int i;
    char **ptr;
    char a[255];
    char b[255];

    init_des ();

#ifndef PROF
    for (ptr = strings; *ptr; ptr++)
    {
	strcpy (a, crypt (*ptr, *ptr));
	strcpy (b, fcrypt (*ptr, *ptr));

	printf ("\nCheck Old: %s\tNew: %s\n", a, b);

	if (!strcmp (a, b))
	{
	    printf ("fcrypt() is compatible with standard crypt()\n");
	} else
	{
	    printf ("fcrypt() is not compatible !!! \7\n");
	    exit (1);
	}
    }
    printf ("Doing %d fcrypts()\n", EEK);
#endif

    for (i = EEK; i; i--)
    {
	fcrypt ("fredfred", "eek");
    }

    return (0);
}
