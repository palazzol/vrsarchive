/*
 * Operating system dependent routines
 *
 * This version is for UNIX 4.2 bsd.
 * Note the random function must be truely random, otherwise things
 * don't work out as well.
 */
#include <sgtty.h>
static struct sgttyb newp,oldp;
extern long time();

Oinit()			/* initialize */
{
  srand((unsigned)(getuid()+time(0)));
  ioctl(0, TIOCGETP, &oldp);
  ioctl(0, TIOCGETP, &newp);
  newp.sg_flags &= ~ECHO;
  newp.sg_flags |= CBREAK;
  ioctl(0, TIOCSETP, &newp);
}

Oterminate()		/* terminate */
{
  ioctl(0, TIOCSETP, &oldp);
}

int Orandom(lower,upper)	/* generate a random integer between	*/
int lower,upper;		/* lower and upper (inclusive)		*/
{ extern int rand();
  float R;
  R = (float)rand();
  R /= (float)(((unsigned)~0)>>1);
  R *= (float)(upper - lower);
  R += (float)lower;
  return( (int)(R + 0.5) );
}
