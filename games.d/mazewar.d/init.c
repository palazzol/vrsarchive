#ifdef SCCS
static char *sccsid = "@(#)init.c	1.2	4/5/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "war.h"

static	int	checkcount;		/* counter for board checking */
static	int	didescape;		/* did escape */
char	*ask();				/* routine to ask a question */
int	getaskchar();			/* routine to get chars */
FILE	*opensetupfile();		/* open setup files */


/*
 * Get commands from the user to initialize the board.
 */
userinit()
{
	register struct	object	*obj;	/* object being placed */
	register int	ch;		/* current character */
	register char	*str;		/* answer for user's question */
	register int	countone;	/* command count defaulted to one */
	register int	countinf;	/* count defaulted to infinity */
	int	gotcount;		/* true if read a count */
	int	row;			/* row to place it on */
	int	col;			/* column to place it on */

	row = 0;
	col = 0;
	obj = NULL;
	newstat = 1;
	setjmp(ttyjmp);			/* come back here for every command */
	if (newstat) viewstats();
	dpymove(row, col);
	dpyupdate();
	countone = 0;
	gotcount = 0;
	while (1) {			/* read numeric argument if any */
		ch = scanchar();
		if ((ch < '0') || (ch > '9')) break;
		countone = (countone * 10) + ch - '0';
		gotcount = 1;
	}
	if (countone < 0) countone = 0;
	if (countone > 100) countone = 100;
	countinf = countone;
	if (gotcount == 0) {		/* default values if necessary */
		countone = 1;
		countinf = 100;
	}
	switch (ch) {
		case '\033':		/* ESCAPE - ignore command */
			break;
		case '\014':		/* ^L - redraw screen */
			dpyredraw();
			break;
		case '\n':		/* move to next line */
			row += countone;
			if (row > HOMEROW) row = HOMEROW;
			col = 0;
			break;
		case '-':		/* move to previous line */
			row -= countone;
			if (row < 0) row = 0;
			col = 0;
			break;
		case '\t':		/* move to next tab stop */
			while ((col < (COLS-1)) && (++col % 8)) ;
			break;
		case '^':		/* move to beginning of row */
			col = 0;
			break;
		case '$':		/* move to end of row */
			col = (COLS - 1);
			break;
		case 'h':		/* move left */
			col -= countone;
			if (col < 0) col = 0;
			break;
		case 'j':		/* move down */
			row += countone;
			if (row > HOMEROW) row = HOMEROW;
			break;
		case 'k':		/* move up */
			row -= countone;
			if (row < 0) row = 0;
			break;
		case 'l':		/* move right */
		case ' ':
			col += countone;
			if (col >= COLS) col = COLS - 1;
			break;
		case 'y':		/* move to upper left */
			while (countone-- && (row > 0) && (col > 0)) {
				row--;
				col--;
			}
			break;
		case 'u':		/* move to upper right */
			while (countone-- && (row > 0) && (col < COLS-1)) {
				row--;
				col++;
			}
			break;
		case 'b':		/* move to lower left */
			while (countone-- && (row < HOMEROW) && (col > 0)) {
				row++;
				col--;
			}
			break;
		case 'n':		/* move to lower right */
			while (countone-- && (row<HOMEROW) && (col<COLS-1)) {
				row++;
				col++;
			}
			break;
		case 'f':		/* flip board left-right */
			flipboard();
			break;
		case 'p':		/* place an object */
			obj = findobject(scanchar());
			/* proceed into next case */
		case '.':		/* place same object */
			placeobject(obj, row, col);
			break;
		case 'x':		/* delete an object */
			while (countone--) {
				removeobject(row, col);
				if (col >= (COLS-1)) break;
				col++;
			}
			break;
		case 'H':		/* place lots left */
			placeline(obj, &row, &col, 0, -1, countinf);
			break;
		case 'J':		/* place lots down */
			placeline(obj, &row, &col, 1, 0, countinf);
			break;
		case 'K':		/* place lots up */
			placeline(obj, &row, &col, -1, 0, countinf);
			break;
		case 'L':		/* place lots right */
			placeline(obj, &row, &col, 0, 1, countinf);
			break;
		case 'Y':		/* place lots to upper left */
			placeline(obj, &row, &col, -1, -1, countinf);
			break;
		case 'U':		/* place lots to upper right */
			placeline(obj, &row, &col, -1, 1, countinf);
			break;
		case 'B':		/* place lots to lower left */
			placeline(obj, &row, &col, 1, -1, countinf);
			break;
		case 'N':		/* place lots to lower right */
			placeline(obj, &row, &col, 1, 1, countinf);
			break;
		case 's':		/* start to play */
			if (editflag || checksetup()) {
				beep();
				break;
			}
			if (yesno("Ready to start playing? ")) return;
			break;
		case 'q':		/* want to quit */
			if (yesno("Want to quit? ")) quit(0);
			break;
		case 'w':		/* write the setup */
			str = ask("Write setup named: ");
			if (str == NULL) break;
			if (writesetup(setupfile, str)) beep();
			break;
		case 'r':		/* read a setup */
			str = ask("Read setup named: ");
			if (str == NULL) break;
			if (readsetup(setupfile, str)) beep();
			break;
		case 'c':		/* clear board */
			if (yesno("Want to clear the board? ")) clearboard();
			break;
		default:
			beep();
			break;
	}
	scanabort();			/* go back up for next command */
}


/*
 * Ask a question and see if the answer is yes.
 * Returns nonzero if so.
 */
yesno(str)
	register char	*str;		/* string to ask, and answer */
{
	str = ask(str);
	if (str == NULL) return(0);
	return((strcmp(str, "y") == 0) || (strcmp(str, "yes") == 0)
		|| (strcmp(str, "Y") == 0) || (strcmp(str, "YES") == 0));
}


/*
 * Ask the player a question and get an answer.
 * Leading and trailing spaces are removed from the answer.
 * If an escape or a null answer is given, a null pointer is returned.
 */
char *
ask(str)
{
	register char	*cp;		/* current character */
	static	char	buf[100];	/* buffer */

	didescape = 0;
	scanreset();
	dpywindow(-1, -1, 2, COLS);
	cp = buf;
	cp += dpyread(str, getaskchar, cp, sizeof(buf));
	if (cp < buf) cp = buf;
	*cp-- = '\0';
	while ((cp >= buf) && ((*cp == ' ') || (*cp == '\t'))) *cp-- = '\0';
	for (cp = buf; ((*cp == ' ') || (*cp == '\t')); cp++) ;
	dpyhome();
	dpyclrwindow();
	dpywindow(0, -1, 1, COLS);
	if (didescape || (*cp == '\0')) cp = NULL;
	return(cp);
}


/* Routine called by dpyread to get characters */
getaskchar()
{
	char	ch;

	if ((read(STDIN, &ch, 1) != 1) || (ch == '\033')) {
		didescape = 1;
		return(-1);
	}
	if (ch == '\n') return(-1);
	return((int)(ch & 0x7f));
}


/*
 * Place a run of an object in a certain direction until it is illegal
 * or until the count runs out.  The new row and column are returned
 * through pointers.
 */
placeline(obj, rowptr, colptr, rowsign, colsign, count)
	register struct	object	*obj;	/* object to place */
	unsigned int	*rowptr;	/* current row */
	unsigned int	*colptr;	/* current column */
	register int	rowsign;	/* changes to make in rows */
	register int	colsign;	/* changes to make in columns */
{
	register unsigned int	row;	/* current row */
	register unsigned int	col;	/* current column */

	row = *rowptr;
	col = *colptr;
	while (count-- > 0) {
		if (placeobject(obj, row, col)) break;
		row += rowsign;
		col += colsign;
		if ((row > HOMEROW) || (col >= COLS)) {
			row -= rowsign;
			col -= colsign;
			break;
		}
		if (board[row][col].c_obj) break;
	}
	*rowptr = row;
	*colptr = col;
}


/*
 * Clear everything from the home area of the board.
 */
clearboard()
{
	register struct	cell *cc;	/* current cell */

	for (cc = homecell; cc; cc = cc->c_next) {
		if (cc->c_obj) removeobject(cc->c_row, cc->c_col);
	}
}


/*
 * Flip the board left to right during the initialization phase
 */
flipboard()
{
	register int	row;		/* current row */
	register struct	cell	*lc;	/* left cell */
	register struct	cell	*rc;	/* right cell */
	register struct	object	*lobj;	/* left object */
	register struct	object	*robj;	/* right object */

	for (row = 0; row <= HOMEROW; row++) {
		lc = &board[row][0];
		rc = &board[row][COLS-1];
		for (; lc < rc; lc = lc->c_right, rc = rc->c_left) {
			lobj = lc->c_obj;
			robj = rc->c_obj;
			if (lobj == robj) continue;
			lc->c_obj = robj;
			rc->c_obj = lobj;
			if (lobj && (lobj->o_max <= 1)) lobj->o_cell = rc;
			if (robj && (robj->o_max <= 1)) robj->o_cell = lc;
			if (lobj) dpyplace(lc->c_row, lc->c_col, ' ');
			if (robj) dpyplace(rc->c_row, rc->c_col, ' ');
			if (lobj) dpyplace(rc->c_row, rc->c_col, lobj->o_ownch);
			if (robj) dpyplace(lc->c_row, lc->c_col, robj->o_ownch);
		}
	}
}


/*
 * Check the setup to make sure we are using all the men we are supposed to.
 * Returns nonzero if some men are missing.
 */
checksetup()
{
	register struct	object	*obj;	/* current object */

	for (obj = objects; obj < endobjects; obj++) {
		if (obj->o_side != myside) continue;
		if (obj->o_count < obj->o_min) return(1);
	}
	return(0);
}


/*
 * Determine if an object just placed on the board leaves a legal layout.
 * This means that each board location is reachable by some path which does
 * not need to cross objects which can be multiply-placed (such as walls).
 * Returns nonzero if the layout is illegal.
 */
checkboard(nc)
	register struct	cell *nc;	/* new cell just placed */
{
	register struct	cell *cc;	/* current cell */
	register struct	cell *rc;	/* beginning of row cell */
	register struct	cell **ptr;	/* pointer to adjacent cells */

	if ((nc == NULL) || (nc->c_obj == NULL)) panic("checkboard");
	if (((nc->c_obj->o_flags&F_WALL) == 0) || (nc->c_obj->o_side != myside))
		return(0);
	/*
	 * See if the placing of the object is trivially known to be legal.
	 * This is true if nothing surrounds the object, or if a single
	 * object lies next to it which isn't surrounded.  We must search
	 * in all eight directions for this check.
	 */
	cc = NULL;
	for (ptr = nc->c_dirs; ptr < &nc->c_dirs[8]; ptr++) {
		rc = *ptr;
		if ((rc->c_obj == NULL) || ((rc->c_obj->o_flags & F_WALL) == 0))
			continue;
		if (cc) goto hard;
		cc = rc;
	}
	if (cc == NULL) return(0);
	for (ptr = cc->c_dirs; ptr < &cc->c_dirs[8]; ptr++) {
		rc = *ptr;
		if ((rc->c_obj == NULL) || ((rc->c_obj->o_flags & F_WALL) == 0))
			return(0);
	}
	/*
	 * At least two other objects lie next to the new one.
	 * We must do the brute force check now.
	 */
hard:	rc = &board[HOMEROW+1][0];
	rc->c_checkcount = ++checkcount;
	checkboardloop(rc);
	for (rc = homecell; rc; rc = rc->c_next)
		if (rc->c_checkcount != checkcount) return(1);
	return(0);
}


/*
 * Recursive subroutine used for marking accessible locations on the board.
 * Call ourself for each new cell in the four orthagonal directions from
 * the given cell.
 */
checkboardloop(cc)
	register struct	cell	*cc;	/* current cell to check */
{
	register struct	cell	*tc;	/* temporary cell */
	register struct	cell	**ptr;	/* cell pointer */
	register int	count;		/* count of cells to check */
	struct	cell	*celltab[4];	/* table of cells to check */

	while (1) {
		count = 0;
		for (ptr = cc->c_dirs; ptr < &cc->c_dirs[4]; ptr++) {
			tc = *ptr;
			if (tc->c_obj == &edgeobj) continue;
			if (tc->c_row > (HOMEROW + 1)) continue;
			if (tc->c_checkcount == checkcount) continue;
			tc->c_checkcount = checkcount;
			if (tc->c_obj && (tc->c_obj->o_flags & F_WALL))
				continue;
			celltab[count++] = tc;
		}
		if (count != 1) break;
		cc = celltab[0];	/* only one, no recursion needed */
	}
	while (count > 0) checkboardloop(celltab[--count]);
}


/*
 * Initialize the cells of the board
 */
boardinit()
{
	register struct	cell *cc;	/* current cell */
	register int	row;		/* current row */
	register int	col;		/* current column */
	register int	ch;		/* character */

	cc = &edge;
	cc->c_row = -1;
	cc->c_col = -1;
	cc->c_up = cc;
	cc->c_down = cc;
	cc->c_left = cc;
	cc->c_right = cc;
	cc->c_ul = cc;
	cc->c_ur = cc;
	cc->c_ll = cc;
	cc->c_lr = cc;
	cc->c_obj = &edgeobj;
	edgeobj.o_ownch = '*';
	edgeobj.o_altch = '*';
	edgeobj.o_side = -1;
	edgeobj.o_flags = (F_WALL|F_EDGE);
	for (row = 0; row < ROWS; row++) {
		for (col = 0; col < COLS; col++) {
			cc = &board[row][col];
			cc->c_row = row;
			cc->c_col = col;
			cc->c_up = &board[row-1][col];
			cc->c_down = &board[row+1][col];
			cc->c_left = &board[row][col-1];
			cc->c_right = &board[row][col+1];
			cc->c_ul = &board[row-1][col-1];
			cc->c_ur = &board[row-1][col+1];
			cc->c_ll = &board[row+1][col-1];
			cc->c_lr = &board[row+1][col+1];
			cc->c_next = cc->c_left;
			cc->c_obj = NULL;
		}
		cc = &board[row][0];
		cc->c_left = &edge;
		cc->c_ul = &edge;
		cc->c_ll = &edge;
		cc->c_next = &board[row-1][COLS-1];
		cc = &board[row][COLS-1];
		cc->c_right = &edge;
		cc->c_ur = &edge;
		cc->c_lr = &edge;
	}
	for (col = 0; col < COLS; col++) {
		cc = &board[0][col];
		cc->c_up = &edge;
		cc->c_ul = &edge;
		cc->c_ur = &edge;
		cc = &board[ROWS-1][col];
		cc->c_down = &edge;
		cc->c_ll = &edge;
		cc->c_lr = &edge;
	}
	board[0][0].c_next = NULL;
	firstcell = &board[ROWS-1][COLS-1];
	homecell = &board[HOMEROW][COLS-1];
	/*
	 * Initialize the looks of the board
	 */
	for (row = 0; row < ROWS; row++) {
		ch = '|';
		if ((row == HOMEROW) || (row == (ROWS - HOMEROW - 1))) ch = '+';
		dpychar(ch);
		for (col = 0; col < COLS; col++) dpychar(' ');
		dpychar(ch);
		dpychar('\n');
	}
	dpywindow(0, -1, 1, COLS);
}


/*
 * Clear the board and read in a setup from a board file.
 * If a null board name is specified, a default one in the user's home
 * directory is used.  Returns nonzero if the file cannot be found.
 */
readsetup(setupfile, setupname)
	char	*setupfile;		/* filename for board setups */
	register char	*setupname;	/* name of setup */
{
	register char	*cp;		/* current character */
	register int	row;		/* current row */
	register int	col;		/* current column */
	register FILE	*fd;		/* file handle */
	char	buf[200];		/* line of the file */

	if (setupname == NULL) return(0);
	/*
	 * Open file and search for setup name
	 */
	fd = opensetupfile(setupfile, "r");
	if (fd == NULL) return(1);
	do {
		cp = buf;
		if (fgets(cp, sizeof(buf), fd) == NULL) {
			fclose(fd);
			return(1);
		}
		if (*cp++ != '"') continue;
		while (*cp && (*cp != '"') && (*cp != '\n')) cp++;
		*cp = '\0';
	} while (strcmp(setupname, &buf[1]));
	/*
	 * Found the setup, clear the board and read it in
	 */
	clearboard();
	for (row = 0; row <= HOMEROW; row++) {
		cp = buf;
		if (fgets(cp, sizeof(buf), fd) == NULL) break;
		if (*cp == '"') break;
		for (col = 0; (col < COLS) && *cp; col++, cp++) {
			if (*cp == '\n') break;
			if (*cp == '\t') col |= 7;
			if ((*cp <= ' ') || (*cp == 0177)) continue;
			placeobject(findobject(*cp), row, col);
		}
	}
	fclose(fd);
	return(0);
}


/*
 * Append the current setup to the specified file.
 * Existing setups by the same name are not removed (which is a bug).
 * This is to make the algorithm easy.
 */
writesetup(setupfile, setupname)
	char	*setupfile;		/* filename for board setups */
	char	*setupname;		/* name of setup */
{
	register struct	cell	*rc;	/* cell at front of row */
	register struct	cell	*cc;	/* current cell in row */
	register char	*cp;		/* current character */
	register FILE	*fd;		/* file handle */
	int	error;			/* error flag */
	char	buf[COLS+2];		/* data to write */

	if (setupname == NULL) return(1);
	fd = opensetupfile(setupfile, "a");
	if (fd == NULL) return(1);
	fprintf(fd, "\"%s\"\n", setupname);
	for (rc = &board[0][0]; rc->c_row <= HOMEROW; rc = rc->c_down) {
		for (cc = rc, cp = buf; cc != &edge; cc = cc->c_right) {
			*cp++ = cc->c_obj ? cc->c_obj->o_ownch : ' ';
		}
		cp = &buf[COLS-1];
		while ((cp >= buf) && (*cp == ' ')) cp--;
		*++cp = '\n';
		*++cp = '\0';
		fputs(buf, fd);
	}
	error = ferror(fd);
	fclose(fd);
	return(error);
}


/*
 * Open the given setup file name, defaulting it if necessary.
 * Returns a stdio FILE pointer if successfull, or NULL on an error.
 */
FILE *
opensetupfile(name, mode)
	register char	*name;		/* file name, or NULL for default */
	char	*mode;			/* mode to open file in */
{
	char	buf[200];		/* buffer for default name */

	if ((name == NULL) || (*name == '\0')) {
		name = (char *) getenv("HOME");
		if (name == NULL) return(NULL);
		sprintf(buf, "%s/%s", name, SETUPFILE);
		name = buf;
	}
	return(fopen(name, mode));
}


/*
 * Read in the parameters for the objects from the specified file.
 * If a null name is given, the default file is used.
 * Doesn't return if an error is encountered.
 */
readobjects(name)
	register char	*name;		/* filename for objects */
{
	register FILE	*fd;		/* file variable */
	register struct	object	*obj;	/* current object */
	register int	line;		/* line number */
	register int	err;		/* error occurred */
	register int	count;		/* token count */
	char	flags[21];		/* flag characters */
	char	seen[3];		/* characters seen as */
	char	eol[2];			/* end of line character */
	int	life;			/* life of the object */
	int	view;			/* viewing range of the object */
	int	min;			/* minimum number of objects */
	int	max;			/* maximum number of objects */
	char	buf[200];		/* data buffer */
	char	altname[200];		/* alternate name */

	if ((name == NULL) || (*name == '\0')) name = OBJECTFILE;
	fd = fopen(name, "r");
	if (fd == NULL) {
		if (index(name, '/') == 0) {
			sprintf(altname, "%s/%s", LIBDIR, name);
			name = altname;
			fd = fopen(name, "r");
		}
		if (fd == NULL) {
			perror(name);
			quit(1);
		}
	}
	for (endobjects = objects, err = 0, line = 1; ; line++) {
		fgets(buf, sizeof(buf), fd);
		if (feof(fd) || ferror(fd)) break;
		if (buf[0] == '#') continue;		/* comment line */
		eol[0] = '#';
		count = sscanf(buf, "%20s%2s%d%d%d%d%1s\n",
			flags, seen, &life, &view, &min, &max, eol);
		if ((count < 6) || (seen[0] < ' ') || (seen[0] == 0177)
			|| (view < 0) || (min < 0) || (max < min)
			|| (eol[0] != '#')) {
				fprintf(stderr, "%s, line %d: bad object\n",
					name, line);
				err = 1;
				continue;
		}
		if (endobjects >= &objects[OBJS-1]) {
			fprintf(stderr, "%s: Too many objects\n", name);
			quit(1);
		}
		obj = endobjects++;		/* allocate object */
		obj->o_side = 0;
		obj->o_id = (line * 2);
		obj->o_ownch = seen[0];
		obj->o_altch = seen[1];
		if ((obj->o_altch < ' ') || (obj->o_altch == 0177))
			obj->o_altch = ' ';
		obj->o_view = view;
		obj->o_life = life;
		obj->o_min = min;
		obj->o_max = max;
		obj->o_count = 0;
		obj->o_cell = NULL;
		if (setflags(obj, flags)) {
			fprintf(stderr, "%s, line %d: bad flag bits\n",
				name, line);
			err = 1;
		}
		setcmds(obj, "");		/* set to fight */
		if ((obj->o_life == 0) && (obj->o_flags & F_GOAL))
			obj->o_life = 1;
		if (obj->o_max > 1) {
			obj->o_flags |= F_IMMOB;
			obj->o_flags &= ~(F_FIGHT|F_BLAST);
		}
		*endobjects = *obj;		/* duplicate for other side */
		endobjects->o_side = 1;
		endobjects->o_id++;
		endobjects++;
	}
	if (err) quit(1);
	if (ferror(fd)) {
		perror(name);
		quit(1);
	}
	fclose(fd);
	myside = 1;
}


/*
 * Parse the flag string for an object and set the appropriate flags.
 * Returns nonzero if they were illegal.
 */
setflags(obj, str)
	register struct	object	*obj;	/* object to set flags for */
	register char	*str;		/* string */
{
	register int	flags;		/* flag bits */

	flags = 0;
	while (*str) switch (*str++) {
		case 'b':	flags |= F_BLAST; break;
		case 'f':	flags |= F_FIGHT; break;
		case 'g':	flags |= F_GOAL; break;
		case 'i':	flags |= F_IMMOB; break;
		case 't':	flags |= F_TRANS; break;
		case 'v':	flags |= F_VIS; break;
		case 'w':	flags |= (F_WALL|F_IMMOB); break;
		case 'x':	flags |= F_XRAY; break;
		case '-':	break;
		default:	return(1);
	}
	obj->o_flags = flags;
	return(0);
}
