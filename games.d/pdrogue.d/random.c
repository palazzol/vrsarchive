#include "macros.h"

get_rand(x, y)
int x, y;
{
	long r, random();
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
