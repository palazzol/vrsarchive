#include "espel.h"

extern	int	Debug;
extern	ENTRY	*table[ HASHSIZE ];

ENTRY	*new_space();
char	*savestr();
char	*malloc();
int	in_dex();


hashtab()
{
	register	int	i;

	/*
	 * initialize the hashtable to nulls
	 */

	for( i=0; i < HASHSIZE; i++ )
		table[ i ] = NULL;
}

/*
 * hashin
 *	takes the target pattern and find the
 *	index into the hashtable, then it will make out a new
 *	entry using the parameters t and r for the feilds and
 *	insert the entry into the table.
 *
 * t: target pattern
 * r: replacement pattern
 *
 */

hashin( t, r )
	char	*t, *r;
{
	ENTRY	*p;

	p = new_space();
	p->e_target = savestr( t );
	p->e_replace = savestr( r );

	p->e_next = table[ in_dex( t ) ];
	table[ in_dex( t ) ] = p;
	DEBUG( 8, "target: %s", t );
	DEBUG( 8, "replac: %s\n", r );
}

/*
 * new_space 
 *	assigns to pointer the address of the next
 *	available memory space for an entry pointer.
 */

ENTRY	*
new_space()
{
	return( ( ENTRY * )malloc( sizeof( ENTRY ) ) );
}

/*
 * savestr
 *	copies the string pointed to by str into
 *	the next available memory space and return a pointer to it.
 */

char *
savestr( str )
	register	char	*str;
{
	register char	*copy, *pointerval;

	copy = pointerval = malloc( strlen(str) + 1 );

	while( ( *copy++ = *str++ ) != EOSTR )
		continue;

	return( pointerval );
}

/*
 * in_dex
 *	computes the index into the hashtable of the parameter str
 */

in_dex( str )
	register	char	*str;
{
	register	int hashval;

	for ( hashval = 0; (*str != BLANK) && (*str != EOSTR);)
		hashval += *str++;
	return( hashval % HASHSIZE );
}

printhashtable()
{
	ENTRY	*p;
	int	i;

	for( i = 0; i< HASHSIZE ; i++ )
		if ( table[ i ] != NULL )
		{
			p = table[ i ];
			while ( p != NULL )
			{
				printf( "\t%s\n", p->e_target );
				printf( "\t%s\n", p->e_replace );
				p = p->e_next;
			}
		}
}
