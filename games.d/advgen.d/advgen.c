#include	"advgen.h"

void
main(nargs, args)
    int		nargs;
    char	*args[];
{
    char	*envval();
    void	print();
    void	init_tabs();
#ifdef lint
    char	*optarg = CNULL;
    int		optind = 0;
#else
    extern char	*optarg;
    extern int	optind;
#endif
    int		opt;
    bool	oflag = FALSE;
    bool	wflag = FALSE;
    bool	errflag = FALSE;
    char	*outfilename = DEFOUTPUT;
    char	*interpreter = envval("ADVINT", DEFINTERPRETER);
    static char	badusage[] = "[-w] [-o output] [input]";
    FILE	*inf = stdin;
    FILE	*outf;

    setprogname(nargs, args);
    while ((opt = getopt(nargs, args, "wo:")) != EOF)
    {
	switch (opt)
	{
	    case 'o':
	        if (oflag)
		{
		    errflag = TRUE;
		}
		else
		{
		    oflag = TRUE;
		    outfilename = optarg;
		}
		break;
	    case 'w':
		if (wflag)
		{
		    errflag = TRUE;
		}
		else
		{
		    wflag = TRUE;
		    novarwarnings();
		}
		break;
	    default:
		errflag = TRUE;
		break;
	}
    }
    if (errflag)
    {
	usage(badusage);
    }
    nargs -= optind;
    args += optind;
    switch (nargs)
    {
	case 0:
	    break;
	case 1:
	    if ((inf = fopen(args[0], "r")) == NULL)
	    {
		fatal("cannot open input file %s", args[0]);
	    }
	    setinfile(args[0]);
	    break;
	default:
	    usage(badusage);
	    break;
    }
    init_tabs();

    if (parse(inf))
    {
    	if ((outf = fopen(outfilename, "w")) == NULL)
	{
	    fatal("cannot write output file %s", outfilename);
	}
	if (*interpreter != '\0')
	{
	    if (chmod(outfilename, 0755) != 0)
	    {
		warning("cannot set execute permission for %s", outfilename);
	    }
	    fprintf(outf, "#!%s\n", interpreter);
	}
	print(outf);
	exit(0);
    }
    else
    {
	exit(1);
    }
    /*NOTREACHED*/
}

static void
print(f)
    FILE	*f;
{
    printspecials(f);
    printrooms(f);
    printvocab(f);
    printitems(f);
    printmsgs(f);
    printcode(f);
    (void) fclose(f);
}

static void
init_tabs()
{
    init_syms();
    (void) decl_room(lookup("Limbo"), 0, SPNULL, "", FALSE);
    decl_item(lookup("NilITEM"), SNULL, SNULL, "");
}

static char *
envval(var, deflt)
    char	*var;
    char	*deflt;
{
    char	*result = getenv(var);

    return result == CNULL ? deflt : result;
}
