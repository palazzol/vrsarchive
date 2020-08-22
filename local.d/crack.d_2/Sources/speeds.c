#include <sys/time.h>
#include <signal.h>
#include <stdio.h>

static int cnt;
#define ITIME	10		/* Number of seconds to run test. */

void
Stop ()
{
    printf ("Did %f %s()s per second.\n",
	    ((float) cnt) / ((float) ITIME),
#if defined(T1)
	    "fcrypt"
#elif defined(T2)
	    "XForm"
#else
	    "crypt"
#endif
    );
    exit (0);
}
main ()
{
    struct itimerval itv;
    static int quarters[4];

    bzero (&itv, sizeof (itv));

    printf ("Running for %d seconds of virtual time ...\n", ITIME);

#if defined(T1) || defined(T2)
    init_des ();
#endif

    signal (SIGVTALRM, Stop);
    itv.it_value.tv_sec = ITIME;
    itv.it_value.tv_usec = 0;
    setitimer (ITIMER_VIRTUAL, &itv, NULL);

    for (cnt = 0;; cnt++)
    {
#if defined(T1)
	fcrypt ("fredfred", "eek");
#elif defined(T2)
	XForm (quarters, 0);
#else
	crypt ("fredfred", "eek");
#endif
    }
}
