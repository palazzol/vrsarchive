/*
 *   For each suitable active file line, suck up the news.
 */
#include <stdio.h>
#include <sys/stat.h>

int n = 1;	/* Output file number */

int
cklist(a, s)
char *a[];
char *s;
{   while (*a) {
	if (strcmp(*a++, s) == 0)
	    return(1);
    }
    return(0);
}

main(argc, argv)
int argc;
char *argv[];
{   FILE *actfp;
    int ngfirst, nglast;
    char ngname[256];
    char rcname[256];

    actfp = nntp_active();
    if (!actfp)
	abort();
    while (fscanf(actfp," %s %d %d %s",ngname,&nglast,&ngfirst,rcname) == 4) {
	fprintf(stdout, "%s %d %d %s\n", ngname, nglast, ngfirst, rcname);
    }
    return(0);
}
