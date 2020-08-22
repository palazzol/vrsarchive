#include "espel.h"

extern	int	Debug;
extern	ENTRY	*table[ HASHSIZE ];

char	in_line[ LNSIZE ];
char	outline[ LNSIZE ];

FILE	*infile;
FILE	*outfile;

correct( source, scratch )
	char	*source;
	char	*scratch;
{
	/*
	 * Copy the file a line at a time to a temp file
	 *	correcting any words from the hash table
	 */
	DEBUG(4,"correcting file: %s\n", source );

	OPEN( infile, source, "r" );
	OPEN( outfile, scratch, "w" );

	while ( fgets( in_line, LNSIZE, infile ) != NULL )
	{
		exhange( in_line, outline );
		fprintf( outfile, "%s\n", outline );
	}

	fclose( infile );
	fclose( outfile );
	/*
	 * Now copy the temp file back into the original
	 */
	 if ( copy( scratch, source ) == 1 )
		printf( "copy unsuccessfull;file untouched\n" );
}



exhange( src, trg )
	register	char	*src;
	register	char	*trg;
{
	char	token[ WORDLN ];
	register	char	*c; 

	/*
	 * copy over any leading delimitors
	 */

	while ( !ALPHANUM( *src ) && ( *src != EOLN ) )
		*trg++ = *src++;

	while ( *src != EOLN )
	{
		/*
		 * tokenize the next word
		 */

		c = token;
		while ( TOKENPART( src ) )
			*c++ = *src++ ;
		*c = EOSTR;

		/*
		 * see if its in the hashtable
		 */

		lookup_and_replace( token );

		/*
		 * copy the token into the output
		 */

		c = token;
		while ( *c != EOSTR )
			*trg++ = *c++;

		/*
		 * now copy input to output until end of line or next token
		 */

		while ( !ALPHANUM( *src ) && ( *src != EOLN ) )
			*trg++ = *src++;
	}
	
	*trg = EOSTR;
}

lookup_and_replace( t )
	char	*t;
{
	register	ENTRY	*p;
	
	for( p = table[ in_dex( t ) ]; p != NULL; p = p->e_next )
		if ( strcmp( p->e_target, t ) == 0 )
			/*
			 * if t in table then replace
			 * with replacement e_replace
			 */
			if ( strlen(in_line) + strlen(p->e_replace) < LNSIZE) {
				strcpy( t, p->e_replace );
				break;
			}
}
