/*
 * Terminal control dependent routines
 * (Actually the graphing stuff is dependent on a 24x80 terminal, but...)
 *
 * This version is for termcaps under UNIX 4.2 bsd.
 *
 * The upper-left corner is labeled 0,0.
 */
#include <curses.h>

Tinit()			/* initialize */
{
  initscr();
}

Tterminate()		/* terminate */
{
  clear();
  refresh();
  endwin();
}

Tcls()			/* clear entire screen */
{
  clear();
}

Tstandout()		/* start writing bold/reverse video characters */
{
  standout();
}

Tstandend()		/* end of previous mode */
{
  standend();
}

Tmoveto(x,y)		/* move to position x,y */
int x,y;
{
  move(y,x);
}

Tprintf(s,d,f)		/* printf onto the screen */
char *s;
int d;
float f;
{
  printw(s,d,f);
}

Tcenter(s,y)		/* center the string on a given line */
char *s;
int y;
{
  Tmoveto( (80-strlen(s))/2, y );
  Tprintf(s);
}

Tpchar(c)		/* put one character where the cursor is */
char c;
{
  addch(c);
}

int Tgetonekey()	/* get one key from the keyboard and return */
{
  return( getchar() );
}

int Tkey_avail()	/* return true if a key has been pressed */
{ int i;
#ifdef FIONREAD
  ioctl(0, FIONREAD, &i);
#else
  i = rdchk(0);
#endif FIONREAD
  return( i > 0 );
}

Trefresh()		/* update the screen to reflect all changes */
{
  refresh();
}

Tcleos()		/* clear to the end (bottom) of the screen */
{
  clrtobot();
}
