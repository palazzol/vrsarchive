/*
 * uurec - receive articles via /bin/mail.
 */

#include "defs.h"

#undef OTHER

#define FROM	01
#define NLIN	02
#define BLANK	03
#define OTHER	04

#define SKIPPING	010
#define READING		020

#define BFSZ 250

main()
{
	char buf[BFSZ];
	register FILE 	*pipe;
	register int mode, frmflg;

	pipe = stdout;
	mode = SKIPPING;
	frmflg = FALSE;
	while (fgets(buf, BFSZ, stdin) != NULL)
		switch (mode | type(buf)) {
		case FROM | SKIPPING:
			frmflg = TRUE;
			break;

		case FROM | READING:
			if (!frmflg) {
				frmflg = TRUE;
				pclose(pipe);
			}
			break;

		case NLIN | SKIPPING:
			mode = READING;

		case NLIN | READING:
			if (frmflg) {
				frmflg = FALSE;
				if ((pipe = popen(RNEWS, "w")) == NULL) {
					perror("uurec: popen failed");
					exit(1);
				}
			}
			fputs(buf + 1, pipe);
			break;

		case OTHER | SKIPPING:
			break;

		case OTHER | READING:
			pclose(pipe);
			mode = SKIPPING;
		}
	if (pipe)
		pclose(pipe);
	exit(0);
}


type(p)
register char *p;
{
	while (*p == ' ' || *p == '?')
		++p;

	if (*p == 'N')
		return (NLIN);

	if (CMPN(p, ">From", 5) == 0)
		return (FROM);

	if (CMPN(p, "From", 4) == 0)
		return (FROM);

	return(OTHER);
}


