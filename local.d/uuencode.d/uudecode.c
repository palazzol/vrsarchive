/* #ifndef lint
static char sccsid[] = "@(#)uudecode.c  5.3-1 (Berkeley) 9/1/87";
#endif */

/* Written by Mark Horton */
/* Modified by ajr (Alan J Rosenthatl,flaps@utcsri.UUCP) to use checksums */
/* Modified by fnf (Fred Fish,well!fnf) to use Keith Pyle's suggestion for
   compatibility */
/* Modified by bcn (Bryce Nesbitt,ucbvax!cogsci!bryce) to fix a misleading
   error message on the Amiga port, to fix a bug that prevented decoding
   certain files, to work even if trailing spaces have been removed from a
   file, to check the filesize (if present), to add some error checking, to
   loop for multiple decodes from a single file, and to handle common
   BITNET mangling.  Also kludged around a missing string function in Aztec
   C */

/*
 * uudecode [input]
 *
 * Decode a file encoded with uuencode.  WIll extract multiple encoded
 * modules from a single file.	Can deal with most mangled files, including
 * BITNET.
 */

#include <stdio.h>
#include <ctype.h>

#ifdef AMIGA
#define AMIGA_LATTICE	    /* Set for Amiga Lattice C */
#define MCH_AMIGA
#define MPU68000
#endif

#ifdef unix
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#define SUMSIZE 64
#define DEC(c)	(((c) - ' ') & 077)    /* single character decode */

main(argc, argv)
char **argv;
{
FILE	*in, *out;
int	through_loop=0; /* Dejavu indicator */
int	mode;		/* file's mode (from header) */
long	filesize;	/* theoretical file size (from header) */
char	dest[128];
char	buf[80];

#ifdef AMIGA_LATTICE
extern	int Enable_Abort;
	Enable_Abort=1;
#endif

    /* A filename can be specified to be uudecoded, or nothing can
    be specified, and the input will come from STDIN */

    switch (argc)
	{
	case 1:
	in=stdin;
	break;

	case 2:
	if ((in = fopen(argv[1], "r")) == NULL)
	    {
	    fprintf(stderr, "ERROR: can't find %s\n", argv[1]);
	    fprintf(stderr, "USAGE: uudecode [infile]\n");
	    exit(10);
	    }
	break;

	default:
	fprintf(stderr, "USAGE: uudecode [infile]\n");
	exit(11);
	break;
	}

    /* Loop through file, searching for headers.  Decode anything with a
       header, complain if there where no headers. */

for (;;)
{
    /* search file for header line */
    for (;;)
	{
	if (fgets(buf, sizeof buf, in) == NULL)
	    {
	    if (!through_loop)
		{
		fprintf(stderr, "ERROR: no `begin' line!\n");
		exit(12);
		}
	    else
		{
		exit(0);
		}
	    }
	if (strncmp(buf, "begin ", 6) == 0)
	    break;
	}
    sscanf(buf, "begin %o %s", &mode, dest);

#ifdef unix
    /* handle ~user/file format */
    if (dest[0] == '~')
	{
	char *sl;
	struct passwd *getpwnam();
	char *index();
	struct passwd *user;
	char dnbuf[100];

	sl = index(dest, '/');
	if (sl == NULL)
	    {
	    fprintf(stderr, "Illegal ~user\n");
		exit(13);
	    }
	*sl++ = 0;
	user = getpwnam(dest+1);
	if (user == NULL)
	    {
	    fprintf(stderr, "No such user as %s\n", dest);
	    exit(14);
	    }
	strcpy(dnbuf, user->pw_dir);
	strcat(dnbuf, "/");
	strcat(dnbuf, sl);
	strcpy(dest, dnbuf);
	}
#endif

    /* create output file */
    if ((out = fopen(dest, "w")) == NULL)
	{
	fprintf(stderr, "ERROR: can't open output file %s\n", dest);
	exit(15);
	}
#ifdef unix
    chmod(dest, mode);
#endif

    decode(in, out, dest);

    if (fgets(buf, sizeof buf, in) == NULL || strncmp(buf,"end",3))
	{	       /* don't be overly picky about newline ^ */
	fprintf(stderr, "ERROR: no `end' line\n");
	exit(16);
	}

    if (!(fgets(buf,sizeof buf,in) == NULL || strncmp(buf,"size ",3)))
	{
	sscanf(buf, "size %ld", &filesize);
	if (ftell(out) != filesize)
	    {
	    fprintf(stderr, "ERROR: file should have been %ld bytes long but was %ld.\n", filesize, ftell(out));
	    exit(17);
	    }
	}
    through_loop = 1;
}   /* forever */
}   /* main */

/*
 * Copy from in to out, decoding as you go.
 * If a return or newline is encountered too early in a line, it is
 * assumed that means that some editor has truncated trailing spaces.
 */
decode(in, out, dest)
FILE *in;
FILE *out;
char *dest;
{
char buf[81];
char *bp;
int nosum=0;
#ifndef unix
extern errno;
#endif
register int j;
register int n;
int checksum, line;

    for (line = 1; ; line++)	/* for each input line */
	{
	if (fgets(buf, sizeof buf, in) == NULL)
	    {
	    fprintf(stderr, "ERROR: input ended unexpectedly!\n");
	    exit(18);
	    }

	/* Pad end of lines in case some editor truncated trailing
	   spaces */

	for (n=0;n<79;n++)  /* search for first \r, \n or \000 */
	    {
	    if (buf[n]=='\176')     /* If BITNET made a twiddle, */
		buf[n]='\136';     /* we make a caret           */
	    if (buf[n]=='\r'||buf[n]=='\n'||buf[n]=='\000')
		break;
	    }
	for (;n<79;n++)     /* when found, fill rest of line with space */
	    {
	    buf[n]=' ';
	    }
	buf[79]=0;	    /* terminate new string */

	checksum = 0;
	n = DEC(buf[0]);
	if (n <= 0)
	    break;	/* 0 bytes on a line??	Must be the last line */

	bp = &buf[1];

	/* FOUR input characters go into each THREE output charcters */

	while (n >= 4)
	    {
	    j = DEC(bp[0]) << 2 | DEC(bp[1]) >> 4; putc(j, out); checksum += j;
	    j = DEC(bp[1]) << 4 | DEC(bp[2]) >> 2; putc(j, out); checksum += j;
	    j = DEC(bp[2]) << 6 | DEC(bp[3]);	   putc(j, out); checksum += j;
	    checksum = checksum % SUMSIZE;
	    bp += 4;
	    n -= 3;
	    }

	    j = DEC(bp[0]) << 2 | DEC(bp[1]) >> 4;
		checksum += j;
		if (n >= 1)
		    putc(j, out);
	    j = DEC(bp[1]) << 4 | DEC(bp[2]) >> 2;
		checksum += j;
		if (n >= 2)
		    putc(j, out);
	    j = DEC(bp[2]) << 6 | DEC(bp[3]);
		checksum += j;
		if (n >= 3)
		    putc(j, out);
	    checksum = checksum % SUMSIZE;
	    bp += 4;
	    n -= 3;

#ifndef unix
	 /* Error checking under UNIX??? You must be kidding... */
	 /* Check if an error occured while writing to that last line */
	if (errno)
	    {
	    fprintf(stderr, "ERROR: error writing to %s\n",dest);
	    exit(19);
	    }
#endif

	/* The line has been decoded; now check that sum */

	nosum |= !isspace(*bp);
	if (nosum)			/* Is there a checksum at all?? */
	    {
	    if (checksum != DEC(*bp))	/* Does that checksum match? */
		{
		fprintf(stderr, "ERROR: checksum mismatch decoding %s, line %d.\n",dest, line);
		}
	    }	/* sum */
    }	/* line */
}   /* function */

#ifdef unix
/*
 * Return the ptr in sp at which the character c appears;
 * 0 if not found
 */
char *
index(sp, c)
register char *sp, c;
{
    do
	{
	if (*sp == c)
	    return(sp);
	}
    while (*sp++);

    return(0);
}
#endif unix

