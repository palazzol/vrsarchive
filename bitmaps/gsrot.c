/*
 * gsrot.c	- a program to rotate 256 level grayscale pictures.
 *			- The picture is rotated to be narrower than it is tall,
 *			  so it will fit onto a printed page.
*/
#include <malloc.h>
#include <stdio.h>

static char *theimage;

/* ARGSUSED */
main (argc, argv)
int argc;
char *argv[];
{
    int height, width, size;
    register int x, y;
	char *p;
    
    /* Get dimensions */
    (void) scanf("%d %d", &width, &height);
	(void) getchar();	/* Kill the newline */
	size = width * height;

	/* Read in the image */
	theimage = malloc(size);
	for (x = y = 0; x < size; x += y) {
		y = fread(theimage+x, 1, size, stdin);
		if (y <= 0) {
			perror("fread");
			return(1);
		}
	}

	/* Determine the correct orientation */
	if (width > height) {
		/* Output the image rotated */
		(void) printf("%d %d\n", height, width);
		for (x = 0; x < width; x++) {
			for (y = height; --y >= 0; ) {
				p = theimage + y*width + x;
				(void) putchar(*p);
			}
		}
		return(0);
	}

	/* Output the image unchanged */
	(void) printf("%d %d\n", width, height);
	if (fwrite(theimage, sizeof(*theimage), size, stdout) != size) {
		perror("fwrite");
		return(1);
	}
	return(0);
}
