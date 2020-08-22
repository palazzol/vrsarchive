/*
I suppose this has been done a million times before...

I was hacked off at the sluggishness of the "which" utility, so I
whipped up this C version while waiting for the shell version to
finish.  It produces identical output as the /usr/ucb version and
doesn't attempt to expand aliases (which is bogus unless one sets
their aliases in the .cshrc file).  It also issues a usage message if
one doesn't give it a list of files.

For those of you who don't have a bezerkly style unix, this program
should still work out Ok.

	Larry Barello

	..!uw-beaver!teltone!larry
*/
#include <stdio.h>
 
char *getenv();
char *strchr();

int
main(ac,av)
char **av;
{
    char *path, *cp;
    char buf[200];
    char patbuf[512];
    int quit, none;

    if (ac < 2) {
	fprintf(stderr, "Usage: %s cmd [cmd, ..]\n", *av);
	exit(1);
    }
    av[ac] = 0;
    for(av++ ; *av; av++) {

	quit = 0;
	none = 1;
	strcpy(patbuf, getenv("PATH"));
	path = patbuf;
	cp = path;

	while(1) {
	    cp = strchr(path, ':');
	    if (cp == NULL) 
		quit++;
	    else
		*cp = '\0';

	    sprintf(buf, "%s/%s", path, *av);
	    path = ++cp;

	    if (access(buf, 1) == 0) {
		printf("%s\n", buf);
		none = 0;
	    }
	    if (quit) {
		if (none)
		    printf("No %s in %s\n", *av, getenv("PATH"));
		break;
	    }
	}
    }
    exit(0);
}
