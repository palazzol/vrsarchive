#ifndef lint
static char *sccsid = "@(#)mkfile.c	1.2 11/28/84";
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
	register room;
	char bol;
	register i, j;
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
	fprintf(to, "struct room %s[] = {\n", argv[1]);
	fprintf(to, "\t{ 0 },\n");
	while ((c = getc(from)) != '\n' && c != EOF)
		;
	for (room = 1; (i = getnum()) >= 0; room++) {
		if (c != ':')
			error("Format error (character '%s').", unctrl(c));
		if (room != i)
			error("Room number mismatch (%d should be %d).",
				i, room);
		while ((c = getc(from)) == ' ' || c == '\t')
			;
		fprintf(to, "\t{ \"");
		for (; c != '\n' && c != EOF; c = getc(from)) {
			if (c == '"')
				putc('\\', to);
			putc(c, to);
		}
		fprintf(to, "\",\n");
		if (c == EOF)
			error("Format error (character '%s').", unctrl(c));
		for (i = 0; i < 8 && (j = getnum()) >= 0; i++)
			fprintf(to, "%s%d", i ? ", " : "\t", j);
		fprintf(to, ",\n");
		if (j < 0)
			error("Format error (character '%s').", unctrl(c));
		for (; c == ' ' || c == '\t'; c = getc(from))
			;
		if (c != '\n')
			error("Format error (character '%s').", unctrl(c));
		bol = 0;
		putc('"', to);
		while ((c = getc(from)) != EOF && (c != '\n' || !bol)) {
			if (bol)
				fprintf(to, "\\n"), bol = 0;
			if (c == '\n') {
				line++;
				bol = 1;
			} else {
				if (c == '"')
					putc('\\', to);
				putc(c, to);
			}
		}
		fprintf(to, "\\n\" },\n");
	}
	fprintf(to, "};\n");
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
