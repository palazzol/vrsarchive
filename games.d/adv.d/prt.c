/*
 * Print string s in an orderly fashion.  (Provide linebreaks
 * where necessary.)
 */

#include "adv.h"

#ifndef CURSED
int (*nl_proc)();		/* if not null, called before printing \n */

#endif
static char pbuf[256];
static int pp;

prt(s)
	register char *s;
{
	if (s == 0) {
		pflush();
		return;
	}
	while (*s)
		if (*s == '\n') {
			pflush();
#ifndef CURSED
			if (nl_proc)
				(*nl_proc)();
#else
			/*
			 * Hack by VRS to avoid using curses scrolling.
			 * Curses refreshes every scroll, so on terminals
			 * without il/dl scrolling is very slow.  Worse,
			 * most sites run versions of curses that ignore
			 * il/dl.  This hack relies on deleteln() not
			 * implying a refresh.  Thus the whole window
			 * still gets repainted, but only once.
			*/
			{	int x, y;
				getyx(CurWin, y, x);
				/*
				 * There is no approved way to do this test.
				 * We are forced to rely on implementation
				 * detail at the wrong level of abstraction,
				 * instead of "if (y+1 > wlines(CurWin)) {"
				 * Hope all curses implementations do this
				 * the same way!
				*/
				if (y+1 == CurWin->_maxy) {
					wmove(CurWin, 0, 0);
					wdeleteln(CurWin);
					wmove(CurWin, y-1, x);
				}
			}
#endif
			Wputc(*s++, CurWin);
		}
		else
			pput(*s++);
}

/*
 * Put character into buffer up to blank; then flush buffer
 * If current character would "spill over", go to next line.
 */
pput(c)
	register int c;
{
	register char *p;
#ifdef CURSED
	int	y, x;
#endif

	pbuf[pp++] = c;
#ifndef CURSED
	if (CurWin->w_cursor.col + pp >= COLS) {
#else
	getyx(CurWin, y, x);
	if (x + pp >= COLS) {
#endif
		p = &pbuf[pp];
		*p = 0;
		while (--p >= pbuf)
			if (*p == ' ')
				break;
		if (p < pbuf)
			p = &pbuf[pp];
		c = *p;
		*p = 0;
		Wputs(pbuf, CurWin);
		*p = c;
#ifndef CURSED
		if (CurWin->w_cursor.col) {
			if (nl_proc)
				(*nl_proc)();
#else
		getyx(CurWin, y, x);
		if (x) {
#endif
			Wputc('\n', CurWin);
		}
		while (*p == ' ')
			p++;
		if (*p) {
			strcpy(pbuf, p);
			pp = strlen(pbuf);
		}
		else
			pp = 0;
	}
}

/*
 * Write the buffered chars to the window
 */
pflush()
{

	pbuf[pp] = 0;
	Wputs(pbuf, CurWin);
	pp = 0;
}
