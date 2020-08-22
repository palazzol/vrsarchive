/*
 *	Representation of the state of the game
*/
typedef struct {
	unsigned	row:5;	/* 00-23 */
	unsigned	col:7;	/* 00-79 */
	unsigned	obj:2;	/* below */
} piece;
#define	PLAYER	0
#define ROBOT	1
#define HEAP	2

/*
 *	The state of the game is essentially a dynamically allocated list of
 *	objects and some general information.
*/
typedef struct {
	int allocated;		/* Number of pieces allocated	*/
	int count;			/* Number of pieces in use		*/
	piece *pieces;		/* Pointer to them				*/
	int ranking;		/* How good is this state?		*/
} board;
