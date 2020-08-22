/* @(#)pick.c	2.4	(c) copyright 10/18/86 (Dan Heller) */

#include "mush.h"

static int before, mdy[3], search_from, search_subj, search_to, xflg, icase;

do_pick(n, argv, list)
register int n;
register char **argv, list[];
{
    if (n > 1 && !strcmp(argv[1], "-?"))
	return help(0, "pick", cmd_help);

    /* if is_pipe, then the messages to search for are already set.
     * if not piped, then reverse the bits for all message numbers.
     * That is, search EACH message. only those matching will be returned.
     */
    if (isoff(glob_flags, IS_PIPE))
	bitput(list, list, msg_cnt, =~) /* macro, turn on all bits */
    if (pick(n, argv, list, isoff(glob_flags, DO_PIPE)) == -1)
	return -1;
    if (isoff(glob_flags, DO_PIPE)) {
	if (istool)
	    print("Messages: ");
	for (n = 0; n < msg_cnt; n++)
	    if (msg_bit(list, n))
		if (istool)
		    print_more("%d ", n+1);
		else
		    print("%s\n", compose_hdr(n));
    }
    return 0;
}

pick(ret, argv, list, verbose)
register int ret;
register char **argv, list[];
{
    register char c;
    int o_before = before, o_mdy[3], o_search_from = search_from,
	o_search_subj = search_subj, o_search_to = search_to, o_xflg = xflg, n;
    for (c = 0; c < 3; c++)
	o_mdy[c] = mdy[c];

    ret = -1;
    if (!msg_cnt) {
	print("No Messages.\n");
	goto bad;
    }

    icase = before = mdy[0] = search_from = search_subj = xflg = 0;
    while (*argv && *++argv && **argv == '-')
	switch(c = argv[0][1]) {
	    /* users specifies a range */
	    case 'r': {
		int X = 2;
		/* if not a pipe, then clear all bits cuz we only want
		 * to search the message specified here...
		 * If it is a pipe, then add to the messages searched for.
		 */
		if (isoff(glob_flags, IS_PIPE))
		    clear_msg_list(list);
		/*  "-r10-15"
		 *     ^argv[1][2]  if NULL, then
		 * list detached from "r" e.g. "-r" "5-20"
		 */
		if (!argv[0][X])
		    argv++, X = 0;
		(*argv) += X;
		n = get_msg_list(argv, list);
		(*argv) -= X;
		if (n == -1)
		    goto bad;
		argv += (n-1); /* we're going to increment another up top */
	    }
	    when 'd':
		if (!*++argv) {
		    print("specify a date for -%c\n", c);
		    goto bad;
		}
		if (!date1(*argv))
		    goto bad;
	    when 's' : case 'f': case 't':
		if (search_subj + search_from + search_to > 1) {
		    print("specify one of `s', `f' or `t' only\n");
		    goto bad;
	        }
	        if (c == 's')
		    search_subj = 1;
		else if (c == 'f')
		    search_from = 1;
		else
		    search_to = 1;
	    when 'x' : xflg = 1;
	    when 'i' : icase = 1;
	    otherwise:
		print("pick: unknown flag: %c\n", argv[0][1]);
		clear_msg_list(list);
		return -1;
	}
    if (verbose) {
	print_more("Searching for messages");
	if (mdy[1] == 0)
	    print(" that %s \"%s\"", (xflg)? "doesn't contain": "contains",
				(*argv)? *argv: "<previous expression>");
	if (search_subj)
	    print_more(" in subject line");
	else if (search_from)
	    print_more(" from author names");
	else if (search_to)
	    print_more(" from the To: field");
	if (mdy[1] > 0) {
	    extern char *month_names[]; /* from dates.c */
	    print_more(" dated on or %s %s. %d, 19%d.",
		  (before)? "before": "after",
		  month_names[mdy[0]], mdy[1], mdy[2]);
	}
	print_more("\n");
    }
    if (mdy[1] > 0 && icase)
	print("using date: -i flag ignored.\n");
    ret = find_pattern(*argv, list);
bad:
    before = o_before, search_from = o_search_from,
	search_subj = o_search_subj, search_to = o_search_to, xflg = o_xflg;
    for (c = 0; c < 3; c++)
	mdy[c] = o_mdy[c];
    return ret;
}

find_pattern(p, list)
register char *p, list[];
{
    register int n, val, i; /* val is return value from regex or re_exec */
    long bytes = 0;
    char buf[BUFSIZ];
    static char *err = (char *)-1;
#ifndef SYSV
    char *re_comp();
#else
    char *regcmp(), *regex();
#endif SYSV

    if (p && *p == '\\')
	p++;  /* take care of escaping special cases (`-', `\') */

    /* specify what we're looking for */
    if (p && *p) {
	if (icase)
	    p = lcase_strcpy(buf, p);
#ifndef SYSV
	if (err = re_comp(p)) {
	    print("re_comp error: %s\n", err);
	    clear_msg_list(list);
	    return -1;
	}
#else
	if (err && p)
	    xfree(err);
	if (p && !(err = regcmp(p, NULL))) {
	    print("regcmp error: %s\n", p);
	    clear_msg_list(list);
	    return -1;
	}
#endif SYSV
    } else if (err == (char *)-1 && mdy[1] <= 0) {
	print("No previous regular expression\n");
	clear_msg_list(list);  /* doesn't matter really */
	return -1;
    }
    /* start searching: set bytes, and message number: n */
    for (n = 0; n < msg_cnt; n++)
	if (msg_bit(list, n)) {
	    if (mdy[1] > 0) {
		int msg_mdy[3];
		if (!(p = msg_date(n))) {
		    unset_msg_bit(list, n);
		    continue; /* error: don't consider this message */
		}       /* yr mo da */
		(void) sscanf(p, "%2d%2d%2d",
		     &msg_mdy[2], &msg_mdy[0], &msg_mdy[1]);
		msg_mdy[0]--;
		Debug("checking %d's date: %d-%d-%d  ",
			     n+1, msg_mdy[0]+1, msg_mdy[1], msg_mdy[2]);
		/* start at year and wrap around.
		 * only when match the day (4), check for == (match)
		 */
		for (i = 2; i < 5; i++)
		    if (before && msg_mdy[i%3] < mdy[i%3]
			|| !before && msg_mdy[i%3] > mdy[i%3]
			|| i == 4 && (msg_mdy[i%3] == mdy[i%3])) {
			    Debug("matched (%s).\n",
				(i == 2)? "year" : (i == 3)? "month" : "day");
			    break;
		    } else if (msg_mdy[i%3] != mdy[i%3]) {
			Debug("failed.\n");
			unset_msg_bit(list, n);
			break;
		    }
		continue;
	    }
	    /* we must have the right date -- if we're searching for a
	     * string, find it.
	     */
	    (void) fseek(tmpf, msg[n].m_offset, L_SET);
	    bytes = 0;
	    while (bytes < msg[n].m_size) {
		if (!search_subj && !search_from && !search_to &&
				!(p = fgets(buf, BUFSIZ, tmpf)))
		    break;
		else if (search_subj) {
		    if (!(p = header_field(n, "subject"))) {
			unset_msg_bit(list, n);
			break;
		    }
		} else if (search_from) {
		    if (!(p = header_field(n, "from"))) {
			register char *p2;
			(void) fseek(tmpf, msg[n].m_offset, L_SET);
			p2 = fgets(buf, BUFSIZ, tmpf);
			p = index(p2, ' '), p++;
			if (p2 = any(p, " \t"))
			    *p2 = 0;
		    }
		} else if (search_to) {
		    if (!(p = header_field(n, "to"))) {
			unset_msg_bit(list, n);
			break;
		    }
		}
		if (icase)
		    p = lcase_strcpy(buf, p);
#ifndef SYSV
		val = re_exec(p);
#else
		val = !!regex(err, p, NULL); /* convert value to a boolean */
#endif SYSV
		if (val == -1) {   /* doesn't apply in system V */
		    print("Internal error for pattern search.\n");
		    clear_msg_list(list); /* it doesn't matter, really */
		    return -1;
		}
		if (xflg)
		    val = !val;
		if (!val) {
		    /* unset the bit if match failed. If it matches later
		     * in the search, we'll reset to true in the next iteration.
		     * this doesn't apply for author or subject picks.
		     */
		    unset_msg_bit(list, n);
		    bytes += strlen(p);
		} else {
		    set_msg_bit(list, n);
		    break;
		}
		if (search_subj || search_from || search_to)
		    break;
	    }
    }
    return 0;
}

#ifdef CURSES
/*
 * search for a pattern in composed message headers -- also see next function
 * flags ==  0   forward search (prompt).
 * flags == -1   continue search (no prompt).
 * flags ==  1   backward search (prompt).
 */
search(flags)
register int flags;
{
    register char   *p;
    char   	    pattern[128];
    register int    this_msg = current_msg, val = 0;
    static char     *err = (char *)-1, direction;
    int		    (*oldint)(), (*oldquit)();
#ifndef SYSV
    char *re_comp();
#else SYSV
    char *regcmp();
#endif SYSV

    if (msg_cnt <= 1) {
	print("Not enough messages to invoke a search.\n");
	return 0;
    }
    pattern[0] = '\0';
    if (flags == -1)
	print("continue %s search...", direction? "forward" : "backward");
    else
	print("%s search: ", flags? "backward" : "forward");
    if (flags > -1)
	if (Getstr(pattern, COLS-18, 0) < 0)
	    return 0;
	else
	    direction = !flags;
#ifndef SYSV
    if (err = re_comp(pattern)) {
	print(err);
	return 0;
    }
#else
    if (err && *pattern)
	xfree(err);
    else if (err == (char *)-1 && !*pattern) {
	print("No previous regular expression.");
	return 0;
    }
    if (*pattern && !(err = regcmp(pattern, NULL))) {
	print("Error in regcmp in %s", pattern);
	return 0;
    }
#endif SYSV
    move(LINES-1, 0), refresh();
    on_intr();

    do  {
	if (direction)
	    current_msg = (current_msg+1) % msg_cnt;
	else
	    if (--current_msg < 0)
		current_msg = msg_cnt-1;
        p = compose_hdr(current_msg);
#ifndef SYSV
	val = re_exec(p);
#else
	val = !!regex(err, p, NULL); /* convert value to a boolean */
#endif SYSV
	if (val == -1)     /* doesn't apply in system V */
	    print("Internal error for pattern search.\n");
    } while (!val && current_msg != this_msg && isoff(glob_flags, WAS_INTR));

    if (ison(glob_flags, WAS_INTR)) {
	print("Pattern search interruped.");
	current_msg = this_msg;
    } else if (val == 0)
	print("Pattern not found.");

    off_intr();
    return val;
}
#endif CURSES

/*
 * parse a user given date string and set mdy[] array with correct
 * values.  Return 0 on failure.
 */
date1(p)
register char *p;
{
    register char *p2;
    long	  t;
    int 	  i;
    struct tm 	  *today, *localtime();

    if (*p == '-') {
	before = 1;
	skipspaces(1);
    }
    if (!isdigit(*p) && *p != '/') {
	print("syntax error on date: \"%s\"\n", p);
	return 0;
    }
    (void) time (&t);
    today = localtime(&t);
    for (i = 0; i < 3; i++)
	if (!p || !*p || *p == '/') {
	    switch(i) {   /* default to today's date */
		case 0: mdy[0] = today->tm_mon;
		when 1: mdy[1] = today->tm_mday;
		when 2: mdy[2] = today->tm_year;
	    }
	    if (p && *p)
		p++;
	} else {
	    p2 = (*p)? index(p+1, '/') : NULL;
	    mdy[i] = atoi(p); /* atoi will stop at the '/' */
	    if (i == 0 && (--(mdy[0]) < 0 || mdy[0] > 11)) {
		print("Invalid month: %s\n", p);
		return 0;
	    } else if (i == 1 && (mdy[1] < 1 || mdy[1] > 31)) {
		print("Invalid day: %s\n", p);
		return 0;
	    }
	    if (p = p2) /* set p to p2 and check to see if it's valid */
		p++;
	}
    return 1;
}
