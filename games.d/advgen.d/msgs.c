#include	"advgen.h"

static int	nmsgs = 1;
static char	*msgs[MAXMSGS] = { "Enjoy your adventure!" };

void
decl_msg(s, m)
    symbol	*s;
    char	*m;
{
    switch (s->s_type)
    {
	case S_MSG:
	    muldecl(s->s_sym, "message");
	    break;
	case S_UNKNOWN:
	    if (nmsgs == MAXMSGS)
	    {
		fatal("too many messages");
	    }
	    msgs[nmsgs] = m;
	    s->s_type = S_MSG;
	    s->s_value = nmsgs++;
	    break;
	default:
	    confdecl(s->s_sym, "message", s->s_type);
	    break;
    }
}

symbol *
anon_msg(m)
    char	*m;
{
    static symbol	s;
    int			i;

    for (i=0; i<nmsgs; i++)
    {
	if (streq(m, msgs[i]))
	{
	    s.s_value = i;
	    return &s;
	}
    }
    if (nmsgs == MAXMSGS)
    {
	fatal("too many messages");
    }
    msgs[nmsgs] = m;
    s.s_value = nmsgs++;
    s.s_type = S_MSG;
    return &s;
}

void
greetmsg(m)
    char	*m;
{
    msgs[0] = m;
}

void
printmsgs(f)
    FILE	*f;
{
    int		i;

    fprintf(f, "%d\n", nmsgs);
    for (i=0; i<nmsgs; i++)
    {
	fprintf(f, "%s\n", msgs[i]);
    }
}
