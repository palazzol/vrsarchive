/* hdr_procs.c 	(c) copyright 1986 (Dan Heller) */

/* hdrs.h -- routines that look at message headers */
#include "mush.h"

/*
 * get which message via the offset and search for the headers which
 * match the string "str". there may be more than one of a field (like Cc:)
 * so get them all and "cat" them together into the static buffer
 * "buf" and return its address.
 */
char *
header_field(n, str)
char *str;
{
    static char    buf[BUFSIZ];
    char 	   tmp[BUFSIZ];
    register char  *p, *p2, *b = buf;
    int contd_hdr;  /* true if next line is a continuation of the hdr we want */

    if (fseek(tmpf, msg[n].m_offset, L_SET) == -1) {
	error("fseek in %s (msg %d, folder=%s)", tempfile, n+1, mailfile);
	turnon(glob_flags, READ_ONLY);
	return NULL;
    }
    *b = 0;
    while((p = fgets(tmp, BUFSIZ, tmpf)) && *p != '\n') {
	if (*p != ' ' && *p != '\t') {
	    contd_hdr = 0;
	    /* strcmp ignoring case */
	    for(p2 = str; *p && *p2 && *p2 == lower(*p); ++p, ++p2);
	    /* MATCH is true if p2 is at the end of str and *p is ':' */
	    if (*p2 || *p++ != ':')
		continue;
	    else
		contd_hdr = 1;
	} else if (!contd_hdr)
	    continue;
	skipspaces(0);
	p2 = no_newln(p);
	*++p2 = ' ', *++p2 = 0;
	b += Strcpy(b, p);
    }
    if (b > buf) /* now get rid of the trailing blank */
	*--b = 0;
    return (*buf)? buf: NULL;
}

do_hdrs(argc, argv, list)
register char **argv, list[];
{
    register int   pageful = 0, fnd;
    int 	   (*oldint)(), (*oldquit)(), show_deleted;
    static int     cnt;
    register char  *p;
    char 	   first_char = (argc) ? **argv: 'h';

    if (!msg_cnt) {
	if (isoff(glob_flags, DO_PIPE))
	    return;
#ifdef CURSES
	if (iscurses)
	    clear();
#endif CURSES
#ifdef SUNTOOL
	if (istool)
	    mail_status(0);
#endif SUNTOOL
	return 0;
    }
    if (first_char == ':' || (argc > 1 && argv[1][0] == ':')) {
	if (first_char != ':')
	    argv++;
	return specl_hdrs(argv, list);
    }

    if (argc > 1 && !strcmp(argv[1], "-?"))
	return help(0, "headers", cmd_help);

    on_intr();

    if (argc && (argv[0][1] == '+' || argc > 1 && !strcmp(argv[1], "+")) ||
	    first_char == 'z' && !argv[1])
	if (msg_cnt > screen)
	    cnt = min(msg_cnt - screen, n_array[0] + screen);
	else
	    cnt = 0;
    else if (argc && (argv[0][1] == '-' || argc > 1 && !strcmp(argv[1], "-")))
	cnt = max((cnt - 2*screen), 0);
    else if (argc && *++argv &&
	(isdigit(**argv) || **argv == '^' || **argv == '$')
	 || ison(glob_flags, IS_PIPE)) {
	/* if we're coming from a pipe, start display at the first msg bit
	 * set in the msg_list
	 */
	if (ison(glob_flags, IS_PIPE)) {
	    if (isoff(glob_flags, DO_PIPE))
		for (fnd = 0; fnd < msg_cnt; fnd++)
		    if (msg_bit(list, fnd))
			wprint("%s\n", compose_hdr(fnd));
	    off_intr();
	    return 0;
	}
	/* if a number was given, use it */
	if (!(fnd = chk_msg(*argv))) {
	    off_intr();
	    return -1;
	}
	for (cnt = fnd - 1; cnt > 0 && cnt + screen > msg_cnt; cnt--);
    } else if (current_msg < n_array[0] || current_msg > n_array[screen-1])
	cnt = current_msg; /* adjust if user reads passed screen bounds */
    else if (cnt >= msg_cnt || !argc || !*argv)
	cnt = max((cnt - screen), 0); /* adjust window to maintian position */

    show_deleted = !!do_set(set_options, "show_deleted");

    for (;pageful<screen && cnt<msg_cnt && isoff(glob_flags, WAS_INTR); cnt++) {
	if (!iscurses && !show_deleted && first_char == 'h'
	    && ison(msg[cnt].m_flags, DELETE))
	    continue;
	n_array[pageful++] = cnt;
	/* this message was displayed -- set the bit */
	if (list)
	    set_msg_bit(list, cnt);
	/* if do_pipe, don't output anything */
	if (ison(glob_flags, DO_PIPE))
	    continue;
	p = compose_hdr(cnt);
	if (!istool && (!iscurses || ison(glob_flags, IS_GETTING)))
	    puts(p);
#ifdef SUNTOOL
	else if (istool) {
	    if (cnt == current_msg) /* embolden or reverse_video */
		highlight(hdr_win, 0,pageful*l_height(DEFAULT), DEFAULT,p);
	    else
		pw_text(hdr_win, 0, pageful * l_height(DEFAULT), PIX_SRC,
							fonts[DEFAULT], p);
	    Clrtoeol(hdr_win, strlen(p)*l_width(DEFAULT),
		     pageful*l_height(DEFAULT), DEFAULT);
	}
#endif SUNTOOL
#ifdef CURSES
        else if (iscurses)
	    mvprintw(pageful, 0, "%-*s", COLS-2, p);
#endif CURSES
    }
    /* just in case a signal stopped us */
    off_intr();
    pageful++;
#ifdef CURSES
    if (iscurses && pageful < screen)
	move(pageful, 0), clrtobot();
#endif CURSES
    if (cnt == msg_cnt) {
	while (pageful <= screen) {
	    n_array[pageful-1] = msg_cnt+1; /* assign out-of-range values */
#ifdef SUNTOOL
	    if (istool)
		Clrtoeol(hdr_win, 0, pageful * l_height(DEFAULT), DEFAULT);
#endif SUNTOOL
	    ++pageful;
	}
    }
#ifdef SUNTOOL
    if (istool) {
	if (msg_cnt > screen) {
	    panel_set(next_scr, PANEL_SHOW_ITEM, TRUE, 0);
	    panel_set(prev_scr, PANEL_SHOW_ITEM, TRUE, 0);
	}
	mail_status(0);
    }
#endif SUNTOOL
    return 0;
}

#define NEW 1
#define ALL 2

specl_hdrs(argv, list)
char **argv, list[];
{
    u_long	special = 0;
    int 	n = 0;

    while (argv[0][++n])
	switch(argv[0][n]) {
	    case 'a': special = ALL;
	    when 'n': special = NEW;
	    when 'u': special = UNREAD;
	    when 'o': special = OLD;
	    when 'd': special = DELETE;
	    otherwise: print("choose from n,u,o,d, or a"); return -1;
	}
    if (debug)
	(void) check_flags(special);

    for (n = 0; n < msg_cnt; n++) {
	/*
	 * First, see if we're looking for NEW messages.
	 * If so, then check to see if the msg is unread and not old.
	 * If special > ALL, then special has a mask of bits describing
	 * the state of the message.
	 */
	if (ison(glob_flags, IS_PIPE)&& !msg_bit(list, n))
	    continue;
	if (special == ALL || special == NEW &&
	       (ison(msg[n].m_flags, UNREAD) && isoff(msg[n].m_flags, OLD))) {
	    if (isoff(glob_flags, DO_PIPE))
		print("%s\n", compose_hdr(n));
	    if (list)
		set_msg_bit(list, n);
#ifndef SYSV
	/*
	 * XENIX compiler can't handle "special" in ison() macro.
	 * It only works if the second argument is a constant!
	 */
	} else if (special > ALL && ison(msg[n].m_flags, special)) {
	    if (isoff(glob_flags, DO_PIPE))
		print("%s\n", compose_hdr(n));
	    if (list)
		set_msg_bit(list, n);
#endif SYSV
	} else {
	    if (list)
		unset_msg_bit(list, n);
	    Debug("msg[%d].m_flags: %d", n, msg[n].m_flags),
			 (void) check_flags(msg[n].m_flags);
	}
    }
    return 0;
}

char *
compose_hdr(cnt)
{
    static char buf[256];
    register char *p, *b, *status;
    char from[256], subject[256], date[17], lines[6], chars[6], line[256];
    char to[512], addr[256], name[256];

    /* status of the message */
    if (ison(msg[cnt].m_flags, DELETE))
	status = "*";
    else if (ison(msg[cnt].m_flags, OLD) && ison(msg[cnt].m_flags, UNREAD))
	status = "U";
    else if (ison(msg[cnt].m_flags, PRESERVE))
	status = "P";
    else if (isoff(msg[cnt].m_flags, UNREAD))
	status = " ";
    else
	status = "N";

    to[0] = from[0] = subject[0] = date[0] = lines[0] = chars[0] =
    addr[0] = name[0] = line[0] = 0;

    /* who's the message to */
    if (p = header_field(cnt, "to"))
	(void) strcpy(to, p);

    /* who the messages is from--
	%f		From field
	%a		From address
	%n		From name

	where %a is anything in angle brackets, or if no angle brackets,
	anything not in parenthesis, and %n is anything in parenthesis, or if
	nothing in parenthesis, anything not in angle brackets.
     */
    if (!(p = header_field(cnt, "from"))) {
	/* if all else fails, then get the first token in "From" line */
	register char *p2;
	p = ""; /* just in case */
	if (fseek(tmpf, msg[cnt].m_offset, L_SET) == -1 ||
	    !(p2 = fgets(line, BUFSIZ, tmpf))) {
	    error("fseek in %s (msg %d, folder=%s)", tempfile, cnt+1, mailfile);
	    turnon(glob_flags, READ_ONLY);
	} else if (!(p = index(p2, ' ')))
	    print("Fudged \"From\" line: %s", p2);
	else if (p2 = any(++p, " \t"))
	    *p2 = 0;
    }
    skipspaces(0);
    (void) no_newln(p);
    /* if the "from" line produced the user's login name, then the message is
     * from the user -- attempt to give more useful information by telling
     * to whom the message was sent.  This is not possible if the "to" header
     * failed to get info (which is probably impossible).
     */
    if (!strcmp(p, login) && *to)
	(void) sprintf(from, "TO: %s", to);
    else
	(void) strcpy(from, p);
    /* look for < or (  If <, then whatever is inside goes in addr, and
     * whatever isn't inside goes in name.  If it's a (, then whatever is
     * inside goes in name and whatever isn't, goes in addr.  It the same
     * type of thing, but exclusive of each other.
     */
    if (p = any(from, "<(")) {
	register char *p2, *out, *in;
	int diff;
	if (*p == '<')
	    out = name, in = addr;
	else
	    out = addr, in = name;

	(void) strcpy(in, p+1);
	if (p2 = index(in, (*p == '<')? '>' : ')'))
	    *p2 = 0;
	/* whatever isn't in <'s gets put in "name". Put p at end of name */
	diff = p - from;
	if (diff > 1) {
	    (void) strncpy(out, from, diff);
	    p = out + diff;
	} else
	    p = out;
	if (p2) {
	    /* p2 will be at >, so there will be at least a blank following */
	    while (isspace(*++p2));
	    /* copy remainder of line onto p (end of name) */
	    (void) strcpy(p, p2);
	}
    }

    (void) sprintf(lines, "%d", msg[cnt].m_lines);
    (void) sprintf(chars, "%d", msg[cnt].m_size);

    /* the date */
    if (p = msg_date(cnt))
	(void) strcpy(date, date_to_string(p));

    /* and the subject */
    if (p = header_field(cnt, "subject"))
	(void) strcpy(subject, p);

    /* now, construct a header */
    if (!hdr_format) /* this should NEVER be true, but just in case */
	hdr_format = DEF_HDR_FMT;

    (void) sprintf(buf, "%3.d ", cnt+1);
    b = buf+4;
    *b++ = ((cnt == current_msg && !iscurses)? '>': ' ');
    cnt = 5;
    for (p = hdr_format; *p; p++)
	if (*p == '\\')
	    switch (*++p) {
		case 't':
		    while (cnt % 8)
			cnt++, *b++ = ' ';
		when 'n':
		    cnt = 1, *b++ = '\n';
		otherwise: cnt++, *b++ = *p;
	    }
	else if (*p == '%') {
	    int buffer_space = 0;
	    int sign = -1;
	    register char *p2;

	    if (*++p == '-')
		sign = 1, p++;
	    if (isdigit(*p))
		p = my_atoi(p, &buffer_space);
	    switch (*p) {
		case 'f': p2 = from;
		when 'a':
		    if (!*(p2 = addr))
			p2 = from;
		when 'n':
		    if (!*(p2 = name))
			p2 = from;
		when '%': p2 = "%";
		when 't': p2 = to;
		when 'd': p2 = date;
		when 's': p2 = subject;
		when 'l': p2 = lines;
		when 'c': p2 = chars;
		when 'S': p2 = status;
		otherwise: continue; /* unknown formatting char */
	    }
	    if (!buffer_space)
		buffer_space = strlen(p2);
	    (void) sprintf(b, "%*.*s", sign * buffer_space, buffer_space, p2);
	    cnt += buffer_space, b += buffer_space;
	} else
	    cnt++, *b++ = *p;
    for (*b-- = 0; isspace(*b); --b)
	*b = 0;
    return buf;
}

/*
 * Using message "n", build a list of recipients that you would mail to if
 * you were to reply to this message.  If "all" is true, then it will take
 * everyone from the To line in addition to the original sender.
 * fix_address() is caled from mail.c, not from here.  There are too many
 * other uses for reply_to to always require reconstruction of return paths.
 * Note that we do NOT deal with Cc paths here either.
 * Check to make sure that we in fact return a legit address (i.e. not blanks
 * or null). If such a case occurs, return login name.  Always pad blank at end.
 */
char *
reply_to(n, all, buf)
register char *buf;
{
    register char *p = NULL, *p2, *b = buf, *field;
    char line[256];

    if (field = do_set(set_options, "reply_to_hdr")) {
	if (!*field)
	    goto From; /* special case -- get the colon-less From line */
	field = lcase_strcpy(line, field);
	while (*field) {
	    if (p2 = any(field, " \t,:"))
		*p2 = 0;
	    if ((p = header_field(n, field)) || !p2)
		break;
	    else {
		field = p2+1;
		while (isspace(*field) || *field == ':' || *field == ',')
		    field++;
	    }
	}
	if (!p)
	    print("Warning: message contains no `reply_to_hdr' headers.\n");
    }
    if (p || (!p && ((p = header_field(n, "reply-to")) ||
        (p = header_field(n, "return-path")) ||
        (p = header_field(n, "from"))))) {
	skipspaces(0);
	/* correct reply address is in "<correct-address>" */
	if (p2 = index(p, '<')) {
	    if (p = index(p2, '>'))
		*p = 0;
	    p = ++p2;
	}
    } else if (!p) {
From:
	/* if all else fails, then get the first token in "From" line */
	if (fseek(tmpf, msg[n].m_offset, L_SET) == -1 ||
		!(p2 = fgets(line, BUFSIZ, tmpf))) {
	    error("fseek in %s (msg %d, folder=%s)", tempfile, n+1, mailfile);
	    turnon(glob_flags, READ_ONLY);
	    return "";
	}
	p = index(p2, ' ') + 1;
    }
    /* find the first space and plug a null */
    if (p2 = any(p, " \t\n"))
	*p2 = 0;
    b += Strcpy(buf, p);

    /*
     * if `all' is true, append everyone on the "To:" line.
     * cc_to(), called separately, will catch the cc's
     */
    if (all && (p = header_field(n, "to")) && *p)
	(void) sprintf(b, " %s", p);
    rm_cmts_in_addr(buf);
    for (p = buf; isspace(*p); p++);
    if (!*p)
	(void) strcpy(buf, login);
    return strcat(buf, " ");
}

char *
subject_to(n, buf)
register char *buf;
{
    register char *p;
    buf[0] = 0; /* make sure it's already null terminated */
    if (!(p = header_field(n, "subject")))
	return NULL;
    if (strncmp(p, "Re:", 3))
	(void) strcpy(buf, "Re: ");
    return strcat(buf, p);
}

char *
cc_to(n, buf)
register char *buf;
{
    register char *p;
    buf[0] = 0; /* make sure it's already null terminated */
    if (!(p = header_field(n, "cc")))
	return NULL;
    rm_cmts_in_addr(p);
    return strcpy(buf, p);
}

/*
 * fix addresses according to the sender's address.  If he's on a remote
 * machine, chances are that the addresses of everyone else he mailed to
 * are addresses from his machine.  Reconstruct those addresses to route
 * thru the senders machine first.
 */
fix_addresses(to, cc)
char *to, *cc;
{
    int   to_argc, cc_argc, cnt;
    char  **to_argv, **cc_argv, pre_path[128], *p, c;
    char  **new_to, **new_cc, **calloc(), buf[256];

    pre_path[0] = 0;
    if (!(to_argv = mk_argv(to, &to_argc, 0)))
	return;   /* what can we do?  Just return unchanged To and Cc lists */
    cc_argv = mk_argv(cc, &cc_argc, 0);  /* the cc list may be empty */
    if (cc_argc == 0 && to_argc == 1)
	return;   /* there's only one reciepient, nothing to alter */

    if (p = any(to_argv[0], "!=")) {
	char *p2;
	while (p2 = any(++p, "!="))
	    p = p2;
	c = *p, *p = 0;
	(void) strcpy(pre_path, to_argv[0]);
	*p = c;
	for (cnt = 0; cnt < MAX_HOST_NAMES && ourname[cnt]; cnt++)
	    if (!strcmp(pre_path, ourname[cnt])) {
		*pre_path = 0;
		break;
	    }
    }

    if (!(new_to = calloc((unsigned)(to_argc+1), sizeof (char *))) ||
	!(new_cc = calloc((unsigned)(cc_argc+1), sizeof (char *)))) {
	error("malloc in fix_headers");
	return;
    }
    /* this is the original sender */
    strdup(new_to[0], to_argv[0]);

    /* now modify the addresses to be routed thru the sender's adderess */
    for (cnt = 1; cnt < to_argc; cnt++) {
	if (p = index(to_argv[cnt], ','))
	    *p = 0;
	Debug("Changing \"%s\" to ", to_argv[cnt]);
	(void) sprintf(buf, "%s%s", pre_path, to_argv[cnt]);
	strdup(new_to[cnt], buf);
	Debug("\"%s\"\n", new_to[cnt]);
    }
    for (cnt = 0; cnt < cc_argc; cnt++) {
	Debug("Changing \"%s\" to ", cc_argv[cnt]);
	if (p = index(cc_argv[cnt], ','))
	    *p = 0;
	(void) sprintf(buf, "%s%s", pre_path, cc_argv[cnt]);
	strdup(new_cc[cnt], buf);
	Debug("\"%s\"\n", new_cc[cnt]);
    }
    (void) argv_to_string(to, new_to);
    (void) argv_to_string(cc, new_cc);
    free_vec(to_argv);
    free_vec(cc_argv);
    free_vec(new_to);
    free_vec(new_cc);
}

/*
 * pass a string describing header like, "Subject: ", current value, and
 * whether or not to prompt for it or to just post the information.
 * If do_prompt is true, "type in" the current value so user can either
 * modify it, erase it, or add to it.
 */
char *
set_header(str, curstr, do_prompt)
register char *str, *curstr;
{
    static char	   buf[BUFSIZ];
    int 	   offset = 0;
    register char  *p = curstr;

    buf[0] = 0;
    wprint(str);
    fflush(stdout);		 /* force str curstr */
    if (do_prompt) {
	if (curstr)
	    for (p = curstr; *p; p++)
#ifdef SUNTOOL
		if (istool)
		    rite(*p); /* mimics typing for the tool */
		else
#endif /* SUNTOOL */
		if (isoff(glob_flags, ECHO_FLAG))
		    fputc((buf[offset++] = *p), stdout);
		else
#ifdef TIOCSTI
		    if (ioctl(0, TIOCSTI, p) == -1) {
			error("ioctl: TIOCSTI");
			wprint("You must retype the entire line.\n%s", str);
			break;
		    }
#else
                {
		    wprint("WARNING: -e flag! Type the line over.\n%s", str);
		    break;
		}
#endif TIOCSTI

	if (istool)
	    return NULL;
	if (Getstr(buf, BUFSIZ, offset) == -1)
	    buf[0] = 0;
    } else
	puts(strcpy(buf, curstr));
    if (debug > 1)
	print("returning (%s) from set_header\n", buf);
    return buf;
}

/*
 * rm_cmts_in_addr() removes the comment lines in addresses that result from
 * sendmail or other mailers which append the user's "real name" on the
 * from lines.  Comments are in parens.
 * `take_me_off' takes occurences of login out of a list of recipients
 * if the variable "metoo" is set.  Some say that "metoo" is supposed to
 * pass the -m flag along to sendmail, but not everyone runs sendmail :-)
 *
 * First expand a possible alias. Note that same string returns if not aliased.
 * strip off host from login name (considers user@host, host!user and so on)
 * if (user == ourlogin)
 *    match host with _this_ host, or
 *    match host with alternate hosts (alternate hostname, * matches all cases)
 *    if matched, then remove name from list.
 * ignore parenthesized names (those are comments ignored by "correct" mailers)
 */
rm_cmts_in_addr(p)
register char *p;
{
    char buf[BUFSIZ], *start = p, *cmnt;
    register char **argv;
    int cnt, argc, x = 0, take_me_off = !do_set(set_options, "metoo");
    int in_cmnt = FALSE, i;

    if (!(argv = mk_argv(p, &argc, 0)))
	return;
    for (cnt = 0; cnt < argc; cnt = x+1) {
	/* comments are in parens, as in: "argv (Dan Heller)" */
	if ((cmnt = any(argv[cnt], "()")) || in_cmnt) {
	    x++;
	    if (cmnt && *cmnt != ')')
		in_cmnt = TRUE;
	    else if (cmnt && *cmnt == ')')
		in_cmnt = FALSE; /* else, unchange cmnt flag */
	    /* if the previous argument was deleted, then delete this one
	     * example, we are "user" and our host is "host" take off:
	     * list ... user@host (firstname lastname) more users...
	     * "user@host" "(Firstname" "Last name)"
	     */
	    xfree(argv[cnt]), argv[cnt] = "";
	    continue;
	}
	(void) strcpy(buf, argv[x = cnt]);
	/* example: "Dan Heller <argv>" -- real address is in angle brackets
	 * delete the angle brackets.  Potential bug: the name is still in
	 * the address.. result might be: "Dan Heller argv"
	 */
	if (*buf == '<') {
	    while (!(p = index(argv[x], '>')))
		if (++x == argc)
		    break;
		else
		    (void) strcat(buf, argv[x]);
	    if (!p)  /* this one is in error, but parse the rest anyway */
		continue;
	    if (p = index(buf, '>'))
		*p = 0; /* play with buf, not argv[]! */
	}

	/* if we're removing ourselves from the list, see if we're on it */
	if (!take_me_off || !chk_two_lists(login, buf, "!@%="))
	    continue;

	/* we must be on the list -- see if the hostnames match alternates
	 * first match our current host if a host was even given.
	 * Bug: if a login name is a subset (or a complete) hostname, then
	 * incorrect results may occur.
	 */
	if (any(buf, "!@%=")) {
	    for (i = 0; i < MAX_HOST_NAMES && ourname[i]; i++)
		if (chk_two_lists(buf, ourname[i], "!@%="))
		    break;
	    /* If the hostname in the address is one of our hostnames,
	     * remove this address.  Else, see if the alternates hostnames
	     * are in the address.
	     */
	    if ((i == MAX_HOST_NAMES || !ourname[i]) &&
		(!(p = do_set(set_options, "alternates")) || *p != '*' &&
		 !chk_two_lists(buf, p, "!@%= \t,")))
		    continue;
	}
	/* all's well that ends well -- take us off the list */
	xfree(argv[cnt]), argv[cnt] = "";
    }
    (void) argv_to_string(start, argv);
    free_vec(argv);
}

/*
 * improve uucp paths by looking at the name of each host listed in the
 * path given.
 *    sun!island!pixar!island!argv
 * It's a legal address, but redundant. Also, if we know we talk to particular
 * hosts via uucp, then we can just start with that host and disregard the
 * rest of the line.  So, first get the known hosts and save them. Then start
 * at the end of the original path (at the last ! found), and move backwards
 * saving each hostname.  If we get to a host that we know about, stop there
 * and use that address.  If we get to a host we've already seen, then
 * delete it and all the hosts since then until the first occurance of that
 * hostname.  When we get to the beginning, the address will be complete.
 *
 * return all results into the original buffer passed to us.  Since we can
 * at worst not touch the path (shorten it if anything), we know we're not
 * going to overrun the buffer.
 */

#define go_back_to_bang() for (--p; p > start && *p != '!'; p--)

improve_uucp_paths(original)
register char *original;
{
    char	   hostnames[32][64], buf[BUFSIZ], *knowns, *end;
    register char *p, *recipient, *start = original, *b = buf, c;
    int		   saved_hosts, i;

    if (!original || !*original)
	return;

    knowns = do_set(set_options, "known_hosts");

    while ((end = any(start, " \t,")) || *start) {
	saved_hosts = 0;
	if (!end)
	    p = start + strlen(start);
	else
	    p = end; /* set p to the "end" cuz p moves. start will equal end */
	c = *p, *p = 0; /* save the char we're nulling out (separating addrs) */
	go_back_to_bang();
	if (p == start)
	    b += Strcpy(b, start), recipient = NULL;
	else {
	    recipient = p+1;
	    while (p > start) {
		/* null remainder of string to save the hostname */
		*p = 0;
		go_back_to_bang();
		/* if p is not at the start, move it forward past the '!' */
		if (p != start)
		    ++p;
#ifndef SYSV
		/* next host on the list is ourselves, ignore preceding names */
		for (i = 0; i < MAX_HOST_NAMES && ourname[i]; i++)
		    if (!strcmp(p, ourname[i]))
			saved_hosts = 0;
#endif SYSV
		for (i = 0; i < saved_hosts; i++)
		    if (!strcmp(hostnames[i], p))
		       saved_hosts = i;
		(void) strcpy(hostnames[saved_hosts++], p);
		if (saved_hosts == 32) {
		    print("Too many hosts in uucp path! Address not fixed.\n");
		    return; /* original buffer is unchanged */
		}
		/* If we know that we call this host, break */
		if (p == start || knowns && chk_two_lists(p, knowns, " ,\t"))
		    break;
		--p; /* move p back onto the '!'; it != start */
	    }
	}
	while (saved_hosts-- > 0)
	    b += strlen(sprintf(b, "%s!", hostnames[saved_hosts]));
	if (recipient) {
	    /* if there were no saved_hosts, add a ! */
	    if (*(b-1) != '!')
		*b++ = '!';
	    b += Strcpy(b, recipient);
	}
	*b++ = c;
	if (!end)
	    break;
	start = end + 1;
	if (isspace(*start)) {
	    *b++ = ' ';
	    while (isspace(*start))
		start++;
	}
    }
    *b = 0;
    (void) strcpy(original, buf);
}
