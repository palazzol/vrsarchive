/* @(#)curs_io.c	(c) copyright 3/18/87 (Dan Heller) */

/* curs_io.c -- curses based I/O */
#include "mush.h"

#ifdef CURSES
#include "bindings.h"
#endif CURSES

#ifdef Addch
#undef Addch
#endif Addch

#ifndef CURSES

#define Addch(c) if (isoff(glob_flags, ECHO_FLAG)) \
		      fputc(c, stdout), fflush(stdout)

#else

/* see end of Getstr */
#define Addch(c)  \
    if (iscurses) \
	addch(c), refresh(); \
    else if (isoff(glob_flags, ECHO_FLAG)) \
	fputc(c, stdout), fflush(stdout)

#ifndef A_REVERSE
#define erasechar()	_tty.sg_erase
#define killchar()	_tty.sg_kill
#endif
#endif CURSES

/*
 * get a string of at most 'length' chars.
 * allow backspace-space-backspace, kill word and kill line
 * (options set by user in stty).
 * length is the max length this string can get. offset is from beginning
 * of string.
 * input of ^D returns -1; otherwise, return the number of chars in string.
 */
#define backspace() 	{ Addch('\b'); Addch(' '); Addch('\b'); }

Getstr(String, length, offset)
char String[];
register int length;
{
    register int c;
    register int count = offset;

    while ((c = getchar()) != '\n' && c != 4 /* ^D */ && c != EOF &&
	isoff(glob_flags, WAS_INTR)) {
	/* echo isn't set, so whatever the character, enter it */
	if (ison(glob_flags, ECHO_FLAG) && count < length)
	    String[count++] = c;
	else if (c == erasechar() || c == 8 || c == 127) {
	    if (count) {
		backspace() /* macro */
		count--;
	    } else if (!count && iscurses) {
		String[0] = '\0';
		return -1;
	    }
	} else if (c == killchar()) {
	    if (count) {
		do
		    backspace()  /* macro */
		while (--count);
	    }
	} else
#ifndef TIOCGLTC
	if (c == '\022') /* system doesn't have ltchars */
#else
	if (c == ltchars.t_rprntc) /* reprint line */
#endif TIOCGLTC
	    String[count] = 0, printf("\n%s", String);
        else
#ifndef TIOCGLTC
	if (c == '\027') /* system doesn't have ltchars */
#else
	if (c == ltchars.t_werasc) /* word erase */
#endif TIOCGLTC
	    while (count) {
		backspace();
		String[count--] = 0;
		if (!count || (String[count-1]== ' '&& !isspace(String[count])))
		    break;
	    }
	else if (c == '\t')
	    do  {
		Addch(' ');
		String[count] = ' ';
	    } while (++count % 8 && count < length);
	else if (count == length)
	    bell();
	else if (c > 31 && c != 127) {
	    Addch(c);
	    String[count++] = c;
	}
    }
    if (c == 4 || c == EOF || ison(glob_flags, WAS_INTR)) {     /* ^D */
	if (feof(stdin))
	    clearerr(stdin);
	return -1;
    }
    if (count && String[count-1] == '\\') {
	int count2;
	if (isoff(glob_flags, ECHO_FLAG))
	    putchar('\n');
	if ((count2 = Getstr(&String[count-1], length - count + 1, 0)) == -1)
	    return -1;
	return count + count2;
    }
    if (!iscurses && isoff(glob_flags, ECHO_FLAG))
	putchar('\n');
    while (count > 0 && isspace(String[count-1]))
	--count;
    String[count] = 0;
    return count;
}

#undef backspace
#undef Addch

#ifdef CURSES
/* curses based "more" like option */
void
c_more()
{
    register char c;

    print_more("--more--");
    while ((c = getchar()) != ' ' && c != '\n')
	bell();
    if (!iscurses && c != '\n')
	while (getchar() != '\n');
}

/*
 * prompt for a carriage return, but return whatever user types unless
 * it's a character which he might regret (like 'q' or 'x'). Ignore
 * interrupts (kind of) because we have nowhere to longjmp to.  When we
 * return, we'll setjmp again (top of loop.c)
 */
hit_return()
{
    int c;

    turnon(glob_flags, IGN_SIGS);
    iscurses = FALSE;
    (void) check_new_mail();
    iscurses = TRUE;
    mail_status(1), addstr("...continue... "), refresh();
    c = getcmd();
    turnoff(glob_flags, IGN_SIGS);

    /* don't let the user type something he might regret */
    if (c == C_QUIT || c == C_EXIT)
	return 0;
    return c;
}

curses_msg_list(str, list, m_list)
register char *str, *list;
char m_list[];
{
    register char *p = NULL;

    print(str);
    if (Getstr(list, COLS-13, 0) <= 0 || !(p = do_range(list, m_list)) ||
	(p == list && *p && *p != '$' && *p != '^')) {
	if (p)
	    print("Invalid message list: %s", p);
	return 0;
    }
    move(LINES-1, 0), refresh();
    return 1;
}

curs_vars(which, cntd_cmd)
int which;  /* really, a char */
int *cntd_cmd;
{
    char c, buf[128], buf2[128], *string;
    struct options **list;

    switch(which) {
	case C_OWN_HDR : string = "own_hdrs", list = &own_hdrs;
	when C_ALIAS : string = "alias", list = &aliases;
	when C_IGNORE : string = "ignore", list = &ignore_hdr;
	when C_VAR_SET : string = "set", list = &set_options;
	otherwise : clr_bot_line(); return;
    }

    print("%s [? Set Unset All]: ", string);
    c = getchar();
    clr_bot_line();
    switch (Lower(c)) {
	/* if help, print help -- if "all", show all settings. */
	case '?' : case 'a' :
	    if (c == '?') {
		if (!strcmp(string, "set")) {
		    print("which variable? [All <var>]: ");
		    if ((c = Getstr(buf, COLS-40, 0)) < 0)
			return;
		    clr_bot_line();
		    if (c > 0) {
			Lower(*buf);
			if (!strcmp(buf, "a"))
			    (void) strcpy(buf, "all");
			if (!strcmp(buf, "all"))
			    *cntd_cmd = 1;
			(void) cmd_line(sprintf(buf2, "set ?%s", buf),msg_list);
			return;
		    }
		}
		/* help returns next command (hit_return) */
		help(0, string, cmd_help);
		*cntd_cmd = 1;
		return;
	    }
	    *cntd_cmd = 1;
	    do_set(*list, NULL);

	/* if set, prompt for string and let user type */
	when 's' :
	    print("set: ");
	    c = Getstr(buf, COLS-18, 0);
	    clr_bot_line();
	    if (c > 0)
		(void) cmd_line(sprintf(buf2, "%s %s", string, buf), msg_list);

	/* if unset, just as easy as set! */
	when 'u' :
	    print("unset: ", string);
	    if (Getstr(buf, COLS-18, 0) > 0 && !un_set(list, buf)) {
		print("%s isn't set", buf);
		if (*cntd_cmd)
		    putchar('\n');
	    }
    }
}

#endif CURSES
