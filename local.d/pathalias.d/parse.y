%{
/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
static char	*sccsid = "@(#)parse.y	7.1 (down!honey) 85/08/06";
#endif lint

#include "def.h"

%}

%union {
	node	*y_node;
	Cost	y_cost;
	char	y_net;
	char	*y_name;
	struct {
		node *ys_node;
		Cost ys_cost;
		char ys_net;
		char ys_dir;
	} y_s;
}

%type <y_s> site
%type <y_node> links aliases plist network nlist nsite host
%type <y_cost> cost cexpr
%type <y_net> netchar

%token <y_node>	SITE HOST
%token <y_cost>	COST
%token <y_net>	NET
%token NL PRIVATE

%left	'+' '-'
%left	'*' '/'

%%
map	:	/* empty */
	|	map NL
	|	map links NL
	|	map aliases NL
	|	map network NL
	|	map private NL
	|	error NL
	;

host	:	HOST
	|	PRIVATE	{$$ = addnode("private");}
	;

private	:	PRIVATE '{' {Scanstate = PRIVATING;} plist {Scanstate = OTHER;} '}'
	;

plist	:	SITE		{$1->n_flag |= ISPRIVATE;}
	|	plist ',' SITE	{$3->n_flag |= ISPRIVATE;}
	|	plist ','	/* admit this benign error  */
	;

network	:	host '=' nlist cost
			{fixnet($1, $3, $4, DEFNET, DEFDIR);}
	|	host '=' netchar nlist cost
			{fixnet($1, $4, $5, $3, LRIGHT);}
	|	host '=' nlist netchar cost
			{fixnet($1, $3, $5, $4, LLEFT);}
	;

nlist 	:	'{' nsite '}'	{$$ = $2;}
	;

nsite	:	SITE
	|	nsite ',' SITE {
			/* be careful not to put anything on the list twice */
			if ($3->n_net == 0) {
				$3->n_net = $1;
				$$ = $3;
			}
		}
	|	nsite ','	/* admit this benign error */
	;
		
aliases	:	host '=' SITE		{alias($1, $3);}
	|	aliases ',' SITE	{alias($1, $3);}
	|	aliases ','	/* admit this benign error */
	;

links	:	host site cost {
			addlink($1, $2.ys_node, $3, $2.ys_net, $2.ys_dir);
			/*
			 * give a default path for the return link.
			 * this is wrong, but it's soothes the masses,
			 * who insist on putting error output in the
			 * output.  who said vox populi, vox Dei?
			 */
			addlink($2.ys_node, $1, INF, $2.ys_net, $2.ys_dir);
		}
	|	links ',' site cost {
			addlink($1, $3.ys_node, $4, $3.ys_net, $3.ys_dir);
			/* ditto */
			addlink($3.ys_node, $1, INF, $3.ys_net, $3.ys_dir);
		}
	|	links ','	/* admit this benign error */
	;

site	:	SITE	{
			$$.ys_node = $1;
			$$.ys_net = DEFNET;
			$$.ys_dir = DEFDIR;
		}
	|	netchar SITE	{
			$$.ys_node = $2;
			$$.ys_net = $1;
			$$.ys_dir = LRIGHT;
		}
	|	SITE netchar {
			$$.ys_node = $1;
			$$.ys_net = $2;
			$$.ys_dir = LLEFT;
		}
	;

cost	:	/* empty -- cost is always optional */
			{$$ = DEFCOST;}
	|	'(' {Scanstate = COSTING;} cexpr {Scanstate = OTHER;} ')'
			{$$ = $3;}
	;

cexpr	:	COST
	|	'(' cexpr ')'	{$$ = $2;}
	|	cexpr '+' cexpr	{$$ = $1 + $3;}
	|	cexpr '-' cexpr	{$$ = $1 - $3;}
	|	cexpr '*' cexpr	{$$ = $1 * $3;}
	|	cexpr '/' cexpr	{
			if ($3 == 0)
				yyerror("zero term in divison\n");
			else
				$$ = $1 / $3;
		}
	;

netchar	:	NET		/* normal network operator */
	|	NET NET {	/* for "domains" */
			if ($1 != $2)
				yyerror("invalid domain specifier\n");
			else
				$$=($1 | 0200);
		}
	;
%%

node	*revnetlist();

yyerror(s)
char *s;
{
	/* a concession to bsd error(1) */
	if (Cfile)
		fprintf(stderr, "\"%s\", ", Cfile);
	else
		fprintf(stderr, "%s: ", ProgName);
	fprintf(stderr, "line %d: %s\n", Lineno, s);
}

/*
 * patch in the costs of getting on/off the network.
 *
 * for each network member on netlist, add links:
 *	network -> member	cost = parameter;
 *	member -> network	cost = 0.
 * note that a network can have varying costs to its members, by suitable
 * multiple declarations.  this is a feechur.
 */
fixnet(netnode, netlist, cost, netchar, netdir)
register node	*netnode, *netlist;
Cost	cost;
char	netchar, netdir;
{
	register node	*nextnet;

	netnode->n_flag |= NNET;

	/*
	 * avoid quadratic behavior in addlink(), by reversing net list.
	 * this is cheap, and not necessarily effective, but in practice,
	 * it cuts the cost of addlink() by three.  can you believe that?!?
	 */
	netlist = revnetlist(netlist);

	/* now insert the links */
	for ( ; netlist; netlist = nextnet) {
		/* network -> member */
		(void) addlink(netnode, netlist, cost, netchar, netdir);

		/* member -> network */
		(void) addlink(netlist, netnode, (Cost) 0, netchar, netdir);
		nextnet = netlist->n_net;
		netlist->n_net = 0;	/* clear for later use */
	}
}

STATIC node	*
revnetlist(n)
node	*n;
{
	register node	*pred, *current, *succ;

	if ((pred = n) == 0 || (current = n->n_net) == 0)
		return(n);

	pred->n_net = 0;

	while (current) {
		succ = current->n_net;
		current->n_net = pred;
		pred = current;
		current = succ;
	}
	return(pred);
}
