#include	"advgen.h"

static int	nitems = 0;
static item	items[MAXITEMS];

static int newitem();

void
decl_item(sym, itemname, location, desc)
    symbol	*sym;
    symbol	*itemname;
    symbol	*location;
    char	*desc;
{
    item 	*i;

    switch (sym -> s_type)
    {
	case S_ITEM: 
	    muldecl (sym -> s_sym, "item");
	    break;
	case S_UNKNOWN: 
	    sym->s_value = newitem (sym -> s_sym);
	    break;
	default: 
	    confdecl (sym -> s_sym, "item", sym -> s_type);
	    break;
    }
    i = items + sym->s_value;
    sym->s_type = S_ITEM;
    if (itemname == SNULL)
    {
	i -> i_noun = 0;
    }
    else if (itemname->s_noun == 0)
    {
	gramerror(TRUE, "%s is not a noun", itemname->s_sym);
	add_error();
    }
    else
    {
	i->i_noun = itemname->s_noun;
    }
    i -> i_desc = desc;
    if (*desc == '*')
    {
	add_treasure();
    }
    if (location == SNULL)
    {
	i -> i_room = LIMBO;
    }
    else
    {
	switch (location -> s_type)
	{
	    case S_ROOM: 
		i -> i_room = location -> s_value;
		break;
	    default: 
		gramerror (TRUE, "%s is not a room", location -> s_sym);
		add_error ();
		break;
	}
    }
}

static int
newitem(s)
    char	*s;
{
    if (nitems == MAXITEMS)
    {
	fatal("too many items");
    }
    items[nitems].i_name = s;
    return nitems++;
}

void
printitems(f)
    FILE	*f;
{
    int		i;
    extern int	wsize;

    fprintf(f, "%d\n", nitems);
    for (i=0; i<nitems; i++)
    {
	fprintf(f, "%d", items[i].i_room);
	if (items[i].i_noun)
	{
	    fprintf(f, "=%-.*s", wsize, noun_name(items[i].i_noun));
	}
	fprintf(f, ":%s\n", items[i].i_desc);
    }
}
