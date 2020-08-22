/* select.c	(c) copyright 1986 (Dan Heller) */

/* 
 * Routine which handle io (selection on file descriptors) between user and
 * the various windows.
 *
 * In toolmode, the user types characters and each character is interpreted
 * here and, if applicable, is sent to rite.c where it is appended to a 
 * string similar to a tty driver and fgets. When the user types a '\n' the
 * rite() routine returns the string and we call add_to_letter to append the
 * string to the letter.  Signals are caught here as well.  that is the signal
 * characters setup by the user are checked and if one matches, call the signal
 * handling routine as if there were a real signal.
 *
 * Mouse handling is done here. See code for more detail.
 */
#include "mush.h"
#define swap(a,b)         (a ^= b ^= a)   /* swap values of a pair of ints */

#define READ_MSG	(char *)'r'
#define DEL_MSG		(char *)'d'
#define UNDEL_MSG	(char *)'u'
#define REPL_MSG	(char *)'R'
#define SAVE_MSG	(char *)'s'
#define PRNT_MSG	(char *)'p'
#define PRE_MSG		(char *)'P'
#define E_EDIT     	(char *)'e'
#define E_VIEW     	(char *)'v'
#define E_INCLUDE  	(char *)'i'
#define E_SEND		(char *)'S'
#define E_ABORT   	(char *)'a'
#define MENU_HELP	(char *)'h'
#define O_SAVE		(char *)'s'
#define O_QUIT		(char *)'q'
#define O_RSTR		(char *)'r'

#define N_MENU_ITEMS	8
#define E_MENU_ITEMS	6

msg_io(gfxsw, ibits, obits, ebits, timer)
register struct gfxsubwindow *gfxsw;
register int *ibits,*obits,*ebits;
struct timeval **timer;
{
    register char	*p;
    struct inputevent 	event;
    static char 	lastchar;
    static int 		line, count;

    if (*ibits & ~(1 << gfxsw->gfx_windowfd)) {
	*ibits = *obits = *ebits = 0;
	return;
    }
    if (input_readevent(msg_sw->ts_windowfd, &event) == -1) {
	error("input event");
	return;
    }
    /*
    if (ID == LOC_WINENTER) {
	int x;
	struct inputmask im;
	win_getinputmask(gfxsw->gfx_windowfd, &im, &x);
	win_setinputmask(hdr_sw->ts_windowfd, &im, &im, x);
    }
    */
    if (ID >= KEY_LEFTFIRST)
	if (ison(glob_flags, IS_GETTING))
	    print("Finish editing letter first");
	else
	    (void) func_key(ID);
    else if (isascii(ID) && (msg_pix || ison(glob_flags, IS_GETTING) ||
	getting_opts)) {
	if (getting_opts) {
	    /*
	     * txt.x <= 5 indicates not to accept typed input for options
	     * and function key setting.
	     */
	    if (txt.x > 5) {
		/* ^C, ^\ or ^U kills line */
		pw_char(msg_win, txt.x,txt.y, PIX_OR, fonts[curfont], '_');
		if (ID == tchars.t_intrc || ID == tchars.t_quitc ||
					    ID == _tty.sg_kill) {
		    rite(_tty.sg_kill), txt.x = 5;
		    if (getting_opts == 1)
			option_line(line), display_opts(0);
		    else
			set_key(0, 0, 0);
		} else if (p = rite((char)ID)) {
		    /* if no string entered, preserve old value */
		    if (*p && getting_opts == 1)
			add_opt(p, line);
		    if (getting_opts == 2)
			set_key(p, 0,0);
		} else
		    pw_char(msg_win, txt.x, txt.y, PIX_OR, fonts[curfont], '_');
	    }
	}
	/*
	 * This section MUST BE BEFORE the following "is_getting" section.
	 * If user displays a message while editing a letter, he must hit 'q'
	 * to return to edit mode.  He may not edit a new letter while one is
	 * already being edited.
	 */
	else if (msg_pix)
	    if (isdigit(ID)) {
		if (!isdigit(lastchar))
		    count = 0;
		count = count * 10 + ID - '0';
	    } else {
		/* scroll <count> lines */
		if (!count || count > msg_pix->pr_size.y / l_height(curfont))
		    count = 1;
		if (ID == 'k' || ID == 'K' || ID == '-')
		    scroll_win(-count);
		else if (ID == '\n' || ID == '\r' || ID == 'j')
		    scroll_win(count);
		else if (ID == ' ')
		    scroll_win(crt);
		else if ((ID == 'q' || ID == 'Q') &&
			ison(glob_flags, IS_GETTING)) {
		    pr_destroy(msg_pix), msg_pix = (struct pixrect *)NULL;
		    win_setcursor(msg_sw->ts_windowfd, &write_cursor);
		    txt.x = 5, txt.y = msg_rect.r_height - l_height(curfont);
		    wprint("\n(continue editing letter)\n");
		    clr_bot_line();
		    pw_char(msg_win, txt.x,txt.y, PIX_SRC, fonts[curfont], '_');
		}
	    }
	/*
	 * If msg_pix is NULL, then we are not reading a message. If we are
	 * editing a letter, then enter the keys typed.  If we are doing
	 * nothing, ignore this input.
	 */
	else if (ison(glob_flags, IS_GETTING)) {
	    pw_char(msg_win, txt.x,txt.y, PIX_SRC^PIX_DST, fonts[curfont],'_');
	    if (lastchar != ltchars.t_lnextc &&
		(ID == tchars.t_intrc || ID == tchars.t_quitc)) {
		    (void) rite(_tty.sg_kill);
		    (void) rm_edfile(SIGINT);
	    } else {
		register int n = 1;
		if (ID == tchars.t_eofc && txt.x == 5
		    || (p = rite((char)ID)) && !(n = add_to_letter(p)))
		    finish_up_letter();
		else if (n > 0)
		    pw_char(msg_win, txt.x,txt.y, PIX_SRC, fonts[curfont], '_');
	    }
	}
	lastchar = ID;
    } else switch(ID) {
	when MS_LEFT : case MS_MIDDLE:
	    if (getting_opts == 2)
		if (ID == MS_LEFT)
		    set_key(NULL, event.ie_locx, event.ie_locy);
		else {
		    register char *p = find_key(event.ie_locx, event.ie_locy);
		    if (p)
			print("Function key %s:  %s", p, key_set_to(p));
		}
	    else if (getting_opts) {
		int y = event.ie_locy - 50;
		if (y < -24)
		    break;
		if (y < 0) {
		    register int x = event.ie_locx;
		    register int X = 60*l_width(LARGE);
		    if (x >= X && x <= X+16)
			display_opts(-1); /* scroll options back one value */
		    else if (x >= X+20 && x <= X+36)
			display_opts(1); /* scroll options forward one value */
		    break;
		}
		/* the user was typing something -- stopped by using mouse */
		if (txt.x > 5) {
		    pw_char(msg_win, txt.x,txt.y, PIX_CLR, fonts[curfont], '_');
		    (void) rite(_tty.sg_kill), txt.x = 5;
		    option_line(line), display_opts(0);
		}
	        line = y/20;
		if (ID == MS_LEFT)
		    toggle_opt(line);
		help_opt(line);   /* display help (short info) in both cases */
	    } else if (msg_pix)
		if (ID == MS_LEFT)
		    scroll_win(crt-3);
		else
		    scroll_win(-(crt-3));
	when MS_RIGHT:
	    if (getting_opts)
		(void) view_opts_menu(&event, gfxsw->gfx_windowfd);
	    else if (isoff(glob_flags, IS_GETTING))
		(void) do_menu(&event, gfxsw->gfx_windowfd, current_msg);
	    else
		(void) edit_menu(&event, gfxsw->gfx_windowfd);
	otherwise: ;
    }
    *ibits = *obits = *ebits = 0;
}

struct cursor *mice[3] = { &l_cursor, &m_cursor, &r_cursor };

hdr_io(gfxsw, ibits, obits, ebits, sw_timer)
register struct gfxsubwindow *gfxsw;
int *ibits,*obits,*ebits;
struct timeval **sw_timer;
{
    static int 		which_cursor;
    struct inputmask 	im;
    struct inputevent 	event;
    int 		line;

    if (*ibits & ~(1 << gfxsw->gfx_windowfd)) {
	*ibits = *obits = *ebits = 0;
	return;
    }
    /* make curosr change which button is lit */
    win_setcursor(gfxsw->gfx_windowfd, mice[which_cursor]);

    which_cursor = (which_cursor+1) % 3;
    if (input_readevent(hdr_sw->ts_windowfd, &event) == -1) {
	error("input event");
	return;
    }
    /* I'm not sure why I have to do this.
     * I'm doing it because sometimes the IO hangs completely and no input
     * is accepted. What I do here is get the current mask, save it, then
     * reset it. This action seems to flush the IO queue, and I don't have hung
     * IO anymore.  This shouldn't be necessary, but it appears to work.
     * (occurances have droped about 90%)
     */
    if (ID == LOC_WINENTER) {
	int x;
	win_getinputmask(gfxsw->gfx_windowfd, &im, &x);
	win_setinputmask(hdr_sw->ts_windowfd, &im, &im, x);
    }
    /* just return -- we just wanted to make the cursor flicker */
    if (ID == LOC_STILL || ID == LOC_MOVE || ID == LOC_WINENTER) {
	*ibits = *obits = *ebits = 0;
	return;
    }
    line = event.ie_locy / l_height(DEFAULT);
    if (ID >= KEY_LEFTFIRST)
	(void) func_key(ID);
    else if (n_array[line] > msg_cnt)
	if (!msg_cnt)
	    print("You have no messages -- this function is useless now.");
	else
	    print("Message out of range.  Place mouse over a legal message.");
    else switch(ID) {
	when MS_LEFT: case MS_MIDDLE:
	    (void) do_menu((ID == MS_LEFT)? READ_MSG: DEL_MSG, 0,n_array[line]);
	when MS_RIGHT:
	    (void) do_menu(&event, gfxsw->gfx_windowfd, n_array[line]);
	otherwise : print("Unkown ID = %d", ID);
    }
    *ibits = *obits = *ebits = 0;
}

/* if "fd" is 0, then event points to the action to be taken.
 * otherwise, determine action to be taken by displaying a menu.
 * message is the number current_msg should be changed to (may be the same).
 */
do_menu(event, fd, message)
caddr_t event;
{
    static char buf[20];
    struct menuitem *m_item;
    char *action;
    static struct menuitem msg_menu_items[] = {
	{ MENU_IMAGESTRING,  "Read",     READ_MSG   },
	{ MENU_IMAGESTRING,  "Delete",   DEL_MSG    },
	{ MENU_IMAGESTRING,  "Undelete", UNDEL_MSG  },
	{ MENU_IMAGESTRING,  "Reply",    REPL_MSG   },
	{ MENU_IMAGESTRING,  "Save",     SAVE_MSG   },
	{ MENU_IMAGESTRING,  "Preserve", PRE_MSG    },
	{ MENU_IMAGESTRING,  "Print",    PRNT_MSG   },
	{ MENU_IMAGESTRING,  "Help",     MENU_HELP  }
    };
    static struct menu help_menu = {
        MENU_IMAGESTRING, "Item Help",
	N_MENU_ITEMS, msg_menu_items,
	(struct menu *)NULL, NULL
    };
    static struct menu msgs_menu = {
        MENU_IMAGESTRING, buf, N_MENU_ITEMS,
	msg_menu_items, &help_menu, NULL
    };
    /* to have the menu stack maintain order of menus upon each invokation,
     * declare menu_ptr to be static and remove the following two lines
     * after the declaration.
     */
    struct menu *menu_ptr = &msgs_menu;
    msgs_menu.m_next = &help_menu;
    help_menu.m_next = (struct menu *)NULL;

    if (!msg_cnt) {
	print("No Messages.");
	return;
    }
    if (fd) {
	(void) sprintf(buf, "Message #%d", message+1);
	if (m_item = menu_display(&menu_ptr, (struct inputevent *)event, fd))
	    action = m_item->mi_data;
	else
	    return;
    } else
	action = event;

    if (menu_ptr == &help_menu || action == MENU_HELP) {
	switch(action) {
	    when DEL_MSG: case UNDEL_MSG:
		(void) help(fd, "menu_delete", tool_help);
	    when READ_MSG: (void) help(fd, "next", tool_help);
	    when REPL_MSG: (void) help(fd, "menu_respond", tool_help);
	    when SAVE_MSG: (void) help(fd, "save", tool_help);
	    when PRE_MSG: (void)  help(fd, "preserve", tool_help);
	    when PRNT_MSG: (void) help(fd, "printer", tool_help);
	    when MENU_HELP:
		if (menu_ptr == &help_menu)
		    (void) help(fd, "help_menu_help_msg", tool_help);
		else
		    (void) help(fd, "msg_menu", tool_help);
	}
	return;
    }
    set_isread(message);
    if (action == SAVE_MSG) {
	swap(current_msg, message);
	((struct inputevent *)event)->ie_code = MS_LEFT;
	do_file_dir(save_item, 0, event);
	swap(current_msg, message);
	return;
    } else if (action == PRNT_MSG  || action == PRE_MSG ||
	       action == UNDEL_MSG || action == DEL_MSG) {
	fkey_misc(action, message);
	return;
    }
    if (isoff(glob_flags, IS_GETTING)) {
	current_msg = message;
	(void) do_hdrs(0, DUBL_NULL, NULL);
    }
    if (action == REPL_MSG) {
	respond_mail(respond_item, 0, event);
	return;
    } else if (ison(glob_flags, IS_GETTING)) {
	print("Finish editing message first");
	return;
    }
    display_msg(current_msg, (long)0);
}

/* miscellaneous function key actions there are here because the defines
 * for DEL_MSG, etc are here in this file and the function is called from
 * here more often.
 */
fkey_misc(action, message)
char *action;
{
    int argc;
    register char **argv;
    char buf[30];

    print("Message #%d ", message+1);
    if (action == UNDEL_MSG || action == DEL_MSG)
	print_more("%sd. ", sprintf(buf, "%selete",
			    (action == DEL_MSG)? "d": "und"));
    else if (action == PRNT_MSG) {
	print_more("sent to printer");
	(void) strcpy(buf, "lpr");
    } else if (action == PRE_MSG)
	print_more("%sd", strcpy(buf, "preseve"));
    (void) sprintf(&buf[strlen(buf)], " %d", message+1);

    if (argv = make_command(buf, DUBL_NULL, &argc))
	(void) do_command(argc, argv, msg_list);
    return;
}

view_opts_menu(event, fd)
struct inputevent *event;
{
    static char buf[5];
    struct menuitem *m_item;
    char *action;
    static struct menuitem opts_items[] = {
	{ MENU_IMAGESTRING,  "Save Options",	O_SAVE  },
	{ MENU_IMAGESTRING,  "Restore Options",	O_RSTR  },
	{ MENU_IMAGESTRING,  "Quit Options",	O_QUIT  },
	{ MENU_IMAGESTRING,  "Help",		MENU_HELP  }
    };
    static struct menu msgs_menu = {
        MENU_IMAGESTRING, "Options", 4, opts_items, (struct menu *)NULL, NULL
    };
    struct menu *menu_ptr = &msgs_menu;

    if (m_item = menu_display(&menu_ptr, event, fd))
	action = m_item->mi_data;
    else
	return;
    switch(action) {
	case O_SAVE:
	    save_opts(0, DUBL_NULL);
	when O_RSTR:
	    init();
	    if (getting_opts == 1)
		view_options();
	    else
		set_fkeys();
	when O_QUIT:
	    do_clear();
	    unlock_cursors(); /* actually resets msg_win's cursor */
	    if (isoff(glob_flags, IS_GETTING) && msg_cnt)
		if (isoff(msg[current_msg].m_flags, DELETE))
		    display_msg(current_msg, (long)0);
		else
		    (void) read_mail(NO_ITEM, 0, NO_EVENT);
	when MENU_HELP:
	    (void) help(fd, (getting_opts == 1)? "options": "fkeys", tool_help);
    }
}
