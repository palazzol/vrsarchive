/*
 * 4.2bsd getwd simulation
 */

#include <stdio.h>

#define MAXWD 1024		/* limited by 4.2 getwd(2) */

char *
getwd(path)
char *path;
{
	char *nlp;
	FILE *fp;
	FILE *popen();
	char *rindex();

	fp = popen("PATH=/bin:/usr/bin pwd", "r");
	if (fp == NULL)
		return 0;
	if (fgets(path, MAXWD, fp) == NULL) {
		(void) pclose(fp);
		return 0;
	}
	if ((nlp = rindex(path, '\n')) != NULL)
		*nlp = '\0';
	(void) pclose(fp);
	return path;
}
