/*
**  Checkout program for rfc822-address parser
**  Takes address-lines from stdin up to EOF or empty line.
**  Writes results to stdout, errors to stderr.
*/
#include <stdio.h>
#include "addr.h"


static void
prtroute(dp)
    register Dom	*dp;
{
    register char	**p;

    for (; dp; dp = dp->next) {
	printf("\t");
	for (p = dp->sub; p != dp->top; p++)
	    printf("%s.", *p);
	printf("%s\n", *p);
    }
}


main()
{
    register Addr	*ap;
    register int	 i;
    char		 line[100];

    while (gets(line) && *line) {
	parseit(line);
	for (i = 0; i < 2; i++)
	    for (ap = i ? adrlist : errlist; ap; ap = ap->next) {
		if (ap->error)
		    printf("error:\t%s\n", ap->error);
		if (ap->name)
		    printf("name:\t%s\n", ap->name);
		if (ap->comment)
		    printf("comment:\t%s\n", ap->comment);
		if (ap->group)
		    printf("group:\t%s\n", ap->group);
		if (ap->localp)
		    printf("localp:\t%s\n", ap->localp);
		if (ap->destdom) {
		    printf("destdom:\n");
		    prtroute(ap->destdom);
		}
		if (ap->route) {
		    printf("route:\n");
		    prtroute(ap->route);
		}
		printf("\n");
	    }
    }
    exit(0);
}
