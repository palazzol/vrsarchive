/*
 *	Globals affected by command line options
*/
#ifdef GLOBAL
#  define EXTERN
#  define EQUALS(x)	= (x)
#else
#  define EXTERN	extern
#  define EQUALS(x)
#endif

#define MAXVDISP	4

EXTERN int cmdchar EQUALS('_'&037);
EXTERN int curvdsp EQUALS(0);
EXTERN int trace EQUALS(0);
