#include "macros.h"

get_rand(x, y)
int x, y;
{
#ifdef srandom
	/*
	 *	If srandom is defined, it should be defined to be srand.
	 *	This causes the conditional compilation here to use the
	 *	older (more standard?) srand/rand routines.
	*/
#define random rand
	unsigned r, random();
#else
	long r, random();
#endif
	int s;

	two_sort(x, y);

	r = random();
	r = (r % ((y-x)+1)) + x;
	s = (int) r;
	return(s);
}

rand_percent(percentage)
{
	return(get_rand(1, 100) <= percentage);
}
