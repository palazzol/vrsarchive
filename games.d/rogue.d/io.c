/*
 * Various input/output functions
 *
 * @(#)io.c	6.4 (Berkeley) 10/3/84
 */

#include "curses.h"
#include <ctype.h>
#include "rogue.h"
#include <varargs.h>

/*
 * msg:
 *	Display a message at the top of the screen.
 */

static char msgbuf[BUFSIZ];
static int newpos = 0;

/*VARARGS1*/
msg(va_alist)
va_dcl
{
    va_list ap;
    char *fmt;

    va_start(ap);
    fmt = va_arg(ap, char *);
    /*
     * if the string is "", just clear the line
     */
    if (*fmt == '\0')
    {
	move(0, 0);
	clrtoeol();
	mpos = 0;
	return;
    }
    /*
     * otherwise add to the message and flush it out
     */
    vsprintf(msgbuf+newpos, fmt, ap);
    newpos = strlen(msgbuf);
    endmsg();
}

/*
 * add things to the current message
 */
/*VARARGS*/
void
addmsg(va_alist)
va_dcl
{
	va_list ap;
	char *fmt;

	va_start(ap);
	fmt = va_arg(ap, char *);
	vsprintf(msgbuf+newpos, fmt, ap);
	newpos = strlen(msgbuf);
}

/*
 * Display a new msg (giving him a chance to see the previous one if it
 * is up there with the --More--)
 */
endmsg()
{
    if (msgbuf[1] != ')' && *msgbuf >= 'a' && *msgbuf <= 'z')
	*msgbuf -= 'a' - 'A';
    strcpy(huh, msgbuf);
    if (mpos)
    {
	move(0, mpos);
	addstr("--More--");
	wrefresh(stdscr);
	wait_for(' ');
    }
    mvaddstr(0, 0, msgbuf);
    clrtoeol();
    mpos = newpos;
    newpos = 0;
    wrefresh(stdscr);
}

/*
 * step_ok:
 *	returns true if it is ok to step on ch
 */
step_ok(ch)
{
    switch (ch)
    {
	case ' ':
	case '|':
	case '-':
	    return FALSE;
	default:
	    return (!isalpha(ch));
    }
}

/*
 * readchar:
 *	flushes stdout so that screen is up to date and then returns
 *	getchar.
 */

readchar()
{
    char c;

    fflush(stdout);
    while (read(0, &c, 1) < 0)
	continue;
    return c;
}

/*
 * unctrl:
 *	Print a readable version of a certain character
 */
#ifndef __STDC__
#ifndef unctrl
char *
unctrl(ch)
char ch;
{ static char s[] = {
    '^', '\0', '\0'
  };

  s[1] = ch & 0177;
  if (ch >= ' ')
    return(s+1);
  s[1] += '@';
  return(s);
}
#endif
#endif

/*
 * status:
 *	Display the important stats line.  Keep the cursor where it was.
 */

status()
{
    register int oy, ox, temp;
    register char *pb;
    static char buf[80];
    static int hpwidth = 0, s_hungry = -1;
    static int s_lvl = -1, s_pur, s_hp = -1, s_str, s_ac = 0;
    static long s_exp = 0;

    /*
     * If nothing has changed since the last status, don't
     * bother.
     */
    if (s_hp == pstats.s_hpt && s_exp == pstats.s_exp && s_pur == purse
	&& s_ac == (cur_armor != NULL ? cur_armor->o_ac : pstats.s_arm)
	&& s_str == pstats.s_str
	&& s_lvl == level && s_hungry == hungry_state)
	    return;
	
    getyx(curscr, oy, ox);
    if (s_hp != max_hp)
    {
	temp = s_hp = max_hp;
	for (hpwidth = 0; temp; hpwidth++)
	    temp /= 10;
    }
    sprintf(buf, "Level: %d  Gold: %-5d  Hp: %*d(%*d)  Str: %-2d(%2d)",
	level, purse, hpwidth, pstats.s_hpt, hpwidth, max_hp,
	pstats.s_str, max_stats.s_str);
    pb = &buf[strlen(buf)];
    sprintf(pb, "  Ac: %-2d  Exp: %d/%ld",
	cur_armor != NULL ? cur_armor->o_ac : pstats.s_arm, pstats.s_lvl,
	pstats.s_exp);
    /*
     * Save old status
     */
    s_lvl = level;
    s_pur = purse;
    s_hp = pstats.s_hpt;
    s_str = pstats.s_str;
    s_exp = pstats.s_exp; 
    s_ac = (cur_armor != NULL ? cur_armor->o_ac : pstats.s_arm);
    mvaddstr(LINES - 1, 0, buf);
    switch (hungry_state)
    {
	case 0: ;
	when 1:
	    addstr("  Hungry");
	when 2:
	    addstr("  Weak");
	when 3:
	    addstr("  Fainting");
    }
    clrtoeol();
    s_hungry = hungry_state;
    move(oy, ox);
}

/*
 * wait_for
 *	Sit around until the guy types the right key
 */

wait_for(ch)
register char ch;
{
    register char c;

    if (ch == '\n')
        while ((c = readchar()) != '\n' && c != '\r')
	    continue;
    else
        while (readchar() != ch)
	    continue;
}

/*
 * show_win:
 *	function used to display a window and wait before returning
 */

show_win(scr, message)
register WINDOW *scr;
char *message;
{
    mvwaddstr(scr, 0, 0, message);
    touchwin(scr);
    wmove(scr, hero.y, hero.x);
    wrefresh(scr);
    wait_for(' ');
    clearok(stdscr, TRUE);
    touchwin(stdscr);
}
