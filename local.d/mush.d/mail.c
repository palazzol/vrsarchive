/* @(#)mail.c 	(c) copyright 1986 (Dan Heller) */

#include "mush.h"

/*
 * mail.c --
 *    do_mail() 	invoked from within mail.  see function for description.
 *    abort_mail()	suntools specific: resets panel items and so forth.
 *    start_file()      creates the editing file and reset signal catching.
 *    mail_someone()    called from do_mail() or from the shell.
 *    add_to_letter()	adds the next line to letter --determine ~ escapes.
 *    finish_up_letter()  prompts for Cc:, verifies user really wants to send
 *    send_it()		invokes mailer, sends to record file, adds signature,
 *			fortune, expands aliases, adds own_hdrs.
 *    rm_edfile()	signals are directed here. remove letter, longjmp
 *
 * The flow of control in this file is NOT obvious to allow for both text
 * and suntools _event driven_ attributes.  In text, the flow is far more
 * obvious because it is sequential. In suntools, each function is called
 * from outside this modual.  Keystrokes are interpreted individually and
 * queued up in "rite.c".  select.c calls add_to_letter when a \n is entered
 * passing the string stored in rite.c.  If you're trying to follow the flow
 * of control for suntools, keep the event drivers in mind and follow select.c
 * and rite.c
 */
#define TO_FIELD	1
#define SUBJECT		2
#define CC_FIELD	3
#define BC_FIELD	4

static char Subject[BUFSIZ], To[BUFSIZ], Cc[BUFSIZ], Bcc[BUFSIZ];
static char edfile[MAXPATHLEN], in_reply_to[256];
static int killme, in_editor;
static long flags;
static int (*oldhup)(), (*oldterm)(), (*oldint)(), (*oldquit)();
static void send_it();
static FILE *fp;
static jmp_buf cntrl_c_buf;

/* argc, and argv could be null if coming from compose */
do_mail(n, argv)
register int n;   /* no need for "argc", so use the space for a variable */
register char **argv;
{
    char firstchar = (argv)? **argv: 'm';
    char list[MAXMSGS_BITS];
    register char *to = NULL, *cc = NULL, *subj = NULL;
    register int flgs = 0;

    if (ison(glob_flags, IS_GETTING)) {
	print("You must finish the letter you are editing first.\n");
	return -1;
    }
    turnon(flgs, NO_IGNORE); /* if we include a message, include all hdrs */

    clear_msg_list(list);
    if (do_set(set_options, "autoedit"))
	turnon(flgs, EDIT);
#ifdef VERBOSE_ARG
    if (do_set(set_options, "verbose"))
	turnon(flgs, VERBOSE);
#endif VERBOSE_ARG
    if (do_set(set_options, "autosign"))
	turnon(flgs, SIGN);
    if (lower(firstchar) == 'r' && do_set(set_options, "autoinclude"))
	turnon(flgs, INCLUDE), set_msg_bit(list, current_msg);
    while (argv && *argv && *++argv && **argv == '-') {
	n = 1;
	while (n && argv[0][n])
	    switch (argv[0][n]) {
#ifdef VERBOSE_ARG
		case 'v': turnon(flgs, VERBOSE); n++; break;
#endif VERBOSE_ARG
		case 'e': turnon(flgs, EDIT); n++;
		when 'F': turnon(flgs, DO_FORTUNE); n++;
		when 'i': case 'h': case 'f': {
		    int m;
		    if (!msg_cnt) {
			print("No message to include!\n");
			return -1;
		    }
		    if (argv[0][n] == 'i')
			turnon(flgs, INCLUDE);
		    else if (argv[0][n] == 'h')
			turnon(flgs, INCLUDE_H);
		    else if (argv[0][n] == 'f')
			turnon(flgs, FORWARD);
		    /* "-i 3-5" or "-i3-5"  Consider the latter case first */
		    if (!argv[0][++n])
			argv++, n = 0;
		    (*argv) += n;
		    m = get_msg_list(argv, list);
		    (*argv) -= n;
		    if (m == -1)
			return -1;
		    /* if there were args, then go back to the first char
		     * in the next argv
		     */
		    if (m)
			n = 0;
		    if (!n) /* n may be 0 from above! */
			argv += (m-1);
		}
		otherwise:
		    if (argv[0][n] != '?')
			wprint("%c: unknown option\n", argv[0][n]);
		    wprint("available options\n");
#ifdef VERBOSE_ARG
		    wprint("v      verbose (passed to mail delivery system)\n");
#endif VERBOSE_ARG
		    wprint("e      immediately enter editor (autoedit)\n");
		    wprint("F      add fortune to the end of message.\n");
		    wprint("i [msg#'s]   include msg_list into letter.\n");
		    wprint("h [msg#'s]   include msg_list with headers.\n");
		    wprint("f [msg#'s]   forward msg_list (not indented).\n");
		    return -1;
	    }
    }
    *in_reply_to = *To = *Subject = *Cc = *Bcc = 0;
    if (lower(firstchar) == 'r') {
	char buf[BUFSIZ], *p;
	to = reply_to(current_msg, (firstchar == 'R'), To);
	if (firstchar == 'R') {
	    cc = cc_to(current_msg, Cc);
	    if (do_set(set_options, "fixaddr"))
		fix_addresses(To, Cc);
	}
	subj = subject_to(current_msg, Subject);
	if (do_set(set_options, "in_reply_to")) {
	    (void) reply_to(current_msg, FALSE, buf);
	    (void) sprintf(in_reply_to, "Message from %s", buf);
	    if (p = header_field(current_msg, "date"))
		(void) strcat(in_reply_to, p);
	}
    }
    if (argv && *argv) {
	char buf[BUFSIZ];
	(void) argv_to_string(buf, argv);
	to = strcat(To, buf);
    }
    if (do_set(set_options, "auto_route"))
	improve_uucp_paths(To), improve_uucp_paths(Cc);
    /* if fortune is set, check to see if fortunates is set. If so,
     * check to see if the recipient is on the fortunates list.
     */
    if (do_set(set_options, "fortune")) {
	register char *p2 = do_set(set_options, "fortunates");
	if (!to || !p2 || *p2 && (chk_two_lists(to, p2, " \t,") ||
		     (firstchar == 'r' && cc && chk_two_lists(cc, p2, " \t,"))))
	    turnon(flgs, DO_FORTUNE);
    }
#ifdef SUNTOOL
    if (istool) {
	do_clear();
	panel_set(abort_item,   PANEL_SHOW_ITEM, TRUE,  0);
	panel_set(comp_item,    PANEL_SHOW_ITEM, FALSE, 0);
	panel_set(read_item,    PANEL_SHOW_ITEM, FALSE, 0);
	panel_set(respond_item, PANEL_SHOW_ITEM, FALSE, 0);
    }
#endif SUNTOOL
    return mail_someone(to, subj, cc, flgs, list);
}

#ifdef SUNTOOL
/* panel item selection -- it's here because of local (static) variables */
abort_mail(item, value)
Panel_item item;
{
    get_hdr_field = 0;
    if (item == abort_item && value != 2) {
	print("Aborted letter.");
	killme = 1, rm_edfile(SIGINT);
	flags = 0;
    }
    panel_set(comp_item,    PANEL_SHOW_ITEM, TRUE,  0);
    panel_set(send_item,    PANEL_SHOW_ITEM, FALSE, 0);
    panel_set(edit_item,    PANEL_SHOW_ITEM, FALSE, 0);
    panel_set(abort_item,   PANEL_SHOW_ITEM, FALSE, 0);
    panel_set(read_item,    PANEL_SHOW_ITEM, TRUE,  0);
    panel_set(respond_item, PANEL_SHOW_ITEM, TRUE,  0);
    unlock_cursors();
}
#endif SUNTOOL

mail_someone(to, subject, cc, flgs, list)
register char *to, *subject, *cc, *list;
{
    register char *p;

    flags = flgs;
#ifdef SUNTOOL
    if (istool)
	rite(_tty.sg_kill), do_clear(), wprint("To: ");
#endif SUNTOOL
    if (to && *to) {
	if (!*To)
	    (void) strcpy(To, to);
	if (istool)
	    wprint("%s\n", To);
    } else
#ifdef SUNTOOL
	if (istool)
	    turnon(get_hdr_field, TO_FIELD);
	else
#endif SUNTOOL
	    to = NO_STRING;
    if (subject && *subject) {
	if (!*Subject)
	    (void) strcpy(Subject, subject);
	if (istool)
	    wprint("Subject: %s\n", Subject);
    } else
#ifdef SUNTOOL
	if (istool && !*Subject)
	    turnon(get_hdr_field, SUBJECT);
	else
#endif SUNTOOL
	    subject = NO_STRING;
    if (cc && *cc) {
	if (!*Cc)
	    (void) strcpy(Cc, cc);
	if (istool)
	    wprint("Cc: %s\n", Cc);
    } else
#ifdef SUNTOOL        /* get_hdr_field -- prevents prompting on replies */
	if (istool && get_hdr_field && do_set(set_options, "askcc"))
	    turnon(get_hdr_field, CC_FIELD);
	else
#endif SUNTOOL
	    cc = NO_STRING;

    if (ison(glob_flags, REDIRECT)) {
	send_it(); /* doesn't return */
	return 0;
    }
    /* if (!*to) then prompting will be done */
    if (!istool) {
	if (!(p = set_header("To: ", to, !*to)) || !*p) {
	    puts("No recipients, can't mail.");
	    return -1;
	}
	(void) strcpy(To, p);
	if (p = set_header("Subject: ", subject, !*subject))
	    (void) strcpy(Subject, p);
	if (*Cc)
	    printf("Cc: %s\n", Cc);
	putchar('\n');
    }
#ifdef SUNTOOL
      else if (!get_hdr_field) {
	panel_set(send_item, PANEL_SHOW_ITEM, TRUE, 0);
	panel_set(edit_item, PANEL_SHOW_ITEM, TRUE, 0);
    }
#endif SUNTOOL
    return start_file(list);
}

start_file(list)
char *list;
{
    register char  *home;
    register int   i;
    char  	   line[256];

    if (!(home = do_set(set_options, "home")) || !*home)
	home = ALTERNATE_HOME;
    (void) mktemp(sprintf(edfile, "%s/%s", home, EDFILE));
    {
	int omask = umask(077);
	fp = fopen(edfile, "w+");
	(void) umask(omask);
	if (!fp) {
#ifdef SUNTOOL
	    if (istool)
		abort_mail(NO_ITEM, 0);
#endif SUNTOOL
	    error("can't create %s", edfile);
	    return -1;
	}
    }
    if (!istool) {
	oldint = signal(SIGINT, rm_edfile);
	oldquit = signal(SIGQUIT, rm_edfile);
	oldterm = signal(SIGTERM, rm_edfile);
    }
    oldhup = signal(SIGHUP, rm_edfile);

    /* if flags call for it, include current message (with header?) */
    if (ison(flags, INCLUDE) || ison(flags,FORWARD) || ison(flags,INCLUDE_H)) {
	long copy_flgs = 0, is_forw = ison(flags, FORWARD);
	char buf[256];
	if (!is_forw)
	    turnon(copy_flgs, INDENT);
	if (ison(flags, INCLUDE))
	    turnon(copy_flgs, NO_HEADER);
	if (ison(flags, INCLUDE) || ison(flags, FORWARD))
	    turnon(copy_flgs, NO_IGNORE);
#ifdef SUNTOOL
		if (istool)
		    lock_cursors();
#endif SUNTOOL
	for (i = 0; i < msg_cnt; i++)
	    if (msg_bit(list, i)) {
		if (is_forw) {
		    (void) reply_to(i, FALSE, buf);
		    fprintf(fp, "--- Forwarded mail from %s\n\n", buf);
		}
		wprint("%sing message %d ...",
		    is_forw? "forward" : "includ", i+1);
		wprint("(%d lines)\n", copy_msg(i, fp, copy_flgs));
		set_isread(i); /* if we included it, we read it, right? */
		if (is_forw)
		    fprintf(fp,"\n--- End of forwarded message from %s.\n",buf);
	    }
	fflush(fp);
#ifdef SUNTOOL
	if (istool)
	    unlock_cursors();
#endif SUNTOOL
    }
    if (ison(glob_flags, WARNING)) {
	if (escape && *escape != DEF_ESCAPE[0])
	    wprint("(escape character is set to `%c')\n", *escape);
    }
    turnon(glob_flags, IS_GETTING);
#ifdef SUNTOOL
    /* enter editor if autoedit and replying to mail */
    if (istool && get_hdr_field)
	turnoff(flags, EDIT);
#endif SUNTOOL
    /* do an "if" again in case editor not found and EDIT turned off */
    if (ison(flags, EDIT)) {
	char *argv[3];
	argv[0] = (visual)? visual : editor;
	argv[1] = edfile;
	argv[2] = NULL;
	print("Starting \"%s\"...\n", argv[0]);
	in_editor = 1;
	fclose (fp);
	execute(argv);
	in_editor = 0;
	turnoff(flags, EDIT);
	fp = fopen(edfile, "r+");
	if (!fp) {
	    error("can't reopen %s", edfile);
	    return -1;
	}
	/* upon exit of editor, user must now type ^D or "." to send */
	if (istool)
	    return 0;
	fflush(fp), (void) fseek(fp, 0L, 2);
	puts("continue editing letter or ^D to send");
    }
#ifdef SUNTOOL
    if (istool) {
	pw_char(msg_win, txt.x,txt.y, PIX_SRC^PIX_DST, fonts[curfont], '_');
	win_setcursor(msg_sw->ts_windowfd, &write_cursor);
	return 0;
    }
#endif SUNTOOL
    i = 0;
    do  {
	/* If the user hits ^C in cbreak mode, mush will return to
	 * Getstr and not clear the buffer. whatever is typed next will
	 * be appended to the line.  jumping here will force the line to
	 * be cleared cuz it's a new call.
	 */
	(void) setjmp(cntrl_c_buf);
	while (Getstr(line, 256, 0) > -1)
	    if ((i = add_to_letter(line)) <= 0)
		break;
    } while (i >= 0 && !finish_up_letter());
    return 0;
}

char *tilde_commands[] = {
    "commands: [OPTIONAL argument]",
    "e [editor]\tEnter editor. Editor used: \"set editor\", env EDITOR, vi",
    "v [editor]\tEnter visual editor. \"set visual\", env VISUAL, vi",
    "p [pager]\tPage message; pager used: \"set pager\", env. PAGER, more",
    "i [msg#'s]\tInclude current msg body [msg#'s] indented by \"indent_str\"",
    "H [msg#'s]\tSame, but include the message headers from included messages",
    "f [msg#'s]\tForward mail. Not indented, but marked as \"forwarded mail\"",
    "t [list]\tChange list of recipients",
    "s [subject]\tModify [set] subject header",
    "c [cc list]\tModify [set] carbon copy recipients",
    "b [bcc list]\tModify [set] blind carbon recipients",
    "h\t\tModify all message headers",
    "S[!]\t\tInclude Signature file [suppress file]",
    "F[!]\t\tAdd a fortune at end of letter [don't add]",
    "w file\t\tWrite msg buffer to file name",
    "a file\t\tAppend msg buffer to file name",
    "r file\t\tRead filename into message buffer",
    "q \t\tQuit message; save in dead.letter (unless \"nosave\" is set).",
    "x \t\tQuit message; don't save in dead.letter.",
    "$variable\tInsert the string value for \"variable\" into message.",
    ":cmd\t\tRun the mail command \"cmd\".",
    "u\t\tedit previous line in file.",
    "E\t\tErase message buffer; clear all contents of letter",
    0
};

add_to_letter(line)
register char *line;
{
    register char *p;
    char buf[256];

    killme = 0;
    (void) fseek(fp, 0L, 2); /* seek to end in case more was added by editor */
#ifdef SUNTOOL
    if (get_hdr_field) {
	/* These are received in order by design! */
	if (ison(get_hdr_field, TO_FIELD)) {
	    if (!*line) {
	        wprint("There must be a recipient!\nTo: ");
		return 1;
	    }
	    (void) strcpy(To, line), turnoff(get_hdr_field, TO_FIELD);
	} else if (ison(get_hdr_field, SUBJECT)) {
	    (void) strcpy(Subject, line);
	    turnoff(get_hdr_field, SUBJECT);
	} else if (ison(get_hdr_field, CC_FIELD)) {
	    (void) strcpy(Cc, line);
	    turnoff(get_hdr_field, CC_FIELD);
	} else if (ison(get_hdr_field, BC_FIELD)) {
	    (void) strcpy(Bcc, line);
	    turnoff(get_hdr_field, BC_FIELD);
	}

        if (ison(get_hdr_field, SUBJECT))
	    (void) set_header("Subject: ", Subject, 1);
        else if (ison(get_hdr_field, CC_FIELD))
	    (void) set_header("Cc: ", Cc, 1);
        else if (ison(get_hdr_field, BC_FIELD))
	    (void) set_header("Bcc: ", Bcc, 1);
	panel_set(send_item, PANEL_SHOW_ITEM, (get_hdr_field==0), 0);
	panel_set(edit_item, PANEL_SHOW_ITEM, (get_hdr_field==0), 0);
	return 1;
    }
#endif SUNTOOL
    if (!strcmp(line, ".") && (istool || do_set(set_options, "dot")))
	return 0;
    if (*line != *escape) {
	fputs(line, fp), fputc('\n', fp), fflush(fp);
	return 1;
    }
    /* all commands are "~c" (where 'c' is the command). set p = first
     * character after 'c' and skip whitespace
     */
    p = line+2;
    skipspaces(0);
    switch (line[1]) {
	case 'v' : case 'p': case 'e':
	    if (!*p || *p == 'i')
		p = (line[1] == 'p')? pager:
		    (visual && line[1] == 'v')? visual: editor;
	    if (line[1] == 'p') {
		wprint("To: %s\n", To);
		if (Subject[0])
		    wprint("Subject: %s\n", Subject);
		if (Cc[0])
		    wprint("Cc: %s\n", Cc);
		if (Bcc[0])
		    wprint("Bcc: %s\n", Bcc);
		wprint("-----------\nMessage contains:\n");
	    }
	    if (line[1] == 'p' && (istool || !istool && lines_in(fp, crt))) {
		rewind(fp);
		while (fgets(buf, BUFSIZ, fp))
#ifdef SUNTOOL
		    if (istool)
			Addstr(buf);
		    else
#endif SUNTOOL
			(void) fputs(buf, stdout);
	    } else {
	        int doedit = line[1] != 'p';
		char *argv[3];
		argv[0] = p;
		argv[1] = edfile;
		argv[2] = NULL;
		in_editor = 1;
		if (doedit)
			fclose (fp);
		execute(argv); /* page the message using pager */
		in_editor = 0;
		if (doedit && !(fp = fopen (edfile, "r+"))) {
			error ("can't reopen %s", edfile);
			return -1;
		}
		if (istool)
		    return 1;
	    }
	when '$': {
	    register char *p2;
	    if (!(p2 = do_set(set_options, p)))
		wprint("(%s isn't set)\n", p);
	    else
		fprintf(fp, "%s\n", p2);
	}
	when ':': {
	    char new[MAXMSGS_BITS];
	    long save_flags = glob_flags;

	    turnon(glob_flags, IGN_SIGS);
	    turnon(glob_flags, IGN_BANG);
	    turnoff(glob_flags, DO_PIPE);
	    turnoff(glob_flags, IS_PIPE);
	    (void) cmd_line(p, new);
	    glob_flags = save_flags;
#ifdef SUNTOOL
	    if (istool && msg_pix) /* the command was to read a message */
		return 1;
#endif SUNTOOL
	}
	when 'i': case 'f': case 'H': case 'm': {
	    int  n;
	    long copy_flgs = 0;
	    char list[MAXMSGS_BITS];

	    if (!msg_cnt) {
		print("No messages.\n");
		break;
	    }
	    clear_msg_list(list);
	    if (line[1] != 'f')
		turnon(copy_flgs, INDENT);
	    if (line[1] == 'i')
		turnon(copy_flgs, NO_HEADER);
	    if (!*p)
		set_msg_bit(list, current_msg);
	    else if (!do_range(p, list))
		return 1;
#ifdef SUNTOOL
	    if (istool)
		lock_cursors();
#endif SUNTOOL
	    for (n = 0; n < msg_cnt; n++)
		if (msg_bit(list, n)) {
		    if (line[1] == 'f') {
			(void) reply_to(n, FALSE, buf);
			fprintf(fp, "--- Forwarded mail from %s\n\n", buf);
		    }
		    wprint("Including message %d ... ", n+1);
		    wprint("(%d lines)\n", copy_msg(n, fp, copy_flgs));
		    set_isread(n);
		    if (line[1] == 'f')
		fprintf(fp, "\n--- End of forwarded message from %s\n\n", buf);
		}
#ifdef SUNTOOL
	    if (istool)
		unlock_cursors();
#endif SUNTOOL
	}
	when 't':
#ifdef SUNTOOL
	    if (!*p && istool) {
		turnon(get_hdr_field, TO_FIELD);
		(void) set_header("To: ", To, 1);
		panel_set(send_item, PANEL_SHOW_ITEM, FALSE, 0);
		panel_set(edit_item, PANEL_SHOW_ITEM, FALSE, 0);
		return 1;
	    }
#endif SUNTOOL
	    /* ~t address   is a special case ... append to address */
	    if (*p)
		(void) sprintf(To+strlen(To), " %s", p);
	    else if (p = set_header("To: ", To, 1))
		if (!*p) {
		    wprint("There must be a recipient!\n");
#ifdef SUNTOOL
		    turnoff(get_hdr_field, TO_FIELD);
		    panel_set(send_item, PANEL_SHOW_ITEM, TRUE, 0);
		    panel_set(edit_item, PANEL_SHOW_ITEM, TRUE, 0);
#endif SUNTOOL
		} else
		    (void) strcpy(To, p);
	when 's':
#ifdef SUNTOOL
	    if (!*p && istool) {
		turnon(get_hdr_field, SUBJECT);
		panel_set(send_item, PANEL_SHOW_ITEM, FALSE, 0);
		panel_set(edit_item, PANEL_SHOW_ITEM, FALSE, 0);
		(void) set_header("Subject: ", Subject, 1);
		return 1;
	    }
#endif SUNTOOL
	    if (*p || (p = set_header("Subject: ", Subject, 1)))
		if (!*p)
		    Subject[0] = 0;
		else
		    (void) strcpy(Subject, p);
	when 'c':
#ifdef SUNTOOL
	    if (!*p && istool) {
		turnon(get_hdr_field, CC_FIELD);
		(void) set_header("Cc: ", Cc, 1);
		panel_set(send_item, PANEL_SHOW_ITEM, FALSE, 0);
		panel_set(edit_item, PANEL_SHOW_ITEM, FALSE, 0);
		return 1;
	    }
#endif SUNTOOL
	    if (*p || (p = set_header("Cc: ", Cc, 1)))
		if (!*p)
		    Cc[0] = 0;
		else
		    (void) strcpy(Cc, p);
	when 'b':
#ifdef SUNTOOL
	    if (!*p && istool) {
		turnon(get_hdr_field, BC_FIELD);
		(void) set_header("Bcc: ", Bcc, 1);
		panel_set(send_item, PANEL_SHOW_ITEM, FALSE, 0);
		panel_set(edit_item, PANEL_SHOW_ITEM, FALSE, 0);
		return 1;
	    }
#endif SUNTOOL
	    if (*p || (p = set_header("Bcc: ", Bcc, 1)))
		if (!*p)
		    Bcc[0] = 0;
		else
		    (void) strcpy(Bcc, p);
	when 'h':
#ifdef SUNTOOL
	    if (istool) {
		turnon(get_hdr_field, TO_FIELD);
		turnon(get_hdr_field, SUBJECT);
		turnon(get_hdr_field, CC_FIELD);
		turnon(get_hdr_field, BC_FIELD);
		(void) set_header("To: ", To, 1);
		panel_set(send_item, PANEL_SHOW_ITEM, FALSE, 0);
		panel_set(edit_item, PANEL_SHOW_ITEM, FALSE, 0);
		return 1;
	    }
#endif SUNTOOL
	    while ((p = set_header("To: ", To, 1)) && !*p)
		wprint("(There must be a recipient.)\n");
	    (void) strcpy(To, p);
	    if (p = set_header("Subject: ", Subject, 1))
		if (!*p)
		    Subject[0] = 0;
		else
		    (void) strcpy(Subject, p);
	    if (p = set_header("Cc: ", Cc, 1))
		if (!*p)
		    Cc[0] = 0;
		else
		    (void) strcpy(Cc, p);
	    if (p = set_header("Bcc: ", Bcc, 1))
		if (!*p)
		    Bcc[0] = 0;
		else
		    (void) strcpy(Bcc, p);
	when 'S':
	    if (*p == '!')
		turnoff(flags, SIGN), wprint("not ");
	    else
		turnon(flags, SIGN);
	    wprint("adding signature file at end of message.\n");
	when 'F':
	    if (*p == '!')
		turnoff(flags, DO_FORTUNE), wprint("not ");
	    else
		turnon(flags, DO_FORTUNE);
	    wprint("adding fortune at end of message.\n");
	when 'w': case 'a': case 'r':
	    if (!*p) {
		wprint("(you must specify a filename)\n");
		return 1;
	    }
	    (void) fseek(fp, 0L, 2); /* append */
	    file_to_fp(p, fp, (line[1] == 'r')? "r":
			      (line[1] == 'w')? "w": "a");
	/* go up one line in the message file and allow the user to edit it */
	when 'u': {
	    long newpos, pos = ftell(fp);
	    char oldline[256];
	    if (istool) {
		wprint("(Not available in tool mode.)\n");
		return 1;
	    }
	    if (pos <= 0L) { /* pos could be -1 if ftell() failed */
		wprint("(No previous line in file.)\n");
		return 1;
	    }
	    /* get the last 256 bytes written and read backwards from the
	     * current place until '\n' is found. Start by moving past the
	     * first \n which is at the end of the line we want to edit
	     */
	    newpos = max(0, pos - 256);
	    (void) fseek(fp, newpos, L_SET);
	    /* don't fgets -- it'll stop at a \n */
	    (void) read(fileno(fp), line, (int)(pos-newpos));
	    pos--;
	    /* the last char in line should be a \n cuz it was last input */
	    if (line[(int)(pos-newpos)] != '\n')
		wprint("I don't know how, but your last line ended with %c.\n",
		    line[(int)(pos-newpos)]);
	    else
		line[(int)(pos-newpos)] = 0; /* null terminate \n for ^H-ing */
	    for (pos--; pos > newpos && line[(int)(pos-newpos)] != '\n'; pos--)
		;
	    /* we've gone back to the end of the second previous line. Check
	     * to see if the char we're pointing to is a \n.  It should be, but
	     * if it's not, we moved back to the first line of the file.
	     */
	    if (line[(int)(pos-newpos)] == '\n')
		++pos;
	    /* save the old line that's there in case the user boo-boo's */
	    (void) strcpy(oldline, line+(int)(pos-newpos));
	    /* let set header print out the line and get the input */
	    if (!(p = set_header("", line+(int)(pos-newpos), TRUE))) {
		wprint("Something bad happened and I don't know what it is.\n");
		p = oldline;
	    } else if (*p == *escape && *++p != *escape) {
		wprint("(No %c escapes on %cu lines. Line unchanged.)\n",
				*escape, *escape);
		p = oldline;
	    }
	    /* seek to to the position where the new line will go */
	    (void) fseek(fp, pos, L_SET);
	    /* put the newly typed line */
	    (void) fputs(p, fp); /* don't add \n in case padding is necessary */
	    /* if the new line is less than the old line, we're going to do
	     * one of two things.  The best thing to do is to truncate the
	     * file to the end of the new line.  Sys-v can't do that, so we
	     * pad the line with blanks.  May be messy in some cases, but...
	     */
	    if ((pos = strlen(p) - strlen(oldline)) < 0) {
#ifndef SYSV
		/* add the \n, flush the file, truncate to the current pos */
		fputc('\n', fp), fflush(fp);
		(void) ftruncate(fileno(fp), (int)ftell(fp));
#else
		/* pad with blanks to the length of the old line. add \n */
		while (pos++ < 0)
		    fputc(' ', fp);
		fputc('\n', fp), fflush(fp);
#endif SYSV
	    } else
		/* the new line is >= the old line, add \n -- no trunc req. */
	        fputc('\n', fp);
	    return 1;
	 }
	/* break;  not here cuz of "return" (lint). */
	case 'E':
	    wprint("Message buffer empty\n");
	    if (emptyfile(&fp, edfile) == -1)
		error(edfile);
	when 'q':
	    /* save in dead.letter if nosave not set -- rm_edfile(-2). */
	    rm_edfile(-2); /* doesn't return out of tool mode */
	    return -1;
	    /* break; not stated cuz of "return" (lint) */
	case 'x':
	    /* don't save dead.letter -- simulate normal rm_edfile() call */
	    rm_edfile(0);
#ifdef SUNTOOL
	    if (istool) {
		wprint("*Letter aborted*");
		pw_char(msg_win, txt.x,txt.y, PIX_CLR, fonts[curfont], '_');
	    }
#endif SUNTOOL
	    return -1;
	    /* break; (not specified for lint) */
	default:
	    if (line[1] == *escape) {
		fputs(line+1, fp), fputc('\n', fp), fflush(fp);
		return 1;
	    } else {
		register int x;
		for (x = 0; tilde_commands[x]; x++)
		    wprint("%s%s\n", escape, tilde_commands[x]);
		wprint("%s%s\t\tbegin a line with a single %s\n",
		    escape, escape, escape);
#ifdef SUNTOOL
		if (istool)
		    (void) help(0, "compose", tool_help);
#endif SUNTOOL
	    }
    }
    (void) fseek(fp, 0L, 2); /* seek to end of file in case there's more */
    wprint("(continue editing letter)\n");
#ifdef SUNTOOL
    if (istool)
	pw_char(msg_win, txt.x,txt.y, PIX_SRC, fonts[curfont], '_');
#endif SUNTOOL
    return 1;
}

/*
 * finish up the letter. ask for the cc line, if verify is set, ask to
 * verify sending, continue editing, or to dump the whole idea.
 * Then check for signature and fortune.  Finally, pass it to send_it()
 * to actually send it off.
 */
finish_up_letter()
{
    register char *p;
    char buf[256];

#ifdef SUNTOOL
    if (istool)
	lock_cursors();
    else
#endif SUNTOOL
    if (isoff(glob_flags, REDIRECT)) {
	if (do_set(set_options, "askcc") && (p = set_header("Cc: ", Cc, 1)))
	    (void) strcpy(Cc, p);
	/* ~v on the Cc line asks for verification, first initialize p! */
	p = NULL;
	if (!strncmp(Cc, "~v", 2) || (p = do_set(set_options, "verify"))) {
	    if (!p) /* so we don't Cc to ~v! */
		*Cc = 0;
	    fprintf(stderr, "send, continue editing, discard [s,c,d]? ");
	    if (Getstr(buf, 256, 0) < 0 || lower(*buf) == 'd') {
		rm_edfile(-2);
		return 1;
	    } else if (lower(*buf) == 'c') {
		puts("(continue editing letter)");
		return 0;
	    }
	}
    }

    if (ison(flags, SIGN)) {
	char *home = do_set(set_options, "home");
	if (!home || !*home)
	    home = ALTERNATE_HOME;
	if (!(p = do_set(set_options, "autosign")) || !*p)
	    (void) sprintf(buf, "%s/%s", home, SIGNATURE);
	else
	    (void) strcpy(buf, p);
	wprint("Signing letter... "), fflush(stdout);
	fputc('\n', fp), fflush(fp);
	(void) fseek(fp, 0L, 2); /* guarantee position at end of file */
	if (*buf == '$')
	    if (!(p = do_set(set_options, buf)))
		wprint("(%s isn't set -- letter not signed)\n", buf);
	    else
		fprintf(fp, "%s\n", p), wprint("\n"), fflush(fp);
	else if (*buf == '\\')
	    fprintf(fp, "%s\n", buf+1), wprint("\n"), fflush(fp);
	else
	    file_to_fp(buf, fp, "r");
    }

    if (ison(flags, DO_FORTUNE)) {
	char 	foo[256];
	FILE 	*pp2;
	int 	lines = 0;

	wprint("You may be fortunate... "), fflush(stdout);
	if ((p = do_set(set_options, "fortune")) && *p == '/')
	    (void) strcpy(foo, p);
	else
	    (void) sprintf(foo, "%s %s", FORTUNE, (p && *p == '-')? p: "-s");
	if (!(pp2 = popen(foo, "r")))
	    error(foo);
	else {
	    turnon(glob_flags, IGN_SIGS);
	    (void) fseek(fp, 0L, 2); /* go to end of file */
	    while (fgets(foo, 256, pp2))
		fputs(foo, fp), lines++;
	    (void) pclose(pp2);
	    turnoff(glob_flags, IGN_SIGS);
	    fflush(fp);
	    wprint("added %d line%s\n", lines, lines == 1? "" : "s");
	}
    }
    send_it();
    turnoff(glob_flags, IS_GETTING);
    return 1;
}

/*
 * actually send the letter.
 * 1. Reset all the signals because of fork.
 * 2. determine recipients (users, address, files, programs)
 * 3. Determine mailer, fork and return (if not verbose).
 * 4. popen mailer, $record, and other files specified in step 1.
 * 5. make the headers; this includes To: line, and user set hedaers, etc...
 * 6. copy the letter right into the array of file pointers (step 1).
 * 7. close the mailer and other files (step 1) and remove the edit-file.
 */
static void
send_it()
{
    register char *p;
#ifdef MAXFILES
    register int size = MAXFILES - 1;
    FILE *files[MAXFILES];
#else
    register int size = getdtablesize() - 1;
    FILE *files[30];  /* 30 should be sufficiently large enough */
#endif /* MAXFILES */
    int next_file = 1; /* reserve files[0] for the mail delivery program */
    char buf[3*BUFSIZ];

    if (!istool) {
	(void) signal(SIGINT, oldint);
	(void) signal(SIGQUIT, oldquit);
	(void) signal(SIGTERM, oldterm);
    }
    (void) signal(SIGHUP, oldhup);

#ifdef VERBOSE_ARG
    if (ison(flags, VERBOSE) || do_set(set_options, "verbose"))
	(void) sprintf(buf, "%s %s", MAIL_DELIVERY, VERBOSE_ARG);
    else
#endif VERBOSE_ARG
	(void) strcpy(buf, MAIL_DELIVERY);
#ifdef METOO_ARG
    if (do_set(set_options, "metoo"))
	(void) sprintf(buf+strlen(buf), " %s", METOO_ARG);
#endif METOO_ARG

    /*
     * For alias expansion on To, Cc and Bcc lines, first expand the recipients
     * lists so that aliases are expanded.  Then detemine which items in the
     * list are files or programs and remove those from the list. Finally,
     * copy the resulting buffer back into the original To, Cc, and Bcc buffer.
     * rm_edfile(-2) will save in dead letter (only if "nosave" set) and
     * will long jump back to main loop if any alias expansions fail. Insure
     * a forced dead letter by rm_edfile(-1). But, rm_edfile will exit with -1
     * arg, so kludge by turning on the VERBOSE bit in "flags".
     */
    if (!(p = alias_to_address(To))) {
	print("address expansion failed for To: line.\n");
	turnon(flags, VERBOSE);
	rm_edfile(-1);
    } else {
	next_file += find_files(p, files+next_file, size - next_file);
	if (!*strcpy(To, p)) {
	    print("There must be at least 1 legal recipient on the To line\n");
	    while (--next_file > 1)
		fclose(files[next_file]);
	    rm_edfile(-2);
	    return;
	}
    }
    if (*Cc)
	if (!(p = alias_to_address(Cc))) {
	    print("address expansion failed for Cc: line.\n");
	    turnon(flags, VERBOSE);
	    while (--next_file > 1)
		fclose(files[next_file]);
	    rm_edfile(-1);
	} else {
	    next_file += find_files(p, files+next_file, size - next_file);
	    (void) strcpy(Cc, p);
	}
    if (*Bcc)
	if (!(p = alias_to_address(Bcc))) {
	    print("address expansion failed for Bcc: line.\n");
	    turnon(flags, VERBOSE);
	    while (--next_file > 1)
		fclose(files[next_file]);
	    rm_edfile(-1);
	} else {
	    next_file += find_files(p, files+next_file, size - next_file);
	    (void) strcpy(Bcc, p);
	}

    /*
     * build the mailer's "command line" -- we build our own headers later.
     * the mailer will mail to everyone listed.  The headers will contain
     * the to and cc lines  -- Bcc is never printed.
     */
    (void) sprintf(buf+strlen(buf), " %s %s %s", To, Cc, Bcc);

#ifdef SYSV
    /*
     * Sys-v does not recover from SIGCLD elegantly. That is, all system calls
     * return -1 and does not complete whatever it was doing when the signal
     * gets delievered -- we could be reading from stdin, a file, or feeding
     * to a pager -- this isn't good.  I'm not good enough at SYSV to know how
     * to recover from this correctly.  BSD systems recover elegantly and
     * deliver the sigchld properly.
     */
    turnon(flags, VERBOSE);
#endif SYSV

    Debug("mail command: %s\n", buf);

#ifdef SUNTOOL
    if (istool)
	abort_mail(NO_ITEM, 0);
#endif SUNTOOL

    if (isoff(flags, VERBOSE) && debug < 3)
	switch (fork()) {
	    case  0:  /* the child will send the letter. ignore signals */
		(void) signal(SIGINT, SIG_IGN);
		(void) signal(SIGHUP, SIG_IGN);
		(void) signal(SIGQUIT, SIG_IGN);
		(void) signal(SIGTERM, SIG_IGN);
#ifdef SIGCONT
		(void) signal(SIGCONT, SIG_IGN);
		(void) signal(SIGTSTP, SIG_IGN);
#endif SIGCONT
		turnon(glob_flags, IGN_SIGS);
		break;
	    case -1:
		error("fork failed trying to send mail");
	    default:
		if (isoff(glob_flags, REDIRECT))
		    fclose(fp);
#ifdef SUNTOOL
                if (istool) {
		    wprint("Letter sent.");
		    print("Letter sent.");
		    pw_char(msg_win, txt.x,txt.y, PIX_CLR, fonts[curfont], '_');
		}
#endif SUNTOOL
		while (--next_file > 0)
		    fclose(files[next_file]);
		return;
	}

    if (debug > 2)
	files[0] = stdout;
    else if (!(files[0] = open_file(buf, TRUE))) {
	rm_edfile(-1); /* force saving of undeliverable mail */
	return;
    }

    if (ison(flags, VERBOSE))
	wprint("Sending letter ... "), fflush(stdout);

    /* see if record is set.  If so, open that file for appending and add
     * the letter in a format such that mail can be read from it
     */
    if (p = do_set(set_options, "record")) {
	if (!*p)
	    p = "~/record";
	(void) strcpy(buf, p);
	next_file += find_files(buf, files+next_file, size - next_file);
    }

    /* don't send this to Sendmail --make folders conform to RFC-822 */
    for (size = 1; size < next_file; size++)
	if (files[size]) {
	    time_t t;
	    (void) time(&t);
	    fprintf(files[size], "From %s %s", login, ctime(&t));
	    fprintf(files[size], "From: %s\n", login);
	    fprintf(files[size], "Date: %s", ctime(&t));
	    fprintf(files[size], "Status: OR\n");
	}

    /* first print users own message headers */
    if (own_hdrs && !do_set(set_options, "no_hdrs")) {
	struct options *opts;
	for (opts = own_hdrs; opts; opts = opts->next)
	    for (size = 0; size < next_file; size++)
		fprintf(files[size], "%s %s\n", opts->option, opts->value);
    }

    /* send the header stuff to sendmail and end header with a blank line */
    if (*in_reply_to)
	for (size = 0; size < next_file; size++)
	    fprintf(files[size], "In-Reply-To: %s\n", in_reply_to);
    for (size = 0; size < next_file; size++)
	fprintf(files[size], "X-Mailer: %s\n", VERSION);
    for (size = 0; size < next_file; size++)
	fprintf(files[size], "To: %s\n", To);
    if (*Subject)
	for (size = 0; size < next_file; size++)
	    fprintf(files[size], "Subject: %s\n", Subject);
    if (*Cc)
	for (size = 0; size < next_file; size++)
	    fprintf(files[size], "Cc: %s\n", Cc);

    for (size = 0; size < next_file; size++)
	fputc('\n', files[size]);

    /* if redirection, fp = stdin, else rewind the file just made */
    if (isoff(glob_flags, REDIRECT))
	rewind(fp);
    else
	fp = stdin;

    /* read from stdin or the edfile till EOF and send it all to the mailer */
    while (fgets(buf, BUFSIZ, fp))
	for (size = 0; size < next_file; size++) {
	    if (!strncmp(buf, "From ", 5))
		fputc('>', files[size]);
	    fputs(buf, files[size]);
	}

    for (size = 1; size < next_file; size++)
	if (files[size])
	    fclose(files[size]); /* if it was popened, sigchld will close it */

    rm_edfile(0);
    if (debug < 3)
	(void) pclose(files[0]);

    if ((ison(flags, VERBOSE) || debug > 2) && isoff(glob_flags, REDIRECT))
	wprint("sent.\n");
    else
	exit(0); /* not a user exit -- a child exit */
}

/* ARGSUSED */
rm_edfile(sig)
{
    if (sig > 0 && !killme) {
	(void) signal(sig, rm_edfile);
	killme = 1;
	wprint("\n** interrupt -- one more to kill letter **\n");
#ifdef SUNTOOL
	if (istool) {
	    pw_char(msg_win, txt.x,txt.y, PIX_SRC, fonts[curfont], '_');
	    return;
	}
#endif SUNTOOL
	longjmp(cntrl_c_buf, 1);
    }
    in_editor = killme = 0;
    /* if sig == -1, force a save into dead.letter.
     * else, check for nosave not being set and save anyway if it's not set
     * sig == 0 indicates normal exit (or ~x), so don't save a dead letter.
     */
    if (sig == -1 || sig != 0 && !do_set(set_options, "nosave")) 
	dead_letter();
    if (isoff(glob_flags, REDIRECT))
	fclose(fp);
    (void) unlink(edfile);

    if (sig == -1 && isoff(flags, VERBOSE) && debug < 3)
	exit(-1);

    turnoff(glob_flags, IS_GETTING);
#ifdef SUNTOOL
    if (sig && istool > 1) {
	wprint("*Letter aborted*");
	abort_mail(abort_item, 2);
    }
#endif SUNTOOL

    if (sig == SIGHUP)
	cleanup(0);
    (void) signal(SIGHUP, oldhup);
    if (!istool) {
	(void) signal(SIGINT, oldint);
	(void) signal(SIGQUIT, oldquit);
	(void) signal(SIGTERM, oldterm);
    }

    if (sig == 0)
	return;
    if (istool || sig == -2) /* make sure sigchld is reset first */
	return;

    if (isoff(glob_flags, DO_SHELL)) {  /* If we're not in a shell, exit */
	puts("exiting");
	echo_on();
	exit(1);
    }
    longjmp(jmpbuf, 1);
}

/* save letter into dead letter */
dead_letter()
{
    char 	*p, buf[BUFSIZ];
    long 	t;
    FILE 	*dead;

    if (ison(glob_flags, REDIRECT)) {
	print("input redirected -- can't save dead letter.\n");
	return;
    }
    /* don't save a dead letter if there's nothing to save. */
    if (fseek(fp, 0L, 2) || ftell(fp) == 0L)
	return;
    if (!(p = do_set(set_options, "dead")))
	p = "~/dead.letter";
    if (!(dead = open_file(p, FALSE)))
	return;
    (void) time (&t);
    fflush(fp);
    rewind(fp);
    fprintf(dead, "Unfinished letter from %s", ctime(&t));
    fprintf(dead, "To: %s\nSubject: %s\nCc: %s\n", To, Subject, Cc);
    while(fgets(buf, BUFSIZ, fp))
	(void) fputs(buf, dead);
    (void) fputc('\n', dead);
    (void) fclose(dead);
    print("Saved unfinished letter in %s.\n", p);
}
