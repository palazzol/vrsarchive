/*
 * @(#)war.h	1.1	4/5/85
 * @(#)Copyright (C) 1985 by D Bell
 */

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>
#include <sgtty.h>
#include <strings.h>

#ifndef GAMEDIR
#define	GAMEDIR	"/usr/tmp"		/* for UNIX domain socket names */
#endif
#define	SETUPFILE ".warsetups"		/* default setup file in user's HOME */

#define	ROWS	24		/* rows on board */
#define	COLS	48		/* columns on board */
#define	HOMEROW	9		/* last row of my home area */
#define	DATAROW	17		/* row where data statistics go */
#define	INFOCOL	(COLS + 3)	/* leftmost column of information area */
#define	OBJS	50		/* max number of objects */
#define	CMDS	20		/* maximum number of commands */
#define	INFINITY 10000		/* "infinite" loop value */

#define	SCAN_SIZE 100		/* buffer size for scanning */
#define	SCAN_EDIT 2		/* kinds of scan longjmps */
#define	SCAN_ABORT 3
#define	SCAN_EOF 4

#define	STDIN	0		/* normal file descriptors */
#define	STDOUT	1
#define	STDERR	2


/*
 * Stored commands for execution by an object
 */
struct	cmd	{
	char	c_type;		/* command type */
	char	c_subtype;	/* subtype */
	short	c_count;	/* repeat counter */
};


/*
 * Information about each object in the game (walls, men, goals, or edges).
 * Singly placed objects point to the cell the object is at.
 * Multiply placed objects (such as walls) have a NULL pointer.
 * In all cases, each cell the object is at points back to the object.
 */
struct	object	{
	short o_flags;		/* flags about object (see below) */
	short o_id;		/* unique id for object */
	short o_life;		/* hit points left */
	short o_min;		/* minimum number of this object to place */
	short o_max;		/* maximum number of this object to place */
	short o_count;		/* current count of objects on board */
	char o_view;		/* viewing range of object */
	char o_side;		/* which side object is owned by */
	char o_ownch;		/* object as seen by its owner */
	char o_altch;		/* object as seen by others */
	struct cell *o_cell;	/* cell object is at if a single object */
	struct cmd o_cmds[CMDS];	/* current command list */
};


/* Flags in the o_flags field of an object */
#define	F_FIGHT 0x1		/* object can fight others - 'f' */
#define	F_BLAST	0x2		/* object can blast walls - 'w' */
#define	F_IMMOB	0x4		/* object is immobile - 'i' */
#define	F_GOAL	0x8		/* object is a goal - 'g' */
#define	F_WALL	0x10		/* object is a wall - 'w' */
#define	F_XRAY	0x20		/* object has x-ray vision - 'x' */
#define	F_TRANS	0x40		/* object is transparent - 't' */
#define	F_VIS	0x80		/* object is always visible - 'v' */
#define	F_MOVED	0x100		/* object has moved this turn */
#define	F_EDGE	0x200		/* object is an edge */

/* Macro to find the proper character for an object */
#define	objectchar(obj)	(((obj)->o_side==myside)?(obj)->o_ownch:(obj)->o_altch)


/*
 * Information about each location on the board.  Each cell has a pointer
 * to the object which is at that cell, if any.  Cells are linked together
 * in various ways to make it easy to move to adjacent cells.  The edges of
 * the board are linked to a special 'edge' cell.
 */
struct	cell	{
	unsigned char c_row;	/* row that this cell is for */
	unsigned char c_col;	/* column that this cell is for */
	char c_seen;		/* object is seen by the player */
	struct object *c_obj;	/* object at this location */
	long c_checkcount;	/* counter for board validity checking */
	struct cell *c_next;	/* next cell on board */
	struct cell *c_dirs[8];	/* pointers to other cells (see below) */
};


/* Directions.  The first four must be the orthagonal directions. */
#define	c_up	c_dirs[0]	/* next cell upwards */
#define	c_right	c_dirs[1]	/* next cell to the right */
#define	c_down	c_dirs[2]	/* next cell downwards */
#define	c_left	c_dirs[3]	/* next cell to the left */
#define	c_ul	c_dirs[4]	/* cell to upper left */
#define	c_ur	c_dirs[5]	/* cell to upper right */
#define	c_ll	c_dirs[6]	/* cell to lower left */
#define	c_lr	c_dirs[7]	/* cell to lower right */



/*
 * Information which is transferred between the players.
 * When it is a player's turn to transmit, as many of these messages are
 * sent as is necessary.  The last such message has a type of 'r' as a
 * signal that it is the opponent's turn to transmit.
 */
struct	info	{
	unsigned char i_type;	/* type of information */
	unsigned char i_row;	/* row number */
	unsigned char i_col;	/* column number */
	short i_id;		/* id of object */
};


/*
 * Statistics about the games which is known to both players.
 * There is one of these structures for each player.
 */
struct	data	{
	short d_fightmen;	/* count of fighting men */
	short d_blastmen;	/* count of blasting men */
	short d_movemen;	/* count of moving men */
	short d_goalmen;	/* count of goal men */
	short d_totalmen;	/* total number of men */
	short d_fightlife;	/* total life of fighting men */
	short d_blastlife;	/* total life of blasting men */
	short d_movelife;	/* total life of movable men */
	short d_goallife;	/* total life of goal men */
	short d_totallife;	/* total life of all men */
	short d_totalwalls;	/* total number of walls */
};


extern	int	errno;			/* error value */
char	myside;				/* what side I am on */
char	newstat;			/* need new status display */
char	playing;			/* actually playing now */
char	editflag;			/* just want to edit some setups */
char	*objectfile;			/* file name for reading objects */
char	*setupfile;			/* file name for board setups */
char	*setupname;			/* setup name to read in */
char	*enemyname;			/* login name of the enemy */
char	*enemytty;			/* tty name of the enemy (if given) */
struct	object	*endobjects;		/* end of active objects */
struct	cell	*firstcell;		/* first cell of whole board */
struct	cell	*homecell;		/* first cell in home area */
struct	cell	edge;			/* edge cell */
struct	object	edgeobj;		/* edge object */
struct	object	objects[OBJS];		/* table of objects */
struct	cell	board[ROWS][COLS];	/* cells of the board */
struct	data	mydata;			/* data for myself */
struct	data	hisdata;		/* data for other player */
jmp_buf	ttyjmp;				/* input jump buffer */

struct	cell	*pickdir();		/* routine to select a direction */
struct	object	*findobject();		/* routine to look up an object */
struct	object	*findid();		/* find an object by its id */
int	ttychar();			/* terminal input routine */
