#ifndef lint
static char RCSid[] = "$Header: /home/Vince/cvs/local.d/se.d/scriptse.c,v 1.1 1987-02-07 19:03:48 vrs Exp $";
#endif

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1  86/05/06  13:38:32  osadr
 * Initial revision
 * 
 * 
 */

/*
** scriptse.c
**
** turn input into a form se
** can use as a script.
**
** This is very quick-and-dirty, not checking
** for any of se's control characters.
*/

#include <stdio.h>

main (argc, argv)
int argc;
char **argv;
{
	register int c;
	register int dflag = 0;

	if (argc > 1)
		if (strcmp (argv[1], "-d") == 0)
			dflag = 1;
		else
		{
			fprintf (stderr, "usage: %s [-d] < file > newfile\n",
				argv[0]);
			exit (1);
		}

	while ((c = getchar()) != EOF)
	{
		if (c != '\n')		/* most frequent case */
			putchar (c);
		else
		{
			putchar ('\r');
			if (! dflag)
				putchar ('\177');
		}
	}
	
	exit (0);
}
