/*
 * Memory initialilzation, shark and fish initialization, first draw
 * of the screen, and random number generator.
 */

#include <stdio.h>
#include "wator.h"

extern struct item *get_item();

/*
 * init_free()
 *
 * This malloc's memory for enough items to fill the screen.
 *
 */

init_free()
{
	register int i;
	register struct item *ip;

	ip = (struct item *) malloc((unsigned) sizeof(struct item));
	if (ip == (struct item *) -1) {
		perror("init_free");
		exit(1);
	}

	ip->prev = NULL_PTR;
	free_list = ip;

	for (i=0; i < wide*(lines-1) + 4; i++) {
		ip->next = (struct item *) malloc((unsigned) sizeof(struct item));
		if (ip->next == (struct item *) -1) {
			perror("init_free");
			exit(1);
		}
		ip->next->prev = ip;
		ip = ip->next;
	}

	ip->next = NULL_PTR;

}

/*
 * init_screen()
 *
 * Set all cells to EMPTY.  The NULL_PTR is nice for debugging print outs.
 */

init_screen()
{
	register int i, j;

	for (i=0; i < wide; i++)
		for (j=0; j < lines-1; j++) {
			screen[i][j].item_type = EMPTY;
			screen[i][j].item_ptr = NULL_PTR;
		}
}

/*
 * seed_fish()
 *
 * Random locations for fish are chosen.  Screen is initialized.
 * Linked list build for fish.
 */

seed_fish()
{
	register int i;
	register struct item *ip;
	register int x, y;

	for(i=0; i < num_fish; i++) {
again:		x = r_num(0, wide-1);
		y = r_num(0, lines-2);
		if (screen[x][y].item_type != EMPTY)
			goto again;
		ip = get_item();
		ip->xloc = x;
		ip->yloc = y;
		ip->age = r_num(0, f_breed);
		screen[x][y].item_type = FISH;
		screen[x][y].item_ptr = ip;
		put_in(ip, &active_fish);
	}
}

/*
 * seed_shark()
 *
 * Random locations for sharks are chosen.  Screen is initialized.
 * Linked list build for sharks.
 */

seed_shark()
{
	register int i;
	register struct item *ip;
	register int x, y;

	for(i=0; i < num_shark; i++) {
again:		x = r_num(0, wide-1);
		y = r_num(0, lines-2);
		if (screen[x][y].item_type != EMPTY)
			goto again;
		ip = get_item();
		ip->xloc = x;
		ip->yloc = y;
		ip->age = r_num(0, s_breed);
		ip->eat = r_num(0, starve);
		screen[x][y].item_type = SHARK;
		screen[x][y].item_ptr = ip;
		put_in(ip, &active_shark);
	}
}


/*
 * r_num(low, high)
 *
 * A kind of klutzy random number generator for integers
 * in a range: low <= r_num <= high.
 */

#ifdef FOUR2
extern long random();
#endif

r_num(low, high)
	int low, high;
{
	unsigned long rval;
	double fract, l, h, v;
	int num;

again:	;

#ifdef FOUR2
	rval = (long) random();
#else
	rval = rand();
#endif

	rval &= 0xffff;
	fract = (double) rval / 65536.0;

	l = (double) low;
	h = (double) high;
	v = ((h - l) * fract) + l + 0.5;

	num = (int) v;

	if ((num < low) || (num > high))	/* Paranoia -- no failures */
		goto again;			/* in 1000 or so tries */

	return(num);
}

/*
 * init()
 *
 * Set up seed for random number generator. If using the 4.2 random
 * number generator we have to build the state array.
 * We also initialize the head pointers for the linked list.
 */

init()
{
	seed = getpid();
#ifdef FOUR2
	(void) initstate((unsigned) seed, state, sizeof(state));
#else
	(void) srand(seed);
#endif

	active_shark = NULL_PTR;
	active_fish = NULL_PTR;
}
