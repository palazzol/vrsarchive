#include <stdio.h>

#define BPB				8	/* Bits per byte */
#define ibm_width		(((width+BPB-1)/BPB)*BPB)
#define ibm_height		height

#define BM_BYTE(x,y,w)	((x)/BPB+(y)*(w/BPB))
#define BM_BITS(m,x,y,w) (((m)[BM_BYTE(x,y,w)]>>(x)%BPB) & ((1<<scale)-1))

#define obm_width		((((ibm_width/scale)+BPB-1)/BPB)*BPB)
#define obm_height		((ibm_height+scale-1)/scale)

int scale = 2;
unsigned char *obm_bits;

void
setnbit(x, y, w, b)	
int x, y, w, b;
{	int bits = x % BPB;

	obm_bits[BM_BYTE(x,y,w)] |= (b<<bits);
}

int
bitsin(byte)
register char byte;
{	register int bits = 0;

	while (byte) {
		bits++;
		byte &= byte-1;
	}
	return(bits);
}

main(argc, argv)
int argc;
char *argv[];
{	int width, height;
	int x, y, i, bitson, threshold;
	unsigned char *ibm_bits;

	/* Determine the scale factor */
	if ((argc != 2) || (atoi(argv[1]) <= 0)) {
		fprintf(stderr, "Usage: %s <factor>\n", argv[0]);
		exit(2);
	}
	scale = atoi(argv[1]);

	/* Read the input header */
	scanf("#define %*s %d\n", &width);
	scanf("#define %*s %d\n", &height);
	scanf("static char %*s = {\n");

	/* Write the output header */
	printf("#define obm_width %d\n", width/scale);
	printf("#define obm_height %d\n", height/scale);
	printf("static char obm_bits[] = {\n");

	/* Read the input bitmap */
	ibm_bits = (unsigned char *)malloc(((width+BPB-1)/BPB) * height);
	for (y = 0; y < ibm_height; y++) {
		for (x = 0; x < ibm_width; x += BPB) {
			scanf(" 0x%x %*[,}]", &i);
			ibm_bits[BM_BYTE(x,y,ibm_width)] = (unsigned char)i;
		}
	}

	/* Compute the output bitmap */
	obm_bits = (unsigned char *)calloc(obm_width*obm_height/BPB,
										sizeof(*obm_bits));
	bitson = 0;
	for (y = 0; y < ibm_height; y += scale) {
		for (x = 0; x < ibm_width; x += scale) {
			for (i = 0; i < scale; i++) {
				bitson += bitsin(BM_BITS(ibm_bits, x, y+i, ibm_width));
			}
		}
	}
	threshold = (scale*scale) / bitson;
	for (y = 0; y < ibm_height; y += scale) {
		for (x = 0; x < ibm_width; x += scale) {
			bitson = 0;
			for (i = 0; i < scale; i++) {
				bitson += bitsin(BM_BITS(ibm_bits, x, y+i, ibm_width));
			}
			setnbit(x/scale, y/scale, obm_width, (bitson>1));
		}
	}

	/* Write the output bitmap */
	for (y = 0; y < ibm_height/scale; y++) {
		for (x = 0; x < ibm_width/scale; x += BPB) {
			printf("0x%02x, ", obm_bits[BM_BYTE(x,y,obm_width)]);
			if ((BM_BYTE(x,y,obm_width)%12) == 11)
				printf("\n");
		}
	}
	printf("};\n");
	return(0);
}
