/*
**	Cake main file.
*/

static	char
rcs_id[] = "$Header: /home/Vince/cvs/local.d/cake.d/main.c,v 1.1 1987-11-16 17:21:08 vrs Exp $";

#include	"cake.h"
#include	<pwd.h>
#include	<signal.h>
#include	<sys/stat.h>

typedef	struct	passwd	Pwent;
typedef	struct	stat	Stat;

int	Gflag = FALSE;
int	Lflag = FALSE;
int	Rflag = FALSE;
int	Xflag = FALSE;
int	Zflag = FALSE;
int	bflag = FALSE;
int	cflag = FALSE;
int	dflag = FALSE;
int	gflag = FALSE;
int	iflag = FALSE;
int	kflag = TRUE;
int	nflag = FALSE;
int	qflag = FALSE;
int	rflag = FALSE;
int	sflag = FALSE;
int	tflag = FALSE;
int	vflag = FALSE;
int	wflag = FALSE;
int	xflag = FALSE;
int	zflag = FALSE;

char	*cakefile  = NULL;
char	*shellfile[2] = { SYSTEM_CMD, SCRIPT_CMD };
char	*metachars = METACHARS;
int	maxprocs   = 1;
List	*active_procs;
char	scratchbuf[128];

int	cakedebug	= FALSE;
int	entrydebug	= FALSE;
int	patdebug	= FALSE;
int	lexdebug	= FALSE;

char	cakeflagbuf[MAXSIZE];
char	*cppargv[MAXARGS];
int	cppargc = 0;

main(argc, argv)
int	argc;
char	**argv;
{
	extern		cake_abort(), cake_finish();
	extern	int	parse_args(), process_args();
	extern	Node	*chase();
	extern	char	*getenv();
	extern	char	*dir_setup();
	extern	Pwent	*getpwuid();
	extern	int	geteuid();
	extern	FILE	*cake_popen();
	extern	FILE	*yyin;
	extern	int	yydebug;
	Stat		statbuf;
	int		envc;
	char		*envv[MAXARGS];
	reg	Pwent	*pwent;
	reg	char	*envstr;
	reg	int	status;
	reg	Node	*rootnode;

	signal(SIGINT,  cake_finish);
	signal(SIGQUIT, cake_finish);
	signal(SIGILL,  cake_abort);
	signal(SIGTRAP, cake_abort);
	signal(SIGIOT,  cake_abort);
	signal(SIGEMT,  cake_abort);
	signal(SIGFPE,  cake_abort);
	signal(SIGBUS,  cake_abort);
	signal(SIGSEGV, cake_abort);
	signal(SIGSYS,  cake_abort);
	signal(SIGPIPE, cake_abort);
	signal(SIGALRM, cake_abort);

	yydebug = FALSE;
	active_procs = makelist0();

	if (rindex(argv[0], 'f') != NULL
	&&  streq(rindex(argv[0], 'f'), "fake"))
		cakedebug = TRUE;

	init_sym();
	cppargv[cppargc++] = new_name(CPP);
	strcpy(cakeflagbuf, "-DCAKEFLAGS=");

	if ((envstr = getenv("CAKE")) != NULL)
	{
		envc = parse_args(envstr, envv);
		process_args(envv, &envc, 0);
		if (envc > 0)
			fprintf(stderr, "cake: non-options in environment ignored\n");
	}

	argv += process_args(argv, &argc, 1);

#ifdef	CAKEDEBUG
	if (cakedebug || entrydebug || patdebug || lexdebug)
		setlinebuf(stdout);
#endif

	if (cakefile == NULL)
	{
		if (stat("cakefile", &statbuf) == 0)
			cakefile = "cakefile";
		or (stat("Cakefile", &statbuf) == 0)
			cakefile = "Cakefile";
		or (stat("recipe", &statbuf) == 0)
			cakefile = "recipe";
		or (stat("Recipe", &statbuf) == 0)
			cakefile = "Recipe";
		else
		{
			fprintf(stderr, "cake: cannot locate a cakefile\n");
			exit(1);
		}
	}

	if (gflag)
		cakefile = dir_setup(cakefile);

	pwent = getpwuid(geteuid());
	strcpy(scratchbuf, "-I");
	strcat(scratchbuf, pwent->pw_dir);
	strcat(scratchbuf, ULIB);
	cppargv[cppargc++] = new_name(scratchbuf);
	strcpy(scratchbuf, "-I");
	strcat(scratchbuf, SLIB);
	cppargv[cppargc++] = new_name(scratchbuf);
	cppargv[cppargc++] = cakeflagbuf;
	cppargv[cppargc++] = cakefile;
	cppargv[cppargc]   = NULL;

	if (cakedebug)
	{
		reg	int	i;

		for (i = 0; i < cppargc; i++)
			printf("%s\n", cppargv[i]);
	}

	if ((yyin = cake_popen(cppargv, "r")) == NULL)
	{
		fprintf(stderr, "cake: cannot open cpp filter\n");
		exit(1);
	}

	if (Zflag)
	{
		reg	int	c;

		while ((c = getc(yyin)) != EOF)
			putchar(c);

		cake_pclose(yyin);
		exit(0);
	}

	yyinit();
	init_entry();
	if (yyparse())
	{
		fprintf(stderr, "cake: cannot parse %s\n", cakefile);
		exit(1);
	}

	shell_setup(shellfile[0], 0);
	shell_setup(shellfile[1], 1);
	meta_setup(metachars);

	cake_pclose(yyin);
	dir_start();
	prep_entries();
	final_entry(argc, argv);

	rootnode = chase(CHASEROOT, 0, (Entry *) NULL);

	if (! qflag)
		execute(rootnode);
	
	dir_finish();
	cleanup();
#ifdef	STATS_FILE
	statistics();
#endif

	status = (off_node(rootnode, nf_ERR) && is_ok(rootnode))? 0: 1;
	cdebug("exit status %d\n", status);
	exit(status);
}

int
process_args(vector, count, base)
reg	char	**vector;
reg	int	*count;
reg	int	base;
{
	reg	int	i, j;

	j = 0;
	cdebug("process args:");
	while (*count > base && vector[base][0] == '-')
	{
		putflag(base, vector[base]);

		for (i = 1; vector[base][i] != '\0'; i++)
		{
			switch (vector[base][i])
			{

#ifdef	CAKEDEBUG
		when 'C':	cdebug(" -C");
				cakedebug  = ! cakedebug;
		
		when 'E':	cdebug(" -E");
				entrydebug = ! entrydebug;
		
		when 'P':	cdebug(" -P");
				patdebug   = ! patdebug;
		
		when 'W':	cdebug(" -W");
				lexdebug   = TRUE;
		
		when 'Y':	cdebug(" -Y");
				yydebug    = TRUE;
#endif
		when 'G':	cdebug(" -G");
				Gflag = TRUE;
		
		when 'L':	cdebug(" -L");
				Lflag = TRUE;
		
		when 'R':	cdebug(" -R");
				Rflag = TRUE;
		
		when 'X':	cdebug(" -X");
				Xflag = TRUE;
		
		when 'Z':	cdebug(" -Z");
				Zflag = TRUE;
		
		when 'a':	cdebug(" -a");
				kflag = FALSE;
		
		when 'b':	cdebug(" -b");
				bflag = TRUE;
		
		when 'c':	cdebug(" -c");
				cflag = TRUE;
		
		when 'd':	cdebug(" -d");
				dflag = TRUE;
		
		when 'g':	cdebug(" -g");
				gflag = TRUE;
		
		when 'i':	cdebug(" -i");
				iflag = TRUE;
		
		when 'k':	cdebug(" -k");
				kflag = TRUE;
		
		when 'n':	cdebug(" -n");
				nflag = TRUE;
				tflag = FALSE;
				qflag = FALSE;
		
		when 'q':	cdebug(" -q");
				qflag = TRUE;
				nflag = FALSE;
				tflag = FALSE;
		
		when 'r':	cdebug(" -r");
				rflag = TRUE;
		
		when 's':	cdebug(" -s");
				sflag = TRUE;
		
		when 't':	cdebug(" -t");
				tflag = TRUE;
				nflag = FALSE;
				qflag = FALSE;
		
		when 'v':	cdebug(" -v");
				vflag = TRUE;
		
		when 'w':	cdebug(" -w");
				wflag = TRUE;
		
		when 'x':	cdebug(" -x");
				xflag = TRUE;
		
		when 'z':	cdebug(" -z");
				zflag = TRUE;
		
		when 'D':
		case 'I':
		case 'U':	if (i != 1)
					usage();

				cdebug(" %s", vector[base]);
				cppargv[cppargc++] = new_name(vector[base]);
				goto nextword;
		
		when 'N':	putflag(base, vector[base+1]);
				sscanf(vector[base+1], "%d", &maxprocs);
				if (vector[base][i+1] != '\0')
					usage();

				cdebug(" -N %d", maxprocs);
				(*count)--;
				vector++, j++;
				goto nextword;
		
		when 'S':	putflag(base, vector[base+1]);
				if (vector[base][i+1] == '1')
					shellfile[0] = new_name(vector[base+1]);
				or (vector[base][i+1] == '2')
					shellfile[1] = new_name(vector[base+1]);
				else
					usage();

				if (vector[base][i+2] != '\0')
					usage();

				cdebug(" -S%c %s", vector[base][i+1], vector[base+1]);
				(*count)--;
				vector++, j++;
				goto nextword;
		
		when 'T':	putflag(base, vector[base+1]);
				metachars = new_name(vector[base+1]);
				if (vector[base][i+1] != '\0')
					usage();

				cdebug(" -T %s", metachars);
				(*count)--;
				vector++, j++;
				goto nextword;
		
		when 'f':	putflag(base, vector[base+1]);
				cakefile = new_name(vector[base+1]);
				if (vector[base][i+1] != '\0')
					usage();

				cdebug(" -f %s", cakefile);
				(*count)--;
				vector++, j++;
				goto nextword;

		otherwise:	usage();
			}
		}

nextword:
		(*count)--;
		vector++, j++;
	}

	cdebug(" \n%return %d\n", j);
	return j;
}

/*
**	Put a flag into the CAKEFLAGS definition.
*/

putflag(base, flag)
reg	int	base;
reg	char	*flag;
{
	if (base == 1)
	{
		strcat(cakeflagbuf, " ");
		strcat(cakeflagbuf, flag);
		if (strlen(cakeflagbuf) >= MAXSIZE)
		{
			fprintf(stderr, "cake: CAKEFLAGS too long\n");
			exit(1);
		}
	}
}

/*
**	Tell the unfortunate user how to use cake.
*/

usage()
{
	fprintf(stderr, "Usage: cake [-abcdgiknqrstvwxzGLRXZ] [-ffile]\n");
	fprintf(stderr, "       [-Ddefn] [-Idir] [-Uname] [-S shell] [-T metachars] [file ...]\n");
	exit(1);
}

exit_cake(needtrail)
reg	int	needtrail;
{
	if (cakedebug && needtrail)
		get_trail(stdout);
	else
		dir_finish();

	exit(1);
}

/*
**	Handle bus errors and segmentation violations.
*/

cake_abort()
{

	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	printf("Abort on signal\n");
	if (cakedebug)
		get_trail(stdout);

	signal(SIGQUIT, SIG_DFL);
	kill(getpid(), SIGQUIT);
}

/*
**	Handle user interrupts.
*/

cake_finish()
{
	reg	List	*ptr;
	reg	Proc	*proc;

	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	printf("*** Interrupt\n");
	fflush(stdout);
	for_list (ptr, active_procs)
	{
		proc = (Proc *) ldata(ptr);
		if (proc->pr_node != NULL)
			cake_error(proc->pr_node);
	}

	exit_cake(FALSE);
}

#ifdef	STATS_FILE
#ifdef	ATT
#include	<sys/times.h>

typedef	struct	tms	Tms;
#else
#include	<sys/time.h>
#include	<sys/resource.h>

typedef	struct	rusage	Rusage;
#endif

statistics()
{
	extern	char	*getlogin();
	extern		getpw();
	extern	int	out_tried, out_found;
	extern	int	stat_tried, stat_found;
	FILE		*sfp;

	if ((sfp = fopen(STATS_FILE, "a")) != NULL)
	{
#ifdef	ATT
		Tms	tbuf;
#else
		Rusage	s, c;
#endif
		long	su, ss, cu, cs;
		char	*usr;

		if ((usr = getlogin()) == NULL)
		{
			char	buf[256];
			char	*usr_end;

			if (getpw(getuid(), buf) != 0)
				usr = "NULL";
			else
			{
				usr = buf;
				if ((usr_end = index(usr, ':')) != NULL)
					*usr_end = '\0';
				else
					usr = "NULL";
			}

			usr = new_name(usr);
		}

#ifdef	ATT
		if (times(&tbuf) == -1)
		{
			fclose(sfp);
			return;
		}

		su = tbuf.tms_utime*100/TIMERES;
		ss = tbuf.tms_stime*100/TIMERES;
		cu = tbuf.tms_cutime*100/TIMERES;
		cs = tbuf.tms_cstime*100/TIMERES;
#else
		getrusage(RUSAGE_SELF,     &s);
		getrusage(RUSAGE_CHILDREN, &c);

		su = s.ru_utime.tv_sec*100 + s.ru_utime.tv_usec/10000;
		ss = s.ru_stime.tv_sec*100 + s.ru_stime.tv_usec/10000;
		cu = c.ru_utime.tv_sec*100 + c.ru_utime.tv_usec/10000;
		cs = c.ru_stime.tv_sec*100 + c.ru_stime.tv_usec/10000;
#endif
		fprintf(sfp, "%s %ld %ld %ld %ld ", usr, su, ss, cu, cs);
		fprintf(sfp, "%d %d %d %d %d\n", sbrk(0),
			out_tried, out_found, stat_tried, stat_found);

		fclose(sfp);
	}
}
#endif
