/*
** This file defines the star and planet images for vector type devices.
** All vector formed images are here (for tektronix, hp series and laserjet)
**
** Original images by Alan W. Paeth, dynamic 'star compilation' and
** cluster/galaxy images added by (pl) 15-Mar-87
*/

#include <stdio.h>
#define IMAROWS		20
#define IMACOLS		20		/* both should be more than plenty */
#define ENDCOORD	32767		/* should be out of offset range */

/**********************************************************************
**
** cstar:
**
** Parameters:
**	ptr		char pointer to star row, or NULL to compile
**
** Function:
**	'compiles' star image data for starlen from char pointers given
**	this method simplifies the creation and updating of images.
**	Image can consist of max IMAROWS of data, and the maximum
**	length/row is IMACOLS.  The lengths of rows MUST be equal !!
**
** Returns:
**	NULL when ptr != NULL, pointer to allocated vector defining
**	drawlen information and ending to ENDCOORD value when ptr == NULL
**
** History:
** 
*/
int *
cstar (ptr)
char *ptr;
{
	static int n;				/* number of lines gathered */
	static int len;				/* longest line found */
	static char image [IMAROWS][IMACOLS+1];	/* image data area */
	int cx, cy;				/* center points */
	int *retarea;
	int i,j, x, y, s, l;
	
	/*
 	** this is a two state machine - first it just gathers ptr strings
	** until NULL is given, then it 'compiles' the previous strings
	** and returns offset values for printing the star.
	*/

	if  (ptr != NULL) {			/* gather */
		strcpy (image [n++], ptr);	/* copy data */
		cx = strlen (ptr);		/* get max len */
		if  (cx > len)
			len = cx;
		return NULL;
	}
/*	else {					/* start 'compilation' */
	cy = n / 2;				/* align */
	cx = len / 2;

	/*
	** allocate offset area
	*/
	retarea = (int *) calloc 
		(len * n * (n / 2 + 1) * 3 + 1, sizeof (int));
	if  (retarea == NULL) {
		perror ("Memory allocation failed");
		exit (2);
	}
	/*
	** fill return area
	*/
	j = 0;
	for (i = 0; i < n; i++) {
		l = 0;  s = 0;  x = 0;
		ptr = image [i];
		while (*ptr) {
			if  (*ptr == ' ') {
				if  (l) { /* flush */
					retarea [j++] = s - cx;
					retarea [j++] = i - cy;
					retarea [j++] = l;
							
					l = 0;
				}
			}
			else {
				if  (l)
					l++;
				else {
					s = x;
					l = 1;
				}
			}
			ptr++;
			x++;
		} /* while */
		if  (l) { /* flush */
			retarea [j++] = s - cx;
			retarea [j++] = i - cy;
			retarea [j++] = l;
		}
	} /* for */
	n = 0;
	retarea [j] = ENDCOORD;
	return retarea;
} /* cstar */

drawPlan(x, y)
int x,y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
		(void) cstar (" *** ");
		(void) cstar ("*   *");
		(void) cstar ("*****");
		(void) cstar ("*   *");
		(void) cstar (" *** ");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}

drawStar (x, y, mag, type, color)
int x, y, mag;
char type, *color;
    {
    switch (mag)
	{
	case -1:
	case  0: draw0 (x, y); break;
	case  1: draw1 (x, y); break;
	case  2: draw2 (x, y); break;
	case  3: draw3 (x, y); break;
	case  4: draw4 (x, y); break;
	default: draw5 (x, y); break;
	}
    }

draw0(x, y)
int x, y;
    {
    static int def, *coord;
    int i;
    if  (! def)
	{
	def = 1;
   	(void) cstar (" ***** ");
	(void) cstar ("*******");
	(void) cstar ("*** ***");
	(void) cstar ("**   **");
	(void) cstar ("*** ***");
	(void) cstar ("*******");
	(void) cstar (" ***** ");
	coord = cstar (NULL);
        }
    i = 0;
    while (coord [i] != ENDCOORD)
        {
	drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
	i += 3;
	}
    }

draw1(x, y)
int x, y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
   		(void) cstar (" *** ");
		(void) cstar ("*****");
		(void) cstar ("*****");
		(void) cstar ("*****");
		(void) cstar (" *** ");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}

draw2(x, y)
int x, y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
   		(void) cstar ("  *  ");
		(void) cstar (" *** ");
		(void) cstar ("*****");
		(void) cstar (" *** ");
		(void) cstar ("  *  ");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}

draw3(x, y)
int x, y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
		(void) cstar ("***");
		(void) cstar ("***");
		(void) cstar ("***");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}

draw4(x, y)
int x, y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
		(void) cstar (" * ");
		(void) cstar ("***");
		(void) cstar (" * ");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}

draw5(x, y)
int x, y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
		(void) cstar ("*");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}

drawGalx(x, y)
int x, y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
   		(void) cstar (" *** ");
		(void) cstar ("*    ");
		(void) cstar (" *** ");
		(void) cstar ("    *");
		(void) cstar (" *** ");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}

drawClus(x, y)
int x, y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
   		(void) cstar (" * * ");
		(void) cstar ("* * *");
		(void) cstar (" * * ");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}

drawNebu(x, y)
int x, y;
{
	static int def, *coord;
	int i;

	if  (! def) {
		def = 1;
   		(void) cstar (" ** ");
		(void) cstar ("*  *");
		(void) cstar (" ** ");
		coord = cstar (NULL);
	}

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
}
