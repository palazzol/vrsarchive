/***************************************************************\
*								*
*	PCB program						*
*								*
*	Lee Router section	(part 2: confined maze run)	*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"
#include "pleer.h"

static int viahcnt;		/* via hole count for L*_route	 */
struct vht {			/* via hole table		 */
    char *t;			/* koordinate			 */
    int c;			/* cost				 */
} vhct[maxvhct];

maze_run1 (xs, ys)		/* run through the maze on side 1 */
    int xs, ys;
{
    int    i, mz_vf (), mz_hf ();

    if ((xs <= ox) || (xs >= ox + sx - 1) || (ys <= oy) || (ys >= oy + sy - 1))
	err ("maze_run1: invalid start point", xs, ys, ox, oy);

    set_dir ();
    hvrt = (unsigned *) p_malloc ((maxrtl + sftmrg) * sizeof (*hvrt));
    drt = (unsigned *) p_malloc ((maxrtl + sftmrg) * sizeof (*drt));
    drtmax = hvrtmax = maxrtl;
    vhtc = drtc = drtcd = hvrtcd = hvrtc = 0;/* reset table counter */

    wtvf = mz_vf;		/* mark start			 */
    wthf = mz_hf;
    wtrace (xs, ys, vec);

    for (i = 0; i < drtc; i++)	/* remove entries for wrong side */
	if (drt[i] & rtsb) {
	    drt[i] = 0;
	    drtcd++;
	}
    for (i = 0; i < hvrtc; i++)
	if (hvrt[i] & rtsb) {
	    hvrt[i] = 0;
	    hvrtcd++;
	}

    i = maze_run (xs, ys);	/* find the path		 */

    p_free (drt);
    p_free (hvrt);

    return (i);
}

maze_run2 (xs, ys)		/* run through the maze on side 1 */
    int xs, ys;
{
    int    i, mz_vf (), mz_hf ();

    if ((xs <= ox) || (xs >= ox + sx - 1) || (ys <= oy) || (ys >= oy + sy - 1))
	err ("maze_run2: invalid start point", xs, ys, ox, oy);

    set_dir ();
    hvrt = (unsigned *) p_malloc ((maxrtl + sftmrg) * sizeof (*hvrt));
    drt = (unsigned *) p_malloc ((maxrtl + sftmrg) * sizeof (*drt));
    drtmax = hvrtmax = maxrtl;
    vhtc = drtc = drtcd = hvrtcd = hvrtc = 0;/* reset table counter */

    wtvf = mz_vf;		/* mark start			 */
    wthf = mz_hf;
    wtrace (xs, ys, vec);

    for (i = 0; i < drtc; i++)	/* remove entries for wrong side */
	if (!(drt[i] & rtsb)) {
	    drt[i] = 0;
	    drtcd++;
	}
    for (i = 0; i < hvrtc; i++)
	if (!(hvrt[i] & rtsb)) {
	    hvrt[i] = 0;
	    hvrtcd++;
	}

    i = maze_run (xs, ys);	/* find the path		 */

    p_free (drt);
    p_free (hvrt);

    return (i);
}

s_route (xs, ys, xd, yd)
    int xs, ys, xd, yd;
/******************************************************************\
* 								   *
*  Segment route: <xs,ys> and <xd,yd> of a *selected* net will be  *
*  connected by a wire. No action is taken if they are already	   *
*  connected. Routing starts from <xd,yd>, therefore it is more	   *
*  efficient if <xs,ys> is part of the larger object.		   *
* 								   *
\******************************************************************/
{   int     r, lx, ly, dpin (), plt ();

    if (!(pcb[ys][xs] & pcb[yd][xd] & selb))
	err ("S-route: unselected net", xs, ys, xd, yd);

    lx = abs (xs - xd);
    ly = abs (ys - yd);

    if (!RT_sel || (lx + ly) < 10)
	return (S_route (xs, ys, xd, yd));

    r = 0;
    if (pcb[ys][xs] & ahb && pcb[yd][xd] & ahb)
	r = filter (xs, ys, xd, yd);/* crude attempt		 */
    if (!r) {
	if (lx > ly && ly < K1)	/* simple horizontal run	 */
	    r = a_route (xs, ys, xd, yd, s1b);
	else if (lx < ly && lx < K1)/* simple vertical run	 */
	    r = a_route (xs, ys, xd, yd, s2b);
	else
	    r = b_route (xs, ys, xd, yd);/* non-trivial run	 */
    }
    return (r);
}

a_route (xs, ys, xd, yd, sd)
    int xs, ys, xd, yd, sd;
/***************************************************************************\
* 									    *
*  a-route: connect <xd,yd> to <xs,ys> with a single side run (side=<sd>).  *
*  prerequisites: <xd,yd> was de-selected to avoid shortcuts.		    *
*                 <x?,y?> must be centered if on a hole.		    *
* 									    *
\***************************************************************************/
{
    int     i;

    if (!(pcb[yd][xd] & ahb) && ((pcb[yd][xd] & vec) != sd)) {
				/* start on wrong side		 */
	return (0);		/* not yet implemented		 */
    }

    cr_gmaze (xs, ys, xd, yd, K2);/* create maze		 */

    i = (sd == s1b) ? maze_run1 (xd, yd) : maze_run2 (xd, yd);

    if (!i) {			/* failed?			 */
	i = *(abm + (xs - ox + (ys - oy) * sx));
	i = ((sd == s1b) ? i : i >> 4) & MSK1;
	i = i == START || ((i == NOGOD) && !(pcb[ys][xs] & (sd ^ vec)));
				/* =1 if already connected	 */
    }

    p_free (abm);			/* release memory		 */
    abm = 0;

    return (i);
}

b_route (xs, ys, xd, yd)	/* 2 side L-route		 */
    int xs, ys, xd, yd;
{
    int     i;
    static int  lst = s1b;	/* last side			 */

    switch (rt_str) {		/* select side to start		 */
	default: 		/* alternating preferences	 */
	    lst = (pcb[yd][xd] & ahb) ? vec ^ lst : pcb[yd][xd] & vec;
	    break;
	case 1: 		/* prefere side 1		 */
	    lst = (pcb[yd][xd] & ahb) ? s1b : pcb[yd][xd] & vec;
	    break;
	case 2: 		/* prefere side 2		 */
	    lst = (pcb[yd][xd] & ahb) ? s2b : pcb[yd][xd] & vec;
	    break;
    }

    if (!(i = l_route (xs, ys, xd, yd, lst)) && (pcb[yd][xd] & ahb)) {
	if((i = l_route (xs, ys, xd, yd, lst ^ vec)))
	    lst ^= vec;
    }

    if (!i && (abs (xs - xd) + abs (ys - yd) > K4)) {
        viahcnt = 0;
	i = (lst == s1b) ? L1_route (xs, ys, xd, yd) :
			   L2_route (xs, ys, xd, yd);
	if (!i && (pcb[yd][xd] & ahb)) {
	    viahcnt = 0;
	    i = (lst == s2b) ? L1_route (xs, ys, xd, yd) :
			       L2_route (xs, ys, xd, yd);
	    lst ^= (i) ? vec : 0;
	}
    }

    return (i);
}

l_route (xs, ys, xd, yd, sd)	/* 2 side L-route		 */
    int xs, ys, xd, yd, sd;
{
    int i, j, x, y, x1, y1, x2, y2, vh_key();
    int ox1, oy1, sx1, sy1, ox2, oy2, sx2, sy2;
    char *t, *t1, *abm1, *abm2;

    if (sd == s1b) {		/* start on side 1		 */
	cr_gmaze (xd, yd, xs, yd, K3);
	if (maze_run1 (xd, yd)) {/* surprise: done		 */
	    p_free (abm);
	    abm = 0;
	    return (1);
	}
	x = (xs > xd) ? xs - K3 : xs + K3;
	if (x <= ox || x >= ox + sx - 1)
	    x = (xd + xs) / 2;
	t = abm + (x - ox);
	for (i = 0; i <= 2 * K3; i++) {
	    if (*t & HDT)
		break;
	    t += sx;
	}
	if (i > 2 * K3) {
	    p_free (abm);		/* failed to reach via-area	 */
	    abm = 0;
	    return (0);
	}}
    else {			/* start on side 2		 */
	cr_gmaze (xd, yd, xd, ys, K3);
	if (maze_run2 (xd, yd)) {/* surprise: done		 */
	    p_free (abm);
	    abm = 0;
	    return (1);
	}
	y = (ys > yd) ? ys - K3 : ys + K3;
	if (y <= oy || y >= oy + sy - 1)
	    y = (yd + ys) / 2;
	t = abm + (y - oy) * sx;
	for (i = 0; i <= 2 * K3; i++) {
	    if (*t & HDT << 4)
		break;
	    t++;
	}
	if (i > 2 * K3) {
	    p_free (abm);		/* failed to reach via-area	 */
	    abm = 0;
	    return (0);
	}
    }

    abm1 = abm;			/* save aux bit map 1		 */
    ox1 = ox;
    oy1 = oy;
    sx1 = sx;
    sy1 = sy;

    if (sd == s1b) {		/* start on side 1		 */
	cr_gmaze (xs, ys, xs, yd, K3);
	if (maze_run2 (xs, ys)) {/* surprise: done		 */
	    p_free (abm1);
	    p_free (abm);
	    abm = 0;
	    return (1);
	}}
    else {			/* start on side 2		 */
	cr_gmaze (xs, ys, xd, ys, K3);
	if (maze_run1 (xs, ys)) {/* surprise: done		 */
	    p_free (abm1);
	    p_free (abm);
	    abm = 0;
	    return (1);
	}
    }

    abm2 = abm;			/* save aux bit map 2		 */
    ox2 = ox;
    oy2 = oy;
    sx2 = sx;
    sy2 = sy;

    x1 = ((ox > ox1) ? ox : ox1) + 1;/* get via-hole area	 */
    y1 = ((oy > oy1) ? oy : oy1) + 1;
    x2 = ((ox + sx > ox1 + sx1) ? ox1 + sx1 : ox + sx) - 2;
    y2 = ((oy + sy > oy1 + sy1) ? oy1 + sy1 : oy + sy) - 2;

    if (x1 > x2 || y1 > y2)
	err ("b_route: 0-area", x1, y1, x2, y2);

    color (ishb | selb, ishb | selb);
    i = 0;
    for (y = (y1 + 1) & 0xfffffffe; y <= y2; y += 2)
	for (x = (x1 + 1) & 0xfffffffe; x <= x2; x += 2) {
	    t = abm + (x - ox + (y - oy) * sx);
	    t1 = abm1 + (x - ox1 + (y - oy1) * sx1);
	    if ((((sd == s1b) && (*t1 & HDT) && (*t & HDT << 4)) ||
		 ((sd == s2b) && (*t & HDT) && (*t1 & HDT << 4)))) {
		vhct[i].t = t;
		vhct[i].c = (sd == s1b) ? vh_cost (t1, sx1, t, sx) :
					  vh_cost (t, sx, t1, sx1);
		if (sd == s1b)
		    vhct[i].c += ((*t1 & 1) ? 0 : K8) +
				 ((*t & 0x10) ? 0 : K8);
		else
		    vhct[i].c += ((*t1 & 0x10) ? 0 : K8) +
				 ((*t & 1) ? 0 : K8);
		i++;
	    }
	}

    qsort (vhct, i, sizeof (vhct[0]), vh_key);

    for (j = 0; j < i; j++) {
	t = vhct[j].t;
	y = (int) (t - abm);
	x = ox + y % sx;
	y = oy + y / sx;
	if (!pin (x, y)) {	/* got via hole			 */
	    t1 = abm1 + (x - ox1 + (y - oy1) * sx1);

	    mz_done (t, (sd == s1b) ? 7 & ((*t) >> 4) : 7 & *t, vec ^ sd);

	    abm = abm1;		/* restore aux bit map 1	 */
	    ox = ox1;
	    oy = oy1;
	    sx = sx1;
	    sy = sy1;
	    set_dir ();

	    if (mz_chk (t1, (sd == s2b) ? 7 & ((*t1) >> 4) : 7 & *t1, sd)){
				/* got a problem		 */
		p_free (abm);
		abm = 0;
		cr_maze (ox, oy, sx, sy);/* redo first maze	 */
		if (((sd == s1b) && maze_run1 (x, y)) ||
		    ((sd == s2b) && maze_run2 (x, y))) { /* recovered	*/
		    p_free (abm);
		    abm = 0;
		    p_free (abm2);
		    return (1);
		}
		p_free (abm);	/* recovery failed		 */

		abm = abm2;	/* restore aux bit map 2	 */
		ox = ox2;
		oy = oy2;
		sx = sx2;
		sy = sy2;
		set_dir ();
	        mz_undone (t, (sd == s1b) ? 7 & ((*t) >> 4) : 7 & *t, vec ^ sd);
		p_free (abm);
		abm = 0;
		color (0, selb | ishb);
		dpin (x, y);	/* remove hole			 */
		return (0);
	    }
	    mz_done (t1, (sd == s2b) ? 7 & ((*t1) >> 4) : 7 & *t1, sd);
	    p_free (abm);
	    abm = 0;
	    p_free (abm2);

	    return (1);
	}
    }

    p_free (abm);
    abm = 0;
    p_free (abm1);
    return (0);			/* unsuccessful return		 */
}

vh_key (a, b)			/* sort key for via-holes	 */
    struct vht *a, *b;
{
    return ((a -> c > b -> c) - (a -> c < b -> c));
}

vh_cost (t1, sx1, t2, sx2)	/* compute via-hole cost	 */
    char *t1, *t2;
    int sx1, sx2;
{
    int     c, dirt[8];

    c = 0;			/* initial cost			 */

    dirt[0] = 1;
    dirt[1] = 1 + sx1;
    dirt[2] = sx1;
    dirt[3] = sx1 - 1;
    dirt[4] = -1;
    dirt[5] = -sx1 - 1;
    dirt[6] = -sx1;
    dirt[7] = 1 - sx1;

    do {			/* count length on side 1	 */
	c++;
	t1 += dirt[7 & *t1];
    } while (*t1 & HDT);

    dirt[1] = 1 + sx2;
    dirt[2] = sx2;
    dirt[3] = sx2 - 1;
    dirt[5] = -sx2 - 1;
    dirt[6] = -sx2;
    dirt[7] = 1 - sx2;

    do {			/* count length on side 2	 */
	c++;
	t2 += dirt[7 & ((*t2) >> 4)];
    } while (*t2 & HDT << 4);

    return (c);
}

L1_route (xs, ys, xd, yd)
    int xs, ys, xd, yd;
/***********************************************************************\
* 								        *
*  Start on side 1 for an connection between <xs,ys> and <xd,yd>.       *
*  The wire may change sides several times. L1_route calls L2_route     *
*  and vice versa. Routing is aborted if no significant progress        *
*  is made in one step. Routing starts at <xd,yd> and works backwards.  *
*  If the distance between <xs,ys> and <xd,yd> is less than K4,	        *
*  l_route is used.						        *
* 								        *
\***********************************************************************/
{
    int i, j, x, y, x1, y1, x2, y2, sx1, sy1, ox1, oy1, c, vh_key(), xl;
    char *t, *abm1;

    if (abs (ys - yd) < K1)	/* single side run only		 */
	return (a_route (xs, ys, xd, yd, s1b));

    if (abs (xd - xs) + abs (yd - ys) <= K4)
				/* short net: use l_route	 */
	return (l_route (xs, ys, xd, yd, s1b));

    if (viahcnt > K6)		/* limit the number of via holes */
	return (0);

    cr_gmaze (xd, yd, xs, yd, K3);/* create maze		 */

    if (maze_run1 (xd, yd)) {
	p_free (abm);
	abm = 0;
	return (1);		/* successful termination	 */
    }

    if (xs > xd) {		/* check direction		 */
	x1 = K3;
	x2 = sx - 2;}
    else {
	x1 = sx - K3 - 1;
	x2 = 1;
    }

    while (abs (x1 - x2) > 1) {	/* check max. extension of maze  */
	x = (x1 + x2) >> 1;
	t = abm + (sx + x);
	for (i = 0; i < 2 * K3 - 1; (t += sx, i++))
	    if (*t & HDT)
		break;
	if (i >= 2 * K3 - 1)	/* no trace			 */
	    x2 = x;
	else
	    x1 = x;
    }

    xl = x + ox;

    while (abs (xl - xd) > K5) {/* try next leg 	 	 */

	if (xs > xd) {		/* locate via-hole area		 */
	    x2 = xl;
	    x1 = xl - 2 * K3;
	}
	else {
	    x1 = xl;
	    x2 = xl + 2 * K3;
	}
	y1 = oy + 1;
	y2 = oy + sy - 2;

	i = 0;			/* collect potential via points	 */
	for (y = (y1 + 1) & 0xfffffffe; y <= y2; y += 2)
	    for (x = (x1 + 1) & 0xfffffffe; x <= x2; x += 2) {
		t = abm + (x - ox + (y - oy) * sx);
		if (*t & HDT) {
		    vhct[i].t = t;
		    c = abs (x - xs) + abs (y - ys);
		    c += (*t & 1) ? 0 : K8;
		    do {	/* count length (for cost)	 */
			c++;
			t += dir[7 & *t];
		    } while (*t & HDT);
		    vhct[i++].c = c;
		}
	    }

	qsort (vhct, i, sizeof (vhct[0]), vh_key);/* check best first */

	color (ishb | selb, ishb | selb);/* try to create a via hole */
	for (j = 0; j < i; j++) {
	    t = vhct[j].t;
	    y = (int) (t - abm);
	    x = ox + y % sx;
	    y = oy + y / sx;
	    if (!pin (x, y)) {	/* got a via hole		 */
		viahcnt++;
		mz_done (t, 7 & *t, s1b);
		abm1 = abm;	/* save bit-map			 */
		sx1 = sx;
		sy1 = sy;
		ox1 = ox;
		oy1 = oy;
		if (L2_route (xs, ys, x, y)) {/* done		 */
		    p_free (abm1);
		    abm = 0;
		    return (1);
		}		/* success			 */
		else {
		    color (0, ishb | selb);
		    dpin (x, y);/* remove pin			 */
		    viahcnt--;
		    abm = abm1;	/* restore bitmap		 */
		    sx = sx1;
		    sy = sy1;
		    ox = ox1;
		    oy = oy1;
		    set_dir ();
		    mz_undone (t, 7 & *t, s1b);
		    break;	/* failure			 */
		}
	    }
	}

	if (viahcnt > K7)
	    break;		/* no retries at this level	 */
	xl = (xl + xd) / 2;
    }
    p_free (abm);
    abm = 0;
    return (0);			/* via allocation failure	 */
}

L2_route (xs, ys, xd, yd)
    int xs, ys, xd, yd;
/***********************************************************************\
* 								        *
*  Start on side 2 for an connection between <xs,ys> and <xd,yd>.       *
*  The wire may change sides several times. L2_route calls L1_route     *
*  and vice versa. Routing is aborted if no significant progress        *
*  is made in one step. Routing starts at <xd,yd> and works backwards.  *
*  If the distance between <xs,ys> and <xd,yd> is less than K4,	        *
*  l_route is used.						        *
* 								        *
\***********************************************************************/
{
    int i, j, x, y, x1, y1, x2, y2, sx1, sy1, ox1, oy1, c, vh_key(), yl;
    char *t, *abm1;

    if (abs (xs - xd) < K1)	/* single side run only		 */
	return (a_route (xs, ys, xd, yd, s2b));

    if (abs (xd - xs) + abs (yd - ys) <= K4)
				/* short net: use l_route	 */
	return (l_route (xs, ys, xd, yd, s2b));

    if (viahcnt > K6)
	return (0);		/* too many via holes		 */

    cr_gmaze (xd, yd, xd, ys, K3);/* create maze		 */

    if (maze_run2 (xd, yd)) {
	p_free (abm);
	abm = 0;
	return (1);		/* successful termination	 */
    }

    if (ys > yd) {		/* check direction		 */
	y1 = K3;
	y2 = sy - 2;}
    else {
	y1 = sy - K3 - 1;
	y2 = 1;
    }

    while (abs (y1 - y2) > 1) {	/* check max. extension of maze  */
	y = (y1 + y2) >> 1;
	t = abm + (sx * y + 1);
	for (i = 0; i < 2 * K3 - 1; (t++, i++))
	    if (*t & HDT << 4)
		break;
	if (i >= 2 * K3 - 1)	/* no trace			 */
	    y2 = y;
	else
	    y1 = y;
    }

    yl = y + oy;

    while (abs (yl - yd) > K5) {/* try next leg			 */

	if (ys > yd) {		/* locate via-hole area		 */
	    y2 = yl;
	    y1 = yl - 2 * K3;}
	else {
	    y1 = yl;
	    y2 = yl + 2 * K3;
	}
	x1 = ox + 1;
	x2 = ox + sx - 2;

	i = 0;			/* collect potential via points	 */
	for (y = (y1 + 1) & 0xfffffffe; y <= y2; y += 2)
	    for (x = (x1 + 1) & 0xfffffffe; x <= x2; x += 2) {
		t = abm + (x - ox + (y - oy) * sx);
		if (*t & HDT << 4) {
		    vhct[i].t = t;
		    c = abs (x - xs) + abs (y - ys);
		    c += (*t & 0x10) ? 0 : K8;
		    do {	/* count length (for cost)	 */
			c++;
			t += dir[7 & ((*t) >> 4)];
		    } while (*t & HDT << 4);
		    vhct[i++].c = c;
		}
	    }

	qsort (vhct, i, sizeof (vhct[0]), vh_key);/* check best first */

	color (ishb | selb, ishb | selb);/* try to create a via hole */
	for (j = 0; j < i; j++) {
	    t = vhct[j].t;
	    y = (int) (t - abm);
	    x = ox + y % sx;
	    y = oy + y / sx;
	    if (!pin (x, y)) {	/* got a via hole		 */
		viahcnt++;
		mz_done (t, 7 & ((*t) >> 4), s2b);
		abm1 = abm;	/* save bit-map			 */
		sx1 = sx;
		sy1 = sy;
		ox1 = ox;
		oy1 = oy;
		if (L1_route (xs, ys, x, y)) {/* done		 */
		    p_free (abm1);
		    return (1);
		}		/* success			 */
		else {
		    color (0, ishb | selb);
		    dpin (x, y);/* remove pin			 */
		    viahcnt--;
		    abm = abm1;	/* restore bitmap		 */
		    sx = sx1;
		    sy = sy1;
		    ox = ox1;
		    oy = oy1;
		    set_dir ();
		    mz_undone (t, 7 & ((*t) >> 4), s2b);
		    break;	/* failure			 */
		}
	    }
	}

	if (viahcnt > K7)
	    break;		/* no retries at this level	 */
	yl = (yl + yd) / 2;
    }

    p_free (abm);
    abm = 0;
    return (0);			/* via allocation failure	 */
}
