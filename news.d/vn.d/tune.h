/*
** vn news reader.
**
** tune.h - system tuning parameters
**
** see copyright disclaimer / history in vn.c source file
*/

/*
**	buffer size needed for tmpnam()
*/
#ifndef L_tmpnam
#define L_tmpnam 48
#endif

/*
** hash table size.  linked list type of table which can expand to
** arbitrary density, including densities > 100%.  Number of entries
** will be number of newsgroups in active list.  This should be a prime
** number ("long division" of string modulo table size hash function).
*/
#define HASHSIZE 809

/*
**	maximum number of columns on terminal.  If made smaller, there
**	will be a savings in the size of the temporary file used
**	for holding displays, at the penalty of not being able to use
**	the entire screen width on terminals actually possessing more
**	columns than this.  A block roughly on the order of this value
**	times the number of lines the terminal has is maintained per page in
**	the temp file, and read / written as displays are interacted
**	with.  MIN_C put here because MAX_C > MIN_C.  MIN_C is the minumum
**	number of columns for which a "reasonable" display can be produced.
**	before making it smaller, look at all uses of C_allow and variable
**	to see that a setting that small won't screw up array bounds.
*/
#define MAX_C 132
#define MIN_C 36

/*
**	large size for general purpose local buffers.  only used in automatic
**	variable declarations.  Used with fgets for buffer size when reading
**	file records, to hold pathnames, commands, etc.  Reduce if you blow
**	out stack storage.  If reduced too far, may eventually show up
**	as syntax errors on interacting with vns_ routines, or command line
**	botches.
*/
#define RECLEN 1200

/* block sizes for allocation routines */
#define STRBLKSIZE 1800	/* string storage allocation block */
#define NDBLKSIZE 50	/* NODE structures to allocate at a time */

/*
** maximum number of articles to allow for processing in a single user
** list.  Used only to declare an array of pointers on the stack, so it
** can be fair sized without much problem.  In practicality, there is
** no use for it being larger than the greatest line length available
** on the CRT's being used.
*/
#define MAXARTLIST 200
