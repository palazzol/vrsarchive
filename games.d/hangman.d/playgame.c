# include	"hangman.h"

extern char *strchr();
/*
 * playgame:
 *	play a game
 */
playgame()
{
	register bool	*bp;

	getword();
	Errors = 0;
	bp = Guessed;
	while (bp < &Guessed[26])
		*bp++ = FALSE;
	while (Errors < MAXERRS && strchr(Known, '-') != NULL) {
		prword();
		prdata();
		prman();
		getguess();
	}
	endgame();
}
