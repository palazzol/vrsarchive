/***************************************************************\
*								*
*	PCB program						*
*								*
*	Work routine section					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include "pparm.h"
#include "pcdst.h"

extern int plt(), dot();
extern struct hole *fndh();

int sp[9][2] =	{ { 0,  0},	/* search pattern		*/
		  { 1,  0},
		  { 0,  1},
		  {-1,  0},
		  { 0, -1},
		  { 1,  1},
		  {-1,  1},
		  { 1, -1},
	 	  {-1, -1}  };

int wtph;			/* wire trace phase		*/
int snct;			/* sub net counter		*/


selnet (n)			/* select a net to work on	*/
    struct nlhd *n;
/************************************************************\
* 							     *
*   note: it is assumed that that nothing else is selected!  *
* 							     *
\************************************************************/
{
    register int i, x, y;
    int schk (), sel_plt ();
    register struct nlst *p;

/* printf("*t:selnet nodes=%d\n",n->l); */

    color (selb, selb);		/* prepare operation		 */
    V.cnet = n;			/* save current net		 */
    snct = 0;
    wtvf = sel_plt;
    wthf = schk;

    i = 0;			/* remove old marks		 */
    p = n -> lp;
    while (p) {
	p -> mk = (-1);
	++i;
	p = p -> n;
    }
    if (i != n -> l)
	err ("inconsistent net length", i, n -> l, 0, 0);

    for (p = n -> lp; p; p = p -> n) {	/* select the net	 */
	x = p -> c -> x;
	y = p -> c -> y;
	if (p -> mk == -1) {
	    wtrace (x, y, vec);
	    snct++;
	}
    }

    if ((!n -> f) && (snct <= 1)) {
	printf("sel: f inconcistency -- recovered\n");
	n -> f = 1;
    }

    return (snct <= 1);		/* snct = number of sub-nets	 */
}

sel_plt (x1, y1, x2, y2)	/* select plot (insert resb)	 */
    int x1, y1, x2, y2;
{
    if (wtsb == s2b) {		/* add resb for s2b		 */
	ccc = selb | resb;
	ccm = ~(selb | resb) & 255;
    }

    plt (x1, y1, x2, y2);

    ccc = selb;
    ccm = ~selb & 255;
}

schk (x, y)			/* select check			 */
    int x, y;
{
    struct hole *hp;

/* printf("schk: x=%d y=%d\n",x,y); */
/*
 * Note: The consistency checks may be removed (to save time)
 *       if the program is more debugged.
 */
    if (pcb[y][x] & selb)
/*	err ("schk: already selected", x, y, 0, 0);*/
	err_msg("select problem: please clean");

    if (pcb[y][x] & chb) {	/* compnent hole		 */
	hp = fndh (x, y);
	if (!hp)
	    err ("schk: couldn't find hole", x, y, 0, 0);
	if (hp -> n -> mk >= 0)
	    err ("schk: already marked", x, y, 0, 0);
	hp -> n -> mk = snct;
    }

    dpin (x, y);		/* select hole			 */

    return (0);
}

center (xh, yh)			/* center on hole		 */
    int *xh, *yh;
/******************************************************************\
* 								   *
*  Returns '1' if <x,y> is part of a hole. <x,y> will be adjusted  *
*  to the center of the hole. Returns '0' with unchanged <x,y>	   *
*  otherwise. This routine is not fooled by guard bits.		   *
* 								   *
\******************************************************************/
{
    int x, y;

    x = *xh;
    y = *yh;

    if (!(pcb[y][x] & ahb))	/* no hole at all		 */
	return (0);

    if (pcb[y][x - 1] & ahb) {
	if (!(pcb[y][x + 1] & ahb)) {
	    x--;
	    if (!(pcb[y][x - 1] & ahb))
		return (0);
	}}
    else {
	x++;
	if (!(pcb[y][x] & pcb[y][x + 1] & ahb))
	    return (0);
    }

    if (pcb[y - 1][x] & ahb) {
	if (!(pcb[y + 1][x] & ahb)) {
	    y--;
	    if (!(pcb[y - 1][x] & ahb))
		return (0);
	}}
    else {
	y++;
	if (!(pcb[y][x] & pcb[y + 1][x] & ahb))
	    return (0);
    }

    *xh = x;			/* center coordinates		 */
    *yh = y;

    return (1); 		/* this is a hole!		 */
}

wtrace(x, y, b)			/* wire trace			 */
/****************************************************************\
* 								 *
*   note: don't cross wires of the same net without connection.  *
* 								 *
\****************************************************************/
    int x, y, b;
{
    register int i, j, k;
/* printf("wtrace: x=%d y=%d\n",x,y); */

    b &= pcb[y][x];
    if (center (&x, &y) && (b = vec, wthf) && (*wthf) (x, y))
	return;			/* aborted on first hole	 */

    if (!(pcb[y][x] & b))	/* no wire			 */
	return;

    pcb[y][x] |= mrkb;		/* mark start			 */
    wtph = 0;			/* phase 0			 */

    for (i = 0; i < 8; ++i) {	/* mark wires			 */
	j = pcb[y + dr[i][1]][x + dr[i][0]];
	if (!(j & mrkb) && (j &= b)) {
	    if ((j & s1b) && vtrace (x, y, i, s1b))
		break;		/* abort exit			 */
	    if (j == s2b && vtrace (x, y, i, s2b))
		break;		/* abort exit			 */
	    if (j == vec) {
		wtsb = s2b;
		if (wtvf)
		    wtvf (x, y, x + dr[i][0], y + dr[i][1]);
		j = pcb[y + 2 * dr[i][1]][x + 2 * dr[i][0]];
		if (!(j & mrkb) && (j & s2b) &&
		    vtrace (x + dr[i][0], y + dr[i][1], i, s2b))
		    break;
		k = (i + 1) & 7;
		j = pcb[y + dr[i][1] + dr[k][1]][x + dr[i][0] + dr[k][0]];
		if (!(j & mrkb) && (j & s2b) &&
		    vtrace (x + dr[i][0], y + dr[i][1], k, s2b))
		    break;
		k = (i + 7) & 7;
		j = pcb[y + dr[i][1] + dr[k][1]][x + dr[i][0] + dr[k][0]];
		if (!(j & mrkb) && (j & s2b) &&
		    vtrace (x + dr[i][0], y + dr[i][1], k, s2b))
		    break;
	    }
	}
    }

    pcb[y][x] &= ~mrkb;		/* unmark start			 */
    wtph = 1;			/* phase 1			 */

    for (i = 0; i < 8; ++i) 	/* remove marks			 */
	if (pcb[y + dr[i][1]][x + dr[i][0]] & mrkb)
		vtrace (x, y, i, mrkb);

}

vtrace (x, y, r, b)		/* vector trace		*/
    int x, y, r, b;
{
    register int i, j, x1, y1,
		 dx, dy;	/* direction vector		 */
    int     k,
	    in1, in2,		/* inhit diag. branches		 */
            x2, y2;

    static int  sm[8][4]	/* search mask			 */
			    = {	{0x0e, 0x0c, 0x06, 0x02},
				{0xe0, 0x60, 0xc0, 0x80},
				{0x38, 0x30, 0x18, 0x08},
				{0x83, 0x81, 0x03, 0x02},
				{0xe0, 0xc0, 0x60, 0x20},
				{0x0e, 0x06, 0x0c, 0x08},
				{0x83, 0x03, 0x81, 0x80},
				{0x38, 0x18, 0x30, 0x20}   };

    static int	smd[8][3]	/* search mask (diagonals)	 */
			    = { {0x1c, 0x04, 0x08},
				{0xc1, 0x01, 0x80},
				{0x70, 0x10, 0x20},
				{0x07, 0x04, 0x02},
				{0xc1, 0x40, 0x80},
				{0x1c, 0x10, 0x08},
				{0x07, 0x01, 0x02},
				{0x70, 0x40, 0x20}  };

/* printf("vtrace: x=%d y=%d r=%d b=%2x\n",x,y,r,b); */

    dx = dr[r][0];		/* save array look up's		 */
    dy = dr[r][1];

    x1 = x + dx;		/* scan vector			 */
    y1 = y + dy;
    i = 1;			/* reset length counter		 */

    if (wtph) {			/* phase 1			 */

	if (!(pcb[y1][x1] & mrkb))
	    return (0);		/* already done			 */

	do {
	    pcb[y1][x1] &= ~mrkb;
				/* remove marks			 */
	    x1 += dx;
	    y1 += dy;
	    ++i;
	} while (pcb[y1][x1] & mrkb); }

    else {			/* phase 0			 */

	if (pcb[y1][x1] & mrkb)
	    return (0);		/* already done			 */

	do {
	    j = pcb[y1][x1] |= mrkb;/* insert marks		 */

	    if ((j & ahb) && (pcb[y1 + 1][x1 + 1] & ahb) &&
		    (pcb[y1 - 1][x1 - 1] & ahb) && vhole (x1, y1, b))
		return (1);	/* trace abort			 */

	    x1 += dx;
	    y1 += dy;
	    ++i;
	    j = pcb[y1][x1];
	} while ((j & b) && !(j & mrkb));

	wtsb = b;		/* interface to trace routines	 */
	if (wtvf)		/* execute v-fuction (if any) 	 */
	    (*wtvf) (x, y, x1 - dx, y1 - dy);
    }

    if (r & 1) {		/* diagonal scan (tricky)	 */
	r--;			/* lets start at 0		 */
	i--;
	x2 = x + (dx - dy) / 2;
	x1 = x2 + dx;
	y2 = y + (dx + dy) / 2;
	y1 = y2 + dy;
	for (j = 0; j < i; j++) {
	    if (pcb[y1][x1] & b) {
		k = smd[r][0];
		if (j == i - 1)
		    k &= smd[r][1];
		if (ckv (x1, y1, k, b))
		    return (1);
	    }
	    x1 += dx;
	    y1 += dy;
	}
	x1 = x + dx - dy;
	y1 = y + dy + dx;
	in2 = pcb[y2][x2];
	for (j = 0; j < i; j++) {
	    x2 += dx;
	    y2 += dy;
	    in1 = in2;
	    in2 = pcb[y2][x2];
	    if ((pcb[y1][x1] & b & ~(in1 | in2)) &&
		    ckv (x1, y1, smd[r][2], b))
		return (1);
	    x1 += dx;
	    y1 += dy;
	}

	r++;
	x2 = x + (dx + dy) / 2;
	x1 = x2 + dx;
	y2 = y + (dy - dx) / 2;
	y1 = y2 + dy;
	for (j = 0; j < i; j++) {
	    if (pcb[y1][x1] & b) {
		k = smd[r][0];
		if (j == i - 1)
		    k &= smd[r][1];
		if (ckv (x1, y1, k, b))
		    return (1);
	    }
	    x1 += dx;
	    y1 += dy;
	}
	x1 = x + dx + dy;
	y1 = y + dy - dx;
	in2 = pcb[y2][x2];
	for (j = 0; j < i; j++) {
	    x2 += dx;
	    y2 += dy;
	    in1 = in2;
	    in2 = pcb[y2][x2];
	    if ((pcb[y1][x1] & b & ~(in1 | in2)) &&
		    ckv (x1, y1, smd[r][2], b))
		return (1);
	    x1 += dx;
	    y1 += dy;
	} }

    else {
	x1 = x + dx - dy;
	y1 = y + dy + dx;
	for (j = 0; j < i; ++j) {
	/* look for other wires		 */
	    if (pcb[y1][x1] & b) {
		k = sm[r][0];
		if (!j)
		    k &= sm[r][1];
		if (j == i - 2)
		    k &= sm[r][2];
		if (j == i - 1)
		    k &= sm[r][3];
		if (ckv (x1, y1, k, b))
		    return (1);
	    }
	    x1 += dx;
	    y1 += dy;
	}

	x1 = x + dx + dy;
	y1 = y + dy - dx;
	r++;
	for (j = 0; j < i; ++j) {/* just an other look		 */
	    if (pcb[y1][x1] & b) {
		k = sm[r][0];
		if (!j)
		    k &= sm[r][1];
		if (j == i - 2)
		    k &= sm[r][2];
		if (j == i - 1)
		    k &= sm[r][3];
		if (ckv (x1, y1, k, b))
		    return (1);
	    }
	    x1 += dx;
	    y1 += dy;
	}
    }
    return (0);
}

vhole (x, y, b)			/* some kind of interconnect hole*/
    int x, y, b;
{
    register int i, j, k;
/* printf("vhole: x=%d y=%d b=%2x\n",x,y,b); */
    if (wthf && wthf (x, y))	/* abort requested ?		 */
	return (1);

    b = vec ^ b;		/* change side			 */

    if (!(pcb[y][x] & b))	/* wire present ?		 */
	return (0);

    for (i = 0; i < 8; ++i) {
	j = pcb[y + dr[i][1]][x + dr[i][0]];
	if (j & b) {
	    if (j & (b ^ vec)) {/* this hack caused by single marker	 */
		wtsb = b;
		if (wtvf)
		    wtvf (x, y, x + dr[i][0], y + dr[i][1]);
                for (j = 0; j < 8; ++j) {
		    k = pcb[y + dr[i][1] + dr[j][1]][x + dr[i][0] + dr[j][0]];
		    if (!(k & (ahb | mrkb)) && (k & vec)) {
			if ((k & b) &&
			    vtrace(x + dr[i][0], y + dr[i][1], j, b)) {
			    pcb[y + dr[i][1]][x + dr[i][0]] |= mrkb;
			    return 1;
			}
			if ((k & (b ^ vec)) &&
			    vtrace(x + dr[i][0], y + dr[i][1], j, b ^ vec)) {
			    pcb[y + dr[i][1]][x + dr[i][0]] |= mrkb;
			    return 1;
			}
		    }
		}}
	    else if (vtrace (x, y, i, b))
		return 1;
	}
    }

    return (0);
}

ckv (x, y, s, b)		/* decide on a new direction	 */
    int  x, y, s, b;
{
    register int i, j, k;

/* printf("ckv: x=%d y=%d s=%2x b=%2x\n",x,y,s,b); */

    switch (s) {
	case 0: 
	    err ("ckv: internal program error", x, y, s, b);
	case 0x01: 
	    return (vtrace (x - dr[0][0], y - dr[0][1], 0, b));
	case 0x02: 
	    return (vtrace (x - dr[1][0], y - dr[1][1], 1, b));
	case 0x04: 
	    return (vtrace (x - dr[2][0], y - dr[2][1], 2, b));
	case 0x08: 
	    return (vtrace (x - dr[3][0], y - dr[3][1], 3, b));
	case 0x10: 
	    return (vtrace (x - dr[4][0], y - dr[4][1], 4, b));
	case 0x20: 
	    return (vtrace (x - dr[5][0], y - dr[5][1], 5, b));
	case 0x40: 
	    return (vtrace (x - dr[6][0], y - dr[6][1], 6, b));
	case 0x80: 
	    return (vtrace (x - dr[7][0], y - dr[7][1], 7, b));
	default: 		/* decide on most likely candidate	 */
	    j = 1;
	    if (wtph) {		/* phase 1				 */
		for (i = 0; i < 8; i++) {
				/* scan options 			 */
		    k = pcb[y + dr[i][1]][x + dr[i][0]];
		    if ((s & j) && (k & mrkb)) {
			return (vtrace (x - dr[i][0], y - dr[i][1], i, b));
		    }
		    j += j;
		} }
	    else {		/* phase 0				 */
		for (i = 0; i < 8; i++) {
				/* scan options 			 */
		    k = pcb[y + dr[i][1]][x + dr[i][0]];
		    if ((s & j) && (k & b) && !(k & mrkb)) {
			return (vtrace (x - dr[i][0], y - dr[i][1], i, b));
		    }
		    j += j;
		}
	    }
	    j = 1;
	    for (i = 0; i < 8; i++) {
				/* single point - take first 		 */
		if (s & j)
		    return (vtrace (x - dr[i][0], y - dr[i][1], i, b));
		j += j;
	    }
    }

    return 0;			/* to keep lint happy		 */
}

static struct nlhd *net_ptr;	/* net-ptr for fndnet - calls	 */

struct nlhd *get_net (x, y, s)	/* get net-pointer 		 */
    int x, y, s;
{
    int fndnet();

    net_ptr = 0;
    wtvf = nil;			/* identify net			 */
    wthf = fndnet;
    wtrace (x, y, s);
    return (net_ptr);		/* success			 */
}

fndnet (x, y)			/* find net pointer		 */
    int x, y;
{
    struct hole *hp, *fndh();

    if (pcb[y][x] & ishb)
	return (0);		/* don't bother with ish's	 */

    hp = fndh (x, y);
    if (!hp)
	err ("fndnet: missing hole", x, y, 0, 0);
    if (!hp -> n) {		/* unconnected pin		 */
	if (pcb[y][x] & vec)
	    err ("fndnet: net connected to invalid pin", x, y, 0, 0);
	else 
	    return (1);		/* just a unused pin		 */
    }
    net_ptr = hp -> n -> nlp;	/* found it !			 */

    return (1);			/* abort search			 */
}

deseln (n)		/* de select a net		 */
    struct nlhd *n;
{
    register int  x, y;
    int des_plt (), dpin();
    register struct nlst *p;

    color (0, selb);
    wtvf = des_plt;
    wthf = dpin;
    for (p = n -> lp; p; p = p -> n) {
	x = p -> c -> x;
	y = p -> c -> y;
	if (pcb[y][x] & selb)
	    wtrace (x, y, vec);
    }
    V.cnet = nil;
}

des_plt (x1, y1, x2, y2)	/* de - select plot (rm resb)	 */
    int x1, y1, x2, y2;
{
    if (wtsb == s2b)		/* remove resb for s2b		 */
	ccm = ~(selb | resb) & 255;

    plt (x1, y1, x2, y2);

    ccm = ~selb & 255;
}


cchk (net)		/* check connectivity on a selected net	 */
/***********************************************************************\
* 								        *
* Note:	this routine could run faster if it uses context information    *
* 	of 'wire', however that would increase side effects.	        *
* 								        *
* 	snct becomes invalid (to save time, the count is stopped if     *
* 	there are more than 1 sub nets).			        *
* 
* 	
* 								        *
\***********************************************************************/
    struct nlhd *net;
{
    struct nlst *p;
    int     cchku ();
    register int x, y;

/* printf("cchk: called\n"); */

    for (p = net -> lp; p; p = p -> n)
        p -> mk = 1;		/* set all marks to 1		 */

    wtvf = nil;			/* reset all marks to 0		 */
    wthf = cchku;
    x = net -> lp -> c -> x;
    y = net -> lp -> c -> y;
    wtrace (x, y, vec);

    for (p = net -> lp; p; p = p -> n)
	if (p -> mk)
	    return (0);		/* stop at the first counter example */

    return (1);			/* net is connected		 */
}

cchku (x, y)			/* connectivity check		 */
    int x, y;
{
    struct hole *hp;
/* printf("cchku: x=%d y=%d\n",x,y); */
    if (pcb[y][x] & chb) {
	hp = fndh (x, y);
	if (hp)
	    hp -> n -> mk = 0;
	else
	    err ("cchku: invalid connection", x, y, V.cnet, 0);
    }

    return (0);
}

struct nlhd *choose()		/* choose a net for work	*/
{
    int     ii, i, j, k, c[6], l[6], wcx, wcy;
    struct nlhd *p, *ck_lrnb();
    static int  lst = 0;

/*******************************************************\
* 						        *
*  preferences:	0. check learn buffer		        *
* 	   	1. within current window	        *
* 		2. window sized within current frame    *
* 		3. within current frame		        *
* 		4. window sized			        *
* 		5. frame sized			        *
* 		6. any net			        *
* 		7. last net (if it is incomplete)       *
* 						        *
\*******************************************************/

    p = ck_lrnb ();		/* something lerned ?		 */
    if (p)
	return (p);

    for (i=0; i < 6; (c[i] = -1, l[i++] = xmax + ymax));

    wcx = wx + 256 / cz;
    wcy = wy + 241 / cz;

    for (ii = 0; ii < V.nnh; ii++) {
	i = (ii + lst) % V.nnh;	/* do a circular shift		 */

	if (!NH[i].f && (i != lst || V.nnh < 2)) {
	    k = abs (wcx - (NH[i].x1 + NH[i].x1) / 2) +
		abs (wcy - (NH[i].y1 + NH[i].y1) / 2);
	    if ((wx <= NH[i].x1) && (wx + 512 / cz > NH[i].x2) && 
                (wy <= NH[i].y1) && (wy + 482 / cz > NH[i].y2)) {
		c[0] = i;	/* within current window	 */
		break;
	    }
	    j = ((NH[i].x2 - NH[i].x1) < 512 / cz) &&
		((NH[i].y2 - NH[i].y1) < 482 / cz);
	    if ((fx <= NH[i].x1) && (fx + 512 > NH[i].x2) &&
		(fy <= NH[i].y1) && (fy + 512 > NH[i].y2)) {
		if (j) {
		    if (k < l[1]) {
			c[1] = i;	/* window sized in currend frame */
			l[1] = k;}}
		else {
		    if (k < l[2]) {
			l[2] = k;
			c[2] = i;}}}	/* within current frame		 */
	    else if (j) {
		if (k < l[3]) {
		     l[3] = k;
		     c[3] = i;}}	/* just window sized		 */
	    else if (((NH[i].x2 - NH[i].x1) < 512) &&
		     ((NH[i].y2 - NH[i].y1) < 512)) {
		if (k < l[4]) {
		    l[4] = k;
		    c[4] = i;}}	/* just frame sized		 */
	    else if (k < l[5]) {
		l[5] = k;
		c[5] = i;}		/* just an incomplete net	 */
	}
    }

    for (i = 0; i < 6; ++i)	/* select highest priority	 */
	if (c[i] >= 0) {
	    lst = c[i];
	    return (&NH[lst]);
	}

    if ((lst > 0) && !NH[lst].f)
	return (&NH[lst]);	/* resort to previous net	 */

    return (nil);		/* no net found			 */
}

static struct nlhd *net_t_del;	/* net to delete		 */

del (x, y, b)			/* delete wire segment		 */
    int x, y, b;
{
    register int  i, j, x1, y1, x2, y2;
    int     k, delv (), delh ();
    struct  nlhd *get_net();

/* printf("del: x=%d y=%d\n",x,y); */

    x1 = y1 = (-1);
    for (i = 0; i < 9; ++i)	/* find a wire		 */
	if ((pcb[y + sp[i][1]][x + sp[i][0]] & b) && 
	    !(pcb[y + sp[i][1]][x + sp[i][0]] & selb)) {
		x1 = x + sp[i][0];
		y1 = y + sp[i][1];
		break;
	    }

    if (x1 < 0)
	return (1);		/* nothing to delete	 */

    b = pcb[y1][x1] & b;
    if (b == vec)
	b = top_side;		/* prefere top side	 */

    net_t_del = get_net (x1, y1, b);/* identify the net	 */

    if (!net_t_del) {		/* no net ?!?!		 */
	printf ("Deleting unconnected object\n");
	wtvf = delv;
	wthf = delh;
	wtrace (x1, y1, b);
	ckgp (x1, y1, b);
	return (0);
    }

    for (i = 0; i < 8; i++)	/* find direction	 */
	if (pcb[y1 + dr[i][1]][x1 + dr[i][0]] & b)
	    break;
    i = i % 8;

    x2 = x1;
    y2 = y1;
    while ((pcb[y2 + dr[i][1]][x2 + dr[i][0]] & b) &&
	   !(pcb[y2+1][x2+1] & pcb[y2-1][x2-1] & ahb)) {
	x2 += dr[i][0];		/* find one end		 */
	y2 += dr[i][1];
    }

    while ((pcb[y1 - dr[i][1]][x1 - dr[i][0]] & b) &&
	   !(pcb[y1+1][x1+1] & pcb[y1-1][x1-1] & ahb)) {
	x1 -= dr[i][0];		/* find other end	 */
	y1 -= dr[i][1];
    }

    k = abs (x1 - x2);
    k = k ? k : abs (y1 - y2);	/* get length		 */

    if (k > 1) {/* remove line */
	color (0, b);
	plt (x1 + dr[i][0], y1 + dr[i][1], x2 - dr[i][0], y2 - dr[i][1]);
    }

    j = cchk (net_t_del);	/* update conectivity		 */
    nettgo += j ^ net_t_del -> f;
    net_t_del -> f = j;
    nets_msg (nettgo);

    delc (x1, y1, b);		/* clean ends		 */
    delc (x2, y2, b);

    while (0 < --k) {		/* scan for branches	 */
	x1 += dr[i][0];
	y1 += dr[i][1];
	for (j = 0; j < 8; j++)
	     if (pcb[y1 + dr[j][1]][x1 + dr[j][0]] & b)
		delc (x1 + dr[j][0], y1 + dr[j][1], b);
    }

    return (0);
}

delv (x1, y1, x2, y2)	/* delete vector		 */
    int x1, y1, x2, y2;
{
    color (0, wtsb);	/* select color			 */
    plt (x1, y1, x2, y2);
}

delh (x, y)		/* delete hole			 */
    int x, y;
{
    if (pcb[y][x] & ishb) {
	color (0, ishb);
	dpin (x, y);
    }
    return (0);
}

delc (x, y, b)			/* clean deleted wire segments	 */
    int     x, y, b;
{
    struct nlhd *t, *get_net ();

/* printf ("delc: x=%d y=%d b=%2x\n", x, y, b); */


    t = get_net(x, y, b);	/* check connection		 */

    if (!t) {			/* dead wire: delete it		 */
	wtvf = delv;
	wthf = delh;
	wtrace (x, y, b);}
    else if (t != net_t_del) {	/* inconsistent connection !	 */
	printf ("delc: hey! inconsistent wire\n");
	return;			/* don't touch it		 */
    }

    ckgp (x, y, b);		/* clean the wire end		 */
}

home (xp, yp, xs, ys, xo, yo, b, ob)
/********************************\
* 				 *
*  try a home run to pin	 *
* 				 *
*  <xp,yp> current position	 *
*  <xs,ys> last position	 *
*  <xo,yo> origin of this trace	 *
*  <b>     desired side		 *
*  <ob>    last side		 *
* 				 *
\********************************/
    int xp, yp, xs, ys, xo, yo, b;
{
    int     i, j;

    for (i = xp - 3; i <= xp + 3; i += 3)/* look for a nearby pin */
	if ((i > 0) && (i < xmax))
	    for (j = yp - 3; j <= yp + 3; j += 3)
		if ((j > 0) && (j < ymax) && !(~pcb[j][i] & (chb | selb))) {
		    xp = i;
		    yp = j;
		    center (&xp, &yp);
		    i += 10;	/* break out			 */
		    break;
		}

    if (((xp == xo) && (yp == yo)) || (~pcb[yp][xp] & (chb | selb)))
	return (0);		/* do not head for the start point */
/*
 * note: this is the place to add a more powerful filter
 */

    if (!no_hrout)		/* use s-route			 */
	return (s_route (xs, ys, xp, yp));

    if (b & ~ob) {		/* need to change sides first	 */
	if (viaalg (b))
	    return (0);		/* sorry - no space for a hole	 */
    }

    i = abs (xs - xp);
    j = abs (ys - yp);
    i = (i < j) ? i : j;	/* largest square		 */

    if (i > 2) {		/* try diagonal approach	 */
	if (i == abs (xs - xp)) {
	    if (hpok (xs, ys, xp, (yp > ys) ? ys + i : ys - i, xp, yp, b))
		return (1);
	    if (hpok (xs, ys, xs, (yp > ys) ? yp - i : yp + i, xp, yp, b))
		return (1);
	}
	else {
	    if (hpok (xs, ys, (xp > xs) ? xs + i : xs - i, yp, xp, yp, b))
		return (1);
	    if (hpok (xs, ys, (xp > xs) ? xp - i : xp + i, ys, xp, yp, b))
		return (1);
	}
    }
    if (hpok (xs, ys, xs, yp, xp, yp, b))
	return (1);
    if (hpok (xs, ys, xp, ys, xp, yp, b))
	return (1);

    return (0);			/* it did not work	 */

}

hpok (x1, y1, x2, y2, x3, y3, b)/* check proposed home path	 */
int     x1,
        y1,
        x2,
        y2,
        x3,
        y3,
        b;
{
/* printf("hpok: x1=%d y1=%d x2=%d y2=%d x3=%d y3=%d b=%2x\n",
	x1,y1,x2,y2,x3,y3,b); */
    if (wayok (x1, y1, x2, y2, b) && wayok (x2, y2, x3, y3, b)) {
	color (b | selb, b | selb);
	plt (x1, y1, x2, y2);
	plt (x2, y2, x3, y3);
	return (1);
    }
    return (0);
}

wayok (x1, y1, x2, y2, b)	/* check one line		 */
int     x1,
        y1,
        x2,
        y2,
        b;
{
    int     i,
            j,
            dx,
            dy,
            dxd,
            dyd;
/* printf("wayok: x1=%d y1=%d x2=%d y2=%d b=%2x\n",x1,y1,x2,y2,b); */
    dx = (x1 < x2) - (x1 > x2);
    dy = (y1 < y2) - (y1 > y2);
    i = (abs (x1 - x2) > abs (y1 - y2)) ? abs (x1 - x2) : abs (y1 - y2);

    if (dx && dy) {		/* diagonal scan		 */
	dxd = (dx - dy) / 2;
	dyd = (dx + dy) / 2;
	for (j = 0; j < i; ++j) {/* check for illegal areas	 */
	    x1 += dx;
	    y1 += dy;
	    if (wayck (x1 + dx, y1 + dy, b))
		return (0);
	    if (wayck (x1 - dy, y1 + dx, b))
		return (0);
	    if (wayck (x1 + dy, y1 - dx, b))
		return (0);
	    if (wayck (x1 + dxd, y1 + dyd, b))
		return (0);
	    if (wayck (x1 + dyd, y1 - dxd, b))
		return (0);
	}
    }
    else {			/* horizontal / vertical check	 */
	for (j = 0; j < i; ++j) {/* check for illegal areas	 */
	    x1 += dx;
	    y1 += dy;
	    if (wayck (x1 + dx, y1 + dy, b))
		return (0);
	    if (wayck (x1 + dx - dy, y1 + dy + dx, b))
		return (0);
	    if (wayck (x1 + dx + dy, y1 + dy - dx, b))
		return (0);
	}
    }

    return (1);			/* check is ok			 */
}

wayck (x, y, b)			/* test illegal overlap		 */
int     x,
        y,
        b;
{
    int     i;
/* printf("wayck: x=%d y=%d b=%2x\n",x,y,b); */
    i = pcb[y][x];
    if (!(i & selb))
	return (i & (b | ahb));	/* return not 0 for an error	 */
    return (!(i & chb));
}

#define con(X, Y) prev_x+(pvws*((X)))/(i*cz), prev_y+(pvws*((Y)))/(i*cz)
static int prev_x = -1, prev_y;

prev (n, x, y)			/* display preview map		 */
    struct nlhd *n;
    int x, y;
{
    register int i, xx, yy;
    struct nlst *p;

    i = (V.BSX > V.BSY) ? V.BSX : V.BSY;
    prev_x = (x < wx) ? wx :
	     ((x > wx + (512 - pvws) / cz) ? wx + (512 - pvws) / cz : x);
    prev_y = (y < wy) ? wy :
	     ((y > wy + (482 - pvws) / cz) ? wy + (482 - pvws) / cz : y);
    color (resb, resb);

    plts (con (0, 0), con (V.BSX, 0));
    plts (con (V.BSX, 0), con (V.BSX, V.BSY));
    plts (con (V.BSX, V.BSY), con (0, V.BSY));
    plts (con (0, V.BSY), con (0, 0));

    plts (con (wx, wy),
	    con ((wx + 511 / cz > i) ? i : wx + 511 / cz, wy));
    plts (con ((wx + 511 / cz > i) ? i : wx + 511 / cz, wy),
	    con ((wx + 511 / cz > i) ? i : wx + 511 / cz,
		(wy + 481 / cz > i) ? i : wy + 481 / cz));
    plts (con ((wx + 511 / cz > i) ? i : wx + 511 / cz,
		(wy + 481 / cz > i) ? i : wy + 481 / cz),
	    con (wx, (wy + 481 / cz > i) ? i : wy + 481 / cz));
    plts (con (wx, (wy + 481 / cz > i) ? i : wy + 481 / cz),
	    con (wx, wy));

    p = n -> lp;
    while (p) {
	xx = p -> c -> x;
	yy = p -> c -> y;
	point (con (xx, yy));
	p = p -> n;
    }
}

unprev ()			/* un-display preview map	 */
{
    int i;
    if (prev_x >= 0) {
	i = (V.BSX > V.BSY) ? V.BSX : V.BSY;
	color (0, resb);
	rect (con (0, 0), con (i, i));
    }
}

#undef con

adjw (n)			/* adjust window		 */
    struct nlhd *n;
{
    int     tx, ty;

    if (no_adjw)
	return;

    if ((wx <= n -> x1) && (wx + 512 / cz > n -> x2) &&
	    (wy <= n -> y1) && (wy + 482 / cz > n -> y2))
	return;			/* everything is ok		 */

    if (((n -> x2 - n -> x1) < 510 / cz) &&
	    ((n -> y2 - n -> y1) < 480 / cz)) {
				/* fit's in readjusted window	 */
	if ((fx <= n -> x1) && (fx + 512 > n -> x2) &&
		(fy <= n -> y1) && (fy + 512 > n -> y2)) {
				/* fit's into current frame	 */
	    tx = (n -> x1 + n -> x2) / 2 - 256 / cz;
	    tx = (tx > fx + 511 - 512 / cz) ? fx + 511 - 512 / cz : tx;
	    tx = (tx < fx) ? fx : tx;
	    ty = (n -> y1 + n -> y2) / 2 - 242 / cz;
	    ty = (ty > fy + 511 - 512 / cz) ? fy + 511 - 512 / cz : ty;
	    ty = (ty < fy) ? fy : ty;
	    window (tx, ty); }
	else
	    window ((n -> x1 + n -> x2) / 2 - 256 / cz,
		    (n -> y1 + n -> y2) / 2 - 242 / cz);
	return;
    }

    if ((wx > n -> x2) || (wx + 512/cz < n -> x1) ||
	(wy > n -> y2) || (wy + 482/cz < n -> y1))
	window (n -> x1 - 10, n -> y1 - 10); /* doesnt fit in window */
    prev (n, V.BSX, V.BSY);
}

ckgp (x, y, b)		/* check for garbage points		 */
    int x, y, b;
/************************************************************\
* 							     *
*  This routine removes bits on plane <b> at location <x,y>  *
*  if they do not contribute to a connection. A via-hole     *
*  will be removed if it is redundant.			     *
* 							     *
\************************************************************/
{
    register int  i, j, k, p;

    p = pcb[y][x];
    if (!(p & b))
	return;			/* nothing to do		 */

    if (center (&x, &y)) {	/* this is a hole		 */

	color (0, b | selb);	/* scan arround hole		 */
	for (i = 0; i < 8; i++)	/* scan hole area		 */
	    ck_rdnb (x + dr[i][0], y + dr[i][1], b);
	color (0, (b ^ vec) | selb);/* just for fun: do other side */
	for (j = i = 0; i < 8; i++) {/* scan hole area */
	    ck_rdnb (x + dr[i][0], y + dr[i][1], b ^ vec);
	    j |= pcb[y + dr[i][1]][x + dr[i][0]];
	}

	if (~j & vec) {		/* remove center		 */
	    color (0, (~j & vec) | selb);
	    pxl (x, y);
	    if (p & ishb) {	/* remove via holes only	 */
		color (0, selb | ishb);
		dpin (x, y);
	    }
	}

	if (p & selb) {		/* undo deselect		 */
	    k = 1;
	    for (i = x - 1; i <= x + 1; ++i)
		for (j = y - 1; j <= y + 1; ++j)
		    if (pcb[j][i] & (ahb | vec)) {
			if (k) {
			    k = 0;
			    color (selb, selb);
			} pxl (i, j);
		    }
	}}
    else {			/* this is just a bit		 */
	color (0, b | (p & selb));
	ck_rdnb (x, y, b);
    }
}

char uc_tab[256] = {		/* table of useful connections	 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
		1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
		1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
		1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
		1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
		1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
		1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
		0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
		1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
		1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
		1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
		1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1  };

ck_rdnb (x, y, b)	/* check for redundant bit		*/
    int x, y, b;
/***********************************************************\
* 							    *
* The bit <b> at location <x,y> will be removed if it does  *
* not contribute to a electrical connection.		    *
* 							    *
\***********************************************************/
{   int i, j, k;

    for ((j = 1, i = k = 0); i < 8; (++i, j += j))
	k |= (pcb[y + dr[i][1]][x + dr[i][0]] & b) ? j : 0;
    if (uc_tab[k])
	pxl (x, y);
}
