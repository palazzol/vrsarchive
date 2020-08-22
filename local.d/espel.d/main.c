# include "espel.h"
#define	FIRSTFILE	5

extern	ENTRY	*hashtable[ HASHSIZE ];

char	sourcelist[ WORDLN ];	/* name of list of files needing corrections */
char	*misslist;		/* name of list of mispellings */

FILE	*scratch;		/* list of word pairs for correcting files */
FILE	*misspellings;		/* list of mispelled words */
FILE	*additions;		/* list of words for the local dictionary */
int	Debug;

main( argc, argv )
	int	argc;
	char	*argv[];
{
	int	count;
	char	temp[WORDLN];
	int	update;

	if ( argc < 4 ) {
		printf( "useage: %s <list of misspellings> \
		<file for storing additions to local dictionary> \
		<scratch file> <debug level> <documents>", argv[0] );
	exit(1);
	}

	OPEN( misspellings, argv[1], "r" );
	OPEN( additions, argv[2], "w" );

	Debug = GREATEST( atoi( &argv[4][2] ), 1 );
	/*
	 * this is a list for the context option 
	 */
	for( count=FIRSTFILE, *sourcelist = 0; count < argc; count++ ) {
		strcat(sourcelist, argv[count]);
		if ( count < ( argc - 1 ) )
			strcat(sourcelist, " ");
	}
	/*
	 * initialize the hashtable
	 */
	hashtab();
	/*
	 * call the interactive routine for corrections, etc.
	 */
	update = interact();
	if ( update )  /* returns the update flag */
		for ( count=FIRSTFILE ; count <argc ; count++ ) 
			correct( argv[count], argv[3] );
	else
		fseek( additions, 0, 0 );
		
	close( additions );
}
