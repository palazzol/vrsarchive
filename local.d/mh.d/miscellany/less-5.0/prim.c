/*
 * Primitives for displaying the file on the screen.
 */

#include "less.h"
#include "position.h"

public int hit_eof;	/* Keeps track of how many times we hit end of file */
public int screen_trashed;

static int squished;

extern int quiet;
extern int sigs;
extern int how_search;
extern int top_scroll;
extern int back_scroll;
extern int sc_width, sc_height;
extern int quit_at_eof;
extern int caseless;
extern int linenums;
extern int plusoption;
extern char *line;
extern char *first_cmd;
#if TAGS
extern int tagoption;
#endif

/*
 * Sound the bell to indicate he is trying to move past end of file.
 */
	static void
eof_bell()
{
	if (quiet == NOT_QUIET)
		bell();
	else
		vbell();
}

/*
 * Check to see if the end of file is currently "displayed".
 */
	static void
eof_check()
{
	POSITION pos;

	if (sigs)
		return;
	/*
	 * If the bottom line is empty, we are at EOF.
	 * If the bottom line ends at the file length,
	 * we must be just at EOF.
	 */
	pos = position(BOTTOM_PLUS_ONE);
	if (pos == NULL_POSITION || pos == ch_length())
		hit_eof++;
}

/*
 * If the screen is "squished", repaint it.
 * "Squished" means the first displayed line is not at the top
 * of the screen; this can happen when we display a short file
 * for the first time.
 */
	static void
squish_check()
{
	if (!squished)
		return;
	squished = 0;
	repaint();
}

/*
 * Display n lines, scrolling forward, 
 * starting at position pos in the input file.
 * "force" means display the n lines even if we hit end of file.
 * "only_last" means display only the last screenful if n > screen size.
 */
	static void
forw(n, pos, force, only_last)
	register int n;
	POSITION pos;
	int force;
	int only_last;
{
	int eof = 0;
	int nlines = 0;
	int do_repaint;
	static int first_time = 1;

	squish_check();

	/*
	 * do_repaint tells us not to display anything till the end, 
	 * then just repaint the entire screen.
	 */
	do_repaint = (only_last && n > sc_height-1);

	if (!do_repaint)
	{
		if (top_scroll && n >= sc_height - 1)
		{
			/*
			 * Start a new screen.
			 * {{ This is not really desirable if we happen
			 *    to hit eof in the middle of this screen,
			 *    but we don't yet know if that will happen. }}
			 */
			if (top_scroll == 2)
				clear();
			home();
			force = 1;
		} else
		{
			lower_left();
			clear_eol();
		}

		if (pos != position(BOTTOM_PLUS_ONE))
		{
			/*
			 * This is not contiguous with what is
			 * currently displayed.  Clear the screen image 
			 * (position table) and start a new screen.
			 */
			pos_clear();
			add_forw_pos(pos);
			force = 1;
			if (top_scroll)
			{
				if (top_scroll == 2)
					clear();
				home();
			} else if (!first_time)
			{
				putstr("...skipping...\n");
			}
		}
	}

	while (--n >= 0)
	{
		/*
		 * Read the next line of input.
		 */
		pos = forw_line(pos);
		if (pos == NULL_POSITION)
		{
			/*
			 * End of file: stop here unless the top line 
			 * is still empty, or "force" is true.
			 */
			eof = 1;
			if (!force && position(TOP) != NULL_POSITION)
				break;
			line = NULL;
		}
		/*
		 * Add the position of the next line to the position table.
		 * Display the current line on the screen.
		 */
		add_forw_pos(pos);
		nlines++;
		if (do_repaint)
			continue;
		/*
		 * If this is the first screen displayed and
		 * we hit an early EOF (i.e. before the requested
		 * number of lines), we "squish" the display down
		 * at the bottom of the screen.
		 * But don't do this if a + option or a -t option
		 * was given.  These options can cause us to
		 * start the display after the beginning of the file,
		 * and it is not appropriate to squish in that case.
		 */
		if (first_time && line == NULL && !top_scroll && 
#if TAGS
		    !tagoption &&
#endif
		    !plusoption)
		{
			squished = 1;
			continue;
		}
		if (top_scroll == 1)
			clear_eol();
		put_line();
	}

	if (eof && !sigs)
		hit_eof++;
	else
		eof_check();
	if (nlines == 0)
		eof_bell();
	else if (do_repaint)
		repaint();
	first_time = 0;
	(void) currline(BOTTOM);
}

/*
 * Display n lines, scrolling backward.
 */
	static void
back(n, pos, force, only_last)
	register int n;
	POSITION pos;
	int force;
	int only_last;
{
	int nlines = 0;
	int do_repaint;

	squish_check();
	do_repaint = (n > get_back_scroll() || (only_last && n > sc_height-1));
	hit_eof = 0;
	while (--n >= 0)
	{
		/*
		 * Get the previous line of input.
		 */
		pos = back_line(pos);
		if (pos == NULL_POSITION)
		{
			/*
			 * Beginning of file: stop here unless "force" is true.
			 */
			if (!force)
				break;
			line = NULL;
		}
		/*
		 * Add the position of the previous line to the position table.
		 * Display the line on the screen.
		 */
		add_back_pos(pos);
		nlines++;
		if (!do_repaint)
		{
			home();
			add_line();
			put_line();
		}
	}

	eof_check();
	if (nlines == 0)
		eof_bell();
	else if (do_repaint)
		repaint();
	(void) currline(BOTTOM);
}

/*
 * Display n more lines, forward.
 * Start just after the line currently displayed at the bottom of the screen.
 */
	public void
forward(n, only_last)
	int n;
	int only_last;
{
	POSITION pos;

	if (quit_at_eof && hit_eof)
	{
		/*
		 * If the -e flag is set and we're trying to go
		 * forward from end-of-file, go on to the next file.
		 */
		next_file(1);
		return;
	}

	pos = position(BOTTOM_PLUS_ONE);
	if (pos == NULL_POSITION)
	{
		eof_bell();
		hit_eof++;
		return;
	}
	forw(n, pos, 0, only_last);
}

/*
 * Display n more lines, backward.
 * Start just before the line currently displayed at the top of the screen.
 */
	public void
backward(n, only_last)
	int n;
	int only_last;
{
	POSITION pos;

	pos = position(TOP);
	if (pos == NULL_POSITION)
	{
		/* 
		 * This will almost never happen,
		 * because the top line is almost never empty. 
		 */
		eof_bell();
		return;   
	}
	back(n, pos, 0, only_last);
}

/*
 * Repaint the screen, starting from a specified position.
 */
	static void
prepaint(pos)	
	POSITION pos;
{
	hit_eof = 0;
	forw(sc_height-1, pos, 1, 0);
	screen_trashed = 0;
}

/*
 * Repaint the screen.
 */
	public void
repaint()
{
	/*
	 * Start at the line currently at the top of the screen
	 * and redisplay the screen.
	 */
	prepaint(position(TOP));
}

/*
 * Jump to the end of the file.
 * It is more convenient to paint the screen backward,
 * from the end of the file toward the beginning.
 */
	public void
jump_forw()
{
	POSITION pos;

	if (ch_end_seek())
	{
		error("Cannot seek to end of file");
		return;
	}
	lastmark();
	pos = ch_tell();
	clear();
	pos_clear();
	add_back_pos(pos);
	back(sc_height - 1, pos, 0, 0);
}

/*
 * Jump to line n in the file.
 */
	public void
jump_back(n)
	register int n;
{
	register int c;
	int nlines;

	/*
	 * This is done the slow way, by starting at the beginning
	 * of the file and counting newlines.
	 *
	 * {{ Now that we have line numbering (in linenum.c),
	 *    we could improve on this by starting at the
	 *    nearest known line rather than at the beginning. }}
	 */
	if (ch_seek((POSITION)0))
	{
		/* 
		 * Probably a pipe with beginning of file no longer buffered. 
		 * If he wants to go to line 1, we do the best we can, 
		 * by going to the first line which is still buffered.
		 */
		if (n <= 1 && ch_beg_seek() == 0)
			jump_loc(ch_tell());
		error("Cannot get to beginning of file");
		return;
	}

	/*
	 * Start counting lines.
	 */
	for (nlines = 1;  nlines < n;  nlines++)
	{
		while ((c = ch_forw_get()) != '\n')
			if (c == EOI)
			{
				char message[40];
				sprintf(message, "File has only %d lines", 
					nlines-1);
				error(message);
				return;
			}
	}

	jump_loc(ch_tell());
}

/*
 * Jump to a specified percentage into the file.
 * This is a poor compensation for not being able to
 * quickly jump to a specific line number.
 */
	public void
jump_percent(percent)
	int percent;
{
	POSITION pos, len;
	register int c;

	/*
	 * Determine the position in the file
	 * (the specified percentage of the file's length).
	 */
	if ((len = ch_length()) == NULL_POSITION)
	{
		error("Don't know length of file");
		return;
	}
	pos = (percent * len) / 100;

	/*
	 * Back up to the beginning of the line.
	 */
	if (ch_seek(pos) == 0)
	{
		while ((c = ch_back_get()) != '\n' && c != EOI)
			;
		if (c == '\n')
			(void) ch_forw_get();
		pos = ch_tell();
	}
	jump_loc(pos);
}

/*
 * Jump to a specified position in the file.
 */
	public void
jump_loc(pos)
	POSITION pos;
{
	register int nline;
	POSITION tpos;

	if ((nline = onscreen(pos)) >= 0)
	{
		/*
		 * The line is currently displayed.  
		 * Just scroll there.
		 */
		forw(nline, position(BOTTOM_PLUS_ONE), 1, 0);
		return;
	}

	/*
	 * Line is not on screen.
	 * Seek to the desired location.
	 */
	if (ch_seek(pos))
	{
		error("Cannot seek to that position");
		return;
	}

	/*
	 * See if the desired line is BEFORE the currently
	 * displayed screen.  If so, then move forward far
	 * enough so the line we're on will be at the bottom
	 * of the screen, in order to be able to call back()
	 * to make the screen scroll backwards & put the line
	 * at the top of the screen.
	 * {{ This seems inefficient, but it's not so bad,
	 *    since we can never move forward more than a
	 *    screenful before we stop to redraw the screen. }}
	 */
	tpos = position(TOP);
	if (tpos != NULL_POSITION && pos < tpos)
	{
		POSITION npos = pos;
		/*
		 * Note that we can't forw_line() past tpos here,
		 * so there should be no EOI at this stage.
		 */
		for (nline = 0;  npos < tpos && nline < sc_height - 1;  nline++)
			npos = forw_line(npos);

		if (npos < tpos)
		{
			/*
			 * More than a screenful back.
			 */
			lastmark();
			clear();
			pos_clear();
			add_back_pos(npos);
		}

		/*
		 * Note that back() will repaint() if nline > back_scroll.
		 */
		back(nline, npos, 1, 0);
		return;
	}
	/*
	 * Remember where we were; clear and paint the screen.
	 */
  	lastmark();
  	prepaint(pos);
}

/*
 * The table of marks.
 * A mark is simply a position in the file.
 */
#define	NMARKS		(27)		/* 26 for a-z plus one for quote */
#define	LASTMARK	(NMARKS-1)	/* For quote */
static POSITION marks[NMARKS];

/*
 * Initialize the mark table to show no marks are set.
 */
	public void
init_mark()
{
	int i;

	for (i = 0;  i < NMARKS;  i++)
		marks[i] = NULL_POSITION;
}

/*
 * See if a mark letter is valid (between a and z).
 */
	static int
badmark(c)
	int c;
{
	if (c < 'a' || c > 'z')
	{
		error("Choose a letter between 'a' and 'z'");
		return (1);
	}
	return (0);
}

/*
 * Set a mark.
 */
	public void
setmark(c)
	int c;
{
	if (badmark(c))
		return;
	marks[c-'a'] = position(TOP);
}

	public void
lastmark()
{
	marks[LASTMARK] = position(TOP);
}

/*
 * Go to a previously set mark.
 */
	public void
gomark(c)
	int c;
{
	POSITION pos;

	if (c == '\'')
		pos = marks[LASTMARK];
	else if (badmark(c))
		return;
	else 
		pos = marks[c-'a'];

	if (pos == NULL_POSITION)
		error("mark not set");
	else
		jump_loc(pos);
}

/*
 * Get the backwards scroll limit.
 * Must call this function instead of just using the value of
 * back_scroll, because the default case depends on sc_height and
 * top_scroll, as well as back_scroll.
 */
	public int
get_back_scroll()
{
	if (back_scroll >= 0)
		return (back_scroll);
	if (top_scroll)
		return (sc_height - 2);
	return (sc_height - 1);
}

/*
 * Search for the n-th occurence of a specified pattern, 
 * either forward or backward.
 */
	public void
search(search_forward, pattern, n, wantmatch)
	register int search_forward;
	register char *pattern;
	register int n;
	int wantmatch;
{
	POSITION pos, linepos;
	register char *p;
	register char *q;
	int linenum;
	int linematch;
#if RECOMP
	char *re_comp();
	char *errmsg;
#else
#if REGCMP
	char *regcmp();
	static char *cpattern = NULL;
#else
	static char lpbuf[100];
	static char *last_pattern = NULL;
#endif
#endif

	if (caseless && pattern != NULL)
	{
		/*
		 * For a caseless search, convert any uppercase
		 * in the pattern to lowercase.
		 */
		for (p = pattern;  *p != '\0';  p++)
			if (*p >= 'A' && *p <= 'Z')
				*p += 'a' - 'A';
	}
#if RECOMP

	/*
	 * (re_comp handles a null pattern internally, 
	 *  so there is no need to check for a null pattern here.)
	 */
	if ((errmsg = re_comp(pattern)) != NULL)
	{
		error(errmsg);
		return;
	}
#else
#if REGCMP
	if (pattern == NULL || *pattern == '\0')
	{
		/*
		 * A null pattern means use the previous pattern.
		 * The compiled previous pattern is in cpattern, so just use it.
		 */
		if (cpattern == NULL)
		{
			error("No previous regular expression");
			return;
		}
	} else
	{
		/*
		 * Otherwise compile the given pattern.
		 */
		char *s;
		if ((s = regcmp(pattern, 0)) == NULL)
		{
			error("Invalid pattern");
			return;
		}
		if (cpattern != NULL)
			free(cpattern);
		cpattern = s;
	}
#else
	if (pattern == NULL || *pattern == '\0')
	{
		/*
		 * Null pattern means use the previous pattern.
		 */
		if (last_pattern == NULL)
		{
			error("No previous regular expression");
			return;
		}
		pattern = last_pattern;
	} else
	{
		strcpy(lpbuf, pattern);
		last_pattern = lpbuf;
	}
#endif
#endif

	/*
	 * Figure out where to start the search.
	 */

	if (position(TOP) == NULL_POSITION)
	{
		/*
		 * Nothing is currently displayed.
		 * Start at the beginning of the file.
		 * (This case is mainly for first_cmd searches,
		 * for example, "+/xyz" on the command line.)
		 */
		pos = (POSITION)0;
	} else if (!search_forward)
	{
		/*
		 * Backward search: start just before the top line
		 * displayed on the screen.
		 */
		pos = position(TOP);
	} else if (how_search == 0)
	{
		/*
		 * Start at the second real line displayed on the screen.
		 */
		pos = position(TOP);
		do
			pos = forw_raw_line(pos);
		while (pos < position(TOP+1));
	} else if (how_search == 1)
	{
		/*
		 * Start just after the bottom line displayed on the screen.
		 */
		pos = position(BOTTOM_PLUS_ONE);
	} else
	{
		/*
		 * Start at the second screen line displayed on the screen.
		 */
		pos = position(TOP_PLUS_ONE);
	}

	if (pos == NULL_POSITION)
	{
		/*
		 * Can't find anyplace to start searching from.
		 */
		error("Nothing to search");
		return;
	}

	linenum = find_linenum(pos);
	for (;;)
	{
		/*
		 * Get lines until we find a matching one or 
		 * until we hit end-of-file (or beginning-of-file 
		 * if we're going backwards).
		 */
		if (sigs)
			/*
			 * A signal aborts the search.
			 */
			return;

		if (search_forward)
		{
			/*
			 * Read the next line, and save the 
			 * starting position of that line in linepos.
			 */
			linepos = pos;
			pos = forw_raw_line(pos);
			if (linenum != 0)
				linenum++;
		} else
		{
			/*
			 * Read the previous line and save the
			 * starting position of that line in linepos.
			 */
			pos = back_raw_line(pos);
			linepos = pos;
			if (linenum != 0)
				linenum--;
		}

		if (pos == NULL_POSITION)
		{
			/*
			 * We hit EOF/BOF without a match.
			 */
			error("Pattern not found");
			return;
		}

		/*
		 * If we're using line numbers, we might as well
		 * remember the information we have now (the position
		 * and line number of the current line).
		 */
		if (linenums)
			add_lnum(linenum, pos);

		if (caseless)
		{
			/*
			 * If this is a caseless search, convert 
			 * uppercase in the input line to lowercase.
			 * While we're at it, remove any backspaces
			 * along with the preceeding char.
			 * This allows us to match text which is 
			 * underlined or overstruck.
			 */
			for (p = q = line;  *p != '\0';  p++, q++)
			{
				if (*p >= 'A' && *p <= 'Z')
					/* Convert uppercase to lowercase. */
					*q = *p + 'a' - 'A';
				else if (q > line && *p == '\b')
					/* Delete BS and preceeding char. */
					q -= 2;
				else
					/* Otherwise, just copy. */
					*q = *p;
			}
		}

		/*
		 * Test the next line to see if we have a match.
		 * This is done in a variety of ways, depending
		 * on what pattern matching functions are available.
		 */
#if REGCMP
		linematch = (regex(cpattern, line) != NULL);
#else
#if RECOMP
		linematch = (re_exec(line) == 1);
#else
		linematch = match(pattern, line);
#endif
#endif
		/*
		 * We are successful if wantmatch and linematch are
		 * both true (want a match and got it),
		 * or both false (want a non-match and got it).
		 */
		if (((wantmatch && linematch) || (!wantmatch && !linematch)) &&
		      --n <= 0)
			/*
			 * Found the line.
			 */
			break;
	}

	jump_loc(linepos);
}

#if (!REGCMP) && (!RECOMP)
/*
 * We have neither regcmp() nor re_comp().
 * We use this function to do simple pattern matching.
 * It supports no metacharacters like *, etc.
 */
	static int
match(pattern, buf)
	char *pattern, *buf;
{
	register char *pp, *lp;

	for ( ;  *buf != '\0';  buf++)
	{
		for (pp = pattern, lp = buf;  *pp == *lp;  pp++, lp++)
			if (*pp == '\0' || *lp == '\0')
				break;
		if (*pp == '\0')
			return (1);
	}
	return (0);
}
#endif
