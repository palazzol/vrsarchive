/* @(#)doproc.c		(c) copyright	10/18/86 (Dan Heller) */

/* do main panel item procedures */
#include "mush.h"

respond_mail(item, value, event)
Panel_item item;
int value;
struct inputevent *event;
{
    char buf[80];

    if (value == 4)
	return help(panel_sw->ts_windowfd, "respond", tool_help);
    if (ison(glob_flags, IS_GETTING)) {
	print("Finish editing current message first");
	return;
    }
    if (!msg_cnt) {
	print("No messages to respond to.\n");
	return;
    }
    print("Responding to message %d", current_msg+1);
    if (event && event->ie_code == MS_LEFT)
	value = 0;
    (void) sprintf(buf, "%s %s %d",
	(value == 2 || value == 3)? "replyall" : "replysender",
	(value == 1 || value == 3)? "-i": NO_STRING, current_msg+1);
    (void) cmd_line(buf, msg_list);
}

/* following macro is for the next two procedures */
#define hdr_item (item == sub_hdr_item[0] || item == sub_hdr_item[1] || \
                  item == sub_hdr_item[2] || item == sub_hdr_item[3] || \
                  item == sub_hdr_item[4] || item == sub_hdr_item[5])

delete_mail(item, val, event)
register Panel_item item;
int val;
register struct inputevent *event;
{
    register int value = val, c;
    char buf[92];

    panel_set(item, PANEL_VALUE, 0, 0);
    if (hdr_item && event->ie_code != MS_LEFT || value == 6)
	return help(panel_sw->ts_windowfd, "delete", tool_help);
    /* if selected "delete" in header panel, set value = 4 (delete "range") */
    if (hdr_item)
	value = 4;
    /* delete current message */
    if (!value || value == 1 && event && event->ie_code == MS_LEFT)
	if (ison(msg[current_msg].m_flags, DELETE)) {
	    print("%d Already deleted", current_msg+1);
	    return;
	} else {
	    print("Deleted Message %d. ", current_msg+1);
	    (void) strcpy(buf, "delete");
	}
    else switch(value) {
	case 1:
	    print("Really delete everything?");
	    if ((c = confirm(panel_sw->ts_windowfd)) == 'y' ||
			 c == MS_LEFT) {
		print("Deleted All Messages. ");
		(void) strcpy(buf, "delete *");
	    } else { print("Whew!"); return; }
	when 2:
	    /* undelete current message */
	    if (isoff(msg[current_msg].m_flags, DELETE)) {
		print("%d isn't deleted", current_msg+1);
		return;
	    }
	    print("Undeleted Message #%d. ", current_msg+1);
	    (void) strcpy(buf, "undelete");
	when 3:
	    /* undelete all messages */
	    print("Uneleted All Messages. ");
	    (void) strcpy (buf, "undelete *");
	when 4: case 5:
	    /* delete a range of messages */
	    (void) sprintf(buf, "%selete \"%s\"", (value == 4)? "d": "und",
		    panel_get_value(msg_num_item));
	    panel_set(msg_num_item, PANEL_VALUE, NO_STRING, 0);
	}
    (void) cmd_line(buf, msg_list);
}

read_mail(item, value, event)
register Panel_item item;
register int value;
register struct inputevent *event;
{
    register int this_msg = current_msg;

    /* check "event" in case we were called from select.c
     * in which case event would be NULL
     */
    if (event && event->ie_code == MS_RIGHT &&
        item && (item == read_item && value ||
	(item == sub_hdr_item[0] || item == sub_hdr_item[1])))
	return help(panel_sw->ts_windowfd, "next", tool_help);
    if (item && (item == sub_hdr_item[4] || item == sub_hdr_item[5]))
	return help(panel_sw->ts_windowfd, "msg_menu", tool_help);
    if (!msg_cnt) {
	print ("No Mail.");
	return -1;
    }
    if (item && item == read_item || ison(msg[current_msg].m_flags, DELETE))
	(void) next_msg(FALSE, DELETE);
    if (this_msg != current_msg || ison(msg[current_msg].m_flags, UNREAD) ||
	    (current_msg < n_array[0] || current_msg > n_array[screen])) {
	set_isread(current_msg);
	(void) do_hdrs(0, DUBL_NULL, NULL);
    }
    display_msg(current_msg, (long)0);
    return -1;
}

/* the panel button that says "filename" and "directory", etc... text item */
file_dir(item, event)
Panel_item item;
struct inputevent *event;
{
    register char *p;
    char buf[128], *which = panel_get(item, PANEL_LABEL_STRING);

    if (!strcmp(which, "folder:"))
	if (event->ie_code == '\n' || event->ie_code == '\r')
	    (void) sprintf(buf, "folder %s", panel_get_value(item));
	else
	    (void) sprintf(buf, "folder ! %s", panel_get_value(item));

    else if (!strcmp(which, "directory:"))
	(void) sprintf(buf, "cd %s", panel_get_value(item));

    else if (!msg_cnt)
	print("No messages to save");

    else if (!strcmp(which, "filename:")) {
	int x = 1;
	if (event->ie_code == '\n' || event->ie_code == '\r')
	    (void) strcpy(buf, "save  ");
	else
	    (void) strcpy(buf, "write ");
	if (!(p = panel_get_value(item)) || !*p &&
	    (!(p = do_set(set_options, "mbox")) || !*p))
		p = DEF_MBOX;
	(void) sprintf(buf+6, "%d %s", current_msg+1, p);
	print("save message %d in %s? ", current_msg+1, p);
	if ((x = confirm(print_sw->ts_windowfd)) != 'y' && x != MS_LEFT) {
	    print("Message not saved");
	    return;
	}
    }
    (void) cmd_line(buf, msg_list);
}

do_file_dir(item, value, event)
Panel_item item;
int value;
struct inputevent *event;
{
    char buf[92];
    int x; /* used for confirmation */

    if (item == folder_item) {
	(void) strcpy(buf, "folder ");
	if (event->ie_code == MS_LEFT) {
	    panel_set(file_item, PANEL_LABEL_STRING, "folder:", 0);
	    panel_set(file_item, PANEL_MENU_CHOICE_STRINGS,
			    "Change without updating current folder", 0, 0);
	} else {
	    if (!value)
		(void) strcat(buf, "%");
	    else if (value == 1)
		(void) strcat(buf, "&");
	    else if (value == 2)
		(void) strcat(buf, "#");
	    else {
		(void) sprintf(buf, "folder %s",
			      panel_get(item, PANEL_CHOICE_STRING, value));
		if (!strcmp(buf+8, "Help"))
		    return help(panel_sw->ts_windowfd, "folder", tool_help);
	    }
	}
    } else if (item == cd_item) {
	(void) strcpy(buf, "cd ");
	if (event->ie_code == MS_LEFT || !value) {
	    panel_set(file_item, PANEL_LABEL_STRING, "directory:", 0);
	    panel_set(file_item, PANEL_MENU_CHOICE_STRINGS,
				"Change to specified directory", 0, 0);
	} else if (value == 1)
	    (void) strcat(buf, "~");
	else if (value == 2)
	    (void) strcat(buf, "+");
	else
	    return help(panel_sw->ts_windowfd, "chdir", tool_help);
    } else if (item == save_item) {
	(void) strcpy(buf, "save ");
	if (event->ie_code == MS_LEFT)
	    if (!strcmp("filename:", panel_get(file_item,PANEL_LABEL_STRING))) {
		event->ie_code = '\n';  /* let file_dir think it got a \n */
		return file_dir(file_item, event);
	    } else {
		panel_set(file_item, PANEL_LABEL_STRING, "filename:", 0);
		panel_set(file_item, PANEL_MENU_CHOICE_STRINGS,
			"Save message WITHOUT headers", 0,0);
		print("Type in Main Panel Window a filename to save message");
		return;
	    }
	else if (value == 1) {
	    register char *p = panel_get_value(file_item);
	    if ((!p || !*p) && (!(p = do_set(set_options, "mbox")) || !*p))
		p = DEF_MBOX;
	    print("Save in %s? ", p);
	    if ((x = confirm(panel_sw->ts_windowfd)) != 'y' && x != MS_LEFT) {
		print("Message not saved");
		return;
	    }
	    (void) sprintf(buf, "save \"%s\" %s",
					   panel_get_value(msg_num_item), p);
	    panel_set(msg_num_item, PANEL_VALUE, NO_STRING, 0);
	} else {
	    (void) sprintf(buf, "save %s",
		panel_get(item, PANEL_CHOICE_STRING, value));
	    if (!strcmp(buf+6, "Help"))
		return help(panel_sw->ts_windowfd, "save", tool_help);
	}
    }
    (void) cmd_line(buf, msg_list);
    panel_set(item, PANEL_VALUE, NO_STRING, 0); /* remove last value */
}

text_done(item, event)
Panel_item item;
struct inputevent *event;
{
    char opt[30], buf[82], cmd[82];
    register char *p;
    Panel_item which = NO_ITEM;
    int set_it;

    if ((event->ie_code == '\n' || event->ie_code == '\r') && 
				 *strcpy(buf, panel_get_value(item))) {
	(void) strcpy(opt, panel_get(item, PANEL_LABEL_STRING));
	set_it = (*opt == 'S');
	if (!(p = index(opt, ' '))) {
	    print("Hmmm... there seems to be a problem here.");
	    return;
	}
	++p;
	switch(lower(*p)) {
	    case 'o':
		(void) sprintf(cmd, "%set %s", (set_it)? "s": "uns", buf);
		which = option_item;
	    when 'i':
		(void) sprintf(cmd, "%sgnore %s", (set_it)? "i": "uni", buf);
		which = ignore_item;
	    when 'a':
		(void) sprintf(cmd, "%slias %s", (set_it)? "a": "una", buf);
		which = alias_item;
	    otherwise: print("HUH!? (%c)", *p); return;
	}
	(void) cmd_line(buf, msg_list);
    }
    panel_set(input_item, PANEL_VALUE, NO_STRING, 0); /* remove last value */
    panel_set(item, PANEL_SHOW_ITEM, FALSE, 0);
}

do_help(item, value, event)
Panel_item item;
register int value;
struct inputevent *event;
{
    register char *p, *helpfile = tool_help;
    switch(value) {
	case 1: p = "help";
	when 2: p = "mouse";
	when 3: p = "windows";
	when 4: p = "function keys";
	when 5: p = "hdr_format", helpfile = cmd_help;
	when 6: p = "msg_list", helpfile = cmd_help;
	otherwise: p = "general";
    }
    (void) help(panel_sw->ts_windowfd, p, helpfile);
}

toolquit(item, value, event)
Panel_item item;
int value;
struct inputevent *event;
{
    register int which;

    if (!value || event->ie_code == MS_LEFT) {
	do_update(NO_ITEM, 0, NO_EVENT);
	turnoff(glob_flags, NEW_MAIL);
	mail_status(0); /* lower flag (if up) print current num of msgs */
	wmgr_changestate (tool->tl_windowfd, rootfd, TRUE);
	wmgr_changelevel (tool->tl_windowfd, parentfd, TRUE);
	return;
    } else if (value == 2) {
	(void) help(panel_sw->ts_windowfd, "quit", tool_help);
	return;
    }
    print("Left updates changes. Middle does not. Right aborts quit.");
    if ((which = confirm(panel_sw->ts_windowfd)) == MS_RIGHT) {
	print("Quit aborted.");
	return;
    }
    abort_mail(NO_ITEM, 0);
    if (which == MS_LEFT)
	lock_cursors(), copyback();
    else
	print("Bye bye");
    cleanup(0);
}

do_lpr(item, value, event)
Panel_item item;
int value;
struct inputevent *event;
{
    char buf[128];

    if (event && (event->ie_code == MS_LEFT || value == 1)) {
	print("Sending message %d to printer...", current_msg+1);
	(void) strcpy(buf, "lpr ");
	if (value)
	    (void) sprintf(buf, "lpr \"%s\"", panel_get_value(msg_num_item));
	lock_cursors();
	(void) cmd_line(buf, msg_list);
	unlock_cursors();
    } else
	(void) help(panel_sw->ts_windowfd, "printer", tool_help);
}

do_clear()
{
    /* actions that clears window indicates user wants to quit getting opts */
    if (msg_pix)
	pr_destroy(msg_pix), msg_pix = (struct pixrect *)NULL;
    if (getting_opts)
	getting_opts = 0, unlock_cursors();
    pw_writebackground(msg_win, 0,0, msg_rect.r_width,msg_rect.r_height,
		       PIX_CLR);
    txt.x = 5, txt.y = l_height(curfont) - 1;
}

do_update(item, value, event)
Panel_item item;
register int value;
struct inputevent *event;
{
    char *argv[2];
    if (event && event->ie_code != MS_LEFT)
	return help(panel_sw->ts_windowfd, "update", tool_help);
    argv[0] = "update";
    argv[1] = NULL;
    (void) folder(0, argv, NULL);
}

/* panel selction button to send a letter.
 * add a CR if necessary, and finish up letter
 */
do_send(item, value, event)
Panel_item item;
register int value;
register struct inputevent *event;
{
    if (event->ie_code != MS_LEFT)
	return help(panel_sw->ts_windowfd, "send", tool_help);
    if (txt.x > 5) {
	pw_char(msg_win, txt.x,txt.y, PIX_CLR, fonts[curfont], '_');
	add_to_letter(rite('\n')); /* if line isn't complete, flush it */
    }
    finish_up_letter();
}

do_edit(item, value, event)
Panel_item item;
register int value;
register struct inputevent *event;
{
    char buf[4];
    if (event->ie_code != MS_LEFT)
	return help(panel_sw->ts_windowfd, "edit", tool_help);
    if (txt.x > 5)
	add_to_letter(rite('\n')); /* flush line for him */
    add_to_letter(sprintf(buf, "%cv", *escape));
}

do_compose(item, value, event)
Panel_item item;
register int value;
struct inputevent *event;
{
    if (event && event->ie_code != MS_LEFT)
	return help(panel_sw->ts_windowfd, "compose", tool_help);
    print("Composing letter.");
    win_setcursor(msg_sw->ts_windowfd, &write_cursor);
    do_mail(0, DUBL_NULL, NULL);
}

change_font(item, value, event)
Panel_item item;
register int value;
struct inputevent event;
{
    if (ison(glob_flags, IS_GETTING))
	pw_char(msg_win, txt.x, txt.y, PIX_SRC^PIX_DST, fonts[curfont], '_');
    curfont = value % total_fonts;
    print("New font: %s\n",
	    (!curfont)? "Normal": (curfont == 1)? "Small": "Large");
    if (ison(glob_flags, IS_GETTING))
	pw_char(msg_win, txt.x, txt.y, PIX_SRC^PIX_DST, fonts[curfont], '_');
    crt = msg_rect.r_height / l_height(curfont);
}
