#ifdef SCCS
static char *sccsid = "@(#)cmd.c	1.2	4/5/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "war.h"

int	testblast();		/* routine to test is blasting is ok */
int	testfight();		/* routine to test if fighting is ok */

/*
 * Read any command from us and set them for the object to work on.
 * This routine never blocks for input.  Partially finished commands
 * will be delayed until they are complete.  Such partial commands
 * can be edited.  Command errors ring the terminal bell.
 */
docommands()
{
	register char	*cp;			/* character pointer */
	register int	ch;			/* current character */
	register int	objchar;		/* character for object */
	char	buf[SCAN_SIZE+1];		/* input buffer */

	if (setjmp(ttyjmp) == SCAN_EOF) {	/* done with commands */
		runcommands();
		sendinfo('r', 0, 0, 0);
		return;
	}
	objchar = scanchar();			/* get object char */
	if ((objchar == ' ') || (objchar == '\n') || (objchar == '\033'))
		scanabort();
	if (ch == '\014') {
		dpyredraw();
		scanabort();
	}
	cp = buf;
	while (1) {				/* get commands for object */
		ch = scanchar();
		if ((ch == ' ') || (ch == '\n')) break;
		if (ch == '\033') scanabort();
		*cp++ = ch;
	}
	*cp = '\0';
	if (setcmds(findobject(objchar), buf))
		write(STDERR, "\7", 1);
	scanabort();
}


/*
 * Routine call by scanchar to read the next input character from
 * the terminal.  Longjmps away if a character is not ready yet.
 */
ttychar()
{
	long	n;			/* char count */
	unsigned char	ch;		/* char to return */

#ifdef FIONREAD
	if (playing && (ioctl(STDIN, FIONREAD, &n) == 0) && (n <= 0)) {
#else
	if (playing && !rdchk(STDIN)) {
#endif
		scaneof();		/* no char available now */
	}
	do {
		errno = 0;
		n = read(STDIN, (char *)&ch, 1); /* read one char */
	} while ((n < 0) && (errno == EINTR));
	return((int)(ch &= 0x7f));
}



/*
 * Execute one move's worth of commands for all of the user's objects.
 * Returns nonzero if something at all moved.
 */
runcommands()
{
	register struct	object	*obj;		/* current object */
	register int	trymore;		/* nonzero if try again */
	register int	moved;			/* something moved */

	for (obj = objects; obj < endobjects; obj++)
		obj->o_flags &= ~F_MOVED;
	moved = 0;
	do {
		trymore = 0;
		for (obj = objects; obj < endobjects; obj++) {
			trymore |= runobject(obj);
		}
		moved |= trymore;
	} while (trymore);
	return(moved);
}


/*
 * Attempt to run one move's worth of commands for an object.
 * Returns nonzero if a command was actually executed.
 * In addition, the object is flagged as having made it's move.
 */
runobject(obj)
	register struct	object	*obj;		/* object to execute */
{
	register struct	cmd	*cmd;		/* current command to execute */
	register struct	cell	*cell;		/* current cell */
	register struct	cell	*newcell;	/* new cell */
	register struct	object	*newobj;	/* other object */
	int	trying;				/* trying to do commands */

	if ((obj->o_side != myside) || (obj->o_flags & F_MOVED)) return(0);
	cell = obj->o_cell;
	if (cell == NULL) return(0);
	trying = 0;
	for (cmd = obj->o_cmds; cmd->c_type; cmd++) {
		if (cmd->c_count <= 0) continue;
		newcell = NULL;
		switch (cmd->c_type) {
			case 'b':		/* blast a wall */
				if (tryblast(obj, cmd->c_subtype)) return(1);
				break;
			case 'f':		/* fight the enemy */
				if (tryfight(obj, cmd->c_subtype)) return(1);
				break;
			case 'h':		/* move left */
				newcell = cell->c_left;
				break;
			case 'j':		/* move down */
				newcell = cell->c_down;
				break;
			case 'k':		/* move up */
				newcell = cell->c_up;
				break;
			case 'l':		/* move right */
				newcell = cell->c_right;
				break;
			default:
				panic("running unknown command");
		}
		if ((newcell == NULL) || (newcell == &edge)) continue;
		newobj = newcell->c_obj;
		if (newobj) {
			trying |= ((newobj->o_flags & F_WALL) == 0);
			continue;
		}
		placeobject(obj, newcell->c_row, newcell->c_col);
		sendinfo('p', newcell->c_row, newcell->c_col, obj->o_id);
		obj->o_flags |= F_MOVED;
		if (cmd->c_count < INFINITY) cmd->c_count--;
		return(1);
	}
	return(0);
}


/*
 * Attempt to blast a wall in a given direction.
 * Returns nonzero if we actually tried to do it.
 */
tryblast(obj, dir)
	register struct	object	*obj;		/* object doing fighting */
{
	register struct	cell	*cc;		/* current cell */

	if ((obj->o_flags & F_MOVED) || ((obj->o_flags & F_BLAST) == 0))
		return(0);
	cc = pickdir(obj->o_cell, dir, testblast);
	if (cc == NULL) return(0);
	obj->o_flags |= F_MOVED;
	obj = cc->c_obj;
	if ((obj->o_life >= 100) ||
		((obj->o_life > 0) && (random() % obj->o_life)))
			return(1);
	removeobject(cc->c_row, cc->c_col);
	sendinfo('b', cc->c_row, cc->c_col, 0);
	return(1);
}


/* See if a location is worth blasting */
testblast(cc)
	register struct	cell *cc;		/* cell to check */
{
	return((cc != &edge) && cc->c_obj && (cc->c_obj->o_flags & F_WALL));
}


/*
 * Attempt to fight in a given certain direction.
 * Returns nonzero if we actually did fight.
 */
tryfight(obj, dir)
	register struct	object	*obj;		/* object doing fighting */
{
	register struct	cell	*cc;		/* current cell */

	if ((obj->o_flags & F_MOVED) || ((obj->o_flags & F_FIGHT) == 0))
		return(0);
	cc = pickdir(obj->o_cell, dir, testfight);
	if (cc == NULL) return(0);
	obj->o_flags |= F_MOVED;
	if (random() % 2) return(1);
	sendinfo('h', cc->c_row, cc->c_col, 0);
	hitobject(cc->c_row, cc->c_col);
	return(1);
}


/* See if a cell is worth fighting */
testfight(cc)
	struct	cell *cc;			/* cell to check */
{
	register struct	object	*obj;		/* object to fight */

	obj = cc->c_obj;
	if (obj == NULL) return(0);
	return(((obj->o_flags&(F_WALL|F_EDGE))== 0) && (obj->o_side != myside));
}


/*
 * Specify the given list of commands to the specified object.
 * Returns nonzero if a bad command is given or if too many are specified.
 * If the string is valid, any previous commands are cancelled and
 * the new ones are in effect.
 */
setcmds(obj, str)
	register struct	object	*obj;		/* current object */
	register char	*str;			/* command string */
{
	register struct	cmd	*cmd;		/* next command to store */
	register int	count;			/* count for command */
	register int	gotcount;		/* got a count */
	struct	cmd	newcmds[CMDS];		/* new commands */

	if ((obj == NULL) || (obj->o_side != myside)) return(1);
	cmd = newcmds;
	while (*str) {
		count = 0;			/* read numeric argument */
		gotcount = 0;
		while ((*str >= '0') && (*str <= '9')) {
			count = (count * 10) + *str++ - '0';
			gotcount = 1;
		}
		if (count >= (INFINITY - 1)) count = (INFINITY - 1);
		if (gotcount == 0) count = INFINITY;
		if (count <= 0) return(1);
		switch (*str) {
			case 'b':		/* blast in a direction */
			case 'f':		/* fight in a direction */
				if (cmd >= &newcmds[CMDS-2]) return(1);
				cmd->c_type = *str++;
				switch (*str) {		/* verify */
					case 'h': case 'l':
					case 'j': case 'k':
					case 'a': case 'n':
						cmd->c_subtype = *str++;
						cmd->c_count = INFINITY;
						cmd++;
						continue;
				}
				return(1);

			case 'h':		/* move left small amount */
			case 'j':		/* move down small amount */
			case 'k':		/* move up small amount */
			case 'l':		/* move right small amount */
				if (cmd >= &newcmds[CMDS-2]) return(1);
				if (gotcount == 0) count = 1;
				cmd->c_type = *str++;
				cmd->c_subtype = '\0';
				cmd->c_count = count;
				cmd++;
				continue;

			case 'H':		/* move left large amount */
			case 'J':		/* move down large amount */
			case 'K':		/* move up large amount */
			case 'L':		/* move right large amount */
				if (cmd >= &newcmds[CMDS-2]) return(1);
				cmd->c_type = *str++ - 'A' + 'a';
				cmd->c_subtype = '\0';
				cmd->c_count = count;
				cmd++;
				continue;

			default:		/* illegal */
				return(1);
		}
	}
	cmd->c_type = 'f';			/* add default fight command */
	cmd->c_subtype = 'a';
	cmd->c_count = INFINITY;
	cmd++;
	cmd->c_type = '\0';			/* end command table */
	bcopy(newcmds, obj->o_cmds, sizeof(obj->o_cmds));	/* set cmds */
	return(0);
}
