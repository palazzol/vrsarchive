/*
 * TREK73: parsit.c
 *
 * Parse and get input
 *
 * Gets, parsit (courtesy, P. Lapsley)
 *
 */

#ifdef SYS5
#define index strchr
#endif

#include <stdio.h>
extern void free();
extern char *gets(), *malloc(), *strcpy(), *index();

static int gindx;
static char **argv;

char *
Gets(buf, len)
char *buf;
int len;
{
	register char *tmp;

	if (argv[gindx] == NULL) {
		(void) fgets(buf, len, stdin);
		if (tmp = index(buf, '\n'))
			*tmp = '\0';
		return(buf);
	}
	++gindx;
	if (argv[gindx] == NULL) {
		(void) fgets(buf, len, stdin);
		if (tmp = index(buf, '\n'))
			*tmp = '\0';
		return (buf);
	}
	(void) strcpy (buf, argv[gindx]);
	puts (buf);
	return (buf);
}

/*
** parsit.c  23 September 1984  P. Lapsley (phil@Berkeley.ARPA)
**
** Parse a string of words separated by spaces into an
** array of pointers to characters, just like good ol' argv[]
** and argc.
**
** Usage:
**
** char line[132];
** char **argv;
** int argc;
**
**	argv = (char **) NULL;
**	argc = parsit(line, &argv);
**
** returns the number of words parsed in argc.  argv[argc] will
** be (char *) NULL to indicate end of list, if you're not
** happy with just knowing how many words you have.
**
** Note that setting argv = (char **) NULL is only done the first
** time the routine is called with a new "argv" -- it tells
** parsit that "argv" is a new array, and parsit shouldn't free
** up the elements (as it would do if it were an old array).
*/


parsit(line, array)
char *line;
char ***array;
{
	char *malloc();
	char word[132];
	char *linecp;
	int i, j, num_words;

	gindx = 0;
	argv = *array;
	if (argv != (char **) NULL) {  /* Check to see if we should */
		/* free up the old array */
		for (i=0; argv[i] != (char *) NULL; i++) {
			free(argv[i]);	/* If so, free each member */
		}
		free((char *)argv);		/* and then free the ptr itself */
	}

	linecp = line;
	num_words = 0;
	while (1) {	/* count words in input */
		for (; *linecp == ' ' || *linecp == '\t'; ++linecp)
			;
		if (*linecp == '\0')
			break;

		for (; *linecp != ' ' && *linecp != '\t' && *linecp != '\0'; ++linecp)
			;
		++num_words;
		if (*linecp == '\0')
			break;
	}

	/* Then malloc enough for that many words plus 1 (for null) */

	if ((argv = (char **) malloc((unsigned)((num_words + 1) * sizeof(char *)))) ==
		(char **) NULL) {
		fprintf(stderr, "parsit: malloc out of space!\n");
		return(0);
	}

	j = i = 0;
	while (1) {	/* Now build the list of words */
		for (; *line == ' ' || *line == '\t'; ++line)
			;
		if (*line == '\0')
			break;

		i = 0;
		for (; *line != ' ' && *line != '\t' && *line != '\0'; ++line)
			word[i++] =  *line;
		word[i] = '\0';
		argv[j] = malloc(strlen(word) + 1);
		if (argv[j] == (char *) NULL) {
			fprintf(stderr, "parsit: malloc out of space!\n");
			return(0);
		}

		(void) strcpy(argv[j], word);
		++j;
		if (*line == '\0')
			break;
	}
	argv[j] = (char *) NULL;  /* remember null at end of list */
	*array = argv;
	return(j);
}


