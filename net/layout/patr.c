/***************************************************************\
*								*
*	PCB program						*
*								*
*	Auto router section					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include "pparm.h"
#include "pcdst.h"
#include "pleer.h"
#include <stdio.h>

#define netmax 50		/* max. net length		*/
#define ratio 3			/* ratio for filter		*/
#define pxsb s1b		/* prefere x side bit		*/
#define pysb s2b		/* prefere y side bit		*/
#define lmax 96			/* max length for filter	*/
#define bbox 16			/* bounding box for local route */
#define maxtry1 0		/* limit for N_route (see def)	*/
#define maxtry2 1		/* limit for N_route (see def)	*/
#define maxtry3 3		/* limit for N_route (see def)	*/

#define new_p1 0		/* use sorted segments in pass2 */

struct sntl {			/* sorted net list		 */
    struct nlhd *p;		/* pointer to net		 */
    int c;			/* cost for this net		 */
};

static struct ssegl {		/* sorted segment list		 */
    struct nlhd *p;		/* pointer to net		 */
    int c;			/* cost for this net		 */
    int xs, ys;			/* start coordinates		 */
    int xe, ye;			/* end coordinates		 */
} *SSEGL;

static int sseglc;		/* count of nets		 */


autor()
/*****************************************************************\
* 								  *
*  Auto Router							  *
* 								  *
*  The Auto router uses several phases:				  *
*        1)  use 'filter' no via-holes, 2 direction changes only  *
*        2)  use full router on level 0				  *
*        3)  use full router on level 1				  *
*        4)  use full router on level 2				  *
*        5)  use full router on level 3				  *
*        							  *
* 								  *
\*****************************************************************/
{
    register struct sntl *snl, *s;
    register int i, j, k;
    int netc_key (), seg_key (), segc, netc, max_nl;

    Ferr_msg ("Preparing autoroute");

    snl = (struct sntl *) malloc (V.nnh * sizeof (*snl));


    segc = 0;			/* count segments and nets	 */
    netc = 0;
    max_nl = 0;
    for ((s = snl, j = i = 0); i < V.nnh; i++)/* evaluate nets	 */
	if (NH[i].l > 1 && !(NH[i].f)) {
	    netc++;
	    k = NH[i].l;
	    if (k > max_nl)
		max_nl = k;
	    segc += k - 1;
	    s -> p = &NH[i];
	    s -> c = net_cst (&NH[i]);
	    j++;
	    s++;
	}

    qsort (snl, j, sizeof (*snl), netc_key);/* sort nets		 */

    SSEGL = (struct ssegl *) malloc (sizeof (struct ssegl) * segc);

    printf ("%4d nets with %5d segments needs to be routed\n", netc, segc);

    Ferr_msg ("Autoroute: pass 1");
    netc = segc = 0;
    for ((s = snl, i = 0); i < j; (s++, i++)){/* try routing	 */
	segc += P_route (s -> p);
	netc += s -> p -> f;
    }
    qsort (SSEGL, sseglc, sizeof (struct ssegl), seg_key);

    printf ("\tPass1: %4d nets, %5d segments routed\n", netc, segc);

    Ferr_msg ("Autoroute: pass 2");
    netc = segc = 0;
    set_rtps (0);

#ifdef new_p2
    for (i = 0; i < sseglc; i++)	/* try routing	segments */
	if (!(SSEGL[i].p -> f)) {
	
	    if (V.cnet != SSEGL[i].p) {
		if (V.cnet)
		    deseln (V.cnet);	/* deselect current net	 */
	        selnet (SSEGL[i].p);	/* select net		 */
	    }
	    
	    segc += s_route (SSEGL[i].xs, SSEGL[i].ys,
			     SSEGL[i].xe, SSEGL[i].ye);
 	    netc += SSEGL[i].p -> f = cchk (SSEGL[i].p);
	}

    if (V.cnet)
	deseln (V.cnet);
#else
    for ((s = snl, i = 0); i < j; (s++, i++))/* try routing	 */
	if (!(s -> p -> f)) {
	    segc += N_route (s -> p, 0, 0, 0, V.BSX, V.BSY);
	    netc += s -> p -> f;
	}
#endif
    free (SSEGL);
    printf ("\tPass2: %4d nets, %5d segments routed\n", netc, segc);

    Ferr_msg ("Autoroute: pass 3");
    netc = segc = 0;
    set_rtps (1);
    for ((s = snl, i = 0); i < j; (s++, i++))/* try routing	 */
	if (!(s -> p -> f)) {
	    segc += N_route (s -> p, maxtry1, 0, 0, V.BSX, V.BSY);
	    netc += s -> p -> f;
	}
    printf ("\tPass3: %4d nets, %5d segments routed\n", netc, segc);

    Ferr_msg ("Autoroute: pass 4");
    netc = segc = 0;
    set_rtps (2);
    for ((s = snl, i = 0); i < j; (s++, i++))/* try routing	 */
	if (!(s -> p -> f)) {
	    segc += N_route (s -> p, maxtry2, 0, 0, V.BSX, V.BSY);
	    netc += s -> p -> f;
	}
    printf ("\tPass4: %4d nets, %5d segments routed\n", netc, segc);

    Ferr_msg ("Autoroute: pass 5");
    netc = segc = 0;
    set_rtps (3);
    for ((s = snl, i = 0); i < j; (s++, i++))/* try routing	 */
	if (!(s -> p -> f)) {
	    segc += N_route (s -> p, maxtry3, 0, 0, V.BSX, V.BSY);
	    netc += s -> p -> f;
	}
    printf ("\tPass5: %4d nets, %5d segments routed\n", netc, segc);

    free (snl);			/* free memory			 */

    Ferr_msg ("Saving result");

    save (0);

    Ferr_msg ("Done");
}

netc_key (a, b)			/* sort key for nets		 */
    struct sntl *a, *b;
{
    return ((a -> c > b -> c) - (a -> c < b -> c));
}

seg_key (a, b)			/* sort key for segments	 */
    struct ssegl *a, *b;
{
    return ((a -> c > b -> c) - (a -> c < b -> c));
}

net_cst (net)			/* net cost function		 */
    struct nlhd *net;
/**************************************************************\
* 							       *
*  This function determins the order of rout - attempts. Nets  *
*  with a low cost are routed first.			       *
* 							       *
\**************************************************************/
{
    int     c, dx, dy;

    dx = abs (net -> x1 - net -> x2);
    dy = abs (net -> y1 - net -> y2);

    c = 500 * abs (dx - dy);
    c /= (dx + dy);		/* prefere square nets		 */

    c += net -> l * 100;	/* large nets later		 */

    c += (dx + dy) * 2;		/* long nets later (significat)	 */

    return (c);
}

filter (x1, y1, x2, y2)		/* look for promissing candiates */
    int x1, y1, x2, y2;
/****************************************************************\
* 								 *
*  This code is obsolete, but it is kept because it runs faster	 *
*  for simple cases than the full router in pleer.c		 *
* 								 *
\****************************************************************/
{
    int     i, j;

    i = abs (x1 - x2);
    j = abs (y1 - y2);
    if ((i + j) > lmax)
	return (0);		/* invalid length		 */

    if (i > ratio * j) {
	if (x1 < x2)
	    return (tryx (x1, y1, x2 - x1, y2 - y1, pxsb));
	else
	    return (tryx (x2, y2, x1 - x2, y1 - y2, pxsb));
    }

    if (i * ratio < j) {
	if (y1 < y2)
	    return (tryy (x1, y1, y2 - y1, x2 - x1, pysb));
	else
	    return (tryy (x2, y2, y1 - y2, x1 - x2, pysb));
    }

    return (0);			/* invalid ratio		 */
}

tryx (x, y, l, o, b)		/* try to route a wire (X)	 */
    int x, y, l, o, b;
{
    int i, l1, l2, c1, c2, c3, t;
    char *p, *p1, *p2, *p3;

    t = ahb | b | fb | selb;		/* test bits	 */

    if (o >= 0) {
	p2 = p1 = &pcb[y - 1][x + 2];
	l1 = l - 2;
	c2 = c3 = 0;
	for (i = (-1); i < o + bbox; i++) {
	    c1 = 1;		/* assume space for wire	 */
	    p = p1;
	    for (l2 = (abs(o-i) > 2) ? l1 - abs(o-i) : l1 - 2; l2 >= 0; l2--)
				/* check space			 */
		if (*(p++) & t) {
		    c1 = 0;
		    break;
		}

	    if (c1 & c2 & c3) {	/* looks good so far	 */
		if (cmplx(x, y, --i, l, o, b))
		    return (1);
		break;
	    }

	    c3 = c2;
            c2 = c1;
	    p3 = p2;
	    p2 = p1;
	    p1 += xmax;
	    if (i > 1) {
		if ((*p3 & t) || (*(++p3) & t) || (*p1 & t) || (*(p1-1) & t))
		    break;
		p1++;
		l1--;
	    }
	}

	p1 = &pcb[y][x + 2];	/* ok, try other side next	 */
	l1 = l - 2;
	c2 = c3 = 0;
	for (i = 0; i > -bbox; i--) {
	    c1 = 1;		/* assume space for wire	 */
	    p = p1;
	    for (l2 = ((o-i) > 2) ? l1 - (o-i) : l1 - 2; l2 >= 0; l2--)
				/* check space			 */
		if (*(p++) & t) {
		    c1 = 0;
		    break;
		}

	    if (c1 & c2 & c3) {	/* looks good so far	 */
		if (cmplx(x, y, ++i, l, o, b))
		    return (1);
		break;
	    }

	    c3 = c2;
            c2 = c1;
	    p3 = p2;
	    p2 = p1;
	    p1 -= xmax;
	    if (i < -1) {
		if ((*p3 & t) || (*(++p3) & t) || (*p1 & t) || (*(p1-1) & t))
		    break;
		p1++;
		l1--;
	    }
	}}
    else {		/* same thing, but reversed order	 */
	p1 = &pcb[y + 1][x + 2];
	l1 = l - 2;
	c2 = c3 = 0;
	for (i = 1; i > o - bbox; i--) {
	    c1 = 1;		/* assume space for wire	 */
	    p = p1;
	    for (l2 = (abs(o-i) > 2) ? l1 - abs(o-i) : l1 - 2; l2 >= 0; l2--)
				/* check space			 */
		if (*(p++) & t) {
		    c1 = 0;
		    break;
		}

	    if (c1 & c2 & c3) {	/* looks good so far	 */
		if (cmplx(x, y, ++i, l, o, b))
		    return (1);
		break;
	    }

	    c3 = c2;
            c2 = c1;
	    p3 = p2;
	    p2 = p1;
	    p1 -= xmax;
	    if (i < -1) {
		if ((*p3 & t) || (*(++p3) & t) || (*p1 & t) || (*(p1-1) & t))
		    break;
		p1++;
		l1--;
	    }
	}

	p1 = &pcb[y][x + 2];	/* ok, try other side	 */
	l1 = l - 2;
	c2 = c3 = 0;
	for (i = 0; i < bbox; i++) {
	    c1 = 1;		/* assume space for wire	 */
	    p = p1;
	    for (l2 = ((i-o) > 2) ? l1 - (i-o) : l1 - 2; l2 >= 0; l2--)
				/* check space			 */
		if (*(p++) & t) {
		    c1 = 0;
		    break;
		}

	    if (c1 & c2 & c3) {	/* looks good so far	 */
		if (cmplx(x, y, --i, l, o, b))
		    return (1);
		break;
	    }

	    c3 = c2;
            c2 = c1;
	    p3 = p2;
	    p2 = p1;
	    p1 += xmax;
	    if (i > 1) {
		if ((*p3 & t) || (*(++p3) & t) || (*p1 & t) || (*(p1-1) & t))
		    break;
		p1++;
		l1--;
	    }
	}
    }

    return (0);				/* no success		 */
}

cmplx (x, y, po, l, o, b)		/* complete try (x)	 */
    int x, y, po, l, o, b;
{
    int i, d, t;
    char *p;

    t = ahb | b | fb | selb;

    d = o - po;			/* get displacement	 */

    if (d) {			/* check last segment	 */
	if (d > 0) {		/* down segment		 */
	    if (d > 1) {	/* deserves attention	 */
		d--;
		p = &pcb[y + o - 3][x + l - 1];
		for (i = d; i > 0; (p -= xmax, i--))
		    if ((*p & t) || (*(--p) & t) ||
			(*(p + 2 * xmax) & t) || (*(p + 3 * xmax) & t))
			return (0);	/* fail		 */
		color (b | selb, b | selb);
		plt (x + l, y + o, x + l - d, y + o - d);
		l -= d + 1; }
	    else {
		color (b | selb, b | selb);
		pxl (x + l, y + o);
		l--;		/* just cheat on length	 */
	    } }
	else {			/* up segment		 */
	    d = (-d);
	    if (d > 1) {	/* need attention	 */
		d--;
		p = &pcb[y + o + 3][x + l - 1];
		for (i = d; i > 0; (p += xmax, i--))
		    if ((*p & t) || (*(--p) & t) ||
			(*(p - 2 * xmax) & t) || (*(p - 3 * xmax) & t))
			return (0);/* fail	 */
		color (b | selb, b | selb);
		plt (x + l, y + o, x + l - d, y + o + d);
		l -= d + 1; }
	    else {
		color (b | selb, b | selb);
		pxl (x + l, y + o);
		l--;		/* cheat on length	 */
	    }
	} }
    else
	color (b | selb, b | selb);


    if (abs (po) > 1) {
	plt (x, y, x + abs (po), y + po);
	plt (x + abs (po) + 1, y + po, x + l, y + po); }
    else {
	if (po)
	    pxl (x, y);
	plt (x + abs (po), y + po, x + l, y + po);
    }

    return (1);			/* success			 */
}

tryy (x, y, l, o, b)		/* try to route a wire (Y)	 */
    int x, y, l, o, b;
{
    int i, l1, l2, c1, c2, c3, t;
    char  *p, *p1, *p2, *p3;

    t = ahb | b | fb | selb;		/* test bits	 */

    if (o >= 0) {
	p2 = p1 = &pcb[y + 2][x - 1];
	l1 = l - 2;
	c2 = c3 = 0;
	for (i = (-1); i < o + bbox; i++) {
	    c1 = 1;		/* assume space for wire	 */
	    p = p1;
	    for (l2 = (abs(o-i) > 2) ? l1 - abs(o-i) : l1 - 2; l2 >= 0; l2--){
				/* check space			 */
		if (*p & t) {
		    c1 = 0;
		    break;
		}
		p += xmax;
	    }

	    if (c1 & c2 & c3) {	/* looks good so far	 */
		if (cmply(x, y, --i, l, o, b))
		    return (1);
		break;
	    }

	    c3 = c2;
            c2 = c1;
	    p3 = p2;
	    p2 = p1;
	    ++p1;
	    if (i > 1) {
		if ((*p3 & t) || (*(p3 + xmax) & t) ||
		    (*p1 & t) || (*(p1 - xmax) & t))
		    break;
		p1 += xmax;
		l1--;
	    }
	}

	p1 = &pcb[y + 2][x];	/* ok, try other side next	 */
	l1 = l - 2;
	c2 = c3 = 0;
	for (i = 0; i > -bbox; i--) {
	    c1 = 1;		/* assume space for wire	 */
	    p = p1;
	    for (l2 = ((o-i) > 2) ? l1 - (o-i) : l1 - 2; l2 >= 0; l2--) {
				/* check space			 */
		if (*p & t) {
		    c1 = 0;
		    break;
		}
		p += xmax;
	    }

	    if (c1 & c2 & c3) {	/* looks good so far	 */
		if (cmply(x, y, ++i, l, o, b))
		    return (1);
		break;
	    }

	    c3 = c2;
            c2 = c1;
	    p3 = p2;
	    p2 = p1;
	    --p1;
	    if (i < -1) {
		if ((*p3 & t) || (*(p3 + xmax) & t) ||
		    (*p1 & t) || (*(p1 - xmax) & t))
		    break;
		p1 += xmax;
		l1--;
	    }
	}}
    else {		/* same thing, but reversed order	 */
	p1 = &pcb[y + 2][x + 1];
	l1 = l - 2;
	c2 = c3 = 0;
	for (i = 1; i > o - bbox; i--) {
	    c1 = 1;		/* assume space for wire	 */
	    p = p1;
	    for (l2 = (abs(o-i) > 2) ? l1 - abs(o-i) : l1 - 2; l2 >= 0; l2--){
				/* check space			 */
		if (*p & t) {
		    c1 = 0;
		    break;
		}
		p += xmax;
	    }

	    if (c1 & c2 & c3) {	/* looks good so far	 */
		if (cmply(x, y, ++i, l, o, b))
		    return (1);
		break;
	    }

	    c3 = c2;
            c2 = c1;
	    p3 = p2;
	    p2 = p1;
	    --p1;
	    if (i < -1) {
		if ((*p3 & t) || (*(p3 + xmax) & t) ||
		    (*p1 & t) || (*(p1 - xmax) & t))
		    break;
		p1 += xmax;
		l1--;
	    }
	}

	p1 = &pcb[y + 2][x];	/* ok, try other side	 */
	l1 = l - 2;
	c2 = c3 = 0;
	for (i = 0; i < bbox; i++) {
	    c1 = 1;		/* assume space for wire	 */
	    p = p1;
	    for (l2 = ((i-o) > 2) ? l1 - (i-o) : l1 - 2; l2 >= 0; l2--) {
				/* check space			 */
		if (*p & t) {
		    c1 = 0;
		    break;
		}
		p += xmax;
	    }

	    if (c1 & c2 & c3) {	/* looks good so far	 */
		if (cmply(x, y, --i, l, o, b))
		    return (1);
		break;
	    }

	    c3 = c2;
            c2 = c1;
	    p3 = p2;
	    p2 = p1;
	    ++p1;
	    if (i > 1) {
		if ((*p3 & t) || (*(p3 + xmax) & t) ||
		    (*p1 & t) || (*(p1 - xmax) & t))
		    break;
		p1 += xmax;
		l1--;
	    }
	}
    }

    return (0);				/* no success		 */
}

cmply (x, y, po, l, o, b)		/* complete try (x)	 */
    int x, y, po, l, o, b;
{
    int i, d, t;
    char *p;

    t = ahb | b | fb | selb;

    d = o - po;			/* get displacement	 */

    if (d) {			/* check last segment	 */
	if (d > 0) {		/* down segment		 */
	    if (d > 1) {	/* deserves attention	 */
		d--;
		p = &pcb[y + l - 1][x + o - 3];
		for (i = d; i > 0; (p--, i--))
		    if ((*p & t) || (*(p -= xmax) & t) ||
			(*(p + 2) & t) || (*(p + 3) & t))
			return (0);/* fail	 */
		color (b | selb, b | selb);
		plt (x + o, y + l, x + o - d, y + l - d);
		l -= d + 1; }
	    else {
		color (b | selb, b | selb);
		pxl (x + o, y + l);
		l--;		/* just cheat on length	 */
	    } }
	else {			/* up segment		 */
	    d = (-d);
	    if (d > 1) {	/* need attention	 */
		d--;
		p = &pcb[y + l - 1][x + o + 3];
		for (i = d; i > 0; (p++, i--))
		    if ((*p & t) || (*(p -= xmax) & t) ||
			(*(p - 2) & t) || (*(p - 3) & t))
			return (0);/* fail		 */
		color (b | selb, b | selb);
		plt (x + o, y + l, x + o + d, y + l - d);
		l -= d + 1; }
	    else {
		color (b | selb, b | selb);
		pxl (x + o, y + l);
		l--;		/* cheat on length	 */
	    }
	} }
    else
	color (b | selb, b | selb);


    if (abs (po) > 1) {
	plt (x, y, x + po, y + abs (po));
	plt (x + po, y + abs (po) + 1, x + po, y + l); }
    else {
	if (po)
	    pxl (x, y);
	plt (x + po, y + abs (po), x + po, y + l);
    }

    return (1);			/* success			 */
}

/***************************************\
* 				        *
*  Local stuff for N_route and P_route  *
* 				        *
\***************************************/

struct htab {			/* hole table			 */
   int x, y;			/* coordinate			 */
   int snn;			/* subnet number		 */
};

struct clst {			/* connection list		 */
    struct htab *s, *d;		/* pointer to holes		 */
    int c;			/* cost (= length) or 0		 */
};

ct_key (a, b)
    struct clst *a, *b;
{
    return ((a -> c > b -> c) - (a -> c < b -> c));
}

static int s1, s2, nh;		/* current subnet (for Nr_cup)	 */
static struct htab *htp;	/* hole table pointer		 */

Nr_cup (x, y)			/* dumb update function		 */
    int x, y;
{
    int i;
    struct htab *h;

    if (pcb[y][x] & ishb)
	return (0);		/* don't care			 */

    for ((h = htp, i = 0); i < nh; (h++, i++))
	if (h -> x == x && h -> y == y)
	    h -> snn = s1;

    return (0);
}

N_route (net, max, xl, yl, xh, yh)
    struct nlhd *net;
    int max;
    int xl, yl, xh, yh;
/*******************************************************************\
* 								    *
*  Net-route: This routine tries to route an entire net. A minimal  *
*  spanning tree - approach is used, but if this does not succeed,  *
*  all possible routes are tried. This may take some time for long  *
*  nets. <max> is a limit on the number of attempts to connect 2    *
*  subnets.							    *
*  N_route updates the connectivity flag in the net-list header.    *
*  The number of successfully routed segments is returned.	    *
* 								    *
*  <x/y-h/l> defines a subarea for net parts.			    *
* 								    *
\*******************************************************************/
{
    int    i, j, k, l, ct_key (), Nr_cup (), no_seg;
    struct htab *h1, *h2;
    struct clst *c0, *c1, *c2, *clp;
    struct nlst *p;

    if (V.cnet)
	deseln (V.cnet);	/* deselect current net		 */

    if (net -> l < 2)
	return (0);		/* trivial nets always succeed	 */

    if (selnet (net)) {		/* select net			 */
	deseln (net);
	return (0);		/* net is already done		 */
    }

    no_seg = 0;

#ifdef debug
    if(net == testbp) {
	Ferr_msg ("Break");
	i = 0;			/* set breakpoint here		 */
    }
#endif

    nh = net -> l;		/* (max) number of holes in net	 */
    htp = (struct htab *) malloc (nh * sizeof (*htp));
    clp = (struct clst *) malloc (nh * (nh - 1) * sizeof (*clp) / 2);

    nh = 0;			/* count hole in area of interest */
    for (h1 = htp, p = net -> lp; p; p = p -> n) {
				/* scan holes of net		 */
	h1 -> x = p -> c -> x;
	h1 -> y = p -> c -> y;
	if (h1 -> x >= xl && h1 -> x <= xh &&
	    h1 -> y >= yl && h1 -> y <= yh    ) {
	    h1 -> snn = p -> mk;
	    h1++;
	    nh++;
	}
    }

    if (nh < 2) {		/* nothing to do		 */
	free (htp);
	free (clp);
        deseln (net);
	return 0;
    }

    k = 0;			/* number of possible connections*/
    c0 = clp;
    for ((i = 1, h1 = htp); i < nh; (i++, h1++))
	for ((j = i + 1, h2 = htp + i); j <= nh; (j++, h2++))
	    if (h1 -> snn != h2 -> snn) {

				/* pre-select segment orientation */
		if (abs (h1 -> x - h2 -> x) > abs (h1 -> y - h2 -> y)) {
		    if (h1 -> x > h2 -> x) {
			c0 -> d = h1;
			c0 -> s = h2;}
		    else {
		        c0 -> s = h1;
		        c0 -> d = h2;
		    }}
		else {
		    if (h1 -> y > h2 -> y) {
			c0 -> d = h1;
			c0 -> s = h2;}
		    else {
		        c0 -> s = h1;
		        c0 -> d = h2;
		    }
		}

		c0 -> c = abs (h1 -> x - h2 -> x) + abs (h1 -> y - h2 -> y);
		c0++;
		k++;
	    }

    qsort (clp, k, sizeof (*clp), ct_key);

#ifdef debug
    if (ttt) {
	c0 = clp;
	for (i = 0; i < k; (c0++, i++))
	printf ("%2d: s=%3d,%3d (%3d) d=%3d,%3d (%3d) c=%d\n", i,
	        c0 -> s -> x, c0 -> s -> y, c0 -> s -> snn,
		c0 -> d -> x, c0 -> d -> y, c0 -> d -> snn, c0 -> c);
    }
#endif

    for ((i = 0, c0 = clp); i < k; i++)
	(c0++) -> c = 0;	/* re-use cost as retry count	 */

    for ((i = 0, c0 = clp); i < k; (c0++, i++)) {
				/* try to connect the subnets	 */
	if (c0 -> c > max)	/* max. try count exceeded ?	 */
	    continue;

	s1 = c0 -> s -> snn;
	s2 = c0 -> d -> snn;

	if ((s1 != s2) &&	/* try to join 			 */
	    s_route (c0 -> s -> x, c0 -> s -> y, c0 -> d -> x, c0 -> d -> y)){
#ifdef debug
		if (ttt) printf("connected subnet %d to %d\n", s1, s2);
#endif
	    no_seg++;
	    wtvf = nil;		/* update conectivity information*/
	    wthf = Nr_cup;
	    wtrace (c0 -> s -> x, c0 -> s -> y, vec);

	    c1 = c2 = c0 + 1;
	    l = k;
	    for (j = i + 1; j < k; (c1++, j++)) {
		if (s1 != c1 -> s -> snn || s1 != c1 -> d -> snn) {
				/* viable connection */
		    c2 -> s = c1 -> s;
		    c2 -> d = c1 -> d;
		    c2 -> c = c1 -> c;
		    c2++;}
		else
		    l--;
	    }
	    k = l;}
	else {
#ifdef debug
	    if (ttt)
		printf ("failed to connect %d to %d\n", s1, s2);
#endif
	    c1 = c0 + 1;
	    for (j = i + 1; j < k; (c1++, j++))
		if ((s1 == c1 -> s -> snn && s2 == c1 -> d -> snn) ||
		    (s2 == c1 -> s -> snn && s1 == c1 -> d -> snn))
		    (c1 -> c)++;
	}
#ifdef debug
	if (ttt) {
	    c1 = c0 +1;
	    Ferr_msg ("test");
	    for(l = i + 1; l < k; (c1++, l++))
		printf ("%2d: s=%3d,%3d (%3d) d=%3d,%3d (%3d) c=%d\n", l,
		        c1 -> s -> x, c1 -> s -> y, c1 -> s -> snn,
			c1 -> d -> x, c1 -> d -> y, c1 -> d -> snn, c1 -> c);
	}
#endif
    }

    j = htp -> snn;
    h1 = htp + 1;
    for (i = 1; i < nh;(h1++, i++))/* more than one subnet left? */
	if (h1 -> snn != j)
	    break;

			/* net done ?			 */
    net -> f = (i >= nh && nh == net -> l) || cchk (net);

    deseln (net);
    free (clp);			/* free memory			 */
    free (htp);
    return (no_seg);
}

P_route (net)
    struct nlhd *net;
/*********************************************************************\
* 								      *
*  Primitive-route: This routine tries to route the *trivial* parts   *
*  of a net. A minimal spanning tree approch is used. This code	      *
*  should run on empty nets only. It is intended to be used in	      *
*  a pre-routing phase of the auto-router. Multiple connections	      *
*  may result on a partially completed net. This routine is	      *
*  faster than N_route.						      *
* 								      *
*  The parts of the MST that are not routed are saved to the tot_mst  *
*  structure. You understand that this side-effect is used	      *
*  by 'autor', so do not call P_route elsewhere.		      *
* 								      *
\*********************************************************************/
{
    int    i, j, k, l, ct_key (), no_seg;
    struct htab *h1, *h2;
    struct clst *c0, *c1, *c2, *clp;
    struct nlst *p;
    int dx, dy;
    int ttc = 0;		/* test count (check MST)	 */

    if (V.cnet)
	deseln (V.cnet);	/* deselect current net		 */

    if (net -> l < 2)
	return (0);		/* trivial nets always succeed	 */
    no_seg = 0;

    if (selnet (net)) {		/* select net			 */
	deseln (net);
	return (0);		/* net is already done		 */
    }

    nh = net -> l;		/* number of holes in net	 */
    htp = (struct htab *) malloc (nh * sizeof (*htp));
    clp = (struct clst *) malloc (nh * (nh - 1) * sizeof (*clp) / 2);

    i = 0;			/* count subnets		 */
    for ((h1 = htp, p = net -> lp); p; (h1++, p = p -> n)) {
				/* scan holes of net		 */
	h1 -> x = p -> c -> x;
	h1 -> y = p -> c -> y;
	h1 -> snn = p -> mk;
    }

    k = 0;			/* number of possible connections*/
    c0 = clp;
    for ((i = 1, h1 = htp); i < nh; (i++, h1++))
	for ((j = i + 1, h2 = htp + i); j <= nh; (j++, h2++)) {

				/* pre-select segment orientation */
		if (abs (h1 -> x - h2 -> x) > abs (h1 -> y - h2 -> y)) {
		    if (h1 -> x > h2 -> x) {
			c0 -> d = h1;
			c0 -> s = h2;}
		    else {
		        c0 -> s = h1;
		        c0 -> d = h2;
		    }}
		else {
		    if (h1 -> y > h2 -> y) {
			c0 -> d = h1;
			c0 -> s = h2;}
		    else {
		        c0 -> s = h1;
		        c0 -> d = h2;
		    }
		}
				/* get cost (length) of this segment */
		c0 -> c = abs (h1 -> x - h2 -> x) + abs (h1 -> y - h2 -> y);
		c0++;
		k++;
	}

    qsort (clp, k, sizeof (*clp), ct_key);

    for ((i = 0, c0 = clp); i < k; (c0++, i++)) {
				/* try to connect the subnets	 */
	s1 = c0 -> s -> snn;
	s2 = c0 -> d -> snn;

	if (s1 != s2) {		/* try connection		 */

	    ttc++;		/* count MST edges		 */

	    if (filter (c0 -> s -> x, c0 -> s -> y,
			c0 -> d -> x, c0 -> d -> y ))
		no_seg++;
	    else {		/* add segments to SSEGL	 */
		SSEGL[sseglc].p = net;

/************************************************\
* 						 *
*  Segment list cost function: small cost first	 *
* 						 *
\************************************************/
		dx = abs (c0 -> s -> x - c0 -> d -> x);
		dy = abs (c0 -> s -> y - c0 -> d -> y);
		SSEGL[sseglc].c = c0 -> c * 2 +	     /* long segments later */
		   (100 * abs (dx - dy)) / abs (dx + dy);  /* square bonus  */

		SSEGL[sseglc].xs = c0 -> s -> x;
		SSEGL[sseglc].ys = c0 -> s -> y;
		SSEGL[sseglc].xe = c0 -> d -> x;
		SSEGL[sseglc].ye = c0 -> d -> y;
		sseglc++;
	    }

	    for ((j = 0, h1 = htp); j < nh; (j++, h1++))
		if (h1 -> snn == s2)
		    h1 -> snn = s1;	/* subnet s2 is gone	 */
	    c0 -> d -> snn = s1;
	    c1 = c2 = c0 + 1;
	    l = k;
	    for (j = i + 1; j < k; (c1++, j++)) {
		if (s1 != c1 -> s -> snn || s1 != c1 -> d -> snn) {
				/* viable connection */
		    c2 -> s = c1 -> s;
		    c2 -> d = c1 -> d;
		    c2++;}
		else
		    l--;
	    }
	    k = l;
	}
    }

    net -> f = cchk (net);	/* net done ?			 */

    if(ttc > (net->l -1))
	err("P_route: not MST", ttc, net -> l,
	    (int) (net - NH) / sizeof(NH[0]), 0);

    free (clp);			/* free memory			 */
    free (htp);
    deseln (net);
    return (no_seg);
}
