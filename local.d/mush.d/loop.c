/* loop.c 	(c) copyright 1986 (Dan Heller) */

/*
 * Here is where the main loop for text mode exists. Also, all the
 * history is kept here and all the command parsing and execution
 * and alias expansion in or out of text/graphics mode is done here.
 */

#include "mush.h"

#ifdef BSD
#include <sys/wait.h>
#else
#ifndef SYSV
#include <wait.h>
#endif SYSV
#endif BSD

#define ever (;;)
#define MAXARGS		100
#define isdelimeter(c)	(index(" \t;|", c))

char *alias_expand(), *hist_expand(), *reference_hist(), *hist_from_str();
char **calloc();

struct history {
    int histno;
    char **argv;
    struct history *prev;
    struct history *next;
};
static struct history *hist_head, *hist_tail;
struct history *malloc();
#define NULL_HIST	(struct history *)0

static char *last_aliased;
static int hist_size, print_only;

do_loop()
{
    register char *p, **argv;
    char	  **last_argv = DUBL_NULL, line[256];
    int   	  argc, c = (iscurses - 1);
    struct history *new;
#ifdef CURSES
    int		  save_echo_flg = FALSE;
#endif CURSES

    /* catch the right signals -- see main.c for other signal catching */
    (void) signal(SIGINT, catch);
    (void) signal(SIGQUIT, catch);
    (void) signal(SIGHUP, catch);
    (void) signal(SIGTERM, catch);
    (void) signal(SIGCHLD, sigchldcatcher);
    (void) signal(SIGPIPE, SIG_IGN); /* if pager is terminated before end */

    turnoff(glob_flags, IGN_SIGS);
    if (hist_size == 0) /* if user didn't set history in .rc file */
	hist_size = 1;

    for ever {
	if (setjmp(jmpbuf)) {
	    Debug("jumped back to main loop (%s: %d)\n", __FILE__,__LINE__);
#ifdef CURSES
	    if (c > 0) { /* don't pass last command back to curses_command() */
		iscurses = TRUE;
		c = hit_return();
	    }
#endif CURSES
	}
#ifdef CURSES
	if (iscurses || c > -1) {
	    /* if !iscurses, we know that we returned from a curses-based
	     * call and we really ARE still in curses. Reset tty modes!
	     */
	    if (ison(glob_flags, ECHO_FLAG)) {
		turnoff(glob_flags, ECHO_FLAG);
		echo_off();
		save_echo_flg = TRUE;
	    }
	    if (!iscurses) {
		iscurses = TRUE;
		c = hit_return();
	    }
	    if ((c = curses_command(c)) == -1 && save_echo_flg) {
		echo_on();
		turnon(glob_flags, ECHO_FLAG);
		save_echo_flg = FALSE;
	    }
	    continue;
	}
#endif CURSES
	clear_msg_list(msg_list);
	(void) check_new_mail();

	/* print a prompt according to printf like format:
	 * (current message, deleted, unread, etc) are found in mail_status.
	 */
	mail_status(1);
	if (Getstr(line, 256, 0) > -1)
	    p = line;
	else {
	    if (p = do_set(set_options, "ignoreeof")) {
		if (!*p)
		    continue;
		else
		    p = strcpy(line, p); /* so processing won't destroy var */
	    } else {
		(void) quit(0, DUBL_NULL);
		continue; /* quit may return if new mail arrives */
	    }
	}

	skipspaces(0);
	if (!*p && !(p = do_set(set_options, "newline"))) {
	    (void) readmsg(0, DUBL_NULL, msg_list);
	    continue;
	}
	if (!*p) /* if newline is set, but no value, then continue */
	    continue;

	/* upon error, argc = -1 -- still save in history so user can
	 * modify syntax error. if !argv, error is too severe.  We pass
	 * the last command typed in last_argv for history reference, and
	 * get back the current command _as typed_ (unexpanded by aliases
	 * or history) in last_argv.
	 */
	if (!(argv = make_command(p, &last_argv, &argc)))
	    continue;
	/* now save the new argv in the newly created history structure */
	if (!(new = malloc(sizeof (struct history))))
	    error("can't increment history");
	else {
	    new->histno = ++hist_no;
	    new->argv = last_argv; /* this is the command _as typed_ */
	    new->next = NULL_HIST;
	    new->prev = hist_head;
	    /* if first command, the tail of the list is "new" because
	     * nothing is in the list.  If not the first command, the
	     * head of the list's "next" pointer points to the new command.
	     */
	    if (hist_head)
		hist_head->next = new;
	    else
		hist_tail = new;
	    hist_head = new;
	}
	/*
	 * truncate the history list to the size of the history.
	 * Free the outdated command (argv) and move the tail closer to front.
	 * use a while loop in case the last command reset histsize to "small"
	 */
	while (hist_head->histno - hist_tail->histno >= hist_size) {
	    hist_tail = hist_tail->next;
	    free_vec(hist_tail->prev->argv);
	    xfree(hist_tail->prev);
	    hist_tail->prev = NULL_HIST;
	}

	if (print_only) {
	    print_only = 0;
	    free_vec(argv);
	} else if (argc > -1)
	    (void) do_command(argc, argv, msg_list);
    }
}

/* make a command from "buf".
 * first, expand history references. make an argv from that and save
 * in last_argv (to be passed back and stored in history). After that,
 * THEN expand aliases. return that argv to be executed as a command.
 */
char **
make_command(start, last_argv, argc)
register char *start, ***last_argv;
int *argc;
{
    register char *p, **tmp;
    char buf[BUFSIZ];

    if (!last_argv)
	tmp = DUBL_NULL;
    else
	tmp = *last_argv;
    /* first expand history -- (here's where argc gets set)
     * pass the buffer, the history list to reference if \!* (or whatever)
     * result in static buffer (pointed to by p) -- even if history parsing is
     * ignored, do this to remove \'s behind !'s and verifying matching quotes
     */
    if (!(p = hist_expand(start, tmp, argc)) || Strcpy(buf, p) > BUFSIZ)
	return DUBL_NULL;
    /* if history was referenced in the command, echo new command */
    if (*argc)
	puts(buf);

    /* argc may == -1; ignore this error for now but catch it later */
    if (!(tmp = mk_argv(buf, argc, 0)))
	return DUBL_NULL;

    /* save this as the command typed */
    if (last_argv)
	*last_argv = tmp;

    /* expand all aliases (recursively)
     * pass _this_ command (as typed and without aliases) to let aliases
     * with "!*" be able to reference the command line just typed.
     */
    if (alias_stuff(buf, *argc, tmp) == -1)
	return DUBL_NULL;

    /* now, expand variable references and make another argv */
    if (!variable_expand(buf))
	return DUBL_NULL;

    if (!last_argv)
	free_vec(tmp);

    /* with everything expanded, build final argv from new buffer
     * Note that backslashes and quotes still exist. Those are removed
     * because argument final is 1.
     */
    tmp = mk_argv(buf, argc, 1);
    return tmp;
}

/*
 * do the command specified by the argument vector, argv.
 * First check to see if argc < 0. If so, someone called this
 * command and they should not have! make_command() will return
 * an argv but it will set argc to -1 if there's a sytanx error.
 */
do_command(argc, argv, list)
char **argv, list[];
{
    register char *p;
    char **tmp = argv;
    int i, status;
    long do_pipe = ison(glob_flags, DO_PIPE);

    turnoff(glob_flags, IS_PIPE);

    if (argc <= 0) {
	turnoff(glob_flags, DO_PIPE);
	return -1;
    }

    clear_msg_list(list);

    for (i = 0; do_pipe >= 0 && argc; argc--) {
	p = argv[i];
	if (!strcmp(p, "|") || !strcmp(p, ";")) {
	    if (do_pipe = (*p == '|'))
		turnon(glob_flags, DO_PIPE);
	    argv[i] = NULL;
	    /* if piping, then don't call next command if this one fails. */
	    if ((status = exec_argv(i, argv, list)) <= -1 && do_pipe) {
		print("Broken pipe.\n");
		do_pipe = -1, turnoff(glob_flags, DO_PIPE);
	    }
	    /* if command failed and piping, or command worked and not piping */
	    if (do_pipe <= 0)
		status = 0, clear_msg_list(list);
	    /* else command worked and piping: set is_pipe */
	    else if (!status)
		turnon(glob_flags, IS_PIPE), turnoff(glob_flags, DO_PIPE);
	    argv[i] = p;
	    argv += (i+1);
	    i = 0;
	} else
	    i++;
    }
    if (do_pipe >= 0)
	status = exec_argv(i, argv, list);
    Debug("freeing: "), print_argv(tmp);
    free_vec(tmp);
    turnoff(glob_flags, DO_PIPE);
    return status;
}

exec_argv(argc, argv, list)
register char **argv, list[];
{
    register int n;

    if (!argv || !*argv || **argv == '\\' && !*++*argv) {
	if (ison(glob_flags, IS_PIPE) || ison(glob_flags, DO_PIPE))
	    print("Invalid null command.\n");
	return -1;
    }
    Debug("executing: "), print_argv(argv);

    /* if interrupted during execution of a command, return -1 */
    if (isoff(glob_flags, IGN_SIGS) && setjmp(jmpbuf)) {
	Debug("jumped back to exec_argv (%s: %d)\n", __FILE__, __LINE__);
	return -1;
    }

    /* standard commands */
    for (n = 0; cmds[n].command; n++)
	if (!strcmp(argv[0], cmds[n].command))
	    return (*cmds[n].func)(argc, argv, list);

    /* ucb-Mail compatible commands */
    for (n = 0; ucb_cmds[n].command; n++)
	if (!strcmp(argv[0], ucb_cmds[n].command))
	    return (*ucb_cmds[n].func)(argc, argv, list);

    /* for hidden, undocumented commands */
    for (n = 0; hidden_cmds[n].command; n++)
	if (!strcmp(argv[0], hidden_cmds[n].command))
	    return (*hidden_cmds[n].func)(argc, argv, list);

#ifdef SUNTOOL
    /* check tool-only commands */
    if (istool)
	for (n = 0; fkey_cmds[n].command; n++)
	    if (!strcmp(argv[0], fkey_cmds[n].command))
		return (*fkey_cmds[n].func)(argc, argv);
#endif SUNTOOL

    if ((isdigit(**argv) || index("^.*$-`{}", **argv))
			&& (n = get_msg_list(argv, list)) != 0) {
	if (n > 0 && isoff(glob_flags, DO_PIPE))
	    for (n = 0; n < msg_cnt; n++)
		if (msg_bit(list, n)) {
		    display_msg((current_msg = n), (long)0);
		    unset_msg_bit(list, n);
		}
	return 0;
    } else if (strlen(*argv) == 1 && index("$^.", **argv)) {
	if (!msg_cnt)
	    print("No messages.");
	else {
	    if (**argv != '.')
		current_msg = (**argv == '$') ? msg_cnt-1 : 0;
	    set_msg_bit(list, current_msg);
	    display_msg(current_msg, (long)0);
	}
	return 0;
    }
    /* get_msg_list will set the current message bit if nothing parsed */
    unset_msg_bit(list, current_msg);

    if (!istool && do_set(set_options, "unix")) {
	if (ison(glob_flags, IS_PIPE) || ison(glob_flags, DO_PIPE))
	    print("There is no piping to or from UNIX commands.\n");
	else
	    execute(argv);  /* try to execute a unix command */
	return -1; /* doesn't affect messages! */
    }

    print("%s: command not found.\n", *argv);
    if (!istool)
	print("type '?' for valid commands, or type `help'\n");
    return -1;
}

/* recursively look for aliases on a command line.  aliases may
 * reference other aliases.
 */
alias_stuff(b, argc, Argv)
register char 	*b, **Argv;
{
    register char 	*p, **argv = DUBL_NULL;
    register int 	n = 0, i = 0, Argc;
    static int 		loops;
    int 		dummy;

    if (++loops == 20) {
	print("Alias loop.\n");
	return -1;
    }
    for (Argc = 0; Argc < argc; Argc++) {
	register char *h = Argv[n + ++i];
	register char *p2 = "";

	/* we've hit a command separator or the end of the line */
	if (h && strcmp(h, ";") && strcmp(h, "|"))
	    continue;

	/* create a new argv containing this (possible subset) of argv */
	if (!(argv = calloc((unsigned)(i+1), sizeof (char *))))
	    continue;
	while (i--)
	    strdup(argv[i], Argv[n+i]);

	if ((!last_aliased || strcmp(last_aliased, argv[0]))
			&& (p = alias_expand(argv[0]))) {
	    /* if history was referenced, ignore the rest of argv
	     * else copy all of argv onto the end of the buffer.
	     */
	    if (!(p2 = hist_expand(p, argv, &dummy)))
		break;
	    if (!dummy)
		(void) argv_to_string(p2+strlen(p2), argv+1);
	    if (Strcpy(b, p2) > BUFSIZ) {
		print("Not enough buffer space.\n");
		break;
	    }
	    /* release old argv and build a new one based on new string */
	    free_vec(argv);
	    if (!(argv = mk_argv(b, &dummy, 0)))
		break;
	    if (alias_stuff(b, dummy, argv) == -1)
		break;
	} else
	    b = argv_to_string(b, argv);
	xfree(last_aliased), last_aliased = NULL;
	free_vec(argv);
	b += strlen(b);
	if (h) {
	    p2 = h;
	    while (++Argc < argc && (h = Argv[Argc]))
		if (strcmp(h, ";") && strcmp(h, "|"))
		    break;
	    b += strlen(sprintf(b, " %s ", p2));
	    n = Argc--;
	}
	i = 0;
    }
    xfree(last_aliased), last_aliased = NULL;
    --loops;
    if (Argc < argc) {
	free_vec(argv);
	return -1;
    }
    return 0;
}

char *
alias_expand(cmd)
register char *cmd;
{
    register char *p;
    register int x;

    if (!(p = do_set(functions, cmd)))
	return NULL;
    last_aliased = savestr(cmd); /* to be freed elsewhere; don't strdup! */
    if (isoff(glob_flags, WARNING))
	return p;
    for (x = 0; cmds[x].command; x++)
	if (!strcmp(cmd, cmds[x].command)) {
	    wprint("(real command: \"%s\" aliased to \"%s\")\n", cmd, p);
	    return p;
	}
    for (x = 0; ucb_cmds[x].command; x++)
	if (!strcmp(cmd, ucb_cmds[x].command)) {
	    wprint("(ucb-command: \"%s\" aliased to \"%s\")\n", cmd, p);
	    return p;
	}
    return p;
}

/* expand history references and separate message lists from other tokens */
char *
hist_expand(str, argv, hist_was_referenced)
register char *str, **argv;
register int *hist_was_referenced;
{
    static char   buf[BUFSIZ];
    register int  b = 0, inquotes = 0;
    int 	  first_space = 0, ignore_bang;

    ignore_bang = (ison(glob_flags, IGN_BANG) ||
		   do_set(set_options, "ignore_bang"));

    if (hist_was_referenced)
	*hist_was_referenced = 0;
    while (*str) {
	while (!inquotes && isspace(*str))
	    str++;
	do  {
	    if (!*str)
		break;
	    if (b >= BUFSIZ-1) {
		print("argument list too long.\n");
		return NULL;
	    }
	    if ((buf[b] = *str++) == '\'') {
		/* make sure there's a match! */
		inquotes = !inquotes;
	    }
	    if (!first_space && !inquotes && index("0123456789{}*$", buf[b])
			     && b && !index("0123456789{}- \t", buf[b-1])) {
		buf[b+1] = buf[b];
		buf[b++] = ' ';
		while ((buf[++b] = *str++) && index("0123456789-,${}", buf[b]))
		    ;
		if (!buf[b])
		    str--;
		first_space++;
	    }
	    /* check for (;) (|) or any other delimeter and separate it from
	     * other tokens.
	     */
	    if (!inquotes && buf[b] != '\0' && isdelimeter(buf[b])) {
		if (b && !isspace(buf[b-1]))
		    buf[b+1] = buf[b], buf[b++] = ' ';
		b++;
		break;
	    }
	    /* if double-quotes, just copy byte by byte, char by char ... */
	    if (buf[b] == '"') {
		int B = b;
		while ((buf[++B] = *str++) && buf[B] != '"')
		    ;
		if (buf[B])
		    b = B;
		else
		    str--;
		b++;
		continue;
	    }
	    if (buf[b] == '\\') {
		if ((buf[++b] = *str) == '!')
		    buf[--b] = '!';
		++str;
	    } else if (buf[b] == '!' && *str && *str != '\\' && !isspace(*str)
			&& !ignore_bang) {
		char word[BUFSIZ];
		if (!(str = reference_hist(str, word, argv)))
		    return NULL;
		if (hist_was_referenced)
		    *hist_was_referenced = 1;
		if (strlen(word) + b >= BUFSIZ) {
		    print("argument list too long.\n");
		    return NULL;
		}
		b += Strcpy(&buf[b], word) - 1;
	    }
	    b++;
	} while (*str && !isdelimeter(*str));
	if (!inquotes)
	    first_space++, buf[b++] = ' ';
    }
    buf[b] = 0;
    return buf;
}

/*
 * find mush variable references and expand them to their values.
 * variables are preceded by a '$' and cannot be within single
 * quotes.  Only if expansion has been made do we copy buf back into str.
 * RETURN 0 on failure, 1 on success.
 */
variable_expand(str)
register char *str;
{
    register int     b = 0;
    char             buf[BUFSIZ], *start = str;
    int		     expanded = 0;

    while (*str) {
	if (*str == '~' && (str == start || isspace(*(str-1)))) {
	    register char *p = any(str, " \t"), *tmp;
	    int x = 1;
	    if (p)
		*p = 0;
	    tmp = getpath(str, &x);
	    /* if error, print message and return 0 */
	    if (x == -1) {
		wprint("%s: %s\n", str, tmp);
		return 0;
	    }
	    b += Strcpy(buf+b, tmp);
	    if (p)
		*p = ' ', str = p;
	    else
		str += strlen(str);
	    expanded = 1;
	}
	/* if single-quotes, just copy byte by byte, char by char ... */
	if ((buf[b] = *str++) == '\'') {
	    while ((buf[++b] = *str++) && buf[b] != '\'')
		;
	    if (!buf[b])
		str--;
	}
	/* If $ is eoln, continue.  Variables must start with a `$'
	 * and continue with {, _, a-z, A-Z or it is not a variable.      }
	 */
	if (buf[b] == '$' &&
	    (isalpha(*str) || *str == '{' || *str == '_'))  /* } */  {
	    register char c, *p, *var, *end;

	    if (*(end = var = str) == '{')  /* } */   {
		if (!isalpha(*++str) && *str != '_') {
		    print("Illegal variable name.\n");
		    return 0;
		}
		if (!(end = index(var, '}'))) /* { */   {
		    print("Unmatched '{'.\n"); /* } */
		    return 0;
		}
		*end++ = 0;
	    } else
		while (isalnum(*++end) || *end == '_')
		    ;
	    /* advance "str" to the next parse-point, replace the end
	     * of "var" (end) with a null, and save char in `c'
	     */
	    c = *(str = end), *end = 0;

	    /* get the value of the variable. */
	    if (p = do_set(set_options, var))
		b += Strcpy(buf+b, p);
	    else {
		print("%s: undefined variable\n", var);
		return 0;
	    }
	    expanded = 1;
	    *str = c; /* replace the null with the old character */
	} else
	    b++;
    }
    buf[b] = 0;
    if (expanded) /* if any expansions were done, copy back into orig buf */
	(void) strcpy(start, buf);
    return 1;
}

/* make an vector of space delimeted character strings out of string "str".
 * place in "argc" the number of args made. If final is true, then remove
 * quotes and backslants according to standard.
 */
char **
mk_argv(str, argc, final)
register char *str;
register int *argc;
{
    register char	*s, *p;
    register int	tmp, err = 0;
    char		*newargv[MAXARGS], **argv, *p2, c;

    *argc = 0;
    while (*str) {
	while (isspace(*str))
	    ++str;
	if (*str) {		/* found beginning of a word */
	    s = p = str;
	    do  {
		if (p - s >= BUFSIZ-1) {
		    print("argument list too long.\n");
		    return DUBL_NULL;
		}
		if ((*p = *str++) == '\\') {
		    if (final && (*str == ';' || *str == '|'))
			/* make ";" look like " ;" */
			*p = ' ';
		    if (*++p = *str) /* assign and compare to NULL */
			str++;
		    continue;
		}
		if (p2 = index("\"'", *p)) {
		    register char c2 = *p2;
		    /* you can't escape quotes inside quotes of the same type */
		    if (!(p2 = index(str, c2))) {
			if (final)
			    print("Unmatched %c.\n", c2);
			err++;
			p2 = str;
		    }
		    tmp = (int)(p2 - str) + 1; /* take upto & include quote */
		    (void) strncpy(p + !final, str, tmp);
		    p += tmp - 2 * final; /* change final to a boolean */
		    if (*(str = p2))
			str++;
		}
	    } while (++p, *str && !isdelimeter(*str));
	    if (c = *str) /* set c = *str, check for null */
		str++;
	    *p = 0;
	    if (*s) {
		newargv[*argc] = savestr(s);
		(*argc)++;
	    }
	    *p = c;
	}
    }
    if (!*argc)
	return DUBL_NULL;
    /* newargv[*argc] = NULL; */
    if (!(argv = calloc((unsigned)((*argc)+1), sizeof(char *)))) {
	perror("mk_argv: calloc");
	return DUBL_NULL;
    }
    for (tmp = 0; tmp < *argc; tmp++)
	argv[tmp] = newargv[tmp];
    if (err)
	*argc = -1;
    return argv;
}

/*
 * reference previous history from syntax of str and place result into buf
 * We know we've got a history reference -- we're passed the string starting
 * the first char AFTER the '!' (which indicates history reference)
 */
char *
reference_hist(str, buf, hist_ref)
register char *str, *buf, **hist_ref;
{
    int 	   relative = *str == '-'; /* relative from current hist_no */
    int 	   old_hist, argstart = 0, lastarg, argend = 0, n = 0;
    register char  *p, **argv = hist_ref;
    struct history *hist;

    buf[0] = 0;
    if (index("!:$*", *str)) {
	old_hist = hist_no;
	if (*str == '!')
	    str++;
    } else if (isdigit(*(str + relative)))
	str = my_atoi(str + relative, &old_hist);
    else if (!(p = hist_from_str(str, &old_hist)))
	return NULL;
    else
	str = p;
    if (relative)
	old_hist = (hist_no-old_hist) + 1;
    if (old_hist == hist_no) {
	if (!(argv = hist_ref))
	    print("You haven't done anything yet!\n");
    } else {
	if (old_hist <= hist_no-hist_size || old_hist > hist_no ||
	    old_hist <= 0) {
	    if (old_hist <= 0)
		print("You haven't done that many commands, yet.\n");
	    else
		print("Event %d %s.\n", old_hist,
		    (old_hist > hist_no)? "hasn't happened yet": "expired");
	    return NULL;
	}
	hist = hist_head;
	while (hist && hist->histno != old_hist)
	    hist = hist->prev;
	if (hist)
	    argv = hist->argv;
    }
    if (!argv)
	return NULL;
    while (argv[argend+1])
	argend++;
    lastarg = argend;
    if (*str && index(":$*", *str)) {
	int isrange;
	if (*str == ':' && isdigit(*++str))
	    str = my_atoi(str, &argstart);
	if (isrange = (*str == '-'))
	    str++;
	if (!isdigit(*str)) {
	    if (*str == 'p')
		str++, print_only = 1;
	    else if (!*str || isdelimeter(*str))
		argend = argstart;
	    else {
		if (*str == '*')
		    if (argv[0])
			argstart = 1, argend = ++lastarg;
		    else
			argstart = 0;
		else if (*str == '$' || !isrange)
		    argstart = argend;
		else
		    print("%c: unknown arguement selector.\n", *str);
		str++;
	    }
	} else
	    str = my_atoi(str, &argend);
    }
    if (argstart > lastarg || argend > lastarg || argstart > argend) {
	print("Bad argument selector.\n");
	return NULL;
    }
    while (argstart <= argend) {
	n += Strcpy(&buf[n], argv[argstart++]);
	buf[n++] = ' ';
    }
    buf[--n] = 0;
    return str;
}

/* find a history command that contains the string "str"
 * place that history number in "hist" and return the end of the string
 * parsed: !?foo (find command with "foo" in it) !?foo?bar (same, but add "bar")
 * in the second example, return the pointer to "bar"
 */
char *
hist_from_str(str, hist_number)
register char *str;
register int *hist_number;
{
    register char *p = NULL, c = 0;
    int 	  full_search = 0, len, found;
    char 	  buf[BUFSIZ];
    struct history *hist;
#ifndef SYSV
    extern char   *re_comp();
#else
    extern char   *regcmp();
#endif SYSV

    if (*str == '?') {
	if (p = index(++str, '?'))
	    *p++ = 0;
	else
	    p = str + strlen(str);
	full_search = 1;
    } else if (*str == '{')
	if (!(p = index(str, '}'))) {   /* { */
	    print("Unmatched '}'");
	    return NULL;
	} else
	    *p++ = 0, ++str;
    else
	p = str;
    while (*p && *p != ':' && !isspace(*p))
	p++;
    c = *p, *p = 0;
    if (*str) {
#ifndef SYSV
	if (re_comp(str))
#else
	if (!regcmp(str, NULL))
#endif SYSV
	    return NULL;
    } else {
	*hist_number = hist_no;
	return p;
    }
    len = strlen(str);
    /* move thru the history in reverse searching for a string match. */
    for (hist = hist_head; hist; hist = hist->prev) {
	if (full_search) {
	    (void) argv_to_string(buf, hist->argv);
	    Debug("Checking for (%s) in (#%d: %s)\n", str, hist->histno, buf);
	}
	if (!full_search) {
	    (void) strcpy(buf, hist->argv[0]);
	    Debug("Checking for (%s) in (#%d: %*s)\n",
		str, hist->histno, len, buf);
	    found = !strncmp(buf, str, len);
	} else
	    found =
#ifndef SYSV
		re_exec(buf)
#else
		!!regex(buf, NULL) /* convert to boolean value */
#endif SYSV
				== 1;
	if (found) {
	    *hist_number = hist->histno;
	    Debug("Found it in history #%d\n", *hist_number);
	    *p = c;
	    return p;
	}
    }
    print("%s: event not found\n", str);
    return NULL;
}

disp_hist(n, argv)  /* argc not used -- use space for the variable, "n" */
register int n;
char **argv;
{
    register int	list_num = TRUE, num_of_hists = hist_size;
    register int	reverse = FALSE;
    struct history	*hist = hist_tail;

    while (*++argv && *argv[0] == '-') {
	n = 1;
	do  switch(argv[0][n]) {
		case 'h': list_num = FALSE;
		when 'r': reverse = TRUE; hist = hist_head;
		otherwise: print("usage: history [-h] [-r] [#histories]\n");
			   return -1;
	    }
	while (argv[0][++n]);
    }
    if (*argv)
	if (!isdigit(**argv)) {
	    print("history: badly formed number\n");
	    return -1;
	} else
	    num_of_hists = atoi(*argv);

    if (num_of_hists > hist_size || num_of_hists > hist_no)
	num_of_hists = min(hist_size, hist_no);

    if (!reverse)
	while (hist_no - hist->histno > num_of_hists) {
	    printf("skipping %d\n", hist->histno);
	    hist = hist->next;
	}

    for (n = 0; n < num_of_hists && hist; n++) {
	if (list_num)
	    wprint("%4.d  ", hist->histno);
	print_argv(hist->argv);
	hist = (reverse)? hist->prev : hist->next;
    }
    return -1;
}

init_history(newsize)
{
    if ((hist_size = newsize) < 1)
	hist_size = 1;
}

