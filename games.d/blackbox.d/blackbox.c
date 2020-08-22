/* BLACKBOX.C: The Black Box Game */

char signon[] = "Black Box Game X101";

/*

The  Black Box Game by William Gervasi,  Intel Corporation, 1984.  This version
borrows heavily  from the Black Box Game written  in PL/M-80 by Ross Morgan and
William Gervasi for the Intel Series-II computer in 1980.

*/

#include <curses.h>
#ifndef A_REVERSE
#  define erasechar()	(_tty.sg_erase)
#endif
#include <ctype.h>

/* excerpted from c.h to avoid definition conflicts with curses.h */
#define PROCEDURE /**/
#define PROGRAM /**/
#define INTERNAL /**/
#define FOREVER for(;;)
#define NIL '\0'
#define BOOL int
#define EJECT /**/
#define AND &&
#define OR ||
#define EQ ==
#define NEQ !=
#define MOD %
#define NOT !
#define SPACE ' '
#define RUBOUT '\177'
#define RETURN '\r'
#define LINEFEED '\n'
#define TVOID int

#define ARRAY_SIZE 8		/* 8 X 8 array */
#define BALL_COUNT 5		/* With 5 hidden balls */

#define ERROR 0
#define NO_ERROR 1

#define WEST 0				/* NOT arbitrary numbers!!! */
#define SOUTH 1
#define EAST 2
#define NORTH 3

#define ABSORBED -2			/* unique values off the playing board */
#define REFLECTED -3

#define COMMAND_Y 22		/* screen coordinates */
#define COMMAND_X 6
#define RESPONSE_Y 22
#define RESPONSE_X 15
#define ERROR_Y 23
#define ERROR_X 6
#define PROBES_Y 15
#define PROBES_X 66
#define SCORE_Y 17
#define SCORE_X 75
#define GAMES_Y 18
#define GAMES_X 72
#define AVE_Y 19
#define AVE_X 67
#define BOTTOM_Y 24
#define BOTTOM_X 0
#define SIGNON_Y 10
#define SIGNON_X 30
#define INSQ_Y 14
#define INSQ_X 26
#define DEBUG_Y 20
#define DEBUG_X 58
#define DEBMSG_Y 20
#define DEBMSG_X 65

BOOL debug;						/* program debugger */

struct
{
	int W;	/* presence information about all neighboring squares */
	int S;
	int E;
	int N;
} board[ARRAY_SIZE + 2][ARRAY_SIZE + 2];	/* playing table */

BOOL guess[ARRAY_SIZE][ARRAY_SIZE];			/* placed guesses */
BOOL probe_used[ARRAY_SIZE * 4];			/* one for each entry/exit point */
int balls[BALL_COUNT];						/* x-y coordinates of balls */

int row;						/* current row of a probe */
int col;						/*   "     column */
char probe_letter;				/* sequence letter for probe */
int probes_used;				/* number of probes used so far */
int score;						/* score this game */
int total_score;				/* total score for all games */
float average_score;			/* average across all games */
int number_of_games;			/* number of games played so far */
int guesses;					/* number of guesses placed */
int direction;					/* indicates direction of probe */
BOOL error_line;				/* indicates error message been printed */
char erasech;					/* tty erase setting */

struct xy						/* screen coordinates */
{
	int x;
	int y;
};
struct xy letters[ARRAY_SIZE * 4];
struct xy squares[ARRAY_SIZE][ARRAY_SIZE];

/* macros */
#define SKIP_BLANKS(ptr) while(isspace(*ptr)) ptr++;
char _x_;
#define UPPER(c) ((((_x_ = c)>='a') && (_x_<='z')) ? (_x_-'a'+'A') : _x_)

/* procedures appear in source code alphabetically */
INTERNAL PROCEDURE TVOID another_game();
INTERNAL PROCEDURE TVOID clear_guesses_and_balls();
INTERNAL PROCEDURE TVOID get_cmd();
INTERNAL PROCEDURE int get_num();
INTERNAL PROCEDURE int guess_where();
INTERNAL PROCEDURE TVOID move_probe();
INTERNAL PROCEDURE TVOID new_game();
INTERNAL PROCEDURE int path();
INTERNAL PROCEDURE TVOID place_guess();
INTERNAL PROCEDURE TVOID play_game();
INTERNAL PROCEDURE TVOID power_up();
INTERNAL PROCEDURE TVOID print_debmsg();
INTERNAL PROCEDURE TVOID print_err();
INTERNAL PROCEDURE TVOID quit();
INTERNAL PROCEDURE TVOID remove_guess();
INTERNAL PROCEDURE int send_probe();
INTERNAL PROCEDURE BOOL tally();
INTERNAL PROCEDURE TVOID update_the_score();

/* from module DRAWBOX */
extern PROCEDURE TVOID draw_board();
extern PROCEDURE TVOID instructions();

/* from UNIX libraries */
extern PROCEDURE int rand();
extern PROCEDURE TVOID srand();
extern PROCEDURE long time();

EJECT
/*
EXTERNAL DESCRIPTION:

The Black Box game hides BALL_COUNT balls in a square array ARRAY_SIZE on each
side. The player rolls probes into the square horizontally or vertically from
any point around the square then watches where the probe rolls back out to
determine the location of the balls. The player places and removes guesses as
the game progresses with no indication from Black Box as to the correctness of
the guesses. When the player has BALL_COUNT guesses placed on the playing
board, he may choose to tally his points and exit the game round.

Points are given to the player for the following: 1 point for each probe used,
5 points for each incorrect guess. The object is for the player to get the
fewest points possible.

The following rules determine the path of the probes and the player indicators:

	1.	If the probe hits a ball head on, it is absorbed into the ball. The
		probe does not exit from the playing board as indicated by a "?" at the
		point where the probe was rolled in.

	2.	If a probe tries to pass between two balls, it is reflected back the
		way it came and exits the playing board at the same point where it
		entered as indicated by a "!" at the point where the probe rolled in.

	3.	If a probe tries to roll immediately to the side of a ball, it is
		deflected 90 degrees away from the ball.

	4.	Absorption has priority over reflection, which has priority over
		deflection.

	5.	Deflection off a ball at the edge of the square is shown as a
		reflection "!".

	6.	A probe that exits at a different point than it entered is given a
		sequence letter, starting at "A".

For example:										Comments:

		   20    19    18    17    16				Probe A deflects off ball
				 A				   B				at 2-18
		+-----+-----+-----+-----+-----+
	1 A	|     |     |     |     |     |		15		Probe in at 5 absorbed
		+-----+-----+-----+-----+-----+
	2	|     |     |  *  |     |     |		14		Probe in at 3 deflected off
		+-----+-----+-----+-----+-----+				ball at 2-18, reflected off
	3 !	|     |     |     |     |     |		13		balls at 5-6 and 5-8
		+-----+-----+-----+-----+-----+
	4 !	|     |     |     |     |     |		12		Probe in at 4 deflected off
		+-----+-----+-----+-----+-----+				ball at 5-6, shown as re-
	5 ?	|  *  |     |  *  |     |     |		11		flection at board edge
		+-----+-----+-----+-----+-----+
					   ?	 !	   B				Probe B untouched
		   6     7     8     9     10

*/
EJECT
/*
INTERNAL DESCRIPTION:

The "board" array is essentially the set of all neighbors of the actual playing
board.  Each element holds no information about the presence of a ball in
itself, but rather only information about its neighbor squares.  For this
reason, the board array is 2 larger than the playing area in both x and y
dimensions.

Each element of the board array has four fields, each representing a view of
the three neighbors seen when facing a particular direction.  The relative
position of the three neighbor squares is weighted so that it is clear which
neighbors contain a ball, if any.  Below is a map of an arbitrary square's
neighbors:

	NW	N	NE

	W	*	E

	SW	S	SE

The weighting of neighboring square's ball presence is +1 for a ball to the
right, +2 for a ball dead ahead, and +4 for a ball to the left.  Therefore, if
the direction of travel is EAST, then the neighbors of interest are NE, E, and
SE, where the weightings are +4, +2, and +1 respectively to indicate a ball in
any of these neighbors.  If there were a ball in the NE and SE squares and none
in the E square, then the ".E" field for this square would equal 5 and the
action would be to reflect back the way we came (see rule #2 above).

Another unique aspect of this implementation of the Black Box game is putting
most of the lookahead work in the initialization code, routine new_game().
This minimizes the amount of processing done during the playing portion of the
game.  It is intended that this game be limited by the I/O time needed to
interact with the player's CRT, not by processing time..

This is the heart of the Black Box Game; the rest of the code is mostly the
human interface.
*/
EJECT
/*


						MAIN PROGRAM LOOP.


Initialize the system, then play games until the player chooses to exit.


*/

PROGRAM
main(argc, argv)

int argc;				/* number of arguments */
char *argv[];			/* array of string pointers */
{
/*
Look for the word "debug" as the first parameter in an argument list.
*/
	if ((argc > 1) AND (strcmp(argv[1], "debug") EQ 0))
		debug = TRUE;
	else
		debug = FALSE;

	power_up();			/* print instructions, initialize screen */
	FOREVER
	{
		new_game();		/* set up random game */
		play_game();	/* play the game */
		another_game();	/* erase leftovers from game */
	}
}
EJECT

/*
NAME: another_game
FUNCTION: query player for another game. if affirmative, clear junk on
screen from last game and continue, else exit.
*/

PROCEDURE TVOID
another_game()
{
	move(COMMAND_Y, COMMAND_X);
	clrtoeol();
	standout();
	printw("Want to play again? ");
	standend();
	refresh();
	if (UPPER(getch()) NEQ 'Y')
	{
		move(BOTTOM_Y, BOTTOM_X);
		refresh();
		resetty();					/* restore original tty settings */
		endwin();
		exit(0);					/* exit, no error */
	}
	number_of_games++;
	clear_guesses_and_balls();
}
EJECT

/*
NAME: clear_guesses_and_balls
FUNCTION: remove junk on screen from game just played. avoid clearing
the screen and rewriting everything because that is not what computer
science is all about.
*/

PROCEDURE TVOID
clear_guesses_and_balls()
{
	int count;				/* loop index */

/*
Clear all squares in balls array. Note that this removes correct guesses.
*/
	for (count = 0; count < BALL_COUNT; count++)
	{
		row = balls[count] / ARRAY_SIZE;
		col = balls[count] MOD ARRAY_SIZE;
		remove_guess();
		if (guess[row][col])
			guess[row][col] = FALSE;
	}

/*
Now remove the player's incorrect guesses.
*/
	for (row = 0; row < ARRAY_SIZE; row++)
		for (col = 0; col < ARRAY_SIZE; col++)
			if (guess[row][col])
				remove_guess();

/*
Clear all used probe entry and exit indicators.
*/
	for (count = 0; count < ARRAY_SIZE * 4; count++)
		if (probe_used[count])
		{
			move(letters[count].y, letters[count].x);
			printw(" ");
		}

/*
Clear junk. Rewrite the screen.
*/
	move(PROBES_Y, PROBES_X);
	clrtoeol();
	move(COMMAND_Y, COMMAND_X);
	clrtoeol();
	standout();
	printw("COMMAND?");
	standend();
	move(ERROR_Y, ERROR_X);
	clrtoeol();
	refresh();
}
EJECT

/*
NAME: get_cmd
FUNCTION: read a line from the player. clear the error area in case
there was an error message printed.
*/

PROCEDURE TVOID
get_cmd(line, size)

char line[];					/* line buffer */
int size;						/* size of line buffer */
{
	int index;					/* index into line */

	move(RESPONSE_Y, RESPONSE_X);
	clrtoeol();
	refresh();
	index = 0;
	while (index < size - 1)
	{
		line[index] = getch();
		if ((line[index] EQ RETURN) OR (line[index] EQ LINEFEED))
			break;
		else if (line[index] EQ erasech)
		{
			if (index > 0)
			{
				index--;
				line[index] = NIL;
				move(RESPONSE_Y, RESPONSE_X);
				clrtoeol();
				printw(line);
				refresh();
			}
		}
		else if ((line[index] >= SPACE)	/* not a control character */
			AND (line[index] < RUBOUT))	/* not rubout */
		{
			addch(line[index]);
			refresh();
			index++;
		}
	}
	line[index] = NIL;

/*
If the previous command produced an error message, clear the error line.
*/
	if (error_line)
	{
		move(ERROR_Y, ERROR_X);
		clrtoeol();
		error_line = FALSE;
	}

/*
Rewrite the screen.
*/
	refresh();
}
EJECT

/*
NAME: get_num
FUNCTION: Get the decimal number at the ptr; leave the ptr beyond the
number.
*/

PROCEDURE int
get_num(ptr)

char **ptr;					/* pointer to character pointer */
{
	int value;				/* calculated sum so far */

	value = 0;
	while (isdigit(**ptr))
	{
		value = (value * 10) + (**ptr - '0');
		(*ptr)++;
	}

	return (value);
}
EJECT

/*
NAME: guess_where
FUNCTION: check for parametric validity, then place a guess at the specified
coordinates if there is not a guess currently there, else remove the guess if
there is one already there.
*/

PROCEDURE int
guess_where(first, second)

int first;						/* first x-y paramter */
int second;						/* second " */
{
	int first_range;			/* which face (N,S,E,W) is first in */
	int second_range;			/* " for second */
	int scratch;				/* swap storage */

/*
Insure that the lower number is first; simplifies later part of algorithm.
*/
	if (first > second)
	{
		scratch = first;
		first = second;
		second = scratch;
	}

/*
The ranges describe which side of the square the number is in. The two
numbers must be in adjacent sides in order to uniquely describe a
single point in the playing board.
*/
	first_range = (first - 1) / ARRAY_SIZE;
	second_range = (second - 1) / ARRAY_SIZE;

	if (first_range EQ second_range)
		return (ERROR);

	if (first_range EQ 0)				/* LEFT side */
	{
		switch(second_range)
		{
			case 2: return (ERROR);		/* RIGHT side */

			case 1:						/* BOTTOM */
				row = first - 1;
				col = second - 1 - ARRAY_SIZE;
				break;

			case 3:						/* TOP */
				row = first - 1;
				col = ARRAY_SIZE * 4 - second;
				break;

			default:
				print_err("CODE ERROR IN guess_where()");
				return (ERROR);
		}
	}

	else if (first_range EQ 1)			/* BOTTOM */
	{
		if (second_range EQ 3)			/* TOP */
			return (ERROR);

		if (second_range EQ 2)			/* RIGHT side */
		{
			row = ARRAY_SIZE * 3 - second;
			col = first - 1 - ARRAY_SIZE;
		}
		else
		{
			print_err("CODE ERROR IN guess_where()");
			return (ERROR);
		}
	}

	else if ((first_range EQ 2) AND (second_range EQ 3))
	{
		row = ARRAY_SIZE * 3 - first;
		col = ARRAY_SIZE * 4 - second;
	}

	else
	{
		print_err("CODE ERROR IN guess_where");
		return (ERROR);
	}

/*
We now have valid x-y coordinates within the playing board. If there is
a guess there, remove it. Otherwise, put a guess there.
*/
	if (guess[row][col])
	{
		if (debug)
			print_debmsg("guess removed");
		guess[row][col] = FALSE;
		guesses--;
		remove_guess();
	}
	else
	{
		if (debug)
			print_debmsg("guess added");
		guess[row][col] = TRUE;
		guesses++;
		place_guess();
	}

	return (NO_ERROR);
}
EJECT

/*
NAME: move_probe
FUNCTION: adjust globals row and col based on the requested move direction.
*/

PROCEDURE TVOID
move_probe(dir)

int dir;				/* direction to move */
{
	switch(dir)
	{
		case WEST: col--; break;
		case SOUTH: row++; break;
		case EAST: col++; break;
		case NORTH: row--; break;
		default: print_err("CODE ERROR IN move_probe()"); break;
	}
}
EJECT

/*
NAME: new_game
FUNCTION: initialize internal tables in preparation for a new game.
*/

PROCEDURE TVOID
new_game()
{
	int count;			/* scratch loop counter */
	int possible;		/* temporary storage for random numbers */

/*
First we initialize the playing board and guess array to empty. Note that the
guess array is smaller than the playing board: the lookahead algorithm for
paths through the board are simplified by placing a ring of excess squares
around the actual playing surface.
*/
	for (row = 0; row < ARRAY_SIZE + 2; row++)
		for (col = 0; col < ARRAY_SIZE + 2; col++)
		{
			board[row][col].N  = 0;
			board[row][col].W  = 0;
			board[row][col].E  = 0;
			board[row][col].S  = 0;
			if ((row < ARRAY_SIZE) AND (col < ARRAY_SIZE))
				guess[row][col] = FALSE;
		}
	guesses = 0;

/*
Now we zero out the probe used array, and set the balls array to nonsense
values (out of the acceptable range).
*/
	for (count = 0; count < ARRAY_SIZE * 4; count++)
		probe_used[count] = FALSE;
	probes_used = 0;

	for (count = 0; count < BALL_COUNT; count++)
		balls[count] = -1;

/*
Fill the balls array with random numbers in the range 0 to ARRAY_SIZE squared
minus one. This range allows for an x and a y coordinate specifier. Check
through the array for uniqueness before setting the first invalid value to the
latest random number.
*/
	while (balls[BALL_COUNT - 1] EQ -1)
	{
		possible = rand() MOD (ARRAY_SIZE * ARRAY_SIZE);
		for (count = 0; count < BALL_COUNT; count++)
		{
			if (balls[count] EQ -1)		/* first invalid # found */
			{
				balls[count] = possible;
				break;
			}
			else if (balls[count] EQ possible)	/* not unique */
				break;
		}
	}

/*
Now we have a handful of random numbers: put balls into those x-y coordinates.
The presence of a ball is indicated by setting the appropriate part of a
neighbor squares' board structure. A square has the following neighbors:

	NW	N	NE

	W	*	E

	SW	S	SE

Based on the "direction" the probe is facing, the weighting factors are:

	ball to right: +1
	ball ahead:    +2
	ball to left:  +4

This replaces the bit manipulation of the original version with a more portable
algorithm.
*/
	for (count = 0; count < BALL_COUNT; count++)
	{
		row = (balls[count] / ARRAY_SIZE) + 1;
		col = (balls[count] MOD ARRAY_SIZE) + 1;
		board[row - 1][col - 1].S += 4;		/* NW neighbor facing S */
		board[row - 1][col - 1].E += 1;		/* NW neighbor facing E */
		board[row - 1][col    ].S += 2;		/* N  neighbor facing S */
		board[row - 1][col + 1].S += 1;		/* NE neighbor facing S */
		board[row - 1][col + 1].W += 4;		/* NE neighbor facing W */
		board[row    ][col - 1].E += 2;		/* W  neighbor facing E */
		board[row    ][col + 1].W += 2;		/* E  neighbor facing W */
		board[row + 1][col - 1].N += 1;		/* SW neighbor facing N */
		board[row + 1][col - 1].E += 4;		/* SW neighbor facing E */
		board[row + 1][col    ].N += 2;		/* S  neighbor facing N */
		board[row + 1][col + 1].N += 4;		/* SE neighbor facing N */
		board[row + 1][col + 1].W += 1;		/* SE neighbor facing W */
	}

/*
If the debug flag is set, show where balls are.
*/
	if (debug)
		for (count = 0; count < BALL_COUNT; count++)
		{
			row = balls[count] / ARRAY_SIZE;
			col = balls[count] MOD ARRAY_SIZE;
			move(squares[row][col].y, squares[row][col].x + 1);
			printw("*");
		}

/*
Finally, start out the probe lettering at A.
*/
	probe_letter = 'A';
}
EJECT

/*
NAME: path
FUNCTION: determine the type and location of the exit of a probe from
the playing board.
*/

PROCEDURE int
path()
{
	BOOL just_entered;			/* indicates first step into playing array */
	int look;					/* temp storage for next-step calc */

	just_entered = TRUE;
	FOREVER
	{
		/* First check for exit from playing board */
		if ((col EQ 1) AND (direction EQ WEST))
			return (row);
		if ((row EQ ARRAY_SIZE) AND (direction EQ SOUTH))
			return (col + ARRAY_SIZE);
		if ((col EQ ARRAY_SIZE) AND (direction EQ EAST))
			return (ARRAY_SIZE * 3 + 1 - row);
		if ((row EQ 1) AND (direction EQ NORTH))
			return (ARRAY_SIZE * 4 + 1 - col);

		/* Get lookahead value based on probe direction. */
		switch(direction)
		{
			case WEST:  look = board[row][col].W; break;
			case SOUTH: look = board[row][col].S; break;
			case EAST:  look = board[row][col].E; break;
			case NORTH: look = board[row][col].N; break;
			default: print_err("CODE ERROR IN path()"); break;
		}

		/* Process lookahead value to determine action. */
		switch(look)
		{
			case 2:
			case 3:
			case 6:
			case 7: return (ABSORBED);				/* ball ahead */

			case 5: return (REFLECTED);				/* ball to R & L */

			case 0: move_probe(direction); break;	/* no ball ahead */

			case 1:									/* ball to right */
				if (just_entered)
					return (REFLECTED);
				direction = (direction EQ 3 ? 0 : direction + 1); /* + MOD */
				break;

			case 4:									/* ball to left */
				if (just_entered)
					return (REFLECTED);
				direction = (direction EQ 0 ? 3 : direction - 1); /* - MOD */
				break;

			default: print_err("CODE ERROR IN path()"); break;
		}

		just_entered = FALSE;
	}
}
EJECT

/*
NAME: place_guess
FUNCTION: print ( ) at appropriate coordinate on playing board to
indicate player's guess.
*/

PROCEDURE TVOID
place_guess()
{
	move(squares[row][col].y, squares[row][col].x);
	printw("( )");
}
EJECT

/*
NAME: play_game
FUNCTION: play one round of game until a valid tallying has occured.
*/

PROCEDURE TVOID
play_game()
{
	char line_buf[80];			/* buffer for player command line input */
	char *line_ptr;				/* command parser pointer */
	int first;					/* store first number in command */
	int second;					/* store second number in command */

	FOREVER
	{
		get_cmd(line_buf, 80);	/* read NIL-terminated line */
		line_ptr = line_buf;
		SKIP_BLANKS(line_ptr);

/*
Parse input line from player. Valid input is:

	@	Tally and exit.
	Q	Emergency exit.
	n	Number; probe entry point.
	n,n	Two numbers; place or remove a guess.
*/
		if (*line_ptr EQ NIL)		/* null input line */
			;

		else if (*line_ptr EQ '@')	/* tally command */
		{
			if (tally())			/* successful tally? */
				break;				/* game round over */
		}

		else if (UPPER(*line_ptr) EQ 'Q')	/* quit command */
			quit();

		else if (NOT isdigit(*line_ptr))
			print_err("BAD COMMAND: NUMBER OR @ EXPECTED");

		else
		{
			first = get_num(&line_ptr);
			SKIP_BLANKS(line_ptr);
			if ((first < 1) OR (first > ARRAY_SIZE * 4))
				print_err("BAD NUMBER: OUT OF RANGE");

			else if (*line_ptr EQ NIL)		/* send probe */
			{
				if (send_probe(first))
				{
					probes_used++;
					move(PROBES_Y, PROBES_X);
					printw("%d", probes_used);
				}
				else
					print_err("BEEN USED: TRY ANOTHER POINT");
			}

			else if (*line_ptr EQ ',')		/* place/remove guess */
			{
				line_ptr++;
				SKIP_BLANKS(line_ptr);
				if (NOT isdigit(*line_ptr))
					print_err("BAD NUMBER: EXPECTED NUMBER AFTER ,");
				else
				{
					second = get_num(&line_ptr);
					if ((second < 1) OR (second > ARRAY_SIZE * 4))
						print_err("BAD NUMBER: OUT OF RANGE");
					else if (NOT guess_where(first, second))
						print_err("BAD COORDINATES: NOT A UNIQUE POINT");
				}
			}

			else
				print_err("BAD INPUT: UNEXPECTED DATA AFTER NUMBER");
		}
	}
}
EJECT

/*
NAME: power_up
FUNCTION: print instructions if requested, then write initial screen
for the game.
*/

PROCEDURE TVOID
power_up()
{
	int rnd;					/* store randomizer seed */

	initscr();					/* initialize curses routines */
	savetty();					/* save tty settings */
	noecho();					/* disable keyboard echo */
	erasech = erasechar();
	crmode();					/* set cbreak mode */
	clear();					/* blank the screen */
	move(SIGNON_Y, SIGNON_X);
	standout();
	printw(signon);
	standend();
	move(INSQ_Y, INSQ_X);
	printw("Do you want instructions? ");
	refresh();					/* write the screen */
	if (UPPER(getch()) EQ 'Y')
		instructions();
	clear();
	draw_board();				/* rewrite the screen with playing board */
	standout();
	move(COMMAND_Y, COMMAND_X);
	printw("COMMAND?");
	standend();
	if (debug)
	{
		move(DEBUG_Y, DEBUG_X);
		printw("DEBUG:");		/* Debug message display area */
	}
	refresh();
	score = 0;
	total_score = 0;
	number_of_games = 1;
	average_score = 0.0;
	error_line = FALSE;			/* initially no error message on screen */

/*
Start the randomizer using "the number of seconds since the epoch" as the
randomizer seed. I used the simpler rand-srand p*ir in preference to the newer
random-srandom routines to assure greater portability of the code.
*/
	srand(rnd = (int) time(0));
	if (debug)
	{
		move(DEBMSG_Y, DEBMSG_X);
		printw("srand seed %d", rnd);
		refresh();
	}

/*
Initialize the letters and squares arrays by calculation of the screen
coordinates. This method is used rather than letting the compiler do
the work so as to allow changing to variable-sized boards someday.

To follow this, it is easiest to copy the screen onto paper and number
the margins; 0,0 is the upper left corner.  The basic square looks like:

	|-----|
	| (*) |			shown with guess and ball printed as in tally
	|-----|
*/
	for (row = 0; row < ARRAY_SIZE; row++)
		for (col= 0; col < ARRAY_SIZE; col++)
		{
			squares[row][col].x = col * 6 + 5;
			squares[row][col].y = row * 2 + 3;
		}

	/* Left side */
	for (col = 0; col < ARRAY_SIZE; col++)
	{
		letters[col].x = 2;
		letters[col].y = col * 2 + 3;
	}

	/* Bottom */
	for (row = 0; row < ARRAY_SIZE; row++)
	{
		letters[row + ARRAY_SIZE].x = row * 6 + 6;
		letters[row + ARRAY_SIZE].y = ARRAY_SIZE * 2 + 3;
	}

	/* Right side */
	for (col = 0; col < ARRAY_SIZE; col++)
	{
		letters[col + ARRAY_SIZE * 2].x = ARRAY_SIZE * 6 + 4;
		letters[col + ARRAY_SIZE * 2].y = (ARRAY_SIZE - col) * 2 + 1;
	}

	/* Top */
	for (row = 0; row < ARRAY_SIZE; row++)
	{
		letters[row + ARRAY_SIZE * 3].x = (ARRAY_SIZE - row) * 6;
		letters[row + ARRAY_SIZE * 3].y = 1;
	}

}
EJECT

/*
NAME: print_debmsg
FUNCTION: print debug message.
*/

PROCEDURE TVOID
print_debmsg(msg)

char *msg;						/* string */
{
	if (debug)
	{
		move(DEBMSG_Y, DEBMSG_X);
		printw("%s", msg);
	}
	else
		print_err("CODE ERROR IN print_debmsg()");
}
EJECT

/*
NAME: print_err
FUNCTION: print error message.
*/

PROCEDURE TVOID
print_err(msg)

char *msg;						/* string */
{
	move(ERROR_Y, ERROR_X);
	printw("ERROR: %s", msg);
	error_line = TRUE;
}
EJECT

/*
NAME: quit
FUNCTION: emergency exit... the boss is coming... clear the screen and
exit.  Someday, this should save the game for later resumption.
*/

PROCEDURE TVOID
quit()
{
	clear();
	refresh();
	resetty();		/* restore initial tty settings */
	endwin();
	exit(1);		/* indicate error exit for shell processing, if needed */
}
EJECT

/*
NAME: remove_guess
FUNCTION: print 3 spaces at appropriate coordinates to remove a
player's guess. also note that during tally, this routine clears the
ball at the same coordinate.
*/

PROCEDURE TVOID
remove_guess()
{
	move(squares[row][col].y, squares[row][col].x);
	printw("   ");
}
EJECT

/*
NAME: send_probe
FUNCTION: check for validity of the number passed in... if a probe has
previously entered or exitted there, return an error. else, send a probe in and
display the appropriate result on the screen.
*/

PROCEDURE int
send_probe(number)

int number;							/* array entry point requested by player */
{
	int path_result;				/* returned value from path() */

	if (probe_used[number - 1])
		return (ERROR);
	probe_used[number - 1] = TRUE;

/*
Determine the direction of probe travel based on which face of the square the
entry point is in.
*/
	if (number <= ARRAY_SIZE)
	{
		row = number;
		col = 0;
		direction = EAST;
	}
	else if (number <= ARRAY_SIZE * 2)
	{
		row = ARRAY_SIZE + 1;
		col = number - ARRAY_SIZE;
		direction = NORTH;
	}
	else if (number <= ARRAY_SIZE * 3)
	{
		row = ARRAY_SIZE * 3 + 1 - number;
		col = ARRAY_SIZE + 1;
		direction = WEST;
	}
	else
	{
		row = 0;
		col = ARRAY_SIZE * 4 + 1 - number;
		direction = SOUTH;
	}

/*
Send the probe through the playing board and watch where (and if) if comes out.
*/
	path_result = path();
	switch(path_result)
	{
		case ABSORBED:
			move(letters[number - 1].y, letters[number - 1].x);
			printw("?");
			break;

		case REFLECTED:
			move(letters[number - 1].y, letters[number - 1].x);
			printw("!");
			break;

		default:
			probe_used[path_result - 1] = TRUE;
			move(letters[number - 1].y, letters[number - 1].x);
			printw("%c", probe_letter); /* entry */
			move(letters[path_result - 1].y, letters[path_result - 1].x);
			printw("%c", probe_letter);	/* exit */
			probe_letter++;
			if (debug)
			{
				move(DEBMSG_Y, DEBMSG_X);
				printw("in %d out %d", number, path_result);
				refresh();
			}
			break;
	}
	return (NO_ERROR);
}
EJECT

/*
NAME: tally
FUNCTION: if exactly five guesses are placed, tally the score and return true,
else return false. Scoring is: 1 pt per probe sent, 5 pts for each wrong guess.
*/

PROCEDURE BOOL
tally()
{
	int count;					/* scratch counter */

	if (guesses < 5)
	{
		print_err("TOO FEW GUESSES");
		return (FALSE);
	}
	if (guesses > 5)
	{
		print_err("TOO MANY GUESSES");
		return (FALSE);
	}

	score = probes_used;
	for (count = 0; count < BALL_COUNT; count++)
	{
		row = balls[count] / ARRAY_SIZE;
		col = balls[count] MOD ARRAY_SIZE;
		move(squares[row][col].y, squares[row][col].x + 1);
		printw("*");
		if (NOT guess[row][col])
			score += 5;
	}
	total_score += score;
	average_score = (float) total_score / (float) number_of_games;

/*
Write the updated score values on the screen.
*/
	update_the_score();

	return (TRUE);
}
EJECT

/*
NAME: update_the_score
FUNCTION: print score for this game in two places, update number of
games played, and average score.
*/

PROCEDURE TVOID
update_the_score()
{
	move(ERROR_Y, ERROR_X);
	clrtoeol();
	printw("Your score this game is %d", score);
	move(SCORE_Y, SCORE_X);
	clrtoeol();
	printw("%d", score);
	move(GAMES_Y, GAMES_X);
	printw("%d", number_of_games);
	move(AVE_Y, AVE_X);
	clrtoeol();
	printw("%4.2g", average_score);
}
