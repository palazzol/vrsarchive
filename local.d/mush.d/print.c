/* @(#)print.c	2.4	(c) copyright 10/15/86 (Dan Heller) */

#include <varargs.h>
#include "mush.h"

/*ARGSUSED*/
/*VARARGS1*/
void
error(fmt, arg1, arg2, arg3, arg4)
register char *fmt;
char *arg1, *arg2, *arg3, *arg4;
{
    print(fmt, arg1, arg2, arg3, arg4);
    print_more(": %s\n", sys_errlist[errno]);
}

#if defined(SUNTOOL) || defined(CURSES)
/*
 * print just like printf -- to a window, to curses, or to stdout.  Use vprintf
 * if available.  msgbuf is the buffer used to print into if necessary.
 * If you're running SUN3.2 or higher, the typecast (unsigned char *)msgbuf
 * (where indicated) otherwise, msgbuf is not typecast at all.
 * Also note same casting in wprint().
 */
/*VARARGS1*/
void
print(va_alist)
va_dcl
{
    static char msgbuf[BUFSIZ];
#ifndef VPRINTF
    FILE foo;
#endif VPRINTF
    register char *fmt;
    va_list args;
#ifdef SUNTOOL
    static int x;
#endif SUNTOOL
    char *p; /* same type as struct file _ptr,_buf in stdio.h */

    va_start(args);
    fmt = va_arg(args, char *);
#ifdef CURSES
    if (iscurses) {
	if (isoff(glob_flags, CONT_PRNT))
	    move(LINES-1, 0), refresh();
	turnoff(glob_flags, CONT_PRNT);
    } else
#endif CURSES
	if (istool < 2) {
#ifdef VPRINTF
	    vprintf(fmt, args), fflush(stdout);
#else
	    _doprnt(fmt, args, stdout), fflush(stdout);
#endif VPRINTF
	    va_end(args);
	    return;
	}
#ifdef VPRINTF
    if (fmt)
	vsprintf(msgbuf, fmt, &args); /* NULL in fmt reprints last msg */
#else VPRINTF
    foo._cnt = BUFSIZ;
    foo._base = foo._ptr = msgbuf; /* may have to be cast(unsigned char *) */
    foo._flag = _IOWRT+_IOSTRG;
    if (fmt) {   /* passing NULL (not "") reprints last message */
	(void) _doprnt(fmt, args, &foo);
	*foo._ptr = '\0'; /* plant terminating null character */
    }
#endif VPIRNTF
    va_end(args);
    p = msgbuf;
    if (iscurses || istool)
	while (p = index(p, '\n'))
	    *p = ' ';
#ifdef CURSES
    if (iscurses) {
	addstr(msgbuf), clrtoeol(), refresh();
	return;
    }
#endif CURSES
#ifdef SUNTOOL
    if (isoff(glob_flags, CONT_PRNT))
	x = 5;
    turnoff(glob_flags, CONT_PRNT);
    pw_text(print_win, x,   l_height(LARGE), PIX_SRC, fonts[LARGE], msgbuf);
    pw_text(print_win, x+1, l_height(LARGE), PIX_SRC|PIX_DST,
		       fonts[LARGE], msgbuf);
    x += strlen(msgbuf) * l_width(LARGE);
    Clrtoeol(print_win, x, l_height(LARGE), LARGE);
#endif SUNTOOL
}
#endif SUNTOOL || CURSES

#ifdef SUNTOOL
/*VARARGS*/
void
wprint(fmt, va_alist)
char *fmt;
va_dcl
{
#ifndef VPRINTF
    FILE foo;
    va_list args;
#endif VPRINTF
    char msgbuf[BUFSIZ]; /* we're not getting huge strings */

    if (istool < 2) {
#ifdef VPRINTF
	vprintf(fmt, &args);
#else VPRINTF
	va_start(args);
	_doprnt(fmt, args, stdout);
	va_end(args);
#endif VPRINTF
	fflush(stdout);
	return;
    }
    if (!fmt)
	return;
#ifdef VPRINTF
    vsprintf(msgbuf, fmt, &args); /* NULL in fmt reprints last msg */
#else VPRINTF
    foo._cnt = BUFSIZ;
    foo._base = foo._ptr = msgbuf; /* may have to typecast (unsigned char *) */
    foo._flag = _IOWRT+_IOSTRG;A
    va_start(args);
    _doprnt(fmt, args, &foo); /* format like printf into msgbuf via foo */
    va_end(args);
    *foo._ptr = '\0'; /* plant terminating null character */
#endif VPRINTF
    Addstr(msgbuf);  /* addstr() will scroll if necessary */
}

/*
 * scroll the msg_win "lines"
 * if `lines' is negative (backwards scroll) msg_pix can't be NULL
 */
void
scroll_win(lines)
register int lines;
{
    register int y = lines * l_height(curfont);
    if (txt.y + y < msg_rect.r_height)
	y = 0;  /* temporary */
    txt.x = 5;

    if (msg_pix) {
	if (txt.y + y >= msg_pix->pr_size.y - 5)
	    y = msg_pix->pr_size.y - txt.y;
	still_more += y; /* let scrolling know where we are */
	txt.y += y;
	pw_rop(msg_win, 0, 5,
	       msg_rect.r_width, crt * l_height(curfont),
	       PIX_SRC, msg_pix, 0, txt.y - msg_rect.r_height + 3);
	tool_more(NULL);
	return;
    }
    /* y must be positive (forward scrolling) so we're scrolling typed
     * text or something like that (~p, ~?, etc...)
     */
    pw_copy(msg_win, 0, 0,
	msg_rect.r_width, msg_rect.r_height - y,
	PIX_SRC, msg_win, 0, y);
    pw_writebackground(msg_win, 0, msg_rect.r_height - y,
	msg_rect.r_width, y, PIX_CLR);
    txt.y -= y;
}
#endif SUNTOOL

#ifdef CURSES
clr_bot_line()
{
    print("");
}
#endif CURSES
