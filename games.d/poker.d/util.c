/*
** Various utility routines. 
*/

# include	<stdio.h>
# include	"util.h"

extern char *strcpy();

char	*strsave( temp )
char	*temp;
{
char	*p;
extern	char	*malloc();

p = malloc((unsigned)(strlen(temp)+1));
strcpy( p, temp );
return( p );
}

int	readln( s, temp )

int	s;		/* fd to read from	*/
char	*temp;		/* area to read into	*/

{
do {
	if (read( s, temp, 1) < 1)
		return(0);
   }
while( *temp++ != '\n');
*temp = NULL;
return(1);
}

writeln( s, temp )

int	s;		/* channel to write on	*/
char	*temp;		/* what to write	*/

{
write( s, temp, (unsigned)strlen(temp) );
write( s, "\n", 1 );
}
