/*
 *	Convert saved EGA image to PPM format.
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

/* This gets the EGA palette */
int color[16];

/*
 *	Extract intensity information about each primary color from an EGA
 *	Palette entry.
*/
#define red(color)		(!!(color&004)*2+ !!(color&040))
#define green(color)	(!!(color&002)*2+ !!(color&020))
#define blue(color)		(!!(color&001)*2+ !!(color&010))

char pixel[HEIGHT][WIDTH];

main(argc, argv)
char *argv[];
{	register int ch, i;
	register char *p;
	int plane, x, y;

	/*
	 *	Read the palette and compute color information.
	*/
	for (i = 0; i < 16; i++) {
		color[i] = getchar();
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
	printf("P3\n");
	printf("%d %d 3\n", WIDTH, HEIGHT);
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0, p = &pixel[y][0]; x < WIDTH; x ++, p++) {
			ch = color[*p];
			printf("%d %d %d\n", red(ch), green(ch), blue(ch));
		}
	}
	return(0);
}
