/*  getbool -- ask user a yes/no question
 *
 *  Usage:  i = getbool (prompt, defalt);
 *
 *  Example:  do {...} while (getbool ("More?",1));
 *
 *  Prints prompt string, asks user for response.  Defalt is
 *  0 (no) or 1 (yes), and is used if user types just carriage return,
 *  or on end-of-file or error in the standard input.
 *
 *  HISTORY
 * 23-Oct-82  Steven Shafer (sas) at Carnegie-Mellon University
 *	Added code to return default if gets returns NULL.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.  Possible changes for the future:  accept "t" (true)
 *	and "f" (false), 0 and 1, etc.
 *
 */

#include <stdio.h>
#define TRUE 1
#define FALSE 0

int getbool (prompt, defalt)
char *prompt;
int defalt;
{
	register int valu;
	register char ch;
	char input [100];

	if (defalt != TRUE && defalt != FALSE)  defalt = TRUE;
	valu = 2;				/* meaningless value */
	do {
		printf ("%s  [%s]  ",prompt,(defalt ? "yes" : "no"));
		fflush (stdout);			/* in case it's buffered */
		if (gets (input) == NULL) {
			valu = defalt;
		}
		else {
			ch = *input;			/* first char */
			if (ch == 'y' || ch == 'Y')		valu = TRUE;
			else if (ch == 'n' || ch == 'N')	valu = FALSE;
			else if (ch == '\0')		valu = defalt;
			else printf ("Must begin with 'y' (yes) or 'n' (no).\n");
		}
	} 
	while (valu == 2);			/* until correct response */
	return (valu);
}
