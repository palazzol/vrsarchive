/* based on 5.1 (Berkeley) 7/2/83 */

/*
 * uuencode [input] output
 *
 * Encode a file so it can be mailed to a remote system.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main(argc, argv)
char **argv;
{
	FILE *in;
	struct stat sbuf;
	int mode;

	/* optional 1st argument */
	if (argc > 2) {
		if ((in = fopen(argv[1], "r")) == NULL) {
			perror(argv[1]);
			exit(1);
		}
		argv++; argc--;
	} else
		in = stdin;

	if (argc != 2) {
		(void) fprintf(stderr, "Usage: uuencode [infile] remotefile\n");
		exit(2);
	}

	/* figure out the input file mode */
	(void) fstat(fileno(in), &sbuf);
	mode = sbuf.st_mode & 0777;
	(void) printf("begin %o %s\n", mode, argv[1]);

	encode(in, stdout);

	(void) printf("end\n");
	exit(0);
}

/*
 * copy from in to out, encoding as you go along.
 */
encode(in, out)
FILE *in;
FILE *out;
{
	register char *ibp, *obp;
	register int inbyte, outbyte;
	register char *ibufend;
	register int incount;
	char inbuf[45+1+3];		/* +3 is due to sloppy for test */
	char outbuf[1+60+1+1];		/* len+text+newline */

/* ENC is the basic 1 character encoding function to make a char printing */
#define ENC(c) (((c) & 077) + ' ')	/* N.B.: c is evaluated exactly once. */

	do {
		incount = fread(inbuf, 1, 45, in);
		ibp = inbuf;
		obp = outbuf;
		*obp++ = ENC(incount);
		for (ibufend = inbuf + incount; ibp < ibufend; ) {
			/*
			 * Encode (as 4 bytes pointed at by obp) one group
			 * of 3 bytes pointed at by ibp.
			 * Code is obscure to make it blaze along (sorry).
			 * Each "*obp++ = ENC(...)" emits the next 6 bits of
			 * the input bytes.
			 */
			*obp++ = ENC((inbyte = *ibp++) >> 2);
			outbyte = (inbyte << 4) /* & 060 */ ;
			*obp++ = ENC(outbyte | (((inbyte = *ibp++) >> 4) & 017));
			outbyte = (inbyte << 2) /* & 074 */ ;
			*obp++ = ENC(outbyte | (((inbyte = *ibp++) >> 6) & 03));
			*obp++ = ENC(inbyte /* & 077 */ );
		}
		*obp++ = '\n';
		*obp = '\0';
		(void) fwrite(outbuf, 1, obp - outbuf, out);
	} while (incount > 0);
}
