
/*
** Basic card playing routines like shuffling, etc.
*/

# include	"cards.h"
# define	ERROR	-1

char	ranks[]="23456789TJQKA";
char	suits[]="DCHS";

void	shuffle (deck)
int	deck[];		/* return deck of shuffled cards here */
{ int	temp[52];
  int	i,x;
  int	crandom();

  for(i=0; i<52; i++)
    temp[i] = i;
#ifdef oldway
  for(i=0; i<52; i++) {
    while (temp[x = crandom(52)] == ERROR) ;
    deck[i] = temp[x];
    temp[x] = ERROR;
  }
#else
  /*
   *	This version of shuffle does the same thing as the old way, but
   *	is guaranteed to terminate in reasonable (constant) time.  The old
   *	way only finished quickly with a high probability, and was observed
   *	to take hours with some random number generators.
  */
  for (i = 51; i >= 0; i--) {
    x = crandom(i+1);		/* Pick a random unused card	*/
    deck[i] = temp[x];		/* Place in output deck		*/
    temp[x] = temp[i];		/* Delete card from list	*/
  }
#endif
}

int	crandom( limit )

int	limit;

{
# ifdef SYS5
# define	random	rand
# endif
#define MAXRAND (((unsigned)-1)>>1)
unsigned long	random();
unsigned long	scale;
int		x;

scale = (unsigned long) MAXRAND/limit;
x = random()/scale;
if ( x < 0 ) x = 0;	/* weird things do happen occasionally */
return(x);
}
