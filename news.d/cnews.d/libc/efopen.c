/*
 * efopen - fopen file, exit with message if impossible
 */

#include <stdio.h>

static char message[] = "can't open file \"%s\" mode ";

FILE *
efopen(file, mode)
char *file;
char *mode;
{
	FILE *fp;
	char fullmsg[sizeof(message)+10];
	extern int errno;

	errno = 0;		/* Wipe out residue of earlier errors. */
	fp = fopen(file, mode);
	if (fp == NULL) {
		strcpy(fullmsg, message);
		strncat(fullmsg, mode, 10);
		error(fullmsg, file);
		/* NOTREACHED */
	}
	return(fp);
}
