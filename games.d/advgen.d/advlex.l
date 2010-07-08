	/*LINTLIBRARY*/
%{
#include	"advgen.h"
#include	"tokens.h"

#ifndef	NO_PRETTY_ERRORS
/* Redefinition of input depends on "lex" implementation; it is done to
 * allow decent error reporting.
 */
#undef	input
#define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):nxtchr())==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#endif

static void lexerror();
static int yywrap();
%}
%option lex-compat
WSP	[ \t\n]
SPACE	{WSP}+
COMMENT	\#.*\n
DIGIT	[0-9]
LETTER	[A-Za-z_$]
ALNUM	({DIGIT}|{LETTER})
IDENT	{LETTER}{ALNUM}*
NIDENT	("^"{ALNUM}+)
NUM	-?{DIGIT}+
STRING	(\'[^'\n]*\')|(\"[^"\n]*\")|(\`[^`\n]*\`)
%%
"&"		{ return AND; }
"=>"		{ return ARROW; }
"@"		{ return AT; }
"|"		{ return BAR; }
":"		{ return COLON; }
","		{ return COMMA; }
"--"		{ return DEC; }
"=="		{ return EQ; }
"="		{ return GETS; }
">"		{ return GT; }
"++"		{ return INC; }
"+="		{ return INCBY; }
"("		{ return LPAR; }
"!"		{ return NOT; }
"<->"		{ return SWAP; }
{NUM}		{ yylval.y_int = atoi(yytext); return NUM; }
"%"		{ return PERCENT; }
")"		{ return RPAR; }
";"		{ return SEP; }
"*"		{ return STAR; }
{IDENT}		{ yylval.y_symbol=lookup(yytext);
		  return (yylval.y_symbol)->s_type == S_SYNTAX ?
				(yylval.y_symbol)->s_value : WORD;
		}
{NIDENT}	{ yylval.y_symbol=lookup(yytext+1);
		  return (yylval.y_symbol)->s_type == S_SYNTAX ?
				(yylval.y_symbol)->s_value : WORD;
		}
{STRING}	{ yytext[yyleng-1] = '\0';
		  yylval.y_string = strsave(yytext+1);
		  return STRING;
		}
{SPACE}		;
{COMMENT}	;
.		{ lexerror(); }
%%
static int
yywrap()
{
	return 1;
}

static void
lexerror()
{
	int	c;

	gramerror(TRUE, "invalid character (%03o, '%c')",
			yytext[0], yytext[0], yylineno);
	add_error();
	while ((c = input()) != '\n' && c != EOF)
		;
}

#ifndef	NO_PRETTY_ERRORS
#	ifndef	BUFSIZ
#		define	BUFSIZ	1024
#	endif

char	linebuf[BUFSIZ+1];
char	*bufp = CNULL;

static int
nxtchr()
{
	int	c;

	if (bufp == CNULL || bufp == linebuf+BUFSIZ)
	{
		if (fgets(linebuf, BUFSIZ, yyin) == NULL)
		{
			return EOF;
		}
		bufp = linebuf;
	}
	if ((c = *bufp++) == '\n')
	{
		bufp = CNULL;
	}
	return c;
}
#endif

void
dumpline()
{
#ifndef	NO_PRETTY_ERRORS
	char	*cp = linebuf;
	char	*errp = linebuf;

	fprintf(stderr, linebuf);
	if (bufp == CNULL)
	{
		while (*errp)
			errp++;
	}
	else
	{
		errp = bufp;
	}
	errp -= yyleng;		/* Backup 1 token */
	if (errp < linebuf)
	{
		errp = linebuf;
	}
	while (cp < errp)
	{
		if (*cp == '\t')
		{
			(void) putc('\t', stderr);
		}
		else if (*cp >= ' ')
		{
			(void) putc(' ', stderr);
		}
		cp++;
	}
	fprintf(stderr, "^\n");
#endif
}

void
setinput(f)
    FILE	*f;
{
    yyin = f;
}
