/*	SC	A Spreadsheet Calculator
 *		Command and expression parser
 *
 *		original by James Gosling, September 1982
 *		modified by Mark Weiser and Bruce Israel,
 *			University of Maryland
 *
 * 		more mods Robert Bond 12/86
 *
 */



%{
#include "sc.h"
%}

%union {
    int ival;
    double fval;
    struct ent *ent;
    struct enode *enode;
    char *sval;
}

%type <ent> var
%type <enode> e term
%token <sval> STRING
%token <ival> NUMBER
%token <fval> FNUMBER
%token <sval> WORD
%token <ival> COL
%token S_FORMAT
%token S_LABEL
%token S_LEFTSTRING
%token S_RIGHTSTRING
%token S_GET
%token S_PUT
%token S_MERGE
%token S_LET
%token S_WRITE
%token S_TBL
%token S_PROGLET
%token S_COPY
%token S_SHOW
%token K_FIXED
%token K_SUM
%token K_PROD
%token K_AVE

%left '?' ':'
%left '|'
%left '&'
%nonassoc '<' '=' '>'
%left '+' '-'
%left '*' '/'

%%
command:	S_LET var '=' e	{ let ($2, $4); }
	|	S_LABEL var '=' STRING
				{ label ($2, $4, 0); }
	|	S_LEFTSTRING var '=' STRING
				{ label ($2, $4, -1); }
	|	S_RIGHTSTRING var '=' STRING
				{ label ($2, $4, 1); }
	|	S_FORMAT COL NUMBER NUMBER
				{ fwidth[$2] = $3;
				  FullUpdate++;
				  modflg++;
				  precision[$2] = $4; }
	|	S_GET STRING	{ readfile ($2,1); }
	|	S_MERGE STRING	{ readfile ($2,0); }
	|	S_PUT STRING	{ writefile ($2); }
	|	S_WRITE STRING	{ printfile ($2); }
	|	S_TBL STRING	{ tblprintfile ($2); }
	|       S_SHOW COL ':' COL  { showcol( $2, $4); }
	|       S_SHOW NUMBER ':' NUMBER  { showrow( $2, $4); }
	|	S_COPY var var ':' var
				{ copy($2, $3, $5); }
	|	/* nothing */
	|	error;

term: 		var		{ $$ = new ('v', $1); }
	|	K_FIXED term	{ $$ = new ('f', 0, $2); }
	|       '@' K_SUM '(' var ':' var ')' 
				{ $$ = new (O_REDUCE('+'), $4, $6); }
	|       '@' K_PROD '(' var ':' var ')' 
				{ $$ = new (O_REDUCE('*'), $4, $6); }
	|       '@' K_AVE '(' var ':' var ')' 
				{ $$ = new (O_REDUCE('a'), $4, $6); }
	|	'(' e ')'	{ $$ = $2; }
	|	'+' term	{ $$ = $2; }
	|	'-' term	{ $$ = new ('m', 0, $2); }
	|	NUMBER		{ $$ = new ('k', (double) $1); }
	|	FNUMBER		{ $$ = new ('k', $1); }
	|	'~' term	{ $$ = new ('~', 0, $2); }
	|	'!' term	{ $$ = new ('~', 0, $2); }
	;

e:		e '+' e		{ $$ = new ('+', $1, $3); }
	|	e '-' e		{ $$ = new ('-', $1, $3); }
	|	e '*' e		{ $$ = new ('*', $1, $3); }
	|	e '/' e		{ $$ = new ('/', $1, $3); }
	|	term
	|	e '?' e ':' e	{ $$ = new ('?', $1, new(':', $3, $5)); }
	|	e '<' e		{ $$ = new ('<', $1, $3); }
	|	e '=' e		{ $$ = new ('=', $1, $3); }
	|	e '>' e		{ $$ = new ('>', $1, $3); }
	|	e '&' e		{ $$ = new ('&', $1, $3); }
	|	e '|' e		{ $$ = new ('|', $1, $3); }
	|	e '<' '=' e	{ $$ = new ('~', 0, new ('>', $1, $4)); }
	|	e '!' '=' e	{ $$ = new ('~', 0, new ('=', $1, $4)); }
	|	e '>' '=' e	{ $$ = new ('~', 0, new ('<', $1, $4)); }
	;

var:		COL NUMBER	{ $$ = lookat($2 , $1); };
