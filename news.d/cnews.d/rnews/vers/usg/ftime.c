/*
 * Uglix ftime simulation
 */

#include <sys/types.h>
#include <sys/timeb.h>		/* HACK */

ftime(tp)
struct timeb *tp;
{
	time_t time();

	tp->time = time(&tp->time);
	tp->millitm = 0;
	tp->timezone = 5*60;	/* HACK */
	tp->dstflag = 1;	/* HACK */
}
