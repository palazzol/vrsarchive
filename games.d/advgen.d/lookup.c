#include	"advgen.h"
#include	"tokens.h"

typedef struct node
{
    struct node	*n_left;
    struct node	*n_right;
    symbol	n_sym;
}			node;
#define NNULL	(node *) NULL

/*  Initial node chosen about the middle of the reserved words */
node	sym_base = { NNULL, NNULL, { "Item", S_SYNTAX, ITEM, 0, 0 }};
node	*sym_tree = &sym_base;

static node *
newnode(s)
    char	*s;
{
#ifdef	lint
    node	*result = NNULL;
#else
    node   	*result = (node *) malloc(sizeof (node));
#endif

    if (result == NNULL)
    {
	fatal("out of memory for symbol table");
    }
    result->n_left = result->n_right = NNULL;
    result->n_sym.s_sym = strsave(s);
    result->n_sym.s_type = S_UNKNOWN;
    result->n_sym.s_value = result->n_sym.s_noun = result->n_sym.s_verb = 0;
    return result;
}

/*  Chris Torek suggested the speed improvements over the original
 *  version of this: check the first character and only do a strcmp()
 *  if it's equal (I'd planned to put that one in anyway), and replacement
 *  of the original tail-recursion by a goto (doing the work that a good
 *  optimiser should do for us, but usually doesn't).  The following
 *  recoding of looksym() is Chris's.
 *  The actual gain is surprisingly small: at most 10% as measured by
 *  recompiling the Pirate's adventure 20 times; the number of calls
 *  to strcmp and the subroutine call overhead are reduced considerably
 *  but processing is heavily dominated by yylook() and yyparse().
 */
static node *
looksym(s, n)
    char		*s;
    register node	*n;
{
    register int	cmp;

top:
    cmp = *s - *n->n_sym.s_sym;	/* fast check on first character */
    if (cmp == 0)		/* need full blown comparision */
    {
	cmp = strcmp(s, n->n_sym.s_sym);
    }
    if (cmp < 0)
    {
	if (n->n_left == NNULL)
	{
	    return n->n_left = newnode(s);
	}
	n = n->n_left;		/* recurse on left subtree */
	goto top;
    }
    else if (cmp > 0)
    {
	if (n->n_right == NNULL)
	{
	    return n->n_right = newnode(s);
	}
	n = n->n_right;		/* recurse on right subtree */
	goto top;
    }
    else
    {
	return n;
    }
}

symbol *
lookup(s)
    char	*s;
{
    return	&(looksym(s, sym_tree)->n_sym);
}

typedef struct keyword
{
    char	*k_word;
    int		k_token;
}			keyword;

/*  This is ordered to give reasonable balance, but it probably doesn't
 *  matter very much, since most lookups will find non-reserved words
 *  anyway. Just for fun, the layout shows the tree structure.
 */
static keyword	ktab[] =
{
    { "Item", ITEM },
	{ "Get", GET },
	    { "Die", DIE },
		{ "Daemon", DAEMON },
		    { "Action", ACTION },
			{ "Carrying", CARRYING },
		    { "Dark", DARK },
			{ "Darken", DARKEN },
		{ "Dropverb", DROPVERB },
		    { "Directions", DIRS },
			{ "Drop", DROP },
		    { "False", FFALSE },
			{ "Emptyhanded",EMPTYHANDED },
			{ "Fetch", FETCH },
	    { "Inlimbo", INLIMBO },
		{ "Goverb", GOVERB },
		    { "Getverb", GETVERB },
		    { "Greeting", GREETING },
			{ "Hellroom", HELL },
		{ "Inventory", INVENT },
		    { "Inroom", INROOM },
		    { "Inventorysize",INVENTORYSIZE },
			{ "Isdark", ISDARK },
	{ "Room", ROOM },
	    { "Moveto", MOVETO },
		{ "Lighten", LIGHTEN },
		    { "Lamplife", LAMPLIFE },
			{ "Lamp", LAMP },
			{ "Lampwarn", LAMPWARNING },
		    { "Location", LOC },
			{ "Message", MESSAGE },
		{ "Print", PRINT },
		    { "Nearto", NEARTO },
			{ "Noun", NOUN },
		    { "Quit", QUIT },
			{ "Refill", REFILL },
	    { "Var", VAR },
		{ "Score", SCORE },
		    { "Sameroom", SAMEROOM },
			{ "Say", SAY },
		    { "Treasury", TREASUREROOM },
			{ "Startroom", STARTROOM },
			{ "True", TTRUE },
		{ "Win", WIN },
		    { "Wordsize", WORDSIZE },
			{ "Verb", VERB },
			{ "Zap", ZAP },
    { CNULL, 0 }
};

void
init_syms()
{
    keyword	*k;
    symbol	*s;

    for (k = ktab; k->k_word != CNULL; k++)
    {
	s = lookup(k->k_word);
	s->s_type = S_SYNTAX;
	s->s_value = k->k_token;
    }
}

void
addsym(s, incr)
    char	*s;
    int		incr;
{
    symbol	*sym = lookup(s);

    if (sym->s_type == S_SYNTAX)
    {
	gramerror(FALSE, "%s is a reserved word", s);
	return;
    }
    addword(sym, incr);
}
