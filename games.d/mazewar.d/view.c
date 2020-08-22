#ifdef SCCS
static char *sccsid = "@(#)view.c	1.1	4/5/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "war.h"

/*
 * See what is currently visible by all of our men.
 */
view()
{
	register struct	cell	*cc;	/* current cell */
	register struct	cell	*tc;	/* test cell */
	register struct	object	*obj;	/* object doing viewing */
	register int	row;		/* current row */
	register int	col;		/* current column */
	register int	view;		/* viewing distance */
	int	objrow;			/* current row of object */
	int	objcol;			/* current column of object */
	int	minrow;			/* minimum row for search */
	int	maxrow;			/* maximum row for search */
	int	mincol;			/* minimum column for search */
	int	maxcol;			/* maximum column for search */

	/*
	 * Remove visibility of movable objects
	 */
	for (obj = objects; obj < endobjects; obj++) {
		if (obj->o_side == myside) continue;
		if (obj->o_flags & (F_IMMOB|F_VIS)) continue;
		cc = obj->o_cell;
		if ((cc == NULL) || (cc->c_seen == 0)) continue;
		cc->c_seen = 0;
		dpyplace(cc->c_row, cc->c_col, ' ');
	}
	/*
	 * See what is visible now
	 */
	for (cc = firstcell; cc; cc = cc->c_next) {
		obj = cc->c_obj;
		if ((obj == NULL) || (obj->o_side != myside)) continue;
		view = obj->o_view;
		if (view <= 0) continue;
		objrow = cc->c_row;
		objcol = cc->c_col;
		minrow = objrow - view;
		maxrow = objrow + view;
		mincol = objcol - view;
		maxcol = objcol + view;
		if (minrow < 0) minrow = 0;
		if (maxrow >= ROWS) maxrow = ROWS - 1;
		if (mincol < 0) mincol = 0;
		if (maxcol >= COLS) maxcol = COLS - 1;
		for (row = minrow; row <= maxrow; row++) {
			col = mincol;
			tc = &board[row][col];
			for (; col <= maxcol; col++, tc = tc->c_right) {
				if (tc->c_obj == NULL) continue;
				if (tc->c_seen) continue;
				if (tc->c_obj->o_side == myside) continue;
				if (((obj->o_flags & F_XRAY) == 0)
					&& obstruct(objcol, objrow, col, row))
						continue;
				tc->c_seen = 1;
				dpyplace(row, col, tc->c_obj->o_altch);
			}
		}
	}
	viewstats();
	dpyupdate();
}


/*
 * Given a rectangle specified by its opposite corners (a,b) and (c,d).
 * Check whether there exists any object in the rectangle which is within
 * a distance of 1/2 from the line segment connecting the two corners (the
 * corners themselves are not tested).  The equation for those points (x,y)
 * which satisfy this distance requirement is:
 * 
 *	4 * square((b-d)x + (c-a)y + (ad-bc)) <= square(b-d) + square(c-a).
 *
 * By precomputing the constant parts of this equation, it becomes:
 *
 *	4 * square(Ax + By + C) <= D.
 *
 * Returns nonzero if some object is within this distance.
 */
obstruct(a, b, c, d)
{
	register int	x;		/* x coordinate of test point */
	register int	y;		/* y coordinate of test point */
	register int	i;		/* number being computed */
	register struct	cell	*cc;	/* current cell being checked */
	struct	cell	*endc1;		/* cell at first endpoint */
	struct	cell	*endc2;		/* cell at last endpoint */
	int	A;			/* (b - d) */
	int	B;			/* (c - a) */
	int	C;			/* (ad - bc) */
	int	D;			/* (square(b-d) + square(c-a)) */
	int	dy;			/* delta for y coordinate */

	if (a > c) {			/* put first point at left */
		i = c;
		c = a;
		a = i;
	}
	dy = 1;				/* see if line rises or falls */
	if (d < b) dy = -1;
	A = b - d;			/* compute constants */
	B = c - a;
	C = (a * d) - (b * c);
	D = (A * A) + (B * B);
	endc1 = &board[b][a];		/* get cells at endpoints */
	endc2 = &board[d][c];
	for (y = b; ; y += dy) {
		for (x = a, cc = &board[y][x]; x <= c; x++, cc = cc->c_right) {
			if (cc == endc2) return(0);
			if ((cc->c_obj == NULL) || (cc == endc1)) continue;
			if (cc->c_obj->o_flags & F_TRANS) continue;
			i = (A * x) + (B * y) + C;
			if ((i * i * 4) <= D) return(1);
		}
	}
}


/*
 * Update the status information if necessary
 */
viewstats()
{

	if (newstat == 0) return;
	viewobjects();
	viewdata();
	dpywindow(0, -1, 1, INFOCOL - 2);
	newstat = 0;
}


/*
 * Give comparisons between our men and the enemy
 */
viewdata()
{
	register struct	data	*md;	/* pointer to my data */
	register struct	data	*hd;	/* pointer to his data */

	dpywindow(DATAROW, -1, INFOCOL, -1);
	md = &mydata;
	hd = &hisdata;
	dpyprintf("STATISTIC       YOU   ENEMY\n");
	dpyprintf("fight men/life %2d/%-3d %2d/%d\n",
		md->d_fightmen, md->d_fightlife,
		hd->d_fightmen, hd->d_fightlife);
	dpyprintf("move men/life  %2d/%-3d %2d/%d\n",
		md->d_movemen, md->d_movelife,
		hd->d_movemen, hd->d_movelife);
	dpyprintf("blast men/life %2d/%-3d %2d/%d\n",
		md->d_blastmen, md->d_blastlife,
		hd->d_blastmen, hd->d_blastlife);
	dpyprintf("goal men/life  %2d/%-3d %2d/%d\n",
		md->d_goalmen, md->d_goallife,
		hd->d_goalmen, hd->d_goallife);
	dpyprintf("total men/life %2d/%-3d %2d/%d\n",
		md->d_totalmen, md->d_totallife,
		hd->d_totalmen, hd->d_totallife);
	dpyprintf("total walls      %-3d    %-3d\n",
		md->d_totalwalls, hd->d_totalwalls);
	dpyclrwindow();
}


/*
 * Give statistics on our objects
 */
viewobjects()
{
	register struct	object	*obj;	/* current object */
	register char	*cp;		/* current flag character */
	register int	flags;		/* current flags */
	char	flagchars[20];		/* flags for object */
	char	count[20];		/* value for counts */

	dpywindow(0, DATAROW - 2, INFOCOL, -1);
	dpystr("OBJ LIFE VIEW FLAGS  COUNT\n");
	for (obj = objects; obj < endobjects; obj++) {
		if (obj->o_side != myside) continue;
		if (playing && (obj->o_count == 0)) continue;
		cp = flagchars;
		flags = obj->o_flags;
		if (flags & F_BLAST) *cp++ = 'b';
		if (flags & F_FIGHT) *cp++ = 'f';
		if (flags & F_GOAL) *cp++ = 'g';
		if (flags & F_IMMOB) *cp++ = 'i';
		if (flags & F_TRANS) *cp++ = 't';
		if (flags & F_VIS) *cp++ = 'v';
		if (flags & F_WALL) *cp++ = 'w';
		if (flags & F_XRAY) *cp++ = 'x';
		*cp = '\0';
		cp = count;
		if (playing)
			sprintf(cp, "%d  ", obj->o_count);
		else if (obj->o_min == obj->o_max)
			sprintf(cp, "%d  ", obj->o_min);
		else if (obj->o_max >= 1000)
			sprintf(cp, "%d-INF", obj->o_min);
		else
			sprintf(cp, "%d-%d", obj->o_min, obj->o_max);
		dpyprintf("%c %c %3d %4d   %-7s%s\n",
			obj->o_ownch, obj->o_altch, obj->o_life,
			obj->o_view, flagchars, count);
	}
	dpyclrwindow();
}
