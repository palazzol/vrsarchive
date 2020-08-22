/*
** rip.c -	code dealing with the end of the game
**
**	[pm by Peter Costantinidis, Jr. @ University of California at Davis]
*/

#include "pm.h"

/*
** die()	- pm has died forever...
*/
void	die (mon)
reg	char	mon;
{
	clear();
	tombstone(thescore, mon);
	printw("[Press return to continue]");
	echo();
	nocrmode();
	getch();
	scores(mon, FL_DIE);
	quit_it();
}

/*
** quitit()	- called when they quit
*/
void	quitit ()
{
	clear();
	move(LINES - 1, 0);
	printw("[Press return to continue]");
	draw();
	echo();
	nocrmode();
	getch();
	scores(NULL, FL_QUIT);
	quit_it();
}

/*
** tombstone()	- print a pretty little pm
*/
static	char	*stone[] =
{	"                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
	"                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
	"                   @@@@@@                            @@@@@",
	"                  @@@@@          @@@@@@@@@@@@@         @@@@",
	"                 @@@@@        @@@@@@@@@@@@@@@@@@@       @@@@",
	"                @@@@@       @@@@@            @@@@@      @@@@",
	"               @@@@@        @@@@   Eaten by    @@@      @@@@@",
	"               @@@@@        @@@@               @@@      @@@@@",
	"              @@@@@         @@@                @@@      @@@@",
	"              @@@@         @@@@                @@@      @@@@",
	"             @@@@@          @@@                @@@      @@@@",
	"             @@@@@          @@@@               @@@     @@@@@",
	"             @@@@@          @@@@               @@@    @@@@@",
	"              @@@@          @@@@@            @@@@@ @@@@@@",
	"              @@@@@           @@@@@@@@@@@@@@@@@@@@@@@@",
	"               @@@@@              @@@@@@@@@@@@  @@@@",
	"               @@@@@",
	"                @@@@@",
	"                 @@@@@",
	"                  @@@@@",
	"                   @@@@@@",
	"                    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
	"                      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
	0
};
void	tombstone (scr, monster)
long	scr;
reg	char	monster;
{
	reg	char	**s = stone;

	clear();
	move(0, 0);
	while (*s)
		printw("%s\n", *s++);
	move(9, 33);
	printw("%9s", mons_str(monster));
	move(18, 30);
	printw("After getting %ld points.", scr);
	move(LINES - 1, 0);
	draw();
}
