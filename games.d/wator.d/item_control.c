/*
 * item_control.c  -- misc routines to manipulate item structures.
 */

#include <stdio.h>
#include "wator.h"

/* struct item *get_item()
 *
 * This pulls an item from the free list and returns a pointer to it.
 * Environment params like age, and eat are initialized since they
 * would be anyway.  Ponters set to null for nice print out if debugging.
 */

struct item *
get_item()
{
	register struct item *ip;

	ip = free_list;
	free_list = ip->next;
	free_list->prev = NULL_PTR;
	ip->next = NULL_PTR;
	ip->prev = NULL_PTR;
	ip->age = ip->eat = ip->xloc = ip->yloc = 0;
	if (free_list == NULL_PTR) {
		printf("\nget_item -- No more items.\n");
		exit(1);
	}
	return(ip);
}

/*
 * put_in(ip, list)
 *     struct item *ip;
 *     struct item **list;
 *
 * Insert an item AT THE HEAD OF A LIST.
 * This is called passing a pointer to an item, and a pointer to
 * one of the three lists we maintain.  This puts this item at the
 * head of the list.  Call as:
 *
 *     struct item *np;
 *
 *     np = get_item;
 *     put_in(np, &active_fish);
 */

put_in(ip, list)
	struct item *ip;
	struct item **list;
{
	if (*list == NULL_PTR) {
		*list = ip;
		ip->next = NULL_PTR;
		ip->prev = NULL_PTR;
	} else {
		ip->next = *list;
		(*list)->prev = ip;
		*list = ip;
		(*list)->prev = NULL_PTR;
	}
}

/*
 * fill_adj(x, y, adj, ax, ay, num_type, type)
 *     int x, y;
 *     struct item *adj[];
 *     int ax[], ay[];
 *     int *num_type;
 *     int type;
 *
 * This looks in surrounding cells for a type of item specified.
 * This is used each fish cycle to find an empty cell for a move.
 * In the shark cycle we look for adjacent fish.  If none are found
 * This will be called again looking for empty cells for a move.
 *
 *     x, y       -- cell location that carries out the search
 *     *adj[]     -- return pointers to items in adjacent cells
 *     ax[], ay[] -- list of the adjacent locations of interest
 *     *num_type  -- the number of items found, 0 if none
 *     type       -- the type of item we are looking for
 */

fill_adj(x, y, adj, ax, ay, num_type, type)
	int x, y;
	struct item *adj[];
	int ax[], ay[];
	int *num_type;
	int type;
{
	register int count = 0;
	register int nx, ny;
	register int i;

	if ((x < 0 || x > wide-1) || (y < 0 || y > lines-2)) {
		printf("\nfill_adj - botched, loc= (%d,%d)\n", x, y);
		exit(1);
	}

	for (i=0; i < 8; i++)
		ax[i] = ay[i] = 0;

	/*
	 * First check is 4 cells like so:
	 *
	 *            X        X
	 *            X  check X
	 *     XXXXXXXXXXXXXXXXXXXXXXXXX
	 *      check X  item  X check
	 *     XXXXXXXXXXXXXXXXXXXXXXXXX
	 *            X  check X
	 *            X        X
	 *
	 * If variable CHECK8 is defined we will later check remaining
	 * cells to cover all 8.
	 */

	/*
	 * Cell BELOW this one.
	 */

	nx = x;
	ny = y+1;
	if (ny > lines-2)
		ny = 0;
	if (screen[nx][ny].item_type == type) {
		adj[count] = screen[nx][ny].item_ptr;
		ax[count] = nx;
		ay[count++] = ny;
	}

	/*
	 * Cell ABOVE this one.
	 */

	ny = y-1;
	if (ny < 0)
		ny = lines-2;
	if (screen[nx][ny].item_type == type) {
		adj[count] = screen[nx][ny].item_ptr;
		ax[count] = nx;
		ay[count++] = ny;
	}

	/*
	 * Cell to the RIGHT of this one.
	 */

	nx = x+1;
	ny = y;
	if (nx > wide-1)
		nx = 0;
	if (screen[nx][ny].item_type == type) {
		adj[count] = screen[nx][ny].item_ptr;
		ax[count] = nx;
		ay[count++] = ny;
	}

	/*
	 * Cell to the LEFT of this one.
	 */

	nx = x-1;
	if (nx < 0)
		nx = wide-1;
	if (screen[nx][ny].item_type == type) {
		adj[count] = screen[nx][ny].item_ptr;
		ax[count] = nx;
		ay[count++] = ny;
	}

	/*
	 * Here we can check another set of "surrounding" cells
	 * like so:
	 *
	 *            X        X
	 *      check X        X check
	 *     XXXXXXXXXXXXXXXXXXXXXXXXX
	 *            X  item  X
	 *     XXXXXXXXXXXXXXXXXXXXXXXXX
	 *      check X        X check
	 *            X        X
	 */

#ifdef CHECK8

	/*
	 * Cell BELOW and LEFT.
	 */

	nx = x-1;
	if (nx < 0)
		nx = wide-1;
	ny = y+1;
	if (ny > lines-2)
		ny = 0;
	if (screen[nx][ny].item_type == type) {
		adj[count] = screen[nx][ny].item_ptr;
		ax[count] = nx;
		ay[count++] = ny;
	}
	/*
	 * Cell BELOW and RIGHT.
	 */

	nx = x+1;
	if (nx > wide-1)
		nx = 0;
	if (screen[nx][ny].item_type == type) {
		adj[count] = screen[nx][ny].item_ptr;
		ax[count] = nx;
		ay[count++] = ny;
	}
	/*
	 * Cell ABOVE and LEFT.
	 */

	nx = x-1;
	if (nx < 0)
		nx = wide-1;
	ny = y-1;
	if (ny < 0)
		ny = lines-2;
	if (screen[nx][ny].item_type == type) {
		adj[count] = screen[nx][ny].item_ptr;
		ax[count] = nx;
		ay[count++] = ny;
	}
	/*
	 * Cell ABOVE and RIGHT.
	 */

	nx = x+1;
	if (nx > wide-1)
		nx = 0;
	if (screen[nx][ny].item_type == type) {
		adj[count] = screen[nx][ny].item_ptr;
		ax[count] = nx;
		ay[count++] = ny;
	}

#endif CHECK8

	*num_type = count;
}

/*
 * show_item(ip)
 *
 * Useful routine for dumpping the linked list structure.
 * Can be called from "fish_cycle" or "shark_cycle" as:
 *    show_item(active_fish);
 *
 * It may be desirable to hard set num_fish and num_shark to a number
 * of 10 or so when printing this information during debugging.
 */

show_item(ip)
	struct item *ip;
{
	int i= 1;

	if (!stat)
		return;

	do {
		fprintf(stat, "item[%d] @ 0x%x, loc (%2d,%2d), age= %2d <p=0x%x,n=0x%x>\n",
		    i, ip, ip->xloc, ip->yloc, ip->age, ip->prev, ip->next);
		i++;
		ip = ip->next;
	} while (ip != NULL_PTR);
}
