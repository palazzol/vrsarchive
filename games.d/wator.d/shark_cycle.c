/*
 * shark_cycle()
 *
 * Sharks hunt and breed.
 *
 * For each shark check adjacent cells for fish.  Choose one at random and
 * eat it.  If no fish just move to empty cell.  Age is incremented and
 * if time to breed a new shark is created.  If a fish is not eaten we
 * increment time since eating and check for starvation.
 */

#include <stdio.h>
#include "wator.h"

extern struct item *get_item();

shark_cycle()
{
	register struct item *ip, *np;
	struct item *adj[8];
	register int i, x, y;
	int ax[8], ay[8];
	int num_adj;
	int n_create, n_starve, n_eat, n_held;


	n_create = n_starve = n_eat = n_held = 0;


	for (ip=active_shark; ip != NULL; ) {
		/*
		 * If adjacent fish then we will eat that fish.
		 */
		fill_adj(ip->xloc, ip->yloc, adj, ax, ay, &num_adj, FISH);
		if (num_adj) {
			ip->eat = 0;
			i = r_num(0, num_adj-1);
			np = adj[i];
			if (np == active_fish)
				active_fish = np->next;
			if (np->prev != NULL_PTR)
				np->prev->next = np->next;
			if (np->next != NULL_PTR)
				np->next->prev = np->prev;
			put_in(np, &free_list);
			x = ax[i];
			y = ay[i];
			movexy(x, y);
			putshark();
			screen[x][y].item_type = SHARK;
			num_fish--;
			n_eat++;
			/*
			 * If time to breed, create a new shark at this
			 * cell.  Otherwise just move to cell.
			 */
			if (ip->age >= s_breed) {
				n_create++;
				ip->age = 0;
				num_shark++;
				np = get_item();
				ip->eat = 0;
				np->xloc = x;
				np->yloc = y;
				screen[x][y].item_ptr = np;
				put_in(np, &active_shark);
			} else {
				movexy(ip->xloc, ip->yloc);
				putempty();
				screen[ip->xloc][ip->yloc].item_type = EMPTY;
				screen[ip->xloc][ip->yloc].item_ptr = NULL_PTR;
				screen[x][y].item_ptr = ip;
				ip->xloc = x;
				ip->yloc = y;
				ip->age++;
				if (ip->age > 250)	/* overflow */
					ip->age = 250;
			}
			ip = ip->next;
			continue;
		} 
		/*
		 * Increment age and time since eating.
		 * If we are to starve on this cycle remove this item.
		 */
		ip->age++;
		ip->eat++;
		if (ip->age > 250)
			ip->age = 250;
		if (ip->eat > 250)
			ip->eat = 250;
		if (ip->eat >= starve) {
			screen[ip->xloc][ip->yloc].item_type = EMPTY;
			screen[ip->xloc][ip->yloc].item_ptr = NULL_PTR;
			movexy(ip->xloc, ip->yloc);
			putempty();
			np = ip;
			ip = ip->next;
			if (active_shark == np)
				active_shark = np->next;
			if (np->prev != NULL_PTR)
				np->prev->next = np->next;
			if (np->next != NULL_PTR)
				np->next->prev = np->prev;
			put_in(np, &free_list);
			num_shark--;
			n_starve++;
			continue;
		}
		/*
		 * Look for free space and try and move to another cell.
		 */
		fill_adj(ip->xloc, ip->yloc, adj, ax, ay, &num_adj, EMPTY);
		if (num_adj) {
			i = r_num(0, num_adj-1);
			x = ax[i];
			y = ay[i];
			screen[x][y].item_type = SHARK;
			if (ip->age >= s_breed) {
				ip->age = 0;
				n_create++;
				num_shark++;
				np = get_item();
				np->xloc = x;
				np->yloc = y;
				screen[x][y].item_ptr = np;
				movexy(x,y);
				putshark();
				put_in(np, &active_shark);
			} else {
				screen[ip->xloc][ip->yloc].item_type = EMPTY;
				screen[ip->xloc][ip->yloc].item_ptr = NULL_PTR;
				screen[x][y].item_ptr = ip;
				movexy(ip->xloc, ip->yloc);
				putempty();
				ip->xloc = x;
				ip->yloc = y;
				movexy(x, y);
				putshark();
			}
		} else {
			n_held++;
		}
		ip = ip->next;
	}

	fflush(stdout);

	if (stat) {
		fprintf(stat, "eaten %4d, # %4d, S create %4d, starve %4d, # %4d\n",
		  n_eat, num_fish, n_create, n_starve, num_shark);
		fflush(stat);
	}
}
