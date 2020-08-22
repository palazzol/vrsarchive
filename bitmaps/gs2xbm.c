/*
 * gs2xbm.c    - a program to convert 256 level gray files to halftone
 *               pictures.  Produces output in format for display on X11
 *               servers.
 *
 * Written by: Steve Hawley   Aztec C V1.06H
 * Modified to produce language BITARRAY for imagen printers by
 *   Geoffrey Cooper at IMAGEN (August 12, 1986).  The main procedure
 *   is basically new.  The dither function is borrowed from Steve's
 *   code.
 * Modified to produce xbm format for X11 by Vincent R. Slyngstad 05/23/90.
 */
#include <stdio.h>

/* Magnification - must be a power of two (fix it if you don't like it) */
/* (and send me the fixes :-).  MAGSHFT is log base 2 of MAG */
#define MAG 4
#define MAGSHFT 2
unsigned char line[1000];

/*
 *	Swap the bits in a byte left for right
*/
#ifdef SLOWWAY
unsigned char revbyte[] = {
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
	0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
	0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
	0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
	0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
	0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
	0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
	0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
	0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
	0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
	0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
	0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
	0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
	0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
	0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
	0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
	0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF,
};
#define putbyte(b)	printf("0x%02x,", revbyte[b])
#else
/* Hex equivalents of a nibble read backward */
static char revhex[] = "084c2a6e195d3b7f";
/* Put out the hex nibbles in reverse order */
#define putbyte(b)	putchar('0'); putchar('x'); putchar(revhex[(b)&0x0F]); \
					putchar(revhex[(b)>>4]); putchar(',');
#endif

#define EXP
#ifdef EXP
static unsigned char luminance[64] = {
	254, 235, 217, 201, 186, 172, 159, 147, 
	136, 125, 116, 107,  99,  91,  84,  78, 
	 72,  66,  61,  57,  52,  48,  44,  41, 
	 38,  35,  32,  30,  27,  25,  23,  21, 
	 20,  18,  16,  15,  14,  13,  12,  11, 
	 10,   9,   8,   7,   7,   6,   6,   5, 
	  5,   4,   4,   3,   3,   3,   2,   2, 
	  2,   1,   1,   1,   1,   1,   1,   1
};
#else
static unsigned char luminance[64] = {
	254, 254, 254, 253, 252, 251, 249, 247,
	245, 242, 240, 236, 233, 230, 226, 222,
	217, 213, 208, 203, 198, 193, 187, 182,
	176, 170, 164, 158, 152, 146, 140, 134,
	127, 121, 115, 109, 103,  97,  91,  85,
	 79,  73,  68,  62,  57,  52,  47,  42,
	 38,  33,  29,  25,  22,  19,  15,  13,
	 10,   8,   6,   4,   3,   2,   1,   1,
};
#endif/*!EXP*/

#undef SPIRAL
#ifdef SPIRAL
static unsigned char grid[64] = {
   32,  31,  33,  30,  34,  29,  35,  28, 
   18,  46,  17,  47,  16,  48,  15,  36, 
   45,   8,  56,   7,  57,   6,  49,  27, 
   19,  55,   2,  62,   1,  58,  14,  37, 
   44,   9,  61,   0,  63,   5,  50,  26, 
   20,  54,   3,  60,   4,  59,  13,  38, 
   43,  10,  53,  11,  52,  12,  51,  25, 
   21,  42,  22,  41,  23,  40,  24,  39
};
#else
static unsigned char grid[64] = {
	 0, 32,  8, 40,  2, 34, 10, 42,
	48, 16, 56, 24, 50, 18, 58, 26,
	12, 44,  4, 36, 14, 46,  6, 38,
	60, 28, 52, 20, 62, 30, 54, 22,
	 3, 35, 11, 43,  1, 33,  9, 41,
	51, 19, 59, 27, 49, 17, 57, 25,
	15, 47,  7, 39, 13, 45,  5, 37,
	63, 31, 52, 23, 61, 29, 53, 21,
};
#endif

/*
 *	An exponential scale of gray levels is used, corresponding closer to
 *	the human eye's luminance response than a linear scale.
 * 
 * If you change the map, don't forget to make sure that 0 and 255 are
 * left out.  That way the whites and blacks can be solid.
 *
 */
#ifdef SLOWWAY
#define GRID(x, y)	grid[((y & 7) << 3) + (x & 7)] /* apply an 8x8 grid */
#else
#define GRID(x, y)	grid[(y<<3)+x] /* apply an 8x8 grid */
#endif
#define dither(x, y, level) (level < (int)luminance[GRID(x,y)])

main (argc, argv)
    char **argv;
{
    int height, width, wremainder;
    register unsigned long b;
    register int x, y, xs, ys;
	register int x1, mx, my;
    register int level;
	register int ocol = 0;
    FILE *fopen(), *fp;
    
    /* put up a new window */
    if ( (fp = fopen(argv[1], "r")) == NULL ) {
            perror("on file open");
            exit(1);
    }
    /* Get dimensions */
    fscanf(fp, "%d %d\n", &width, &height);
    wremainder = width & 31;
    width  &= ~31;
    height &= ~31;

    printf("#define %s_width %d\n", argv[1], width<<MAGSHFT);
    printf("#define %s_height %d\n", argv[1], height<<MAGSHFT);
    printf("static char %s_bits[] = {\n", argv[1]);
    b = 0;
    for ( y = 0; y < height; y++ ) {
		ys = (y << MAGSHFT) & 0x07; /* precompute for dither() */
        for ( x = 0; x < width; x++ )
            line[x] = getc(fp);
        for ( x = 0; x < wremainder; x++ ) getc(fp);
        for ( my = 0; my < MAG; my++ ) {
            for ( x = 0; x < width; x += (32>>MAGSHFT) ) {
                b = 0;
                for ( x1 = 0; x1 < (32>>MAGSHFT); x1++ ) {
					xs = ((x+x1) << MAGSHFT) & 0x07; /* for dither() */
					level = line[x+x1];
                    for ( mx = 0; mx < MAG; mx++ ) {
                        b = (b << 1) | dither(xs+mx, ys+my, level);
                    }
                }
                putbyte((b>>24)&0xFF);
                putbyte((b>>16)&0xFF);
                putbyte((b>>8 )&0xFF);
                putbyte((b    )&0xFF);
				ocol += 4;
				if (ocol > 11) {
					putchar('\n');
					ocol = 0;
				}
            }
        }
    }
    printf("};\n");
	return(0);
}
