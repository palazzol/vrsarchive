/*
 * error.c: general-purpose error-handling
 */

#include	"advgen.h"

static char	*progname = "no name";	/* Set-up by setprogname(argc,argv) */
static char	*infilename = "standard input";

void
setprogname(count,args)
	int	count;
	char	*args[];
{
	char	*p;

	progname = (count>0 ? args[0] : "anonymous program");
	if ((p = rindex(progname, '/')) != NULL)
	{
	    progname = p + 1;
	}
}

void
setinfile(s)
    char	*s;
{
    infilename = s;
}

/*VARARGS3*/
static void
doerror(exitcode, level, message, arg1, arg2, arg3, arg4)
/*
 * if exitcode !=0, exit(exitcode).
 * if message != NULL, fprintf it to stderr (maximum of 4 arguments).
 */
	int	exitcode;
	char	*level;
	char	*message;
{
	if (message != NULL)
	{
		fprintf(stderr, "%s: ", progname);
		if (level != NULL)
		{
			fprintf(stderr, "%s - ", level);
		}
		fprintf(stderr, message, arg1, arg2, arg3, arg4);
		fprintf(stderr, "\n");
	}
	if (exitcode != 0)
	{
		exit(exitcode);
		/*NOTREACHED*/
	}
}

/*VARARGS1*/
void
fatal(message, arg1, arg2, arg3, arg4)
	char	*message;
{
	doerror(1, "fatal", message, arg1, arg2, arg3, arg4);
}

/*VARARGS1*/
void
warning(message, arg1, arg2, arg3, arg4)
	char	*message;
{
	doerror(0, "warning", message, arg1, arg2, arg3, arg4);
}

/*VARARGS1*/
void
usage(use)
	char	*use;
{
	doerror(1, (char *) NULL, "usage: %s %s", progname, use);
}

/*VARARGS2*/
void
gramerror(dump, s, a1, a2, a3, a4)
    bool	dump;
    char	*s;
{
    extern int	yylineno;
    extern char	*infilename;

    if (dump)
    {
	dumpline();
    }
    fprintf(stderr, "%s, line %d - ", infilename, yylineno);
    fprintf(stderr, s, a1, a2, a3, a4);
    (void) putc('\n', stderr);
}

void
muldecl(s, type)
    char	*s;
    char	*type;
{
    gramerror(TRUE, "multiple declaration of %s %s", type, s);
    add_error();
}

void
muldef(s)
    char	*s;
{
    gramerror(TRUE, "warning: %s multiply defined", s);
}

static char	*typename[] =
{
	"n unknown", " keyword", " room", "n item", " variable", " message"
};

void
confdecl(s, type, flag)
    char	*s;
    char	*type;
    smallint	flag;
{
    gramerror(TRUE, "conflicting declaration of (%s) %s as a%s",
    		typename[flag], s, type);
    add_error();
}

void
typeerror(s, t)
    char	*s;
    int		t;
{
    gramerror(TRUE, "%s is not a%s", s, typename[t]);
    add_error();
}
