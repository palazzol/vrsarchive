/* @(#)msgs.c	(c) copyright 10/18/86 (Dan Heller) */

#include "mush.h"
#ifdef SYSV
#ifdef M_XENIX
#include <sys/locking.h>
#else
#include <unistd.h>
#define locking(a,b,c)	lockf(a,b,(long)(c))
#define LK_LOCK		F_LOCK
#define LK_UNLCK	F_ULOCK
#endif /*M_XENIX*/
#endif /*SYSV*/

void
display_msg(n, flg)
register int n;
long flg;
{
    register FILE *pp;

    if (ison(msg[n].m_flags, DELETE)) {
	print("Message %d deleted; ", n+1);
	if (istool)
	    print_more("Select UNDELETE to read.");
	else if (iscurses)
	    print_more("Type 'u' to undelete.");
	else
	    print("Type 'undelete %d' to undelete\n", n+1);
	return;
    }
    set_isread(n);
    if (ison(flg, TOP)) {
	turnon(flg, NO_HEADER);
	print("Top of "), turnon(glob_flags, CONT_PRNT);
    }

    if (!istool && isoff(flg, NO_PAGE) &&
	    crt < msg[n].m_lines && isoff(flg, TOP)) {
	turnon(glob_flags, IGN_SIGS);
	echo_on();
	if (!(pp = popen(pager, "w")))
	    error(pager);
	else {
	    fprintf(pp, "Message #%d (%d lines)\n", n+1, msg[n].m_lines);
	    (void) copy_msg(n, pp, flg);
	    (void) pclose(pp);
	}
	echo_off();
	turnoff(glob_flags, IGN_SIGS);
    } else {
	print("Message #%d (%d lines)\n", n+1, msg[n].m_lines);
	(void) copy_msg(n, stdout, flg);
    }
}

/*
 * copy message 'n' to file "fp" according to various flag arguments
 * return number of lines copied or -1 if system error on fputs.
 */
copy_msg(n, fp, flags)
register int n;
long flags;
register FILE *fp;
{
    register int  ignoring = 0, lines = 0;
    register char *indent_str;
    int  on_hdr = 1, top, squeeze = FALSE;
    char 	  line[BUFSIZ];

    still_more = 0;
    if (ison(flags, TOP)) {
	register char *p = do_set(set_options, "toplines");
	top = (p)? atoi(p) : crt;
    }
    if (do_set(set_options, "alwaysignore"))
	turnoff(flags, NO_IGNORE);
    if (isoff(flags, NO_IGNORE) && do_set(set_options, "squeeze"))
	squeeze = TRUE;

#ifdef SUNTOOL
    if (istool && fp == stdout) {
	register int x = (msg[n].m_lines + 2) * l_height(curfont);
	if (x > 32765) { /* to overcome a bug in pixrects that sun won't fix */
	    print("message too big to display using this font");
	    return 0;
	}
	if (x < msg_rect.r_height) /* make it at least as big as the window */
	    x = msg_rect.r_height;
	do_clear();
	lock_cursors();
	if (!(msg_pix = mem_create(msg_rect.r_width, x, 1))) {
	    error("mem_create");
	    return 0;
	}
	pr_rop(msg_pix, 0,0, msg_rect.r_width-1, x-1, PIX_CLR, 0,0,0);
	on_hdr = 1;
    }
#endif SUNTOOL
    if (ison(flags, INDENT) &&
       (!(indent_str = do_set(set_options, "indent_str")) ||
	!strcmp(indent_str, "indent_str")))
	indent_str = DEF_INDENT_STR;
    (void) fseek(tmpf, msg[n].m_offset, L_SET);
    while (still_more < msg[n].m_size && fgets(line, BUFSIZ, tmpf)) {
	still_more += strlen(line);

	/*
	 * If squeeze is one, all blanks lines squeeze down to one blank line.
	 * If squeeze is two, squeezing is in progress. Otherwise, wait for \n.
	 */
	if (*line == '\n') {
	    if (on_hdr)    /* blank line -- end of header */
		turnoff(flags, NO_HEADER), on_hdr = 0;
	    if (squeeze > 1)
		continue;
	    else if (squeeze)
		squeeze = 2;
	} else if (squeeze > 1)
	    squeeze = 1;

	if (ison(flags, UPDATE_STATUS))
	    if (!strncmp(line, "Status:", 7))
		continue; /* ignore this and other "Status" lines */
	    else if (!on_hdr) {
		/* preserve NEW/UNREAD status on preserved messages */
		if (isoff(msg[n].m_flags, PRESERVE)) {
		    (void) strcpy(line, "Status: O");
		    if (isoff(msg[n].m_flags, UNREAD))
			(void) strcat(line, "R");
		    (void) strcat(line, "\n");
		    fputs(line, fp);
		    (void) strcpy(line, "\n");
		}
		turnoff(flags, UPDATE_STATUS);
	    }
	if (on_hdr && isoff(flags, NO_IGNORE)) {
	    register char *p = any(line, " \t:");
	    if (!p)
		ignoring = 0, on_hdr = 0;
	    else if (ignoring)
		if (*p != ':') {
		    Debug("Ignoring: %s", line);
		    continue;
		}
		else
		    ignoring = 0;
	    if (p && *p == ':') {
		register struct options *opts;
		*p = 0;
		ignoring = 0;
		for (opts = ignore_hdr; opts; opts = opts->next)
		    if (!lcase_strcmp(opts->option, line)) {
			ignoring = 1;
			break;
		    }
		*p = ':';
		if (ignoring) {
		    Debug("Ignoring: %s", line);
		    continue;
		}
	    }
	}
	if (!on_hdr && ison(flags, TOP) && !--top)
	    break;
	if (isoff(flags, NO_HEADER)) {
	    /* note that function returns the number of lines */
	    lines++;
#ifdef SUNTOOL
	    if (istool && fp == stdout) {
		Addstr(line);
		continue;
	    }
#endif SUNTOOL
	    if (ison(flags, INDENT))
		fputs(indent_str, fp);
	    (void) fputs(line, fp);
	    if (errno == ENOSPC)
		return -1;
	}
    }
#ifdef SUNTOOL
    if (istool && fp == stdout) {
	unlock_cursors();
	txt.y = still_more = msg_rect.r_height;
	scroll_win(0);  /* causes a display */
    }
#endif SUNTOOL
    return lines;
}

/* get mail from whatever the mailfile points to. open a tempfile for
 * appending, then close it and reopen it for read-only.  some systems
 * have flakey read/write access.
 */
void
getmail()
{
    register FILE	*mail_fp;
    int 	lines = 0, get_status = 1;
    long 	ftell(), bytes;
    char	line[BUFSIZ];

    if (!(mail_fp = fopen(mailfile, "r"))) {
	error("Cannot open %s", mailfile);
	return;
    }
    /*
     * since this file is usually open for read-only, close it and then
     * reopen it for appending.  This is done to compensate for errors
     * in XENIX and to play it safe with non-essentially writable files.
     * see more notes below (end of proc).
     */
    (void) fclose(tmpf);
    {
	int	omask = umask(077);
	tmpf = fopen(tempfile, "a");
	(void) umask(omask);
	if (!tmpf) {
	    error("can't open %s for appending", tempfile);
	    (void) fclose(mail_fp);
	    return;
	}
    }
#ifdef SYSV
    (void) locking(fileno(mail_fp), LK_LOCK,  0);
#else
#ifdef BSD
    if (flock(fileno(mail_fp), LOCK_SH))  /* if file is locked, flock waits */
	error("WARNING: could not lock %s", mailfile);
#endif BSD
#endif SYSV

    (void) fseek(mail_fp, ftell(tmpf), 0);

    while (fgets(line, BUFSIZ, mail_fp) != NULL) {
	if (!strncmp(line, "From ", 5)) {
	    if (msg_cnt == MAXMSGS-1) {
		print("WARNING: exceeded %d messages.\n", MAXMSGS);
		print("You should split \"%s\" into smaller files.\n",mailfile);
		/* make sure that tempfile isn't removed!! */
		turnon(glob_flags, IGN_SIGS);
		cleanup(0); /* probably a more elegant way to exit, but... */
	    }
	    bytes = ftell(tmpf);
	    /* finish up message structure from previous message.
	     * if this is incorporating new mail, check "lines" to
	     * see if previous message has already been set!
	     */
	    if (msg_cnt && lines) {
		msg[msg_cnt-1].m_size = bytes - msg[msg_cnt-1].m_offset;
		msg[msg_cnt-1].m_lines = lines;
	    }
	    msg[msg_cnt].m_offset = bytes;
	    msg[msg_cnt].m_flags = lines = 0;
	    turnon(msg[msg_cnt].m_flags, UNREAD); /* initialize */

	    fputs(line, tmpf);
	    if (errno == ENOSPC)
		fs_error();

	    /* we've read the "From " line, now read the rest of
	     * the message headers till we get to a blank line.
	     */
	    while (fgets(line, BUFSIZ, mail_fp) && (*line != '\n')) {
		register char *p = line;
		if (get_status && !(get_status = strncmp(p, "Status:", 7))) {
		    turnon(msg[msg_cnt].m_flags, OLD);
		    for (p += 8 ; *p != '\n'; p++)
			switch(*p) {
			    case 'R': turnoff(msg[msg_cnt].m_flags, UNREAD);
			}
		}
		fputs(line, tmpf), lines++;
		if (errno == ENOSPC)
		    fs_error();
	    }
	    msg_cnt++, get_status = 1;
	}
	fputs(line, tmpf), lines++;
	if (errno == ENOSPC)
	    fs_error();
    }
    /* msg_cnt may be 0 if there is an error with the format of mailfile */
    if (msg_cnt) {
	msg[msg_cnt-1].m_size = ftell(tmpf) - msg[msg_cnt-1].m_offset;
	msg[msg_cnt-1].m_lines = lines;
    }
#ifdef SYSV
    locking(fileno(mail_fp), LK_UNLCK, 0);
#endif /* SYSV */
    fclose(mail_fp);			/* implicit unlock */
    /* I've had problems with sys-v opening a file for read/write. I'd
     * try fgets after a seek to an arbitrary place and get NULL. "w+"
     * could be broken (XENIX), so play it safe anyway.
     */
    fclose(tmpf);
    if (!(tmpf = fopen(tempfile, "r")))
	error("can't open %s for reading", tempfile);
}

fs_error()
{
    error("WARNING: can't write to \"%s\"", tempfile);
    print("Read the manual on what to do on full file systems.\n");
    cleanup(0);
}

/*
 * copy temp or whatever back to mailfile
 * Return 0 if new mail came and user doesn't want to exit.
 */
copyback()
{
    register int	new = 0, i, j=0, k=0;
    register long 	flg = 0;
    register FILE 	*mbox = NULL_FILE, *mail_fp;
    char 		*mbox_file, action = 0;
    int 		hold = 0, delete_it = 0, dont_unlink = FALSE;

#ifdef SUNTOOL
    if (istool) {
	timerclear(&(mail_timer.it_interval));
	timerclear(&(mail_timer.it_value));
    }
#endif SUNTOOL
    if (ison(glob_flags, READ_ONLY)) {
	print("Can't update %s: read only\n", mailfile);
	return 1;
    }
    if (check_new_mail()) {
	new = 1;
	if (!istool) {
	    char buf[256];
	    if (iscurses)
		putchar('\n');
	    print("Really quit? "), fflush(stdout);
	    buf[0] = 0;
	    if (!Getstr(buf, 256, 0) || lower(*buf) != 'y')
		return 0;
	}
    } else if (!msg_cnt) /* prevent unnecessary overwrite */
	return 0;
    /* open mbox if: "autodelete" AND "hold" are NOT set. */
    if (is_spool(mailfile)
	    && !(delete_it = !!do_set(set_options, "autodelete"))
	    && !(hold = !!do_set(set_options, "hold"))) {
	register char *p;
	int x = 1; /* tell getpath to ignore "ENOENT" if file not found */

	if (!(p = do_set(set_options, "mbox")))
	    p = DEF_MBOX;
	mbox_file = getpath(p, &x);
	if (x) {
	    if (x > 0)
		print("%s is a directory.\n", mbox_file);
	    else
		print("can't open %s: %s\n", p, mbox_file);
	    mbox = NULL_FILE;
	} else {
	    char *mode = "a";
	    if (access(mbox_file, 0)) /* does it exist? */
		mode = "w"; /* no, create it. */
	    else if (access(mbox_file, 2))   /* if yes, can we write in it? */
		error("can't open %s", mbox_file); /* if no, print why not */
	    if (!(mbox = fopen(mbox_file, mode)))
		error("can't open %s", mbox_file);
	}
    }
    /* reopen the mailfile; make sure it's not readable */
    {
	int omask = umask(077);
	mail_fp = fopen(mailfile, "w");
	(void) umask(omask);
	if (!mail_fp) {
	    error("can't rewrite %s", mailfile);
	    return 0;
	}
    }
    turnon(glob_flags, IGN_SIGS);
    print("Updating \"%s\"", mailfile), fflush(stdout);
#ifdef SYSV
    (void) locking(fileno(mail_fp), LK_LOCK,  0);
#else
#ifdef BSD
    if (flock(fileno(mail_fp), LOCK_EX))
	error("WARNING: could not lock %s", mailfile);
#endif BSD
#endif SYSV
    turnon(flg, UPDATE_STATUS);
    turnon(flg, NO_IGNORE);

    for (i = 0; i < msg_cnt; i++)
	/* check to see if message is marked for deletion or, if read and not
	 * preserved, delete it if autodelete is set. Otherwise, save the
	 * message in the spool file if hold is set. If all fails, save in mbox.
	 */
	if (ison(msg[i].m_flags, DELETE)
	    || isoff(msg[i].m_flags, UNREAD) && isoff(msg[i].m_flags, PRESERVE) 
		&& delete_it) {
	    Debug("%s %d",
		(action!='d')? "\ndeleting message:" : "", i+1), action = 'd';
	    continue;
	} else if (ison(msg[i].m_flags, UNREAD) ||
		 ison(msg[i].m_flags, PRESERVE) || hold || !mbox) {
	    j++;
	    Debug("%s %d",
		(action!='s')? "\nsaving in spool:" : "", i+1), action = 's';
	    if (copy_msg(i, mail_fp, flg) == -1) {
		error("WARNING: could not write back to spool");
		print("ALL mail left in %s\n", tempfile);
		print("Spool mailbox may be corrupted.\n");
		if (new)
		    print("New mail may be lost. :-(\n");
		dont_unlink = TRUE;
		break;
	    }
	} else if (is_spool(mailfile)) {   /* copy back to mbox if spooldir */
	    k++;
	    if (copy_msg(i, mbox, flg) == -1) {
		error("WARNING: could not write to mbox");
		print("Unresolved mail left in %s\n", tempfile);
		dont_unlink = TRUE;
		break;
	    }
	    Debug("%s %d",
		(action!='m')? "\nsaving in mbox:" : "", i+1), action = 'm';
	}
    Debug("\n%s", mailfile);
#ifdef SYSV
    locking(fileno(mail_fp), LK_UNLCK, 0);
#endif SYSV
    fclose(mail_fp);		/* implicit unlock for BSD */
    if (mbox)
	fclose(mbox);
    if (j) {
	long times[2];
	times[1] = time(&times[0]) - (long)2;
	if (is_spool(mailfile) && utime(mailfile, times))
	    error("utime");
	print_more(": saved %d message%s\n", j, (j==1)? NO_STRING: "s");
    } else if (!is_spool(mailfile) && !dont_unlink && !new)
	if (unlink(mailfile))
	    turnon(glob_flags, CONT_PRNT), error(": cannot remove");
	else
	    print_more(": removed\n");
    else
	print_more(": empty\n");
    if (k)
	print("saved %d message%s in %s\n",k,(k==1)? NO_STRING: "s",mbox_file);
    if (new && !istool)
	print("New mail has arrived.\n");
    turnoff(glob_flags, IGN_SIGS);
#ifdef SUNTOOL
    if (istool) {
	mail_timer.it_value.tv_sec = time_out;
	setitimer(ITIMER_REAL, &mail_timer, NULL);
    }
#endif SUNTOOL
    return 1;
}

mail_size()
{
    struct stat buf;
    if (!is_spool(mailfile)) {
	char tmp[128];
	if (!stat(sprintf(tmp, "%s/%s", MAILDIR, login), &buf))
	    spool_size = buf.st_size;
    }
    if (!*mailfile)
	return 0;
    if (stat(mailfile, &buf)) {
	if (errno != ENOENT)
	    error("Can't stat %s", mailfile);
	return 0;
    }
    if (is_spool(mailfile))
	spool_size = buf.st_size;
    if (buf.st_size > last_size) {
	last_size = buf.st_size;
	return 1;
    }
    return 0;
}

void
mail_status(as_prompt)
{
    static char buf[256];
    register int cnt = 0, new = 0, unread = 0, deleted = 0;

    for ( ; cnt < msg_cnt; cnt++) {
	if (ison(msg[cnt].m_flags, UNREAD))
	    unread++;
	if (ison(msg[cnt].m_flags, DELETE))
	    deleted++;
	if (isoff(msg[cnt].m_flags, OLD))
	    new++;
    }
    if (as_prompt) {
	register char *p, *b = buf;
	for (p = prompt; *p; p++)
	    if (*p == '\\')
		switch (*++p) {
		    case 'n': case 'r': *b++ = '\n';
		    when 't': *b++ = '\t';
		    otherwise: *b++ = *p;
		}
	    else if (*p == '%')
		switch (*++p) {
		    case 'm':
			b += strlen(sprintf(b,"%d",(msg_cnt)? current_msg+1:0));
		    when 't':
			b += strlen(sprintf(b, "%d", msg_cnt));
		    when 'd':
			b += strlen(sprintf(b, "%d", deleted));
		    when 'u':
			b += strlen(sprintf(b, "%d", unread));
		    when 'n':
			b += strlen(sprintf(b, "%d", new));
		    when 'f':
			b += Strcpy(b, mailfile);
			if (ison(glob_flags, READ_ONLY))
			    b += Strcpy(b, " [read only]");
		    when 'T': case 'D': case 'Y': case 'M': case 'N':
			b += Strcpy(b, Time(p, (long)0));
		    otherwise: *b++ = *p;
		}
	    else if (*p == '!')
		b += strlen(sprintf(b, "%d", hist_no+1));
	    else
		*b++ = *p;
	*b = 0;
	print("%s", buf); /* buf MIGHT have a % in it... don't pass as fmt */
	return;
    }
    (void) sprintf(buf,"\"%s\"%s: %d message%s, %d new, %d unread",
	mailfile, ison(glob_flags, READ_ONLY)? " [read only]" : "",
	msg_cnt, (msg_cnt != 1)? "s": NO_STRING, new, unread);
    if (istool || iscurses)
	(void) sprintf(buf+strlen(buf), ", %d deleted", deleted);
#ifdef SUNTOOL
    if (istool) {
	static char ic_text[4];
	extern struct pixrect mail_icon_image1, mail_icon_image2;
	(void) sprintf(ic_text, "%3d", msg_cnt);
	tool_set_attributes(tool,
	    WIN_LABEL, buf,
	    WIN_ICON_LABEL, ic_text,
	    WIN_ICON_IMAGE, ison(glob_flags, NEW_MAIL)?
		&mail_icon_image2 : &mail_icon_image1,
	    0);
    } else
#endif SUNTOOL
#ifdef CURSES
	if (iscurses)
	    mvprintw(0, 0, "%-3d %-*s",
		((msg_cnt)? current_msg+1 : 0), COLS-5, buf), clrtoeol();
	else
#endif CURSES
	    puts(buf);
    return;
}

/* return -1 since function doesn't affect messages */
check_flags(flags)
u_long flags;
{
    print_more(" ");
    if (ison(flags, VERBOSE))
	print_more("VERBOSE ");
    if (ison(flags, INCLUDE))
	print_more("INCLUDE ");
    if (ison(flags, INCLUDE_H))
	print_more("INCLUDE_H ");
    if (ison(flags, EDIT))
	print_more("EDIT ");
    if (ison(flags, SIGN))
	print_more("SIGN ");
    if (ison(flags, DO_FORTUNE))
	print_more("DO_FORTUNE ");
    if (ison(flags, NO_HEADER))
	print_more("NO_HEADER ");
    if (ison(flags, DELETE))
	print_more("DELETE ");
    if (ison(flags, OLD))
	print_more("OLD ");
    if (ison(flags, UNREAD))
	print_more("UNREAD ");
    if (ison(flags, UPDATE_STATUS))
	print_more("UPDATE_STATUS ");
    if (ison(flags, NO_PAGE))
	print_more("NO_PAGE ");
    if (ison(flags, INDENT))
	print_more("INDENT ");
    if (ison(flags, NO_IGNORE))
	print_more("NO_IGNORE ");
    if (ison(flags, PRESERVE))
	print_more("PRESERVE ");
    print_more("\n");
    return -1;
}
