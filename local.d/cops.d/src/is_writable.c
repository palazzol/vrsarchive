#include <sys/types.h>
#include <math.h>
#include <sys/stat.h>

#define G_WRITE 0020	/* group writable */
#define W_WRITE 0002	/* world writable */

main(argc,argv)
char **argv;
{
	register int group = 0, xmode = 0;
	static struct stat statb;

	if (argc < 2) {
		printf("Usage: %s [-g] file\n",argv[0]);
		exit(0);
	}

	if (argc > 2) {
		if (!strcmp(argv[1], "-g")) {
			group = 1;
			argc--;
			argv++;
		}
	}

	if (stat(*++argv,&statb) < 0) {
		exit(2);
	}

	if (group) xmode = statb.st_mode & G_WRITE;
	else xmode = statb.st_mode & W_WRITE;

	exit(!xmode);
}

