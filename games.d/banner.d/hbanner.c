/*
 *	Hbanner: A program to print banners on 132-column
 *	line printers, using the Hershey fonts.  
 *
 *	Copyright (c) 1986 by Guy K. Hillyer.
 *	All rights reserved.  Permission is hereby
 *	granted for personal, non-commercial reproduction
 *	and use of this program, provided that this
 *	notice is included in any copy.
 */

#include <stdio.h>

#ifndef FONTDIR
#define FONTDIR "/local/lib/hfonts"
#endif

#define FONTSZ		96	/* number of characters in font */
#define FONTBEGIN	0x20	/* font begins at ascii space (' ') */
#define LINESZ		512	/* sizeof multi-purpose line buffer */
#define YORIGIN		64	/* center of y-axis */
#define MAXY	(YORIGIN*2)	/* y dimension of character bitmaps */
#define MAXX		64	/* x dimension of bitmaps */
#define CUSHION		20	/* space at beginning and end of message */
#define DISPCHAR	'O'	/* a lit pixel */

#define decode(x) ((x) - 'R')

char line[LINESZ];		/* multi-purpose line buffer */
char *font[FONTSZ];		/* table of character descriptions */
char bitmap[3][MAXY][MAXX/8+1];	/* holds previous, current, and next char */
int curr, prev, next;		/* indexes into bitmap[] */
int currwidth, prevwidth;	/* x-axis size of current and previous char */
int lwidth, rwidth;		/* displacement from 0 of L & R sides of char */
int xorigin, yorigin;		/* offset for zero-based graph */

/* Bitmaps exist for the previous, current, and next characters,
because some of the script fonts have characters that overlap adjacent 
character positions.  If the previous character overlapped to the 
right (such as 'l'), the current character bitmap may contain pieces 
of the previous character.  Likewise, The current character bitmap is 
not printed until the next character has been built, because the next 
character may be one of those (such as 'f') that overlap to the left. */

main (argc, argv)
int argc;
char *argv[];
{
	register int c;

	if (argc != 2) {
		fprintf (stderr, "Usage: %s <fontname>\n", argv[0]);
		pr_avail_fonts ();
		exit (-1);
	}

	read_font (argv[1]);

	init_bitmaps();

	while ((c = getchar ()) != EOF) {
		build_char (c);
		print_char (prev, prevwidth);
		rotate_bitmaps ();
	}
	print_char (prev, prevwidth);
	print_char (curr, CUSHION);
}

read_font (fontname)
char *fontname;
{
	FILE *fp;
	int i, dlen;
	char *malloc();

	/* assemble font file name */

	strcpy (line, FONTDIR);
	strcat (line, "/");
	strcat (line, fontname);
	strcat (line, ".fnt");

	if ((fp = fopen (line, "r")) == NULL) {
		fprintf (stderr, "Cannot open %s\n", line);
		perror ("");
		fprintf (stderr, "There is no font named %s.\n", fontname);
		pr_avail_fonts ();
		exit (-1);
	}

	for (i = 0; i < FONTSZ; i++) {
		if (fgets (line, LINESZ, fp) == NULL) {
			strcpy (line, FONTDIR);
			strcat (line, "/");
			strcat (line, fontname);
			strcat (line, ".fnt");
			fprintf (stderr,
			   "The font description file %s is corrupted.\n", line);
			exit (-1);
		}
		dlen = strlen (line);
		line[dlen-1] = '\0';		/* toss out the newline */
		if ((font[i] = malloc (dlen-5)) == NULL) {
			fprintf (stderr, "Oops.  Ran out of memory.\n");
			exit (-1);
		}
		strcpy (font[i], line+5);	/* +5 skips catalogue number */
	}

	fclose (fp);
}

build_char (c)
char c;
{
	char *dp;
	char nbuf[4];
	int npoints;
	int oldx, newx, oldy, newy;
	int skip, i, fchi;

	fchi = (c & 0x7f) - FONTBEGIN;
	if (fchi >= FONTSZ || fchi < 0)		/* character out of range */
		fchi = 0;			/* translate into space ' ' */

	dp = font[fchi];

	/* get number of points in this description */

	nbuf[0] = *dp++;
	nbuf[1] = *dp++;
	nbuf[2] = *dp++;
	nbuf[3] = '\0';
	npoints = atoi (nbuf);

	/* figure left & right horizontal displacement */

	lwidth = abs (decode (*dp++));
	rwidth = decode (*dp++);
	npoints--;

	currwidth = lwidth + rwidth;
	xorigin = lwidth;
	yorigin = YORIGIN;

	skip++;
	for (i = 0; i < npoints; i++) {

		if (*dp == ' ') {	/* "lift pen" */
			dp += 2;
			skip++;
			continue;
		}

		if (skip) {
			skip = 0;
			oldx = decode (*dp++);
			oldy = decode (*dp++);
			continue;
		}

		newx = decode (*dp++);
		newy = decode (*dp++);

		drawline (oldx, oldy, newx, newy);

		oldx = newx;
		oldy = newy;
	}
}


drawline(xfrom, yfrom, xto, yto)
int xfrom, yfrom, xto, yto;
{
	double m, b, dx, dy;
	int x, y;

	/*
	 *	Multiplying y values by two compensates for
	 *	the aspect ratio of a typical display device.
	 *	Adding the origin offsets the numbers so
	 *	that they can be used to index into the bitmap
	 *	array 
	 */

	yfrom *= 2;
	yto *= 2;
	xfrom += xorigin;
	xto += xorigin;
	yfrom += yorigin;
	yto += yorigin;

	dx = (double)(xto - xfrom);
	dy = (double)(yto - yfrom);

	if (abs((int)dx) >= abs((int)dy)) {	/* increment along x-axis */
		if (xfrom > xto) {		/* swap endpoints */
			x = xfrom;
			xfrom = xto;
			xto = x;
			y = yfrom;
			yfrom = yto;
			yto = y;
			dx = (double)(xto - xfrom);
			dy = (double)(yto - yfrom);
		}

		m = dy / dx;				/* slope */
		b = (double)yfrom - m * (double)xfrom;	/* y-intercept */

		for (x = xfrom; x < xto; x++) {
			y = round (m * x + b);
			plot (x, y);
		}
	} else {				/* increment along y-axis */
		if (yfrom > yto) {		/* swap endpoints */
			x = xfrom;
			xfrom = xto;
			xto = x;
			y = yfrom;
			yfrom = yto;
			yto = y;
			dx = (double)(xto - xfrom);
			dy = (double)(yto - yfrom);
		}

		m = dx / dy;
		b = (double)xfrom - m * (double)yfrom;	/* x-intercept */

		for (y = yfrom; y < yto; y++) {
			x = round (m * y + b);
			plot (x, y);
		}
	}
}

round (n)
double n;
{
	int i;

	i = (int)n;
	n -= (double)i;
	if (n > 0.5)
		i++;
	else if (n < -0.5)
		i--;
	return (i);
}

plot (x, y)
int x, y;
{
	int i;

	if (x < 0) {			/* overlap to left */
		x += prevwidth;
		i = prev;
	} else if (x >= currwidth) {	/* overlap to right */
		x -= currwidth;
		i = next;
	} else {			/* within range of current bitmap */
		i = curr;
	}
	bitmap[i][y][x>>3] |= (01 << (7 - (x&07)));
}

print_char (which, width)
int which, width;
{
	int x, y, xlimit, ylimit, lowest_y;

	xlimit = width;
	ylimit = yorigin * 2;

	for (x = 0; x < xlimit; x++) {
		/*
		 *	Look for the last lit pixel on the line.
		 *	This is here to limit the unnecessary printing
		 *	of spaces at the end of each line.
		 */

		for (y = 0; y < ylimit; y++) {
			if (bitmap[which][y][x>>3] & (01 << (7 - (x&07))))
				break;
		}
		lowest_y = y;

		for (y = ylimit-1; y >= lowest_y; y--) {
			if (bitmap[which][y][x>>3] & (01 << (7 - (x&07)))) {
				putchar (DISPCHAR);
			} else {
				putchar (' ');
			}
		}
		putchar ('\n');
	}
}

init_bitmaps ()
{
	prev = 0;
	curr = 1;
	next = 2;

	prevwidth = currwidth = CUSHION;
}

rotate_bitmaps ()
{
	int tmp;

	tmp = prev;
	prev = curr;
	curr = next;
	next = tmp;

	prevwidth = currwidth;

	clear_bitmap (next);
}

clear_bitmap (which)
int which;
{
	int x, y;

	for (y = 0; y < MAXY; y++)
		for (x = 0; x < MAXX/8+1; x++)
			bitmap[which][y][x] = '\0';
}

pr_avail_fonts ()
{
	fprintf (stderr, "Available fonts are:\n");
	sprintf (line,
	   "ls %s/*.fnt | sed -e \"s/^.*\\//	/\" -e \"s/.fnt$//\" 1>&2",
	   FONTDIR);
	system (line);
}
