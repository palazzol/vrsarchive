#include <nlist.h>
#include <stdio.h>

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
	{
	    syserror("Can't open /dev/kmem.");
	    return(-1);                         /* Cannot open. */
	}
	if (nlist("/vmunix", nl) == -1)
	    if (nlist("/unix", nl) == -1)
	    {
		error("Can't get avenrun from nlist in kernel.");
		return -1;
	    }
    }
    if (lseek(kmem,(long)nl[0].n_value, 0) == -1)
    {
	syserror("Can't seek in /dev/kmem.");
	return(-1);    /* sizeof avenrun is 24. */
    }
    if (read(kmem,avenrun,3 * sizeof(double)) == -1)
    {
	syserror("Can't read from /dev/kmem.");
	return(-1);    /* sizeof avenrun is 24. */
    }
    return(0);
}

syserror (str)
    char *str;
{
    extern int errno;
    extern char *sys_errlist[];
    fprintf(stderr, "%s (%s)\n", str, sys_errlist[errno]);
}

error (str)
    char *str;
{
    fprintf(stderr, "%s\n", str);
}
