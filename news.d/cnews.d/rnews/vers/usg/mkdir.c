/*
 * 4.2BSD mkdir simulation
 */

#include <stdio.h>

int
mkdir(dir, mode)
char *dir;
int mode;
{
	int ret;
	char cbuf[BUFSIZ];
	char *sprintf();

	(void) sprintf(cbuf, "mkdir %s", dir);
	ret = system(cbuf);
	if (mode != 0777) {	/* needs special attention */
		int oldmask = umask(0);

		(void) umask(oldmask);
		(void) chmod(dir, mode & ~oldmask);
	}
	return ret;
}
