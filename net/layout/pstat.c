/***************************************************************\
*								*
*	PCB program						*
*								*
*	Statistics routines					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"

#define c_grid 32			/* cc - grid width	 */
#define pscale 20			/* size of project plot  */

static int s1_wlen, s2_wlen;		/* side 1/2 wire length	 */
static int vh_cnt;			/* via hole count	 */
static int seg_done;			/* segments done	 */
static int seg_tot;			/* segments total	 */
static int s_valid = 0;			/* =1 if cc statistic ok */

static int *cc_array = 0;		/* congestion cntr array */
static int nccx, nccy;			/* cca organization	 */
static int xpro[xmax], ypro[ymax];	/* net projections	 */
static int cca_tot, xp_tot, yp_tot;	/* totals		 */
static int n_vect;			/* number of vectors	 */

wire_sta ()			/* update wire statistic	 */
{
    struct nlhd *net;		/* preserve selection		 */
    register int i, j, x, y;
    register struct nlst *p;
    int wstat_hf (),  wstat_vf ();

    if (net = V.cnet)
	deseln (net);		/* deselect net (to prevent side effects) */

    s1_wlen = 0;		/* reset counter		 */
    s2_wlen = 0;
    vh_cnt = 0;
    seg_done = 0;
    seg_tot = 0;
    n_vect = 0;

    wthf = wstat_hf;
    wtvf = wstat_vf;

    for (i = 0; i < V.nnh; i++) {	/* scan nets		 */

	if (!NH[i].l)
	    continue;			/* skip deleted nets	 */

	for (p = NH[i].lp; p; p = p -> n)
	    p -> mk = -1;		/* remove marks		 */

	for (j = 0, p = NH[i].lp; p; p = p -> n)
	    if (p -> mk == -1) {
		j++;
		x = p -> c -> x;
		y = p -> c -> y;
		wtrace (x, y, vec);
	    }

	seg_done += NH[i].l - j;
	seg_tot += (NH[i].l - 1);
    }

    if (net)				/* reselect net		 */
	selnet (net);
}

wstat_hf (x, y)				/* hole function for wire_sta */
    int x, y;
{
    register struct hole *hp;
    struct hole *fndh();

    if (pcb[y][x] & ishb)
	vh_cnt++;
    else {
	hp = fndh (x, y);
	if (!hp)
	    err ("wstat_hf: could not find hole", x, y, 0, 0);
	else
	    hp -> n -> mk = 0;
    }

    return 0;
}

wstat_vf (x1, y1, x2, y2)		/* vector function for wire_sta */
    int x1, y1, x2, y2;
{
    register int i, j;
    double sqrt ();

    n_vect++;

    i = x1 - x2;
    j = y1 - y2;

    i = sqrt ((double) (i * i) + (double) (j * j)) + 0.5;

    if (wtsb == s1b)
	s1_wlen += i;
    else
        s2_wlen += i;
}

cc_stat ()				/* congestion analysis		 */
{
    register int i, j, k, l, m, n;

    if (s_valid)
	return;				/* still valid		 */
    s_valid = 1;

    for (i = 0; i < V.BSX; i++)		/* clear counter arrays	 */
	xpro[i] = 0;	
    for (i = 0; i < V.BSY; i++)
	ypro[i] = 0;	

    if (!cc_array) {			/* allocate cca		 */
	nccx = (V.BSX - c_grid / 2) / c_grid + 1;
	nccy = (V.BSY - c_grid / 2) / c_grid + 1;
	cc_array = (int *) p_malloc (sizeof (int) * nccx * nccy);
    }
    j = nccx * nccy;

    for (i = 0; i < j; i++)
	cc_array[i] = 0;

    for (i = 0; i <V.nnh; i++) {

	if (!NH[i].l)
	    continue;			/* skip deleted nets		 */

	for (j = NH[i].x1; j < NH[i].x2; j++)	/* update x-projection */
	    xpro[j]++;
	for (j = NH[i].y1; j < NH[i].y2; j++)	/* update y-projection */
	    ypro[j]++;

	for (j = NH[i].y1 / c_grid, k = NH[i].y2 / c_grid; j <= k; j++) {
	    l = j * nccx;
	    for (m = NH[i].x1 / c_grid, n = NH[i].x2 / c_grid; m <= n; m++)
		cc_array[l + m]++;
	}
    }

    for (i = 0, j = 0; i < V.BSX; i++)		/* get totals		*/
	j += xpro[i];
    xp_tot =j;

    for (i = 0, j = 0; i < V.BSY; i++)
	j += ypro[i];
    yp_tot =j;

    for (i = 0, j = 0, k = nccx * nccy; i < k; i++)
	j += cc_array[i];
    cca_tot = j;
}

flush_stat ()			/* invalidate statistics		 */
{
   s_valid = 0;
}

pgen_stat ()			/* print general statistics		 */
{
    register int i, j;
    double f, f1;

    Ferr_msg ("Collecting data");

    wire_sta ();		/* get statistics			*/
    cc_stat ();
    
    printf ("PCB Version: %d.%2d\n", V.pver / 100, V.pver % 100);

    f1 = 3.937007874;
    f1 *= rsun;

    for (i = j = V.ncp; i; j -= !strcmp (CP[--i].name, DELETED));
				/* count undeleted components		 */

    printf ("\nGeneral statistics:\n");
    printf ("\tBoard size: . . . . . . . . . . . . . . %5.2lf by %5.2lf cm\n",
	(double) V.BSX / f1, (double) V.BSY / f1);
    printf ("\tNumber of components: . . . . . . . . . %d\n", j);
    printf ("\tNumber of component holes:  . . . . . . %d\n", V.nch);
    printf ("\tCurrent number of via holes:  . . . . . %d\n", vh_cnt);

    for (i = 0, j = 0; i < V.nnh; i++)
	if (NH[i].l) j++;	/* count nets				 */

    printf ("\tNumber of nets: . . . . . . . . . . . . %d\n", j);
    printf ("\tNumber of point-to-point connections: . %d\n", seg_tot);

    printf ("\nRouting information:\n");

    for (i = 0, j = 0; i < V.nnh; i++)
	if (NH[i].f) j++;
    printf ("\tNumber of nets routed:  . . . . . . . . %-4d (%5.1lf%%)\n",
	j, (double) (100 * j) / (double) V.nnh);
    printf ("\tNumber of routed p-p connections: . . . %-4d (%5.1lf%%)\n",
	seg_done, (double) (100 * seg_done) / (double) seg_tot);
    printf ("\tWire length on side 1 (component):  . . %5.1lf cm\n",
	(double) s1_wlen / f1);
    printf ("\t            on side 2 (solder): . . . . %5.1lf cm\n",
	(double) s2_wlen / f1);
    printf ("\t            total:  . . . . . . . . . . %5.1lf cm\n",
	(double) (s1_wlen + s2_wlen) / f1);

    for (i = 0, j = 0; i < V.nnh; i++)
	j += abs (NH[i].x1 - NH[i].x2) + abs (NH[i].y1 - NH[i].y2);
    printf ("\t            estimated:  . . . . . . . . %5.1lf cm\n",
	(double) j / f1);

    printf ("\tNumber of staight wire segments:  . . . %d\n", n_vect);

    f1 = V.BSX * V.BSY;
    f1 /= 100 * rsun * rsun;		/* f1: area in sq inches */
    printf ("\tEquivalent density (14DIP / sqi): . . . %-5.2lf\n",
	(double) V.nch / (14.0 * f1));

    for (i = 0, f = 0.0, j = nccx * nccy; i < j; i++)
	f += cc_array[i] * cc_array[i];
    f1 = cca_tot;
    f1 /= j;
    printf ("\tCongestion index (1=evenly distributed) %5.3lf\n",
	f / ((double) j * f1 * f1));

    err_msg ("Done");
}

dis_pro ()		/* display wire desity projections	*/
{
    register int i, j, k, l;
    int x, y;

    cc_stat ();		/* update statistic			*/
    msg_off ();

    color (resb, resb);

    for (i = wx, j = wx + (512 /cz) - pscale, k = 1; i < j; i++)
	if (k < xpro[i])
	    k = xpro[i];		/* find  x max		*/

    l = wy + (482 / cz) - pscale;	/* plot x-scale		*/
    for (i = wx, j = wx + (512 /cz) - pscale; i < j; i++)
	plts (i, l, i, l + (pscale * xpro[i]) / k);

    for (i = wy, j = wy + (482 /cz) - pscale, k = 1; i < j; i++)
	if (k < ypro[i])
	    k = ypro[i];		/* find  y max		*/

    l = wx + (512 / cz) - pscale;	/* plot y-scale		*/
    for (i = wy, j = wy + (482 /cz) - pscale; i < j; i++)
	plts (l, i, l + (pscale * ypro[i]) / k, i);

    getcur (&x, &y);			/* wait for response	 */

    color (0, resb);

    rect (wx, wy + 482/cz - pscale - 1, wx + 512/cz + 1, wy + 482/cz + 2);
    rect (wx + 512/cz - pscale - 1, wy, wx + 512/cz + 2, wy + 482/cz + 1);

    msg_on ();
}

dis_cc ()			/* display congestion		*/
{
    register int i, j, k, l, m, n;
    int x, y, x1, x2, y1, y2;

    x1 = wx / c_grid;
    x2 = (wx + 512/cz -1) / c_grid;
    y1 = wy / c_grid;
    y2 = (wy + 482/cz -1) / c_grid;

    cc_stat ();

    for (i = y1, k = 1; i < y2; i++) {	/* find max desity */
	l = i * nccx;
	for (m = x1, n = x2; m < n; m++) {
	    j = cc_array[m + l];
	    if (k < j * j)
		k = j * j;
	}
    }

    msg_off ();
    color (resb, resb);

    k += k;
    for (i = y1; i < y2; i++) {		/* plot result		 */
	l = i * nccx;
        y = c_grid / 2 + c_grid * i;
	for (m = x1; m < x2; m++) {
	    x = c_grid / 2 + c_grid * m;
	    j = cc_array[m + l];
	    j *= j * c_grid;
	    j /= k;			/* get size of square */
	    rect (x - j, y - j, x + j, y + j);
	}
    }

    getcur (&x, &y);

    color (0, resb);
    rect ((wx - c_grid/2 < 0) ? 0 : wx - c_grid/2,
	  (wy - c_grid/2 < 0) ? 0 : wy - c_grid/2,
	   wx + 512/cz + c_grid/2, wy + 482/cz + c_grid/2);
    msg_on ();
}
