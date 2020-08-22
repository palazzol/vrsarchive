/* @(#)curses.c	(c) copyright 3/18/87 (Dan Heller) */

/* curses.c -- routine to deal with the curses interface */
#ifdef CURSES

#include "mush.h"
#include "bindings.h"

curses_init(argc, argv)
register char **argv;
{
    char buf[80];

#if 1
    char *UP = "a"; extern char ttytype[];
#else
    extern char *UP, ttytype[];
#endif

    if (argv && *++argv && !strcmp(*argv, "-?"))
	return help(0, "curses", cmd_help);
    if (iscurses) {
	print("You can't run curses from the curses mode (silly).");
	return -1;
    }
    if (ison(glob_flags, IS_GETTING)) {
	print("Finish your letter first.\n");
	return -1;
    }
#ifdef SUNTOOL
    if (istool) {
	print("My, aren't we the adventuresome type!");
	timerclear(&(mail_timer.it_interval));
	timerclear(&(mail_timer.it_value));
	tool_destroy(tool), istool = FALSE;
	curses_init(0, 0);
	do_loop(); /* doesn't return */
    }
#endif SUNTOOL

    /* you can not start curses in no echo mode.. must be in normal mode */
    echo(), nocrmode();
    (void) initscr();
#ifdef SIGCONT
    /* initscr will play with signals -- make sure they're set right. */
    (void) signal(SIGTSTP, stop_start);
    (void) signal(SIGCONT, stop_start);
#endif SIGCONT
    if (!UP || !*UP) {
	print("Terminal type %s can not use the curses interface.\n", ttytype);
	return -1;
    }
    iscurses = TRUE;
    noecho(), crmode(); /* reset tty state -- do not use "echo_on/off()" */
    scrollok(stdscr, TRUE);
    /* if the user hasn't set his screen explicitely, set it for him */
    if (!do_set(set_options, "screen"))
#if 0
	switch (_tty.sg_ospeed) {
	    case B300 : screen = min(LINES-2, 7);
	    when B1200 : screen = min(LINES-2, 14);
	    when B2400 : screen = min(LINES-2, 22);
	    otherwise : screen = LINES-2;
	}
    else
#endif
	screen = min(screen, LINES-2);
    crt = LINES;
    if (argc)
	(void) cmd_line(sprintf(buf, "headers %d", current_msg+1), msg_list);
    if (!do_set(set_options, "no_reverse"))
	turnon(glob_flags, REV_VIDEO);
    turnoff(glob_flags, CONT_PRNT);
    return -1; /* doesn't affect messages */
}

/*
 * get input in cbreak mode and execute the appropriate command.
 * when the command is done (usually), the user is prompted to
 * hit any key to continue. At this point, the user may enter a
 * new command so no screen refreshing needds to be done. This
 * new command is returned to caller and may be passed back.
 *
 * The variable "cntd_cmd" (continued command) is set to true if
 * this routine is called with the passed parameter (c) > 0. If
 * so, then the character passed is the character input by the
 * user at the last "hit return" prompt indicating that he wants
 * to execute a new command and not draw the screen.
 *
 * cntd_cmd is also set to true if the command that the user invokes
 * causes any sort of output that requires a screen refresh.  The
 * variable redo is set to 1 if the header page not only requires
 * redrawing, but updating ... (new call to do_hdrs)
 *
 * calls that say: print("%s", compose_hdr(current_msg)) are constructed
 * that way cuz if the header has a `%' in it, then print will try to
 * expand it.
 */
curses_command(c)
register int c;
{
    char 	buf[BUFSIZ], file[128], list[128];
    int 	n, cntd_cmd = (c > 0), curlin;
    static int  redo;  /* set if headers should be redrawn */

    clear_msg_list(msg_list); /* play it safe */
    if (!cntd_cmd) {
	(void) check_new_mail();
	curlin = max(1, current_msg - n_array[0] + 1);
	(void) strncpy(buf, stdscr->_y[curlin], COLS-1);
	buf[COLS-1] = 0; /* strncpy does not null terminate */
	if (ison(glob_flags, REV_VIDEO) && msg_cnt)
	    STANDOUT(curlin, 0, buf);
	mail_status(0);
	move(curlin, 0), refresh();
	/* reprint to remove reverse video from current line (don't refresh) */
	if (ison(glob_flags, REV_VIDEO))
	    mvaddstr(curlin, 0, buf);
	c = getcmd(); /* get input AFTER line redrawn without reverse video */
    }
    buf[0] = list[0] = file[0] = '\0';

    /* goto a specific line number */
    if (c == C_GOTO_MSG) {
	c = C_NULL;
	if (msg_cnt <= 1)
	    print("Not enough messages.");
	else if (curses_msg_list(strcpy(buf, "goto msg: "), list, msg_list)) {
	    for (n = 0; !msg_bit(msg_list, n); n++)
		;
	    if ((current_msg = n) < n_array[0] || n > n_array[screen-1])
		redo = 1;
	}
	if (cntd_cmd && msg_cnt)
	    print("%s", compose_hdr(current_msg));
	if (cntd_cmd)
	    putchar('\n');
    } else if (c == C_WRITE_LIST || c == C_SAVE_LIST || c == C_COPY_LIST
			       || c == C_DELETE_LIST || c == C_UNDEL_LIST) {
	
	if (msg_cnt <= 1)
	    print("Not enough messages."), c = C_NULL;
	else if (!curses_msg_list(sprintf(buf, "%s msg list: ",
		(c == C_WRITE_LIST)? "write" : (c == C_SAVE_LIST)?  "save" :
		(c == C_DELETE_LIST)? "delete" : "undelete"), list, msg_list))
	    c = C_NULL;
	if (cntd_cmd)
	    putchar('\n');
    }

    /* first do non-mail command stype stuff */
    switch (c) {
	case C_NULL : ;

	/* screen optimization stuff */
	when C_REVERSE :
	    if (ison(glob_flags, REV_VIDEO))
		turnoff(glob_flags, REV_VIDEO);
	    else
		turnon(glob_flags, REV_VIDEO);

	when C_REDRAW : if (!redo) redraw();

	/*
	 * screen movement
	 */
	when C_NEXT_MSG :
	/* case 'j' : case 'J' : case '+' : case '\n' : /* next */
	    if (current_msg + 2 > msg_cnt || !cntd_cmd && curlin == screen)
		bell(); /* reached the end */
	    else {
		if (++current_msg > n_array[screen-1])
		    redo = 1;
		if (cntd_cmd)
		    print("%s", compose_hdr(current_msg)), putchar('\n');
	    }
	when C_PREV_MSG :
	/* when 'k' : case 'K' : case '-' : case CTRL(k) : /* previous */
	    if (!cntd_cmd && curlin == 1 || current_msg == 0)
		bell();  /* at the beginning */
	    else {
		if (--current_msg < n_array[0])
		    redo = 1;
		if (cntd_cmd)
		    print("%s", compose_hdr(current_msg)), putchar('\n');
	    }
	when C_FIRST_MSG : case C_LAST_MSG :
	    n = current_msg;
	    if (c == C_FIRST_MSG && (current_msg = 0) < n_array[0] ||
		c == C_LAST_MSG && (current_msg = msg_cnt-1)> n_array[screen-1])
		if (!cntd_cmd)
		    (void) cmd_line(sprintf(buf, "headers %d", current_msg+1),
			     msg_list);
		else
		    redo = 1;
	    if (cntd_cmd && n != current_msg)
		print("%s", compose_hdr(current_msg)), putchar('\n');
	/* top and bottom of headers screen */
	when C_TOP_PAGE : case C_BOTTOM_PAGE :
	    if (!cntd_cmd)
		if (c == C_TOP_PAGE)
		    current_msg = n_array[0];
		else
		    current_msg = min(n_array[screen-1], msg_cnt-1);
	    else
		bell();
	when C_NEXT_SCREEN : /* next page */
	    if (msg_cnt > screen) {
		(void) cmd_line(strcpy(buf, "headers +"), msg_list);
		current_msg = n_array[0];
		return redo = 0;
	    } else
		bell();
	when C_PREV_SCREEN : /* previous page */
	    if (current_msg > 0 || cntd_cmd)
		(void) cmd_line(strcpy(buf, "headers -"), msg_list), redo = 0;
	    current_msg = (msg_cnt <= 1)? 0 : n_array[0];
	    return 0;
	    /* break;  (not stated for lint) */

	case C_SHOW_HDR :
	    if (cntd_cmd && msg_cnt)
		puts(compose_hdr(current_msg));

	/* read from/save to record file (.mushrc) */
	when C_SOURCE : case C_SAVEOPTS :
	    print("%s filename [default]: ",
		(c == C_SOURCE)? "source" : "save options to");
	    if (Getstr(file, LINES-40, 0) < 0) {
		clr_bot_line();
		return 0;
	    }
	    iscurses = FALSE;
	    turnon(glob_flags, PRE_CURSES);
	    (void) cmd_line(sprintf(buf, "%s %s",
		(c == C_SOURCE) ? "source" : "saveopts", file), msg_list);
	    iscurses = TRUE;
	    turnoff(glob_flags, PRE_CURSES);
	    cntd_cmd = 1;

	/*
	 * search commands
	 */
	when C_NEXT_SEARCH : case C_PREV_SEARCH : case C_CONT_SEARCH :
	    if (c != C_CONT_SEARCH)
		c = search(0 + (c == C_PREV_SEARCH));
	    else
		c = search(-1);
	    if (cntd_cmd)
		putchar('\n');
	    if (c == 0)
		break;
	    if (cntd_cmd)
		print("%s", compose_hdr(current_msg)), putchar('\n');
	    if (n_array[0] > current_msg || n_array[screen-1] < current_msg) {
		redo = 1;
		if (!cntd_cmd)
		    (void) cmd_line(sprintf(buf, "headers %d",
					    current_msg+1), msg_list);
	    }

	/*
	 * actions on messages
	 */
	/* delete/undelete */
	when C_DELETE_MSG : case C_DELETE_LIST :
	case C_UNDEL_MSG : case C_UNDEL_LIST :
	    if (!msg_cnt) {
		print("No messages.");
		if (cntd_cmd)
		    putchar('\n');
		break;
	    }
	    if (!*list)
		set_msg_bit(msg_list, current_msg);
	    turnon(glob_flags, DO_UPDATE);
	    for (n = 0; n < msg_cnt; n++)
		if (msg_bit(msg_list, n)) {
		    if (c == C_DELETE_MSG || c == C_DELETE_LIST)
			turnon(msg[n].m_flags, DELETE);
		    else
			turnoff(msg[n].m_flags, DELETE);
		    if (!cntd_cmd && msg_cnt < screen ||
			!cntd_cmd && n >= n_array[0] && n <= n_array[screen-1])
			mvaddstr(max(1, n - n_array[0] + 1), 0, compose_hdr(n));
		    else
			redo = 1;
		}
	    if (cntd_cmd || *list) {
		if (cntd_cmd) { /* print(), THEN putchar() -- overwrite line */
		    print("%sdeleted %s",
		    (c == C_DELETE_MSG || c == C_DELETE_LIST)? "":"un", list);
		    putchar('\n');
		}
		if (ison(msg[current_msg].m_flags, DELETE))
		    (void) next_msg(FALSE, DELETE);
		if (do_set(set_options, "autoprint"))
		    return C_DISPLAY_MSG;
		if (cntd_cmd)
		    puts(compose_hdr(current_msg));
	    }

	/*
	 * write/save messages.  If a list is necessary, the user already
	 * entered it above since he must have used a capital letter. If so,
	 * list will contain good data (already been validated above).
	 * if a list is given, set iscurses to 0 so that print statements
	 * will scroll and the user sees the multiple output. else, one
	 * line can go on the bottom line just fine.
	 */
	when C_WRITE_MSG : case C_SAVE_MSG : case C_COPY_MSG :
	case C_WRITE_LIST : case C_SAVE_LIST : case C_COPY_LIST : {
	    register char *p =
		(c == C_WRITE_MSG || c == C_WRITE_LIST)? "write" :
		(c == C_SAVE_MSG  || c == C_SAVE_LIST)?  "save"  : "copy";
	    print(sprintf(buf, "filename to %s [mbox]: ", p));
	    if (Getstr(file, COLS-1-strlen(buf), 0) >= 0) {
		char *argv[3];
		clr_bot_line();
		argv[0] = strcpy(buf, p);
		argv[1] = file;
		argv[2] = NULL;
		if (!*list)
		    set_msg_bit(msg_list, current_msg);
		move(LINES-1, 0), refresh();
		if (*list)
		    iscurses = FALSE;
		turnon(glob_flags, IS_PIPE);
		if (save_msg(2, argv, msg_list) < 0)
		    *list = 0;
		turnoff(glob_flags, IS_PIPE);
		if (cntd_cmd)
		    putchar('\n'), puts(compose_hdr(current_msg));
		if (*list)
		    iscurses = cntd_cmd = redo = TRUE;
		else if (!cntd_cmd && msg_cnt)
		    mvaddstr(curlin, 0, compose_hdr(current_msg));
	    } else {
		print("No messages saved.");
		if (cntd_cmd)
		    putchar('\n');
	    }
	}

	/* preserve message */
	when C_PRESERVE :
	    if (!msg_cnt) {
		print("No messages.");
		if (cntd_cmd)
		    putchar('\n');
		break;
	    }
	    if (ison(msg[current_msg].m_flags, PRESERVE))
		turnoff(msg[current_msg].m_flags, PRESERVE);
	    else
		turnon(msg[current_msg].m_flags, PRESERVE);
	    if (cntd_cmd) {
		print("%s", compose_hdr(current_msg)), putchar('\n');
		redo = 1;
	    } else
		mvaddstr(curlin, 0, compose_hdr(current_msg));

	/* order messages (sort) and rediesplay the headers */
	when C_SORT : case C_REV_SORT :
	    (void) strcpy(file, "sort");
	    if (c == C_REV_SORT) {
		print("Reverse "), turnon(glob_flags, CONT_PRNT);
		(void) strcat(file, " -");
	    }
	    print("Order messages by [Status, date, subject, author]: ");
	    if ((c = getchar()) == 's' || c == 'S' || c == 'd' || c == 'a') {
		print("reordering messages...");
		(void) cmd_line(sprintf(buf, "%s %c", file, c), msg_list);
		print_more("done.");
		if (cntd_cmd)
		    putchar('\n'), puts(compose_hdr(current_msg));
		redo = 1;
	    } else
		clr_bot_line();

	when C_QUIT_HARD :
	    (void) quit(0, DUBL_NULL);
	    redo = 1; /* new mail must have come in */

	/* quit or update -- vrfy_update (returns 1 if updated) */
	when C_QUIT : case C_UPDATE :
	    if (!vrfy_update(&cntd_cmd, &redo))
		if (c == C_UPDATE)
		    break;
		else
		    turnoff(glob_flags, DO_UPDATE);
	    if (c == C_QUIT) {
		putchar('\n');
		cleanup(0);
		redo = 1;
	    }

	when C_EXIT : case C_EXIT_HARD :
	    clr_bot_line();
	    iscurses = FALSE;
	    if (c != C_EXIT && c != C_EXIT_HARD)
		putchar('\n');
	    cleanup(0);

	/* change to a new folder */
	when C_FOLDER :
	    for (;;) {
		print("New folder (?=list): ");
		if (Getstr(file, COLS-22, 0) > 0) {
		    if (!strcmp(file, "?")) {
			clr_bot_line();
			iscurses = 0;
			puts("folders in your folder directory:");
			(void) cmd_line(strcpy(buf, "folders"), msg_list);
	puts("Precede folder names with a +. `%' to specify system mailbox.");
			cntd_cmd = iscurses = 1;
			continue;
		    }
		    clearok(stdscr, FALSE);
		    if (strcmp(file, "-?"))
			vrfy_update(&cntd_cmd, &redo);
		    move(LINES-1, 0), refresh();
		    if (cmd_line(sprintf(buf, "folder ! -N %s", file),
			     msg_list) == -1) {
			/* error message was printed; leave room to read it */
			putchar('\n');
			cntd_cmd = 1, redo = 0;
		    } else
			redo = 1, cntd_cmd = 0;
		    break;
		} else {
		    print("\"%s\" unchanged.", mailfile);
		    if (cntd_cmd)
			putchar('\n');
		    break;
		}
	    }

	/* shell escape */
	when C_SHELL_ESC :
	    print("Shell command: ");
	    if (Getstr(file, COLS-24, 0) < 0)
		clr_bot_line();
	    else {
		putchar('\n');
		iscurses = FALSE;
		(void) cmd_line(sprintf(buf, "sh %s", file), msg_list);
		iscurses = TRUE;
		cntd_cmd = 1;
	    }

	/* do a line-mode like command */
	when C_CURSES_ESC :
	    print(":");
	    if (Getstr(buf, COLS-2, 0) < 0)
		break;
	    putchar('\n');
	    iscurses = FALSE;
	    if (!*buf) {
		/* return -1 because iscurses = 0 is not enough! */
		redo = 0;
		endwin(); /* this turns echoing back on! */
		echo_off();
		return -1;
	    }
	    (void) cmd_line(buf, msg_list);
	    /* they may have affected message status or had text output */
	    cntd_cmd = redo = 1;
	    iscurses = TRUE;
	    if (msg_cnt)
		puts(compose_hdr(current_msg));

	/* send message to printer */
	when C_PRINT_MSG : lpr(0, DUBL_NULL, msg_list);

	/* cd */
	when C_CHDIR :
	    print("chdir to [.]: ");
	    if (Getstr(file, COLS-12, 0) < 0)
		break;
	    clr_bot_line();
	    (void) cmd_line(sprintf(buf, "cd %s", file), msg_list);
	    if (cntd_cmd)
		putchar('\n');

	/* variable settings */
	when C_VAR_SET : case C_IGNORE : case C_ALIAS : case C_OWN_HDR :
	    curs_vars(c, &cntd_cmd); /* cntd_cmd is reset if there's output! */

	when C_VERSION :
	    do_version(); /* duh */
	    if (cntd_cmd)
		putchar('\n');

	when C_MAIL_FLAGS :
	    print("flags [-?]: ");
	    if ((c = Getstr(file, COLS-50, 0)) < 0)
		break;
	    putchar('\n');
	    if (c == 0)
		(void) strcpy(file, "-?");
	/* Fall thru */
	case C_MAIL :
	    clr_bot_line();
	    iscurses = FALSE;
	    (void) cmd_line(sprintf(buf, "mail %s", file), msg_list);
	    iscurses = TRUE, cntd_cmd = 1;
	    if (msg_cnt)
		print("%s", compose_hdr(current_msg)), putchar('\n');

	/* reply to mail */
	when C_REPLY_SENDER : case C_REPLY_ALL : {
	    register char *p = (c == C_REPLY_ALL)? "replyall" : "replysender";
	    clr_bot_line();
	    iscurses = FALSE;
	    (void) cmd_line(sprintf(buf, "%s %d", p, current_msg+1),
		msg_list);
	    iscurses = TRUE, cntd_cmd = 1;
	    if (msg_cnt)
		print("%s", compose_hdr(current_msg)), putchar('\n');
	}

	/* type out a message */
	when C_DISPLAY_MSG : case C_TOP_MSG : case C_DISPLAY_NEXT :
	    if (!msg_cnt ||
		c != C_DISPLAY_NEXT && ison(msg[current_msg].m_flags, DELETE)) {
		if (!msg_cnt)
		    print("No messages.");
		else
		    print("Message %d deleted; type 'u' to undelete.",
				      current_msg+1);
		if (cntd_cmd)
		    putchar('\n');
		break;
	    }
	    clr_bot_line();
	    iscurses = FALSE;
	    if (cntd_cmd)
		putchar('\n');
	    if (c == C_DISPLAY_MSG)
		c = cmd_line(strcpy(buf, "type"), msg_list);
	    else if (c == C_TOP_MSG)
		c = cmd_line(strcpy(buf, "top"), msg_list);
	    else
		c = cmd_line(strcpy(buf, "next"), msg_list);
	    if (c > -1)
		cntd_cmd = redo = 1;
	    iscurses = TRUE;
	    puts(compose_hdr(current_msg));

	/* bind a key or string to a command */
	when C_BIND :  case C_UNBIND : {
	    char *argv[2];
	    argv[0] = (c == C_BIND) ? "bind" : "unbind";
	    argv[1] = NULL;
	    if (bind_it(0, argv) < -1)
		cntd_cmd = 1;
	    else if (cntd_cmd) /* if it was already set anyway */
		putchar('\n');
	}

	/* help stuff */
	when C_HELP :
	    (void) c_bind(NULL);
	    cntd_cmd = 1;
	    if (msg_cnt)
		puts(compose_hdr(current_msg));

	/* now do interactive stuff as if run from the mush shell */
	otherwise :
	    bell();
	    if (cntd_cmd) {
		/* use print instead of puts to overwrite hit_return msg */
		print("unknown command"), putchar('\n');
		redo = 1;
	    }
    }

    if (cntd_cmd) {
	int old_cnt = msg_cnt;
	if (!(c = hit_return()) && !redo && msg_cnt == old_cnt)
	    redraw();
	clr_bot_line();
	if (old_cnt !=  msg_cnt)
	    redo = 1;
	if (c)
	    return c;
    }
    if (redo) {
	n = current_msg;
	clear();
	if (msg_cnt < screen || n_array[0] < n && n < n_array[screen-1])
	    (void) do_hdrs(0, DUBL_NULL, NULL);
	else
	    (void) cmd_line(sprintf(buf, "headers %d", n+1), msg_list);
	redo = 0;
    }
    return 0;
}

vrfy_update(cntd_cmd, redo)
int *cntd_cmd, *redo;
{
    char buf[16];
    int c;

    /* update current folder */
    if (ison(glob_flags, DO_UPDATE)) {
	print("Update %s [y]? ", mailfile);
	if ((c = getchar()) == 'n' || c == 'N' || c == 7 || c == 127 || c == 4){
	    print("%s unmodified.", mailfile);
	    if (*cntd_cmd)
		putchar('\n');
	    return 0;
	}
	(void) cmd_line(strcpy(buf, "update"), msg_list);
	if (*cntd_cmd)
	    *redo = 1, *cntd_cmd = 0;
    }
    turnoff(glob_flags, DO_UPDATE);
    return 1; /* make sure bottom line is clear and no reverse video */
}
#endif CURSES
