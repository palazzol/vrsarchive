/*
 *	reversi.h
 *
 *	include file for game program
 */

# define	SIZE	8

/*
 *	The type was was changed from char to short (here, in move.c and
 *	score.c).  This uses more storage, but is portable to machines which
 *	consider char an unsigned type.
*/
typedef short	boardT[SIZE+2][SIZE+2];

typedef boardT	*boardP;

# define	EMPTY	0
# define	WHITE	1
# define	BLACK	-1
