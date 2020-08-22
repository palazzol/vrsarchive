#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/printships.c,v 1.1 1987-12-25 20:55:17 vrs Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/printships.c,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/printships.c,v 1.1 1987-12-25 20:55:17 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  87/10/09  11:10:37  11:10:37  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: printships.c
 *
 * Print Basic Point Values for all standard ships.
 *
 */

#include <stdio.h>
#include "externs.h"

extern struct ship_stat stats[];

main()
{
	double bpv, efficiency;
	int turn, max_speed;
	int i;
	void calculate();

	for(i=0; i<MAXSHIPCLASS; i++) {
		calculate(
		    stats[i].regen,
		    stats[i].pods,
		    stats[i].ph_shield,
		    stats[i].tp_shield,
		    stats[i].num_phaser + stats[i].num_torp,
		    stats[i].o_crew,
		    &bpv,
		    &efficiency,
		    &turn,
		    &max_speed
		);
		printf("%s:", stats[i].abbr);
		printf("\tBPV =\t\t%.2f\n", bpv);
		printf("\tEfficiency =\t%.2f\n", efficiency);
		printf("\tTurn =\t\t%d\n", turn);
		printf("\tMax speed =\t%d\n\n", max_speed);
	}
}
