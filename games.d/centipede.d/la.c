#include <nlist.h>

loadav(avenrun)         /* Function storing 1,5,15 minute load averages in */
double *avenrun;        /* avenrun; should be declared double avenrun[3];  */
{                       /* Returns -1 if error, 0 otherwise.               */
    static struct nlist nl[] = {
	{"_avenrun"},
	{0},
    };
    static kmem = -17;

    if (kmem < 0)          /* Open necessary files. */
    {
	if ((kmem = open("/dev/kmem",0)) == -1)
	    return(-1);                         /* Cannot open. */
	nlist("/vmunix", nl);
    }
    if (lseek(kmem,(long)nl[0].n_value, 0) == -1 || 
      read(kmem,avenrun,3 * sizeof(double)) == -1)
	return(-1);    /* sizeof avenrun is 24. */
    return(0);
}
