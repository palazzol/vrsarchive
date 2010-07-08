/*
 * Operating system dependent routines
 *
 * This version is for UNIX 4.2 bsd.
 * Note the random function must be truely random, otherwise things
 * don't work out as well.
 */
extern long time();
#ifdef __STDC__
#include <limits.h>
#endif

Oinit()			/* initialize */
{
  srand((unsigned)(getuid()+time(0)));
}

Oterminate()		/* terminate */
{
}

int Orandom(lower,upper)	/* generate a random integer between	*/
int lower,upper;		/* lower and upper (inclusive)		*/
{ extern int rand();
  float R;
  R = (float)rand();
#ifdef __STDC__
  R /= INT_MAX;
#else
  R /= 32767;
#endif
  R *= (upper - lower);
  R += lower;
  return( (int)(R + 0.5) );
}
