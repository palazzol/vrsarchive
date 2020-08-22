/*
 *  Program to try and read Tops-20 Dumper format tapes
 *
 *                   Jim Guyton,  Rand Corp.
 *                   Version 1 (10/20/82)
 *                   jdg:   -n added 6/11/83
 */

#include <stdio.h>
#include <ctype.h>
#include "dump.h"
				/* logfile should be changable */
#define LOGFILE "Logfile"

char *ctime(), *strchr(), *strrchr();
char *unixname();

int  fdTape;                    /* File handle for Dumper-20 format tape */
char tapeblock[TAPEBLK];        /* One logical record from tape */
FILE *fpFile;                   /* Output file handle on extracts */
int debug = 0;
int textflg = 0;                /* Non-zero if retr binary files as text */
int numflg = 0;                 /* Non-zero if using numeric filenames */
int number;                     /* Current output file "number" */

#define TAPE "/dev/rmt0"        /* Default input tape */

int  bytesize;          /* Number of bits/byte in current file */
int  numbytes;          /* Number of bytes in current file */
int  pgcount;           /* Number of twenex pages in file */

char *pattern = 0;      /* Filename match pattern */

/*
	pgm  [-f tapefile] [-t] [-n number] pattern

	no pattern == directory only
	no tapefile == /dev/rmt0
	-t == try to pretend files are 7-bit ascii
	-n == use numeric filenames in extracts, number is 1st name
*/

main(argc, argv)
int argc;
char *argv[];
{
	char *tape = TAPE;              /* Pathname for tape device/file */
	int rc;

	/* Do switch parsing */

	while(argc>1 && argv[1][0] == '-'){
		switch(argv[1][1]){
		case 'f':
			if (argc <= 2) punt("Need filename after -f\n");
			tape = argv[2];
			argc--; argv++;
			break;
		case 't':             /* Force text mode on "binary" files */
			textflg = 1;
			break;
		case 'd':
			debug = atoi(&argv[1][2]);
			printf("Debug value set to %d\n", debug);
			break;
		case 'n':               /* numeric output filenames */
			if (argc <= 2) punt("Need number after -n\n");
			number = atoi(argv[2]);         /* First file name */
			numflg = 1;
			argc--; argv++;
			break;
		default:
			printf("unknown flag %s\n", argv[1]);
			exit(1);
			break;
		}
		argc--;  argv++;
	}


	if (argc > 1)
		pattern = argv[1];

	fdTape = open(tape, 0);         /* Open tape for read */
	if (fdTape == -1)
		punt("Couldn't open 'tape' file %s\n", tape);

	for ( ; ; )             /* Loop till end of tape */
	{
					 /*** Read a block ***/
		rc = read(fdTape, tapeblock, TAPEBLK);
		if (rc != TAPEBLK)
		{       if (rc != 0)
			   punt("Oops.  Read block len=%d\n", rc);

			printf("\nEnd of tape.\n");
			exit(0);        /* Normal exit */
		}

					/*** Do something with it ***/
		switch(getrecordtype(tapeblock))
		{
		  case RectypeData:             /* Data block */
					doDatablock(tapeblock);
					break;

		  case RectypeTphd:             /* Saveset header */
					doSaveset(tapeblock, 0);
					break;

		  case RectypeFlhd:             /* File header */
					doFileHeader(tapeblock);
					break;

		  case RectypeFltr:             /* File trailer */
					doFileTrailer(tapeblock);
					break;

		  case RectypeTptr:             /* Tape trailer */
					doTapeTrailer(tapeblock);
					break;

		  case RectypeUsr:              /* User directory info ? */
					printf("User info record skipped\n");
					break;

		  case RectypeCtph:             /* Continued saveset hdr */
					doSaveset(tapeblock, 1);
					break;

		  case RectypeFill:             /* Fill record */
					printf("Fill record skipped\n");
					break;

		  default:
					punt("Unknown record type 0x%x\n",
						  getrecordtype(tapeblock));
					break;
		}
	}
}

int   masks[32] =       /* bitmasks for different length fields */
{              0x00000001, 0x00000003, 0x00000007,
   0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
   0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
   0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
   0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
   0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
   0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
   0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
   0xffffffff
};


long
getfield(block, wordoff, bitoff, bitlen)
char *block;            /* Tape block record */
int wordoff;            /* 36-bit word offset */
int bitoff;             /* Bit offset of field (from msb) */
int bitlen;             /* Bit length of field */
{
	char *p;                /* Used to point into record */
	long int w32;           /* First 32 bits of the 36 bit word */
	int   w4;               /* Last 4 bits of the 36 bit word */
	long  w = 0;            /* the word to return */


				/* First, the "illegal" kludge */
	if (bitoff == 0 && bitlen == 36)
	{       bitoff = 4;
		bitlen = 32;

	}
	if (bitlen > 32) punt("I can't get that large a field!\n");

	/* A PDP-10 (or 20) 36-bit word is laid out with the first 32 bits
	   as the first 4 bytes and the last 4 bits are the low order 4 bits
	   of the 5th byte.   The high 4 bits of that byte should be zero */

	p = block + (5*wordoff);        /* Get ptr to word of interest */
	w32 = *p++ & 0377;                      /* First byte */
	w32 = (w32 << 8) | (*p++ & 0377);       /* 2nd */
	w32 = (w32 << 8) | (*p++ & 0377);       /* 3rd */
	w32 = (w32 << 8) | (*p++ & 0377);       /* 4th */
	w4  = *p;                               /* 5th */
	if (w4 > 017) punt("Not a PDP-10 tape!  w4=%o\n", w4);


	/* Get the field right justified in the word "w".
	   There are three cases that I have to handle:
	      [1] field is contained in w32
	      [2] field crosses w32 and w4
	      [3] field is contained in w4
	*/

	if (bitoff+bitlen <= 32)        /* [1] field is contained in w32 */
	{
		w = w32 >> (32 - (bitoff+bitlen));
	}
	else if (bitoff <= 32)          /* [2] field crosses boundary */
	{
		w =  (w32 << (bitoff+bitlen-32))
		   | (w4  >> (36 - (bitoff+bitlen)));
	}
	else                            /* [3] field is contained in w4 */
	{
		w = w4 >> (36 - (bitoff+bitlen));
	}
	w = w & masks[bitlen-1];          /* Trim to proper size */
	return(w);
}

/* Get the "record type" from the tape block header.  Since it */
/* is stored in 2's complement form, negate it before returning */

getrecordtype(block)
char *block;
{
	long int tl;
	tl = getfield(block, WdoffRectype, BtoffRectype, BtlenRectype);
	return( (int) -tl);
}

doDatablock(block)
char *block;
{
	static char buf[(512*5)+1];         /* A page of characters */
	int ct;
	if (debug > 10) printf("*");
	if (fpFile == NULL) return;
					    /* 7 bit ascii only for now */
	if (numbytes > 512*5) ct = 512*5;
	else ct = numbytes;

	getstring(block, buf, 6, ct);
	buf[ct] = 0;
	fprintf(fpFile, "%s", buf);
	numbytes -= ct;
}

#define SecPerTick  (24.*60.*60.)/0777777
#define DayBaseDelta 0117213            /* Unix day 0 in Tenex format */

long
unixtime(block, wordoff)
char *block;
int  wordoff;
{
	long int t, s;

	t = getfield(block, wordoff, 0, 18);    /* First half is day */
	t -= DayBaseDelta;                      /* Switch to unix base */
						/* Now has # days since */
						/* Jan 1, 1970 */

	s = getfield(block, wordoff, 18, 18);   /* 2nd half is fraction day */
	s = s * SecPerTick;                     /* Turn into seconds */

	s += t*24*60*60;                        /* Add day base */
	return(s);
}

doSaveset(block, contflag)
char *block;
int  contflag;
{
	static char name[100];
	long t;

	if (debug > 10) printf("\nSaveset header:");
	getstring(block, name, WdoffSSName, sizeof(name));

	t = unixtime(block, WdoffSSDate);
	printf("Saveset '%s', %s\n", name, ctime(&t));

}

doFileHeader(block)
char *block;
{
	static char name[100];
	long t;                 /* The time in unix format */
	char *ts;

	if (debug > 5) printf("File Header block:\n");

	getstring(block, name, WdoffFLName, sizeof(name));
	ts = strchr(name, ';');          /* Chop off ;Pprotection;Aacct */
	*ts = 0;

	t = unixtime(block, WdoffFDB_Wrt);
	ts = ctime(&t) + 4;             /* Skip over day-name field */
	ts[strlen(ts)-1] = 0;             /* Chop off \n at end */

	bytesize = getfield(block, WdoffFDB_BSZ, BtoffFDB_BSZ, BtlenFDB_BSZ);
	numbytes = getfield(block, WdoffFDB_Size, BtoffFDB_Size, BtlenFDB_Size);
	pgcount  = getfield(block, WdoffFDB_PGC, BtoffFDB_PGC, BtlenFDB_PGC);

	printf("%6d %11d(%2d) %s %s\n",
		pgcount, numbytes, bytesize, ts, name);

	if (pattern && match(name, pattern))
	{
					      /* Special hack for bad files */
		if (bytesize != 7 && textflg)
		{
			if (bytesize == 0 || bytesize == 36)     /* Sigh */
			{       bytesize = 7;
				numbytes = numbytes * 5;
			}
		}
		if (bytesize != 7)
			fprintf(stderr, "Skipping -- binary file.\n");
		else
		{
		    fpFile = fopen(unixname(name), "w");
		    if (fpFile == NULL)
			    punt("Can't open %s for write!\n", unixname(name));
		    printf("Extracting\n");
		}
	}
	else
		fpFile = NULL;
}

doFileTrailer(block)
char *block;
{
	if (debug > 10) printf(" File trailer\n");
	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
}

doTapeTrailer(block)
char *block;
{
	if (debug > 10) printf("Tape Trailer");
}

punt(s, arg)
char *s;
int  arg;
{
	fprintf(stderr, s, arg);
	exit(1);
}


getstring(block, s, wordoff, max)
char *block;            /* Tape block */
char *s;                /* Destination string buffer */
int  wordoff;           /* 36-bit offset from start of tape block */
int  max;               /* Max number of characters to xfer into s */
{
	register int i;         /* Counter for five characters per word */
	int ct = 0;             /* Number of characters loaded so far */
	char *orig = s;         /* Save for debugging */

	while (ct < max)
	{
		for (i = 0; i < 5; i++)
		{
			*s = getfield(block, wordoff, i*7, 7);
			if (*s == 0) return;
			s++;
		}
		wordoff++;
		ct += 5;
	}
   /**     punt("String greater than %d characters.\n", max);   **/
}



/* See if pattern is in name (very simple) */
match(name, pattern)
char *name, *pattern;
{
	int  plen = strlen(pattern);

	while ((name=strchr(name, *pattern)))
	{
		if (strncmp(name, pattern, plen)==0) return(1);
		name++;
		if (*name == 0) return(0);         /* May not need */
	}
	return(0);
}

char *
unixname(name)
char *name;
{
	static char newname[200];
	static FILE *log = NULL;
	char *t;

	if (numflg)             /* If numeric filenames */
	{
		if (log == NULL) log = fopen(LOGFILE, "a");
		fprintf(log, "%d is %s\n", number, name);
		sprintf(newname, "%d", number++);
		return(newname);
	}

	name = strchr(name, '<');        /* Trim off device */
	t = strrchr(name, '>');          /* find end of directory */

	/* eventually make subdirectories */
	/* eventually optionally lowify filename */

	strcpy(newname, ++t);   /* Skip over the > */
	t = strrchr(newname, '.');       /* find last . */
	*t = 0;                         /* zap it out */
	return(newname);
}
