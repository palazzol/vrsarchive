/*
 * This program is copyright (c) Alec Muffett 1991 except for certain
 * portions of code ("fdes-crypt.c") copyright (c) Robert Baldwin, Icarus
 * Sparry and Alec Muffett.  The author(s) disclaims all responsibility or
 * liability with respect to it's usage or its effect upon hardware or
 * computer systems.  This software is in freely redistributable PROVIDED
 * that this notice remains intact.
 */

#include "conf.h"

#define STRINGSIZE	255

extern void Trim ();
extern char *Reverse ();
extern char *Uppercase ();
extern char *Lowercase ();
extern char *Clone ();
extern char *gethostname ();

#ifdef FAST_TOCASE
#define toupper(x) 	_toupper(x)
#define tolower(x)	_tolower(x)
#endif

#ifdef FCRYPT
#define crypt(a,b)	fcrypt(a,b)
#endif

#ifdef INDEX_NOT_STRCHR
#define strchr(a,b) 	index(a,b)
#endif
