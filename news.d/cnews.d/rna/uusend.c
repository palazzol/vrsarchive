/*
 * send article on standard input to remote host via mail
 */

#include "defs.h"

char mail[]		 = FASTMAIL;

main(argc, argv)
int argc;
char *argv[];
{
	register int c, lastc;
	register FILE	*out;

	if (argc != 2) {
		fprintf(stderr, "Usage: uusend address\n");
		exit(1);
	}
	if ((out = tmpfile()) == NULL) {
		fprintf(stderr, "uusend: can't create tempfile\n");
		exit(1);
	}

	fprintf(out, "Subject: network news article\n");
	fprintf(out, "\n");

	lastc = '\n';
	while ((c = getc(stdin)) != EOF) {
		if (lastc == '\n')
			putc('N', out);
		putc(lastc = c, out);
	}

	rewind(out);
	fclose(stdin);
	dup(fileno(out));
	close(fileno(out));

	execl(mail, strrchr(mail, '/') + 1, argv[1], 0);
	fprintf(stderr, "uusend: can't exec %s\n", mail);
}


