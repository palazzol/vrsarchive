%{
#include	"advgen.h"
#include	"code.h"

#define		percent(n) ((int) ((n)*(255.0/100.0)))

static symbol	*nbrs[6];
static int	lastnoun, lastverb;
%}

%union
{
	char	*y_string;
	int	y_int;
	bool	y_bool;
	symbol	*y_symbol;
	symbol	**y_nbrs;
	int	y_void;
}

%type	<y_nbrs>	neighbours
%type	<y_int>		cond relation number
%type	<y_bool>	darkness
%type	<y_symbol>	itemname itemroom actword description

%token	ACTION
	AND
	ARROW
	AT
	BAR
	COLON
	COMMA
	DAEMON
	DARK
	DARKEN
	DEC
	DROPVERB
	EQ
	GETS
	GETVERB
	GOVERB
	GREETING
	GT
	HELL
	INC
	INCBY
	INVALID
	INVENTORYSIZE
	ITEM
	LAMPLIFE
	LAMPWARNING
	LAMP
	LPAR
	MESSAGE
	NOT
	NOUN
	PERCENT
	ROOM
	RPAR
	SEP
	STAR
	STARTROOM
	TREASUREROOM
	VAR
	VERB
	SWAP
	WORDSIZE
%token <y_int>
	FFALSE
	NUM
	TTRUE
%token <y_symbol>
	CARRYING
	DIE
	DIRS
	DROP
	EMPTYHANDED
	FETCH
	GET
	INROOM
	INVENT
	ISDARK
	LIGHTEN
	INLIMBO
	LOC
	MOVETO
	NEARTO
	PRINT
	QUIT
	REFILL
	SAMEROOM
	SAY
	SCORE
	WIN
	WORD
	ZAP
%token <y_string>
	STRING

%start	game
%%
game
	: specials
		{ init_nouns(); }
		nouns
		{ init_verbs(); }
		verbs
		vars
		rooms
		items
		messages
		daemons
		{ enddaemons(); }
		rules
	;

specials
	: /* Empty */
	| special specials
	;

special
	: HELL WORD SEP
			{ set_hell($2->s_sym); }
	| DIRS WORD WORD WORD WORD WORD WORD SEP
			{ set_dirs($2->s_sym, $3->s_sym, $4->s_sym,
					$5->s_sym, $6->s_sym, $7->s_sym);
			}
	| LAMP WORD SEP
			{ set_lamp($2->s_sym); }
	| WORDSIZE number SEP
			{ set_wsize($2); }
	| STARTROOM WORD SEP
			{ set_start($2->s_sym); }
	| TREASUREROOM WORD SEP
			{ set_treasury($2->s_sym); }
	| INVENTORYSIZE number SEP
			{ set_inventory($2); }
	| LAMPLIFE number SEP
			{ set_llife($2); }
	| LAMPWARNING number SEP
			{ set_lwarn($2); }
	| GREETING STRING SEP
			{ set_greeting($2); }
	| GOVERB { set_go_list(); } verbdef SEP
	| GETVERB { set_get_list(); } verbdef SEP
	| DROPVERB { set_drop_list(); } verbdef SEP
	| error SEP
			{ yyerrok; }
	;

verbs
	: VERB verblist SEP
	| VERB error SEP
			{ yyerrok; }
	| /* Empty */
	;

verblist
	: verbdef
	| verblist verbdef
	;

verbdef
	: WORD
			{ addword($1, 1); }
	| WORD GETS
		{ addword($1, 1); }
		synonyms
	;
nouns
	: NOUN objlist SEP
	| NOUN error SEP
			{ yyerrok; }
	| /* Empty */
	;

objlist
	: objdef
	| objlist objdef
	;

objdef
	: WORD
			{ addword($1, 1); }
	| WORD GETS
		{ addword($1, 1); }
		synonyms
	;

synonyms
	: WORD
			{ addword($1, 0); }
	| WORD GETS
		{ addword($1, 0); }
		synonyms
	;

items
	: ITEM itemlist
	| /* Empty */
	;

itemlist
	: item
	| itemlist item
	| itemlist error SEP
			{ yyerrok; }
	;
item
	: WORD itemname itemroom STRING SEP
			{ decl_item($1, $2, $3, $4); }
	;

itemname
	: GETS WORD
			{ $$ = $2; }
	| /* Empty */
			{ $$ = SNULL; }
	;

itemroom
	: AT WORD
			{ $$ = $2; }
	| /* Empty */
			{ $$ = SNULL; }
	;

description
	: WORD
			{ $$ = $1; }
	| STRING
			{ $$ = anon_msg($1); }
	;

rooms
	: ROOM roomlist
			{ checkrooms(); }
	| /* Empty */
	;

roomlist
	: room
	| roomlist room
	| roomlist error SEP
			{ yyerrok; }
	;

room
	: WORD darkness neighbours STRING SEP
			{ (void) decl_room($1,$2,$3,$4,TRUE); clear_nbrs(); }
	;

darkness
	: DARK
			{ $$ = TRUE; }
	| /* Empty */
			{ $$ = FALSE; }
	;

neighbours
	: LPAR nlist RPAR
			{ $$ = nbrs; }
	| /* Empty */
			{ $$ = SPNULL; }
	;

nlist
	: neighbr
	| nlist neighbr
	;

neighbr
	: WORD COLON WORD
			{
				switch (($1->s_sym)[0])
				{
				case 'N': case 'n':
					dupdir(0);
					nbrs[0] = $3; break;
				case 'S': case 's':
					dupdir(1);
					nbrs[1] = $3; break;
				case 'E': case 'e':
					dupdir(2);
					nbrs[2] = $3; break;
				case 'W': case 'w':
					dupdir(3);
					nbrs[3] = $3; break;
				case 'U': case 'u':
					dupdir(4);
					nbrs[4] = $3; break;
				case 'D': case 'd':
					dupdir(5);
					nbrs[5] = $3; break;
				default:
					gramerror(TRUE,
						"unknown direction - %s",
						$1->s_sym);
					add_error();
					break;
				}
			}
	;

messages
	: MESSAGE messagelist
	| /* Empty */
	;

messagelist
	: message
	| messagelist message
	| messagelist error SEP { yyerrok; }
	;

message
	: WORD STRING SEP
			{ decl_msg($1, $2); }
	;

daemons
	: DAEMON daemonlist
	| /* Empty */
	;

daemonlist
	: daemon
	| daemonlist daemon
	| daemonlist error SEP { yyerrok; }
	;

daemon
	: chance production SEP
			{ endrule(); }
	;

chance
	: /* Empty */
			{ codebyte(C_ALWAYS); }
	| number PERCENT
			{ codebyte(percent($1)); }
	;

rules
	: ACTION rulelist
	| /* Empty */
	;

rulelist
	: rule
	| rulelist rule
	| rulelist error SEP { yyerrok; }
	;

rule
	: utterance alternates SEP
	;

alternates
	: production
			{ endrule(); }
	| alternates BAR
		{ codebyte(lastverb); codebyte(lastnoun); }
		production
			{ endrule(); }
	;

utterance
	: actword actword COLON
			{
			    codebyte(lastverb = findverb($1, TRUE));
			    codebyte(lastnoun = findnoun($2, TRUE));
			}
	| actword COLON
			{
			    lastverb = findverb($1, TRUE);
			    if (lastverb != NOTFOUND)
			    {
				codebyte(lastverb);
				codebyte(lastnoun = C_ALWAYS);
			    }
			    else
			    {
				codebyte(lastverb = C_ALWAYS);
				codebyte(lastnoun = findnoun($1, TRUE));
			    }
			}
	;

actword
	: WORD
			{ $$ = $1; }
	| STAR
			{ $$ = SNULL; }
	;

production
	: conditions ARROW
		{ endcond(); }
		actions
	| conditions
			{ endcond(); }
	| ARROW
		{ endcond(); }
		actions
	| /* Empty */
			{ endcond(); }
	;

conditions
	: condition
	| conditions AND condition
	;

actions
	: action
	| actions COMMA action
	;

condition
	: cond
			{ }
	| NOT cond
			{ negate($2); }
	;

cond
	: EMPTYHANDED
			{ codebyte(C_EMPTYHANDED); $$ = 1; }
	| ISDARK
			{ codebyte(C_ISDARK); $$ = 1; }
	| WORD relation number
			{ $$ = codereln($2, $1, $3); }
	| WORD
			{ $$ = codereln(R_GT, $1, 0); }
	| CARRYING LPAR WORD RPAR
			{
			    codebyte(C_CARRYING);
			    codebyte(typecheck($3, S_ITEM));
			    $$ = 2;
			}
	| SAMEROOM LPAR WORD RPAR
			{
			    codebyte(C_SAMEROOM);
			    codebyte(typecheck($3, S_ITEM));
			    $$ = 2;
			}
	| NEARTO LPAR WORD RPAR
			{
			    codebyte(C_NEARTO);
			    codebyte(typecheck($3, S_ITEM));
			    $$ = 2;
			}
	| INROOM LPAR WORD RPAR
			{
			    codebyte(C_INROOM);
			    codebyte(typecheck($3, S_ROOM));
			    $$ = 2;
			}
	| INLIMBO LPAR WORD RPAR
			{
			    codebyte(C_INLIMBO);
			    codebyte(typecheck($3, S_ITEM));
			    $$ = 2;
			}
	| LOC LPAR WORD RPAR EQ WORD
			{
			    codebyte(C_LOCATION);
			    codebyte(typecheck($3, S_ITEM));
			    codebyte(typecheck($6, S_ROOM));
			    $$ = 3;
			}
	| LPAR cond RPAR
			{ $$ = $2; }
	;

action
	: WIN
			{ codebyte(A_WIN); }
	| DIE
			{ codebyte(A_DIE); }
	| QUIT
			{ codebyte(A_QUIT); }
	| INVENT
			{ codebyte(A_INVENT); }
	| DARKEN
			{ codebyte(A_DARKEN); }
	| LIGHTEN
			{ codebyte(A_LIGHTEN); }
	| REFILL
			{ codebyte(A_REFILL); }
	| SCORE
			{ codebyte(A_SCORE); }
	| WORD GETS number
			{ codeassign($1, $3); }
	| INC WORD
			{ codeinc($2, I_INCR); }
	| DEC WORD
			{ codeinc($2, I_DECR); }
	| PRINT LPAR WORD RPAR
			{ codeprint($3); }
	| SAY description
			{ codebyte(A_SAY); codebyte(typecheck($2, S_MSG)); }
	| description
			{ codebyte(A_SAY); codebyte(typecheck($1, S_MSG)); }
	| WORD INCBY number
			{ codeaugment($1, $3); }
	| MOVETO LPAR WORD RPAR
			{ codebyte(A_MOVETO); codebyte(typecheck($3,S_ROOM));}
	| GET LPAR WORD RPAR
			{ codebyte(A_GET); codebyte(typecheck($3, S_ITEM)); }
	| DROP LPAR WORD RPAR
			{ codebyte(A_DROP); codebyte(typecheck($3, S_ITEM)); }
	| ZAP LPAR WORD RPAR
			{ codebyte(A_ZAP); codebyte(typecheck($3, S_ITEM)); }
	| FETCH LPAR WORD RPAR
			{ codebyte(A_FETCH); codebyte(typecheck($3,S_ITEM)); }
	| LOC LPAR WORD RPAR GETS WORD
			{
			    codebyte(A_LOCATION);
			    codebyte(typecheck($3, S_ITEM));
			    codebyte(typecheck($6, S_ROOM));
			}
	| WORD SWAP WORD
			{
			    codebyte(A_SWAP);
			    codebyte(typecheck($1, S_ITEM));
			    codebyte(typecheck($3, S_ITEM));
			}
	;

vars
	: VAR varlist SEP
	| VAR error SEP
			{ yyerrok; }
	| /* Empty */
	;

varlist
	: WORD
			{ decl_var($1); }
	| varlist WORD
			{ decl_var($2); }
	;

relation
	: EQ
			{ $$ = R_EQ; }
	| GT
			{ $$ = R_GT; }
	;

number
	: NUM
			{ $$ = $1; }
	| FFALSE
			{ $$ = 0; }
	| TTRUE
			{ $$ = 1; }
	;
%%
static int	parseerrors = 0;

static void
yyerror(s)
    char	*s;
{
	gramerror(TRUE, s);
	add_error();
}

void
add_error()
{
	if (parseerrors++ > MAXPARSEERRORS)
	{
	    fatal("Too many errors\n");
	}
}

static void
dupdir(n)
	int	n;
{
	if (nbrs[n] != SNULL)
	{
		gramerror(TRUE, "warning - duplicate direction '%c'",
				"NSEWUD"[n]);
	}
}

static void
clear_nbrs()
{
	int	i;

	for (i=0; i<6; i++)
	{
		nbrs[i] = SNULL;
	}
}

bool
parse(f)
    FILE	*f;
{
    setinput(f);
    if (yyparse() != 0)
    {
	add_error();
    }
    checkspecials();
    return parseerrors == 0;
}
