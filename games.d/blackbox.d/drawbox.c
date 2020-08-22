/* DRAWBOX.C: draw routines for the Black Box Game */

#include <curses.h>
#define M move
#define P printw

draw_board()
{
M( 0,0);P("     32    31    30    29    28    27    26    25            BLACK BOX         ");
M( 1,0);P("                                                               X 101           ");
M( 2,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|                           ");
M( 3,0);P("1  |     |     |     |     |     |     |     |     |  24  Commands:            ");
M( 4,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|       x   = Send a probe  ");
M( 5,0);P("2  |     |     |     |     |     |     |     |     |  23   x,y = Re/Set a guess");
M( 6,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|       @   = Tally & Exit  ");
M( 7,0);P("3  |     |     |     |     |     |     |     |     |  22   Q   = Quit          ");
M( 8,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|                           ");
M( 9,0);P("4  |     |     |     |     |     |     |     |     |  21  Display:             ");
M(10,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|       ?   = Absorbed      ");
M(11,0);P("5  |     |     |     |     |     |     |     |     |  20   !   = Reflected     ");
M(12,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|       *   = Ball          ");
M(13,0);P("6  |     |     |     |     |     |     |     |     |  19   ( ) = Guess         ");
M(14,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|                           ");
M(15,0);P("7  |     |     |     |     |     |     |     |     |  18  Probes:              ");
M(16,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|                           ");
M(17,0);P("8  |     |     |     |     |     |     |     |     |  17  Score last game:     ");
M(18,0);P("   |-----|-----|-----|-----|-----|-----|-----|-----|      Games so far:        ");
M(19,0);P("                                                          Average:             ");
M(20,0);P("      9    10    11    12    13    14    15    16                              ");
}

instructions()
{
char dummy;

clear();

M( 0,0);P("THE BLACK BOX GAME: INSTRUCTIONS");

M( 2,0);P("The Black Box is an 8 X 8 array of squares.  Hidden somewhere inside are 5");
M( 3,0);P("\"balls\", distributed at random.  The purpose of the game is to determine where");
M( 4,0);P("the balls are hidden.");

M( 6,0);P("To find the balls, try rolling \"probes\" into the Black Box and watch where the");
M( 7,0);P("probes roll back out.  Each probe is assigned a unique letter so that you can");
M( 8,0);P("determine the entry and exit points of a given probe.");

M(10,0);P("Guesses as to the location of a ball may be placed during the game by");
M(11,0);P("specifying any two entry points that uniquely locate a square in the array.  A");
M(12,0);P("guess may be removed by specifying the same coordinates.  At tally time there");
M(13,0);P("must be exactly 5 guesses placed.");

M(15,0);P("The object of the game is to minimize your score.  You are given 1 point for");
M(16,0);P("each probe used, and 5 points for each incorrect guess.");

M(18,0);P("   Press any key to continue... ");

refresh();
dummy = getch();
clear();

M( 0,0);P("The rules that determine the path of a probe are:");

M( 2,0);P("1. If a probe tries to pass by a ball, it is deflected 90 degrees away from the");
M( 3,0);P("   ball.");

M( 5,0);P("2. If a probe hits a ball head on, it is absorbed and a ? will print instead of");
M( 6,0);P("   a letter at the probe entry point.");

M( 8,0);P("3. If a probe tries to pass between two balls, it is reflected back the way it");
M( 9,0);P("   approached.  Instead of a letter a ! will print at the entry point.");

M(11,0);P("4. If the probe tries to pass by a ball at the edge of the array, it is");
M(12,0);P("   reflected.  A ! will print instead of a letter at the entry point.");

M(14,0);P("5. Absorption has priority over reflection or deflection.");

M(18,0);P("   Press any key to start the game... ");

refresh();
dummy = getch();
}
