/* @(#)main.c	(c) copyright 10/18/86 (Dan Heller) */

#include "mush.h"

static char *usage_str =
#ifdef SUNTOOL 
    "usage: %s [-C] [-i] [-f [folder] ] [-v] [-S] [-t] [-s subject] [users]\n";
#else
#ifdef CURSES
    "usage: %s [-C] [-i] [-f [folder] ] [-v] [-S] [-s subject] [user list]\n";
#else
    "usage: %s [-i] [-f [folder] ] [-v] [-S] [-s subject] [user list]\n";
#endif CURSES
#endif SUNTOOL

#if defined(sun) && defined(M_DEBUG)
cpu()
{
    print("CPU time limit exceeded!\n");
}
#endif sun && DEBUG

/*ARGSUSED*/   /* we ignore envp */
main(argc, argv)
char **argv;
{
    int 	    flg = NO_FLG, n;
    char 	    f_flags[10], buf[256], *Cc = NULL;
    register char   *p;

    if (prog_name = rindex(*argv, '/'))
	prog_name++;
    else
	prog_name = *argv;

    (void) signal(SIGBUS,  bus_n_seg);
    (void) signal(SIGSEGV, bus_n_seg);

    f_flags[0] = 0;
    mailfile = "";

#if defined(sun) && defined(M_DEBUG)
    (void) signal(SIGXCPU, cpu);

    if (p = getenv("MALLOC_DEBUG"))
	malloc_debug(atoi(p));
    else
	malloc_debug(0);
#endif sun && debug

    if (!isatty(0))
	turnon(glob_flags, REDIRECT);
    else
	initscr ();

    f_flags[0] = '\0';

    n = 0; /* don't ignore no such file or directory */
    p = getpath(COMMAND_HELP, &n);
    if (n) {
	fprintf(stderr, "Warning: can't read %s: %s\n", COMMAND_HELP, p);
	cmd_help = "cmd_help";
    } else
	strdup(cmd_help, p);
#ifdef SUNTOOL
    n = 0;
    p = getpath(TOOL_HELP, &n);
    if (n) {
	fprintf(stderr, "Warning: can't read %s: %s\n", TOOL_HELP, p);
	tool_help = "tool_help";
    } else
	strdup(tool_help, p);

    istool = strlen(prog_name) > 3 &&
		  !strcmp(prog_name+strlen(prog_name)-4, "tool");
    time_out = 60;
#endif SUNTOOL

    /*
     * preparse the command line looking for certain arguments which must
     * be known before sourcing the .mushrc file.  This should definitely
     * be optimized -- see the TODO file accompanying this distibution.
     */
    n = 1;
    if (argc > 1) {
	char **args;
	for (args = argv; *args; args++)
#ifdef SUNTOOL
	    if (!strcmp(*args, "-t"))
		istool = 1;
	    else
#endif SUNTOOL
	    if (!strcmp(*args, "-C") && !hdrs_only)
		/* this will only temporarily be turned on! */
		turnon(glob_flags, PRE_CURSES);
	    /* headers only?  get args later, just set to a non-null value */
	    else if (!strncmp(*args, "-H", 2))
		hdrs_only = NO_STRING, turnoff(glob_flags, PRE_CURSES);
	    else if (!strcmp(*args, "-d"))
		debug = 1;
	    else if (!strcmp(*args, "-n"))
		n = 0;
    }

    /*
     * read mailrc and so forth. Global variables not set there will have
     * default settings here.  Set them first so user overrides in "source()"
     */
    init(); /* must be done before checking mail since "login" is set here */

    /* check to see if we have any mail at all */
    if (argc < 2 && !istool) { /* istool may be set with one arg: "mushtool" */
	struct stat statb;
	if (stat(sprintf(buf, "%s/%s", MAILDIR, login), &statb) ||
		statb.st_size == 0) {
	    printf("No mail for %s.\n", login);
	    exit(1);
	}
    }
    /* -n flag above */
    if (n)
	(void) source(0, DUBL_NULL);

#ifdef SUNTOOL
    if (istool)
	if (ison(glob_flags, REDIRECT))
	    puts("You can't redirect input to a tool."), exit(1);
	else
	    make_tool(argc, argv), turnon(glob_flags, DO_SHELL);
#endif SUNTOOL

    for (argv++; *argv && **argv == '-'; argv++)
	switch (argv[0][1]) {
	    case 'e':
		/*
		 * don't set tty modes -- e.g. echo and cbreak modes aren't
		 * changed.
		 */
		turnon(glob_flags, ECHO_FLAG);
#ifdef CURSES
	    when 'C':
		/* don't init curses -- don't even set iscurses.   */
		if (istool) {
		    puts("-C: You are already running in tool mode");
		    turnoff(glob_flags, PRE_CURSES);
		} else if (hdrs_only) {
		    puts("headers only: ignoring -C flag");
		    turnoff(glob_flags, PRE_CURSES);
		} else
		    turnon(glob_flags, PRE_CURSES);
#endif CURSES
	    when 'N':
		(void) strcat(f_flags, "-N ");
	    when 'r':
		(void) strcat(f_flags, "-r "); /* folder() argument */
	    when 'H':
		if (istool) {
		    puts("running in tool-mode; -H option ignored.");
		    break;
		}
		turnoff(glob_flags, PRE_CURSES);
		if (*(hdrs_only = (*argv)+2) != ':')
		    hdrs_only = ":a";
		(void) strcat(f_flags, "-N "); /* tell folder not to do_hdrs */
	    when 'i':
		/* force interactive even if !isatty(0) */
		turnoff(glob_flags, REDIRECT);
	    /* if no argument for this, use mbox in _current directory_ */
	    when 'f':
		if (argv[1])
		    p = *++argv;
		else if (!(p = do_set(set_options, "mbox")) || !*p)
		    p = DEF_MBOX;
		strdup(mailfile, getpath(p, &n));
		if (n) {
		    if (n == -1)
			print("%s: %s\n", p, mailfile);
		    else
			print("%s: Is a directory\n", mailfile);
		    if (!istool)
			exit(1);
		    strdup(mailfile, "");
		}
	    when '1':
		if (argv[1])
		    strdup(cmd_help, *++argv);
		else
		    puts("-1 \"filename\""), cleanup(0);
#ifdef SUNTOOL
	    when '2':
		if (argv[1])
		    strdup(tool_help, *++argv);
		else
		    puts("-2 \"filename\""), cleanup(0);
#endif SUNTOOL
	    when 's':
		if (istool)
		    puts("bad option when run as a tool");
		else if (argv[1])
		    (void) strcpy(buf, *++argv);
		else
		    puts("-s \"subject\""), cleanup(0);
	    when 'c':
		if (istool)
		    puts("bad option when run as a tool");
		else if (argv[1])
		    strdup(Cc, *++argv);
		else
		    puts("-c \"cc list\""), exit(1);
	    when 'S': turnon(glob_flags, DO_SHELL); /* force the shell */
#ifdef VERBOSE_ARG
	    when 'v':
		if (istool)
		    puts("bad option when run as a tool");
		turnon(flg, VERBOSE);
#endif VERBOSE_ARG
#ifdef SUNTOOL
            when 'T':
		if (!*++argv)
		    continue;
		if (istool && (time_out = atoi(*argv)) <= 29)
		    time_out = 30;
		else
		    printf("-T flag ignored unless run as tool."), exit(1);
	    when 't': ;   /* prevent error message */
#endif SUNTOOL
	    when 'n': case 'd': ;   /* prevent error messages */
	    otherwise:
		print("%s: unknown option: `%c'\n", prog_name,
		    argv[0][1]? argv[0][1] : '-');
		print(usage_str, prog_name);
		if (!istool)
		    exit(1);
	}

    /* now we're ready for I/O */
    if (isoff(glob_flags, REDIRECT)) {
	/* make sure we can always recover from no echo mode */
	(void) signal(SIGINT, catch);
	(void) signal(SIGQUIT, catch);
	if (istool)
	    turnon(glob_flags, ECHO_FLAG);
	savetty();
#ifdef TIOCGLTC
	if (isatty(0) && ioctl(0, TIOCGLTC, &ltchars))
	    error("TIOCGLTC");
#endif TIOCGLTC
#ifdef SIGCONT
	(void) signal(SIGTSTP, stop_start); /* this will take care of SIGCONT */
#endif SIGCONT
	/* echo_off() checks to see if echo_flg is set, so don't worry */
	echo_off();
    }

    if (*argv) {
	char recipients[BUFSIZ];
	if (*mailfile)
	    puts("You can't specify more than one folder"), cleanup(0);
	if (istool)
	    puts("You can't mail someone and run a tool."), cleanup(0);
	(void) argv_to_string(recipients, argv);
	/* prompt for subject and Cc list, but not "To: "
	 * mail_someone() already takes care of redirection.
	 * if -s or -c options are given, they will be passed.
	 */
	if (do_set(set_options, "autosign"))
	    turnon(flg, SIGN);
	if (do_set(set_options, "autoedit"))
	    turnon(flg, EDIT);
	if (do_set(set_options, "verbose"))
	    turnon(flg, VERBOSE);
	if (do_set(set_options, "fortune")) {
	    p = do_set(set_options, "fortunates");
	    if (!p || *p && (chk_two_lists(recipients, p, " \t,")
		   || (Cc && *Cc && chk_two_lists(Cc, p, " \t,"))))
		turnon(flg, DO_FORTUNE);
	}
	/* set now in case user is not running shell, but is running debug */
	(void) signal(SIGCHLD, sigchldcatcher);
	(void) mail_someone(recipients, buf, Cc, flg, NULL);
	/* do shell set from above: "mush -S user" perhaps */
	if (isoff(glob_flags, DO_SHELL)) {
	    if (isoff(glob_flags, REDIRECT))
		echo_on();
	    exit(0);
	}
    }
    if (ison(glob_flags, REDIRECT)) {
	puts("You can't redirect input unless you're sending mail.");
	puts("If you want to run a shell with redirection, use \"-i\"");
	cleanup(0);
    }
    if (!*mailfile) {
	strdup(mailfile, sprintf(buf, "%s/%s", MAILDIR, login));
	if (!mail_size() && isoff(glob_flags, DO_SHELL)) {
	    /* we know it's not the spool file here */
	    printf("No mail in %s.\n", mailfile);
	    echo_on(), exit(0);
	}
    }
    /* At this point, we know we're running a shell, so... */
    if (!hdrs_only) {
	/* catch will test DO_SHELL and try to longjmp if set.  this is a
	 * transition state from no-shell to do-shell to ignore sigs to
	 * avoid a longjmp botch.  Note setjmp isn't called until do_loop().
	 */
	turnon(glob_flags, DO_SHELL);
	turnon(glob_flags, IGN_SIGS);
#ifdef CURSES
	if (ison(glob_flags, PRE_CURSES))
	    (void) curses_init(0, DUBL_NULL);
	turnoff(glob_flags, PRE_CURSES);
#endif CURSES
    }

    /* find a free tmpfile */
    flg = getpid();
    if (!(p = do_set(set_options, "home")) || !*p)
	p = ALTERNATE_HOME;
    while (!access(sprintf(tempfile, "%s/.%s%d", p, prog_name, flg++), 0))
	;
    /* just create the file, make sure it's empty.  It'll close later and
     * be reopened for reading only.
     */
    {
	int omask = umask(077);
	if (!(tmpf = fopen(tempfile, "w"))) {
	    error("Can't create tempfile %s", tempfile);
	    cleanup(0);
	}
	(void) umask(omask);
    }

    /* do pseudo-intelligent stuff with certain signals */
    (void) signal(SIGINT,  catch);
    (void) signal(SIGQUIT, catch);
    (void) signal(SIGHUP,  catch);

    /* if we're a suntool, then state the version and mark first message
     * as "read" since it will be displayed right away
     */
    if (!hdrs_only && !istool && !do_set(set_options, "quiet"))
	printf("%s: Type '?' for help.\n", VERSION);

    (void) sprintf(buf, "folder %s %s", f_flags, mailfile);
    if (argv = make_command(buf, TRPL_NULL, &argc)) {
	(void) folder(argc, argv, NULL);
	free_vec(argv);
    }

    if (hdrs_only) {
	(void) sprintf(buf, "headers %s", hdrs_only);
	if (argv = make_command(buf, TRPL_NULL, &argc))
	    (void) do_hdrs(argc, argv, NULL);
	cleanup(0);
    }

    if (istool && msg_cnt)
	set_isread(current_msg);

    sort_commands();
    sort_variables();

#ifdef SUNTOOL
    if (istool) {
	turnoff(glob_flags, IGN_SIGS);
	(void) do_hdrs(0, DUBL_NULL, NULL);
	timerclear(&(mail_timer.it_interval));
	timerclear(&(mail_timer.it_value));
	mail_timer.it_value.tv_sec = time_out;
	setitimer(ITIMER_REAL, &mail_timer, NULL);
	(void) signal(SIGALRM, check_new_mail);
	unlock_cursors();
	while (!(tool->tl_flags & TOOL_DONE))
	    tool_select(tool, 1);
	cleanup(0);
    }
#endif SUNTOOL
	do_loop();
}

do_version()
{
    print("%s\n", VERSION);
    return -1;
}
