#include "adv.h"

#ifndef CURSED
extern int (*nl_proc)();	/* called by prt before writing newline */

static int dlleft;		/* display lines left */
static int fullsize = TRUE;	/* true => top window is full screen */

/*
 * Before prt dumps a newline into the top window, prt calls this
 * routine.  We make sure that there is enough room in the top
 * window, enlarging it if necessary.
 */
dl_newline()
{

	if (fullsize)		/* already as big as possible */
		return;
	if (--dlleft > 0)	/* top window still has room left */
		return;
	Wsize(TopWin, COLS, ROWS);
	fullsize = TRUE;
}
#endif CURSED

/*
 * Display the current room in the top window.
 */
disply()
{
	register int temp, i;
	register struct room *rp;
	register struct item *ip;
	register char *s;
#ifndef CURSED
	static int f1, f2, f3, f4, f5, f6, f7, f8;
	int (*oldnlproc)();

	/*
	 * Fetch the window frame characters if we haven't got them
	 * yet.  (This needs to be done another way; someday I'll
	 * finish rewriting windows.)
	 */
	if (f1 == 0)
		Wgetframe(&f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8);
#endif CURSED

	/*
	 * Flush pending line (if any).  Switch output to top window,
	 * and clear it.
	 */
	prt(0);			/* Flush pending output */
	CurWin = TopWin;	/* prt() sends to CurWin */
	Wclear(TopWin, 2);
	WAcursor(TopWin, 0, 0);

#ifndef CURSED
	/*
	 * We now have TopWin->IYE lines available to us for printing
	 * description text.  Set the newline procedure so that we can
	 * enlarge the top window if necessary.
	 */
	dlleft = TopWin->IYE;
	oldnlproc = nl_proc;
	nl_proc = dl_newline;
#endif CURSED

	/*
	 * If it's dark, just say so.  Otherwise find all the items
	 * in the room and display their descriptions.
	 */
	if (isdark) {
		prt("I can't see, it's too dark.\n");
		goto cleanup;
	}
	rp = &rooms[rm];
	s = rp->desc;
	if (*s == '*')
		s++;
	else
		prt("I'm in a ");
	prt(s);
	prt(".\n");
	temp = 0;
	for (ip = &items[1]; ip < iteme; ip++) {
		if (ip->rm == rm) {
			if (temp == 0) {
				temp++;
				prt("\nVisible items here: ");
			}
			else
				prt(". ");
			prt(0);		/* fix w_cursor */
#ifndef CURSED
			if (TopWin->w_cursor.col + strlen(ip->desc) > COLS-3)
				prt("\n");
#else CURSED
			{
				int	y, x;
				getyx(TopWin, y, x);
				if (x + strlen(ip->desc) > COLS-3)
					prt("\n");
			}
#endif CURSED
			prt(ip->desc);
		}
	}
	if (temp)
		prt(".\n");

	/*
	 * Print the "obvious" exits.
	 */
	temp = 0;
	for (i = 0; i < 6; i++) {
		if (rp->dirs[i]) {
			if (temp == 0) {
				temp++;
				prt("\nObvious exits: ");
			}
			prt(dirs[i]);
			prt(" ");
		}
	}
	if (temp)
		prt("\n");

cleanup:
#ifndef CURSED
	/*
	 * Put up a nice bottom border, adjusting the window size to
	 * the number of lines used.  Set the margins so that the
	 * bottom border shows, but the rest is re-"glassed".
	 */
	temp = TopWin->w_cursor.row + 1;
	if (temp < 3)			/* this is a kludge */
		temp = 3;
	if (temp != TopWin->OYE) {	/* avoid work if possible */
		Wsize(TopWin, COLS, temp);
		Wborder(TopWin, '#', '#', '#', '#', '#', f7, f7, f7);
		Wsetmargins(TopWin, 0, 0, COLS, TopWin->OYE - 1);
		fullsize = FALSE;
	}
#endif CURSED

	/*
	 * Switch output back to the base window.
	 */
	CurWin = BaseWin;
#ifndef CURSED
	nl_proc = oldnlproc;		/* restore */
#endif CURSED
}
