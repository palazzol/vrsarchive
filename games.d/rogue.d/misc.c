#include <curses.h>
#include "rogue.h"
#include <ctype.h>

/*
 * All sorts of miscellaneous routines
 *
 * @(#)misc.c	6.2 (Berkeley) 5/28/84
 */

/*
 * tr_name:
 *	Print the name of a trap
 */
char *
tr_name(type)
char type;
{
	switch (type)
	{
		case T_DOOR:
			return "A trapdoor";
		case T_BEAR:
			return "A beartrap";
		case T_SLEEP:
			return "A sleeping gas trap";
		case T_ARROW:
			return "An arrow trap";
		case T_TELEP:
			return "A teleport trap";
		case T_DART:
			return "A dart trap";
	}
	msg("Wierd trap: %d", type);
	return NULL;
}

/*
 * look:
 *	A quick glance all around the player
 */
look(wakeup)
bool wakeup;
{
	register int x, y;
	register char ch;
	register int index;
	register THING *tp;
	register struct room *rp;
	register int ey, ex;
	register int passcount = 0;
	register char pfl, *fp, pch;
	register int sy, sx, sumhero, diffhero;

	rp = proom;
	if (!ce(oldpos, hero))
	{
		if ((oldrp->r_flags & (ISGONE|ISDARK)) == ISDARK && !on(player,ISBLIND))
		{
			ey = oldpos.y + 1;
			ex = oldpos.x + 1;
			sy = oldpos.y - 1;
			for (x = oldpos.x - 1; x <= ex; x++)
				for (y = sy; y <= ey; y++)
				{
					if (y == hero.y && x == hero.x)
						continue;
					move(y, x);
					if (inch() == FLOOR)
						addch(' ');
				}
		}
		oldpos = hero;
		oldrp = rp;
	}
	ey = hero.y + 1;
	ex = hero.x + 1;
	sx = hero.x - 1;
	sy = hero.y - 1;
	if (door_stop && !firstmove && running)
	{
		sumhero = hero.y + hero.x;
		diffhero = hero.y - hero.x;
	}
	index = INDEX(hero.y, hero.x);
	pfl = s_flags[index];
	pch = s_level[index];
	for (y = sy; y <= ey; y++)
		if (y > 0 && y < s_LINES - 1) for (x = sx; x <= ex; x++)
		{
			if (x <= 0 || x >= s_COLS)
				continue;
			if (!on(player, ISBLIND))
			{
				if (y == hero.y && x == hero.x)
					continue;
			}
			else if (y != hero.y || x != hero.x)
				continue;

			index = INDEX(y, x);
			/*
			 * THIS REPLICATES THE moat() MACRO.  IF MOAT IS
			 * CHANGED, THIS MUST BE CHANGED ALSO.
			 */
			fp = &s_flags[index];
			ch = s_level[index];
			if (pch != DOOR && ch != DOOR)
				if ((pfl & F_PASS) != (*fp & F_PASS))
					continue;
				else if ((*fp & F_PASS) && (*fp & F_PNUM) != (pfl & F_PNUM))
					continue;

			if ((tp = _monst[index]) == NULL)
			{
				if (after && on(player, ISTRIPY))
						switch (ch)
						{
							case FLOOR:
							case ' ':
							case PASSAGE:
							case '-':
							case '|':
							case DOOR:
							case TRAP:
								break;
							default:
								if (y != stairs.y || x != stairs.x ||
									!seenstairs)
										ch = rnd_thing();
								break;
						}
			}
			else
				if (on(player, SEEMONST) && on(*tp, ISINVIS))
				{
					if (door_stop && !firstmove)
						running = FALSE;
					continue;
				}
				else
				{
					if (wakeup)
						wake_monster(y, x);
					if (tp->t_oldch != ' ' ||
						(!(rp->r_flags & ISDARK) && !on(player, ISBLIND)))
							tp->t_oldch = s_level[index];
					if (see_monst(tp))
						ch = tp->t_disguise;
				}

			if (tp == NULL || !on(player, ISTRIPY))
			{
				move(y, x);
				if (ch != inch())
					addch(ch);
			}

			if (door_stop && !firstmove && running)
			{
				switch (runch)
				{
					case 'h':
						if (x == ex)
							continue;
					when 'j':
						if (y == sy)
							continue;
					when 'k':
						if (y == ey)
							continue;
					when 'l':
						if (x == sx)
							continue;
					when 'y':
						if ((y + x) - sumhero >= 1)
							continue;
					when 'u':
						if ((y - x) - diffhero >= 1)
							continue;
					when 'n':
						if ((y + x) - sumhero <= -1)
							continue;
					when 'b':
						if ((y - x) - diffhero <= -1)
							continue;
				}
				switch (ch)
				{
					case DOOR:
						if (x == hero.x || y == hero.y)
							running = FALSE;
						break;
					case PASSAGE:
						if (x == hero.x || y == hero.y)
							passcount++;
						break;
					case FLOOR:
					case '|':
					case '-':
					case ' ':
						break;
					default:
						running = FALSE;
						break;
				}
			}
		}
	if (door_stop && !firstmove && passcount > 1)
		running = FALSE;
	if (!running || !jump)
	{
		move(hero.y, hero.x);
		addch(PLAYER);
	}
}

/*
 * find_obj:
 *	Find the unclaimed object at y, x
 */
THING *
find_obj(y, x)
register int y, x;
{
	register THING *op;

	for (op = lvl_obj; op != NULL; op = next(op))
	{
		if (op->o_pos.y == y && op->o_pos.x == x)
				return op;
	}
#ifdef WIZARD
	sprintf(prbuf, "Non-object %d,%d", y, x);
	debug(prbuf);
	return NULL;
#else
	/* NOTREACHED */
#endif
}

/*
 * eat:
 *	She wants to eat something, so let her try
 */
eat()
{
	register THING *obj;

	if ((obj = get_item("Eat", FOOD)) == NULL)
		return;
	if (obj->o_type != FOOD)
	{
		msg("That's Inedible!");
		return;
	}
	inpack--;
	if (--obj->o_count < 1)
	{
		detach(pack, obj);
		discard(obj);
	}
	if (food_left < 0)
		food_left = 0;
	if ((food_left += HUNGERTIME - 200 + rnd(400)) > STOMACHSIZE)
		food_left = STOMACHSIZE;
	hungry_state = 0;
	if (obj == cur_weapon)
		cur_weapon = NULL;
	if (obj->o_which == 1)
		msg("My, that was a yummy %s", fruit);
	else
		if (rnd(100) > 70)
		{
			pstats.s_exp++;
			msg("%s, this food tastes awful",
				on(player, ISTRIPY) ? "Bummer" : "Yuk");
			check_level();
		}
		else
			msg("%s, that tasted good", on(player, ISTRIPY) ? "Oh, wow" : "Yum");
}

/*
 * chg_str:
 *	Used to modify the playes strength.  It keeps track of the
 *	highest it has been, just in case
 */
chg_str(amt)
register int amt;
{
	str_t comp;

	if (amt == 0)
		return;
	add_str(&pstats.s_str, amt);
	comp = pstats.s_str;
	if (ISRING(LEFT, R_ADDSTR))
		add_str(&comp, -cur_ring[LEFT]->o_ac);
	if (ISRING(RIGHT, R_ADDSTR))
		add_str(&comp, -cur_ring[RIGHT]->o_ac);
	if (comp > max_stats.s_str)
		max_stats.s_str = comp;
}

/*
 * add_str:
 *	Perform the actual add, checking upper and lower bound limits
 */
add_str(sp, amt)
register str_t *sp;
int amt;
{
	if ((*sp += amt) < 3)
		*sp = 3;
	else if (*sp > 31)
		*sp = 31;
}

/*
 * add_haste:
 *	Add a haste to the player
 */
add_haste(potion)
bool potion;
{
	if (on(player, ISHASTE))
	{
		no_command += rnd(8);
		player.t_flags &= ~ISRUN;
		extinguish(nohaste);
		player.t_flags &= ~ISHASTE;
		msg("You faint from exhaustion");
		return FALSE;
	}
	else
	{
		player.t_flags |= ISHASTE;
		if (potion)
			fuse(nohaste, 0, rnd(4)+4, AFTER);
		return TRUE;
	}
}

/*
 * aggravate:
 *	Aggravate all the monsters on this level
 */
aggravate()
{
	register THING *mi;

	for (mi = mlist; mi != NULL; mi = next(mi))
		runto(&mi->t_pos);
}

/*
 * vowelstr:
 *      For printfs: if string starts with a vowel, return "n" for an
 *	"an".
 */
char *
vowelstr(str)
register char *str;
{
	switch (*str)
	{
		case 'a': case 'A':
		case 'e': case 'E':
		case 'i': case 'I':
		case 'o': case 'O':
		case 'u': case 'U':
			return "n";
		default:
			return "";
	}
}

/* 
 * is_current:
 *	See if the object is one of the currently used items
 */
is_current(obj)
register THING *obj;
{
	if (obj == NULL)
		return FALSE;
	if (obj == cur_armor || obj == cur_weapon || obj == cur_ring[LEFT]
		|| obj == cur_ring[RIGHT])
	{
		msg("That's in use");
		return TRUE;
	}
	return FALSE;
}

/*
 * get_dir:
 *      Set up the direction co_ordinate for use in varios "prefix"
 *	commands
 */
get_dir()
{
	register char *prompt;
	register bool gotit;

		prompt = "Direction: ";
	do {
		gotit = TRUE;
		switch (readchar())
		{
			case 'h': case'H': delta.y =  0; delta.x = -1;
			when 'j': case'J': delta.y =  1; delta.x =  0;
			when 'k': case'K': delta.y = -1; delta.x =  0;
			when 'l': case'L': delta.y =  0; delta.x =  1;
			when 'y': case'Y': delta.y = -1; delta.x = -1;
			when 'u': case'U': delta.y = -1; delta.x =  1;
			when 'b': case'B': delta.y =  1; delta.x = -1;
			when 'n': case'N': delta.y =  1; delta.x =  1;
			when ESCAPE: return FALSE;
			default:
				mpos = 0;
				msg(prompt);
				gotit = FALSE;
		}
	} until (gotit);
	if (on(player, ISHUH) && rnd(5) == 0)
		do
		{
			delta.y = rnd(3) - 1;
			delta.x = rnd(3) - 1;
		} while (delta.y == 0 && delta.x == 0);
	mpos = 0;
	return TRUE;
}

/*
 * sign:
 *	Return the sign of the number
 */
sign(nm)
register int nm;
{
	if (nm < 0)
		return -1;
	else
		return (nm > 0);
}

/*
 * spread:
 *	Give a spread around a given number (+/- 10%)
 */
spread(nm)
register int nm;
{
	return nm - nm / 10 + rnd(nm / 5);
}

/*
 * call_it:
 *	Call an object something after use.
 */
call_it(know, guess)
register bool know;
register char **guess;
{
	char *malloc();
	if (know && *guess)
	{
		cfree(*guess);
		*guess = NULL;
	}
	else if (!know && askme && *guess == NULL)
	{
		msg("Call it: ");
		if (get_str(prbuf, stdscr) == NORM)
		{
			*guess = malloc((unsigned int) strlen(prbuf) + 1);
			if (*guess)
				strcpy(*guess, prbuf);
			else
				msg("No memory in call_it");
		}
	}
}

/*
 * rnd_thing:
 *	Pick a random thing appropriate for this level
 */
rnd_thing()
{
	register int i;
	static char thing_list[] = {
		POTION, SCROLL, RING, STICK, FOOD, WEAPON, ARMOR, STAIRS, AMULET
	};

	if (level >= AMULETLEVEL)
		i = rnd(sizeof thing_list / sizeof (char));
	else
		i = rnd(sizeof thing_list / sizeof (char) - 1);
	return thing_list[i];
}
