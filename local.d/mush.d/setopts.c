/* setopts.c	(c) copyright 1986 (Dan Heller) */

#include "mush.h"

/* add an option indicated by "set option[=value]" or by "alias name alias" 
 * function is recursive, so multilists get appended accordingly
 */
add_option(list, argv)
register struct options **list;
register char **argv;
{
    register struct options *tmp;
    struct options *calloc();
    register char *option, *value = NULL;

    if (!(option = *argv))
	return 1;
    /* check for one of three forms:
     * option=value    option = value  option= value
     * value can be in quotes to preserve whitespace
     */
    if (*++argv && !strcmp(*argv, "=")) {
	if (value = *++argv) /* example: "set foo = " */
	    ++argv;
    } else if (value = index(option, '=')) {
	/* option=value  strip into option="option" value="value"; (quotes?) */
	register char c, *p2;
	*value = 0; /* option is now a null termined `option' */
	if ((c = *++value) == '"' || c == '\'') {
	    *value++ = 0;
	    if (!(p2 = index(value, c))) {
		print("No matching %c for %s.\n", c, option);
		return 0;
	    } else
		*p2 = 0;
	} else if (!c) {  /* example: "set crt=" */
	    if (!*argv) {
		print("No value for %s.\n", option);
		return 0;
	    }
	    value = *argv++;
	}
    }
    /* check to see if option is already set by attempting to unset it */
    (void) un_set(list, option);

    /* now make a new option struct and set fields */
    if (!(tmp = calloc((unsigned)1, sizeof(struct options)))) {
	error("calloc");
	return -1;
    }
    tmp->option = savestr(option);
    tmp->value = savestr(value); /* strdup handles the NULL case */

    tmp->next = *list;
    *list = tmp;

    /* check for options which must have values or are used frequently */
    if (*list == set_options) {
#if defined(CURSES) || defined(SUNTOOL)
	if (!strcmp(tmp->option, "no_reverse"))
	    turnoff(glob_flags, REV_VIDEO);
	else
#endif /* CURSES || SUNTOOL */
	if (!strcmp(tmp->option, "prompt"))
	    prompt = (tmp->value)? tmp->value : DEF_PROMPT;
	else if (!strcmp(tmp->option, "escape"))
	    escape = (tmp->value)? tmp->value : DEF_ESCAPE;
	else if (!strcmp(tmp->option, "pager"))
	    pager = (tmp->value)? tmp->value : DEF_PAGER;
	else if (!strcmp(tmp->option, "editor"))
	    editor = (tmp->value)? tmp->value : DEF_EDITOR;
	else if (!strcmp(tmp->option, "hdr_format"))
	    hdr_format = (tmp->value)? tmp->value : DEF_HDR_FMT;
	else if (!strcmp(tmp->option, "visual"))
	    visual = (tmp->value)? tmp->value : DEF_EDITOR;
	else if (!strcmp(tmp->option, "crt")) {
	    if (!istool)
		crt = (tmp->value)? max(atoi(tmp->value), 1): 18;
	} else if (!strcmp(tmp->option, "screen")) {
	    screen = (tmp->value)? max(atoi(tmp->value), 1): 18;
#ifdef CURSES
	    if (iscurses && screen > LINES-2)
		screen = LINES-2;
#endif CURSES
	} else if (!strcmp(tmp->option, "history"))
	    init_history((value && *value)? atoi(value) : 1);
    }

    if (*argv)
	return add_option(list, argv);
    return 1;
}

/*
 * If str is NULL, just print options and their values. Note that numerical
 * values are not converted to int upon return.  If str is not NULL
 * return the string that matched, else return NULL;
 */
char *
do_set(list, str)
register struct options *list;
register char *str;
{
    register struct options *opts;
#ifdef SUNTOOL
    int x,y;

    if (istool && !str)
	y = 10 + 2 * l_height(LARGE);
#endif SUNTOOL

    for (opts = list; opts; opts = opts->next)
	if (!str) {
#ifdef SUNTOOL
	    if (istool)
		pw_text(msg_win, 5, y, PIX_SRC, fonts[DEFAULT], opts->option);
	    else
#endif SUNTOOL
		fputs(opts->option, stdout);
	    if (opts->value)
#ifdef SUNTOOL
		if (istool) {
		    x = 30*l_width(DEFAULT);
		    pw_text(msg_win, x,y, PIX_SRC, fonts[DEFAULT], opts->value);
		    pw_text(msg_win, x+1, y, PIX_SRC|PIX_DST,
			     fonts[DEFAULT], opts->value);
		    x += strlen(opts->value)*l_width(DEFAULT);
		} else
#endif SUNTOOL
		    printf("     \t%s", opts->value);
#ifdef SUNTOOL
	    if (istool)
		Clrtoeol(msg_win, x, y, DEFAULT), y += l_height(DEFAULT);
	    else
#endif SUNTOOL
	    putchar('\n');
	} else {
	    if (strcmp(str, opts->option))
		continue;
	    if (opts->value)
		return opts->value;
	    else
		return "";
	}
    /* if we still haven't matched, check for environment vars */
    if (str && list == set_options) {
	register int N, n;
	for (N = 0; environ[N]; N++) {
	    char *p = index(environ[N], '=');
	    if (p)
		*p = 0;
	    n = strcmp(str, environ[N]);
	    if (p)
		*p = '=';
	    if (!n)
		return p+1;
	}
    }
    return NULL;
}

/*
 * unset the variable described by p in the list "list".
 * if the variable isn't set, then return 0, else return 1.
 */
un_set(list, p)
register struct options **list;
register char *p;
{
    register struct options *opts = *list, *tmp;

    if (!list || !*list || !p || !*p)
	return 0;
    if (*list == set_options) {
#if defined(CURSES) || defined(SUNTOOL)
	if (!strcmp(p, "no_reverse"))
	    turnon(glob_flags, REV_VIDEO);
	else
#endif /* CURSES || SUNTOOL */
	if (!strcmp(p, "prompt"))
	    prompt = DEF_PROMPT;
	else if (!strcmp(p, "escape"))
	    escape = DEF_ESCAPE;
	else if (!strcmp(p, "pager"))
	    pager = DEF_PAGER;
	else if (!strcmp(p, "editor"))
	    editor = DEF_EDITOR;
	else if (!strcmp(p, "visual"))
	    visual = DEF_EDITOR;
	else if (!strcmp(p, "hdr_format"))
	    hdr_format = DEF_HDR_FMT;
	else if (!strcmp(p, "crt"))
	    crt = 18;
	else if (!strcmp(p, "screen")) {
	    screen = 18;
#ifdef CURSES
	    if (iscurses && screen > LINES-2)
		screen = LINES-2;
#endif CURSES
	} else if (!strcmp(p, "history"))
	    init_history(1);
    }

    if (!strcmp(p, opts->option)) {
	*list = (*list)->next;
	xfree (opts->option);
	if (opts->value)
	    xfree(opts->value);
	xfree((char *)opts);
	return 1;
    }
    for ( ; opts->next; opts = opts->next)
	if (!strcmp(p, opts->next->option)) {
	    tmp = opts->next;
	    opts->next = opts->next->next;
	    xfree (tmp->option);
	    if (tmp->value)
		xfree(tmp->value);
	    xfree ((char *)tmp);
	    return 1;
	}
    return 0;
}

/* The functions below return -1 since they don't affect
 * messages. This prevents piping from do_loop().
 */
set(n, argv)
register int n;
register char **argv;
{
    char firstchar = **argv;
    register char *cmd = *argv;
    register struct options **list;

    if (*cmd == 'u')
	cmd += 2;
    if (*++argv && !strcmp(*argv, "-?"))
	return help(0, (*cmd == 'i')? "ignore": "set", cmd_help);
    if (*argv && **argv == '?') {
	char buf[BUFSIZ];
	if (!strcmp(*argv, "?all")) {
	    FILE *pp = NULL_FILE;
	    turnon(glob_flags, IGN_SIGS);
	    echo_on();
	    if (!istool && !(pp = popen(pager, "w")))
		error(pager);
	    for (n = 0; variable_stuff(n, NULL, buf); n++)
		if (pp)
		    fprintf(pp, "%s\n", buf);
		else
		    print("%s\n", buf);
	    if (pp)
		pclose(pp);
	    echo_off();
	    turnoff(glob_flags, IGN_SIGS);
	} else {
	    (void) variable_stuff(0, (*argv)+1, buf);
	    wprint("%s\n", buf);
	}
	return -1;
    }

    if (firstchar == 'u') {
	if (!*argv)
	    print("%s what?\n", cmd);
	else {
	    list = (*cmd == 'i')? &ignore_hdr : &set_options;
	    do  if (!strcmp(*argv, "*"))
		    while (*list)
			(void) un_set(list, (*list)->option);
		else if (!un_set(list, *argv))
		    print("un%s: %s not set\n",
			(*cmd == 'i')? "ignore" : "set", *argv);
	    while (*++argv);
	}
	return -1;
    }
    if (!*argv)
	(void) do_set((*cmd == 'i')? ignore_hdr: set_options, NULL);
    else
	(void) add_option((*cmd == 'i')? &ignore_hdr: &set_options, argv);
    return -1;
}

alts(argc, argv)
register char **argv;
{
    char buf[256], buf2[256], *p;
    long save_bang = ison(glob_flags, IGN_BANG);

    if (argc && *++argv && !strcmp(*argv, "-?"))
	return help(0, "alts_help", cmd_help);
    if (argc > 1) {
	(void) argv_to_string(buf2, argv);
	(void) sprintf(buf, "set alternates=\"%s\"",  buf2);
	turnon(glob_flags, IGN_BANG);
	if (argv = make_command(buf, TRPL_NULL, &argc))
	    (void) do_command(argc, argv, msg_list);
	if (!save_bang)
	    turnoff(glob_flags, IGN_BANG);
    } else
	if (!(p = do_set(set_options, "alternates")))
	    print("No alternate hostnames set.\n");
	else
	    print("Alternate hostnames: %s\n", p);
    return -1;
}

save_opts(cnt, argv)
char **argv;
{
    char file[50], *tmp;
    register FILE *fp;

    if (cnt && *++argv && !strcmp(*argv, "-?"))
	return help(0, "source_help", cmd_help);
    if (cnt && *argv)
	(void) strcpy(file, *argv);
    else if (tmp = getenv("MAILRC"))
	(void) strcpy(file, tmp);
    else {
	char *home = do_set(set_options, "home");
	if (!home || !*home)
	    home = ALTERNATE_HOME;
	if (access(sprintf(file, "%s/%s", home, MAILRC), R_OK)
          && access(sprintf(file, "%s/%s", home, ALTERNATE_RC), R_OK))
	(void) strcpy(file, DEFAULT_RC);
    }

    cnt = 1;
    tmp = getpath(file, &cnt);
    if (cnt) {
	if (cnt == -1)
	    print("%s: %s\n", file, tmp);
	else
	    print("%s is a directory.\n", tmp);
	return -1;
    }
    if (!(fp = fopen(tmp, "w"))) {
	error("Can't open %s", file);
	return -1;
    }

    save_list("basic variable settings", set_options, "set", '=', fp);

    save_list("mail headers for outgoing mail", own_hdrs, "my_hdr", 0, fp);

    save_list("aliases", aliases, "alias", 0, fp);

    save_list("headers to ignore", ignore_hdr, "ignore", ' ', fp);

    save_list("command abbreviations", functions, "cmd", ' ', fp);

    save_list("command macros for function keys", fkeys, "fkey", ' ', fp);

    fclose(fp);
    print("All variables and options saved in %s\n", tmp);
    return -1;
}

save_list(title, list, command, equals, fp)
struct options *list;
register char *command, *title, equals;
register FILE *fp;
{
    register struct options *opts;
    register char *p;

    if (!list)
	return;
    fprintf(fp, "#\n# %s\n#\n", title);
    for (opts = list; opts; opts = opts->next) {
	fprintf(fp, "%s %s", command, opts->option);
	if (opts->value && *opts->value) {
	    register char *quote;
	    if (!equals)
		quote = NO_STRING;
	    else if (p = any(opts->value, "\"'"))
		if (*p == '\'') quote = "\"";
		else quote = "'";
	    else
		if (!any(opts->value, " \t;|"))
		    quote = NO_STRING;
		else quote = "'";
	    fputc(equals? equals: ' ', fp);
	    fprintf(fp, "%s%s%s", quote, opts->value, quote);
	}
	fputc('\n', fp);
    }
}
