/* #ifndef lint
static char sccsid[] = "@(#)uuencode.c  5.3-1 (Berkeley) 9/1/87";
#endif */

/* Written by Mark Horton */
/* Modified by ajr (Alan J Rosenthatl,flaps@utcsri.UUCP) to use checksums */
/* Modified by fnf (Fred Fish,well!fnf) to use Keith Pyle's suggestion for
   compatibility */
/* Modified by bcn (Bryce Nesbitt,ucbvax!cogsci!bryce) to enable CTRL-C for
   Amiga Lattice C and add a transparant file size trailer for later check. */

/*
 * uuencode >outfile [infile] name
 *
 * Encode a file so it can be mailed to a remote system.  This version
 * transparantly adds line checksums and a file size for sanity checks.
 *
 */

#include <stdio.h>

#ifdef	AMIGA			/* Amiga Lattice C */
#define AMIGA_LATTICE
#define MCH_AMIGA
#define MPU68000
#endif

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#endif

#define SUMSIZE 64  /* 6 bits */
/* ENC is the basic 1 character encode function to make a char printing */
/* Each output character represents 6 bits of input */
#define ENC(c) ((c) ? ((c) & 077) + ' ': '`')
long	totalsize=0;	/* Used to count the file size because ftell() does
			   not return sane results for pipes */

main(argc, argv)
char **argv;
{
    FILE *in;
    int mode;
#ifdef unix
    struct stat sbuf;
#endif
#ifdef AMIGA_LATTICE
    extern int Enable_Abort;	/* Enable CTRL-C for Lattice */
    Enable_Abort=1;
#endif

	/* optional 1st argument */
	if (argc > 2) {
		if ((in = fopen(argv[1], "r")) == NULL) {
			fprintf(stderr, "ERROR: can't find %s\n", argv[1]);
			fprintf(stderr, "USAGE: uuencode >outfile [infile] name\n");
			exit(10);
		}
		argv++; argc--;
	} else
		in = stdin;

	if (argc != 2) {
		fprintf(stderr, "USAGE: uuencode >outfile [infile] name\n");
		exit(11);
	}

#ifdef unix
	/* figure out the input file mode */
	fstat(fileno(in), &sbuf);
	mode = sbuf.st_mode & 0777;
#else
	mode = 0644;	    /* Default permissions */
#endif

	printf("\nbegin %o %s\n", mode, argv[1]);

	encode(in, stdout);

	printf("end\n");
	printf("size %ld\n",totalsize);
	exit(0);
}

/*
 * copy from in to out, encoding as you go along.
 */
encode(in, out)
FILE *in;
FILE *out;
{
#ifndef unix
extern errno;
#endif
	char buf[80];
	int i, n, checksum;

	for (;;) {
		/* 1 (up to) 45 character line */
		n = fr(in, buf, 45);
		putc(ENC(n), out);

		checksum = 0;
		for (i=0; i<n; i += 3)
		    checksum = (checksum+outdec(&buf[i], out)) % SUMSIZE;

		putc(ENC(checksum), out);
		putc('\n', out);

#ifndef unix
		/* Error checking under UNIX?? You must be kidding! */
		if (ferror(out)) {
		    perror("error writing to output");
			exit(12);
		    }
#endif
		if (n <= 0)
			break;
	}
}

/*
 * output one group of 3 bytes, pointed at by p, on file f.
 * return the checksum increment.
 */
int outdec(p, f)
char *p;
FILE *f;
{
	int c1, c2, c3, c4;

	c1 = *p >> 2;
	c2 = (*p << 4) & 060 | (p[1] >> 4) & 017;
	c3 = (p[1] << 2) & 074 | (p[2] >> 6) & 03;
	c4 = p[2] & 077;
	putc(ENC(c1), f);
	putc(ENC(c2), f);
	putc(ENC(c3), f);
	putc(ENC(c4), f);

	return((p[0]+p[1]+p[2]) % SUMSIZE);
}

/* fr: like read but stdio */
int
fr(fd, buf, cnt)
FILE *fd;
char *buf;
int cnt;
{
	int c, i;

	for (i=0; i<cnt; i++) {
		c = getc(fd);
		if (c == EOF)
			return(i);
		totalsize++;
		buf[i] = c;
	}
	return (cnt);
}
