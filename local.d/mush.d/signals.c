/* @(#)signals.c	(c) copyright 10/18/86 (Dan Heller) */

#include "mush.h"

#ifndef SYSV
extern char *sys_siglist[];
#else
/* sys-v doesn't have normal sys_siglist */
static char	*sys_siglist[] = {
/* no error */  "no error",
/* SIGHUP */	"hangup",
/* SIGINT */	"interrupt (rubout)",
/* SIGQUIT */	"quit (ASCII FS)",
/* SIGILL */	"illegal instruction (not reset when caught)",
/* SIGTRAP */	"trace trap (not reset when caught)",
/* SIGIOT */	"IOT instruction",
/* SIGEMT */	"EMT instruction",
/* SIGFPE */	"floating point exception",
/* SIGKILL */	"kill (cannot be caught or ignored)",
/* SIGBUS */	"bus error",
/* SIGSEGV */	"segmentation violation",
/* SIGSYS */	"bad argument to system call",
/* SIGPIPE */	"write on a pipe with no one to read it",
/* SIGALRM */	"alarm clock",
/* SIGTERM */	"software termination signal from kill",
/* SIGUSR1 */	"user defined signal 1",
/* SIGUSR2 */	"user defined signal 2",
/* SIGCLD */	"death of a child",
/* SIGPWR */	"power-fail restart"
};
#endif SYSV

#ifdef SUNTOOL
msgwin_handlesigwinch()
{
    register struct rect rect;
    if (exec_pid)
	return;
    rect = msg_rect;
    pw_damaged(msg_win);
    /* this prevents old screen from being lost when editor finishes */
    if (isoff(glob_flags, IS_GETTING))
	gfxsw_interpretesigwinch(msg_sw->ts_data);
    gfxsw_handlesigwinch(msg_sw->ts_data);
    pw_repairretained(msg_win);
    pw_donedamaged(msg_win);
    win_getrect(msg_sw->ts_windowfd, &msg_rect);
    crt = msg_rect.r_height / l_height(curfont);
    if (rect.r_height != msg_rect.r_height || rect.r_width != rect.r_width)
	if (getting_opts == 1)
	    display_opts(0);
	else if (getting_opts == 2)
	    set_fkeys();
	else if (msg_pix)
	    scroll_win(0);
}

hdrwin_handlesigwinch()
{
    register struct rect rect;
    rect = hdr_rect;
    pw_damaged(hdr_win);
    gfxsw_interpretesigwinch(hdr_sw->ts_data);
    gfxsw_handlesigwinch(hdr_sw->ts_data);
    pw_repairretained(hdr_win);
    pw_donedamaged(hdr_win);
    win_getrect(hdr_sw->ts_windowfd, &hdr_rect);
    if (rect.r_width != hdr_rect.r_width || rect.r_height != hdr_rect.r_height){
	pw_writebackground(hdr_win, 0,0,
			   hdr_rect.r_width, hdr_rect.r_height, PIX_CLR);
	screen = hdr_rect.r_height/l_height(DEFAULT);
	(void) do_hdrs(0, DUBL_NULL, NULL);
    }
}

print_sigwinch()
{
    pw_damaged(print_win);
    gfxsw_handlesigwinch(print_sw->ts_data);
    pw_writebackground(print_win, 0,0,
        win_getwidth(print_sw->ts_windowfd),
	win_getheight(print_sw->ts_windowfd), PIX_CLR);
    pw_donedamaged(print_win);
    print(NULL);  /* reprint whatever was there before damage */
}

sigwinchcatcher()
{
    tool_sigwinch(tool);
}
#endif SUNTOOL

interrupt(sig)
{
    Debug("interrupt() caught: %d\n", sig);
    turnon(glob_flags, WAS_INTR);
}

/*
 * catch signals to reset state of the machine.  Always print signal caught.
 * If signals are ignored, return.  If we're running the shell, longjmp back.
 */
/*ARGSUSED*/
catch(sig)
{
    Debug("Caught signal: %d\n", sig);
    (void) signal(sig, catch);
    if (ison(glob_flags, IGN_SIGS) && sig != SIGTERM && sig != SIGHUP)
	return;
    print("%s: %s\n", prog_name, sys_siglist[sig]);
    turnoff(glob_flags, IS_PIPE);
    if (istool || sig == SIGTERM || sig == SIGHUP) {
	istool = 1;
	(void) setjmp(jmpbuf);
	if (ison(glob_flags, IS_GETTING))
	    rm_edfile(-1);
	cleanup(sig);
    }
    if (ison(glob_flags, DO_SHELL))
	longjmp(jmpbuf, 1);
}

#ifdef SIGCONT
stop_start(sig)
{
    Debug("Caught signal: %d", sig);
    if (sig == SIGCONT) {
	(void) signal(SIGTSTP, stop_start);
	(void) signal(SIGCONT, stop_start);
	if (istool || ison(glob_flags, IGN_SIGS) && !iscurses)
	    return;
	echo_off();
	/* we're not in an editor but we're editing a letter */
	if (ison(glob_flags, IS_GETTING)) {
	    if (!exec_pid)
		print("(Continue editing letter)\n");
	}
#ifdef CURSES
	else if (iscurses)
	    if (ison(glob_flags, IGN_SIGS)) {
		clr_bot_line();
		if (msg_list)
		    puts(compose_hdr(current_msg));
		mail_status(1), addstr("...continue... ");
		refresh();
	    } else {
		int curlin = max(1, current_msg - n_array[0] + 1);
		redraw();
		print("Continue");
		move(curlin, 0);
		refresh();
		/* make sure we lose reverse video on continuation */
		if (ison(glob_flags, REV_VIDEO) && msg_cnt) {
		    char buf[256];
		    (void) strncpy(buf, compose_hdr(current_msg), COLS-1);
		    buf[COLS-1] = 0; /* strncpy does not null terminate */
		    mvaddstr(curlin, 0, buf);
		}
	    }
#endif CURSES
  	else
	    mail_status(1), fflush(stdout);
    } else {
#ifdef CURSES
	if (iscurses) {
	    /* when user stops mush, the current header is not in reverse
	     * video -- note that a refresh() has not been called in curses.c!
	     * so, make sure that when a continue is called, the reverse video
	     * for the current message returns.
	     */
	    if (isoff(glob_flags, IGN_SIGS) && ison(glob_flags, REV_VIDEO) &&
		    msg_cnt) {
		int curlin = max(1, current_msg - n_array[0] + 1);
		char buf[256];
		(void) strncpy(buf, stdscr->_y[curlin], COLS-1);
		buf[COLS-1] = 0; /* strncpy does not null terminate */
		STANDOUT(curlin, 0, buf);
	    }
	    print("Stopping...");
	}
#endif CURSES
	echo_on();
	(void) signal(SIGTSTP, SIG_DFL);
	(void) signal(SIGCONT, stop_start);
	(void) kill(getpid(), sig);
    }
}
#endif SIGCONT

/*ARGSUSED*/
cleanup(sig)
{
    char buf[128], c = ison(glob_flags, IGN_SIGS)? 'n' : 'y';

#ifdef CURSES
    if (iscurses)
	iscurses = FALSE, endwin();
#endif CURSES

    echo_on();

    if (ison(glob_flags, IS_GETTING))
	turnoff(glob_flags, IS_GETTING), dead_letter();
    if ((sig == SIGSEGV || sig == SIGBUS) && isoff(glob_flags, IGN_SIGS)
	&& *tempfile) {
	fprintf(stderr, "remove %s [y]? ", tempfile), fflush(stderr);
	if (fgets(buf, 128, stdin))
	    c = lower(*buf);
    }
    if (c != 'n' && *tempfile && unlink(tempfile) && !sig && errno != ENOENT)
	error(tempfile);
#ifdef SUNTOOL
    if (istool && tool)
	tool_destroy(tool);
#endif SUNTOOL
    if (sig == SIGSEGV || sig == SIGBUS) {
	if (isoff(glob_flags, IGN_SIGS)) {
	    fprintf(stderr, "coredump [n]? "), fflush(stderr);
	    if (fgets(buf, 128, stdin))
		c = lower(*buf);
	}
	if (c == 'y')
	    puts("dumping core for debugging"), abort();
    }
    exit(sig);
}

/*
 * if new mail comes in, print who it's from.  sprintf it all into one
 * buffer and print that instead of separate print statements to allow
 * the tool mode to make one print statment. The reason for this is that
 * when the tool is refreshed (caused by a resize, reopen, move, top, etc)
 * the last thing printed is displayed -- display the entire line.
 */
check_new_mail()
{
    int 	   ret_value;
    char 	   buf[BUFSIZ], buf2[256];
    register char  *p = buf;
    static long    last_spool_size = -1;

#ifdef SUNTOOL
    static int is_iconic, was_iconic;

    if (istool) {
	timerclear(&(mail_timer.it_interval));
	timerclear(&(mail_timer.it_value));
	mail_timer.it_value.tv_sec = time_out;
	setitimer(ITIMER_REAL, &mail_timer, NULL);
    }
#endif SUNTOOL
    /* if fullscreen access in progress (help), don't do anything */
    if (ret_value = mail_size()) {
#ifdef CURSES
	int new_hdrs = last_msg_cnt;
#endif CURSES
#ifdef SUNTOOL
	/* if our status has changed from icon to toolform, then
	 * there will already be a message stating number of new
	 * messages.  reset `n' to msg_cnt so we don't restate
	 * the same # of new messages upon receipt of yet another new message.
	 */
	if (istool && !(is_iconic = (tool->tl_flags&TOOL_ICONIC)) && was_iconic)
	    last_msg_cnt = msg_cnt;
#endif SUNTOOL
	turnon(glob_flags, NEW_MAIL);
	getmail(); /* msg_cnt gets incremented here */
	if (istool) {
	    mail_status(0);
	    (void) do_hdrs(0, DUBL_NULL, NULL);
	}
	p += Strcpy(p, "New mail ");
	if (msg_cnt - last_msg_cnt <= 1)
	    p += strlen(sprintf(p, "(#%d) ", msg_cnt));
	else
	    p += strlen(sprintf(p, "(#%d thru #%d)\n", last_msg_cnt+1,msg_cnt));
#ifdef SUNTOOL
	/*
	 * If mush is in tool mode and in icon form, don't update last_msg_cnt
	 * so that when the tool is opened, print() will print the correct
	 * number of "new" messages.
	 */
	if (!istool || !(was_iconic = tool->tl_flags & TOOL_ICONIC))
#endif SUNTOOL
	    while (last_msg_cnt < msg_cnt) {
		char *p2 = reply_to(last_msg_cnt++, FALSE, buf2);
		if (strlen(p2) + (p - buf) >= BUFSIZ-5) {
		    (void) strcat(p, "...\n");
		    /* force a loop break by setting last_msg_cnt correctly */
		    last_msg_cnt = msg_cnt;
		} else
		    p += strlen(sprintf(p, " %s\n", p2));
	    }
#ifdef CURSES
	if (iscurses) {
	    if (strlen(buf) > COLS-1) {
		printf("%s --more--", buf), fflush(stdout);
		(void) getchar();
		(void) clear();
		(void) strcpy(buf+COLS-5, " ...");
		new_hdrs = n_array[screen-1]; /* force new headers */
	    }
	    if (new_hdrs - n_array[screen-1] < screen)
		(void) do_hdrs(0, DUBL_NULL, NULL);
	}
#endif CURSES
	print("%s", buf); /* buf might have %'s in them!!! */
    } else
#ifdef SUNTOOL
	if (!istool || !is_iconic)
#endif SUNTOOL
	    turnoff(glob_flags, NEW_MAIL);
    if (last_spool_size > -1 && /* handle first case */
	    !is_spool(mailfile) && last_spool_size < spool_size)
	print("You have new mail in your system mailbox.\n"), ret_value = 1;
    last_spool_size = spool_size;
    return ret_value;
}

/*ARGSUSED*/   /* we ignore the sigstack, cpu-usage, etc... */
bus_n_seg(sig)
{
    fprintf(stderr, "%s: %s\n", prog_name,
	(sig == SIGSEGV)? "Segmentation violation": "Bus error");
    cleanup(sig);
}
