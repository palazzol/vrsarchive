/*
 * hostname - return the Usenet name of this machine
 *
 * One interesting possibility would be to assume that the first
 * name in the sys file is our Usenet name, unless it is "ME",
 * which would require our current strategy anyway.
 */

#include <stdio.h>
#include <sys/types.h>

#include "news.h"
#include "newspaths.h"

#ifndef NAMEFILE
#define NAMEFILE libfile("whoami")
#endif

char *
hostname()			/* return this Usenet machine's name */
{
	static char name[MAXHOST];

	if (name[0] == '\0') {	/* try to get the "news hostname" */
		FILE *fp;

		fp = fopen(NAMEFILE, "r");
		if (fp != NULL) {
			(void) fgets(name, sizeof name, fp);
			(void) fclose(fp);
			if (name[0] != '\0' && name[strlen(name) - 1] == '\n')
				name[strlen(name) - 1] = '\0';
		}
	}
	if (name[0] == '\0')	/* else use the ordinary hostname */
		(void) gethostname(name, sizeof name);
	return name;
}
