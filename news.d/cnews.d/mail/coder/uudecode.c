
/* based on 5.1 (Berkeley) 7/2/83 */

/*
 * uudecode [input]
 *
 * create the specified file, decoding as you go.
 * used with uuencode.
 */
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

main(argc, argv)
char **argv;
{
	FILE *in, *out;
	int mode = 0777;
	char dest[128];
	char buf[80];
	char *strcpy(), *strcat();

	/* optional input arg */
	if (argc > 1) {
		if ((in = fopen(argv[1], "r")) == NULL) {
			perror(argv[1]);
			exit(1);
		}
		argv++; argc--;
	} else
		in = stdin;

	if (argc != 1) {
		(void) fprintf(stderr, "Usage: uudecode [infile]\n");
		exit(2);
	}

	/* search for header line */
	for (;;) {
		if (fgets(buf, sizeof buf, in) == NULL) {
			(void) fprintf(stderr, "No begin line\n");
			exit(3);
		}
		if (strncmp(buf, "begin ", 6) == 0)
			break;
	}
	dest[0] = '\0';
	(void) sscanf(buf, "begin %o %s", &mode, dest);

	/* handle ~user/file format */
	if (dest[0] == '~') {
		char *sl;
		struct passwd *getpwnam();
		char *index();
		struct passwd *user;
		char dnbuf[100];

		sl = index(dest, '/');
		if (sl == NULL) {
			(void) fprintf(stderr, "Illegal ~user\n");
			exit(3);
		}
		*sl++ = 0;
		user = getpwnam(dest+1);
		if (user == NULL) {
			(void) fprintf(stderr, "No such user as %s\n", dest);
			exit(4);
		}
		(void) strcpy(dnbuf, user->pw_dir);
		(void) strcat(dnbuf, "/");
		(void) strcat(dnbuf, sl);
		(void) strcpy(dest, dnbuf);
	}

	/* create output file */
	out = fopen(dest, "w");
	if (out == NULL) {
		perror(dest);
		exit(4);
	}
	(void) chmod(dest, mode);

	decode(in, out);

	if (fgets(buf, sizeof buf, in) == NULL || strcmp(buf, "end\n")) {
		(void) fprintf(stderr, "No end line\n");
		exit(5);
	}
	exit(0);
}

/*
 * copy from in to out, decoding as you go along.
 */
decode(in, out)
FILE *in;
FILE *out;
{
	register char *ibp, *obp;
	register int inbyte, outbyte;
	register int loops, incount;
	char inbuf[120];		/* allow for uuencode to grow lines */
	char outbuf[120];

/* single character decode */
#define DEC(c)	(((c) - ' ') & 077)	/* N.B.: c is evaluated exactly once */

	for (; ; ) {
		if (fgets(inbuf, sizeof inbuf, in) == NULL) {
			(void) printf("Short file\n");
			exit(10);		/* premature EOF */
		}
		incount = DEC(inbuf[0]);
		if (incount <= 0)
			break;			/* EOF */

		ibp = &inbuf[1];		/* skip line length */
		obp = outbuf;
		for (loops = incount / 3; --loops >= 0; ) {	/* inner loop */
			/*
			 * output a group of 3 bytes (4 input characters).
			 * the input chars are pointed to by ibp, they are to
			 * be output via obp.  incount is used to tell us
			 * not to output all of them at the end of the line.
			 */
			outbyte = DEC(*ibp++) << 2;
			*obp++ = outbyte | ((inbyte = DEC(*ibp++)) >> 4);
			outbyte = inbyte << 4;
			*obp++ = outbyte | ((inbyte = DEC(*ibp++)) >> 2);
			*obp++ = (inbyte << 6) | DEC(*ibp++);
		}
		loops = incount % 3;
		if (loops > 0) {
			/*
			 * finish off the remaining bytes (loops < 3).
			 */
			if (--loops >= 0) {
				outbyte = DEC(*ibp++) << 2;
				*obp++ = outbyte | ((inbyte = DEC(*ibp++)) >> 4);
			}
			if (--loops >= 0) {
				outbyte = inbyte << 4;
				*obp++ = outbyte | ((inbyte = DEC(*ibp++)) >> 2);
			}
			if (--loops >= 0)
				*obp++ = (inbyte << 6) | DEC(*ibp++);
		}
		*obp = '\0';
		(void) fwrite(outbuf, 1, obp - outbuf, out);
	}
}
