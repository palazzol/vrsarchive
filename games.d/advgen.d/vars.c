#include	"advgen.h"

static int	nvars = 0;

void
decl_var(s)
    symbol	*s;
{
    switch (s->s_type)
    {
	case S_VAR:
	    muldecl(s->s_sym, "variable");
	    break;
	case S_UNKNOWN:
	    s->s_type = S_VAR;
	    s->s_value = nvars++;
	    break;
	default:
	    confdecl(s->s_sym, "variable", s->s_type);
	    break;
    }
}
