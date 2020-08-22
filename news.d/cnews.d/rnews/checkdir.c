/*
 * checkdir - make the directories implied by `name'
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"

int
checkdir(name, uid, gid)
register char *name;
{
	register char *cp;

	for (cp = name; *cp != '\0'; cp++)
		if (*cp == '/') {
			*cp = '\0';
			if (access(name, 01) < 0) {	/* can't search */
				char *cbuf = emalloc((unsigned)STRLEN("mkdir ") +
					strlen(name) + 1);

				(void) sprintf(cbuf, "mkdir %s", name);
				(void) system(cbuf);
				free(cbuf);
				(void) chown(name, uid, gid);
			}
			*cp = '/';
		}
	return cp[-1] == '/';
}
