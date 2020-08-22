/* @(#)edit_menu.c	(c) copyright	10/18/86 (Dan Heller) */

/* edit_menu.c
 * 
 * Routine which handles the menu you get when in the msg_win while editing
 * a letter.  This is just a fancy front end for ~ commands (nyuk, nyuk).
 */
#include "mush.h"

#define EDIT_IT     	(char *)'e'
#define PAGE     	(char *)'p'
#define INC 	 	(char *)'i'
#define INC_HDR		(char *)'H'
#define FWD_MSG		(char *)'f'
#define TO_LIST		(char *)'t'
#define SUBJECT		(char *)'s'
#define CC_LIST		(char *)'c'
#define BC_LIST		(char *)'b'
#define ALL_HDR		(char *)'h'
#define SIGN_IT		(char *)'S'
#define NO_SIGN		(char *)'n'
#define FORT		(char *)'F'
#define NO_FORT		(char *)'N'
#define ERASE   	(char *)'E'
#define SEND	   	(char *)'X'
#define ABORT   	(char *)'q'
#define MENU_HELP	(char *)'?'

static struct menuitem edit_items[] = {
    { MENU_IMAGESTRING,  "Enter Editor",	EDIT_IT  },
    { MENU_IMAGESTRING,  "Send Letter",		SEND     },
    { MENU_IMAGESTRING,  "Abort Letter",	ABORT    },
    { MENU_IMAGESTRING,  "Review Letter",	PAGE     },
    { MENU_IMAGESTRING,  "Include Message",	INC      },
    { MENU_IMAGESTRING,  "Inc. msg w/hdr",	INC_HDR  },
    { MENU_IMAGESTRING,  "Forward message",	FWD_MSG  },
    { MENU_IMAGESTRING,  "Change To line",	TO_LIST  },
    { MENU_IMAGESTRING,  "Change Subject",	SUBJECT  },
    { MENU_IMAGESTRING,  "Change Cc list",	CC_LIST  },
    { MENU_IMAGESTRING,  "Change Bcc list",	BC_LIST  },
    { MENU_IMAGESTRING,  "Change All hdrs",	ALL_HDR  },
    { MENU_IMAGESTRING,  "Add .signature",	SIGN_IT  },
    { MENU_IMAGESTRING,  "No .signature",	NO_SIGN  },
    { MENU_IMAGESTRING,  "Add a fortune",	FORT     },
    { MENU_IMAGESTRING,  "No fortune",		NO_FORT  },
    { MENU_IMAGESTRING,  "Erase Message",	ERASE    },
    { MENU_IMAGESTRING,  "Help",		MENU_HELP  }
};

static struct menu menu = {
    MENU_IMAGESTRING, "Editing options",
    sizeof (edit_items) / sizeof (struct menuitem), edit_items,
    (struct menu *)NULL,
    NULL
};

edit_menu(event, fd)
struct inputevent *event;
{
    static char buf[5];
    struct menuitem *m_item;
    register char *action, *p = buf+1;
    struct menu *menu_ptr = &menu;

    if (!(m_item = menu_display(&menu_ptr, event, fd)) || get_hdr_field) {
	if (get_hdr_field)
	    print("Finish the message header first.");
	return;
    }
    action = m_item->mi_data;
    if (txt.x > 5)
	add_to_letter(rite('\n'));  /* flush line for him */

    if (!msg_cnt && (action == INC || action == INC_HDR || action == FWD_MSG)) {
	print("No messages to include");
	return;
    }
    buf[0] = *escape;
    switch(action) {
	case EDIT_IT  : (void) strcpy(p, "v");
	when PAGE     : (void) strcpy(p, "p");
	when INC      : (void) strcpy(p, "i");
	when INC_HDR  : (void) strcpy(p, "H");
	when ALL_HDR  : (void) strcpy(p, "h");
	when SUBJECT  : (void) strcpy(p, "s");
	when TO_LIST  : (void) strcpy(p, "t");
	when CC_LIST  : (void) strcpy(p, "c");
	when BC_LIST  : (void) strcpy(p, "b");
	when FWD_MSG  : (void) strcpy(p, "f");
	when SIGN_IT  : (void) strcpy(p, "S");
	when NO_SIGN  : (void) strcpy(p, "S!");
	when FORT     : (void) strcpy(p, "F");
	when NO_FORT  : (void) strcpy(p, "F!");
	when ERASE    : (void) strcpy(p, "E");
	when ABORT    : (void) strcpy(p, "q");
	when SEND     : finish_up_letter(); return;
	otherwise     : (void) strcpy(p, "?");
    }
    add_to_letter(buf);
}
