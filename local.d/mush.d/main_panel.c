/* "@(#)main_panel.c	(c) copyright	10/18/86 (Dan Heller) */

#include "mush.h"

make_main_panel(choice_args, button_args)
char **choice_args, **button_args;
{
    /* main panel stuff: */
    panel_sw = panel_create(tool,
	PANEL_HEIGHT, 80,
	0);
    main_panel = (Panel)panel_sw->ts_data;

    quit_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			4,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Done", 6, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Done",
	PANEL_CHOICE_STRINGS, 		"Close to Icon",
					"Quit Tool",
					"Help",
					0,
	PANEL_NOTIFY_PROC, 		toolquit,
	0);

    help_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			79,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Help", 4, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Available Help",
	PANEL_CHOICE_STRINGS, 		"General",
					"Help with \"help\"",
					"The Mouse",
					"Windows",
					"Function Keys",
					"Message headers",
					"Message lists",
					0,
	PANEL_NOTIFY_PROC, 		do_help,
	0);

    read_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			136,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Next", 4, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Next Message",
	PANEL_CHOICE_STRINGS, 		"Read Next", "Help", 0,
	PANEL_NOTIFY_PROC, 		read_mail,
	0);

    respond_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			193,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Reply", 5, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Respond to Current Message",
	PANEL_CHOICE_STRINGS, 		"Sender Only",
					"Sender Only (include msg)",
					"All Recipients",
					"All Recipients (include msg)",
					"Help", 0,
	PANEL_NOTIFY_PROC, 		respond_mail,
	0);

    delete_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			259,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Delete", 6, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Delete/Undelete Messages",
	PANEL_CHOICE_STRINGS, 		"Delete current message",
					"Delete All Messages",
					"Undelete current message",
					"Undelete All Messages",
					"Delete message \"range\"",
					"Undelete message \"Range\"",
					"Help", 0,
	PANEL_NOTIFY_PROC, 		delete_mail,
	0);

    sort_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			334,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Sort", 4, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Sort Messages",
	PANEL_CHOICE_STRINGS, 		"By Date",
					"By Author",
					"By Subject",
					"By Subject (ignore Re:)",
					"By Status",
					"Help", 0,
	PANEL_NOTIFY_PROC, 		do_sort,
	0);

    option_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			391,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Opts", 4, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Mail Options",
	PANEL_CHOICE_STRINGS, 		"Set Options", "Function keys",
					"Help", 0,
	PANEL_NOTIFY_PROC, 		p_set_opts,
	0);

    alias_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			448,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Aliases", 7, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Mail Aliases",
	PANEL_CHOICE_STRINGS, 		"Current Aliases",
					"Add/Change alias",
					"Unalias", "Help", 0,
	PANEL_NOTIFY_PROC, 		p_set_opts,
	0);

    comp_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			532,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Compose", 8, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Compose a letter",
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC, 		do_compose,
	0);

    file_item = panel_create_item(main_panel, PANEL_TEXT,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			4,
	PANEL_ITEM_Y,			30,
	PANEL_LABEL_FONT, 		fonts[DEFAULT],
	PANEL_SHOW_MENU,		TRUE,
	PANEL_LABEL_STRING, 		"filename:",
	PANEL_MENU_CHOICE_STRINGS,	"Save message without message header",0,
	PANEL_VALUE_DISPLAY_LENGTH, 	35,
	PANEL_NOTIFY_STRING, 		"\n\r",
	PANEL_NOTIFY_PROC, 		file_dir,
	0);

    input_item = panel_create_item(main_panel, PANEL_TEXT,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			373,
	PANEL_ITEM_Y,			30,
	PANEL_SHOW_ITEM, 		FALSE,
	PANEL_SHOW_MENU, 		TRUE,
	PANEL_LABEL_FONT, 		fonts[DEFAULT],
	PANEL_VALUE_DISPLAY_LENGTH, 	20,
	PANEL_NOTIFY_STRING, 		"\n\r",
	PANEL_NOTIFY_PROC, 		text_done,
	0);

    print_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			4,
	PANEL_ITEM_Y,			50,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Printer", 7, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Printing Messages",
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC, 		do_lpr,
	0);

    folder_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			88,
	PANEL_ITEM_Y,			50,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "folder", 6, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Change folder",
	PANEL_CHOICE_STRINGS, 		"System Mailbox",
					"Main Mailbox",
					"Last Accessed Folder",
					0,
	PANEL_NOTIFY_PROC, 		do_file_dir,
	0);

    add_folder_to_menu(folder_item, 3);

    save_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			163,
	PANEL_ITEM_Y,			50,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Save", 4, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Save messages",
	PANEL_CHOICE_STRINGS, 		"Save in ~/mbox",
					"Save message \"range\"",
					0,
	PANEL_NOTIFY_PROC, 		do_file_dir,
	0);

    add_folder_to_menu(save_item, 2);

    cd_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			220,
	PANEL_ITEM_Y,			50,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "chdir", 5, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Change Working Directory",
	PANEL_CHOICE_STRINGS, 		"Print Current directory",
					"HOME directory",
					"Private Mail directory.",
					"Help", 0,
	PANEL_NOTIFY_PROC, 		do_file_dir,
	0);

    update_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			286,
	PANEL_ITEM_Y,			50,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Update", 6, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Updating folders",
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC, 		do_update,
	0);

    send_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			361,
	PANEL_ITEM_Y,			50,
	PANEL_SHOW_ITEM, 		FALSE,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Send", 6, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Send Letter",
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC, 		do_send,
	0);

    edit_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			436,
	PANEL_ITEM_Y,			50,
	PANEL_SHOW_ITEM, 		FALSE,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Editor", 4, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Editing",
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC, 		do_edit,
	0);

    abort_item = panel_create_item(main_panel, PANEL_BUTTON,
	PANEL_ATTRIBUTE_LIST, 		button_args,
	PANEL_ITEM_X,			511,
	PANEL_ITEM_Y,			50,
	PANEL_SHOW_ITEM, 		FALSE,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Abort", 5, fonts[LARGE]),
	PANEL_NOTIFY_PROC, 		abort_mail,
	0);

    font_item = panel_create_item(main_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			577,
	PANEL_ITEM_Y,			50,
	PANEL_LABEL_IMAGE,
	    panel_button_image(main_panel, "Fonts", 5, fonts[LARGE]),
	PANEL_MENU_TITLE_STRING, 	"Fonts",
	PANEL_SHOW_MENU_MARK, 		TRUE,
	PANEL_CHOICE_FONTS, 		fonts[0], fonts[1], fonts[2], 0,
	PANEL_CHOICE_STRINGS, 		"Default", "Small", "Large", 0,
	PANEL_NOTIFY_PROC, 		change_font,
	0);
}

/*
 * Open the user's mail folder (either user set or default path) and find all
 * the files (assumed to be mail folders) and add them to the menu list of
 * folders to use.
 */
add_folder_to_menu(item, n)
struct panel_item *item;
register int 	  n;
{
    register FILE 	*pp = NULL_FILE;
    register char 	*p = do_set(set_options, "folder");
    register char	*tmp = NULL;
    int			x = 0;
    char 		buf[128], path[128];

    if (p) {
	tmp = getpath(p, &x);
	if (x == -1) {
	    if (errno != ENOENT)
		print("%s: %s\n", p, tmp);
	    tmp = NULL;
	}
    }
    if (p = tmp) {
	p = sprintf(buf, "%s %s", LS_COMMAND, p);
	if (!(pp = popen(buf, "r")))
	    error(buf);
	else {
	    *path = '+';
	    while (fgets(path+1, 128, pp)) {
		struct stat s_buf;
		if (p = index(path+1, '\n'))
		    *p = 0;
		(void) sprintf(buf, "%s/%s", tmp, path+1);
		if (stat(buf, &s_buf) || s_buf.st_mode & S_IFDIR)
		    continue;
		panel_set(item, PANEL_CHOICE_STRING, n++, path, 0);
	    }
	}
    }
    panel_set(item, PANEL_CHOICE_STRING, n, "Help", 0);
}
