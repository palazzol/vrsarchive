/*
 * emalloc - malloc with error() called when out of space
 */

#define	NULL	0

char *
emalloc(amount)
unsigned amount;
{
	register char *it;
	char camount[25];		/* Enough to sprintf an unsigned. */
	extern char *malloc();

	it = malloc(amount);
	if (it == NULL) {
		sprintf(camount, "%u", amount);
		error("malloc(%s) failed", camount);
	}	

	return(it);
}
