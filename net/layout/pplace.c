/***************************************************************\
*								*
*	PCB program						*
*								*
*	Placement routines					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"

int rot_m[4][4] = { { 1,  0,  0,  1},
		    { 0, -1,  1,  0},
		    {-1,  0,  0, -1},
		    { 0,  1, -1,  0}  };


fnd_cmp (x, y)			/* find component		 */
    int x, y;
{
    register int i, xl, xh, yl, yh;

    for (i = 0; i < V.ncp; i++) {
	switch (CP[i].r) {
	    case 0:
		xl = CP[i].x;
		yl = CP[i].y;
		xh = xl + CP[i].ty -> x;
		yh = yl + CP[i].ty -> y;
		break;
	    case 1:
		xh = CP[i].x;
		yl = CP[i].y;
		xl = xh - CP[i].ty -> y;
		yh = yl + CP[i].ty -> x;
		break;
	    case 2:
		xh = CP[i].x;
		yh = CP[i].y;
		xl = xh - CP[i].ty -> x;
		yl = yh - CP[i].ty -> y;
		break;
	    case 3:
		xl = CP[i].x;
		yh = CP[i].y;
		xh = xl + CP[i].ty -> y;
		yl = yh - CP[i].ty -> x;
		break;
	}
	xl--;
	yl--;
	xh++;
	yh++;
	if (x <= xh && x >= xl && y <= yh && y >= yl)
	    return (i);
    }
    return (-1);		/* no component found		 */
}

draw_cp (x, y, r, cp)		/* draw component shape		 */
    int x, y, r, cp;
{
    register int i, xl, yl, xh, yh;
    register struct pin *pp;
    int j;

    switch (r) {
	case 0: 
	    xl = x;
	    yl = y;
	    xh = xl + CP[cp].ty -> x;
	    yh = yl + CP[cp].ty -> y;
	    break;
	case 1: 
	    xh = x;
	    yl = y;
	    xl = xh - CP[cp].ty -> y;
	    yh = yl + CP[cp].ty -> x;
	    break;
	case 2: 
	    xh = x;
	    yh = y;
	    xl = xh - CP[cp].ty -> x;
	    yl = yh - CP[cp].ty -> y;
	    break;
	case 3: 
	    xl = x;
	    yh = y;
	    xh = xl + CP[cp].ty -> y;
	    yl = yh - CP[cp].ty -> x;
	    break;
    }

    xl -= 2;
    yl -= 2;
    xh += 2;
    yh += 2;

    color (resb, resb);
    plts (xl, yl, xh, yl);
    plts (xh, yl, xh, yh);
    plts (xh, yh, xl, yh);
    plts (xl, yh, xl, yl);

    for (j = 0, i = CP[cp].ty -> np, pp = CP[cp].ty -> p; i; i--, pp++) {
	xl = x + (pp -> x) * rot_m[r][0] + (pp -> y) * rot_m[r][1];
	yl = y + (pp -> x) * rot_m[r][2] + (pp -> y) * rot_m[r][3];
	if (!j) {
	    j = 1;
	    dot (xl, yl);
	}
	else
	    point (xl, yl);
    }
}

del_cp (x, y, r, cp)		/* delete component shape	 */
    int x, y, r, cp;
{
    register int xl, yl, xh, yh;

    switch (r) {
	case 0: 
	    xl = x;
	    yl = y;
	    xh = xl + CP[cp].ty -> x;
	    yh = yl + CP[cp].ty -> y;
	    break;
	case 1: 
	    xh = x;
	    yl = y;
	    xl = xh - CP[cp].ty -> y;
	    yh = yl + CP[cp].ty -> x;
	    break;
	case 2: 
	    xh = x;
	    yh = y;
	    xl = xh - CP[cp].ty -> x;
	    yl = yh - CP[cp].ty -> y;
	    break;
	case 3: 
	    xl = x;
	    yh = y;
	    xh = xl + CP[cp].ty -> y;
	    yl = yh - CP[cp].ty -> x;
	    break;
    }

    color (0, resb);
    c_rect (xl - 2, yl - 2, xh + 2, yh + 2);
}

static int active = 0;		/* !=0 if some coponent is sel	 */
static int cp_id, cp_x, cp_y, cp_r;

cmv_sel (x, y, ctx)		/* select a component		 */
    int x, y, ctx;
{
    char buf[nmmax + nmmax + 3];

    if (active)			/* remove previous selection	 */
	del_cp (cp_x, cp_y, cp_r, cp_id);

    cp_id = fnd_cmp (x, y);

    if (cp_id < 0) {
	active = 0;
	err_msg ("No component found");}
    else {
	active = 1;
	cp_x = CP[cp_id].x;
	cp_y = CP[cp_id].y;
	cp_r = CP[cp_id].r;
	draw_cp (cp_x, cp_y, cp_r, cp_id);

	sprintf (buf, "%s (%s)", CP[cp_id].name, CP[cp_id].ty -> name);
	err_msg (buf);
    }

    return (ctx);
}

/*ARGSUSED*/
exc_cmv (x, y, ctx)		/* execute CP move		 */
    int x, y, ctx;
{
    register int i, x1, y1, or;
    register struct pin *pp;

    if (!active)
	return START;		/* nothing to do		 */

    del_cp (cp_x, cp_y, cp_r, cp_id);
    active = 0;

    if (CP[cp_id].r == cp_r && CP[cp_id].x == cp_x && CP[cp_id].y == cp_y) {
	err_msg ("Not moved");
	return START;		/* move to same location	 */
    }

    or = CP[cp_id].r;		/* get old rotation		 */

    for (i = CP[cp_id].ty -> np, pp = CP[cp_id].ty -> p; i; i--, pp++) {

				/* check alignment of new location */
	x1 = cp_x + (pp -> x) * rot_m[cp_r][0] + (pp -> y) * rot_m[cp_r][1];
	y1 = cp_y + (pp -> x) * rot_m[cp_r][2] + (pp -> y) * rot_m[cp_r][3];
	if (cp_alchk (x1) || cp_alchk (y1)) {
	    err_msg ("Pins not on drill grid");
	    beep ();
	    return START;
	}

				/* check range			 */
	if (x1 < 3 || y1 < 3 || x1 > V.BSX - 3 || y1 > V.BSY - 3) {
	    err_msg ("Can't move off board");
	    beep ();
	    return START;
	}
	
				/* check for wires		 */
	x1 = CP[cp_id].x + pp -> x * rot_m[or][0] + pp -> y * rot_m[or][1];
	y1 = CP[cp_id].y + pp -> x * rot_m[or][2] + pp -> y * rot_m[or][3];
	ckgp (x1, y1, vec);	/* ignore left over grabage	 */
	if (pcb[y1][x1] & (s1b | s2b)) {
	    err_msg ("Disconnect wires first");
	    beep ();
	    return START;
	}
    }

    x1 = CP[cp_id].x;		/* save old place		 */
    y1 = CP[cp_id].y;
    i  = CP[cp_id].r;

    if (!C_unplace (&CP[cp_id]))
	err ("exc_cmv: this should not happen", cp_id, 0, 0, 0);

    if (!C_place (&CP[cp_id], cp_x, cp_y, cp_r)) {
	err_msg ("No space at new place");
	beep ();
	if (!C_place (&CP[cp_id], x1, y1, i))
	    err ("exc_cmv: re-insert to old place failed", x1, y1, i, cp_id);
    }

    CH_update ();

    return START;
}


/*ARGSUSED*/
cmv_rr (x, y, ctx)		/* rotate right			 */
    int x, y, ctx;
{
    if (active) {
	del_cp (cp_x, cp_y, cp_r, cp_id);
	cp_r = 3 & (cp_r + 3);
	draw_cp (cp_x, cp_y, cp_r, cp_id);}
    else
	err_msg ("Select a CP first");

    return (ctx);
}

/*ARGSUSED*/
cmv_rl (x, y, ctx)		/* rotate left			 */
    int x, y, ctx;
{
    if (active) {
	del_cp (cp_x, cp_y, cp_r, cp_id);
	cp_r = 3 & (cp_r + 1);
	draw_cp (cp_x, cp_y, cp_r, cp_id);}
    else
	err_msg ("Select a CP first");

    return (ctx);
}

cmv_plc (x, y, ctx)		/* place component		 */
    int x, y, ctx;
{
    char buf[40];

    if (active) {
	del_cp (cp_x, cp_y, cp_r, cp_id);
	cp_x = cp_align (x);
	cp_y = cp_align (y);
	draw_cp (cp_x, cp_y, cp_r, cp_id);
	sprintf (buf, "X=%6.3f\" Y=%6.3f\"",
		((float) x / rsun) * 0.1, ((float) y / rsun) * 0.1);
	err_msg (buf);}
    else
	err_msg ("Select a CP first");

    return (ctx);
}

C_place (cmp, x, y, r)
    struct comp *cmp;
    int x, y, r;
/****************************************************************\
* 								 *
*  C_place tries to place the component <cmp> on location <x,y>	 *
*  with rotation <r>. If it succeedes, the component entry will	 *
*  be updated and a '1' is returned. A returned '0' indicates	 *
*  that the desired placement was not possible due to design	 *
*  rule constraints.						 *
* 								 *
\****************************************************************/
{
#define xtr(a, b) (x + a * rot_m[r][0] + b * rot_m[r][1])
#define ytr(a, b) (y + a * rot_m[r][2] + b * rot_m[r][3])

    register struct pin *pp;
    register int i, j, k, x1, y1;
    int x2, y2;

    if (!(cmp -> unplaced))
	err ("C_place: already placed", cmp, x, y, r);

    if (r < 0 || r > 3 || x < 3 || y < 3 || x > V.BSX - 3 || y > V.BSX - 3)
	return 0;		/* illegal paramenter		 */

    for (i = cmp -> ty -> np, pp = cmp -> ty -> p; i; pp++, i--) {
	x1 = xtr (pp -> x, pp -> y);
	y1 = ytr (pp -> x, pp -> y);

	if( x1 < 3 || x1 > V.BSX - 3 || y1 < 3 || y1 > V.BSY - 3)
	    return 0;		/* pin is off board		 */

	if (cp_alchk (x1) || cp_alchk (y1))
	    return 0;		/* illegal pin alignment	 */

	if (!ck_pin (x1, y1))
	    return 0;		/* overlapp problem		 */
    }

    if (cmp -> ty -> cif)	/* external CIF type: block check*/
	for (i = cmp -> ty -> cif - 1; CIF[i].blk; i++) {
	    for (y1 = CIF[i].yl - 1; y1 <= CIF[i].yh + 1; y1++)
		for (x1 = CIF[i].xl - 1; x1 <= CIF[i].xh + 1; x1++)
		    if (pcb[ytr (x1, y1)][xtr (x1, y1)] & (fb | ahb | vec))
			return 0;	/* Block problem	 */
	    if (!CIF[i].flg)
		break;
	}

    /**** everthing looks fine: proceed to insert component   ****/

    color (chb, chb);		/* insert pins			 */
    for (i = cmp -> ty -> np, pp = cmp -> ty -> p; i; pp++, i--)
	if (pin (xtr (pp -> x, pp -> y), ytr (pp -> x, pp -> y))) {
	    printf ("Type definition error for '%s': pins too close\n",
		cmp -> ty -> name);
	    color (0, chb);
	    for (pp = cmp -> ty -> p; i < cmp -> ty -> np; pp++, i++)
		dpin (xtr (pp -> x, pp -> y), ytr (pp -> x, pp -> y));
	    return 0;
	}

    if (cmp -> ty -> cif) {	/* insert road blocks		 */
	color (fb, fb);
	for (i = cmp -> ty -> cif - 1; CIF[i].blk; i++) {
	    x1 = xtr (CIF[i].xl, CIF[i].yl);
	    y1 = ytr (CIF[i].xl, CIF[i].yl);
	    x2 = xtr (CIF[i].xh, CIF[i].yh);
	    y2 = ytr (CIF[i].xh, CIF[i].yh);
	    if (x1 > x2) {j = x1; x1 = x2; x2 = j;}
	    if (y1 > y2) {j = y1; y1 = y2; y2 = j;}

	    c_rect (x1, y1, x2, y2);
	    for (; y1 <= y2; y1++)
		for (k = x1; k <= x2; k++)
		    pcb[y1][k] |= fb;

	    if (!CIF[i].flg)
		break;
	}
    }

    k = ((int) cmp - (int) CP) / sizeof (struct comp);	/* get own index */
    pp = cmp -> ty -> p;
    for (i = 0, j = 0; i < V.nch; i++)
	if (CH[i].cpi == k) {
	    j++;			/* count holes	 */
	    CH[i].x = xtr (pp[CH[i].pn].x, pp[CH[i].pn].y);
	    CH[i].y = ytr (pp[CH[i].pn].x, pp[CH[i].pn].y);
	}
    if (j != cmp -> ty -> np)
	err ("C_place: inconsistent pin numbers", j, cmp -> ty -> np, 0, 0);

    cmp -> x = x;		/* update component entry	 */
    cmp -> y = y;
    cmp -> r = r;
    cmp -> unplaced = 0;

    placed = 0;			/* insure update		 */

    return 1;			/* finally done!		 */

#undef xtr
#undef ytr
}

C_unplace (cmp)
    struct comp *cmp;
/***************************************************************\
* 							        *
*  C_unplace tries to remove the component <cmp> from its       *
*  current location. It returns a '1' if it succeeds.	        *
*  A returned '0' indicates that the opaeration failed because  *
*  the component had wires connected to it.		        *
* 							        *
\***************************************************************/
{
#define xtr(a, b) (x + a * rot_m[r][0] + b * rot_m[r][1])
#define ytr(a, b) (y + a * rot_m[r][2] + b * rot_m[r][3])

    register struct pin *pp;
    register int i, x1, x = cmp -> x, y = cmp -> y, r = cmp -> r;
    int j, y1, x2, y2;
    struct hole *fndh (), *hp;
    static struct hole **chl = 0;
    static int chlmax = 100;

    if (cmp -> unplaced)
	return 1;		/* is already unplaced		 */

    for (i = cmp -> ty -> np, pp = cmp -> ty -> p; i; pp++, i--) {
	if (pcb[ytr (pp -> x, pp -> y)][xtr (pp -> x, pp -> y)] & vec)
	    return 0;		/* no wires allowed		 */

    /**** everthing looks fine: proceed to remove component   ****/

    if (!chl) {			/* allocate temp table		 */
	if (chlmax < cmp -> ty -> np)
	    chlmax = 10 + cmp -> ty -> np;
	chl = (struct hole **) p_malloc (sizeof (struct hole *) * chlmax);}
    else if (chlmax < cmp -> ty -> np) {
	p_free (chl);
	chlmax = 10 + cmp -> ty -> np;
	chl = (struct hole **) p_malloc (sizeof (struct hole *) * chlmax);}
    }

    color (0, selb | chb);	/* remove pins			 */
    for (i = cmp -> ty -> np, pp = cmp -> ty -> p; i; pp++) {
	x1 = xtr (pp -> x, pp -> y);
	y1 = ytr (pp -> x, pp -> y);
	hp = fndh (x1, y1);
	if (!hp)
	    err ("C_unplace: missing hole", x1, y1, i, 0);
	chl[--i] = hp;		/* save hole pointer		 */
	dpin (x1, y1);
    }

    for (i = cmp -> ty -> np; i;) {
	chl[--i] -> x = 0;	/* invalidate cooradinates	 */
	chl[i] -> y = 0;
    }

    if (cmp -> ty -> cif) {	/* remove road blocks		 */
	color (0, fb);
	for (i = cmp -> ty -> cif - 1; CIF[i].blk; i++) {
	    x1 = xtr (CIF[i].xl, CIF[i].yl);
	    y1 = ytr (CIF[i].xl, CIF[i].yl);
	    x2 = xtr (CIF[i].xh, CIF[i].yh);
	    y2 = ytr (CIF[i].xh, CIF[i].yh);
	    if (x1 > x2) {j = x1; x1 = x2; x2 = j;}
	    if (y1 > y2) {j = y1; y1 = y2; y2 = j;}

	    c_rect (x1, y1, x2, y2);
	    for (; y1 <= y2; y1++)
		for (j = x1; j <= x2; j++)
		    pcb[y1][j] &= ~fb;

	    if (!CIF[i].flg)
		break;
	}
    }

    placed = 0;			/* no longer placed		 */
    cmp -> unplaced = 1;
    flush_stat ();		/* statistics are invalid	 */

    return 1;			/* finally done!		 */

#undef xtr
#undef ytr
}

CH_update ()
/****************************************************\
* 						     *
*  Sort the hole table and update the pointer to it  *
*  (this is prerequisite for 'fndh' to work)	     *
* 						     *
\****************************************************/
{
    register int i;
    int key ();

    qsort (CH, V.nch, sizeof (CH[0]), key);	/* sort holes	 */
    for (i = 0; i < V.nch; i++)			/* adj pointers  */
	if (CH[i].n)
	    CH[i].n -> c = &CH[i];
}

key (h1, h2)			/* is used to sort the holes	*/
    struct hole *h1, *h2;
{
    if (h1 -> x > h2 -> x)
	return 1;
    if (h1 -> x < h2 -> x)
	return -1;
    if (h1 -> y > h2 -> y)
	return 1;
    if (h1 -> y < h2 -> y)
	return -1;

    return 0;
}

ck_placed ()
/****************************************************************\
* 								 *
*  Check placement:						 *
*   returns a '1' if all components are placed (and updates the	 *
*   net location info). A '0' is returned otherwise.		 *
* 								 *
\****************************************************************/
{
    register int i, j, xl, yl, xh;
    register struct nlst *p;
    int yh;

    for (i = 0, xl = V.ncp; i < xl; i++)
	if (CP[i].unplaced)
	    return 0;

    for (i = 0; i < V.nnh; i++)	/* update net locations		*/
	if (NH[i].l) {
	    xl = xmax;
	    yl = ymax;
	    xh = 0;
	    yh = 0;
	    for (p = NH[i].lp; p; p = p -> n) {
		j = p -> c -> x;
		if (j > xh) xh = j;
		if (j < xl) xl = j;
		j = p -> c -> y;
		if (j > yh) yh = j;
		if (j < yl) yl = j;
	    }
	    if (!xl)
		err ("ck_placed: invlid hole coordinate", i, xl, yl, xh);
	    NH[i].x1 = xl;
	    NH[i].x2 = xh;
	    NH[i].y1 = yl;
	    NH[i].y2 = yh;
	}

    return 1;
}

CPp_ini (ctx)			/* CP place initialization	 */
    int ctx;
{
    register int i;
    char buf[nmmax + nmmax + 3];

    if (ck_placed ()) {
	active = 0;
	return START;		/* nothing to place		 */
    }

    for (i = 0; i < V.ncp; i++)	/* find something to place	 */
	if (CP[i].unplaced)
	    break;

    cp_id = i;
    cp_x = wx + 256 / cz;
    cp_y = wy + 241 / cz;
    cp_r = 0;

    active = 1;
    draw_cp (cp_x, cp_y, 0, i);

    sprintf (buf, "%s (%s)", CP[cp_id].name, CP[cp_id].ty -> name);
    err_msg (buf);

    return ctx;
}

/*ARGSUSED*/
CPp_exc (x, y, ctx)		/* try some other component	 */
    int x, y, ctx;
{
    if (C_place (&CP[cp_id], cp_x, cp_y, cp_r)) {
	del_cp (cp_x, cp_y, cp_r, cp_id);
	CH_update ();
	return CPp_ini (ctx);
    }

    err_msg ("Invalid place");

    return ctx;
}

CPp_del (x, y, ctx)		/* delete a component		 */
    int x, y, ctx;
{
    register int i, j, k;
    char buf[nmmax + nmmax + 3];

    i = fnd_cmp (x, y);

    if (i < 0) {		/* try to select an other cmp	 */
	k = V.ncp;
	for (i = 0, j = cp_id + 1; i < k; i++)
	    if (CP[(i + j) % k].unplaced)
		break;
	i = (i + j) % k;
	if (i == cp_id)
	    err_msg ("only one CMP left");
	else {
	    del_cp (cp_x, cp_y, cp_r, cp_id);

	    cp_id = i;
	    cp_x = x;
	    cp_y = y;
	    cp_r = 0;

	    draw_cp (x, y, 0, i);

	    sprintf (buf, "%s (%s)", CP[cp_id].name, CP[cp_id].ty -> name);
	    err_msg (buf);
	}}

    else if (!C_unplace (&CP[i]))	/* delete component		 */
	err_msg ("Remove wires first");

    CH_update ();

    return ctx;
}

CPp_plc (x, y, ctx)		/* tennativly place component	 */
    int x, y, ctx;
{
    char    buf[40];

    del_cp (cp_x, cp_y, cp_r, cp_id);
    cp_x = cp_align (x);
    cp_y = cp_align (y);
    draw_cp (cp_x, cp_y, cp_r, cp_id);
    sprintf (buf, "X=%6.3f\" Y=%6.3f\"",
	    ((float) x / rsun) * 0.1, ((float) y / rsun) * 0.1);
    err_msg (buf);

    return ctx;
}

