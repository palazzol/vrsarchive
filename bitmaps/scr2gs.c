/*
 *	Convert saved EGA image to grey-scale format.
*/
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 350
#define XSCALE 3
#define YSCALE 5

/*
 *	This is the order the color palette index bits get read in.
*/
char scrbit[] = { 0x1, 0x4, 0x2, 0x8 };	/* Why me? */

/* This gets the EGA palette converted to greyscale */
int greyscale[16];

/*
 *	Extract intensity information about each primary color from an EGA
 *	Palette entry.
*/
#define red(color)		(!!(color&004)*2+ !!(color&040))
#define green(color)	(!!(color&002)*2+ !!(color&020))
#define blue(color)		(!!(color&001)*2+ !!(color&010))

/*
 *	Weight each pixel for overall brightness.  This is stolen from the
 *	EGA implementation of X windows for the PC-RT.
*/
#define brightness(color)	(39*red(color) + 50*green(color) + 11*blue(color))

char pixel[HEIGHT][WIDTH];

main(argc, argv)
char *argv[];
{	register int ch, i;
	register char *p;
	int plane, x, y;

	/*
	 *	Read the palette and compute greyscale information.
	*/
	for (i = 0; i < 16; i++) {
		ch = getchar();
		greyscale[i] = brightness(ch);
	}
	/*
	 *	Read the image, which consists of a bitmap per plane of color
	 *	select information.
	*/ 
	for (plane = 0; plane < 4; plane++) {
		p = &pixel[0][0];
		while (p < &pixel[HEIGHT-1][WIDTH]) {
			ch = getchar();
			if (ch == EOF) {
				fprintf(stderr, "choke\n");
				break;
			}
			for (i = 0x80; i; i /= 2) {
				if (ch & i)
					*p |= scrbit[plane];
				p++;	/* Next pixel */
			}
		}
	}
	/*
	 *	Plot the picture
	*/
	printf("%d %d\n", (WIDTH*2)/3, HEIGHT);
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0, p = &pixel[y][0]; x < WIDTH-2; x += 3, p++) {
			i = (greyscale[*p]+greyscale[*(p+1)]) / 2;
			p++;
			putchar((i*256) / 301);
			i = (greyscale[*p]+greyscale[*(p+1)]) / 2;
			p++;
			putchar((i*256) / 301);
		}
	}
	return(0);
}
