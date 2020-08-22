%{
#include "addr.h"

static char	*errstr;
static char	*comstr;
static char	*cp;
static char	 errbuf[256];
static char	 combuf[256];
static int	 iseol;

Addr		*adrlist;
Addr		*errlist;
%}

%union {
   char	 yChar;
   char	*yString;
   Dom	*yDom;
   Addr	*yAddr;
}

%token	EOL ATOM LIT_DOMAIN QUOTED_STRING

%type	<yString>	word domain_ref sub_domain local_part phrase
%type	<yDom>		domain route_list route
%type	<yAddr>		addr_spec route_addr mailbox mbox_list group address

%start	addr_list

%%
addr_list:	addr_lel
       | addr_list addr_lel
       ;

addr_lel: address EOL {
	    $1->comment = comstr;
	    $1->error = errstr;
	    comstr = NULL;
	    errstr = NULL;
	    appAddr(&adrlist, $1);
	}
	| address ',' {
	    $1->comment = comstr;
	    $1->error = errstr;
	    comstr = NULL;
	    errstr = NULL;
	    appAddr(&adrlist, $1);
	}
	| error {
	    register Addr	*ap;

	    ap = newAddr();
	    Sprintf(errbuf, "after \"%s\", before \"%s\"\n", $<yString>1, cp);
	    errstr = newstring2(errstr, errbuf);
	    ap->error = errstr;
	    errstr = NULL;
	    comstr = NULL;
	    appAddr(&errlist, ap);
	}
	;

address: mailbox {
	    $$ = $1;
	}
	| group {
	    $$ = $1;
	}
	;

group	: phrase ':' mbox_list ';' {
	    register Addr	*a;

	    for (a = $3; a; a = a->next)
		a->group = $1;
	    $$ = $3;
	}
	;

mbox_list: mailbox {
	    $$ = $1;
	}
	| mbox_list ',' mailbox {
	    $3->comment = comstr;
	    $3->error = errstr;
	    comstr = NULL;
	    errstr = NULL;
	    appAddr(&($1), $3);
	    $$ = $1;
	}
	;

mailbox: addr_spec {
	    $$ = $1;
	}
	| route_addr {
	    $$ = $1;
	}
	| phrase route_addr {
	    $2->name = $1;
	    $$ = $2;
	}
	;

phrase	: word {
	    $$ = $1;
	}
	| phrase word {
	    $$ = newstring3($1, " ", $2);
	    free($1);
	    free($2);
       }
       ;

route_addr: '<' addr_spec '>' {
	   $$ = $2;
       }
       | '<' route addr_spec '>' {
	   prepDom(&($3->route), $2);
	   $$ = $3;
       }
       ;

route	: route_list ':' {
	    $$ = $1;
	}
	;

route_list: '@' domain {
	    $$ = $2;
	}
	| route_list ',' '@' domain {
	    appDom(&($1), $4);
	    $$ = $1;
	}
	;

addr_spec: local_part '@' domain {
	    register Addr	*ap;

	    $$ = ap = newAddr();
	    ap->localp = $1;
	    ap->destdom = $3;
	    ap->route = $3;
	}
	| local_part {
	    register Addr	*ap;

	    $$ = ap = newAddr();
	    ap->localp = $1;
	    ap->destdom = NULL;
	}
	;


local_part: word {
		$$ = $1;
	    }
	    | local_part '.' word {
		$$ = newstring3($1, ".", $3);
		free($1);
		free($3);
	    }
	    | local_part '%' word {
		$$ = newstring3($1, "%", $3);
		free($1);
		free($3);
	    }
	    ;

domain	: sub_domain {
	    register Dom	*dp;

	    dp = newDom();
	    dp->sub[0] = $1;
	    dp->top = dp->sub;
	    $$ = dp;
	}
	| domain '.' sub_domain {
	    ($1->top)++;
	    *($1->top) = $3;
	    $$ = $1;
	}
	;

sub_domain: domain_ref {
	    $$ = $1;
	}
	| LIT_DOMAIN {
	    $$ = yylval.yString;
	}
	;

domain_ref: ATOM {
	    $$ = yyval.yString;
	}
	;

word	: ATOM {
	    $$ = yylval.yString;
	}
	| QUOTED_STRING {
	    $$ = yylval.yString;
	}
	;

%%

#include <stdio.h>
#include <ctype.h>

#define ERROR	-2


static char *
newstring3(a, b, c)
    char	*a;
    char	*b;
    char	*c;
{
    char	*p;
    char	*q;
    int		 i;

    i = strlen(a) + strlen(b) + strlen(c) + 1;
    if ((p = malloc((MALLOCT)i)) == NULL) {
	perror("newstring3 (malloc)");
	exit(1);
    }
    q = p + strlen(strcpy(p, a));
    q += strlen(strcpy(q, b));
    Strcpy(q, c);
    return(q);
}


static char *
newstring2(a, b)
    char		*a;
    char		*b;
{
    char		*p;
    int			 i;

    i = strlen(a) + strlen(b) + 1;
    if ((p = malloc((MALLOCT)i)) == NULL) {
	perror("newstring2 (malloc)");
	exit(1);
    }
    Strcpy(p, a);
    Strcat(p, b);
    return(p);
}


static void
yyerror(s)
    char	*s;
{
   switch(yychar) {
       default:
	   Sprintf(errbuf, "%s: \"%c\" unexpected\n", s, yylval.yChar);
	   errstr = newstring2(errstr, errbuf);
	   break;
       case LIT_DOMAIN:
       case QUOTED_STRING:
       case ATOM:
	   Sprintf(errbuf, "%s: \"%s\" unexpected\n", s, yylval.yString);
	   errstr = newstring2(errstr, errbuf);
	   break;
       case 0: /* EOF */
	   Sprintf(errbuf, "%s: unexpected EOF\n", s);
	   errstr = newstring2(errstr, errbuf);
	   break;
   }

}


parseit(line)
    char	*line;
{
    cp = line;
    adrlist = NULL;
    errlist = NULL;
    (void)yyparse();
}


yylex()
{
    register char	*p;
    register int	 paren_count;

    while (isascii(*cp) && (isspace(*cp) || (*cp == '('))) {
	if (*cp == '(') {
	    for (paren_count = 1, p = cp + 1; paren_count; p++) {
		if (*p == '\0')
		    return(EOF);
		if (*p == '(')
		    paren_count++;
		else if( *p == ')')
		    paren_count--;
	    }
	    Strncpy(combuf, cp + 1, (p - 2) - cp);
	    if (comstr == NULL) {
		if ((comstr = malloc((MALLOCT)(strlen(combuf) + 1))) == NULL) {
		    perror("malloc failure");
		    exit(1);
		}
		Strcpy(comstr, combuf);
	    }
	    else
		comstr = newstring3(comstr, ", ", combuf);
	    cp = p;
	}
	else
	    cp++;
    }

    if (!isascii(*cp))
	return(ERROR);

    switch (*cp) {
	case '\0':
	    if (iseol) {
		iseol = 0;
		return(EOF);
	    }
	    iseol = 1;
	    return(EOL);
	case ',':
	case ':':
	case ';':
	case '.':
	case '@':
	case '%':
	case '<':
	case '>':
	    yylval.yChar = *cp;
	    return(*cp++);
	case '[':       /* LIT_DOMAIN */
	    for (p = cp; *++p && *p != ']'; )
		;
	    if (*p == '\0')
		return(EOF);
	    if ((yylval.yString = malloc((MALLOCT)(p - cp + 2))) == NULL) {
		fprintf(stderr,"not enough memory\n");
		return(ERROR);
	    }
	    Strncpy(yylval.yString, cp, p - cp + 1);
	    yylval.yString[p - cp + 2] = '\0';
	    cp = ++p;
	    return(LIT_DOMAIN);
	case '"':       /* QUOTED_STRING */
	    for (p = cp; *++p && *p != '"'; )
		;
	    if (*p == '\0')
		return(EOF);
	    if ((yylval.yString = malloc((MALLOCT)(p - cp + 2))) == NULL) {
		fprintf(stderr,"not enough memory\n");
		return(ERROR);
	    }
	    Strncpy(yylval.yString, cp, p - cp + 1);
	    yylval.yString[p - cp + 2] = '\0';
	    cp = ++p;
	    return(QUOTED_STRING);
    }
    for (p = cp; ; p++)
	switch (*p) {
	    case 'a':
	    case 'A':
		if ((p[1] == 't' || p[1] == 'T') && isspace(p[2])) {
		    yylval.yChar = '@';
		    cp = p + 2;
		    return('@');
		}
		break;
	    case ',':
	    case ':':
	    case ';' :
	    case '.':
	    case '@':
	    case '%':
	    case '<':
	    case '>':
	    case '(':
	    case ')':
	    case '[':
	    case '"':
	    case '\0':
		goto out;
	    default:
		if (isspace(*p))
		    goto out;
	}
out:
    if ((yylval.yString = malloc((MALLOCT)(p - cp + 1))) == NULL) {
	fprintf(stderr,"not enough memory\n");
	return(ERROR);
    }
    Strncpy(yylval.yString, cp, p - cp);
    yylval.yString[p - cp] = '\0';
    cp = p;
    return(ATOM);
}
