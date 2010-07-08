#ifndef lint
static char *sccsid = "@(#)mkwlist.c	1.1 11/18/84";
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
	register i, j;

	if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
		exit(1);
	}
	if ((from = fopen(argv[1], "r")) == 0) {
		perror(argv[1]);
		exit(1);
	}
	if ((to = fopen("wlist.c", "w")) == 0) {
		perror("wlist.c");
		exit(1);
        }
	fprintf(to, "#include \"externs.h\"\n\n");
	fprintf(to, "struct wlist wlist[] = {\n");
	while ((c = getc(from)) != '\n' && c != EOF)
		;
	if (c == '\n')
		c = getc(from);
	while (c != EOF) {
		for (; c == ' ' || c == '\t' || c == '\n'; c = getc(from))
			if (c == '\n')
				line++;
		if (c == EOF)
			break;
		fprintf(to, "\t{ \"");
		for (; c != ' ' && c != '\t' && c != '\n' && c != EOF; c = getc(from))
			putc(c, to);
		putc('"', to);
		if (c == EOF)
			error("Format error (character '%s').", unctrl(c));
		for (i = 0; i < 2 && (j = getnum()) >= 0; i++)
			fprintf(to, ", %d", j);
		fprintf(to, " },\n");
		if (j < 0)
			error("Format error (character '%s').", unctrl(c));
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
