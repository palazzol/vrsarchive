/*
 * All the fighting gets done here
 *
 * @(#)fight.c	6.3 (Berkeley) 10/3/84
 */

#include <curses.h>
#include <ctype.h>
#include "rogue.h"

long e_levels[] = {
	10L,20L,40L,80L,160L,320L,640L,1280L,2560L,5120L,10240L,20480L,
	40920L, 81920L, 163840L, 327680L, 655360L, 1310720L, 2621440L, 0L
};

/*
 * fight:
 *	The player attacks the monster.
 */
fight(mp, mn, weap, thrown)
register coord *mp;
char mn;
register THING *weap;
bool thrown;
{
	register THING *tp;
	register bool did_hit = TRUE;
	register char *mname, ch;

	/*
	 * Find the monster we want to fight
	 */
#ifdef WIZARD
	if ((tp = moat(mp->y, mp->x)) == NULL)
		debug("Fight what @ %d,%d", mp->y, mp->x);
#else
	tp = moat(mp->y, mp->x);
#endif
	/*
	 * Since we are fighting, things are not quiet so no healing takes
	 * place.
	 */
	count = quiet = 0;
	runto(mp);
	/*
	 * Let him know it was really a mimic (if it was one).
	 */
	ch = '\0';
	if (tp->t_type == 'M' && tp->t_disguise != 'M' && !on(player, ISBLIND))
	{
		tp->t_disguise = 'M';
		if (on(player, ISTRIPY)) {
			ch = rnd(26) + 'A';
			mname = monsters[ch - 'A'].m_name;
			mvaddch(tp->t_pos.y, tp->t_pos.x, ch);
		}
		else
			mname = monsters['M'-'A'].m_name;
		if (on(player, ISTRIPY))
			msg("Wait!  That's a monster!");
		else
			msg("Wait!  That's a mimic!");
		if (!thrown)
			return FALSE;
	}
	if (on(player, ISBLIND) ||
	   (on(*tp, ISINVIS) && !on(player, CANSEE|SEEMONST)))
			mname = "it";
	else if (on(player, ISTRIPY)) {
		if (see_monst(tp))
			mname = monsters[toascii(mvinch(tp->t_pos.y, tp->t_pos.x))-'A'].m_name;
		else
			mname = monsters[rnd(26)].m_name;
	} else
		mname = monsters[mn-'A'].m_name;
	did_hit = FALSE;
	if (roll_em(&player, tp, weap, thrown))
	{
		did_hit = FALSE;
		if (thrown)
			thunk(weap, mname);
		else
			hit((char *)NULL, mname);
		if (on(player, CANHUH))
		{
			did_hit = TRUE;
			tp->t_flags |= ISHUH;
			player.t_flags &= ~CANHUH;
			msg("Your hands stop glowing %s",
				on(player, ISTRIPY) ? rnd_color() : "red");
		}
		if (tp->t_stats.s_hpt <= 0)
			killed(tp, TRUE);
		else if (did_hit && !on(player, ISBLIND))
			msg("The %s appears confused", mname);
		did_hit = TRUE;
	}
	else
		if (thrown)
			bounce(weap, mname);
		else
			miss((char *)NULL, mname);
	return did_hit;
}

/*
 * attack:
 *	The monster attacks the player
 */
attack(mp)
register THING *mp;
{
	register char *mname, ch;

	/*
	 * Since this is an attack, stop running and any healing that was
	 * going on at the time.
	 */
	running = FALSE;
	count = quiet = 0;
	if (mp->t_type == 'M' && mp->t_disguise != 'M' && !on(player, ISBLIND))
	{
		mp->t_disguise = 'M';
		if (on(player, ISTRIPY))
			mvaddch(mp->t_pos.y, mp->t_pos.x, rnd(26) + 'A');
	}
	if (on(player, ISBLIND) ||
	   (on(*mp, ISINVIS) && !on(player, CANSEE|SEEMONST)))
			mname = "it";
	else if (on(player, ISTRIPY)) {
		ch = toascii(mvinch(mp->t_pos.y, mp->t_pos.x));
		if (!isupper(ch))
			addch(ch = rnd(26) + 'A');
		mname = monsters[ch-'A'].m_name;
	}
	else
		mname = monsters[mp->t_type-'A'].m_name;
	if (roll_em(mp, &player, (THING *)NULL, FALSE))
	{
		if (mp->t_type != 'E')
			hit(mname, (char *)NULL);
		if (pstats.s_hpt <= 0)
			death(mp->t_type);	/* Bye bye life ... */
		if (!on(*mp, ISCANC))
			switch (mp->t_type)
			{
				case 'R':
					/*
					 * If a rust monster hits, you lose armor, unless
					 * that armor is leather or there is a magic ring
					 */
					if (cur_armor != NULL && cur_armor->o_ac < 9
						&& cur_armor->o_which != LEATHER)
							if (ISWEARING(R_SUSTARM))
								msg("The rust vanishes");
							else
							{
								cur_armor->o_ac++;
								msg("Your armor weakens");
							}
				when 'E':
					/*
					 * The gaze of the floating eye hypnotizes you
					 */
					if (on(player, ISBLIND))
						break;
					player.t_flags &= ~ISRUN;
					if (!no_command)
						msg("You are transfixed");
					no_command += rnd(2) + 2;
				when 'A':
					/*
					 * Ants have poisonous bites
					 */
					if (!save(VS_POISON))
						if (!ISWEARING(R_SUSTSTR))
						{
							chg_str(-1);
							msg("A sting weakens you");
						}
						else
							msg("Sting has no effect");
				when 'W':
				case 'V':
					/*
					 * Wraiths might drain energy levels
					 * and Vampires can steal max_hp
					 */
					if (rnd(100) < (mp->t_type == 'W' ? 15 : 30))
					{
						register int fewer;

						if (mp->t_type == 'W')
						{
							if (pstats.s_exp == 0)
								death('W');		/* All levels gone */
							if (--pstats.s_lvl == 0)
							{
								pstats.s_exp = 0;
								pstats.s_lvl = 1;
							}
							else
								pstats.s_exp = e_levels[pstats.s_lvl-1]+1;
							fewer = roll(1, 10);
						}
						else
							fewer = roll(1, 5);
						pstats.s_hpt -= fewer;
						max_hp -= fewer;
						if (pstats.s_hpt < 1)
							pstats.s_hpt = 1;
						if (max_hp < 1)
							death(mp->t_type);
						msg("You suddenly feel weaker");
					}
				when 'F':
					/*
					 * Violet fungi stops the poor guy from moving
					 */
					player.t_flags |= ISHELD;
					sprintf(monsters['F'-'A'].m_stats.s_dmg,"%dd1",++fung_hit);
				when 'L':
				{
					/*
					 * Leperachaun steals some gold
					 */
					register long lastpurse;

					lastpurse = purse;
					purse -= GOLDCALC;
					if (!save(VS_MAGIC))
						purse -= GOLDCALC + GOLDCALC + GOLDCALC + GOLDCALC;
					if (purse < 0)
						purse = 0;
					remove(&mp->t_pos, mp, FALSE);
					if (purse != lastpurse)
						msg("Your purse feels lighter");
				}
				when 'N':
				{
					register THING *obj, *steal;
					register int nobj;

					/*
					 * Nymph's steal a magic item, look through the pack
					 * and pick out one we like.
					 */
					steal = NULL;
					for (nobj = 0, obj = pack; obj != NULL; obj = next(obj))
						if (obj != cur_armor && obj != cur_weapon
							&& obj != cur_ring[LEFT] && obj != cur_ring[RIGHT]
							&& is_magic(obj) && rnd(++nobj) == 0)
								steal = obj;
					if (steal != NULL)
					{
						remove(&mp->t_pos, moat(mp->t_pos.y, mp->t_pos.x), FALSE);
						inpack--;
						if (steal->o_count > 1 && steal->o_group == 0)
						{
							register int oc;

							oc = steal->o_count--;
							steal->o_count = 1;
							msg("She stole %s!", inv_name(steal, TRUE));
							steal->o_count = oc;
						}
						else
						{
							detach(pack, steal);
							discard(steal);
							msg("She stole %s!", inv_name(steal, TRUE));
						}
					}
				}
				otherwise:
					break;
			}
	}
	else if (mp->t_type != 'E')
	{
		if (mp->t_type == 'F')
		{
			pstats.s_hpt -= fung_hit;
			if (pstats.s_hpt <= 0)
				death(mp->t_type);	/* Bye bye life ... */
		}
		miss(mname, (char *)NULL);
	}
	if (fight_flush)
		flush_type();
	count = 0;
	status();
}

/*
 * swing:
 *	Returns true if the swing hits
 */
swing(at_lvl, op_arm, wplus)
int at_lvl, op_arm, wplus;
{
	register int res = rnd(20);
	register int need = (20 - at_lvl) - op_arm;

	return (res + wplus >= need);
}

/*
 * check_level:
 *	Check to see if the guy has gone up a level.
 */
check_level()
{
	register int i, add, olevel;

	for (i = 0; e_levels[i] != 0; i++)
		if (e_levels[i] > pstats.s_exp)
			break;
	i++;
	olevel = pstats.s_lvl;
	pstats.s_lvl = i;
	if (i > olevel)
	{
		add = roll(i - olevel, 10);
		max_hp += add;
		if ((pstats.s_hpt += add) > max_hp)
			pstats.s_hpt = max_hp;
		msg("Welcome to level %d", i);
	}
}

/*
 * roll_em:
 *	Roll several attacks
 */
roll_em(thatt, thdef, weap, hurl)
THING *thatt, *thdef, *weap;
bool hurl;
{
	register struct stats *att, *def;
	register char *cp;
	register int ndice, nsides, def_arm;
	register bool did_hit = FALSE;
	register int hplus;
	register int dplus;
	register int damage;
	char *strchr();

	att = &thatt->t_stats;
	def = &thdef->t_stats;
	if (weap == NULL)
	{
		cp = att->s_dmg;
		dplus = 0;
		hplus = 0;
	}
	else
	{
		hplus = (weap == NULL ? 0 : weap->o_hplus);
		dplus = (weap == NULL ? 0 : weap->o_dplus);
		if (weap == cur_weapon)
		{
			if (ISRING(LEFT, R_ADDDAM))
				dplus += cur_ring[LEFT]->o_ac;
			else if (ISRING(LEFT, R_ADDHIT))
				hplus += cur_ring[LEFT]->o_ac;
			if (ISRING(RIGHT, R_ADDDAM))
				dplus += cur_ring[RIGHT]->o_ac;
			else if (ISRING(RIGHT, R_ADDHIT))
				hplus += cur_ring[RIGHT]->o_ac;
		}
		if (hurl)
			if ((weap->o_flags&ISMISL) && cur_weapon != NULL &&
			  cur_weapon->o_which == weap->o_launch)
			{
				cp = weap->o_hurldmg;
				hplus += cur_weapon->o_hplus;
				dplus += cur_weapon->o_dplus;
			}
			else
				cp = weap->o_hurldmg;
		else
			cp = weap->o_damage;
	}
	/*
	 * If the creature being attacked is not running (alseep or held)
	 * then the attacker gets a plus four bonus to hit.
	 */
	if (!on(*thdef, ISRUN))
		hplus += 4;
	def_arm = def->s_arm;
	if (def == &pstats)
	{
		if (cur_armor != NULL)
			def_arm = cur_armor->o_ac;
		if (ISRING(LEFT, R_PROTECT))
			def_arm -= cur_ring[LEFT]->o_ac;
		if (ISRING(RIGHT, R_PROTECT))
			def_arm -= cur_ring[RIGHT]->o_ac;
	}
	for (;;)
	{
		ndice = atoi(cp);
		if ((cp = strchr(cp, 'd')) == NULL)
			break;
		nsides = atoi(++cp);
		if (swing(att->s_lvl, def_arm, hplus + str_plus(att->s_str)))
		{
			register int proll;

			proll = roll(ndice, nsides);
#ifdef WIZARD
			if (ndice + nsides > 0 && proll < 1)
				debug("Damage for %dd%d came out %d, dplus = %d, add_dam = %d, def_arm = %d", ndice, nsides, proll, dplus, add_dam(att->s_str), def_arm);
#endif
			damage = dplus + proll + add_dam(att->s_str);
			def->s_hpt -= max(0, damage);
			did_hit = TRUE;
		}
		if ((cp = strchr(cp, '/')) == NULL)
			break;
		cp++;
	}
	return did_hit;
}

/*
 * prname:
 *	The print name of a combatant
 */
char *
prname(mname, upper)
register char *mname;
bool upper;
{
	static char tbuf[MAXSTR];

	*tbuf = '\0';
	if (mname == 0)
		strcpy(tbuf, "you"); 
	else if (strcmp(mname, "it") == 0 || strcmp(mname, "something") == 0)
		strcpy(tbuf, mname);
	else
	{
		strcpy(tbuf, "the ");
		strcat(tbuf, mname);
	}
	if (upper)
		*tbuf = toupper(*tbuf);
	return tbuf;
}

/*
 * hit:
 *	Print a message to indicate a succesful hit
 */
hit(er, ee)
register char *er, *ee;
{
	register char *s;

	addmsg(prname(er, TRUE));
	s = " hit ";
	addmsg(s);
	addmsg(prname(ee, FALSE));
	endmsg();
}

/*
 * miss:
 *	Print a message to indicate a poor swing
 */
miss(er, ee)
register char *er, *ee;
{
	register char *s;

	addmsg(prname(er, TRUE));
	s = (er == 0 ? " miss" : " misses");
	addmsg(s);
	addmsg(" %s", prname(ee, FALSE));
	endmsg();
}

/*
 * save_throw:
 *	See if a creature save against something
 */
save_throw(which, tp)
int which;
THING *tp;
{
	register int need;

	need = 14 + which - tp->t_stats.s_lvl / 2;
	return (roll(1, 20) >= need);
}

/*
 * save:
 *	See if he saves against various nasty things
 */
save(which)
register int which;
{
	if (which == VS_MAGIC)
	{
		if (ISRING(LEFT, R_PROTECT))
			which -= cur_ring[LEFT]->o_ac;
		if (ISRING(RIGHT, R_PROTECT))
			which -= cur_ring[RIGHT]->o_ac;
	}
	return save_throw(which, &player);
}

/*
 * str_plus:
 *	Compute bonus/penalties for strength on the "to hit" roll
 */
str_plus(str)
register str_t str;
{
	if (str == 31)
		return 3;
	if (str > 20)
		return 2;
	if (str > 16)
		return 1;
	if (str > 6)
		return 0;
	return str - 7;
}

/*
 * add_dam:
 *	Compute additional damage done for exceptionally high or low strength
 */
 add_dam(str)
 register str_t str;
 {
	if (str == 31)
		return 6;
	if (str > 21)
		return 5;
	if (str == 21)
		return 4;
	if (str > 18)
		return 3;
	if (str == 18)
		return 2;
	if (str > 15)
		return 1;
	if (str > 6)
		return 0;
	return str - 7;
}

/*
 * raise_level:
 *	The guy just magically went up a level.
 */
raise_level()
{
	pstats.s_exp = e_levels[pstats.s_lvl-1] + 1L;
	check_level();
}

/*
 * thunk:
 *	A missile hits a monster
 */
thunk(weap, mname)
register THING *weap;
register char *mname;
{
	if (weap->o_type == WEAPON)
		addmsg("The %s hits ", w_names[weap->o_which]);
	else
		addmsg("You hit ");
	if (strcmp(mname, "it") == 0 || strcmp(mname, "something") == 0)
		msg(mname);
	else
		msg("the %s", mname);
}

/*
 * bounce:
 *	A missile misses a monster
 */
bounce(weap, mname)
register THING *weap;
register char *mname;
{
	if (weap->o_type == WEAPON)
		addmsg("The %s misses ", w_names[weap->o_which]);
	else
		addmsg("You missed ");
	if (strcmp(mname, "it") != 0 && strcmp(mname, "something") != 0)
		addmsg("the ");
	msg(mname);
}

/*
 * remove:
 *	Remove a monster from the screen
 */
remove(mp, tp, waskill)
register coord *mp;
register THING *tp;
bool waskill;
{
	register THING *obj, *nexti;

	for (obj = tp->t_pack; obj != NULL; obj = nexti)
	{
		nexti = next(obj);
		obj->o_pos = tp->t_pos;
		detach(tp->t_pack, obj);
		if (waskill)
			fall(obj, FALSE);
		else
			discard(obj);
	}
	moat(mp->y, mp->x) = NULL;
	mvaddch(mp->y, mp->x, tp->t_oldch);
	detach(mlist, tp);
	discard(tp);
}

/*
 * is_magic:
 *	Returns true if an object radiates magic
 */
is_magic(obj)
register THING *obj;
{
	switch (obj->o_type)
	{
		case ARMOR:
			return obj->o_ac != a_class[obj->o_which];
		case WEAPON:
			return obj->o_hplus != 0 || obj->o_dplus != 0;
		case POTION:
		case SCROLL:
		case STICK:
		case RING:
		case AMULET:
			return TRUE;
	}
	return FALSE;
}

/*
 * killed:
 *	Called to put a monster to death
 */
killed(tp, pr)
register THING *tp;
bool pr;
{
	register char *mname;

	if (pr)
		if (!on(player, ISTRIPY))
			mname = monsters[tp->t_type-'A'].m_name;
		else
			if (see_monst(tp))
				mname = monsters[toascii(mvinch(tp->t_pos.y, tp->t_pos.x))-'A'].m_name;
			else
				mname = monsters[rnd(26)].m_name;

	pstats.s_exp += tp->t_stats.s_exp;
	/*
	 * If the monster was a violet fungi, un-hold him
	 */
	switch (tp->t_type)
	{
		case 'F':
			player.t_flags &= ~ISHELD;
			fung_hit = 0;
			strcpy(monsters['F'-'A'].m_stats.s_dmg, "000d0");
		when 'L':
		{
			register THING *gold;

			if (fallpos(&tp->t_pos, &tp->t_room->r_gold, TRUE))
			{
				gold = new_item();
				gold->o_type = GOLD;
				gold->o_goldval = GOLDCALC;
				if (save(VS_MAGIC))
					gold->o_goldval += GOLDCALC + GOLDCALC
									 + GOLDCALC + GOLDCALC;
				attach(tp->t_pack, gold);
			}
		}
	}
	/*
	 * Get rid of the monster.
	 */
	remove(&tp->t_pos, tp, TRUE);
	if (pr)
	{
		addmsg("Defeated ");
		if (on(player, ISBLIND) ||
		   (on(*tp, ISINVIS) && !on(player, CANSEE|SEEMONST)))
			msg("it");
		else
		{
			addmsg("the ");
			msg(mname);
		}
	}
	/*
	 * Do adjustments if he went up a level
	 */
	check_level();
}
