#include "espel.h"

extern	int	Debug;
char	inline[ LNSIZE ];

FILE	*infile;

minigrep( word, sourcelist )
	char	*word;
	char	*sourcelist;
{
	char	*c, *t;
	char	filename[ 50 ];

	/*
	 * the following takes a line of words, separated by blanks
	 * and calls search for each name it finds
	 */

	DEBUG(6, "sourcelist %s\n", sourcelist );
	for ( c = sourcelist; *c != EOSTR ; ) {
		t = filename;
		while ( (*c != EOSTR)&&(*c != BLANK) )
			*t++ = *c++;
		*t = EOSTR;
		if ( *c == BLANK )
			c++;
		DEBUG(6, "filename %s\n", filename );
		search( filename, word );
	}
}

search( source, word )
	char	*source;
	char	*word;
{
	OPEN( infile, source, "r" );
	while ( fgets( inline, LNSIZE, infile ) != NULL ) {
		if ( find( word, inline ) )
			printf( "%s", inline);
	}
	fclose( infile );
}

find( target, src )
	register	char	*src;
	register	char	*target;
{
	register	char	*c; 
	char	token[ WORDLN ];

	/*
	 * skip over any leading delimitors
	 */

	while ( !ALPHANUM( *src ) && ( *src != EOLN ) )
		*src++;

	while ( *src != EOLN ) {
		/*
		 * tokenize the next word
		 */

		c = token;
		while ( TOKENPART( src ) )
			*c++ = *src++ ;
		*c = EOSTR;

		if ( strcmp( target, token ) == 0 )
			return( 1 );

		while ( !ALPHANUM( *src ) && ( *src != EOLN ) )
			*src++;
	}
	return( 0 );
}
