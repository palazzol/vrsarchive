/* @(#)viewopts.c	(c) copyright	10/18/86 (Dan Heller) */

#include "mush.h"

struct viewopts {
    char *v_opt;
    char *v_prompt;
    int  v_usage;
#define TOOL  01
#define TEXT  02
    char *v_description;
};

/*
 * struct contains the option, a prompt if it has a string value, whether
 * or not it applies to non suntools, line mode, or both, and a
 * string describing what the option does. If the prompt string starts
 * with a minus sign, then the value can be set without a value. This
 * is there to indicate to option_line to print a toggle (cycle) pixrect
 * and to print TRUE/FALSE telling whether the value is on or off regardless
 * of it's "string" value.
 */
struct viewopts viewopts[] = {
    { "alwaysignore", NULL, TOOL | TEXT,
      "alwaysignore the message headers on the 'ignored' list." },
    { "askcc", NULL, TOOL | TEXT,
      "Ask for list of Carbon Copy recipients whenever sending mail.", },
    { "autodelete", NULL, TOOL | TEXT,
      "Automatically delete ALL READ messages whenever you update mail.", },
    { "autoedit", NULL, TOOL | TEXT,
      "Automatically enter editor whenever you REPLY to mail.", },
    { "autoinclude", NULL, TOOL | TEXT,
      "Include a copy of author's message each time you reply to mail." },
    { "autoprint", NULL, TOOL | TEXT,
      "Display the next message on the list when you delete a message." },
    { "auto_route", NULL, TOOL | TEXT,
      "Automatic optimization of uucp paths is done removing redundancies." },
    { "autosign", "-Filename", TOOL | TEXT,
      "Add file (~/.signature if set but no value) at end of all letters." },
    { "crt", "Lines", TEXT,
      "The number of lines a message must have for 'pager' to be invoked." },
    { "dead", "Filename", TOOL | TEXT,
      "The name of the file to store dead mail. ~/dead.letter by default." },
    { "dot", NULL, TOOL | TEXT,
      "allow \".\" on a line by itself to send letter." },
    { "editor", "Editor name/path", TOOL | TEXT,
      "editor to use by default. Default is evironment EDITOR or \"vi\"" },
    { "escape", "Character", TOOL | TEXT,
      "Escape character for extended editing commands. (default = ~)" },
    { "fixaddr", NULL, TOOL | TEXT,
      "makes \"replyall\" route recipient addresses through sender's host." },
    { "folder", "Pathname", TOOL | TEXT,
      "Full pathname to the directory where personal folders are kept." },
    { "fortune", "-Flag", TOOL | TEXT,
      "Add fortune to end of letters. Flag to \"fortune\" is optional" },
    { "fortunates", "Users", TOOL | TEXT,
      "Those who will receive fortunes if fortune is set (default: All)." },
    { "hdr_format", "Format", TOOL | TEXT,
      "Formatting string for headers. \"headers -?\" or help hdr_format" },
    { "history", "Number", TEXT,
      "How many commands to remember (like csh)." },
    { "hold", NULL, TOOL | TEXT,
      "Read but not deleted messages are saved in spool -- not mbox." },
    { "ignore_bang", NULL, TEXT,
      "Ignore '!' as a history reference. Otherwise, escape by: \\!" },
    { "ignoreeof", "-Command", TEXT,
      "Ignores ^D as exit, or (if set), execute \"command\"." },
    { "indent_str", "String", TOOL | TEXT,
      "String to offset included messages within your letter", },
    { "in_reply_to", NULL, TOOL | TEXT,
      "When responding to mail, add In-Reply-To: to message headers." },
    { "keepsave", NULL, TOOL | TEXT,
      "Prevents messages from being marked as `deleted' when you `save'." },
    { "known_hosts", "Host list", TOOL | TEXT,
      "List of hosts that your site is known to uucp mail to." },
    { "lister", "Arguemnts", TOOL | TEXT,
      "Arguments passed to the 'ls' command." },
    { "mbox", "Filename", TOOL | TEXT,
      "Filename to use instead of ~/mbox for default mailbox." },
    { "metoo", NULL, TOOL | TEXT,
      "When replying to mail, metoo preserves your name on mailing list." },
    { "newline", "-Command", TEXT,
      "Ignore RETURN. If set to a string, execute \"command\"" },
    { "no_hdr", NULL, TOOL | TEXT,
      "If set, personalized headers are NOT inserted to outgoing mail." },
    { "no_reverse", NULL, TOOL | TEXT,
      "disables reverse video in curses mode -- uses \"bold\" in tool mode." },
    { "nosave", NULL, TOOL | TEXT,
      "prevents aborted mail from being saved in dead.letter" },
    { "pager", "Program", TEXT,
      "Program name to be used as a pager for messages longer than crt." },
    { "print_cmd", "Program", TOOL | TEXT,
      "Alternate program to use to send messages to the printer." },
    { "printer", "Printer", TOOL | TEXT,
      "Printer to send messages to. Default is environment PRINTER" },
    { "prompt", "String", TEXT,
      "Your prompt.  \"help prompt\" for more information." },
    { "quiet", NULL, TEXT,
      "Don't print the version number of Mush on startup." },
    { "record", "Filename", TOOL | TEXT,
      "Save all outgoing mail in specified filename" },
    { "reply_to_hdr", "Headers", TOOL | TEXT,
      "List of headers to search in messages to construct reply adresses.", },
    { "screen", "Number of Headers", TEXT,
      "Number of headers to print in non-suntools (text) mode" },
    { "screen_win", "Number of Headers", TOOL,
      "Set the size of the header window." },
    { "show_deleted", NULL, TOOL | TEXT,
      "Show deleted messages in headers listings" },
    { "sort", "-Option", TOOL | TEXT,
      "Sorting upon startup of mail or `update/folder' (sort -? for help)" },
    { "squeeze", NULL, TOOL | TEXT,
      "When reading messages, squeeze all blank lines into one." },
    { "top", "Lines", TOOL | TEXT,
      "Number of lines to print of a message for the 'top' command."  },
    { "unix", NULL, TEXT,
      "Non-mush commands are considered to be UNIX commands." },
    { "verify", NULL, TEXT,
      "Verify to send, re-edit, or abort letter after editing." },
    { "visual", "Visual editor", TOOL | TEXT,
      "Visual editor to use by default. \"editor\" is used if not set." },
    { "warning", NULL, TOOL | TEXT,
      "Warns when standard variables are set differently from the default." }
};

static int total_opts;

#ifdef NOT_NOW
/*
 * put all the tool stuff at the beginning, or all the text stuff at
 * the beginning depending on whether or not we're running as a tool
 * With all the inappropriate variables out of the way, we set total
 * opts to the number of variables which are of the right type and
 * nothing inappropriate will ever be displayed.
 *
 * Note, this doesn't really work right now and I don't wanna fix it now..
 */
static
opt_eliminator(a, b)
register struct viewopts *a, *b;
{
    if (istool)
	return a->v_usage == TOOL;
    return a->v_usage == TEXT;
}
#endif NOT_NOW

static
opt_sorter(a, b)
register struct viewopts *a, *b;
{
    return !strcmp(a->v_opt, b->v_opt);
}

sort_variables()
{
    /*
    register int optnum;

    for (optnum=0; optnum < sizeof viewopts / sizeof(struct viewopts); optnum++)
	if (istool && viewopts[optnum].v_usage & TOOL
		|| !istool && viewopts[optnum].v_usage & TEXT)
	    total_opts++;
    */
    total_opts = sizeof viewopts / sizeof (struct viewopts);
    qsort((char *)viewopts, sizeof viewopts / sizeof (struct viewopts),
		    sizeof(struct viewopts), opt_sorter);
}

#ifdef SUNTOOL

static int start_cnt;

#define twenty 	5 + 20*l_width(DEFAULT)
#define forty 	5 + 40*l_width(DEFAULT)
#define image_at(x,y,image) pw_rop(msg_win, x, y, 16, 16, PIX_SRC, image, 0,0)

/* print in default text, but increment in large text segments */
view_options()
{
    if (msg_rect.r_height < 80) {
	print("Window not big enough to display options.");
	return;
    }
    do_clear();
    getting_opts = 1, start_cnt = 0;
    win_setcursor(msg_sw->ts_windowfd, &checkmark);
    highlight(msg_win, txt.x, txt.y, LARGE,
	    "    : Toggle Value       : Description       : Menu (Help)");
    image_at(txt.x +  2 * l_width(DEFAULT), txt.y - 12, &mouse_left);
    image_at(txt.x + 25 * l_width(DEFAULT), txt.y - 12, &mouse_middle);
    image_at(txt.x + 48 * l_width(DEFAULT), txt.y - 12, &mouse_right);

    pw_vector(msg_win, 0, txt.y+6, msg_rect.r_width, txt.y+6, PIX_SRC, 1);
    pw_vector(msg_win, 0, txt.y+8, msg_rect.r_width, txt.y+8, PIX_SRC, 1);

    txt.y += 24;

    pw_text(msg_win, 5,      txt.y, PIX_SRC, fonts[LARGE], "Option");
    pw_text(msg_win, twenty, txt.y, PIX_SRC, fonts[LARGE], "On/Off");
    pw_text(msg_win, forty,  txt.y, PIX_SRC, fonts[LARGE], "Values");

    pw_vector(msg_win, 0, txt.y+6, msg_rect.r_width, txt.y+6, PIX_SRC, 1);
    pw_vector(msg_win, 0, txt.y+8, msg_rect.r_width, txt.y+8, PIX_SRC, 1);

    pw_text(msg_win, 59*l_width(DEFAULT),txt.y,PIX_SRC,fonts[LARGE],"Scroll:");
    pw_rop(msg_win, 60*l_width(LARGE), txt.y-13,16,16,PIX_SRC, &dn_arrow,0,0);
    pw_rop(msg_win, 60*l_width(LARGE)+20,txt.y-13,16,16,PIX_SRC, &up_arrow,0,0);

    display_opts(0); /* create the pixrect and all that */
}

display_opts(count)
register int count;
{
    register int total_displayable = (msg_rect.r_height - 60) / 20;

    if (count < 0 && start_cnt + count < 0) {
	print("At the beginning");
	return;
    } else if (count && start_cnt + count + total_displayable > total_opts) {
	print("At the end");
	return;
    }
    start_cnt += count;
    if (!msg_pix) {
	register int x = (total_opts+1) * 20;
	if (x < msg_rect.r_height)
	    x = msg_rect.r_height;
	if (!(msg_pix = mem_create(msg_rect.r_width, x, 1))) {
	    error("mem_create");
	    return;
	}
	pr_rop(msg_pix,0,0, msg_rect.r_width-1, x-1, PIX_CLR,0,0,0);
	for (count = 0; count < total_opts; count++)
	    option_line(count);
    }
    pw_rop(msg_win, 0, 50, msg_rect.r_width - 1, msg_rect.r_height - 50,
       PIX_SRC, msg_pix, 0, start_cnt * 20);
}

void
toggle_opt(line)
{
    register char *p = viewopts[start_cnt+line].v_prompt;

    if (do_set(set_options, viewopts[start_cnt+line].v_opt))
	un_set(&set_options, viewopts[start_cnt+line].v_opt);
    else {
	if (p) {
	    txt.x = 5 + 40 * l_width(DEFAULT) +
		    (1 + strlen(p) - (*p=='-')) * l_width(DEFAULT);
	    txt.y = 50 + line*20 + l_height(curfont);
	}
	if (!p || *p == '-') {
	    register char *argv[2];
	    argv[0] = viewopts[start_cnt+line].v_opt;
	    argv[1] = NULL;
	    (void) add_option(&set_options, argv);
	}
    }
    option_line(line);
    display_opts(0);
    if (txt.x > 5)
	pw_char(msg_win, txt.x, txt.y, PIX_SRC, fonts[DEFAULT], '_');
}

void
help_opt(line)
{
    print(viewopts[start_cnt+line].v_description);
}

add_opt(p, line)
register char *p;
{
    char buf[80], **argv;
    int argc, save_bang = ison(glob_flags, IGN_BANG);

    (void) sprintf(buf, "set %s=\"%s\"", viewopts[start_cnt+line].v_opt, p);
    turnon(glob_flags, IGN_BANG);
    if (argv = make_command(buf, DUBL_NULL, &argc))
	(void) do_command(argc, argv, msg_list);
    if (!save_bang)
	turnoff(glob_flags, IGN_BANG);
    option_line(line); /* make sure new value is entered into database */
}

option_line(count)
register int count;
{
    register char *p, *v = do_set(set_options, viewopts[start_cnt+count].v_opt);
    struct pr_prpos win;

    win.pr = msg_pix;
    win.pos.y = (start_cnt + count) * 20 + 16;
    win.pos.x = 5;

    pf_text(win, PIX_SRC, fonts[DEFAULT], blank);
    pf_text(win, PIX_SRC, fonts[DEFAULT], viewopts[start_cnt+count].v_opt);
    win.pos.x = twenty+20;

    if (!(p = viewopts[start_cnt+count].v_prompt) || *p == '-') {
	pr_rop(msg_pix, twenty, win.pos.y-10, 16, 16, PIX_SRC, &cycle, 0, 0);
	pf_text(win, PIX_SRC, fonts[DEFAULT], (v)? "TRUE  ": "FALSE");
	win.pos.x++;
	pf_text(win, PIX_SRC, fonts[DEFAULT], (v)? "TRUE  ": "FALSE");
    }
    if (p) {
	if (*p == '-')
	    p++;
	win.pos.x = forty;
	/* heighlight */
	pf_text(win, PIX_SRC, fonts[DEFAULT], p);
	win.pos.x++;
	pf_text(win, PIX_SRC, fonts[DEFAULT], p);
        win.pos.x = forty + strlen(p) * l_width(DEFAULT);
	pf_text(win, PIX_SRC, fonts[DEFAULT], ":");
	if (v) {
	    win.pos.x += (2 * l_width(DEFAULT));
	    pf_text(win, PIX_SRC, fonts[DEFAULT], v);
	}
    }
}

#endif SUNTOOL

/*
 * return a string describing a variable.
 * parameters: count, str, buf.
 * If str != NULL, check str against ALL variables
 * in viewopts array.  The one that matches, set count to it and 
 * print up all the stuff from the viewopts[count] into the buffer
 * space in "buf" and return it.
 */
char *
variable_stuff(count, str, buf)
register char *str, buf[];
{
    if (str)
	for (count = 0; count < total_opts; count++)
	    if (!strcmp(str, viewopts[count].v_opt))
		break;
    if (count >= total_opts) {
	(void) sprintf(buf, "%s: Not a default %s variable.",
			   str? str : itoa(count), prog_name);
	return NULL;
    }
    return sprintf(buf, "%12.12s: %s",
	viewopts[count].v_opt, viewopts[count].v_description);
}
