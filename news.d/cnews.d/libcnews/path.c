/*
 * news path names (and umask)
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"
#include "newspaths.h"

#ifndef NULL
#define	NULL	0
#endif

#ifndef NEWSCTL
#define NEWSCTL "/usr/lib/news"
#endif
#ifndef NEWSARTS
#define NEWSARTS "/usr/spool/news"
#endif
#ifndef NEWSBIN
#define	NEWSBIN	"/usr/lib/newsbin"
#endif
#ifndef NEWSUMASK
#define	NEWSUMASK	002
#endif

static char *pwd = NULL;	/* Current directory, NULL means unknown. */
static int dirsset = NO;	/* Have the following been set? */
static char *arts;
static char *bin;
static char *ctl;
#define	DIRS()	if (!dirsset) setdirs()
void setdirs();

extern char *strcpy();
extern char *strcat();
extern char *getenv();

extern void unprivileged();	/* user-supplied privilege dropper */

/*
 - spoolfile - historical synonym for artfile
 */
char *
spoolfile(base)
char *base;
{
	static char wholefile[MAXFILE];

	DIRS();

	if (base == NULL) {	/* he just wants the directory */
		(void) strcpy(wholefile, arts);
		return wholefile;
	}

	if (pwd != NULL && STREQ(pwd, arts))
		(void) strcpy(wholefile, "");
	else {
		(void) strcpy(wholefile, arts);
		(void) strcat(wholefile, SFNDELIM);
	}
	(void) strcat(wholefile, base);

	return wholefile;
}

/*
 - artfile - best pathname for a file in NEWSARTS
 */
char *
artfile(base)
char *base;
{
	return spoolfile(base);
}

/*
 - fullartfile - full pathname for a file in NEWSARTS
 */
char *
fullartfile(base)
char *base;
{
	register char *p;
	register char *pwdsave;

	pwdsave = pwd;
	pwd = NULL;		/* fool spoolfile() into giving full path */
	p = spoolfile(base);
	pwd = pwdsave;
	return p;
}

/*
 - fullspoolfile - historical synonym for fullartfile
 */
char *
fullspoolfile(base)
char *base;
{
	return fullartfile(base);
}

/*
 - libfile - historical synonym for ctlfile
 */
char *
libfile(base)
char *base;
{
	static char wholefile[MAXFILE];

	DIRS();

	(void) strcpy(wholefile, ctl);
	if (base != NULL) {
		(void) strcat(wholefile, SFNDELIM);
		(void) strcat(wholefile, base);
	}
	return wholefile;
}

/*
 - ctlfile - full pathname for a file in NEWSCTL
 */
char *
ctlfile(base)
char *base;
{
	return libfile(base);
}

/*
 - binfile - full pathname for a file in NEWSBIN
 */
char *
binfile(base)
char *base;
{
	static char wholefile[MAXFILE];

	DIRS();

	(void) strcpy(wholefile, bin);
	if (base != NULL) {
		(void) strcat(wholefile, SFNDELIM);
		(void) strcat(wholefile, base);
	}
	return wholefile;
}

/*
 - cd - change to a directory, with checking
 */
void
cd(dir)
char *dir;
{
	if (chdir(dir) < 0)
		errunlock("cannot chdir(%s)", dir);
	pwd = dir;
}

/*
 - setdirs - set up directories from environment, for use by other functions
 *
 * Invokes user-supplied function unprivileged() if non-standard values used.
 */
static void
setdirs()
{
	register char *p;
	register int nonstd = NO;

	if (dirsset)
		return;

	p = getenv("NEWSARTS");
	if (p == NULL)
		arts = NEWSARTS;
	else {
		arts = p;
		nonstd = YES;
	}

	p = getenv("NEWSCTL");
	if (p == NULL)
		ctl = NEWSCTL;
	else {
		ctl = p;
		nonstd = YES;
	}

	p = getenv("NEWSBIN");
	if (p == NULL)
		bin = NEWSBIN;
	else {
		bin = p;
		nonstd = YES;
	}

	dirsset = YES;
	if (nonstd)
		unprivileged();
}

/*
 - newsumask - return suitable value of umask for news stuff
 */
int
newsumask()
{
	register char *p;
	register char *scan;
	register int mask;

	p = getenv("NEWSUMASK");
	if (p == NULL)
		return(NEWSUMASK);
	else {
		mask = 0;
		for (scan = p; *scan != '\0'; scan++)
			if ('0' <= *scan && *scan <= '7' && mask <= 077)
				mask = (mask << 3) | (*scan - '0');
			else	/* Garbage, ignore it. */
				return(NEWSUMASK);
		return(mask);
	}
}
