/***************************************************************\
*								*
*	PCB program						*
*								*
*	Manual routing functions				*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include "pparm.h"
#include "pcdst.h"

#define maxtrc 200		/* number of items in trace buf	 */
#define sactrc 10		/* items lost on overflow	 */

extern double atan2();
extern struct nlhd *choose(), *get_net();

static plow_f = 0;		/* plow flag			 */
static int trc_cnt = 0;		/* trace counter		 */
static struct trace {		/* trace buffer (for delete last)*/
    int x, y;			/* coordinate			 */
    int ty;			/* type (hole, side)		 */
} trc_buf[maxtrc];

static int sof_seg = 0;		/* color of first segment	 */

extern int sp[9][2];		/* search pattern		 */

/*ARGSUSED*/
net_sel (ctx)			/* select a net for routing	 */
    int ctx;
{
    struct nlhd *t;

    if (!placed) {		/* routing only after placement	 */
	if (V.cnet)
	    deseln (V.cnet);	/* should not happen		 */
	if (ck_placed ())
	    placed = 1;		/* everything is placed		 */
	else
	    return PLACE;
    }

    if (V.cnet) { 		/* we are within a net		 */
	if (!(V.cnet -> f))
	    return (START);	/* continue on selected net	 */
	else
	    deseln (V.cnet);
    }

    while (t = choose ()) {/* found a net to work on	 */
	if (!selnet (t)) {
            adjw (V.cnet);
	    err_msg (t -> name);
	    return (START);
	}
	deseln (t);		/* skip complete nets		 */
    }
    return (EDIT);		/* no nets found		 */
}

net_desel (ctx)			/* deselect a net		 */
    int ctx;
/******************************************************************\
* 								   *
*  Eficiency hack: to avoid a deselec/select cycle for incomplete  *
*  nets, those net remain selected if the next context is START.   *
* 								   *
\******************************************************************/
{   int i;
    static int svcnt = 0;	/* auto save counter		 */

    plow_f = 0;

    if (ctx == ROUTE || ctx == PLOW)	/* don't deselect to enter R-cntx*/
	return;

    if (V.cnet) {
	i = cchk (V.cnet);	/* update net state		 */
	nettgo -= i ^ V.cnet -> f;
	V.cnet -> f = i;
	nets_msg (nettgo);

	if (V.cnet -> f) {	/* completed net ?		 */
	    if (++svcnt > savcnt) {	/* time to auto save ?	 */
		svcnt = 0;
		Ferr_msg ("Busy: saving work");
		save (0);
		beep ();
		err_msg ("Done!");
	    }
	}

	if (V.cnet -> f || ctx != START) {
	    deseln (V.cnet);	/* time to drop the net		 */
	}
    }
}

/*ARGSUSED*/
start_nt (x, y, ctx)		/* start a copper trace		 */
    int x, y, ctx;
/*****************************************************************\
* 								  *
*  This function tries to find a start point for a copper trace:  *
*    1. if the cursor is near a hole and the hole is part of an	  *
*       unconnected net, the hole is selected as a start.	  *
*    2. if the cursor is near a trace of an unfinished net, the	  *
*       nearest point of that trace is used as an start point.	  *
*       this might fail due to lack of space for a via hole.	  *
*    3. If everything fails, the preview map is displayed.	  *
* 								  *
\*****************************************************************/
{
    struct nlhd *p, *loc (), *fly ();
    char buf[40 + nmmax];

    sof_seg = 0;		/* reset first color		 */

    if (plow_f && trc_buf[0].x == x && trc_buf[0].y == y &&
	!(pcb[y][x] & ahb)) {
	plow_f = 0;
	plow_ini (PLOW);
	return plow_src (x, y, PLOW);
    }

    if (V.cnet -> f)		/* deselect a complted net	 */
	deseln (V.cnet);

    p = loc (&x, &y);		/* try start from hole		 */

    if (!p)
	p = fly (&x, &y);	/* failed, try a wire		 */

    if (!V.cnet) {		/* no net			 */
	if (p && !(p -> f))
	    selnet (p);
	else if (!p) {		/* p != 0 is taken care of later */
	    plow_f = 0;
	    return (net_sel (0));
	}
    }

    if (!p)	 {		/* start failed			 */
	plow_f = 0;
	prev (V.cnet, x, y);	/* display preview map		 */
	return (START);}
    else if (p -> f) {		/* net is already done		 */
	plow_f = 1;
	trc_buf[0].x = x;
	trc_buf[0].y = y;
	sprintf (buf, "Net (%s): done!", p -> name);
	err_msg (buf);
	if (V.cnet)
	    deseln (V.cnet);
	selnet (p);
	return (START);}
    else {			/* start ok: save start point	 */
	plow_f = 0;
	err_msg (p -> name);
	trc_buf[0].x = x;
	trc_buf[0].y = y;
	trc_buf[0].ty = (pcb[y][x] & ahb) ? vec :
			((~pcb[y][x] & vec) ? pcb[y][x] & vec : s1b);
	trc_cnt = 1;
    }

    if (p != V.cnet) {		/* started on unselected net	 */
	deseln (V.cnet);	/* drop current net		 */
	if (selnet (p)) {	/* trouble: net is done		 */
	    deseln (p);
	    return (net_sel (0));
	}
    }

    return (ROUTE);
}

struct nlhd *fly (x, y)		/* try flying start		 */
    int *x, *y;
{
    int     i;

    for (i = 0; i < 9; ++i)	/* look for a selected wire	 */
	if ((pcb[*y + sp[i][1]][*x + sp[i][0]] & vec) &&
	    (pcb[*y + sp[i][1]][*x + sp[i][0]] & selb)) {
		*x += sp[i][0];
		*y += sp[i][1];
		return (V.cnet);
	    }

    for (i = 0; i < 9; ++i)	/* look for any wire		 */
	if (pcb[*y + sp[i][1]][*x + sp[i][0]] & vec) {
		*x += sp[i][0];
		*y += sp[i][1];
		if (~pcb[*y][*x] & vec)
		    return get_net (*x, *y, vec);
		else
		    return get_net (*x, *y, top_side);
	    }

    return (nil);		/* not close to wire		 */
}

struct nlhd *loc(x, y)		/* locate net for given pin	 */
    int *x, *y;
{
    int i, j;

    for (i = *x - 1; i < *x + 1; ++i)/* look for holes 		 */
	for (j = *y - 1; j < *y + 1; ++j)
	    if (center (&i, &j)) { /* center coordinate		 */
		*x = i;
		*y = j;
		return get_net(i, j, vec);
	    }
    return (nil);		/* no hole found		 */
}

/*ARGSUSED*/
cont_s1 (x, y, ctx)		/* continue a wire trace	 */
    int x, y, ctx;
{
    return (cont_nt (x, y, s1b));
}

/*ARGSUSED*/
cont_s2 (x, y, ctx)		/* continue a wire trace	 */
    int x, y, ctx;
{
    return (cont_nt (x, y, s2b));
}

cont_nt (x, y, sd)		/* continue a wire trace	 */
    int x, y, sd;
{   int     i;

    if ((trc_buf[trc_cnt - 1].x == x) && (trc_buf[trc_cnt - 1].y == y)) {
	if (trc_cnt == 1 && !(pcb[y][x] & ahb)) {
	    trc_cnt = 0;
	    plow_ini (PLOW);
	    return plow_src (x, y, PLOW);
	}
	return (ROUTE);		/* ignore 0-length wires	 */
    }

    if (trc_cnt == 2)
	sof_seg = trc_buf[1].ty;

    if (trc_cnt > maxtrc - 3) {	/* sufficient space in buffer	 */
	for (i = sactrc; i < trc_cnt; i++) {
	    trc_buf[i - sactrc].x = trc_buf[i].x;
	    trc_buf[i - sactrc].y = trc_buf[i].y;
	    trc_buf[i - sactrc].ty = trc_buf[i].ty;
	}
	trc_cnt -= sactrc;
    }

    if (home (x, y, trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y,
	      trc_buf[0].x, trc_buf[0].y, sd, trc_buf[trc_cnt - 1].ty))
	return (START);		/* successfull home run		 */

    if (sd & ~trc_buf[trc_cnt - 1].ty) {/* need to change sides	 */
	if (viaalg (sd)) {
	    err_msg ("No space for a hole");
	    beep ();
	    return (ROUTE);
	}
	trc_buf[trc_cnt].ty = sd | ishb;}
    else
	trc_buf[trc_cnt].ty = sd;

    if (pnt (x, y, sd))		/* add a vector to wire		 */
	return (START);

    return (ROUTE);
}

pnt (x_cur, y_cur, si)		/* paint a wire segment		 */
    int    x_cur, y_cur, si;
{
    int     i,
            j,
            k,
            l,			/* random scratch		 */
            x,
            y,
            x1,
            y1,			/* used for test scan		 */
	    xo, yo,
            dx,
            dy,
            dxd,
            dyd;		/* direction vectors		 */

    xo = trc_buf[trc_cnt - 1].x;/* get origin			 */
    yo = trc_buf[trc_cnt - 1].y;
    i = (atan2 ((y_cur - yo) * 0.5, (x_cur - xo) * 0.5) + 6.676885) * 1.27324;
    i = i % 8;			/* chose a direction		 */

    dx = dr[i][0];
    dy = dr[i][1];
    color (si | selb, si | selb);

    j = abs (y_cur - yo);
    k = abs (x_cur - xo);
    if (i & 1)
	j = (j < k) ? j : k;	/* run min. length (diag.)	 */
    else
	j = dx ? k : j;		/* run axis projection		 */

    x = xo + dx;
    y = yo + dy;
    if (i & 1) {		/* diagonal check		 */
	dxd = (dx - dy) / 2;
	dyd = (dx + dy) / 2;
	for (k = 0; k < j; ++k) {/* check for illegal areas	 */
	    l = pchk1 (x - dy, y + dx, si, &x1, &y1);
	    if (l & 1)
		break;
	    l |= pchk1 (x + dy, y - dx, si, &x1, &y1);
	    if (l & 1)
		break;
	    if (!k && l && !(pcb[y1][x1] & ahb))
				/* something special		 */
		l = 0;
	    l |= pchk1 (x + dx, y + dy, si, &x1, &y1);
	    if (l & 1)
		break;
	    l |= pchk1 (x + dxd, y + dyd, si, &x1, &y1);
	    if (l & 1)
		break;
	    l |= pchk1 (x + dyd, y - dxd, si, &x1, &y1);
	    if (l & 1)
		break;
	    if (l & 2) {
		l = pchk2 (x, y, x1, y1, i, si);
		if (l)
		    break;
	    }
	    x += dx;
	    y += dy;
	}
    }
    else {			/* horizontal / vertical check	 */
	for (k = 0; k < j; ++k) {/* check for illegal areas	 */
	    l = pchk1 (x + dx - dy, y + dy + dx, si, &x1, &y1);
	    if (l & 1)
		break;
	    l |= pchk1 (x + dx + dy, y + dy - dx, si, &x1, &y1);
	    if (l & 1)
		break;
	    if (!k && l && !(pcb[y1][x1] & ahb))
				/* something special		 */
		l = 0;
	    l |= pchk1 (x + dx, y + dy, si, &x1, &y1);
	    if (l & 1)
		break;
	    if (l & 2) {
		l = pchk2 (x, y, x1, y1, i, si);
		if (l)
		    break;
	    }
	    x += dx;
	    y += dy;
	}
    }

    if (l == 2)			/* an end connection was made	 */
	return (1);		/* signal success		 */
    else {
	x -= dx;		/* correct length		 */
	y -= dy;
	plt (xo, yo, x, y);	/* plot new segment		 */
	trc_buf[trc_cnt].x = x;
	trc_buf[trc_cnt++].y = y;
	return (0);
    }
}

pchk1 (x, y, b, xs, ys)		/* paint check			 */
    int     x, y, b, *xs, *ys;
{
    int     i;
/* printf("pchk1: x=%d y=%d b=%2x xs=%d ys=%d\n",x,y,b,*xs,*ys); */
    i = pcb[y][x];
    if (!(i & selb))		/* not selected: peace of cake	 */
	return (0 != (i & (fb | b | ahb)));/* touches other net?	 */
    *xs = x;
    *ys = y;
    return (2);			/* needs more careful examination */
}

pchk2 (xe, ye, xt, yt, a, b)/* paint check (the same net)	 */
    int    xe, ye, xt, yt, a, b;
{
    register int i;

    i = pcb[yt][xt];

    if (i & b) {		/* touches net on same side: ok  */
	if (!(pcb[ye][xe] & b) &&
	    !(~pcb[ye + dr[a][1]][xe + dr[a][0]] & (selb | b))) {
	    xe += dr[a][0];	/* fill diagonal approach gap	 */
	    ye += dr[a][1];
	}
	plt (trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y, xe, ye);
	return (2);
    }

    if (i & ahb)		/* hole approach		 */
	return (pnt_ha (xe, ye, xt, yt, a, b));
    else	/* remaining case: touches same net on other side*/
	return (pnt_eha (xe, ye, xt, yt, a, b));
}

pnt_ha (xe, ye, xt, yt, a, b)
    int xe, ye, xt, yt, a, b;
/********************************************************************\
* 								     *
*  paint wire: hole approach - a via hole for the same net is found  *
*  in the paint - scan. A connection between the new wire and	     *
*  the hole need to be made (this is allways possible). <xe,ye> is   *
*  the end point of the new wire on side <b> with direction <a>.     *
*  <xt,yt> is part of the hole (not necessarily the center).	     *
*  <e> and <t> are one unit apart.				     *
* 								     *
*  a 2 is returned if a connection is made. 0 is returned if the     *
*  hole turns out to be a piece of guard area (pseudo hole).	     *
* 								     *
\********************************************************************/
{
    register int i;

    if (!center (&xt, &yt))	/* center on hole		 */
	return 0;

    if ((xt == xe + 2 * dr[a][0]) && (yt == ye + 2 * dr[a][1])) {
	plt (trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y,
		xe + 2 * dr[a][0], ye + 2 * dr[a][1]);
	return (2);		/* hole was staight ahead	 */
    }

    if (a & 1) {		/* diagonal approach		 */
	if ((abs (xe - xt) >= 2) && (abs (ye - yt)) &&
		(pcb[ye - yt][xe - dr[a][0]] & b)) {
	/* this is a wired case		 */
	    if (abs (yt - (ye - dr[a][1])) > 2)
		plt (xt, yt, xt, yt - 2 * dr[a][1]);
	    else
		plt (xt, yt, xt - 2 * dr[a][0], yt);
	    plt (trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y,
		    xe -= dr[a][0], ye -= dr[a][1]);
	    ckgp (xe, ye, b);
	    return (2);
	}
	xe += dr[a][0];
	ye += dr[a][1];
	for (i = 0; i < 3; ++i) {
	    if ((yt == ye) || (xt == xe)) {
		plt (xt, yt, xe, ye);
		plt (trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y, xe, ye);
		ckgp (xe, ye, b);
		return (2);
	    }
	    xe -= dr[a][0];
	    ye -= dr[a][1];
	}
	err ("pchk2: funny error - please save the details", xe, ye, xt, yt);}
    else {
	if ((abs (xt - xe) > 1) && (abs (yt - ye) > 1))
	    plt (xt, yt, xe, ye);
	else {
	    pxl (xt, yt);
	    xe += dr[a][0];
	    ye += dr[a][1];
	}
	plt (trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y, xe, ye);
	ckgp (xe, ye, b);
	return (2);
    }

    return 0;			/* to keep lint happy		 */
}

pnt_eha (xe, ye, xt, yt, de, b)
    int xe, ye, xt, yt, de, b;
/**********************************************************************\
* 								       *
*  paint: end hole allocator - tries to allocate an via-hole.	       *
*  During the paint-wire scan, the new wire finds itself	       *
*  approaching the same net on a the other pcb-side. A via-hole	       *
*  need to be allocated to connect these wires.			       *
*  <xe,ye> is the end point of the new wire, drawn in direction <de>.  *
*  <xt,yt> is the offending point of the same (selected) net on	       *
*  the other side. <e> and <t> are exactly one unit apart. The	       *
*  new wire is drawn on side <b>.				       *
* 								       *
*  On success, a 2 is returned. Unsuccesfull attemps return a 1.       *
* 								       *
\**********************************************************************/
{
    int i, j, x, y, dt;
    static struct hsp {		/* hole search pattern		 */
	int r[7];		/* relative test point position
				   -1: no displacement = xe,ye	 */
	int h[7];		/* help-point: 0=none  -1= xe,ye */
    } hasp[9] = {
	{			/* diag: xt,yt on  +2		 */
	    {  2, -1,  1,  3,  0,  4,  5},
	    {  0,  0,  0,  0,  1,  3, -1}  },
	{			/* diag: xt,yt on  +1		 */
	    {  1, -1,  0,  7,  4,  3,  5},
	    {  0,  0,  0,  0, -1,  0, -1}  },
	{			/* diag: xt,yt on   0		 */
	    {  0, -1,  1,  7,  4,  3,  7},
	    {  0,  0,  0,  0, -1, -1, -1}  },
	{			/* diag: xt,yt on  -1		 */
	    {  7, -1,  0,  1,  4,  5,  3},
	    {  0,  0,  0,  0, -1,  0, -1}  },
	{			/* diag: xt,yt on  -2		 */
	    {  6, -1,  7,  5,  0,  4,  3},
	    {  0,  0,  0,  0,  7,  5, -1}  },
	{			/* H/V:  xt,yt on  +1		 */
	    {  1, -1,  0,  2,  3,  7,  6},
	    {  0,  0,  0,  0,  2,  8,  8}  },
	{			/* H/V:  xt,yt on   0		 */
	    {  0, -1,  4,  1,  7,  2,  6},
	    {  0,  0, -1,  0,  0,  0,  0}  },
	{			/* H/V:  xt,yt on  -1		 */
	    {  7, -1,  0,  6,  5,  1,  2},
	    {  0,  0,  0,  0,  6,  8,  8}  }
    };

    for (dt = 0; dt < 8; dt++)	/* get direction of e->t	 */
	if (xt == xe + dr[dt][0] && yt == ye + dr[dt][1])
	    break;
    if (dt >= 8)
	err ("pnt_eha: <xe,ye> not adjacent to <xt,yt>", xe, ye, xt, yt);
    dt = (8 + dt - de) % 8;	/* need it relative		 */
    dt = (dt > 4) ? dt - 8 : dt;/* 0 ... 7  ->  -3 ... 4	 */

    if ((de & 1 && (dt > 2 || dt < -2)) ||
	    (!(de & 1) && (dt > 1 || dt < -1)))
	err ("pnt_eha: illegal case", xe, ye, xt, yt);

    i = (de & 1) ? 2 - dt : 6 - dt;/* select case (see table)	 */

    color (ishb | selb, ishb | selb);

    for (j = 0; j < 7; j++) {	/* scan through cases		 */
	if (hasp[i].r[j] < 0) {
	    x = xe;
	    y = ye;}
	else {
	    x = xe + dr[(hasp[i].r[j] + de) % 8][0];
	    y = ye + dr[(hasp[i].r[j] + de) % 8][1];
	}
	if ((x | y) & 1 || pin (x, y))
	    continue;		/* not aligned or no space	 */
	color (b | selb, b | selb);
	plt (xe, ye, trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y);
	color (vec, vec);
	pxl (x, y);		/* via hole is inserted		 */
	if (hasp[i].h[j]) {	/* need a intermediate point	 */
	    color (b ^ vec, b ^ vec);
	    pxl ((hasp[i].h[j] < 0) ? xe : xe + dr[(hasp[i].h[j] + de) % 8][0],
		 (hasp[i].h[j] < 0) ? ye : ye + dr[(hasp[i].h[j] + de) % 8][1]);
	}
	ckgp (x, y, b);
	return (2);
    }

    err_msg ("No space for a hole");
    beep ();
    color (b | selb, b | selb);
    return (1);
}

viaalg (b)			/* via hole alignment		 */
    int b;
/********************************************************************\
* 								     *
* tries to insert a via-hole at <trc_buf[trc_cnt-1].x/y> to allow    *
* continuation on side <b>. A intermediate vector might be added to  *
* the trace buffer. trc_buf is assumed to have enough space.	     *
* 								     *
* Returns a 0 if succesful, a 1 otherwise			     *
* 								     *
\********************************************************************/
{
    int     i, j, x, y, dx, dy;
    static int ct[9] = {5, 4, 3, 6, 8, 2, 7, 0, 1};

    x = trc_buf[trc_cnt - 1].x;
    y = trc_buf[trc_cnt - 1].y;

    color (ishb | selb, ishb | selb);
    if (!((x | y) & 1)) {	/* already aligned		 */
	if (pin (x, y))
	    return (1);		/* no space			 */
        else {
	    color (vec, vec);
	    pxl (x, y);
	    return (0); }	/* done !			 */
    }

    if (trc_cnt > 1) {		/* any prefered direction ?	 */
	dx = (trc_buf[trc_cnt - 2].x < x) - (trc_buf[trc_cnt - 2].x > x);
	dy = (trc_buf[trc_cnt - 2].y < y) - (trc_buf[trc_cnt - 2].y > y);

	if (!((x + dx) & 1) && !((y + dy) & 1) && !pin (x + dx, y + dy)) {
				/* extend previous vector	 */
	    x = trc_buf[trc_cnt - 1].x += dx;
	    y = trc_buf[trc_cnt - 1].y += dy;
	    color (vec, vec);
	    pxl (x, y);
	    return (0);
	}

	if (!((x - dx) & 1) && !((y - dy) & 1) && !pin (x - dx, y - dy)) {
				/* shorten previous vector	 */
	    trc_buf[trc_cnt - 1].x -= dx;
	    trc_buf[trc_cnt - 1].y -= dy;
	    color (vec, vec);
	    pxl (trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y);
	    color (0, vec ^ b);
	    pxl (x, y);
	    return (0);
	}
    }

    for (i = 0; i < 8; ++i)	/* brute force search		 */
	if (!((x + dr[i][0]) & 1) && !((y + dr[i][1]) & 1) &&
		!pin (x + dr[i][0], y + dr[i][1])) {
	    if (trc_cnt > 1) {
		j = ct[(dx + 1) * 3 + dy + 1];
		if (((i + 3) % 8 == j) || ((i + 4) % 8 == j) ||
		    ((i + 5) % 8 == j)) {	/* avoid dead maze	*/
		    color (0, vec ^ b);
		    pxl (trc_buf[trc_cnt - 1].x, trc_buf[trc_cnt - 1].y);
		    trc_buf[trc_cnt - 1].x -= dx;
		    trc_buf[trc_cnt - 1].y -= dy;
		}
	    }
	    color (vec, vec);
	    pxl (trc_buf[trc_cnt].x = dr[i][0] + x,
		 trc_buf[trc_cnt].y = dr[i][1] + y);
	    trc_buf[trc_cnt++].ty = vec ^ b;
	    return (0);
	}

    return (1);			/* there is just no space	 */
}

static int dex = 0;		/* delete exit feature		 */

m_delete (x, y, ctx)		/* manual delete		 */
    int x, y, ctx;
{
    plow_f = 0;

    if (del (x, y, vec)) {
	if (dex) {		/* switch to an other net	 */
	    dex = 0;
	    if (V.cnet)
	        deseln (V.cnet);
	    return (net_sel (ctx));}
	else
	    dex = 1;
    }
    return (START);
}

mr_delete (x, y, ctx)		/* manual delete in r-context	 */
    int x, y, ctx;
{
    int     dx, dy;

    plow_f = 0;
    dex = 0;
    if (del (x, y, vec)) {	/* del failed: backtrack	 */
	if (trc_cnt > 1) {
	    x = trc_buf[trc_cnt - 1].x;
	    y = trc_buf[trc_cnt - 1].y;
	    dx = (trc_buf[trc_cnt - 2].x < x) - (trc_buf[trc_cnt - 2].x > x);
	    dy = (trc_buf[trc_cnt - 2].y < y) - (trc_buf[trc_cnt - 2].y > y);
	    color (0, selb | (trc_buf[trc_cnt - 1].ty & vec));
	    plt (x, y, trc_buf[trc_cnt - 2].x + dx, trc_buf[trc_cnt - 2].y + dy);
	    if (trc_buf[trc_cnt - 1].ty & ishb) {
		color (0, selb | vec & trc_buf[trc_cnt - 1].ty);
		ck_rdnb (x = trc_buf[trc_cnt - 2].x,
		   y = trc_buf[trc_cnt - 2].y, vec & trc_buf[trc_cnt - 1].ty);
if(pcb[y][x] & ishb) {
		color (0, ishb | selb);
		dpin (x, y);
		color (selb, selb);
/* guard zones may stay unselected: cosmetic defect */
		for (dy = -1; dy <= 1; dy++)/* un-unselect	 */
		    for (dx = -1; dx <= 1; dx++)
			if (pcb[y + dy][x + dx] & vec)
			    pxl (x + dx, y + dy);
}else{
 printf("This was the bug - please report\n");
 beep();
}
		if (trc_cnt > 2 &&
		    abs(trc_buf[trc_cnt - 3].x - trc_buf[trc_cnt - 2].x) <2 &&
		    abs(trc_buf[trc_cnt - 3].y - trc_buf[trc_cnt - 2].y) <2) {
				/* remove alignment stub	 */
		    color (0, selb | trc_buf[trc_cnt - 2].ty);
		    ck_rdnb (trc_buf[trc_cnt - 3].x, trc_buf[trc_cnt - 3].y,
			trc_buf[trc_cnt - 2].ty);
		    trc_cnt--;
		}
	    }
	    trc_cnt--;
	}
	if (trc_cnt <= 1) {	/* unselect start		 */
	    ckgp (trc_buf[0].x, trc_buf[0].y, s1b);
	    ckgp (x = trc_buf[0].x, y = trc_buf[0].y, s2b);
	    color (selb, selb);
	    for (dy = -1; dy <= 1; dy++)/* un-unselect		 */
		for (dx = -1; dx <= 1; dx++)
		    if (pcb[y + dy][x + dx] & (sof_seg | ahb))
			pxl (x + dx, y + dy);
	    trc_cnt = 0;
	    return (START);
	}
    }
    return (ctx);
}

static int wxl, wyl, wxh, wyh;	/* window extensions		 */

/*ARGSUSED*/
cr_window (ctx)			/* create a window		 */
    int ctx;
{
    wxl = wx + 192 / cz;	/* default size			 */
    wyl = wy + 177 / cz;
    wxh = wx + 320 / cz;
    wyh = wy + 305 / cz;
    di_window ();
}

di_window ()			/* display window		 */
{
    color (resb, resb);
    plts (wxl, wyl, wxh, wyl);
    plts (wxh, wyl, wxh, wyh);
    plts (wxh, wyh, wxl, wyh);
    plts (wxl, wyh, wxl, wyl);
}

rm_window ()			/* remove window		 */
{
    color (0, resb);
    plts (wxl, wyl, wxh, wyl);
    plts (wxh, wyl, wxh, wyh);
    plts (wxh, wyh, wxl, wyh);
    plts (wxl, wyh, wxl, wyl);
}

/*ARGSUSED*/
w_zoomi (x, y, ctx)		/* zoom in (and maintain window) */
    int x, y, ctx;
{
    if (cz < 16) {
	rm_window ();
	zoom (cz + 1);
	di_window ();
    }
    return (ctx);		/* don't change context		 */
}

/*ARGSUSED*/
w_zoomo (x, y, ctx)		/* zoom out (and maintain window) */
    int x, y, ctx;
{
    if (cz > 1) {
	rm_window ();
	zoom (cz - 1);
	di_window ();
    }
    return (ctx);		/* don't change context		 */
}

w_wmv (x, y, ctx)		/* move window			 */
    int x, y, ctx;
{
    rm_window ();
    window (x - 256 / cz, y - 256 / cz);
    di_window ();
    return (ctx);		/* don't change context		 */
}

wnd_urc (x, y, ctx)		/* window: upper right corner	 */
    int x, y, ctx;
{
    rm_window ();

    if (x >= wxl)
	wxh = x;
    else {
	wxh = wxl;
	wxl = x;
    }

    if (y >= wyl)
	wyh = y;
    else {
	wyh = wyl;
	wyl = y;
    }

    di_window ();
    return (ctx);
}

wnd_llc (x, y, ctx)		/* window: lower left corner	*/
    int x, y, ctx;
{
    rm_window ();
    wxh = x + (wxh - wxl);
    wxl = x;
    wyh = y + (wyh - wyl);
    wyl = y;
    di_window ();
    return (ctx);
}

static int ad_ty;		/* area delete type		 */

ini_adel (ctx)			/* initialize area delete	 */
   int ctx;
{
    ad_ty = vec;
    cr_window (ctx);
    return (ctx);
}

/*ARGSUSED*/
sel_adel (x, y, ctx)		/* select area delete options	 */
    int x, y, ctx;
{
    static char *txt[] = {
	"Everything",
	"Side 1 only",
	"Side 2 only",
	"Nothing"
    };

    switch (menu (txt, 4)) {
	case 0: 
	    ad_ty = vec;
	    break;
	case 1: 
	    ad_ty = s1b;
	    break;
	case 2: 
	    ad_ty = s2b;
	    break;
	default: 
	    err_msg ("Area-delete aborted");
	    rm_window ();
	    return (START);
    }

    return (ctx);
}

/*ARGSUSED*/
exc_adel (x, y, ctx)		/* execute area delete		 */
    int x, y, ctx;
{
    int    ad_hck (), ad_vck ();
    struct nlhd *t, *get_net ();
    register int xx, yy;

    rm_window ();
    for (yy = wyl + 1; yy < wyh; yy++)/* delete loop		 */
	for (xx = wxl + 1; xx < wxh; xx++)
	    if (pcb[yy][xx] & ad_ty) {
		t = get_net (xx, yy, vec);

		if (t < &NH[0] || t > &NH[V.nnh])
		    continue;	/* skip power and ground nets	 */

		wthf = ad_hck;
		wtvf = ad_vck;
		wtrace (xx, yy, vec);
		if (t && t -> f && !cchk (t)) {
		    t -> f = 0;
		    nettgo++;
		}
	    }

    for (xx = wxl; xx <= wxh; xx++) {/* clean edges		 */
	if (pcb[wyl][xx] & (ahb | vec))
	    ad_gchk (xx, wyl);
	if (pcb[wyh][xx] & (ahb | vec))
	    ad_gchk (xx, wyh);
    }
    for (yy = wyl + 1; yy < wyh; yy++) {
	if (pcb[yy][wxl] & (ahb | vec))
	    ad_gchk (wxl, yy);
	if (pcb[yy][wxh] & (ahb | vec))
	    ad_gchk (wxh, yy);
    }

    nets_msg (nettgo);		/* update number of nets message */

    return (START);
}

ad_hck (x, y)			/* area-delete hole check	 */
    int x, y;
{
    if (pcb[y][x] & ishb && x > wxl && y > wyl && x < wxh && y < wyh) {
	color (0, ishb);
	dpin (x, y);
    }
    return (0);
}

ad_vck (x1, y1, x2, y2)		/* area-delete vector check	 */
    int x1, y1, x2, y2;
{
    int     i;

    if (y1 > y2) {		/* order y for y-band clipping	 */
	i = x1;
	x1 = x2;
	x2 = i;
	i = y1;
	y1 = y2;
	y2 = i;
    }
    i = ((x1 < x2) - (x2 < x1));

    if (y1 <= wyl) {
	if (y2 <= wyl)
	    return;
	x1 += (wyl + 1 - y1) * i;
	y1 = wyl + 1;
    };

    if (y2 >= wyh) {
	if (y1 >= wyh)
	    return;
	x2 -= (y2 - (wyh - 1)) * i;
	y2 = wyh - 1;
    }

    if (x1 > x2) {		/* order x for x-band clipping	 */
	i = x1;
	x1 = x2;
	x2 = i;
	i = y1;
	y1 = y2;
	y2 = i;
    }
    i = (y1 < y2) - (y2 < y1);

    if (x1 <= wxl) {
	if (x2 <= wxl)
	    return;
	y1 += (wxl + 1 - x1) * i;
	x1 = wxl + 1;
    }

    if (x2 >= wxh) {
	if (x1 >= wxh)
	    return;
	y2 -= (x2 - (wxh - 1)) * i;
	x2 = wxh - 1;
    }

    if (abs (x1 - x2) < 2 && abs (y1 - y2) < 2 &&
	pcb[x1][y1] & chb && pcb[x2][y2] & chb)
	return;			/* do not remove parts of a hole */

    color (0, wtsb);
    plt (x1, y1, x2, y2);
}

ad_gchk (x, y)			/* area delete: garbage check	 */
    int x, y;
{
    struct nlhd *get_net ();
    int delh (), delv ();

    if (!get_net (x, y, vec)) {
	wthf = delh;
	wtvf = delv;
	wtrace (x, y, vec);
    }
}

ini_art (ctx)			/* initialize area route	 */
    int ctx;
{
    cr_window (ctx);
    return (ctx);
}

/*ARGSUSED*/
sel_art (x, y, ctx)		/* select area route options	 */
    int x, y, ctx;
{
    err_msg ("Area-route has no options yet");
    return (ctx);
}

/*ARGSUSED*/
exc_art (x, y, ctx)		/* execute area route		 */
    int x, y, ctx;
{
    register int xx, yy, max, cnt, i;
    struct nlhd **nl, *t, *get_net ();
    char buf[40];

    max = 200;			/* assume 200 for a start	 */
    cnt = 0;
    nl = (struct nlhd **) malloc (sizeof (struct nlhd *) * max);

    rm_window ();
    Ferr_msg ("Busy: routing");
    for (yy = wyl + 1; yy < wyh; yy++)
	for (xx = wxl + 1; xx < wxh; xx++)
	    if ((pcb[yy][xx] & chb) && (t = get_net (xx, yy, vec)) && !(t -> f)) {
		for (i = 0; i < cnt; i++)
		    if (nl[i] == t)
			break;	/* got this net already		 */
		if (i >= cnt) {
		    if (cnt >= max) {
			max += max / 2;
			nl = (struct nlhd **) realloc
					(nl, sizeof (struct nlhd *) * max);
		    }
		    nl[cnt++] = t;
		}
	    }

    for (i = 0; i < cnt; i++) {
	sprintf (buf, "Routing net %d of %d", i, cnt);
	nets_msg (nettgo);
	Ferr_msg (buf);
	N_route (nl[i], 2, wxl, wyl, wxh, wyh);
	nettgo -= nl[i] -> f;
    }

    free (nl);			/* clean up			 */
    nets_msg (nettgo);
    err_msg ("Done");
    beep ();
    return (START);
}
