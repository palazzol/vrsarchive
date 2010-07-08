#include	"advgen.h"
#include	"code.h"

static char	code[MAXCODE];
static char	*code_ptr = code;
static char	*act_ptr;
static bool	okundeclvars = FALSE;

enum { indaemons, inactions } phase = indaemons;

static int	ndaemons = 0;
static int	nactions = 0;

void
codebyte(b)
    int		b;
{
    if (code_ptr == &(code[MAXCODE]))
    {
	fatal("too much code for daemons and actions");
    }
    *code_ptr++ = (char) b;
}

static int variable();

int
codereln(r, s, n)
    int		r;
    symbol	*s;
    int		n;
{
    int		v = variable(s);

    if (v == 0)
    {
	switch (n)
	{
	    case 0:
		codebyte(C_V0ZERO + r);
		return 1;
	    default:
		codebyte(C_V0TEST + r);
		codebyte(n);
		return 2;
	}
    }
    else
    {
	switch (n)
	{
	    case 0:
		codebyte(C_VnZERO+ r);
		codebyte(v);
		return 2;
	    default:
		codebyte(C_VnTEST + r);
		codebyte(v);
		codebyte(n);
		return 3;
	}
    }
}

void
codeassign(s, n)
    symbol	*s;
    int		n;
{
    int		v = variable(s);

    if (v == 0)
    {
	switch (n)
	{
	    case 0:
		codebyte(A_V0ZERO);
		break;
	    case 1:
		codebyte(A_V0ONE);
		break;
	    default:
		codebyte(A_V0SET);
		codebyte(n);
		break;
	}
    }
    else
    {
	switch (n)
	{
	    case 0:
		codebyte(A_VnZERO);
		codebyte(v);
		break;
	    case 1:
		codebyte(A_VnONE);
		codebyte(v);
		break;
	    default:
		codebyte(A_VnSET);
		codebyte(v);
		codebyte(n);
		break;
	}
    }
}

void
codeinc(s, incr)
    symbol	*s;
    int		incr;
{
    int		v = variable(s);

    if (v == 0)
    {
	codebyte(A_V0INCR + incr);
    }
    else
    {
	codebyte(A_VnINCR + incr);
	codebyte(v);
    }
}

void
codeprint(s)
    symbol	*s;
{
    int		v = variable(s);

    if (v == 0)
    {
	codebyte(A_V0PRINT);
    }
    else
    {
	codebyte(A_VnPRINT);
	codebyte(v);
    }
}

void
codeaugment(s, incr)
    symbol	*s;
    int		incr;
{
    int		v = variable(s);

    if (v == 0)
    {
	codebyte(A_V0AUG);
	codebyte(incr);
    }
    else
    {
	codebyte(A_VnAUG);
	codebyte(v);
	codebyte(incr);
    }
}

void
negate(offset)
    int		offset;
{
    code_ptr[-offset]++;
}

void
endcond()
{
    codebyte(0);
}

void
endrule()
{
    codebyte(0);
    switch (phase)
    {
	case indaemons:
	    ndaemons++;
	    break;
	case inactions:
	    nactions++;
	    break;
    }
}

void
enddaemons()
{
    act_ptr = code_ptr;
    phase = inactions;
}

int
typecheck(s, t)
    symbol	*s;
    int		t;
{
    if (s->s_type != t)
    {
	typeerror(s->s_sym, t);
	return 255;
    }
    else
    {
	return s->s_value;
    }
}

static char *writerule();

void
printcode(f)
    FILE	*f;
{
    char	*cp = code;

   fprintf(f, "%d\n", ndaemons);
   while (cp < act_ptr)
   {
       cp = writerule(f, cp);
   }
   fprintf(f, "%d\n", nactions);
   while (cp < code_ptr)
   {
       cp = writerule(f, cp);
   }
}

static char *
writerule(f, cp)
    FILE	*f;
    char	*cp;
{
    /*  Write precondition, including final 0 */
    do
    {
	fprintf(f, "%u,", *cp & 255);
    } while (*cp++);
    /*  Write rule, not including final 0 (to avoid spurious comma) */
    while (*cp)
    {
	fprintf(f, "%u,", *cp++ & 255);
    }
    /*  Write final 0 */
    fprintf(f, "0\n");

    return cp+1;
}

static int
variable(s)
    symbol	*s;
{
    if (s->s_type == S_UNKNOWN)
    {
	decl_var(s);
	if (!okundeclvars)
	{
	    gramerror(TRUE, "warning - variable %s not previously declared",
			s->s_sym);
	}
    }
    return typecheck(s, S_VAR);
}

void
novarwarnings()
{
    okundeclvars = TRUE;
}
