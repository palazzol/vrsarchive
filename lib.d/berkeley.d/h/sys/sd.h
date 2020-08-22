/*
 *	%W% %G%
*/
#include "/usr/include/sys/sd.h"

/*
 *	Why doesn't <sys/sd.h> do this?
*/
#ifdef lint			/* Lint doesn't understand arguments	*/
extern char *sdget();
extern int sdenter();
extern int sdleave();
extern int sdfree();
extern int sdgetv();
extern int sdwaitv();
#else
extern char *sdget(/*char *, int, int, int*/);	/* Blasted varargs!	*/
extern int sdenter(char *, int);
extern int sdleave(char *);
extern int sdfree(char *);
extern int sdgetv(char *);
extern int sdwaitv(char *, int);
#endif lint
