/***************************************************************\
*								*
*	PCB program						*
*								*
*	Wide wire support stuff					*
*								*
*	(c) 1985	A. Nowatzyk				*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"

extern char uc_tab[];			/* see pwork: ck_rdnb		 */
extern int pdr[8];			/* offset direction table	 */
extern int sp[9][2];			/* search pattern		 */
extern char drc_tab1[256];		/* drc decision table		 */

static char **pnt_lst = 0;		/* point list			 */
static int pnt_max = xmax;		/* max number of points		 */
static int pnt_cnt;			/* point count			 */

static int side;			/* side to work on		 */

fnd_pnt (x, y)
    int x, y;
/**************************************************************************\
* 									   *
*   find points: (similar to fnd_seg of pplow.c)			   *
*     <x,y> must point to a part of a trace on side "side". fnd_pnt dumps  *
*     the pointer to all bits of that segment on the segment list. The	   *
*     segment is terminated by either the center of a hole, the center	   *
*     of a 'Y' or the last point of a floating end. The terminating	   *
*     point *is* added to the segment list.				   *
* 									   *
\**************************************************************************/
{
    register int i;

    if (!pnt_lst) {		/* alloacte point list	 */
	pnt_lst = (char **) p_malloc (sizeof (char *) * pnt_max);
    }
    pnt_cnt = 0;		/* reset point list counter	 */

    if (!(pcb[y][x] & side))
	return;			/* no start point !!		 */

    for (i = 0; i <= 8; i++)	/* find direction		 */
	if (pcb[y + dr[i][1]][x + dr[i][0]] & side)
	   break;

    if (i > 7)
	return;			/* start on single point ??	 */

				/* scan one way			 */
    pnt_scan (&pcb[y][x], i, side);
				/* scan the other way		 */
    pnt_scan (&pcb[y + dr[i][1]][x + dr[i][0]], (i + 4) & 7, side);
}

pnt_scan (p, d, s)
    char *p;
    int d, s;
/****************************************************************\
* 								 *
*  Point scan:							 *
*    <p> points to the start point. <d> is the direction of the	 *
*    next point. The next point will be added to the list. Scan	 *
*    stops if that point was a terminal, that is the center of	 *
*    a hole, a 'Y' or an floating end.				 *
* 								 *
\****************************************************************/
{
    register char *pp;
    register int i, j = d, ss = ~s, k, l;
    int m;

    pp = p + pdr[j];

    do {

        if (pnt_cnt >= pnt_max) {	/* need more space	 */
	    pnt_max += 10 + pnt_max / 2;
	    pnt_lst = (char **) p_realloc (pnt_lst, sizeof (char *) * pnt_max);
	}

	pnt_lst[pnt_cnt++] = pp;	/* add point		 */
	
	i = 0;
        if (*(pp        + 1) & ~ss) i++;
        if (*(pp + xmax + 1) & ~ss) i++;
        if (*(pp + xmax    ) & ~ss) i++;
        if (*(pp + xmax - 1) & ~ss) i++;
        if (*(pp        - 1) & ~ss) i++;
        if (*(pp - xmax - 1) & ~ss) i++;
        if (*(pp - xmax    ) & ~ss) i++;
        if (*(pp - xmax + 1) & ~ss) i++;

	if (i > 2) {		/* check for a real Y		 */
	    for (m = 0; m < 8; m++)
		if (*(pp + pdr[m]) & ~ss) {
		    k = 0;
		    for (l = 0; l < 8; l++)
			if (*(pp + pdr[m] + pdr[l]) & ~ss)
			    k |= 1 << l;
		    i -= uc_tab[k];
		}
	}

        if (i != 2) break;	/* floating or Y terminal point	 */

        if (*pp & ahb &&	/* hole terminal point check	 */
	    *(pp        + 1) & ahb &&
	    *(pp + xmax + 1) & ahb &&
	    *(pp + xmax    ) & ahb &&
	    *(pp + xmax - 1) & ahb &&
	    *(pp        - 1) & ahb &&
	    *(pp - xmax - 1) & ahb &&
	    *(pp - xmax    ) & ahb &&
	    *(pp - xmax + 1) & ahb    ) break;

        for (i = 6; i < 11; i++)	/* find new direction	 */
	    if (*(pp + pdr[(j + i) & 7]) & ~ss)
		break;
	pp += pdr[j = (j + i) & 7];

    } while (i < 11);
}

expand (n)			/* widen wire by n		 */
    int n;
{
    static int wt5[12] = 
	{2, xmax + 2, 2 * xmax + 1, 2 * xmax, 2 * xmax - 1, xmax - 2,
	 -2, -2 - xmax, -1 - 2 * xmax, -2 * xmax, 1 - 2 * xmax, 2 - xmax};
    static int *wt[2] = {pdr, wt5};
    static int wl[2] = {8, 12};
    register int i, j, k, t;
    register char *p;

    if (n < 0 || n > 1)
	return;			/* you may want to add 2, 3...	 */

    t = ~side;			/* DRC test			 */

    for (i = 0; i < pnt_cnt; i++)
	for (j = 0; j < wl[n]; j++) {
	    p = pnt_lst[i] + wt[n][j];	/* new candidate	 */
	    if (*p & side)
		continue;		/* already taken	 */

	    for (k = 0; k < 8; k++) 	/* check DRC		 */
		if (drc_tab1[*(p + pdr[k]) & 0xff] & ~t)
		    break;

	    if (k >= 8)			/* ok			 */
		pxl (((int) p - (int) pcb) % xmax,
		     ((int) p - (int) pcb) / xmax);
	}
}

wide_wire (ctx)			/* wide wire function		 */
    int ctx;
{
    struct nlhd *get_net (), *net;
    register int i, j;
    int x, y;
    static char *mtext[] = {
	"13 mil (default)",
	"38 mil",
	"63 mil"
    };

    err_msg ("Point to wire");
    getcur (&x, &y);

    for (i = 0; i < 9; i++)	/* look for wire		 */
	if (pcb[y + sp[i][1]][x + sp[i][0]] & vec)
	    break;

    if (i >= 9) {
	err_msg ("No wire found");
	return ctx;
    }

    x += sp[i][0];		/* update wire pointer		 */
    y += sp[i][1];
    side = pcb[y][x] & vec;
    if (side == vec)
	side = top_side;

    for (i = 0, j = 0; i < 8; i++)
	if (pcb[y + dr[i][1]][x + dr[i][0]] & side)
	    j++;
    if (j > 2) {
	err_msg ("no expand on expanded wire");
	return ctx;
    }

    switch (menu (mtext, 3)) {
	case 1:
	    i = 1;		/* 2 raster units + line width	 */
	    break;
	case 2:
	    i = 2;		/* 4 raster units + line width	 */
	    break;
	default:
	    err_msg ("No change made");
	    return ctx;
    }

    net = get_net (x, y, side);	/* find net			 */

    if (V.cnet != net) {	/* select it			 */
	if (V.cnet)
	    deseln (V.cnet);
	selnet (net);
    }

    fnd_pnt (x, y);		/* find the stuff		 */

    color (selb | side, selb | side);	/* set up color		 */
    for (j = 0; j < i; j++)
	expand (j);		/* here we go			 */

    for (i = 0; i < pnt_cnt; i++)
	if (*pnt_lst[i] & ahb) {	/* clean up holes	 */
	    x = ((int) pnt_lst[i] - (int) pcb) % xmax;
	    y = ((int) pnt_lst[i] - (int) pcb) / xmax;
	    ckgp (x, y, s1b);
	    ckgp (x, y, s2b);
	}

    return net_sel (START);
}
