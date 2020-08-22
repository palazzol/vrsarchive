/*
 *	%W% %G%
 *	Definitions related to time.
*/
#include "/usr/include/time.h"

/*
 *	Structure passed to select emulation to implement timeout.
*/
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds (ignored) */
};

/*
 *	Why doesn't /usr/include/time.h do this?
*/
#ifdef lint
extern long time();
#else
extern long time(long *);
#endif lint
