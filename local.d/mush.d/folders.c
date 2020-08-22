/* @(#)folders.c	(c) copyright 10/18/86 (Dan Heller) */

#include "mush.h"

/* folder %[user]  --new mailfile is the spool/mail/login file [user].
 * folder #  --new mailfile is the folder previous to the current folder
 * folder &  --new mailfile is ~/mbox (or whatever "mbox" is set to)
 * folder +file --new mailfile is in the directory "folder"; name is 'file'
 * folder "path" --full path name or the one in current working directory.
 *
 * in all cases, changes are updated unless a '!' is specified after the
 * folder command (e.g. "f!", "folder !" "fo!" .. all permutations)
 * as usual, if new mail has arrived before the file is copied back, then
 * user will be notified beforehand.
 *
 * RETURN -1 on error -- else return 0. All bits in msg_list are set to true.
 */
folder(argc, argv, list)
register char **argv, list[];
{
    int n, updating = !strcmp(*argv, "update"), do_read_only = 0, no_hdrs = 0;
    static char oldfolder[256];
    char *tmp, *newfolder = NULL, buf[256];

    if (ison(glob_flags, DO_PIPE)) {
	print("You can't pipe to the folder command");
	return -1;
    }
    while (*++argv && (**argv == '-' || **argv == '!'))
	if (!strcmp(*argv, "-?"))
	    return help(0, "folder_help", cmd_help);
	else if (!strcmp(*argv, "-N"))
	    no_hdrs = 1;
	else if (!strcmp(*argv, "-r"))
	    do_read_only = 1;
	else if (!strcmp(*argv, "!"))
	    turnoff(glob_flags, DO_UPDATE);

    if (updating)
	(void) strcpy(buf, mailfile);
    else {
	if (!*argv) {
	    print("%s\n", mailfile);
	    return 0;
	}
	if (!strcmp(*argv, "#"))
	    if (!*oldfolder) {
		print("No previous folder\n");
		return -1;
	    } else
		newfolder = oldfolder;
	else if (!strcmp(*argv, "&")) {
	    if (!(newfolder = do_set(set_options, "mbox")) || !*newfolder)
		newfolder = DEF_MBOX;
	} else
	    newfolder = *argv;
	n = 0;
	tmp = getpath(newfolder, &n);
	if (n == -1) {
	    print("%s: %s\n", newfolder, tmp);
	    return -1;
	} else if (n == 1) {
	    print("%s: is a directory\n", tmp);
	    return -1;
	}
	/* strcpy so copyback() below (which calls getpath) doesn't change
	 * the data that tmp intended to point to.
	 */
	(void) strcpy(buf, tmp);
    }
    if (access(buf, R_OK)) {
	error("Can't open %s", buf);
	return -1;
    }
    if (access(buf, W_OK))
	do_read_only = 1;
#ifdef SUNTOOL
    if (istool) lock_cursors();
#endif SUNTOOL
    if (ison(glob_flags, DO_UPDATE) && !copyback()) {
#ifdef SUNTOOL
	if (istool) unlock_cursors();
#endif SUNTOOL
	/* could be an error, but new mail probably came in */
	return -1;
    }
    (void) strcpy(oldfolder, mailfile);
    strdup(mailfile, buf);
    do_read_only? turnon(glob_flags,READ_ONLY) : turnoff(glob_flags,READ_ONLY);
    last_size = spool_size = 0L;
    msg_cnt = 0;
    turnoff(glob_flags, CONT_PRNT);

    turnon(glob_flags, IGN_SIGS);
    /* clear the tempfile */
    fclose(tmpf);
    if (!(tmpf = fopen(tempfile, "w"))) {
	error("error truncating %s", tempfile);
	turnoff(glob_flags, IGN_SIGS);
	return -1;
    }
    getmail();
    last_msg_cnt = msg_cnt;  /* for check_new_mail */
    (void) mail_size();
#ifdef SUNTOOL
    if (istool) {
	panel_set(next_scr, PANEL_SHOW_ITEM, FALSE, 0);
	panel_set(prev_scr, PANEL_SHOW_ITEM, FALSE, 0);
	pw_rop(hdr_win, 0,0, hdr_rect.r_width, hdr_rect.r_height,PIX_CLR,0,0,0);
    }
#endif SUNTOOL
    current_msg = 0;
    turnoff(glob_flags, IGN_SIGS);

    /* now sort messages according a user_defined default */
    if (!updating && msg_cnt > 1 && is_spool(mailfile) &&
		(tmp = do_set(set_options, "sort"))) {
	(void) sprintf(buf, "sort %s", tmp);
	if (argv = make_command(buf, TRPL_NULL, &argc)) {
	    /* msg_list can't be null for do_command and since we're not
	     * interested in the result, call sort directly
	     */
	    (void) sort(argc, argv, NULL);
	    free_vec(argv);
	}
    }
    turnoff(glob_flags, DO_UPDATE);

    while (current_msg < msg_cnt && isoff(msg[current_msg].m_flags, UNREAD))
	current_msg++;
    if (current_msg == msg_cnt)
	current_msg = 0;

    if (!istool && !iscurses)
	mail_status(0);
    /* be quite if we're piping */
    if ((istool || !updating) && isoff(glob_flags, IS_PIPE) &&
	(istool || !no_hdrs) && msg_cnt)
	(void) cmd_line(sprintf(buf, "headers %d", current_msg+1), msg_list);
#ifdef SUNTOOL
    if (istool) {
	if (!msg_cnt)
	    print("No Mail in %s\n", mailfile);
	else if (isoff(glob_flags, IS_GETTING) && !getting_opts)
	    display_msg(current_msg, (long)0);
	unlock_cursors();
    }
#endif SUNTOOL
    if (list) {
	clear_msg_list(list);
	bitput(list, list, msg_cnt, =~) /* macro */
    }
    return 0;
}

folders(argc, argv)
register char **argv;
{
    register char *p;
    char buf[128], unused[MAXMSGS_BITS];

    if (!(p = do_set(set_options, "folder")) || !*p) {
	print("You have no folder variable set.\n");
	return -1;
    }
    (void) sprintf(buf, "ls %s", p);
    if (argv = make_command(buf, TRPL_NULL, &argc))
	(void) do_command(argc, argv, unused);
    return -1;
}
