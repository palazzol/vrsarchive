#include	"advgen.h"

static room	rooms[MAXROOMS];
static int	nrooms;

static int newroom();

int
decl_room(sym, darkness, nbrs, desc, flag)
    symbol	*sym;
    bool	darkness;
    symbol	**nbrs;
    char	*desc;
    bool	flag;
{
    room	*r;
    int		i;
    
    switch (sym->s_type)
    {
	case S_ROOM:
	    if (flag && rooms[sym->s_value].r_declared)
	    {
		muldecl(sym->s_sym, "room");
	    }
	    break;
	case S_UNKNOWN:
	    sym->s_value = newroom(sym->s_sym);
	    sym->s_type = S_ROOM;
	    break;
	default:
	    confdecl(sym->s_sym, "room", sym->s_type);
	    break;
    }
    if (flag)
    {
	r = rooms+(sym->s_value);
	r->r_declared = 1;
	r->r_dark = darkness;
	r->r_desc = desc;
	for (i = 0; i < 6; i++)
	{
	    r->r_neighbours[i] = (nbrs && nbrs[i]) ?
			decl_room(nbrs[i], FALSE, SPNULL, CNULL, FALSE) : 0;
	}
    }
    return sym->s_value;
}

static int
newroom(s)
    char	*s;
{
    if (nrooms == MAXROOMS)
    {
	fatal("too many rooms");
    }
    rooms[nrooms].r_name = s;
    return nrooms++;
}

void
checkrooms()
{
    int		i;

    for (i=1; i<nrooms; i++)
    {
	if (!rooms[i].r_declared)
	{
	    gramerror(FALSE, "warning: undeclared room %s", rooms[i].r_name);
	}
    }
}

void
printrooms(f)
    FILE	*f;
{
    int		i, d;

    fprintf(f, "%d\n", nrooms);
    for (i = 0; i < nrooms; i++)
    {
	for (d = 0; d < 6; d++)
	{
	    if (rooms[i].r_neighbours[d])
	    {
		fprintf(f, "%c%d", "NSEWUD"[d], rooms[i].r_neighbours[d]);
	    }
	}
	if (rooms[i].r_dark)
	{
	    (void) putc('%', f);
	}
	fprintf(f, ":%s\n", rooms[i].r_desc);
    }
}

int
firstroom()
{
    return nrooms < 2 ? 0 : 1;
}
