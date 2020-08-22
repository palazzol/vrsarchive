/*
 * string operations
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"

/*
 * Copy tokens to firstone, remove all text starting at the first space.
 */
first(tokens, firstone)
register char *tokens, *firstone;
{
	char *space;

	(void) strcpy(firstone, tokens);
	space = index(firstone, ' ');
	if (space != NULL)
		*space = '\0';		/* terminate firstone at first space */
}

/*
 * Turn a newsgroup name into a file name, in place.
 */
mkfilenm(ng)
register char *ng;
{
	for (; *ng != '\0'; ng++)
		if (*ng == NGDELIM)
			*ng = FNDELIM;
}

trim(s)					/* trim trailing newline */
char *s;
{
	register char *nl;

	INDEX(s, '\n', nl);
	if (nl != NULL)
		*nl = '\0';
}

char *
skipsp(s)				/* skip any whitespace at *s */
register char *s;
{
	while (iswhite(*s))
		s++;
	return s;
}

/* like strdup, but error if can't allocate */
char *
strsave(s)			/* copy s into malloced memory, if any */
char *s;
{
	register char *news = malloc((unsigned)strlen(s) + 1);	/* include NUL */

	if (news != NULL)
		(void) strcpy(news, s);
	else
		errunlock("out of memory", "");
	return news;
}
