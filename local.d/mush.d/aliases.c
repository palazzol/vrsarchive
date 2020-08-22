/* (c) copyright @(#)aliases.c	2.4	10/15/86 (Dan Heller) */

#include "mush.h"

/*
 * do_alias handles aliases, header settings, functions, and fkeys.
 * since they're all handled in the same manner, the same routine is
 * used. argv[0] determines which to use.
 * alias is given here as an example
 *
 * alias           identify all aliases
 * alias name      identify alias
 * alias name arg1 arg2 arg3... -> name="arg1 arg2 arg3"; call set_option
 * unalias arg1 [arg2 arg3 ... ]        unalias args
 *
 * same is true for dealing with your own headers.
 * (also the expand command)
 * always return -1 since it nas no effect on messages
 */
do_alias(argc, argv)
register char **argv;
{
    register char *cmd = *argv, *p;
    struct options **list;
    char firstchar = *cmd, buf[BUFSIZ];

    if (argc == 0)
	return -1;
    if (firstchar == 'u')
	firstchar = cmd[2];
    if (*++argv && !strcmp(*argv, "-?")) { /* doesn't apply for fkeys */
	register char *help_str;
	if (firstchar == 'a' || firstchar == 'e')
	    help_str = "alias";
	else if (firstchar == 'c')
	    help_str = "func_help";
	else if (firstchar == 'f')
	    help_str = "fkey_help";
	else
	    help_str = "own_hdrs";
	return help(0, help_str, cmd_help);
    }

    if (firstchar == 'a')
	list = &aliases;
    else if (firstchar == 'c')
	list = &functions;
    else if (firstchar == 'f')
	list = &fkeys;
    else
	list = &own_hdrs;

    if (*cmd == 'u') {
	if (!*argv)
	    print("%s what?\n", cmd);
	/* unset a list separated by spaces or ',' */
	else while (*argv) {
	    if (!strcmp(*argv, "*")) /* unset everything */
		while (*list)
		    (void) un_set(list, (*list)->option);
	    else if (!un_set(list, *argv))
		print("\"%s\" isn't set\n", *argv);
	    argv++;
	}
	return -1;
    }

    if (!*argv && *cmd != 'e') {
	/* just type out all the aliases or own_hdrs */
	(void) do_set(*list, NULL);
	return -1;
    }

    if (*cmd == 'e') {   /* command was "expand" (aliases only) */
	if (!*argv)
	    print("expand which alias?\n");
	else
	    do  {
		print("%s: ", *argv);
		if (p = alias_to_address(*argv))
		    print("%s\n", p);
	    } while (*++argv);
	return -1;
    }

    /* at this point, *argv now points to a variable name ...
     * check for hdr -- if so, *argv better end with a ':' (check *p)
     */
    if (list == &own_hdrs && !(p = index(*argv, ':'))) {
	print("header labels must end with a ':' (%s)\n", *argv);
	return -1;
    }
    if (!argv[1] && !index(*argv, '='))
	if (p = do_set(*list, *argv))
	    print("%s\n", p);
	else
	    print("%s is not set\n", *argv);
    else {
	(void) argv_to_string(buf, argv);
	if ((p = any(buf, " \t=")) && *p != '=')
	    *p = '=';
	argv[0] = savestr(buf);
	argv[1] = NULL;
	(void) add_option(list, argv);
    }
    return -1;
}

/* takes string 's' which can be a name or list of names separated by
 * spaces or commas and checks to see if each is aliased to something else.
 * return address of the static buf.
 */
char *
alias_to_address(s)
register char *s;
{
    static char buf[BUFSIZ];
    register char *p = s, *p2, *tmp;
    char newbuf[BUFSIZ];
    static int recursive;

    if (!aliases)
	return s;
    if (!s || !*s) {
	print("No recipeints!?!\n");
	return NULL;
    }
    skipspaces(0);
    if (!recursive) {
	bzero(buf, BUFSIZ);
	p2 = buf;  /* if we're starting all this, p2 starts at &buf[0] */
    } else
	p2 = buf+strlen(buf);   /* else, pick up where we left off */

    if (++recursive == 30) {
	print("alias references too many addresses!\n");
	recursive = 0;
	return NULL;
    }
    /* find a comma, space, or newline -- if none exists, still go thru once */
    for (s = p; (p = any(s, ", \n")) || *s; s = p) {
	if (p)
	    *p++ = 0;  /* null terminate, comma/space/newline was found */
	else
	    for(p = s; *p; p++);   /* last in list; go to end of string */

	/* if this is an alias, recurse this routine to expand it out */
	if ((tmp = do_set(aliases, s)) && *tmp) {
	    if (!alias_to_address(strcpy(newbuf, tmp)))
		return NULL;
	    else
		p2 = buf+strlen(buf);
	/* Now, make sure the buffer doesn't overflow */
	} else if (strlen(s) + (p2-buf) + 2 > BUFSIZ) {  /* add " "  + NULL */
	    print("address length too long.\n");
	    recursive = 0;
	    return NULL;
	} else {
	    /* append the new alias (or unchanged address) onto the buffer */
	    p2 += Strcpy(p2, s);
	    p2 += Strcpy(p2, ", "); /* two Strcpy's is faster than sprintf */
	}
	skipspaces(0);
    }
    if (recursive)
	recursive--;
    if (!recursive)
	*(p2-2) = 0;  /* get rid of last ", " if end of recursion */
    return buf;
}
