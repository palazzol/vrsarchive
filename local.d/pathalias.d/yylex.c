#ifndef lint
static char	*sccsid = "@(#)yylex.c	7.1 (down!honey) 85/08/06";
#endif /*lint*/

#include "def.h"
#include "y.tab.h"

extern	YYSTYPE yylval;

#define LBRACE '{'
#define RBRACE '}'
#define LPAREN '('
#define RPAREN ')'
#define QUOTE '"'

Cost isacost();

int
yylex()
{
	int	c;
	Cost	cost;
	char	buf[BUFSIZ], errbuf[128];

tailrecursion:
	if (feof(stdin) && yywrap())
		return(EOF);

	if ((c = getchar()) == EOF)
		goto tailrecursion;

	while (c == ' ' || c == '\t')
		c = getchar();

	if (c == '\n') {
		Lineno++;
		c = getchar();
		if (c == ' ' || c == '\t')
			goto tailrecursion;
		ungetc(c, stdin);
		Scanstate = NEWLINE;
		return(NL);
	}

	if (c == '#') {
		while ((c = getchar()) != '\n')
			if (c == EOF)
				goto tailrecursion;
		ungetc(c, stdin);
		goto tailrecursion;
	}

	ungetc(c, stdin);

	switch(Scanstate) {
	case COSTING:
		if (isdigit(c)) {
			cost = 0;
			for (c = getchar(); isdigit(c); c = getchar())
				cost = (cost * 10) + c - '0';

			ungetc(c, stdin);
			yylval.y_cost = cost;
			return(COST);
		}

		
		if (getword(buf) == 0) {
			if ((yylval.y_cost = isacost(buf)) == 0) {
				sprintf(errbuf, "unknown cost (%s), using default", buf);
				yyerror(errbuf);
				yylval.y_cost = DEFCOST;
			}
			return(COST);
		}

		return(getchar());	/* can't be EOF */

	case NEWLINE:
		Scanstate = OTHER;
		if (getword(buf) != 0)
			return(getchar());	/* can't be EOF */
		if (c != '"' && strcmp(buf, "private") == 0)
			return(PRIVATE);

		yylval.y_node = addnode(buf);
		return(HOST);

	case PRIVATING:
		if (getword(buf) == 0) {
			yylval.y_node = addprivate(buf);
			return(SITE);
		}
		return(getchar());	/* can't be EOF */
	}

	if (getword(buf) == 0) {
		yylval.y_node = addnode(buf);
		return(SITE);
	}

	c = getchar();	/* can't be EOF */

	if (index(Netchars, c)) {
		yylval.y_net = c;
		return(NET);
	}

	return(c);
}

getword(str)
char	*str;
{
	int	c;

	c = getchar();
	if (c == QUOTE) {
		for ( ; (*str = getchar()) != '"'; str++) {
			if (*str == '\n') {
				yyerror("newline in quoted string\n");
				ungetc('\n', stdin);
				return(-1);
			}
		}
		*str = 0;
		return(0);
	}

	/* host name must start with alphanumeric or _ */
	if (!isalnum(c) && c != '_') {
		ungetc(c, stdin);
		return(-1);
	}

yymore:
	do {
		*str++ = c;
		c = getchar();
	} while (isalnum(c) || c == '.' || c == '_');

	if (c == '-' && Scanstate != COSTING)
		goto yymore;

	ungetc(c, stdin);
	*str = 0;
	return(0);
}

static struct ctable {
	char *cname;
	Cost cval;
} ctable[] = {
	/*
	 * this list is searched sequentially (with strcmps!).
	 * it is too long.  (they are ordered by frequency of
	 * appearance in a "typical" dataset.)
	 *
	 * adding a 0 cost token breaks isacost().  don't do it.
	 */
	{"DEMAND", 300},
	{"DAILY", 5000},
	{"DIRECT", 200},
	{"EVENING", 1800},
	{"LOCAL", 25},
	{"LOW", 5},	/* baud rate penalty */
	{"HOURLY", 500},
	{"POLLED", 5000},
	{"DEDICATED", 95},
	{"WEEKLY", 30000},
	{"DEAD", INF/2},
	{"HIGH", -5},	/* baud rate bonus */
	/* the remainder are reviled */
	{"ARPA", 100},
	{"DIALED", 300},
	{"A", 300},
	{"B", 500},
	{"C", 1800},
	{"D", 5000},
	{"E", 30000},
	{"F", INF/2},
	0
};

STATIC Cost
isacost(buf)
char	*buf;
{
	struct ctable	*ct;

	for (ct = ctable; ct->cname; ct++)
		if (strcmp(buf, ct->cname) == 0)
			return(ct->cval);

	return((Cost) 0);
}

yywrap()
{
	char	errbuf[100];

	fixprivate();	/* munge private host definitions */

	if (Ifiles == 0) 
		return(1);

	fclose(stdin);
	while (*Ifiles) {
		Lineno = 1;
		Fcnt++;
		if (fopen((Cfile = *Ifiles++), "r"))
			return(0);
		sprintf(errbuf, "%s: %s", ProgName, Cfile);
		perror(errbuf);
	}
	return(1);
}
