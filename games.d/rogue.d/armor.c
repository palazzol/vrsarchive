/*
 * This file contains misc functions for dealing with armor
 * @(#)armor.c	6.2 (Berkeley) 5/28/84
 */

#include <curses.h>
#include "rogue.h"

/*
 * wear:
 *	The player wants to wear something, so let him/her put it on.
 */
wear()
{
	register THING *obj;
	register char *sp;

	if (cur_armor != NULL)
	{
		msg("You have armor on");
		after = FALSE;
		return;
	}
	if ((obj = get_item("Wear", ARMOR)) == NULL)
		return;
	if (obj->o_type != ARMOR)
	{
		msg("You can't wear that");
		return;
	}
	waste_time();
	obj->o_flags |= ISKNOW;
	sp = inv_name(obj, TRUE);
	cur_armor = obj;
	msg("You are now wearing %s", sp);
}

/*
 * take_off:
 *	Get the armor off of the players back
 */
take_off()
{
	register THING *obj;

	if ((obj = cur_armor) == NULL)
	{
		after = FALSE;
		msg("You aren't wearing armor");
		return;
	}
	if (!dropcheck(cur_armor))
		return;
	cur_armor = NULL;
	msg("Was wearing %c) %s", pack_char(obj), inv_name(obj, TRUE));
}

/*
 * waste_time:
 *	Do nothing but let other things happen
 */
waste_time()
{
	do_daemons(BEFORE);
	do_fuses(BEFORE);
	do_daemons(AFTER);
	do_fuses(AFTER);
}
