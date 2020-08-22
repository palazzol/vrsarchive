/* @(#)mush.h	(c) copyright 1986 (Dan Heller) */

#define VERSION "Mail User's Shell (Vers 5.7) Sun Sep  6 19:10:48 PDT 1987"

#include "config.h"

#ifdef CURSES
#include <curses.h>
#else CURSES
#include <stdio.h>
#endif /* CURSES */

#include <ctype.h>
#include <errno.h>
#include <setjmp.h>
#include "strings.h"

#ifdef SUNTOOL
#    include <suntool/tool_hs.h>
#else  SUNTOOL
#    include <sys/types.h>
#    include <signal.h>
#    ifndef SYSV
#        include <sys/time.h>
#	 include <sys/ioctl.h>   /* for ltchars */
#    else
#        include <time.h>
#        include <fcntl.h>
#    endif /* SYSV */
#endif /* SUNTOOL */

#include <sys/stat.h>
#include <sys/file.h>

#ifdef SUNTOOL
#    include <suntool/gfxsw.h>
#    include <suntool/panel.h>
#    include <suntool/ttysw.h>
#    include <suntool/ttytlsw.h>
#    include <suntool/menu.h>
#    include <suntool/icon_load.h>
#endif /* SUNTOOL */

/* if no maximum number of files can be found, we'll use getdtablesize() */
#ifdef _NFILE
#    define MAXFILES _NFILE
#else
#ifdef NOFILE
#    define MAXFILES NOFILE
#endif
#endif

#ifndef CTRL
#define CTRL(c)		('c' & 037)
#endif

#define ESC 		'\033'

#define NO_STRING	""
#ifdef  NULL
#undef  NULL
#endif /*  NULL */
#define NULL		(char *)0
#define NULL_FILE	(FILE *)0
#define DUBL_NULL	(char **)0
#define TRPL_NULL	(char ***)0
#ifdef putchar
#undef putchar
#endif /* putchar */
#define putchar(c)	fputc(c, stdout)
#define bell() 		fputc('\007', stderr)

#define on_intr() \
    turnoff(glob_flags, WAS_INTR), oldint = signal(SIGINT, interrupt), \
    oldquit = signal(SIGQUIT, interrupt)

#define off_intr() \
    turnoff(glob_flags, WAS_INTR), (void) signal(SIGINT, oldint), \
    (void) signal(SIGQUIT, oldquit)

/* Don't flush input when setting echo or cbreak modes (allow typeahead) */
#ifdef TIOCSETN
#ifndef stty
#define stty(fd, sgttybuf)	(ioctl(fd, TIOCSETN, sgttybuf))
#endif
#endif /* TIOCSETN */

#ifndef CURSES
/* if curses is not defined, simulate the same tty based macros */
struct sgttyb _tty;

#define crmode()   (_tty.sg_flags |= CBREAK,  stty(0, &_tty))
#define nocrmode() (_tty.sg_flags &= ~CBREAK, stty(0, &_tty))
#define echo()     (_tty.sg_flags |= ECHO,    stty(0, &_tty))
#define noecho()   (_tty.sg_flags &= ~ECHO,   stty(0, &_tty))
#define savetty()  (void) gtty(0, &_tty)
#endif /* ~CURSES */

#if 0
#define echo_on()	\
    if (_tty.sg_flags && isoff(glob_flags, ECHO_FLAG)) echo(), nocrmode()
#define echo_off()	\
    if (_tty.sg_flags && isoff(glob_flags, ECHO_FLAG)) noecho(), crmode()
#else
#define echo_on()	echo(),nocrmode()
#define echo_off()	noecho(),crmode()
#endif

#define strdup(dst, src) (xfree (dst), dst = savestr(src))
#define Debug		if (debug) printf

#ifdef SYSV
#define L_SET	0
#ifndef F_OK
#define F_OK	000
#define R_OK	004
#define W_OK	002
#endif
#ifndef E_OK
#define E_OK	001
#endif
#define u_long  unsigned long
#define vfork   fork
#define SIGCHLD SIGCLD
#endif /*  SYSV */

#if !defined(SUNTOOL) && !defined(CURSES)

#define TRUE		  1
#define FALSE		  0
#define print		  printf
#define wprint		  printf
#define print_more	  printf

#endif /* SUNTOOL && !CURSES */

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif /*  max */

#if defined(CURSES) && !defined(SUNTOOL)
#define wprint printf
#endif /* !SUNTOOL && CURSES */

#if defined(CURSES) || defined(SUNTOOL)
#define print_more	  turnon(glob_flags, CONT_PRNT), print
void print();		/* printf to window or curses or tty accordingly */
#endif /* CURSES || SUNTOOL */

#ifdef  SUNTOOL

#define NO_ITEM		  (Panel_item)0
#define NO_EVENT	  (struct inputevent *)0
#define TIME_OUT	  60           /* sleep 60 secs between mailchecks */
#define PIX_OR		  PIX_SRC ^ PIX_DST
#define ID 		  event.ie_code
#define l_width(font)	  fonts[font]->pf_defaultsize.x /* width of letter */
#define l_height(font)	  fonts[font]->pf_defaultsize.y /* height of letter */
#define Clrtoeol(w,x,y,f) pw_text(w, x, y, PIX_SRC, fonts[f], blank)

#define highlight(win,x,y,font,s) \
    pw_text(win,x,y, PIX_SRC, fonts[font],s), \
    pw_text(win,x+1,y, \
	(ison(glob_flags, REV_VIDEO))? PIX_NOT(PIX_SRC): PIX_SRC|PIX_DST, \
	fonts[font],s)

/* Fonts */
#define FONTDIR		  "/usr/lib/fonts/fixedwidthfonts"
#define DEFAULT		  0
#define SMALL	 	  1
#define LARGE 		  2
#define MAX_FONTS	  3

#endif /* SUNTOOL */

/* bits and pieces */
#define turnon(flg,val)   ((flg) |= ((u_long)1 << ((u_long)(val)-1L)))
#define turnoff(flg,val)  ((flg) &= ~((u_long)1 << ((u_long)(val)-1L)))
#define ison(flg,val)     ((u_long)(flg) & ((u_long)1 << ((u_long)(val)-1L)))
#define isoff(flg,val)    (!ison((flg), (val)))
#define set_isread(n)	  \
	if (ison(msg[n].m_flags, UNREAD)) \
	    turnon(glob_flags, DO_UPDATE), turnoff(msg[n].m_flags, UNREAD)

/* msg lists represented by bits */
#define clear_msg_list(list)  	(void) bzero(list, (msg_cnt+7)/8)
#define msg_bit(list, n)	((list[(n) / 8] >> ((n) % 8)) & 1)
#define set_msg_bit(list, n)	(list[(n) / 8] |= (1 << ((n) % 8)))
#define unset_msg_bit(list, n)  (list[(n) / 8] &= ~(1 << ((n) % 8)))
#define bput(S1, S2, Len, op)   				\
		{ 						\
		    register char *s1 = S1, *s2 = S2; 		\
		    register int len = Len; 			\
		    while(len--) 				\
			*s2++ op *s1++; 			\
		}
#define bitput(m1,m2,len,op)	bput(m1, m2, (((len)+7)/8), op)

/* convenience and/or readability */
#define when		  break;case
#define otherwise	  break;default
#define lower(c)	  (isupper(c)? tolower(c): c)
#define Lower(c)	  (c = lower(c))
#define upper(c)	  (islower(c)? toupper(c): c)
#define Upper(c)	  (c = upper(c))
#define skipspaces(n)     for(p += (n); *p == ' ' || *p == '\t'; ++p)
#define skipdigits(n)     for(p += (n); isdigit(*p); ++p)

#define NO_FLG		0

/* various flags */
long   glob_flags;	/* global boolean flags thruout the whole program */
#define DO_UPDATE   1	/* check for changes to avoid unnecessary copyback */
#define REV_VIDEO   2	/* reverse video for curses or toolmode */
#define CONT_PRNT   3	/* continue to print (maybe a printf) without a '\n' */
#define DO_SHELL    4	/* run a shell even if no mail? (true if tool) */
#define DO_PIPE     5	/* true if commands are piping to another command */
#define IS_PIPE     6	/* true if commands' "input" is piped from another */
#define IGN_SIGS    7	/* true if catch() should not longjump */
#define IGN_BANG    8	/* ignore ! as a history reference (see source()) */
#define ECHO_FLAG   9	/* if true, echo|cbreak is ON, don't echo typing (-e) */
#define IS_GETTING 10	/* true if we're getting input for a letter */
#define PRE_CURSES 11	/* true if curses will be run, but hasn't started yet */
#define READ_ONLY  12	/* -r passed to folder() (or main) setting read only */
#define REDIRECT   13	/* true if stdin is being redirected */
#define WAS_INTR   14	/* catch interrupts, set this flag (signals.c) */
#define WARNING    15   /* if set, various warning messages may be printed */
#define NEW_MAIL   17   /* new mail has arrived; user is busy or in icon mode */

#define VERBOSE		1       /* verbose flag for sendmail */
#define INCLUDE		2       /* include msg in response */
#define INCLUDE_H	3	/* include msg with header */
#define EDIT		4	/* enter editor by defualt on mailing */
#define SIGN		5	/* auto-include ~/.signature in mail */
#define DO_FORTUNE	6	/* add a fortune at end of msgs */

/* msg flags */
#define NO_HEADER	7	/* don't print header of message (top, write) */
#define DELETE		8
#define OLD	        9
#define UNREAD		10
#define UPDATE_STATUS	11	/* change status of msg when copyback */
#define NO_PAGE		12	/* don't page this message */
#define INDENT		13	/* indent included msg with string */
#define NO_IGNORE	14	/* don't ignore headers */
#define PRESERVE	15      /* preserve in mailbox unless deleted */
#define TOP		15	/* just print the top of the message */
#define FORWARD		16	/* Forward messages into the message buffer */

#define	MAXMSGS_BITS	MAXMSGS/sizeof(char)	/* number of bits for bitmap */

struct msg {
    u_long m_flags;
    long   m_offset;               /* offset in tempfile of msg */
    long   m_size;                 /* number of bytes in msg */
    int    m_lines;                /* number of lines in msg */
} msg[MAXMSGS];

struct options {
    char *option;
    char *value;
    struct options *next;
} *set_options, *aliases, *ignore_hdr, *functions, *fkeys, *own_hdrs;
#ifdef CURSES
struct options *bindings;
#endif /* CURSES */

struct cmd {
    char *command;
    int (*func)();
};
extern struct cmd ucb_cmds[];
extern struct cmd cmds[], hidden_cmds[];
#ifdef SUNTOOL
extern struct cmd fkey_cmds[];
#endif /* SUNTOOL */

FILE
    *tmpf,		/* temporary holding place for all mail */
    *open_file(),	/* open a file or program for write/append */
    *popen();		/* this should be in stdio.h */

extern char
    *sys_errlist[],    /* system's list of global error messages */
#ifdef SUNTOOL
    *font_files[], 	/* the names of the files fonts are kept in */
    *alt_fonts[], 	/* fonts to use if first ones don't work */
#endif /* SUNTOOL */
    **environ;		/* user's environment variables */

extern int errno;	/* global system error number */
jmp_buf jmpbuf;		/* longjmp to jmpbuf on sigs (not in tool) */

char
    debug,		/* debug causes various print statements in code */
    tempfile[40],	/* path to filename of temporary file */
    msg_list[MAXMSGS_BITS],	/* MAXMSGS bits of boolean storage */
    *cmd_help,		/* filename of location for "command -?" commands. */
    *login,		/* login name of user */
    *mailfile,		/* path to filename of current mailfile */
    *ourname[MAX_HOST_NAMES],	/* the name and aliases of the current host */
    *prompt,		/* the prompt string -- may have %d */
    *escape,		/* the escape character when without editor */
    *editor,	 	/* string describing editor to use (default vi) */
    *hdrs_only,		/* true if -H flag was given --set to args */
    *hdr_format,	/* set to the header format string; referenced a lot */
    *visual,	 	/* string describing visual editor to use */
    *pager,	 	/* string describing pager to use (default more) */
    *argv_to_string(),	/* convert a vector of strings into one string */
    **make_command(),	/* build a command vector (argv) */
    **mk_argv(),	/* given a string, make a vector */
    *itoa(),		/* return a string representation of a number */
    *lcase_strcpy(),	/* just like strcpy, but convert all chars to lower */
    *variable_stuff(),	/* return information about variables */
    *no_newln(),	/* remove newline and extra whitespace - return end */
    *savestr(),		/* strcpy arg into malloc-ed memory; return address */
    *date_to_string(),	/* returns a string described by parse_date() */
    *msg_date(),	/* return a string of the date of a message */
    *parse_date(),	/* parse an ascii date, and return message-id str */
    *Time(),		/* returns string expression of time (takes args) */
    *do_range(),	/* parse a string converting to a "range" of numbers */
    *getpath(),		/* static char returning path (expanding ~, +, %, #) */
    *compose_hdr(),	/* returns a formatted line describing passed msg # */
    *my_atoi(), 	/* do an atoi, but return the last char parsed */
    *do_set(),		/* set/unset an option, alias, ignored-hdr */
    *reply_to(),	/* who do we reply to when responding */
    *cc_to(),     	/* when responding, return str which is the cc-list */
    *subject_to(),      /* when responding, return str which is the subject */
    *header_field(),    /* the line in msg described by arg (message header) */
    *alias_to_address(),/* convert a name[list] to "real" names */
    *set_header(), 	/* [interactive] proc to set/display to/subject/cc */
    *getenv(), *prog_name;

int
    last_msg_cnt,	/* when checking for new mail, save the last msg_cnt */
    msg_cnt,		/* total number of messages */
    crt,		/* min number of lines msg contains to invoke pager */
    current_msg,	/* the current message we're dealing with */
    exec_pid,		/* pid of a command that has been "exec"ed */
    hist_no,		/* command's history number */
    iscurses;		/* if we're running curses */
    istool,		/* argv[0] == "xxxxtool", ranges from 0 to 2 */
    n_array[128],	/* array of message numbers in the header window */
    screen,		/* number of headers window can handle */

    quit(), cleanup(), catch(), do_alias(), respond(), cd(), sh(), stop(),
    folder(), save_msg(), delete(), do_mail(), lpr(), alts(), set(), do_hdrs(),
    rm_edfile(), pick(), save_opts(), preserve(), sort(), readmsg(),
    do_pick(), print_help(), folders(), question_mark(), do_from(), my_stty(),
    do_version(), disp_hist(), source(), do_echo(), sigchldcatcher(), ls(),
    nopenfiles(), Setenv(), Unsetenv(), Printenv(), bus_n_seg(), msg_flags(),
    toggle_debug(), stop_start(), interrupt();

long
    still_more,		/* there is still more message to display */
    spool_size,		/* size of sppol mail regardless of current folder */
    last_size,		/* the lastsize of the mailfile since last check */
    time();		/* satisfy lint */

void
    xfree(), free_vec(), error(), getmail(), mail_status(),
    file_to_fp(), init(), display_msg(), c_more();
    /* printf(), fclose(), fflush(), fputs(), fputc() */
#ifndef CURSES
struct sgttyb _tty;			/* tty characteristics */
#endif /* CURSES */
#ifdef TIOCGLTC
struct ltchars ltchars;			/* tty character settings */
#endif /* TIOCGLTC */

#ifdef CURSES

#define STANDOUT(y,x,s) standout(), mvaddstr(y,x,s), standend()
#define redraw()	clearok(curscr, TRUE), wrefresh(curscr)

int
    curses_init(),	/* interpret commands via the curses interface */
    bind_it();		/* bind chars or strings to functions */
#endif /* CURSES */

#ifdef SUNTOOL
void
    lock_cursors(), unlock_cursors(), scroll_win(),
    set_fkeys(), set_key(), toggle_opt(), help_opt();

char
    *rite(),		/* rite a char to msg_win: return string if c == '\n' */
    *find_key(),	/* pass x,y coords to find which function key assoc. */
    *key_set_to(),	/* pass fkey string, return string describing func */
    *panel_get(),      	/* returns what has been typed in a panel item */
    *tool_help,		/* help for tool-related things (sometimes, overlap) */
    blank[128];		/* use to clear to end of line */

int
    time_out,		/* time out interval to wait for new mail */
    rootfd,		/* the root window's fd */
    parentfd,		/* the parent's window fd */
    getting_opts,	/* true if getting/setting opts from msg_win */
    curfont,		/* the current font to use for mail message window */
    total_fonts,	/* total number of fonts available */
    get_hdr_field,	/* bitmask of header fields to be gotten */

    msg_io(), msgwin_handlesigwinch(), hdr_io(), hdrwin_handlesigwinch(),
    sigchldcatcher(), sigtermcatcher(), sigwinchcatcher(), do_sort(),
    do_compose(), do_edit(), read_mail(), delete_mail(), respond_mail(),
    do_hdr(), display_hdrs(), print_sigwinch(), p_set_opts(),
    tool_mgmt(), do_help(), text_done(), msg_num_done(), do_lpr(),
    toolquit(), change_font(), do_clear(), do_update(),
    file_dir(), do_file_dir(), do_send(), abort_mail(), check_new_mail(),
    fkey_cmd(), fkey_settings();

struct tchars  tchars;			/* more tty character settings */

struct tool *tool;      /* main tool structure */
struct toolsw
    *panel_sw,		/* main panel subwindow */
    *hdr_sw, 		/* subwindow for message headers */
    *hdr_panel_sw,	/* panel for headers */
    *tty_sw, 		/* subwindow which forks a shell (usually editor) */
    *print_sw, 		/* subwindow for print statements */
    *msg_sw; 		/* main subwindow to display messages and more */

struct pixwin
    *msg_win,		/* main pixwin for message display and more */
    *hdr_win,		/* pixwin for message headers */
    *print_win;		/* pixwin for printing messages ( print() ) */

struct pr_pos txt;   			/* current position of text written */
struct rect msg_rect, hdr_rect;         /* sizes of the main and hdr rects */
struct pixfont *fonts[MAX_FONTS];	/* array of fonts */

Panel
    main_panel,		/* the main panel dealing with generic items */
    hdr_panel;		/* panel which contains message header specific items */

Panel_item
    abort_item,		/* abort mail in progress */
    alias_item,		/* set/view/change current mail aliases */
    cd_item,		/* changes file_item to cd (for cd-ing) */
    comp_item,		/* compose a letter */
    delete_item,	/* delete/undelete messages */
    edit_item,		/* edit a message */
    font_item,		/* choose which font to use */
    folder_item,	/* change folders */
    file_item, 		/* text item for files or directories (forlder/cd) */
    hdr_display,	/* format message headers are displayed */
    help_item,		/* choose this to get various help */
    ignore_item,	/* set/view/change message headers to be ignored */
    input_item,		/* text item to get values for set/unsetting values */
    msg_num_item,	/* text item to explicity state which message to read */
    next_scr,		/* display the next screenful of message headers */
    option_item,	/* set/view/unset mail options */
    prev_scr,		/* display the previous screen of messages */
    print_item,		/* send current message to the printer */
    quit_item,		/* quit tool/close to icon */
    read_item,		/* read the current message */
    respond_item,	/* respond to messages */
    save_item,		/* saves messages; uses text item input_item */
    send_item,		/* when composing letter, this will send it off */
    sort_item,		/* sort routine... */
    sub_hdr_item[6],	/* display items that just sit there and give help */
    update_item;	/* commit changes to folder */

struct itimerval mail_timer;	/* frequency to check for new mail */

			/* mouse symbols and data */
/* left, middle and right mouse pixrects */
struct cursor
    l_cursor, m_cursor, r_cursor, coffee, read_cursor, write_cursor,
    main_cursor, checkmark;

struct pixrect *msg_pix; /* pixrect holding text of a message */
extern struct pixrect mouse_left, mouse_middle, mouse_right;
extern struct pixrect dn_arrow, up_arrow, cycle, shade_50;

extern struct icon mail_icon;
#endif /* SUNTOOL */
