#ifndef BYTES
#define BYTES 10*1024
#endif
#ifndef LOOPS
#define LOOPS 1000
#endif
#ifndef FROMALIGN
#define FROMALIGN 0
#endif
#ifndef TOALIGN
#define TOALIGN 0
#endif

main()		/* time memcpy(3) */
{
	register int count = LOOPS;
	static char from[BYTES], to[BYTES];

	while (count-- > 0)
		memcpy(to, from, sizeof from - FROMALIGN - TOALIGN);
	return 0;
}
