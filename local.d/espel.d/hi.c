# include "espel.h"
# define	LOOPING	1

int	count;
int	quit;		/* flag for premature exit of session */
int	update;		/* flag for exit with or without update */
int	notlast;	/* flag for keeping count right at EOF */

char	item	[ WORDLN ];
char	choice	[ WORDLN ];
char	command	[ WORDLN ];
char	temp	[ WORDLN ];
char	*argumentof();

extern	int	Debug;
extern	char	sourcelist[WORDLN];
extern	char	*misslist;
extern	FILE	*scratch;
extern	FILE	*misspellings;
extern	FILE	*additions;

interact()
{
	quit = FALSE;
	update = FALSE;
	count = 0;	/* for keep our place in the spelling list */

	DEBUG(9, "starting interactive session\n", "" );

	/*
 	 * prompt user until eof misspellings or quit is selected
	 */
	while ( !quit ) {
		getitem();
		while( promptfor( choice ) == LOOPING )
			inquire( choice, argumentof(choice) );
		do_command( choice, argumentof(choice) );
	}
	return( update );
}

/*
 * getitem
 *	gets the next 'misspelled' word from the misspellings file,
 *	terminates it with endofstring and updates the count.
 */

getitem()
{
	if ( (fgets( item, WORDLN, misspellings ) > 0 ) ) {
		item[ strlen( item ) - 1 ] = '\0';
		count++;
		notlast = 1;
		DEBUG( 7, "count = %d\n", count );
	} else {
		count += notlast;
		notlast = 0;
		strcpy( item, "EOF" );
	}
}

/*
 * promptfor
 *	reads the user input, seperates argument from option
 *	and determines if the option is a query or a command
 */
promptfor( input )
	char	*input;
{
	register int	valid;

	for ( valid=1;; valid-- )
	{
		if ( valid < 0 )
			printf( "to get help type h\n" );

		printf( "%s > ", item );
		fgets( input, WORDLN, stdin );

		if (strlen(input)<=1) /* indicates a carriage return */
			break;
		else if ( BADSYNTAX( input[1] ) )
			printf( "leave a space after the option\n" );
		else if ( !ISOPTION( input[0] ) )
			printf( "unkown option\n" );
		else
			break;
	}
	DEBUG(9, "option is a command: %d\n", COMMAND(input[0]));
	return( !COMMAND( input[0] ) );
}

/*
 * argumentof
 *	takes a char pointer, advances it along the string past any
 *	white space and replaces the end of line will a string terminator.
 *	It returns a pointer to the first non whitespace char.
 */

char	*
argumentof( cp )
	register	char	*cp;
{
	int	l;

	cp++;
	while ( WHITESPACE( *cp ) )
		cp++;

	l = strlen(cp);
	cp[ ( l>0? l-1 : l ) ] = EOSTR;

	return( cp );
}

inquire( option, arg )
	char	*option;
	char	*arg;
{
	int	i, dist;
	long	offset;

	DEBUG(9,"inquire option: %c\n", *option );
	switch ( *option )
	{
	case '?':
	case 'h':
		sprintf( command, "cat %s/espel.help", LIB );
		system( command );
		break;
	case '!':
		sprintf( command, "%s\n", arg );
		system( command );
		break;
	case 's':
		sprintf( command, "look %s\n", arg );
		system( command );
		break;
	case 'l':
		offset = ftell( misspellings );
		dist = atoi( OPTION(arg, BIGNUMBER) );
		for ( i=1; i<=dist; i++ ) {
			if ( fgets( temp, WORDLN, misspellings ) <= 0 )
				break;
			printf("%s", temp);
		}
		fseek( misspellings, offset, 0 );
		break;
	case 'c':
		minigrep( OPTION( arg, item ), sourcelist );
		break;
	default:
		break;
	}
}

do_command( option, arg )
	char	*option;
	char	*arg;
{
	int	offset;
	int	dist;

	switch ( *option )
	{
	case 'q':
		quit = TRUE;
		break;
	case 'x':
		update = FALSE;
		quit = TRUE;
		break;
	case 'a':
		/*
		if ( *argument != EOSTR )
			sprintf( item, "%s", arg ); 
			*/
		fprintf( additions,"%s\n", OPTION(arg, item) );
		break;
	case 'r':
		hashin( item, arg );
		update = TRUE;
		break;
	case '-':
		dist = GREATEST( atoi(arg), 1 );
		offset = GREATEST( count - dist, 0 ) - 1;
		DEBUG(5, "offset = %d\n", offset );
		rewind(misspellings);
		for (count=0; count<offset;count++)
			if ( fgets( item, WORDLN, misspellings ) <= 0 )
				break;
		break;
	default :
		break;
	}
}
