/* execute.c 	(c) copyright	10/28/86 (Dan Heller) */

#include "mush.h"
#ifdef BSD
#include <sys/wait.h>
#else
#ifndef SYSV
#include <wait.h>
#endif SYSV
#endif BSD

#ifdef lint
#include <sys/resource.h>
#endif lint

execute(argv)
char **argv;
{
#ifdef SYSV
    int status;
#else
    union wait status;
#endif SYSV
#ifdef SIGCONT
    int	(*oldstop)(), (*oldcont)();
#endif SIGCONT
    int pid, (*oldint)(), (*oldquit)();

#ifdef SUNTOOL
    if (istool) {
	print("Editing letter...");

	panel_set(abort_item, PANEL_SHOW_ITEM, FALSE, 0);
	panel_set(send_item,  PANEL_SHOW_ITEM, FALSE, 0);
	panel_set(edit_item,  PANEL_SHOW_ITEM, FALSE, 0);
	win_setrect(tty_sw->ts_windowfd, &msg_rect);
	msg_rect.r_height = 0;
	win_setrect(msg_sw->ts_windowfd, &msg_rect);
	if ((exec_pid = ttysw_fork(tty_sw->ts_data, argv,
	    &tty_sw->ts_io.tio_inputmask,
	    &tty_sw->ts_io.tio_outputmask,
	    &tty_sw->ts_io.tio_exceptmask)) == -1)
		error("%s failed", *argv), sigchldcatcher();
	Debug("tty pid = %d\n", exec_pid);
	return;
    }
#endif SUNTOOL
    oldint = signal(SIGINT, SIG_IGN);
    oldquit = signal(SIGQUIT, SIG_IGN);
#ifdef SIGCONT
    oldstop = signal(SIGTSTP, SIG_DFL);
    oldcont = signal(SIGCONT, SIG_DFL);
#endif SIGCONT
    turnon(glob_flags, IGN_SIGS);

    echo_on();
    if ((exec_pid = vfork()) == 0) {
	(void) signal(SIGINT, SIG_DFL);
	(void) signal(SIGQUIT, SIG_DFL);
	execvp(*argv, argv);
	if (errno == ENOENT)
	    print("%s: command not found.\n", *argv);
	else
	    error(*argv);
	_exit(-1);
    }
    /* parent's got to do something; sigchldcatcher will do waiting
     * if other forks die (sendmail), then this wait will catch them,
     * This loop will really get -1, cuz sigchldcatcher will catch all else.
     */
    while ((pid = wait(&status) != -1) && pid != exec_pid)
	Debug("The exec loop caught a signal? (pid = %d)\n", pid);
    /* reset our ttymodes */
    echo_off();
    (void) signal(SIGINT, oldint);
    (void) signal(SIGQUIT, oldquit);
#ifdef SIGCONT
    (void) signal(SIGTSTP, oldstop);
    (void) signal(SIGCONT, oldcont);
#endif SIGCONT
    turnoff(glob_flags, IGN_SIGS);
}

sigchldcatcher()
{
#ifdef SUNTOOL
    struct rect rect;
#endif SUNTOOL
#ifdef SYSV
    int status;
#else
    union wait status;
#endif SYSV
    int	   pid;

#ifdef BSD
    /* The follwoing SHOULDN'T be necessary, but it is!!! ttysw_fork()
     * returns the pid of the thing that it executes, but that's not the
     * pid that dies!  There are many procs that might die from ttysw_fork
     * one of them is the process, another is the tty, etc... other
     * procs that might die are sendmail, fortune, etc... tool_sigchld()
     * handles these, but we can't let it have control unless we KNOW the
     * editor is done.
     * so if what we catch is not the exec_pid from ttysw_fork(), then
     * send ourselves a sigchld to go thru this routine again.  mush -d
     */
    while ((pid = wait3(&status, WNOHANG, (struct rusage *)0)) > 0) {
	Debug("%d died...\n", pid);
	if (pid == exec_pid)
	    break;
    }
#else
#ifndef SYSV
    while ((pid = wait2(&status, WNOHANG)) > 0 && pid != exec_pid)
	Debug("%d died...\n", pid);
#else SYSV
    while ((pid = wait((int *)0)) > 0 && pid != exec_pid)
	Debug("%d died...\n", pid);
#endif SYSV
#endif BSD
#ifndef SUNTOOL
    }
#else SUNTOOL
    if (pid != exec_pid || exec_pid <= 0) /* if the editor didn't die, return */
	return;
    /* editor died -- reset exec_pid so no one thinks we're running */
    exec_pid = 0;
    /* only the tool needs to continue from here.  Reset the win */
    if (istool < 1)
	return;
    tool_sigchld(tool);
    print("Editor done");
    win_getrect(tty_sw->ts_windowfd, &msg_rect);
    if (!msg_rect.r_height) {
	print_more(" (well, something just happened)");
	return;
    }
    rect.r_top = rect.r_left = rect.r_height = 0;
    rect.r_width = msg_rect.r_width;
    win_setrect(tty_sw->ts_windowfd, &rect);
    win_setrect(msg_sw->ts_windowfd, &msg_rect);
    panel_set(comp_item, PANEL_SHOW_ITEM, FALSE, 0);
    panel_set(send_item, PANEL_SHOW_ITEM, TRUE, 0);
    panel_set(edit_item, PANEL_SHOW_ITEM, TRUE, 0);
    panel_set(abort_item, PANEL_SHOW_ITEM, TRUE, 0);
    wprint("(continue editing letter.)\n");
    pw_char(msg_win, txt.x,txt.y, PIX_SRC, fonts[curfont], '_');
}

sigtermcatcher()
{
    ttysw_done(tty_sw->ts_data);
    if (ison(glob_flags, IS_GETTING))
	rm_edfile(-1);
    cleanup(SIGTERM);
}
#endif SUNTOOL
