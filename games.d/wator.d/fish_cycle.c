/*
 * fish_cycle()
 *
 * This routine is responsible for movement and breeding of the fish.
 * Algorithm:
 *
 * For each fish:
 *  Make a list of the adjacent EMPTY positions.
 *  If there are any, chose one at random and move the fish to that
 *  position.  Increment the age for the fish.  If it can breed put
 *  a new fish at the old position and set the age of both fish to zero.
 *  If no adjacent positions just increment the age of the fish.
 */

#include <stdio.h>
#include "wator.h"

extern struct item *get_item();

fish_cycle()
{
	register struct item *ip, *np;
	struct item *adj[8];
	register int i, x, y;
	int ax[8], ay[8];
	int num_adj, n_create, n_held;

	n_create = n_held = 0;

	ip = active_fish;
	do {
		/*
		 * See if we have any free space around us.
		 */
		fill_adj(ip->xloc, ip->yloc, adj, ax, ay, &num_adj, EMPTY);
		if (num_adj) {
			ip->age++;
			i = r_num(0, num_adj-1);
			x = ax[i];
			y = ay[i];
			/*
			 * If we are going to breed just create a new fish
			 * at the selected position.
			 */
			if (ip->age >= f_breed) {
				n_create++;
				ip->age = 0;
				num_fish++;
				np = get_item();
				np->xloc = x;
				np->yloc = y;
				screen[x][y].item_type = FISH;
				screen[x][y].item_ptr = np;
				movexy(x, y);
				putfish();
				put_in(np, &active_fish);
			} else {
				screen[ip->xloc][ip->yloc].item_type = EMPTY;
				screen[ip->xloc][ip->yloc].item_ptr = NULL_PTR;
				movexy(ip->xloc, ip->yloc);
				putempty();
				ip->xloc = x;
				ip->yloc = y;
				screen[x][y].item_type = FISH;
				screen[x][y].item_ptr = ip;
				movexy(x, y);
				putfish();
			}
		} else {
			n_held++;
			ip->age++;	/* no space, just get older */
		}
		ip = ip->next;
	} while (ip != NULL_PTR);

	fflush(stdout);

	if (stat)
		fprintf(stat, "Gen: %4d, F created %4d ", generation,
		  n_create);
}
