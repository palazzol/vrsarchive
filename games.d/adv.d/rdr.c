/*
 * The line reader.  Reads in source lines, skips whitespace (if
 * any), converts inline $n's to newlines, $t's to tabs,
 * $[0-9A-Fa-f][0-9A-Fa-f]'s to their hex characters; then return
 * a pointer to the created line (linebuf).  Also, concatenates
 * lines ending in '$'.
 */

#include "adv.h"
#include <ctype.h>

/*
 * isxdigit is undocumented!
 */
#ifdef isxdigit
#define hex(c) isxdigit(c)
#else
#define	hex(c) ((c)>='0'&&(c)<='9'||(c)>='A'&&(c)<='F'||(c)>='a'&&(c)<='f')
#endif

char *
rdr()
{
	char lbuf[LINELEN+1], c;
	register char *lb, *bp = linebuf;
	register int t, q = 0;

	do {
		do {
			lb = lbuf;
			if (fgets(lbuf, sizeof lbuf, file) == NULL) {
				Wcleanup();
				printf("Premature EOF on input file!\n");
				exit(100);
			}
			skpwht(lb);
			t = strlen(lb);
		} while (t < 2);	/* fgets() keeps newline! */
		while (--t) {
			if (t > 1 && *lb == '$') {
				t--;
				++lb;
				if (*lb == 'n')
					c = '\n';
				else if (*lb == 't')
					c = '\t';
				else if (hex(*lb)) {
					c = todec(*lb++);
					if (t && hex(*lb)) {
						c <<= 4;
						c += todec(*lb);
						t--;
					}
				}
				else
					c = *lb;
			}
			else
				c = *lb;
			*bp++ = c;
			lb++;
			if (++q >= LINELEN && c) {
				Wcleanup();
				printf("Input line too long:\n%s\n", linebuf);
				exit(100);
			}
		}
		--bp;
	} while (*--lb == '$');
	*++bp = 0;
#ifdef DEBUG
	printf("Line: %s\n", linebuf);
#endif
	return (linebuf);
}

todec(c)
	register int c;
{

	if (c <= '9')
		return (c - '0');
	else if (c <= 'F')
		return (c - ('A' - 10));
	else
		return (c - ('a' - 10));
}
