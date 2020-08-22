/* @(#)strings.h	(c) copyright 9/3/86 (Dan Heller) */

/* External function definitions for routines described in string(3).  */
char	*strcat(), *strncat(), *strcpy(), *strncpy();
int	strcmp(), strncmp(), strlen();

#ifdef SYSV
#define sprintf Sprintf
#define	index	strchr
#define	rindex	strrchr
#endif /* SYSV */

char 
    *sprintf(),		/* Sprintf for sysv -- see end of misc.c */
    *any(), 		/* return first char in str2 that exists in str1 */
    *index(), *rindex();
