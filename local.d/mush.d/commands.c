/* @(#)cmds.c	(c) copyright 10/18/86 (Dan Heller) */

#include "mush.h"

/*
 * Note that all of the routines in here act upon and return 0 or -1.
 * if -1, then the main loop will clear message lists.
 */

struct cmd cmds[] = {
#ifdef SIGSTOP
    { "stop", stop },
#endif SIGSTOP
    { "?", 	  question_mark },{ "sh", sh },
    { "alias", 	  do_alias    },  { "unalias",	do_alias   },
    { "expand",	  do_alias    },  { "cmd", 	do_alias   },
    { "uncmd", 	  do_alias    },  { "from",	do_from    },
    { "un_hdr",	  do_alias    },  { "my_hdr",  	do_alias   },
    { "fkey", 	  do_alias    },  { "unfkey", 	do_alias   },
    { "set", 	  set         },  { "unset", 	set 	   },
    { "ignore",	  set         },  { "unignore", set 	   },
    { "version",  do_version  },  { "help",	print_help },
    { "pick", 	  do_pick     },  { "sort", 	sort 	   },
    { "next",	  readmsg     },  { "previous", readmsg    },
    { "type",     readmsg     },  { "print",	readmsg    },
    { "history",  disp_hist   },  { "top",	readmsg	   },
    { "saveopts", save_opts   },  { "source",   source 	   },
    { "headers",  do_hdrs     },  { "ls",	ls	   },
    { "folder",   folder      },  { "update",   folder     },
    { "cd", 	  cd          },  { "pwd",	cd 	   },
    { "exit",	  quit        },  { "quit", 	quit 	   },
    { "write", 	  save_msg    },  { "save", 	save_msg   },
    { "copy", 	  save_msg    },  { "folders",  folders    },
#ifdef CURSES
    { "curses",   curses_init },  { "bind", bind_it   }, { "unbind", bind_it },
#endif CURSES
    { "preserve", preserve    },  { "unpreserve",  preserve   },
    { "replyall", respond     },  { "replysender", respond    },
    { "delete",	  delete      },  { "undelete",    delete     },
    { "mail", 	  do_mail     },  { "echo",	   do_echo    },
    { "lpr", 	  lpr 	      },  { "alternates",  alts       },
    { NULL, quit }
};

struct cmd ucb_cmds[] = {
    { "t",   readmsg   }, { "n",  readmsg  }, { "p", readmsg  },
    { "+",   readmsg   }, { "-", readmsg   },
    { "x",   quit      }, { "q",  quit     },
    { ":a",  do_hdrs   }, { ":d", do_hdrs  },
    { ":o",  do_hdrs   }, { ":u", do_hdrs  }, { ":n", do_hdrs },
    { "z",   do_hdrs   }, { "z-", do_hdrs  }, { "z+", do_hdrs },
    { "h",   do_hdrs   }, { "H",  do_hdrs  },
    { "f",   do_from   }, { "m",  do_mail  }, { "alts", alts  },
    { "d",   delete    }, { "dt", delete   }, { "dp", delete  },
    { "u",   delete    }, { "fo", folder   },
    { "s",   save_msg  }, { "co", save_msg },
    { "pre", preserve  }, { "w",  save_msg },
    { "R",   respond   }, { "r",   respond },
    { "reply", respond }, { "respond", respond },
    { NULL, quit }
};

struct cmd hidden_cmds[] = {
    { "debug", toggle_debug }, { "open", 	nopenfiles },
    { "flags", msg_flags    }, { "stty",	my_stty    },
    { "setenv",	Setenv      }, { "unsetenv", 	Unsetenv   },
    { "printenv", Printenv  },
    { NULL, quit }
};

toggle_debug(argc, argv)
char **argv;
{
    if (argc < 2) /* no value -- toggle "debug" (off/on) */
	debug = !debug;
    else
	debug = atoi(*++argv);
    print("debugging value: %d\n", debug);
    return -1;
}

static
sorter(cmd1, cmd2)
register struct cmd *cmd1, *cmd2;
{
    return strcmp(cmd1->command, cmd2->command);
}

sort_commands()
{
    qsort((char *)cmds, sizeof(cmds)/sizeof(struct cmd)-1,
			sizeof(struct cmd), sorter);
}

/* if + was specified, then print messages without headers.
 * n or \n (which will be NULL) will print next unread or undeleted message.
 */
readmsg(x, argv, list)
register char **argv, list[];
{
    register char *p = x? *argv : NULL;
    register long flg = 0;

    if (x && *++argv && !strcmp(*argv, "-?"))
	return help(0, "readmsg", cmd_help);
#ifdef NOT_NOW
    if (ison(glob_flags, IS_GETTING)) {
	print("Finish editing your letter first.\n");
	return -1;
    }
#endif NOT_NOW
    if (!msg_cnt) {
	print("No messages.\n");
	return -1;
    }
    if (x && !strcmp(p, "top"))
	turnon(flg, TOP);
    if (p && (*p == '+')) {
	turnon(flg, NO_PAGE);
	turnon(flg, NO_HEADER);
    }
    if (x && (x = get_msg_list(argv, list)) == -1)
	return -1;
    else if (x == 0) {  /* no arguments were parsed (or given) */
	/* get_msg_list sets current msg on */
	unset_msg_bit(list, current_msg);
	/* most commands move to the "next" message. type and print don't */
	if ((!p || !*p || *p == 'n' || *p == '+') && current_msg < msg_cnt &&
				    isoff(msg[current_msg].m_flags, UNREAD))
	    current_msg++;
	if (p && (*p == '-' || !strcmp(p, "previous"))) {
	    while (--current_msg >= 0 && ison(msg[current_msg].m_flags, DELETE))
		;
	    if (current_msg < 0) {
		print("No previous message.\n");
		current_msg = 0;
		return -1;
	    }
	} else {
	    /*
	     * To be compatible with ucb-mail, find the next available unread
	     * message.  Stop at the end (don't wrap). Not the greatest way to
	     * do it, but people complain if it doesn't do it.
	     */
#ifdef NOT_ANYMORE
	    if (current_msg == msg_cnt) /* wrap around */
		current_msg = 0;
#endif NOT_ANYMORE
	    /* "type" or "print" prints the current only -- "next" goes on.. */
	    if (!p || !*p || *p == 'n')
		while (current_msg < msg_cnt &&
		    ison(msg[current_msg].m_flags, DELETE))
			current_msg++;
	    if (current_msg >= msg_cnt) {
		print("No more messages.\n");
		current_msg = msg_cnt - 1;
		return -1;
	    }
	}
	set_msg_bit(list, current_msg);
    }
    /* If we're piping messages, just return the message list */
    if (ison(glob_flags, DO_PIPE))
	return 0;
    current_msg = 0;
    for (x = 0; x < msg_cnt; x++)
	if (msg_bit(list, x)) {
	    current_msg = x;
#ifdef SUNTOOL
	    if (istool > 1) {
		read_mail(NO_ITEM, 0, NO_EVENT);
		return 0;
	    }
#endif SUNTOOL
	    display_msg(x, flg);
	}
    return 0;
}

preserve(n, argv, list)
register int n;		/* no use for argc, so use space for a local variable */
register char **argv, list[];
{
    register int unpre;

    unpre = !strncmp(*argv, "un", 2);
    if (*++argv && !strcmp(*argv, "-?"))
	return help(0, "preserve_help", cmd_help);
    if (get_msg_list(argv, list) == -1)
	return -1;
    for (n = 0; n < msg_cnt; n++)
	if (msg_bit(list, n))
	    if (unpre)
		turnoff(msg[n].m_flags, PRESERVE);
	    else {
		turnon(msg[n].m_flags, PRESERVE);
		turnoff(msg[n].m_flags, DELETE);
	    }
    if (istool)
	(void) do_hdrs(0, DUBL_NULL, NULL);
    return 0;
}

lpr(n, argv, list)
register int n;  /* no use for argc, so use its address space for a variable */
register char **argv, list[];
{
    register FILE	*pp;
    register long 	flags = 0;
    char		print_cmd[128], *printer, c, *cmd;
    int			total = 0, (*oldint)(), (*oldquit)();

    turnon(flags, NO_IGNORE);
    if (!(printer = do_set(set_options, "printer")) || !*printer)
	printer = DEF_PRINTER;
    while (argv && *++argv && **argv == '-') {
	n = 1;
	while (c = argv[0][n++])
	    switch(c) {
		case 'n': turnon(flags, NO_HEADER);
		when 'h': turnoff(flags, NO_IGNORE);
		when 'P': case 'd':
		    if (!argv[0][n]) {
		        print("specify printer!\n");
		        return -1;
		    }
		    printer = argv[0] + n;
		    n += strlen(printer);
		otherwise: return help(0, "lpr", cmd_help);
	    }
    }
    if (get_msg_list(argv, list) == -1)
	return -1;

    if (cmd = do_set(set_options, "print_cmd"))
	(void) strcpy(print_cmd, cmd);
    else
#ifdef SYSV
	(void) sprintf(print_cmd, "%s -d%s", LPR, printer);
#else
	(void) sprintf(print_cmd, "%s -P%s", LPR, printer);
#endif SYSV
    Debug("print command: %s\n", print_cmd);
    if (!(pp = popen(print_cmd, "w"))) {
	error("cannot print");
	return -1;
    }
    on_intr();
    for (n = 0; isoff(glob_flags, WAS_INTR) && n < msg_cnt; n++) {
	if (msg_bit(list, n)) {
	    if (total++)
		fputc('\f', pp); /* send a formfeed for multiple copies */
	    print("printing message %d...", n+1);
	    print_more("(%d lines)\n", copy_msg(n, pp, flags));
	}
    }
    off_intr();
    (void) pclose(pp);
    print_more("%d message%s printed ", total, (total==1)? "": "s");
    if (cmd)
	print_more("through \"%s\".\n", cmd);
    else
	print_more("at \"%s\".\n", printer);
    return 0;
}

/* save [msg_list] [file] */
save_msg(n, argv, list)   /* argc isn't used, so use space for variable 'n' */
register char **argv, list[];
{
    register FILE	*mail_fp;
    register char 	*file = NULL, *mode, firstchar = **argv, *tmp;
    int 		msg_number, force;
    long 		flg = 0;

    if (*++argv && !strcmp(*argv, "-?"))
	return help(0, "save_help", cmd_help);
    if (force = (*argv && !strcmp(*argv, "!")))
	argv++;
    if ((n = get_msg_list(argv, list)) == -1)
	return -1;
    argv += n;
    if (*argv && *(file = *argv) == '\\')
	file++;
    else if (!file) {
	/* if no filename specified, save in ~/mbox */
	if (firstchar == 'w') {
	    /* mbox should have headers. If he really wants it, specify it */
	    print("Must specify file name for 'w'\n");
	    return -1;
	}
	if (!(file = do_set(set_options, "mbox")) || !*file)
	    file = DEF_MBOX;
    }
    n = 1; /* tell getpath to ignore no such file or directory */
    tmp = getpath(file, &n);
    if (n < 0) {
	print("%s: %s\n", file, tmp);
	return -1;
    } else if (n) {
	print("%s is a directory\n", file);
	return -1;
    }
    file = tmp;
    if (force || access(file, 0))
	mode = "w", force = 0;
    else
	mode = "a";

    if (!(mail_fp = fopen(file, mode))) {
	error("cannot save in \"%s\"", file);
	return -1;
    }
#ifdef SUNTOOL
    if (istool)
	lock_cursors();
#endif SUNTOOL
    turnon(flg, NO_IGNORE);
    if (firstchar == 'w')
	turnon(flg, NO_HEADER);
    else
	turnon(flg, UPDATE_STATUS);
    if (do_set(set_options, "keepsave"))
	firstchar = 'c';
    for (n = msg_number = 0; msg_number < msg_cnt; msg_number++)
	if (msg_bit(list, msg_number)) {
            print("%sing msg %d... ",
		(firstchar == 's')? "Sav" : "Writ", msg_number+1);
	    print_more("(%d lines)\n", copy_msg(msg_number, mail_fp, flg));
	    /* only mark "deleted" if mailfile is /usr/spool/mail and
	     * we're not "copying.  If keepsave is set, then firstchar
	     * will have already been changed to 'c'
	     */
	    if (is_spool(mailfile) && firstchar != 'c')
		turnon(msg[msg_number].m_flags, DELETE);
	    n++;
	}
    fclose(mail_fp);
    print_more("%s %d msg%s to %s\n",
	    (*mode == 'a')? "Appended" : "Saved", n, (n != 1)? "s": "", file);
    if (is_spool(mailfile))
	turnon(glob_flags, DO_UPDATE);
#ifdef SUNTOOL
    if (istool) {
	unlock_cursors();
	(void) do_hdrs(0, DUBL_NULL, NULL);
    }
#endif SUNTOOL
    return 0;
}

respond(n, argv, list)
register int n;  /* no use for argc, so use its address space for a variable */
register char **argv, *list;
{
    register char *cmd = *argv;
    char list1[MAXMSGS_BITS];

    if (*++argv && !strcmp(*argv, "-?"))
	return help(0, "respond_help", cmd_help);
    if ((n = get_msg_list(argv, list)) == -1)
	return -1;
    /* make into our own list so ~: commands don't overwrite this list */
    bitput(list, list1, MAXMSGS, =);
    /* back up one arg to replace "cmd" in the new argv[0] */
    argv += (n-1);
    if (!strcmp(cmd, "replyall"))
	Upper(*cmd);
    strdup(argv[0], cmd);
    current_msg = -1;
    for (n = 0; n < msg_cnt; n++)
	if (msg_bit(list1, n)) {
	    if (current_msg > -1 && istool) {
		print("tool mode can not respond to multiple messages.");
		continue;
	    }
	    current_msg = n;
	    set_isread(n);
	    if (do_mail(1, argv) == -1) /* do_mail doesn't look at argc */
		return -1;
	}
    return 0;
}

/* cd to a particular directory specified by "p" */
cd(x, argv) /* argc, unused -- use space for a non-register variable */
register char **argv;
{
    char *tmp, cwd[128], *p = argv[1];
#ifdef SYSV
    char *getcwd();
#else
    char *getwd();
#endif SYSV
    int err = 0;

    if (!strcmp(*argv, "pwd")) {
	p = do_set(set_options, "cwd");
	if (p && *p) {
	    print("%s\n", p);
	    return -1;
	}
    }
    if (!p || !*p)
	p = (**argv == 'p')? "." : "~";
    x = 0;
    tmp = getpath(p, &x);
    if (x == -1)
        print("%s: %s.\n", p, tmp), err++;
    else if (!x)
        print("%s: not a directory.\n", tmp), err++;
    else if (chdir(tmp))
        error("can't chdir to %s", tmp), err++;
#ifdef SYSV
    if (getcwd(cwd, 128) == NULL)
#else
    if (getwd(cwd) == NULL)
#endif SYSV
	print("can't get cwd: %s.\n", cwd), err++;
    else {
	char **new_argv, buf[256];
	(void) sprintf(buf, "set cwd = \"%s\"", cwd);
	Debug("%s\n", buf);
	if (new_argv = mk_argv(buf, &x, 1))
	    (void) add_option(&set_options, new_argv), free_vec(new_argv);
    }
    if (istool || iscurses || err) {
	if (err)
	    turnon(glob_flags, CONT_PRNT);
	print("Working dir: %s\n", cwd);
    }
    return -1;
}

quit(argc, argv)
char **argv;
{
    if (argc > 1 && !strcmp(argv[1], "-?"))
	 return help(0, "quit_help", cmd_help);
    if ((!argc || (*argv && **argv == 'q')) && ison(glob_flags, DO_UPDATE)
	&& !copyback())
	return -1;
#ifdef CURSES
    if (iscurses) {
	/* we may already be on the bottom line; some cases won't be */
	move(LINES-1, 0), refresh();
    }
#endif CURSES
    cleanup(0);
#ifdef lint
    return 0;
#endif lint
}

delete(argc, argv, list)
register int argc;
register char **argv, list[];
{
    register int prnt_next, undel = argc && **argv == 'u';

    prnt_next = (argv && (!strcmp(*argv, "dt") || !strcmp(*argv, "dp")));

    if (argc && *++argv && !strcmp(*argv, "-?")) {
	print("usage: delete/undelete [msg_list]\n");
	return -1;
    }

    if (get_msg_list(argv, list) == -1)
	return -1;
    for (argc = 0; argc < msg_cnt; argc++)
	if (msg_bit(list, argc))
	    if (undel)
		turnoff(msg[argc].m_flags, DELETE);
	    else
		turnon(msg[argc].m_flags, DELETE);

    /* only if current_msg has been affected && not in curses mode */
    if (prnt_next == 0 && !iscurses && msg_bit(list, current_msg))
	prnt_next = !!do_set(set_options, "autoprint"); /* change to boolean */

    turnon(glob_flags, DO_UPDATE);

    /* goto next available message if current was just deleted.
     * If there are no more messages, turnoff prnt_next.
     */
    if (!iscurses && !undel && ison(msg[current_msg].m_flags, DELETE) &&
				!next_msg(FALSE, DELETE) && prnt_next)
	    prnt_next = 0;

    if (prnt_next)
	display_msg(current_msg, (long)0);
    if (istool)
	(void) do_hdrs(0, DUBL_NULL, NULL);
    return 0;
}

/*
 * historically from the "from" command in ucb-mail, this just prints
 * the composed header of the messages set in list or in pipe.
 */
do_from(n, argv, list)
char **argv, list[];
{
    int inc_cur_msg = 0;

    if (argv && *++argv && !strcmp(*argv, "-?"))
	return help(0, "from", cmd_help);
    if (argv && *argv && (!strcmp(*argv, "+") || !strcmp(*argv, "-")))
	if (!strcmp(*argv, "+")) {
	    if (!*++argv && current_msg < msg_cnt-1)
		current_msg++;
	    inc_cur_msg = 1;
	} else if (!strcmp(*argv, "-")) {
	    if (!*++argv && current_msg > 0)
		current_msg--;
	    inc_cur_msg = -1;
	}
    if (get_msg_list(argv, list) == -1)
	return -1;
    for (n = 0; n < msg_cnt; n++)
	if (msg_bit(list, n)) {
	    wprint("%s\n", compose_hdr(n));
	    /* if -/+ given, set current message pointer to this message */
	    if (inc_cur_msg) {
		current_msg = n;
		/* if - was given, then set to first listed message.
		 * otherwise, + means last listed message -- let it go...
		 */
		if (inc_cur_msg < 0)
		    inc_cur_msg = 0;
	    }
	}
    return 0;
}

/*
 * Do an ls from the system.
 * Read from a popen and use wprint in case the tool does this command.
 * The folders command uses this command.
 */
ls(x, argv)
char **argv;
{
    register char  *p, *tmp;
    char	   buf[128];
    register FILE  *pp;

    if (*++argv && !strcmp(*argv, "-?"))
	return help(0, "ls", cmd_help);
    if (!(p = do_set(set_options, "lister")))
	p = "";
    (void) sprintf(buf, "%s -C%s ", LS_COMMAND, p);
    p = buf+strlen(buf);
    for ( ; *argv; ++argv) {
	x = 0;
	if (**argv != '-')
	    tmp = getpath(*argv, &x);
	else
	    tmp = *argv;
	if (x == -1) {
	    wprint("%s: %s\n", *argv, tmp);
	    return -1;
	}
	p += strlen(sprintf(p, " %s", tmp));
    }
    if (!(pp = popen(buf, "r"))) {
	error(buf);
	return -1;
    }
    turnon(glob_flags, IGN_SIGS);
    while (fgets(buf, 127, pp))
	wprint(buf);
    (void) pclose(pp);
    turnoff(glob_flags, IGN_SIGS);
    return 0;
}

sh(un_used, argv)
char **argv;
{
    register char *p;
    char buf[128];

    if (istool > 1 || *++argv && !strcmp(*argv, "-?"))
	return help(0, "shell", cmd_help);
    if (!(p = do_set(set_options, "shell"))
	&& !(p = do_set(set_options, "SHELL")))
	p = DEF_SHELL;
    if (!*argv)
	(void) strcpy(buf, p);
    else
	(void) argv_to_string(buf, argv);
    echo_on();
    (void) system(buf);
    echo_off();
    return 0;
}

question_mark(x, argv)
char **argv;
{
    int n = 0;
    char *Cmds[50], *p, *malloc(), buf[30];

    if (!*++argv) {
	for (x = 0; cmds[x].command; x++) {
	    if (!(x % 5))
		if (!(p = Cmds[n++] = malloc(80))) {
		    error("malloc in question_mark()");
		    free_vec(Cmds);
		    return -1;
		}
	    p += strlen(sprintf(p, "%-11.11s  ", cmds[x].command));
	}
	Cmds[n++] = savestr("Type: `command' -? for help with most commands.");
	Cmds[n] = NULL;
	(void) help(0, Cmds, NULL);
	free_vec(Cmds);
    } else {
	for (x = 0; cmds[x].command; x++)
	    if (!strcmp(*argv, cmds[x].command))
		return cmd_line(sprintf(buf, "%s -?", *argv), msg_list);
	print("Unknown command: %s\n", *argv);
    }
    return -1;
}

#ifdef SIGSTOP
stop(argc, argv)
char **argv;
{
    if (istool)
	print("Not a tool-based option.");
    if (argc && *++argv && !strcmp(*argv, "-?"))
	return help(0, "stop", cmd_help);
    if (kill(getpid(), SIGTSTP) == -1)
	error("couldn't stop myself");
    return -1;
}
#endif SIGSTOP

extern char **environ;
static int spaces = 0;

Setenv(i, argv)
char **argv;
{
    char *newstr;

    if (i < 2 || i > 3 || !strcmp(argv[1], "-?"))
	return help(0, "setenv", cmd_help);

    if (i == 3) {
	newstr = malloc(strlen(argv[1]) + strlen(argv[2]) + 2);
	(void) sprintf(newstr, "%s=%s", argv[1], argv[2]);
    } else {
	newstr = malloc(strlen(argv[1]) + 2);
	(void) sprintf(newstr, "%s=", argv[1]);
    }

    (void) Unsetenv(2, argv);

    for (i = 0; environ[i]; i++);
    if (!spaces) {
	char **new_environ = (char **)malloc((i+2) * sizeof(char *));
	/* add 1 for the new item, and 1 for null-termination */
	if (!new_environ) {
	    free(newstr);
	    return -1;
	}
	spaces = 1;
	for (i = 0; new_environ[i] = environ[i]; i++);
	xfree(environ);
	environ = new_environ;
    }
    environ[i] = newstr;
    environ[i+1] = NULL;
    spaces--;
    return -1;
}

Unsetenv(n, argv)
char **argv;
{
    char **envp, **last;

    if (n != 2 || !strcmp(argv[1], "-?"))
	return help(0, "unsetenv", cmd_help);

    n = strlen(argv[1]);
    for (last = environ; *last; last++);
    last--;

    for (envp = environ; envp <= last; envp++) {
	if (strncmp(argv[1], *envp, n) == 0 && (*envp)[n] == '=') {
	    xfree(*envp);
	    *envp = *last;
	    *last-- = NULL;
	    spaces++;
	}
    }
    return -1;
}

Printenv()
{
    char **e = environ;
    while (*e)
	wprint("%s\n", *e++);
    return -1;
}

/*
 * internal stty call to allow the user to change his tty character
 * settings.  sorry, no way to change cbreak/echo modes.  Save echo_flg
 * so that execute() won't reset it.
 */
my_stty(un_used, argv)
char **argv;
{
    long save_echo = ison(glob_flags, ECHO_FLAG);

    turnon(glob_flags, ECHO_FLAG);
    execute(argv);
    if (save_echo)
	turnon(glob_flags, ECHO_FLAG);
    else
	turnoff(glob_flags, ECHO_FLAG);

    savetty();
#ifdef TIOCGLTC
    if (ioctl(0, TIOCGLTC, &ltchars))
	error("TIOCGLTC");
#endif TIOCGLTC
    echo_off();
    return -1;
}
