static char *sccsid = "@(#)main.c	1.2	4/5/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";

/*
 * Two player war game played in mazes (by David I. Bell).
 * These war game sources are in the public domain, and can be copied
 * or used as desired, with the following restrictions:
 * 1.	All copyright notices (and this notice) must be preserved.
 * 2.	The war game sources (even if modified) cannot be sold for profit.
 * 3.	If any sources are modified, a sentence must exist by the
 *	copyright notice of each modified source file which says that
 *	the file has been modified.
 */

#include "war.h"
#ifdef SYS5
#define random()	rand()
#endif

/*
 * Here on an interrupt to quit.
 */
SIG_T
sigint(dummy)
{
	dpyclose();
	exit(0);
}

main(argc, argv)
	register int	argc;		/* argument count */
	register char	**argv;		/* argument name */
{
	register char	*errstr;	/* reason for option error */

	while (argc-- > 1) {
		argv++;
		if (**argv != '-') {		/* not option, must be name */
			if (enemyname) usage();
			enemyname = *argv;
			if ((argc > 1) && (argv[1][0] != '-')) {
				enemytty = *++argv;
				argc--;
			}
			continue;
		}
		errstr = NULL;
		switch (argv[0][1]) {
		case 'e':			/* just editing */
			editflag = 1;
			break;
		case 'f':			/* setup file */
			errstr = "setup file";
			setupfile = *++argv;
			argc--;
			break;
		case 'o':			/* object file */
			errstr = "object file";
			objectfile = *++argv;
			argc--;
			break;
		case 's':			/* setup name */
			errstr = "setup";
			setupname = *++argv;
			argc--;
			break;
		default:			/* errors */
			usage();
		}
		if (errstr && ((*argv == NULL) || (**argv == '-'))) {
			fprintf(stderr, "missing %s name\n", errstr);
			exit(1);
		}
	}
	if ((enemyname == NULL) && (editflag == 0)) usage();
	if (editflag == 0) findplayer(enemyname, enemytty);
	signal(SIGINT, sigint);
	if (dpyinit(NULL, NULL)) exit(1);
	scaninit(ttychar, ttyjmp);
	boardinit();				/* build the board */
	if (readobjects(objectfile)) {		/* read in objects */
		fprintf(stderr, "bad object file\n");
		exit(1);
	}
	if (readsetup(setupfile, setupname)) {	/* read in setup */
		fprintf(stderr, "cannot get setup\n");
		exit(1);
	}
	userinit();
	dpymove(0, 0);
	talkinit();				/* set up talking with enemy */
	while (1) {
		readinfo();			/* get enemy's changes */
		view();				/* show current board */
		sleep(1);			/* wait a bit */
		docommands();			/* read and execute commands */
	}
}


/*
 * Tell how to run the game and exit
 */
usage()
{
	fprintf(stderr, "\
usage:  war [-f setupfile] [-s setupname] [-o objectfile] username [ttyname]\n\
   or:  war -e [-f setupfile] [-s setupname] [-o objectfile]\n");
	exit(1);
}


/*
 * Find the object of ours with the given character.
 * Lower case letters are converted to upper case if reasonable.
 * Returns NULL if the object cannot be found.
 */
struct object *
findobject(ch)
{
	register struct	object	*obj;		/* current object */

	for (obj = objects; obj < endobjects; obj++) {
		if ((obj->o_ownch == ch) && (obj->o_side == myside)
			&& ((playing == 0) || obj->o_count))
				return(obj);
	}
	if ((ch >= 'a') && (ch <= 'z'))		/* try upper case */
		return(findobject(ch - 'a' + 'A'));
	return(NULL);
}


/*
 * Find the object which has the given id.
 * Fails if the object could not be found.
 */
struct object *
findid(id)
{
	register struct	object	*obj;	/* found object */

	for (obj = objects; obj < endobjects; obj++) {
		if (obj->o_id == id)
			return(obj);
	}
	panic("unknown object");
}


/*
 * Place an object at a particular location.  If the object is incapable
 * of being multiply placed, it is removed from its old location.
 * If another object is currently at this location, it is removed.
 * If the object can be multiply placed then a check is made to insure
 * that all multiply paced objects can be reached from the common area.
 * Returns nonzero if placement cannot be done.
 * The screen database is also updated.
 */
placeobject(obj, row, col)
	register struct	object	*obj;	/* object to place */
	unsigned int	row;		/* row to place object at */
	unsigned int	col;		/* column to place object at */
{
	register struct	cell	*cc;	/* cell location */
	register struct	object	*oldobj;/* old object at this location */

	if (obj == NULL) return(0);
	if ((row >= ROWS) || (col >= COLS)) panic("placeloc");
	/*
	 * Remove any object already at this location
	 */
	oldobj = board[row][col].c_obj;
	if (oldobj) {
		if (oldobj == obj) return(0);	/* already here */
		removeobject(row, col);
	}
	/*
	 * If we are elsewhere on the board, remove us from there
	 */
	cc = obj->o_cell;
	if (cc) {
		if (cc->c_obj != obj) panic("badcell");
		removeobject(cc->c_row, cc->c_col);
	}
	/*
	 * Put object at the current location
	 */
	cc = &board[row][col];
	cc->c_obj = obj;
	cc->c_seen = ((obj->o_side == myside) || (obj->o_flags & F_VIS));
	if (obj->o_max <= 1) obj->o_cell = cc;
	if (cc->c_seen) dpyplace(row, col, objectchar(obj));
	obj->o_count++;
	adjustdata(obj, 1);
	/*
	 * Verify that this placement is legal
	 */
	if (obj->o_side != myside) return(0);
	if ((obj->o_count > obj->o_max) || checkboard(cc)) {
		removeobject(row, col);
		placeobject(oldobj, row, col);
		return(1);
	}
	return(0);
}


/*
 * Remove an object from the board at a given coordinate.
 * The screen database is also updated.
 */
removeobject(row, col)
	unsigned int	row;		/* row to remove object from */
	unsigned int	col;		/* column to remove object from */
{
	register struct	cell	*cc;	/* cell location */
	register struct	object	*obj;	/* object to remove */

	if ((row >= ROWS) || (col >= COLS)) panic("removeloc");
	cc = &board[row][col];
	obj = cc->c_obj;
	if (obj == NULL) return;
	adjustdata(obj, -1);
	obj->o_count--;
	obj->o_cell = NULL;
	cc->c_obj = NULL;
	dpyplace(row, col, ' ');
}


/*
 * Adjust the data counts as necessary when an object is added or removed.
 * Delta is 1 if the object is being added, and -1 if the object is
 * being removed.
 */
adjustdata(obj, delta)
	register struct	object	*obj;	/* object being added/removed */
	register int	delta;		/* change to be made */
{
	register struct	data	*dp;	/* pointer to appropriate data */
	register int	flags;		/* flag bits */
	register int	deltalife;	/* total life change for object */

	dp = &hisdata;
	if (obj->o_side == myside) dp = &mydata;
	flags = obj->o_flags;
	deltalife = (obj->o_life * delta);
	if ((flags & F_IMMOB) == 0) {		/* movable men */
		dp->d_movemen += delta;
		dp->d_movelife += deltalife;
	}
	if (flags & F_FIGHT) {			/* fighting men */
		dp->d_fightmen += delta;
		dp->d_fightlife += deltalife;
	}
	if (flags & F_BLAST) {			/* blasting men */
		dp->d_blastmen += delta;
		dp->d_blastlife += deltalife;
	}
	if (flags & F_GOAL) {			/* goals */
		dp->d_goalmen += delta;
		dp->d_goallife += deltalife;
	}
	if ((flags & F_WALL) == 0) {		/* total men */
		dp->d_totalmen += delta;
		dp->d_totallife += deltalife;
	}
	if (flags & F_WALL) {			/* walls */
		dp->d_totalwalls += delta;
	}
	newstat = 1;
}


/*
 * Pick one of a specified set of directions from a cell.
 * Each applicable direction is tested using a supplied routine.
 * The routine returns nonzero if the cell in a direction is acceptable.
 * Returns the cell in the picked direction.
 */
struct cell *
pickdir(cc, dir, routine)
	register struct	cell	*cc;	/* cell to move from */
	int	dir;			/* direction to pick */
	int	(*routine)();		/* routine to decide */
{
	register struct	cell	*tc;	/* test cell */
	register int	count;		/* number of enemies */
	register int	i;		/* index */
	struct	cell	*table[4];	/* possible directions */

	switch (dir) {
		case 'h':	tc = cc->c_left; break;
		case 'j':	tc = cc->c_down; break;
		case 'k':	tc = cc->c_up; break;
		case 'l':	tc = cc->c_right; break;
		case 'a':	tc = NULL; break;
		default:	return(NULL);
	}
	if (tc) {				/* try specified direction */
		if ((*routine)(tc) == 0) tc = NULL;
		return(tc);
	}
	for (count = 0, i = 0; i < 4; i++) {	/* search all directions */
		tc = cc->c_dirs[i];
		if ((*routine)(tc)) table[count++] = tc;
	}
	if (count == 0) return(NULL);
	if (count > 1) count = (random() % count) + 1;
	return(table[count - 1]);
}


/*
 * Remove one hit point from an object at a given location.
 * If it goes to zero, kill the object.
 */
hitobject(row, col)
{
	register struct	object	*obj;

	obj = board[row][col].c_obj;
	if (obj == NULL) return;
	adjustdata(obj, -1);
	obj->o_life--;
	adjustdata(obj, 1);
	if (obj->o_life <= 0) removeobject(row, col);
	newstat = 1;
}


/*
 * Write a single bell to warn the user of an error
 */
beep()
{
	write(STDERR, "\7", 1);
}


/*
 * Fatal error routine
 */
panic(str)
	char	*str;
{
	dpyclose();
	fprintf(stderr, "Fatal error: %s\n", str);
	exit(1);
}


/*
 * Reset the terminal modes and exit
 */
quit(status)
{
	dpyclose();
	exit(status);
}
