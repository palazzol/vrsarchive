/*
 * Interface routines for dungeon.
 * These routines are for functions expected by the game
 * that are not available in the Unix/f77 library.
 */

/* routine to get time in hours minutes and seconds */

#include <sys/types.h>
#include <sys/timeb.h>
/* for V7 this should be <time.h> */
#include <sys/time.h>


long time();
struct tm *localtime();
struct tm *tmptr;
long timebuf;

itime_(hrptr,minptr,secptr)

int *hrptr,*minptr,*secptr;
{

	time(&timebuf);
	tmptr = localtime(&timebuf);
	
	*hrptr  = tmptr->tm_hour;
	*minptr = tmptr->tm_min;
	*secptr = tmptr->tm_sec;

	return;
}

/* random number initializer */
inirnd_(seedptr)

int *seedptr;
{
int seed;

	seed = *seedptr;
	srand(seed);
	return;
}

/*  random number generator */
rnd_(maxval)

int *maxval;
{
/* note: returned random number ranges from 0 to maxval */

int rndval;

	rndval = rand();

	rndval = rndval % *maxval;

	return(rndval);
}
