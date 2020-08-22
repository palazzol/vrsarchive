/* rite.c	(c) copyright 1986 (Dan Heller) */

#include "mush.h"

#define LASTLINE 	  (msg_rect.r_height - l_height(LARGE)-5)

static char *_unctrl[] = {
    "^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "^H", "^I", "^J", "^K",
    "^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S", "^T", "^U", "^V", "^W",
    "^X", "^Y", "^Z", "^[", "^\\", "^]", "^~", "^_",
    " ", "!", "\"", "#", "$",  "%", "&", "'", "(", ")", "*", "+", ",", "-",
    ".", "/", "0",  "1", "2",  "3", "4", "5", "6", "7", "8", "9", ":", ";",
    "<", "=", ">",  "?", "@",  "A", "B", "C", "D", "E", "F", "G", "H", "I",
    "J", "K", "L",  "M", "N",  "O", "P", "Q", "R", "S", "T", "U", "V", "W",
    "X", "Y", "Z",  "[", "\\", "]", "^", "_", "`", "a", "b", "c", "d", "e",
    "f", "g", "h",  "i", "j",  "k", "l", "m", "n", "o", "p", "q", "r", "s",
    "t", "u", "v",  "w", "x",  "y", "z", "{", "|", "}", "~", "^?"
};

static char String[BUFSIZ];
static int count, backchars, save_orig_x;

char *
rite(c)
register char c;
{
    static int literal_next;

    if (c == ltchars.t_lnextc || literal_next) {
	if (literal_next)
	    Addch(c);
	else /* don't call Addch to prevent cursor from advancing */
	    pw_char(msg_win, txt.x, txt.y, PIX_SRC, fonts[curfont], '^');
	literal_next = !literal_next;
	return NULL;
    }
    literal_next = 0;
    if (c == _tty.sg_erase) {
	if (count)
	    backspace(), String[count--] = 0;
    } else if (c == _tty.sg_kill) {
	if (count) {
	    Clrtoeol(msg_win, txt.x = save_orig_x, txt.y,curfont);
	    String[count=0] = 0;
	}
    } else if (c == ltchars.t_werasc)
	while (count) {
	    backspace();
	    String[count--] = 0;
	    if (!count || (String[count-1]==' ' && !isspace(String[count])))
		break;
	}
    else if (c == '\n' || c == '\r' || c == 13) {
	String[count] = 0;
	if ((txt.y += l_height(curfont)) >= LASTLINE && !getting_opts)
	    scroll_win(1);
	/* else Clrtoeol(msg_win, txt.x, txt.y, curfont); */
	count = 0, txt.x = 5;
	return String;
    } else if (c == 12) {
	if (ison(glob_flags, IS_GETTING))
	    Addch(c);
	else do_clear();
    } else if (count == BUFSIZ-1)
	print("Text too long for String!"), count--;
    else if (c == '\t')
	do Addch(' ');
	while (count % 8 && count < BUFSIZ);
    else
	Addch(c);
    return NULL;
}

backspace()
{
    if (backchars) {
	pw_text(msg_win, save_orig_x, txt.y, PIX_SRC, fonts[curfont],
		&String[--backchars]);
	Clrtoeol(msg_win, msg_rect.r_width-10-l_width(curfont), txt.y, curfont);
    } else if ((txt.x -= l_width(curfont)) >= 5)
	pw_char(msg_win, txt.x, txt.y, PIX_SRC, fonts[curfont], ' ');
    else
	txt.x = 5;
}

static
Addch(c)
register char c;
{
    if (!count)
	save_orig_x = txt.x, bzero(String, BUFSIZ);
    if (c > 31 && c != 127)
	String[count++] = c;
    else {
	Addch('^');
	Addch(_unctrl[c][1]);
	return;
    }
    pw_char(msg_win, txt.x, txt.y, PIX_SRC, fonts[curfont], c);
    if ((txt.x += l_width(curfont)) <= msg_rect.r_width-5-l_width(curfont))
	return;
    if (getting_opts) {
	pw_text(msg_win, save_orig_x, txt.y, PIX_SRC, fonts[curfont],
		&String[++backchars]);
	txt.x -= l_width(curfont);
	pw_char(msg_win, txt.x, txt.y, PIX_SRC, fonts[curfont], ' ');
    } else {
	txt.x = 5;
	if ((txt.y += l_height(curfont)) >= LASTLINE)
	    scroll_win(1);
    }
}

Addstr(s)
register char *s;
{
    char buf[BUFSIZ];
    register int cnt = 0, max_len;
    register char *p = buf, newline = 0;

    max_len = (msg_rect.r_width - 10) / l_width(curfont) + 1;

    while ((*p = *s++) && *p != '\n' && cnt < max_len)
	if (*p == '\t')
	    do *p++ = ' ';
	    while (++cnt % 8);
	else p++, cnt++;
    *p = 0;

    if (*--s)
	newline = *s, *s = 0; /* newline may or may not be a '\n' */
    else
	s = 0;

    if (*buf) {
	if (msg_pix) {
	    struct pr_prpos pixr;
	    pixr.pr = msg_pix;
	    pixr.pos = txt;
	    pf_text(pixr, PIX_SRC, fonts[curfont], buf);
	} else
	    pw_text(msg_win, txt.x, txt.y, PIX_SRC, fonts[curfont], buf);
	txt.x += cnt * l_width(curfont);
    }
    if (newline) {
	if (newline != '\n')
	    *s = newline;
	if ((txt.y += l_height(curfont)) >= LASTLINE && !msg_pix)
	    scroll_win(1);
	txt.x = 5;
	if (newline == '\n' && !*++s)
	    return;
	Addstr(s);
    }
}

tool_more(p)
register char *p;
{
    int percent;
    /* we are typing -- scrool the window */
    if (!msg_pix) {
	scroll_win(1);
	return;
    }
    if (p)
	print(p);
    else {
	if ((percent = (still_more * 100) / msg_pix->pr_size.y) >= 100)
	    print( "--End of Message %d--", current_msg+1);
	else
	    print("--Message %d--(%d%%)", current_msg+1, percent);
	if (ison(glob_flags, IS_GETTING))
	    print_more(" ('q' returns to type-in mode)");
    }
}
