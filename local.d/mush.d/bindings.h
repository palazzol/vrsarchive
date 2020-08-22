/* bindings.h  -- command bindings */

/* to see if a key sequece matches, prefixes or misses a set binding */
#define NO_MATCH	0
#define MATCH		1
#define A_PREFIX_B	2
#define B_PREFIX_A	3

#define NULL_MAP	(struct cmd_map *)0
#define C_NULL		0
#define C_GOTO_MSG	1
#define C_WRITE_MSG	2
#define C_WRITE_LIST	3
#define C_SAVE_MSG	4
#define C_SAVE_LIST	5
#define C_COPY_MSG	6
#define C_COPY_LIST	7
#define C_DELETE_MSG	8
#define C_DELETE_LIST	9
#define C_UNDEL_MSG	10
#define C_UNDEL_LIST	11
#define C_REVERSE	12
#define C_REDRAW	13
#define C_NEXT_MSG	14
#define C_PREV_MSG	15
#define C_FIRST_MSG	16
#define C_LAST_MSG	17
#define C_TOP_PAGE	18
#define C_BOTTOM_PAGE	19
#define C_NEXT_SCREEN	20
#define C_PREV_SCREEN	21
#define C_SHOW_HDR	22
#define C_SOURCE	23
#define C_SAVEOPTS	24
#define C_NEXT_SEARCH	25
#define C_PREV_SEARCH	26
#define C_CONT_SEARCH	27
#define C_PRESERVE	28
#define C_SORT		29
#define C_REV_SORT	30
#define C_QUIT_HARD	31
#define C_QUIT		32
#define C_EXIT_HARD	33
#define C_EXIT		34
#define C_UPDATE	35
#define C_FOLDER	36
#define C_SHELL_ESC	37
#define C_CURSES_ESC	38
#define C_PRINT_MSG	39
#define C_CHDIR		40
#define C_VAR_SET	41
#define C_IGNORE	42
#define C_ALIAS		43
#define C_OWN_HDR	44
#define C_VERSION	45
#define C_MAIL_FLAGS	46
#define C_MAIL		47
#define C_REPLY_SENDER	48
#define C_REPLY_ALL	49
#define C_DISPLAY_MSG	50
#define C_TOP_MSG	51
#define C_DISPLAY_NEXT	52
#define C_BIND		53
#define C_UNBIND	54
#define C_HELP		55 /* THIS MUST BE THE LAST ITEM */
