/* @(#)hdr_procs.c	(c) copyright	10/18/86 (Dan Heller) */

/* hdr_procs.c  -- panel item procedures for the message hdrs */
#include "mush.h"

/* which message headers are to be displayed */
do_hdr(foo, value, event)
Panel_item foo;
int value;
struct inputevent *event;
{
    register char *argv[3];
    argv[2] = NULL;
    argv[0] = "headers";

    if (!msg_cnt) {
	print("No Mail.");
	return;
    }
    if (!value || event->ie_code == MS_LEFT)
	argv[1] = (foo == next_scr)? "+": "-";
    else
	argv[1] = (foo == next_scr)? "-": "+";

    panel_set(foo, PANEL_VALUE, 0, 0);
    (void) do_hdrs(2, argv, NULL);
}

/* alters display of the message headers */
display_hdrs(foo, value, event)
Panel_item foo;
int value;
struct inputevent *event;
{
    int show_deleted = !!do_set(set_options, "show_deleted");

    if (event->ie_code != MS_LEFT) {
	switch(value) {
	    case 0 : case 1: {
		char buf[25];
		show_deleted = !value;
		(void) cmd_line(sprintf(buf, "%sset show_deleted",
			(value == 0)? "" : "un"), msg_list);
	    }
	    when 2 :
		turnoff(glob_flags, REV_VIDEO);
	    when 3 :
		turnon(glob_flags, REV_VIDEO);
	    when 4:
		return help(hdr_panel_sw->ts_windowfd, "headers", tool_help);
	}
	(void) do_hdrs(0, DUBL_NULL, NULL);
    }
    print("%sShow Deleted headers, \"Current Message\" header in %s",
			(show_deleted)? NO_STRING: "Don't ",
			(ison(glob_flags, REV_VIDEO))? "reverse": "boldface");
}

p_set_opts(item, value, event)
Panel_item item;
int value;
struct inputevent *event;
{
    static char   *p, *oldp;
    static char   buf[8];

    if (event->ie_code == MS_LEFT)
	value = 0;
    if (value && (value != 3 || value != 2)
	      && panel_get(input_item, PANEL_SHOW_ITEM)) {
	print("Need value for %s first!", oldp);
	return;
    } else if (!value && ison(glob_flags, IS_GETTING)) {
	print("Finish editing letter first");
	return;
    } else if (item == option_item) {
	if (!value) /* first menu item */
	    view_options();
	else if (value == 1)
	    set_fkeys();
	else
	    (void) help(panel_sw->ts_windowfd, "opts", tool_help);
	return;
    } else if (item == ignore_item)
	if (value == 3)
	    return help(hdr_panel_sw->ts_windowfd, "ignore", tool_help);
	else
	    oldp = p = strcpy(buf, "ignore");
    else if (item == alias_item)
	if (value == 3)
	    return help(panel_sw->ts_windowfd, "aliases", tool_help);
	else
	    oldp = p = strcpy(buf, "alias");
    if (value) {
	char tmp[30];
	(void) sprintf(tmp, "%set %s:", (value == 1)? "S": "Uns", p);
	panel_set(input_item,
	    PANEL_LABEL_STRING, tmp,
	    PANEL_MENU_CHOICE_STRINGS, "Abort", 0,
	    PANEL_SHOW_ITEM, TRUE,
	    0);
	oldp = p;
	return;
    }
    panel_set(item, PANEL_VALUE, 0, 0);
    do_clear();
    pw_text(msg_win, l_width(DEFAULT), 15, PIX_SRC, fonts[LARGE], p);
    if (item != ignore_item)
	pw_text(msg_win, 30*l_width(DEFAULT),15,PIX_SRC,fonts[LARGE],"Values");
    (void) cmd_line(p, msg_list);
}

msg_num_done(item, event)
Panel_item item;
struct inputevent *event;
{
    char buf[82];
    register char *p;
    int n;

    if (event->ie_code != '\n' && event->ie_code != '\r')
	return help(hdr_panel_sw->ts_windowfd, "message range", tool_help);
    (void) sprintf(buf, "headers %s", (p = (char *)panel_get_value(item)));
    panel_set(item, PANEL_VALUE, NO_STRING, 0);
    if (!(n = chk_msg(p)))
	return;
    current_msg = --n;
    (void) cmd_line(buf, msg_list);
    display_msg(n, (long)0);
}

do_sort(item, value, event)
Panel_item item;
int value;
struct inputevent *event;
{
    register char *argv[3];
    argv[0] = "sort";
    argv[2] = NULL;
    if (event->ie_code == MS_LEFT)
	argv[1] = do_set(set_options, "sort");
    else switch(value) {
	case 0: argv[1] = "d";
	when 1: argv[1] = "a";
	when 2: argv[1] = "s";
	when 3: argv[1] = "S";
	when 4: argv[1] = "R";
	when 5: argv[1] = "-?";
    }
    (void) sort(2, argv, NULL);
    if (value != 5)
	(void) do_hdrs(0, DUBL_NULL, NULL);
    panel_set(item, PANEL_VALUE, 0, 0);
}
