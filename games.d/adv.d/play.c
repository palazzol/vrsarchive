/*
 * Strategy:
 * Run the %chancer
 * Display current room (on top 1/2 of screen if possible),
 * Print "Tell me what to do:" (on bottom 1/2),
 * Parse input; take special actions if required (SAVE/LOAD GAME,QUIT)
 * take semispecial actions when required (N,S,E,W,U,D are like GO N)
 * Perform action - GET, DROP, & GO are special
 * Continue forever
 */

#include "adv.h"

BOOL	huhwhat,		/* True => command not understood */
	nocando;		/* True => can't do it (yet?) */

play()
{

	while (!isend) {
		percnt();
		if (isend)
			break;
		parse();
		if (isend)
			break;
#ifdef DEBUG
		printf("Verbv=%d, objv=%d\n",verbv,objv);
#endif
		actout();
		if (huhwhat)
			prt("I don't understand your command.\n");
		else if (nocando)
			prt("I can't do that...yet!\n");
	}
}

/*
 * Run the random chance events.  An event fires if the random number
 * generator picks a number <= to its chance value.
 */
percnt()
{
	register int i, j;

	for (i = 0; i < npcts; i++) {
		do {
			j = (rand() >> 8) & 255;
		} while (j == 0);
		if (j <= iconv(pct[i][0]))
			acton(pct[i]+1);
	}
}

/*
 * Act out the (verb, object) command pair.  Handle motion
 * commands by moving to the appropriate destination.
 * Search for an action for other (verb, obj) pairs; if
 * not found, handle GET and PUT command default actions.
 */
actout()
{
    register int i, j;
    register struct item *ip;

    /*
     * Initially assume the command is bad, and that we can't do it.
     */
    huhwhat = TRUE;
    nocando = TRUE;
    if (verbv == 1 && objv < 7) {		/* GO [NSEWUD] */
	/*
	 * Move in the indicated direction.  If the user tries to
	 * move somewhere he can't, and it's dark, kill him.
	 */
	huhwhat = nocando = FALSE;
	if (isdark)
	    prt("It's dangerous to move in the dark!\n");
	if (!objv)
	    prt("I need a direction too.\n");
	else {
	    i = rooms[rm].dirs[objv-1];
	    if (i) {
		isdark = rooms[i].dark && !lighted(i);
		rm = i;
	    }
	    else {
		if (isdark) {
		    prt("I fell down and broke my neck.\n");
		    rm = hellrm;
		    isdark = rooms[rm].dark && !lighted(rm);
		}
		else
		    prt("I can't go in that direction.\n");
	    }
	}
	return;
    }

    /*
     * Search the action table.  If we get a hit, and we can do the
     * action, stop right away.  (That is, disjunction in the tables
     * is like ||, not like |.)
     */
    for (i = 0; i < nactns; i++) {
	register int t1, t2;

	t1 = iconv(actn[i][0]);
	t2 = iconv(actn[i][1]);
	if ((t1 == 255 || t1 == verbv) && (t2 == 255 || objv && t2 == objv)) {
	    huhwhat = FALSE;
	    acton(actn[i] + 2);
	    if (!nocando)
		return;
	}
    }

    /*
     * If the command was not understood and it was GET or DROP,
     * take the default action.
     */
    if (!huhwhat)
	return;
    if (verbv != 2 && verbv != 3)
	return;

    nocando = huhwhat = FALSE;
    if (objv == 0)
	prt("WHAT?\n");
    else {			/* GET or DROP item */

#define	e_Ok			(-1)	/* No error ("Ok") */
#define e_AlreadyCarrying	0	/* already carring item */
#define	e_NotCarrying		1	/* not carrying item */
#define	e_NotHere		2	/* item not in room */
#define e_BeyondPower		3	/* "beyond my power to do that" */

	int errcod = e_BeyondPower;
	static char *errmsg[] = {
	    "I'm already carrying it!\n",
	    "I'm not carrying it.\n",
	    "I don't see it here.\n",
	    "It's beyond my power to do that.\n",
	};

	/* Look for the named item */
	for (ip = &items[1]; ip < iteme; ip++) {
	    if (*ip->name && wrdval(nobjs, objs, ip->name) == objv) {
		j = ip->rm;		/* What room is it in? */
		if (j == -1) {		/* He is carrying it */
		    if (verbv == 2)
			errcod = e_AlreadyCarrying;
		    else {
			ip->rm = rm;
			prt("Ok, ");	/* Drop it */
			errcod = e_Ok;
			break;
		    }
		}
		else {			/* He is not carrying it */
		    if (verbv == 3)
			errcod = e_NotCarrying;
		    else {
			if (j == rm) {	/* If it is in the room... */
			    register struct item *p;
			    register int curinv = 0;

			    errcod = e_Ok;
			    for (p = &items[1]; p < iteme; p++)
				if (p->rm == -1)
				    curinv++;
			    if (curinv >= maxinv)
				prt(toomch);/* "Carrying too much" */
			    else {	/* Pick it up */
				ip->rm = -1;
				prt("Ok, ");
				break;
			    }
			}
			else		/* It is not in the room */
			    errcod = e_NotHere;
		    }
		}
	    }
	}
	if (errcod >= 0)
	    prt(errmsg[errcod]);
    }
}

/*
 * Act on a set of conditions.  If all the conditions hold,
 * act on the predicate.
 */
acton(cp)
    register char *cp;		/* Condition/predicate */
{
    register BOOL test;
    register struct item *ip;
    int m, n, c, temp;

    /*
     * Magic numbers, yucko.  Things between 9 and 26 have one parameter
     * (n); things 27 up have 2 (n, m).
     */
    while (c = iconv(*cp++)) {
	if (c > 26)
	    m = iconv(*cp++);
	if (c > 8)
	    n = iconv(*cp++);
	/*
	 * Even numbers are inverted tests; decrement those.
	 */
	switch (c - ((c & 1) == 0)) {
	case 1:				/* Be emptyhanded */
	    test = 1;
	    for (ip = &items[1]; ip < iteme; ip++)
		if (ip->rm == -1)
		    test = 0;
	when 3:				/* Be dark */
	    test = isdark;
	when 5:				/* V0=0 */
	    test = var[0] == 0;
	when 7:				/* V0>0 */
	    test = var[0] > 0;
	when 9:				/* Vn=0 */
	    test = var[n] == 0;
	when 11:			/* Vn>0 */
	    test = var[n] > 0;
	when 13:			/* V0=n */
	    test = var[0] == n;
	when 15:			/* V0>n */
	    test = var[0] > n;
	when 17:			/* Carrying item n */
	    test = items[n].rm == -1;
	when 19:			/* Item n in room */
	    test = items[n].rm == rm;
	when 21:			/* Item n in room or carried */
	    test = items[n].rm == rm || items[n].rm == -1;
	when 23:			/* Be in room n */
	    test = rm == n;
	when 25:			/* Item n in limbo */
	    test = items[n].rm == 0;
	when 27:			/* Item m in room n */
	    test = items[m].rm == n;
	when 29:			/* Vm=n */
	    test = var[m] == n;
	when 31:			/* Vm>n */
	    test = var[m] > n;
	}
	if ((c & 1) == 0)
	    test = !test;
	if (!test)
	    return;
    }

    /*
     * We made it.  Perform the actions.
     */
    nocando = FALSE;
    while (c = iconv(*cp++)) {
	if (c > 26)
	    m = iconv(*cp++);
	if (c > 13)
	    n = iconv(*cp++);
	switch (c) {
	case 1:				/* Win */
win:
	    prt("FANTASTIC!! You've won, ");
	    prt(name);
	    prt("!\n");
	    isend = TRUE;
	    return;
	case 2:				/* Die */
	    prt(name);
	    prt(", you're dead...\n");
	    rm = hellrm;
	    isdark = rooms[rm].dark && !lighted(rm);
	when 3:				/* Quit */
	    prt("The game is now over.\n");
	    disply();
	    isend = TRUE;
	    return;
	case 4:				/* Inventory */
	    prt("I'm carrying");
	    test = TRUE;
	    for (ip = &items[1]; ip < iteme; ip++) {
		if (ip->rm == -1) {
		    if (test) {
			prt(":\n");
			test = FALSE;
		    }
#ifndef CURSED
		    if (CurWin->w_cursor.col + strlen(ip->desc) > COLS-3)
			prt("\n");
#else
		    {
		    	int	y, x;
			getyx(CurWin, y, x);

		 	if (x + strlen(ip->desc) > COLS-3)
				prt("\n");
		    }
#endif
		    prt(ip->desc);
		    prt(". ");
		}
	    }
	    prt(test ? " nothing\n" : "\n");
	when 5:				/* Make dark */
	    isdark = TRUE;
	when 6:				/* Make light */
	    isdark = FALSE;
	when 7:				/* Refill lamp */
	    littim = litmax;
	    items[lititm].rm = -1;
	    isdark = FALSE;
	when 8:				/* Score */
	    if (tottrs == 0) {	/* Fix: Chris Miller */
		prt("There aren't any treasures in this game.\n");
		break;
	    }			/* End of fix. */
	    temp = 0;
	    for (ip = &items[1]; ip < iteme; ip++)
		if (ip->rm == trsrm && *ip->desc == '*')
		    ++temp;
	    prt("I've stored ");
	    prt(itoa (temp));
	    prt(" treasure");
	    if (temp != 1)
		prt("s");
	    prt(".  On a scale of 0 to 100 that rates a ");
	    prt(itoa ((temp * 100) / tottrs));
	    prt(".\n");
	    if (temp == tottrs)
		goto win;
	when 9:				/* V0=0 */
	    var[0] = 0;
	when 10:			/* V0=1 */
	    var[0] = 1;
	when 11:			/* V0++ */
	    var[0]++;
	when 12:			/* V0-- */
	    var[0]--;
	when 13:			/* Print V0 */
	    prt(itoa(iconv(var[0])));
	when 14:			/* Print msg n */
	    prt(msg[n]);
	when 15:			/* Print Vn */
	    prt(itoa(iconv(var[n])));
	when 16:			/* Vn=0 */
	    var[n] = 0;
	when 17:			/* Vn=1 */
	    var[n] = 1;
	when 18:			/* Vn++ */
	    var[n]++;
	when 19:			/* Vn-- */
	    var[n]--;
	when 20:			/* V0=n */
	    var[0] = n;
	when 21:			/* V0+=n */
	    var[0] += n;
	when 22:			/* Move to room n */
	    rm = n;
	    isdark = rooms[rm].dark && !lighted(rm);
	when 23:			/* Get item n */
	    if (items[n].rm == -1)
		break;
#if 0
	    /* this can really kill a game! */
	    temp = 0;
	    for (ip = &items[1]; ip < iteme; ip++)
		if (ip->rm == -1)
		    ++temp;
	    if (temp >= maxinv)
		prt(toomch);
	    else
#endif
		items[n].rm = -1;
	when 24:			/* Drop item n */
	    if (items[n].rm != -1)
		prt("I'm not carrying it!\n");
	    else
		items[n].rm = rm;
	when 25:			/* Move item n to limbo */
	    items[n].rm = 0;
	when 26:			/* Move item n to room */
	    items[n].rm = rm;
	when 27:			/* Vm=n */
	    var[m] = n;
	when 28:			/* Vm+=n */
	    var[m] += n;
	when 29:			/* Move item m to room n */
	    items[m].rm = n;
	when 30:			/* Swap items m,n */
	    temp = items[m].rm;
	    items[m].rm = items[n].rm;
	    items[n].rm = temp;
	}
    }
}
