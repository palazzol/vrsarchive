/* @(#)hdr_panel.c	(c) copyright	10/18/86 (Dan Heller) */

#include "mush.h"

make_hdr_panel(choice_args, button_args)
char **choice_args, **button_args;
{
    hdr_panel_sw = panel_create(tool,
	PANEL_HEIGHT, 30,
	0);
    hdr_panel = (Panel)hdr_panel_sw->ts_data;

    msg_num_item = panel_create_item(hdr_panel, PANEL_TEXT,
	PANEL_ATTRIBUTE_LIST,		choice_args,
	PANEL_ITEM_X,			4,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_STRING,		"Range:",
	PANEL_MENU_CHOICE_STRINGS, 	"Help", 0,
	PANEL_VALUE_DISPLAY_LENGTH, 	10,
	PANEL_VALUE_STORED_LENGTH, 	80,
	PANEL_LABEL_FONT, 		fonts[DEFAULT],
	PANEL_NOTIFY_STRING, 		"\n\r",
	PANEL_NOTIFY_PROC, 		msg_num_done,
	0);

    sub_hdr_item[0] = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			149,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,    		&mouse_left,
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC,    		read_mail,
	0);
    sub_hdr_item[1] = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			174,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_STRING,   		"Read ",
	PANEL_MENU_TITLE_IMAGE, 	&mouse_left,
	PANEL_CHOICE_STRINGS,		"Help", 0,
	PANEL_NOTIFY_PROC,    		read_mail,
	0);
    sub_hdr_item[2] = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			223,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,    		&mouse_middle,
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC,    		delete_mail,
	0);
    sub_hdr_item[3] = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			248,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_STRING,   		"Delete ",
	PANEL_MENU_TITLE_IMAGE, 	&mouse_middle,
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC,    		delete_mail,
	0);
    sub_hdr_item[4] = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			313,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,    		&mouse_right,
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC,    		read_mail,
	0);
    sub_hdr_item[5] = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			338,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_STRING,   		"Menu ",
	PANEL_MENU_TITLE_IMAGE,	    	&mouse_right,
	PANEL_CHOICE_STRINGS, 		"Help", 0,
	PANEL_NOTIFY_PROC,    		read_mail,
	0);

    hdr_display = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			387,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(hdr_panel, "Display", 7, fonts[DEFAULT]),
	PANEL_MENU_TITLE_STRING, 	"Header Display Formats",
	PANEL_CHOICE_STRINGS, 		"Show Deleted Messages",
					"Don't Show Deleted Messages",
					"Current Header in Bold Text",
					"Current Header in Reverse Text",
					"Help",
					0,
	PANEL_NOTIFY_PROC, 		display_hdrs,
	0);

    ignore_item = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, choice_args,
	PANEL_ITEM_X,			464,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(hdr_panel, "Headers", 7, fonts[DEFAULT]),
	PANEL_MENU_TITLE_STRING, 	"Ignored Headers",
	PANEL_CHOICE_STRINGS, 		"Current Settings",
					"Add Values",
					"Delete Values",
					"Help",
					0,
	PANEL_NOTIFY_PROC, 		p_set_opts,
	0);

    next_scr = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			541,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(hdr_panel, "Next", 4, fonts[DEFAULT]),
	PANEL_MENU_TITLE_STRING, 	"Display Message Headers",
	PANEL_CHOICE_STRINGS, 		"Show Next screenful",
					"Show Previous screenful",
					0,
	PANEL_SHOW_ITEM, 		FALSE,
	PANEL_NOTIFY_PROC,		do_hdr,
	0);

    prev_scr = panel_create_item(hdr_panel, PANEL_CHOICE,
	PANEL_ATTRIBUTE_LIST, 		choice_args,
	PANEL_ITEM_X,			594,
	PANEL_ITEM_Y,			4,
	PANEL_LABEL_IMAGE,
	    panel_button_image(hdr_panel, "Prev", 4, fonts[DEFAULT]),
	PANEL_MENU_TITLE_STRING, 	"Display Message Headers",
	PANEL_CHOICE_STRINGS, 		"Show Previous screenful",
					"Show Next screenful",
					0,
	PANEL_SHOW_ITEM, 		FALSE,
	PANEL_NOTIFY_PROC, 		do_hdr,
	0);
}
