#include <stdio.h>
#include <ctype.h>
main (argc, argv) char **argv;
{
	while (--argc) include (*++argv, 0);
}

include (file, depth)
{
	int 	indent;
	FILE	*ioptr;
	for (indent = 0; indent < depth; indent++) putchar ('\t');
	if (ioptr = fopen (file, "r")) {
		char	line[BUFSIZ];
		register char *lptr;
		printf ("%s\n", file);
		while (fgets (lptr = line, BUFSIZ, ioptr))
			if (*line == '#') {
				lptr++;
				while (isspace (*lptr)) lptr++;
				if (!strncmp (lptr, "include", 7)) {
					int 	matcher;
					char	*eptr;
					lptr += 7;
					while (isspace (*lptr)) lptr++;
					matcher = *lptr++;
					if (matcher == '<') matcher = '>';
					for (eptr = lptr; *eptr && *eptr != matcher; eptr++);
					*eptr = NULL;
					if (matcher == '>') {
						char	fullname[BUFSIZ];
						sprintf (fullname, "/usr/include/%s", lptr);
						lptr = fullname;
					}
					include (lptr, depth+1);
				}
				}
		fclose (ioptr);
	}
	else printf ("%s: can't open!\n", file);
}
