/*
 * same as usual tempnam.c except force placement in the directory
 * we ask for
 */
#include <stdio.h>

#ifndef P_tmpdir
#define P_tmpdir	"/tmp/"
#define L_tmpnam	(sizeof(P_tmpdir) + 15)
#endif

#define max(A,B) (((A)<(B))?(B):(A))

extern char *malloc(), *getenv(), *mktemp();
extern char *strncat(), *strcat(), *strcpy();
extern int access();

static char *pcopy(), *seed = "AAA";

char *
mtempnam(dir, pfx)
char *dir;		/* use this directory please (if non-NULL) */
char *pfx;		/* use this (if non-NULL) as filename prefix */
{
	register char *p, *q;
	int x = 0, y = 0, z;

	z = strlen(P_tmpdir);
	if (dir != NULL) {
		y = strlen(dir);
	}
	if ((p = malloc((unsigned)(max(max(x, y), z) + 16))) == NULL)
		return(NULL);
	if (y > 0 && pcopy(p, dir))
		goto OK;
	if (access(pcopy(p, P_tmpdir), 3) == 0)
		goto OK;
	if (access(pcopy(p, "/tmp"), 3) != 0)
		return(NULL);
OK:
	strcat(p, "/");
	if (pfx) {
		*(p + strlen(p) + 5) = '\0';
		strncat(p, pfx, 5);
	}
	strcat(p, seed);
	strcat(p, "XXXXXX");
	q = seed;
	while (*q == 'Z')
		*q++ = 'A';
	++ * q;
	if (*mktemp(p) == '\0')
		return(NULL);
	return(p);
}


static char *
pcopy(space, arg)
char *space, *arg;
{
	char *p;

	if (arg) {
		strcpy(space, arg);
		p = space - 1 + strlen(space);
		if (*p == '/')
			*p = '\0';
	}
	return(space);
}


