/*
 * putenv - add a variable to the environment
 */

#include <stdio.h>
#include <sys/types.h>

#define YES 1
#define NO 0

int
putenv(var)			/* put var in the environment */
char *var;
{
	register char **envp, **newenv;
	register int oldenvcnt;
	extern char **environ;
	extern char *malloc();

	/* count variables, look for var */
	for (envp = environ; *envp != 0; envp++) {
		register char *varp = var, *ep = *envp;
		register int namesame;

		namesame = NO;
		for (; *varp == *ep && *varp != '\0'; ++ep, ++varp)
			if (*varp == '=')
				namesame = YES;
		if (*varp == *ep && *ep == '\0')
			return;			/* old & new var's are the same */
		if (namesame) {
			*envp = var;		/* replace var with new value */
			return;
		}
	}
	oldenvcnt = envp - environ;

	/* allocate new environment with room for one more variable */
	newenv = (char **)malloc((oldenvcnt + 1 + 1)*sizeof(*envp));
	if (newenv == NULL)
		return NO;

	/* copy old environment pointers, add var, switch environments */
	(void) memcpy(newenv, environ, oldenvcnt*sizeof(*envp));
	newenv[oldenvcnt] = var;
	newenv[oldenvcnt+1] = NULL;
	environ = newenv;
	return YES;
}
