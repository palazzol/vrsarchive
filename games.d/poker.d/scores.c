/*
**	Maintain players' cash from one play to the next.
**	Money is stored according to NAME. Thus if a player plays on different
**	systems, he's still playing with the same cash. Also note that if a
**	player changes his name, he starts anew with START_CASH.
**	Userid is also now included, so that someone can not "steal" another
**	player's cash by usurping his name temporarily.
*/

# include	"scores.h"
# include	<stdio.h>
# ifdef MASSCOMP
#	include		<sys/types.h>
	char		*strchr();
# else
# 	include		<strings.h>
# endif
# ifdef MASSCOMP
#	include		<fcntl.h>
# else
# 	include		<sys/file.h>
# endif

# define	SCORE_FILE	GAMLIB/pokerscores"
# define	TRUE		1
# define	FALSE		0

extern void free();
extern long lseek();
extern char *malloc();

static	int	scorefd=(-1);	/* file channel for scores	*/
static	char	temp[81];

played_before( name, id )

char	*name;		/* player's name to search for	*/
int	id;

{
int	found=FALSE;
char	*p;

if ( scorefd == -1 )
	if ( ( scorefd = open( SCORE_FILE, O_RDWR, 0 ) ) == -1 )
		{
		perror( SCORE_FILE );
		return( FALSE );
		}
lseek( scorefd, 0L, 0 );
while( read( scorefd, temp, 80 ) == 80 )
	if ( strncmp( temp, name, (p = strchr( temp, ':' )) - temp ) == 0 && atoi( p + 1 ) == id )
		{
		found = TRUE;
		break;
		}
return( found );
}


get_cash()

{
return( atoi( strchr( strchr( temp, ':' ) + 1, ':' ) + 1 ) );
}


void	put_cash( name, id, cash )

char	*name;		/* player's name	*/
int	id;		/* player's userid	*/
long	cash;		/* how much dough he had*/

{
int	found=FALSE;
char	*p;
char	idstr[10];

lseek( scorefd, 0L, 0 );
while( read( scorefd, temp, 80 ) == 80 )
	if ( strncmp( temp, name, (p = strchr( temp, ':' )) - temp ) == 0 && atoi( p + 1 ) == id )
		{
		found = TRUE;
		break;
		}
if ( found )
	lseek( scorefd, -80L, 1 );
sprintf( idstr, "%d", id );
sprintf( temp, "%s:%s:%-*d\n", name, idstr, 77 - strlen( name ) - strlen( idstr) , cash );
write( scorefd, temp, 80 );
}

void	high_score_list( s )

int	s;		/* player's socket	*/

{
struct	stuff	{
	char	name[30];
	int	score;
	struct	stuff	*next;
	} *head, *ptr, *tempptr, *last;
int	l;
int	num;
int	score;
char	name[30];
char	output[80];

head = NULL;
lseek( scorefd, 0L, 0 );
writeln( s, "HIGH SCORE LIST" );
writeln( s, "=====================================" );
while( read( scorefd, temp, 80 ) == 80 )
	{
	strncpy( name, temp, l = ( strchr( temp, ':' ) - temp ) );
	name[l] = NULL;
	score = get_cash();
	ptr = head;
	last = NULL;
	while( ptr != NULL && ptr->score > score )
		{
		last = ptr;
		ptr = ptr->next;
		}
	if ( head == NULL )
		{
		head = (struct stuff *) malloc( sizeof(struct stuff) );
		ptr = head;
		ptr->next = NULL;
		}
	else if ( ptr == head )
		{
		head = (struct stuff *) malloc( sizeof(struct stuff) );
		head->next = ptr;
		ptr = head;
		}
	else
		{
		tempptr = ptr;
		ptr = last;
		ptr->next = (struct stuff *) malloc( sizeof(struct stuff) );
		ptr->next->next = tempptr;
		ptr = ptr->next;
		}
	strcpy( ptr->name, name );
	ptr->score = score;
	}
num = 0;
ptr = head;
while( num++ < 10 && ptr != NULL )
	{
	sprintf( output, "%6d %-30s", ptr->score, ptr->name );
	writeln( s, output );
	ptr = ptr->next;
	}
writeln( s, ":" );	/* tell client that we are truly finished */
ptr = head;
while( ptr != NULL )
	{
	tempptr = ptr->next;
	free( (char *)ptr );
	ptr = tempptr;
	}
}

# ifdef MASSCOMP	/* should be in library somewhere, but where? */

char	*strchr( str, ch )

char	*str;		/* string to search in */
char	ch;		/* character to look for */

{
while( *str != NULL && *str != ch )
	str++;
return( (*str == NULL) ? NULL : str );
}

# endif
