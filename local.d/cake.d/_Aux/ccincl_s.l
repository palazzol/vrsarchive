%{
/*
**	Scanner for ccincl
*/

static	char
rcs_id[] = "$Header: /home/Vince/cvs/local.d/cake.d/_Aux/ccincl_s.l,v 1.1 1987-11-16 19:36:06 vrs Exp $";

#undef	YYLMAX
#define	YYLMAX	512
%}

nl	[\n\f]
nonl	[^\n\f]

%%

#{nonl}*{nl}		{
				process(yytext);
				fflush(stdout);
			}

{nonl}*{nl}		;

%%

yywrap()
{
	return 1;
}
