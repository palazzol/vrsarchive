/* bind.c */

#ifdef CURSES

#include "bindings.h"
#include "mush.h"

#define MAX_BIND_LEN 20   /* max length a string can be to bind to a command */

struct cmd_map {
    int m_cmd;  /* the command this is mapped to  */
    char *m_str;  /* the string user types (cbreak) */
    struct cmd_map *m_next;
} *cmd_map;

init_bindings()
{
    add_bind("g", 1);
    add_bind("w", 2);
    add_bind("W", 3);
    add_bind("s", 4);
    add_bind("S", 5);
    add_bind("c", 6);
    add_bind("C", 7);
    add_bind("d", 8);
    add_bind("D", 9);
    add_bind("u", 10);
    add_bind("U", 11);
    add_bind("\\CR", 12);
    add_bind("\\CL", 13);
    add_bind("j", 14), add_bind("J", 14), add_bind("\\n", 14), add_bind("+",14);
    add_bind("k", 15), add_bind("K", 15), add_bind("-",15), add_bind("\\CK",15);
    add_bind("^", 16);
    add_bind("$", 17);
    add_bind("{", 18);
    add_bind("}", 19);
    add_bind("z", 20);
    add_bind("Z", 21);
    add_bind("H", 22);
    add_bind("(", 23);
    add_bind(")", 24);
    add_bind("/", 25);
    add_bind("\\C_", 26);  /* this is really ^/ */
    add_bind("\\CN", 27);
    add_bind("\\CP", 28);
    add_bind("o" ,29);
    add_bind("O", 30);
    add_bind("Q", 31);
    add_bind("q", 32);
    add_bind("X", 33);
    add_bind("x", 34);
    add_bind("\\CU", 35);
    add_bind("f", 36);
    add_bind("!", 37);
    add_bind(":", 38);
    add_bind("|", 39);
    add_bind("%", 40);
    add_bind("v", 41);
    add_bind("i", 42);
    add_bind("a", 43);
    add_bind("h", 44);
    add_bind("V", 45);
    add_bind("M", 46);
    add_bind("m", 47);
    add_bind("r", 48);
    add_bind("R", 49);
    add_bind("t", 50), add_bind(".", 50), add_bind("p", 50);
    add_bind("T", 51);
    add_bind("n", 52);
    add_bind("b", 53);
    add_bind("B", 54);
    add_bind("?", 55); /* C_HELP Must be the last one! */
}

struct cmd_map map_func_names[] = {
    { C_NULL,		NULL,			NULL_MAP },
    { C_GOTO_MSG,	"goto msg",		NULL_MAP },
    { C_WRITE_MSG,	"write",		NULL_MAP },
    { C_WRITE_LIST,	"write list",		NULL_MAP },
    { C_SAVE_MSG,	"save",			NULL_MAP },
    { C_SAVE_LIST,	"save list",		NULL_MAP },
    { C_COPY_MSG,	"copy",			NULL_MAP },
    { C_COPY_LIST,	"copy list",		NULL_MAP },
    { C_DELETE_MSG,	"delete",		NULL_MAP },
    { C_DELETE_LIST,	"delete list",		NULL_MAP },
    { C_UNDEL_MSG,	"undelete",		NULL_MAP },
    { C_UNDEL_LIST,	"undelete list",	NULL_MAP },
    { C_REVERSE,	"reverse video",	NULL_MAP },
    { C_REDRAW,		"redraw",		NULL_MAP },
    { C_NEXT_MSG,	"next msg",		NULL_MAP },
    { C_PREV_MSG,	"back msg",		NULL_MAP },
    { C_FIRST_MSG,	"first msg",		NULL_MAP },
    { C_LAST_MSG,	"last msg",		NULL_MAP },
    { C_TOP_PAGE,	"top page",		NULL_MAP },
    { C_BOTTOM_PAGE,	"bottom page",		NULL_MAP },
    { C_NEXT_SCREEN,	"screen next",		NULL_MAP },
    { C_PREV_SCREEN,	"screen back",		NULL_MAP },
    { C_SHOW_HDR,	"show hdr",		NULL_MAP },
    { C_SOURCE,		"source",		NULL_MAP },
    { C_SAVEOPTS,	"saveopts",		NULL_MAP },
    { C_NEXT_SEARCH,	"search up",		NULL_MAP },
    { C_PREV_SEARCH,	"search down",		NULL_MAP },
    { C_CONT_SEARCH,	"search cont",		NULL_MAP },
    { C_PRESERVE,	"preserve",		NULL_MAP },
    { C_SORT,		"sort",			NULL_MAP },
    { C_REV_SORT,	"sort reverse",		NULL_MAP },
    { C_QUIT_HARD,	"quit!",		NULL_MAP },
    { C_QUIT,		"quit",			NULL_MAP },
    { C_EXIT_HARD,	"exit!",		NULL_MAP },
    { C_EXIT,		"exit",			NULL_MAP },
    { C_UPDATE,		"update",		NULL_MAP },
    { C_FOLDER,		"folder",		NULL_MAP },
    { C_SHELL_ESC,	"shell escape",		NULL_MAP },
    { C_CURSES_ESC,	"line mode",		NULL_MAP },
    { C_PRINT_MSG,	"lpr",			NULL_MAP },
    { C_CHDIR,		"chdir",		NULL_MAP },
    { C_VAR_SET,	"variable",		NULL_MAP },
    { C_IGNORE,		"ignore",		NULL_MAP },
    { C_ALIAS,		"alias",		NULL_MAP },
    { C_OWN_HDR,	"my hdrs",		NULL_MAP },
    { C_VERSION,	"version",		NULL_MAP },
    { C_MAIL_FLAGS,	"mail flags",		NULL_MAP },
    { C_MAIL,		"mail",			NULL_MAP },
    { C_REPLY_SENDER,	"reply",		NULL_MAP },
    { C_REPLY_ALL,	"reply all",		NULL_MAP },
    { C_DISPLAY_MSG,	"display",		NULL_MAP },
    { C_TOP_MSG,	"top",			NULL_MAP },
    { C_DISPLAY_NEXT,	"display next",		NULL_MAP },
    { C_BIND,		"bind",			NULL_MAP },
    { C_UNBIND,		"unbind",		NULL_MAP },
    { C_HELP,		"help",			NULL_MAP }
};

getcmd()
{
    char 		buf[MAX_BIND_LEN];
    register int 	c, m, match;
    register char	*p = buf;
    register struct cmd_map *list;

    bzero(buf, MAX_BIND_LEN);
    c = getchar();
    if (isdigit(c)) {
	(void) ungetc(c, stdin);
	return C_GOTO_MSG;
    }
    for (;; p += strlen(p), c = getchar()) {
	if (c == ESC)
	    (void) strcpy(buf, "\\E");
	else if (c == '\n' || c == '\r')
	    (void) strcpy(p, "\\n");
	else if (c == '\t')
	    (void) strcpy(p, "\\t");
	else if (iscntrl(c))
	    (void) sprintf(p, "\\C%c", upper(unctrl(c)[1]));
	else
	    *p = c;
	m = 0;
	for (list = cmd_map; list; list = list->m_next)
	    if ((match = prefix(buf, list->m_str)) == MATCH) {
		if (debug)
		    print("\"%s\" ", map_func_names[list->m_cmd].m_str);
		return list->m_cmd;
	    } else if (match != NO_MATCH)
		m++;
	if (m == 0) {
	    if (debug)
		print("No binding for \"%s\" found.", buf);
	    return 0;
	}
    }
}

/*
 * bind chars or strings to commands -- doesn't touch messages; return -1
 * for curses mode, return -2 to have curses command set cntd_cmd to
 * prevent screen refresh to allow user to read output in case of multilines.
 */
bind_it(len, argv)
char **argv;
{
    char buf[MAX_BIND_LEN], buf2[30];
    register int x;
    int (*oldint)(), (*oldquit)();
    int unbind = (argv && **argv == 'u');
    int ret = -1; /* return value */

    if (istool > 1 || argv && *++argv && !strcmp(*argv, "-?"))
	return help(0, "bind", cmd_help) - 1;

    on_intr();

    if (unbind) {
	if (!*argv) {
	    print("Unbind what? ");
	    if (Getstr(buf, MAX_BIND_LEN-1, 0) <= 0) {
		off_intr();
		return -1;
	    }
	} else
	    (void) strcpy(buf, *argv);
	if (!un_bind(buf))
	    print("\"%s\" isn't bound to a command.\n", buf);
	off_intr();
	return -1;
    }
    if (argv && *argv) {
	(void) strncpy(buf, *argv, MAX_BIND_LEN-1);
	if (!argv[1]) {
	    int binding = c_bind(*argv);
	    if (binding)
		print("\"%s\" is bound to \"%s\".\n",
			*argv, map_func_names[binding].m_str);
	    else
		print("\"%s\" isn't bound to a command.\n", *argv);
	    off_intr();
	    return -1;
	} else
	    argv++;
    } else {
	print("bind [<CR>=all, -?=help]: ");
	if ((len = Getstr(buf, MAX_BIND_LEN-1, 0)) == 0) {
	    if (iscurses)
		putchar('\n');
	    (void) c_bind(NULL);
	    off_intr();
	    return -2;
	}
	if (len < 0) {
	    off_intr();
	    return -1;
	}
    }
    /* if a binding was given on the command line */
    if (argv && *argv)
	(void) argv_to_string(buf2, argv);
    else {
	int binding;
	
	if (!strcmp(buf, "-?")) {
	    if (iscurses)
		clr_bot_line();
	    (void) help(0, "bind", cmd_help);
	    off_intr();
	    return -2;
	}

	binding = c_bind(buf);

	for (len = 0; len == 0; ) {
	    print("\"%s\" = <%s>: New function [<CR> for list]: ",
		buf, (binding? map_func_names[binding].m_str : "unset"));
	    len = Getstr(buf2, 29, 0);
	    if (iscurses)
		clr_bot_line();
	    if (len == 0) {
		char *maps[C_HELP+1], *p, *malloc();
		int n = 0;

		if (iscurses)
		    putchar('\n');
		for (x = 0; x < C_HELP; x++) {
		    if (!(x % 4))
			if (!(p = maps[n++] = malloc(81))) {
			    error("malloc in bind()");
			    free_vec(maps);
			    off_intr();
			    return -1;
			}
		    p += strlen(sprintf(p, "%-18.18s  ",
					map_func_names[x+1].m_str));
		}
		maps[n] = NULL;
		(void) help(0, maps, NULL);
		free_vec(maps);
		ret--;
	    }
	}
	/* if list was printed, ret < -1 -- tells cntd_cmd to be set and
	 * prevents screen from being refreshed (lets user read output
	 */
	if (len == -1) {
	    off_intr();
	    return ret;
	}
    }
    for (x = 1; x <= C_HELP; x++)
	if (!strcmp(buf2, map_func_names[x].m_str)) {
	    int add_to_ret;
	    if (debug)
		print("\"%s\" will execute \"%s\".\n", buf, buf2);
	    add_to_ret = do_bind(buf, map_func_names[x].m_cmd);
	    /* if do_bind hda no errors, it returned -1.  If we already
	     * messed up the screen, then ret is less than -1.  return the
	     * lesser of the two to make sure that cntd_cmd gets set right
	     */
	    off_intr();
	    return min(add_to_ret, ret);
	}
    print("\"%s\": Unknown function.\n", buf2);
    off_intr();
    return ret;
}

/*
 * print current key to command bindings if "str" is NULL.
 * else return the integer "m_cmd" which the str is bound to.
 */
c_bind(str)
register char *str;
{
    register struct cmd_map *opts;
    register int    incurses = iscurses;
    register FILE   *pp = NULL_FILE;

    if (!str && !istool) {
	echo_on();
	if (!(pp = popen(pager, "w")))
	    error(pager);
	else if (incurses)
	    clr_bot_line(), iscurses = FALSE;
	turnon(glob_flags, IGN_SIGS);
	if (pp)
	    fprintf(pp, "Current key to command bindings:\n\n");
	else
	    wprint("Current key to command bindings:\n\n");
    }

    for (opts = cmd_map; opts; opts = opts->m_next)
	if (!str)
	    if (pp)
		fprintf(pp, "%-20.20s %s\n",
		    opts->m_str, map_func_names[opts->m_cmd].m_str);
	    else
		wprint("%-20.20s %s\n",
		    opts->m_str, map_func_names[opts->m_cmd].m_str);
	else
	    if (strcmp(str, opts->m_str))
		continue;
	    else if (opts->m_cmd)
		return opts->m_cmd;
	    else
		return 0;
    if (pp)
	(void) pclose(pp);
    if (incurses)
	iscurses = TRUE;
    echo_off();
    turnoff(glob_flags, IGN_SIGS);
    return 0;
}

/*
 * doesn't touch messages: return -1.  Error output causes return < -1.
 */
do_bind(str, func)
register char *str;
{
    register struct cmd_map *list;
    register int match, ret = -1;

    (void) un_bind(str);
    for (list = cmd_map; list; list = list->m_next)
	if ((match = prefix(str, list->m_str)) == MATCH)
	    puts("Something impossible just happened."), ret--;
	else if (match == A_PREFIX_B)
	    printf("Warning: \"%s\" prefixes \"%s\" (%s)\n",
		str, list->m_str, map_func_names[list->m_cmd].m_str), ret--;
	else if (match == B_PREFIX_A)
	    printf("Warning: \"%s\" (%s) prefixes: \"%s\"\n",
		list->m_str, map_func_names[list->m_cmd].m_str, str), ret--;
    add_bind(str, func);
    /* errors decrement ret.  If ret returns less than -1, cntd_cmd is set
     * and no redrawing is done so user can see the warning signs
     */
    return ret;
}

add_bind(str, func)
register char *str;
{
    register struct cmd_map *tmp;
    struct cmd_map *calloc();

    /* now make a new option struct and set fields */
    if (!(tmp = calloc((unsigned)1, sizeof(struct cmd_map)))) {
	error("calloc");
	return;
    }
    tmp->m_next = cmd_map;
    cmd_map = tmp;

    tmp->m_str = savestr(str);
    tmp->m_cmd = func; /* strdup handles the NULL case */
}

un_bind(p)
register char *p;
{
    register struct cmd_map *list = cmd_map, *tmp;

    if (!list || !*list->m_str || !p || !*p)
	return 0;

    if (!strcmp(p, cmd_map->m_str)) {
	cmd_map = cmd_map->m_next;
	xfree (list->m_str);
	xfree((char *)list);
	return 1;
    }
    for ( ; list->m_next; list = list->m_next)
	if (!strcmp(p, list->m_next->m_str)) {
	    tmp = list->m_next;
	    list->m_next = list->m_next->m_next;
	    xfree (tmp->m_str);
	    xfree ((char *)tmp);
	    return 1;
	}
    return 0;
}

prefix(a, b)
register char *a, *b;
{
    while (*a && *b && *a == *b)
	a++, b++;
    if (!*a && !*b)
	return MATCH;
    if (!*a && *b)
	return A_PREFIX_B;
    if (*a && !*b)
	return B_PREFIX_A;
    return NO_MATCH;
}
#endif CURSES
