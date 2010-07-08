/*
** msg.c -	code dealing with the printing of messages
**
**	[pm by Peter Costantinidis, Jr. @ University of California at Davis]
*/
#include <signal.h>
#include "pm.h"
#ifdef CYGWIN
#include <stdarg.h>
#else
#include <varargs.h>
#endif

/*
** strucpy()	- copy string using punctrl for things
**		- ctrl chars count double
*/
void	strucpy (s1, s2, len)
reg	char	*s1, *s2;
reg	int	len;
{
	reg	char	*sp;

	while (len-- && *s2)
	{
		if ((*s2 < ' ') && !len--) /* if len = 0, then no room */
			return;
		strcpy(s1, sp = punctrl(*s2++));
		s1 += strlen(sp);
	}
	*s1 = '\0';
}

/*
** msg:
**	Display a message on the screen.
*/
static char msgbuf[BUFSIZ];

#ifdef CYGWIN
/*VARARGS*/
void msg(char *fmt, ...)
#else
/*VARARGS*/
void msg(va_alist)
va_dcl
#endif
{
	va_list ap;
#ifdef CYGWIN
	va_start(ap, fmt);
#else
	char *fmt;

	va_start(ap);
	fmt = va_arg(ap, char *);
#endif
	alarm(0);
	/*
	 * if the string is "", just clear the line
	 */
	if (*fmt == '\0') {
		move(5, 55);
		clrtoeol();
		return;
	}
	/*
	 * otherwise printto the message and flush it out
	 */
	vsprintf(msgbuf, fmt, ap);
	move(5, 55);
	addstr(msgbuf);
	clrtoeol();
	refresh();
	/*
	** set off an alarm to erase it
	*/
#ifndef	LINT
	signal(SIGALRM, msg_erase);
#endif
	alarm(ALARM_TIME);
}

/*
** msg_erase()	- erase the msg line
*/
void	msg_erase ()
{
	alarm(0);
	move(5, 55);
	addstr("                       ");
}

/*
** re_msg()	- reprint the last message
*/
void	re_msg ()
{
	msg(msgbuf);
}

/*
** punctrl()	- print a readable version of a certain character
**
**	Note:	Due to the inconsistent availability of a function to perform
**		this, my own version has been built in and used in place of
**		any pre-existing function.  I believe that this particular
**		version suts down on data space considerably from the versions
**		I have found on the Berkley systems.
*/
char	*punctrl (chr)
char	chr;
{
	static	char	*str = "^ ";

	chr &= 0177;
	if (chr >= ' ' && chr <= '~')
	{
		static	char	*str1 = " ";

		*str1 = chr;
		return(str1);
	}
	if (chr == MYCTRL('?'))
		return("^?");
	*(str+1) = chr + '@';
	return(str);
}
