#include <stdio.h>

int swab = 0;

void
output(byte)
int byte;
{	static int count = 0;
	static unsigned bytes = 0x0;

	if (!swab) {
		printf("0x%02x, ", byte);
		return;
	}
	count++;
	bytes = (bytes<<8)|byte;
	if (count++ == 3) {
		printf("0x%02x, ", 0xFF&(bytes>>24));
		printf("0x%02x, ", 0xFF&(bytes>>16));
		printf("0x%02x, ", 0xFF&(bytes>> 8));
		printf("0x%02x, ", 0xFF&(bytes));
		count = 0;
		bytes = 0x0;
	}
}

main()
{	int inimage = 0;
	int height = 0;
	int width = 0;
	int c, byte, count;
	char buf[BUFSIZ];

	while (1) {
		gets(buf);
		if (strncmp(buf, "\r/height ", 8) == 0)
			height = atoi(buf+8);
		if (strncmp(buf, "\r/width ", 7) == 0)
			width = atoi(buf+7);
		if (strncmp(buf, "\rtheimage", 8) == 0)
			break;
		if (sscanf(buf, " %d %d %*d bitdump", &width, &height) == 2) {
			swab = 1;
			break;
		}
	}
	printf("#define bm_width %d\n", width);
	printf("#define bm_height %d\n", height);
	printf("static char bm_bits[] = {\n");
	count = 0;
	while (1) {
		do {
			c = getchar();
			if (c == EOF)
				break;
		} while (!isxdigit(c));
		byte = (c&0x0F) + (isalpha(c)*9);	/* Assumes ASCII */
		c = getchar();
		if (c == EOF)
			break;
		if (!isxdigit(c)) {
			gets(buf);
			printf("%c%s\n", c, buf);
			abort();
		}
		byte <<= 4;
		byte += (c&0x0F) + (isalpha(c)*9);	/* Assumes ASCII */
		output(byte);
		if ((++count % 12) == 0) {
			printf("\n");
		}
		if (!(count*8 % width)) {
			output(0xFF);
		}
		if (count*8 == width*height)
			break;
	}
	printf("};\n");
	return(0);
}
