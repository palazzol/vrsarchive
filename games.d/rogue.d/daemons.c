/*
 * All the daemon and fuse functions are in here
 *
 * @(#)daemons.c	6.2 (Berkeley) 5/28/84
 */

#include <curses.h>
#include "rogue.h"

/*
 * doctor:
 *	A healing daemon that restors hit points after rest
 */
doctor()
{
	register int lv, ohp;

	lv = pstats.s_lvl;
	ohp = pstats.s_hpt;
	quiet++;
	if (lv < 8)
	{
		if (quiet + (lv << 1) > 20)
			pstats.s_hpt++;
	}
	else
		if (quiet >= 3)
			pstats.s_hpt += rnd(lv - 7) + 1;
	if (ISRING(LEFT, R_REGEN))
		pstats.s_hpt++;
	if (ISRING(RIGHT, R_REGEN))
		pstats.s_hpt++;
	if (ohp != pstats.s_hpt)
	{
		if (pstats.s_hpt > max_hp)
			pstats.s_hpt = max_hp;
		quiet = 0;
	}
}

/*
 * Swander:
 *	Called when it is time to start rolling for wandering monsters
 */
swander()
{
	daemon(rollwand, 0, BEFORE);
}

/*
 * rollwand:
 *	Called to roll to see if a wandering monster starts up
 */
rollwand()
{
	static int between = 0;

	if (++between >= 4)
	{
		if (roll(1, 6) == 4)
		{
			wanderer();
			kill_daemon(rollwand);
			fuse(swander, 0, WANDERTIME, BEFORE);
		}
		between = 0;
	}
}

/*
 * unconfuse:
 *	Release the poor player from his confusion
 */
unconfuse()
{
	player.t_flags &= ~ISHUH;
	msg("You feel less %s now", on(player, ISTRIPY) ? "trippy" : "confused");
}

/*
 * unsee:
 *	Turn off the ability to see invisible
 */
unsee()
{
	register THING *th;

	for (th = mlist; th != NULL; th = next(th))
		if (on(*th, ISINVIS) && see_monst(th))
		{
			move(th->t_pos.y, th->t_pos.x);
			addch(th->t_oldch);
		}
	player.t_flags &= ~CANSEE;
}

/*
 * sight:
 *	He gets his sight back
 */
sight()
{
	if (on(player, ISBLIND))
	{
		extinguish(sight);
		player.t_flags &= ~ISBLIND;
		if (!(proom->r_flags & ISGONE))
			enter_room(&hero);
		if (on(player, ISTRIPY))
			msg("Far out!  Everything is cosmic again");
		else
			msg("The veil of darkness lifts");
	}
}

/*
 * nohaste:
 *	End the hasting
 */
nohaste()
{
	player.t_flags &= ~ISHASTE;
	msg("You feel yourself slowing down");
}

/*
 * stomach:
 *	Digest the hero's food
 */
stomach()
{
	register int oldfood;

	if (food_left <= 0)
	{
		if (food_left-- < -STARVETIME)
			death('s');
		/*
		 * the hero is fainting
		 */
		if (no_command || rnd(5) != 0)
			return;
		no_command += rnd(8) + 4;
		player.t_flags &= ~ISRUN;
		running = FALSE;
		count = 0;
		hungry_state = 3;
		if (on(player, ISTRIPY))
			msg("You freak out");
		else
			msg("You faint");
	}
	else
	{
		oldfood = food_left;
		food_left -= ring_eat(LEFT) + ring_eat(RIGHT) + 1 - amulet;

		if (food_left < MORETIME && oldfood >= MORETIME)
		{
			hungry_state = 2;
			if (on(player, ISTRIPY))
				msg("The munchies are interfering with your motor skills");
			else
				msg("You are starting to feel weak");
		}
		else if (food_left < 2 * MORETIME && oldfood >= 2 * MORETIME)
		{
			hungry_state = 1;
			if (on(player, ISTRIPY))
				msg("Getting the munchies");
			else
				msg("Getting hungry");
		}
	}
}

/*
 * come_down:
 *	Take the hero down off her acid trip.
 */
come_down()
{
	register THING *tp;
	register bool seemonst;

	if (!on(player, ISTRIPY))
		return;

	kill_daemon(visuals);

	if (on(player, ISBLIND))
		return;

	/*
	 * undo the things
	 */
	for (tp = lvl_obj; tp != NULL; tp = next(tp))
		if (cansee(tp->o_pos.y, tp->o_pos.x))
			mvaddch(tp->o_pos.y, tp->o_pos.x, tp->o_type);

	/*
	 * undo the stairs
	 */
	if (seenstairs)
		mvaddch(stairs.y, stairs.x, STAIRS);

	/*
	 * undo the monsters
	 */
	seemonst = on(player, SEEMONST);
	for (tp = mlist; tp != NULL; tp = next(tp))
		if (cansee(tp->t_pos.y, tp->t_pos.x))
			if (!on(*tp, ISINVIS) || on(player, CANSEE))
				mvaddch(tp->t_pos.y, tp->t_pos.x, tp->t_disguise);
			else
				mvaddch(tp->t_pos.y, tp->t_pos.x, chat(tp->t_pos.y, tp->t_pos.x));
		else if (seemonst)
			mvaddch(tp->t_pos.y, tp->t_pos.x, tp->t_type);
	player.t_flags &= ~ISTRIPY;
	msg("Everything looks SO boring now");
}

/*
 * visuals:
 *	change the characters for the player
 */
visuals()
{
	register THING *tp;
	register bool seemonst;

	if (!after)
		return;
	/*
	 * change the things
	 */
	for (tp = lvl_obj; tp != NULL; tp = next(tp))
		if (cansee(tp->o_pos.y, tp->o_pos.x))
			mvaddch(tp->o_pos.y, tp->o_pos.x, rnd_thing());

	/*
	 * change the stairs
	 */
	if (!seenstairs && cansee(stairs.y, stairs.x))
			mvaddch(stairs.y, stairs.x, rnd_thing());

	/*
	 * change the monsters
	 */
	seemonst = on(player, SEEMONST);
	for (tp = mlist; tp != NULL; tp = next(tp))
		if (see_monst(tp))
			if (tp->t_type == 'M' && tp->t_disguise != 'M')
				mvaddch(tp->t_pos.y, tp->t_pos.x, rnd_thing());
			else
				mvaddch(tp->t_pos.y, tp->t_pos.x, rnd(26) + 'A');
		else if (seemonst)
			mvaddch(tp->t_pos.y, tp->t_pos.x, rnd(26) + 'A');
}
