#include "adv.h"
#include <ctype.h>

/*
 * Get input line; break into words;
 * If 'verb' matches objects 1-6, make 'verb' "GO" and 'object'
 * 'verb'; else match verb/object.
 */

char	*wrdbrk();

parse()
{
	register char *vp, *op;

	for (;;) {
		vp = wrdbrk(0);
		op = wrdbrk(1);
#ifdef DEBUG
		prt("verb: %s; obj: %s\n", vp, op);
#endif
		/*
		 * Handle "save game", "load game", and "quit".
		 */
		if (strcmp(vp, scom) == 0 && strcmp(op, gobj) == 0) {
			save();
			continue;
		}
		if (strcmp(vp, lcom) == 0 && strcmp(op, gobj) == 0) {
			load();
			continue;
		}
		if (strcmp(vp, qcom) == 0) {
			isend = TRUE;
			return;
		}

		/*
		 * Convert verb & object strings to numeric equivalents.
		 */
		if (*op)
			objv = wrdval(nobjs, objs, op);
		else {
			objv = wrdval(nobjs, objs, vp);
			if (objv > 0 && objv < 7) {
				verbv = 1;/* Special hack: "go" dir */
				return;
			}
			objv = 0;
		}
		verbv = wrdval(nverbs, verbs, vp);
		if (verbv == -1 || objv == -1)
			prt("You use word(s) I don't know.\n");
		else
			return;
	}
}

/*
 * Find a word in a wordlist
 */
wrdval(n, wl, s)
	register int n;
	register struct word *wl;
	register char *s;
{

	while (--n > 0) {
		wl++;
		if (strcmp(wl->word, s) == 0)
			return (wl->val);
	}
	return (-1);
}

/*
 * Break off a word.  Obj is true if we can ignore the lack of a word
 * (i.e., we're looking for an object).  Words come in pairs, unless
 * separated by periods.
 */
char *wrdbrk(obj)
	int obj;
{
	register char *cp;
	char *st, *en;
	int lpcount = 0;
	static char *lp = "";
	static char inbuf[LINELEN+1];
	static BOOL per = FALSE;

	/*
	 * If we saw a period (or the end of an input line), and we're
	 * looking for an object, return the null string.
	 * In any case clear the period/end-of-line flag.
	 */
	if (per) {
		per = FALSE;
		if (obj)
			return ("");
	}

	/*
	 * Start searching from previous stop point.
	 */
	cp = lp;
	for (;;) {
		/*
		 * Find the beginning of the next word.  If there is
		 * a word, take it, and note whether it is followed by
		 * a period (or end of line).
		 */
		skpwht(cp);
		if (*cp) {
			st = cp;
			while (*cp && !isspace(*cp) && *cp != '.')
				cp++;
			en = cp;
			if (en - st > wrdsiz)
				en = st + wrdsiz;
			skpwht(cp);
			if (*cp == '.') {
				cp++;
				per = TRUE;
				skpwht(cp);
			}
			if (*cp == 0)
				per = TRUE;
			lp = cp;
			*en = 0;
			return (st);
		}

		/*
		 * Read an input line from the user.  Convert to
		 * uppercase.  (Ok, so it oughta use lowercase....)
		 */
		disply();
		prt("Tell me what to do: ");
		prt(0);
		wgets(inbuf, sizeof inbuf);
		cp = inbuf;
		while (*cp) {
			if (islower(*cp))
				*cp = toupper(*cp);
			cp++;
		}
		cp = inbuf;
	}
}

/*
 * Let the user type in a string, but only choice characters
 * (and at most bs-1 of them).
 */
wgets(bp, bs)
	register char *bp;
	register int bs;
{
	register int nch = 0, c;
#define Ctl(c) ((c) & 0x1f)

	/*
	 * What is this doing here?
	 */
#ifndef CURSED
	if (COLS - CurWin->w_cursor.col > bs) {
		if (COLS - CurWin->w_cursor.col < 4)
			Wputc('\n', CurWin);
		if (COLS - CurWin->w_cursor.col > bs)
			bs = COLS - CurWin->w_cursor.col;
#else
	{
		int	y, x;
		getyx(CurWin, y, x);

		if (COLS - x > bs) {
			if (COLS - x < 4) {
				waddch(CurWin, '\n');
				x = 0;
			}
			if (COLS - x > bs)
				bs = COLS - x;
		}
#endif
	}

	/*
	 * Should probably use user's editing chars, but for now...
	 */
	refresh();
	ReadingTerminal = 2;
	while ((c = getchar()) != '\n' &&c != '\r') {
		ReadingTerminal = 0;
		if (c == '\b' || c == 0177) {
			if (nch) {
				--nch;
				Wputs("\b \b", CurWin);
			}
		}
		else if (c == Ctl('u') || c == Ctl('x')) {
			while (--nch >= 0)
				Wputs("\b \b", CurWin);
			nch = 0;
		}
		else if (c == Ctl('w')) {
			while (--nch >= 0 && bp[nch] == ' ')
				Wputs("\b \b", CurWin);
			nch++;
			while (--nch >= 0 && bp[nch] != ' ')
				Wputs("\b \b", CurWin);
			nch++;
		}
		else if (c == Ctl('l'))	/* redraw screen */
#ifndef CURSED
			ScreenGarbaged++;
#else
			wrefresh(curscr);
#endif
		else if (c >= ' ' && c < 0177) {
			bp[nch++] =c;
			if (nch > bs)
				--nch;
			else
				Wputc(c, CurWin);
		}
		wrefresh(CurWin);
		ReadingTerminal = 2;
	}
	ReadingTerminal = 0;
	bp[nch] = 0;
	Wputc('\n', CurWin);
#undef Ctl
}
