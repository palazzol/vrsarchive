#ifndef lint
static char *sccsid = "@(#)mkobjs.c	1.1 11/18/84";
#endif

#include <stdio.h>

FILE *from, *to;
int line = 1;
int c;
char *unctrl();

main(argc, argv)
	int argc;
	char **argv;
{
	register i;
	char temp[100];

	if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
		exit();
	}
	sprintf(temp, "%.*s.c", sizeof temp - 3, argv[1]);
	if ((from = fopen(argv[1], "r")) == 0) {
		perror(argv[1]);
		exit(1);
	}
	if ((to = fopen(temp, "w")) == 0) {
		perror(temp);
		exit(1);
        }
	fprintf(to, "#include \"externs.h\"\n\n");
	fprintf(to, "struct objs %s[] = {\n", argv[1]);
	while ((c = getc(from)) != '\n' && c != EOF)
		;
	while ((i = getnum()) >= 0) {
		fprintf(to, "\t{ %d, ", i);
		if ((i = getnum()) < 0)
			error("Format error (character '%s').", unctrl(c));
		fprintf(to, "%d },\n", i);
	}
	fprintf(to, "\t0\n};\n");
	fclose(from);
	fclose(to);
	exit(0);
}

getnum()
{
	register i = 0;

	for (; c == '\n' || c == '\t' || c == ' '; c = getc(from))
		if (c == '\n')
			line++;
	for (; c >= '0' && c <= '9'; c = getc(from))
		i = i * 10 + c - '0';
	return c == EOF ? -1 : i;
}

error(fmt, a, b, c, d)
char *fmt;
{
	printf("line %d: ", line);
	printf(fmt, a, b, c, d);
	exit(1);
}

char *
unctrl(c)
	register c;
{
	static char buf[5];

	if (c == EOF)
		return "EOF";
	c = (unsigned char) c;
	if (c < ' ')
		sprintf(buf, "^%c", c + '@');
	else if (c < 0x7f)
		sprintf(buf, "%c", c);
	else if (c == 0x7f)
		return "^?";
	else sprintf(buf, "\\%03o", c);
	return buf;
}
