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
{   FILE *actfp, *artfp, *rcfp, *outfp;
    int ngfirst, nglast, rclast;
    char ngname[256];
    char rcname[256];
    char buf[256];
    register char *p, *q;
    int found, count;
    struct stat statbuf;

    argv++;
    p = *argv;
    if (!p)
	abort();
    rcfp = fopen(p, "r");
    if (!rcfp)
	abort();
    actfp = nntp_active();
    if (!actfp)
	abort();
    while (fscanf(actfp, " %s %d %d %*s", ngname, &nglast, &ngfirst) == 3) {
	fseek(rcfp, 0L, 0);
	found = 0;
	while (fscanf(rcfp, " %s %d", rcname, &rclast) == 2) {
	    if (strcmp(rcname, ngname) == 0) {
		found = 1;
		break;
	    }
	}
	if (!found) {
	    strcpy(rcname, ngname);
	    rclast = ngfirst - 1;
	}
	if (ngfirst < rclast+1)
	    ngfirst = rclast+1;
	q = ngname;
	found = 0;
	for (p = ngname; *p; p++) {
	    if (*p == '.') {
		*p = '\0';
		if (!found && cklist(argv, q))
		    found = 1;
		*p = '/';
		q = p+1;
	    }
	}
	if (!found && cklist(argv, q))
	    found = 1;
	if (!found)
	    continue;
	if (ngfirst <= nglast)
	    fprintf(stderr, "%s: %d-%d\n", rcname, ngfirst, nglast);
	for (found = ngfirst; found <= nglast; found++) {
	    sprintf(buf, "/usr/spool/news/%s/%d", ngname, found);
	    artfp = nntp_open(buf);
	    if (!artfp)
		continue;
	    for (;;) {
		sprintf(buf, "%d", n++);
		if (stat(buf, &statbuf) != 0)
		    break;
	    }
	    outfp = fopen(buf, "w");
	    if (!outfp)
		goto bail;
	    for (;;) {
		count = fread(buf, 1, sizeof(buf), artfp);
		if (count <= 0)
		    break;
		if (count != fwrite(buf, 1, count, outfp))
		    goto bail;
	    }
	    fclose(artfp);
	    fclose(outfp);
	}
bail:	found--;
	printf("%s %d\n", rcname, found);
    }
    return(0);
}
