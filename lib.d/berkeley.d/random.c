/*
 *	%W% %G%
 *	Random number generation routines.
*/
extern void srand(unsigned);
extern int rand();

void
srandom(seed)
int seed;
{ srand((unsigned)seed);
}

long
random()
{ return(rand());
}
