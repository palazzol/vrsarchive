/*
 * WATOR -- Sharks and fish wage an ecological war on the
 *          toroidal planet Wa-Tor.
 *
 * ------------------------------------------------------------------
 * A computer recreation.  See the December 1984 Scientific American.
 * Volume 251, Number 6, page 14.
 * ------------------------------------------------------------------
 * This program written by Dale Mosby of Sequent Computer Systems, Inc.
 * 14360 NW Science Park Drive,  Portland, Oregon 97229 (503) 626-5700.
 * Permission for not for profit use and distribution is hereby granted
 * providing this acknowledgment is included.  No warranties implied!
 * ------------------------------------------------------------------
 *
 * wator [-hs] #sharks sbreed #fish fbreed starve
 *
 *       -h   help info
 *       -s    generate statistic log file wator.stat
 *
 * See "hlp_msg" at end of this file for information on parameters.  This
 * program displays a flat map of the world.  If you move off one edge you
 * come back on the other.  Sharks and fish are randomly placed on the world.
 * Time proceeds in cycles with fish moving randomly to an adjacent unoccupied
 * cell.  Fish breed after some number of cycles, creating more fish.
 * Each cycle the sharks will choose one adjacent fish and eat it.  Sharks
 * also breed after some number of cycles (not necessarily the same number
 * as for fish).  If sharks find no adjacent fish they move randomly as do
 * fish.  (Read the magazine article for a good explanation).
 */

#include <stdio.h>
#include "wator.h"
#ifdef FOUR2
#include <signal.h>
#endif

main(argc, argv)
	int argc;
	char *argv[];
{
#ifdef FOUR2
	struct sigvec vec, ovec;
	int sc();

	vec.sv_handler = sc;
	vec.sv_mask = 0;
	vec.sv_onstack = 0;
	if (sigvec(SIGCONT, &vec, &ovec)) {
		printf("Sigvec(SIGCONT) fails.\n");
		exit(1);
	}
#endif
	init();
	parse_args(argc, argv);
	screen_control();
	init_free();
	init_screen();
	seed_fish();
	seed_shark();
	draw_screen();
	put_stat();

	do {
		generation++;
		fish_cycle();
		shark_cycle();
		put_stat();
	} while ( (num_shark && num_fish) &&
		  (num_shark+num_fish < wide * lines-1) );

	done_msg();
}

#ifdef FOUR2
/*
 * After we get the continue signal, assume screen is messed up so redraw it.
 * If we get clobbered in the middle of some IO there could be a botched cursor
 * control sequence going out.  A second redraw in the middle of the main
 * "while" loop would pretty much guarantee that things were fine.
 */
sc()
{
	draw_screen();
	put_stat();
}
#endif

done_msg()
{
	int i;

	movexy(wide-12, lines-1);
	printf("Hit char");
	i = getchar();
}

char *hlp_msg[]= {
"\n",
"WATOR - sharks and fish wage an ecological war on the toroidal planet Wa-Tor.\n\n",
"Based on the 'Computer Recreations' column from the December 1984 Scientific\n",
"American, Vol 251, Number 6, page 14.  By A. K. Dewdney\n\n",
"With no arguments this program chooses random values and begins.\n",
"Sharks display as 'O', and fish as '.'  Arguments:",
"\n\nwator [-hls] #sharks #fish sbreed fbreed starve\n",
" -h -- this help message\n",
" -s -- generate statistics in a file (default /tmp/wator.stat)\n\n",
"After these options specify 5 numbers:\n",
"   #sharks - initial shark population\n",
"   #fish   - innitial fish population\n",
"   sbreed  - how many generations before sharks breed\n",
"   fbreed  - how many generations before fish breed\n",
"   starve  - how many generations without eating before sharks starve\n\n",
"Example:   'wator 50 250 6 4 5'\n\n",
"Final line gives statistics: generation number, number of sharks, number\n",
"of fish, how often sharks/fish breed, and when sharks starve.\n",
0 };

usage()
{
	char **p;

	p = hlp_msg;
	while (*p) {
		printf(*p);
		p++;
	}
}
