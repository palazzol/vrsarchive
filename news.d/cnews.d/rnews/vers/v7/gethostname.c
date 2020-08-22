/*
 * v7 gethostname simulation
 *	taken from pathalias and cleaned up.  Thanks, peter.
 */

#include <stdio.h>

#define MAXLINE 256
#define min(a,b) ((a) < (b)? (a): (b))

static char defhost[] = "INSERT-YOUR-HOST-NAME-HERE";

int
gethostname(hostname, size)
char *hostname;
int size;
{
	char *ptr;
	FILE *whoami;
	char *index(), *strncpy();
	FILE *fopen(), *popen();

	*hostname = '\0';
	/* try /etc/whoami */
	if ((whoami = fopen("/etc/whoami", "r")) != NULL) {
		(void) fgets(hostname, size, whoami);
		(void) fclose(whoami);
		if ((ptr = index(hostname, '\n')) != NULL)
			*ptr = '\0';
	}
	if (*hostname != '\0')
		return 0;
	/* try /usr/include/whoami.h */
	if ((whoami = fopen("/usr/include/whoami.h", "r")) != NULL) {
		while (!feof(whoami)) {
			char sysname[MAXLINE];

			if (fgets(sysname, MAXLINE, whoami) == NULL)
				break;
			if (sscanf(sysname, "#define sysname \"%[^\"]\"",
			    hostname) > 0)
				break;
		}
		(void) fclose(whoami);
		if (*hostname != '\0')
			return 0;
	}
	/* ask uucp */
	if ((whoami = popen("PATH=/bin:/usr/bin:/usr/ucb uuname -l", "r")) != NULL) {
		(void) fgets(hostname, size, whoami);
		(void) pclose(whoami);
		if ((ptr = index(hostname, '\n')) != NULL)
			*ptr = '\0';
	}
	if (*hostname != '\0')
		return 0;
	/* aw hell, i give up!  is this a real unix? */
	(void) strncpy(hostname, defhost, min(sizeof defhost, size));
	return 0;
}
