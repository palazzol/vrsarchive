# include	<stdio.h>
# include 	<ctype.h>

# define	WORDLN	100 
# define	LNSIZE	0x400
# define	BIGNUMBER	"1000"
# define	EOLN	'\n'
# define	EOSTR	'\0'
# define	BLANK	' '
# define	TRUE	1
# define	FALSE	0

# define	QUERY(z)	( \
				(z=='!')|| \
				(z=='?')|| \
				(z=='h')|| \
				(z=='l')|| \
				(z=='s')|| \
				(z=='c')   \
		)
# define	COMMAND(z)	( \
				(z=='q')|| \
				(z=='x')|| \
				(z=='r')|| \
				(z=='a')|| \
				(z=='-')|| \
				(z=='\n')  \
		)
# define	ISOPTION(z)	( COMMAND( z )||QUERY( z ) )

# define	BADSYNTAX(z)	( (z!=' ')&&(z!='\n') )
# define	WHITESPACE(z)	( (z==' ')||(z=='\t') )

# define	ISUPPER( c )	( ( c >= 'a' ) && ( c <= 'z' ) )
# define	ISLOWER( c )	( ( c >= 'A' ) && ( c <= 'Z' ) ) 
# define	ISDIGIT( c )	( ( c >= '0' ) && ( c <= '9' ) )
# define	ALPHANUM( c )	( ISLOWER( c ) || ISUPPER( c ) || ISDIGIT( c ) )
# define	APOSTROPHE( c )	( ( *c == '\'' )&&( ALPHANUM( *(c+1) ) ) )
# define	DOT( c )	( ( *c == '.'  )&&( ALPHANUM( *(c+1) ) ) )
# define	TOKENPART(s)	( ALPHANUM(*s)||APOSTROPHE(s)||DOT(s) )

# define	DEBUG(l, f, s)	if (Debug >= l) fprintf(stderr, f, s)
# define	GREATEST(a, b)	( a > b ? a : b )
# define	OPTION(a, b)	( *a == 0 ? b : a )
# define	OPEN(i, f, q)	if  ( (i=fopen(f, q)) == NULL) { \
					printf( "cannot open file: %s\n", f ); \
					exit( 1 ); \
				} else \
					DEBUG(5, "opened: %s, fid: %d\n", f)

# define	 NEW( t )	( t 	*)malloc( sizeof( t ) )

# define	 HASHSIZE	0x100

typedef	struct	xchg {
	char		*e_target;
	char		*e_replace;
	struct	xchg	*e_next;
	} ENTRY;

ENTRY	*table[ HASHSIZE ];
