/***************************************************************\
*								*
*	PCB program						*
*								*
*	Lee Router section  (main part: common code)		*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include "pparm.h"
#include "pcdst.h"
#define mainleer		/* this is the main section	 */
#include "pleer.h"


get_rtprm ()			/* get route parameter		 */
{
    RT_sel = getint ("Select router (0=full, 1=confined)", 0, 1, RT_sel);

    if (RT_sel) {
        rt_str = getint("Preference start (0=alt, 1=Componet-, 2=Soder side)",
			0, 2, rt_str);
	K1 = getint ("Max wrong distance for singe side runs", 0, 100, K1);
	K2 = getint ("Border size for single side traces", 1, 100, K2);
	K3 = getint ("Border size for multiple side trcaes", 1, K3max, K3);
	K4 = getint ("Min length for multiple via hole traces", 20, 1000, K4);
	K5 = getint ("Min length for each segment", K3 + 1, 1000, K5);
	K6 = getint ("Max number of via holes for one connection", 2, 10, K6);
	K7 = getint ("Max level for L-routes retries", 0, 10, K7);
	K8 = -getint ("Bonus for rectangular vias", -20, 20, -K8);}
    else {
	C1 = getint ("Boarder size for routing area", 1, 100, C1);
	C2 = getint ("Max path in wrong direction", 1, 100, C2);
	C3 = getint ("Max number of via holes in one segment", 0, 10, C3);
	C4 = getint ("Penalty for HV via holes", 0, 50, C4);
	C5 = getint ("Penalty for D via hols", C4, 50, C5);
	C7 = getint ("Penalty progression for via hols", 0, 50, C7);
	C6 = 2 * getint ("Via hole alignment", 1, 8, C6 / 2);
    }
}

set_rtps (n)			/* set routing parameters	 */
    int n;
/**********************************************************************\
* 								       *
*  n denotes a standart set of routing parameter. These parameters     *
*  increase the search space with increasing n. A larger search space  *
*  needs more CPU time and produces more obscure result.	       *
* 								       *
\**********************************************************************/
{
#define nmax 4			/* tere are thre sets defined	 */
    static struct rtp {
	int c1, c2, c3, c4, c5, c6, c7;
    } rtps[nmax] = {
	{4, 8, 0, 10, 15, 8, 9},
	{10, 8, 1, 30, 45, 8, 9},
	{15, 12, 2, 20, 30, 4, 6},
	{25, 16, 4, 10, 20, 2, 3}
    };

    if (n < 0 || n >= nmax) {
	printf ("set_rtps: Unkown parameter set - ignored\n");
	return;
    }

    RT_sel = 0;			/* use full router		 */

    C1 = rtps[n].c1;
    C2 = rtps[n].c2;
    C3 = rtps[n].c3;
    C4 = rtps[n].c4;
    C5 = rtps[n].c5;
    C6 = rtps[n].c6;
    C7 = rtps[n].c7;
}

cr_gmaze (x1, y1, x2, y2, b)	/* create a good maze		 */
    int x1, y1, x2, y2, b;
{   register int i;

    if (x1 > x2) {		/* sort x			 */
	i = x1;
	x1 = x2;
	x2 = i;
    }
    if (y1 > y2) {		/* sort y			 */
	i = y1;
	y1 = y2;
	y2 = i;
    }
    x1 -= b;			/* add border			 */
    y1 -= b;
    x2 += b;
    y2 += b;
    x1 = (x1 < 0) ? 0 : x1;	/* bitmap clipping		 */
    y1 = (y1 < 0) ? 0 : y1;
    x2 = (x2 > V.BSX) ? V.BSX : x2;
    y2 = (y2 > V.BSY) ? V.BSY : y2;
    cr_maze (x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

cr_maze (px, py, qx, qy)	/* create maze (aux. bit map)	 */
    int px, py, qx, qy;
{
    static int tab1[256] = {
/*************************************************************************\
* 									  *
*  Pixel conversion table: indexed by an element of pcb[][], it returns:  *
*  MSB ===---===---=== LSB						  *
*      00.00.00.00.00.       These bits are allways 0			  *
*      ..............1       (1)   : selected hole	slahb		  *
*      ...........1...       (8)   : invalid side 1	ivs1b		  *
*      ........1......       (64)  : invalid side 2	ivs2b		  *
*      .....1.........       (512) : selected side 1	sls1b		  *
*      ..1............       (4096): selected side 2	sls2b		  *
* 									  *
\*************************************************************************/
		   0,    8,   64,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		   0,    8,   64,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		   0,    8,   64,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		   0,    8,   64,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		   0,  512, 4096,  576,    1,    1,    1,    1,
		   1,    1,    1,    1,    1,    1,    1,    1,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		   0,  512, 4096, 4104,    1,    1,    1,    1,
		   1,    1,    1,    1,    1,    1,    1,    1,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		   0,  512, 4096,  576,    1,    1,    1,    1,
		   1,    1,    1,    1,    1,    1,    1,    1,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72,
		   0,  512, 4096, 4104,    1,    1,    1,    1,
		   1,    1,    1,    1,    1,    1,    1,    1,
		  72,   72,   72,   72,   72,   72,   72,   72,
		  72,   72,   72,   72,   72,   72,   72,   72  };

    static int tab2[512] = {
/****************************************************************************\
* 									     *
*  DDE-table: if indexed by an 9 bit integer LSB<876543210>LSB, it returns:  *
*  MSB ===---=== LSB							     *
*      00.00.00.      These bits are allways 0				     *
*      ........1      (1) : if  bit0 & bit1 & bit2			     *
*      .....1...      (8) : if  bit3 | bit4 | bit5			     *
*      ..1......      (64): if  bit6 | bit7 | bit8			     *
* 									     *
\****************************************************************************/
		 0, 0, 0, 0, 0, 0, 0, 1, 8, 8, 8, 8, 8, 8, 8, 9,
		 8, 8, 8, 8, 8, 8, 8, 9, 8, 8, 8, 8, 8, 8, 8, 9,
		 8, 8, 8, 8, 8, 8, 8, 9, 8, 8, 8, 8, 8, 8, 8, 9,
		 8, 8, 8, 8, 8, 8, 8, 9, 8, 8, 8, 8, 8, 8, 8, 9,
		64,64,64,64,64,64,64,65,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		64,64,64,64,64,64,64,65,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		64,64,64,64,64,64,64,65,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		64,64,64,64,64,64,64,65,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		64,64,64,64,64,64,64,65,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		64,64,64,64,64,64,64,65,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		64,64,64,64,64,64,64,65,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73,
		72,72,72,72,72,72,72,73,72,72,72,72,72,72,72,73  };

    static int tab3[128] = {
/***********************************************************\
* 							    *
*  decision table:					    *
*  Input:                 MSB ======= LSB		    *
*                             .x..x..      Don't care bits  *
* 			      1......      invalid side 2   *
*                             ..1....      selected side 2  *
*                             ...1...      invalid side 1   *
*                             .....1.      selected side 1  *
*                             ......1      selected hole    *
* 							    *
*  Output:               MSB ----==== LSB		    *
* 			     ....tttt      side 1 token	    *
* 			     tttt....      side 2 token	    *
* 							    *
\***********************************************************/
		 0,34,50,34, 0,34,50,34, 1,34,50,34, 1,34,50,34,
		35,34,34,34,35,34,34,34,33,34,34,34,33,34,34,34,
		 0,34,50,34, 0,34,50,34, 1,34,50,34, 1,34,50,34,
		35,34,34,34,35,34,34,34,33,34,34,34,33,34,34,34,
		16,34,18,34,16,34,18,34,17,34,18,34,17,34,18,34,
		35,34,34,34,35,34,34,34,33,34,34,34,33,34,34,34,
		16,34,18,34,16,34,18,34,17,34,18,34,17,34,18,34,
		35,34,34,34,35,34,34,34,33,34,34,34,33,34,34,34  };

    char          *p1a;
    register char *p1, *p3;
    int           i, *xbuf;
    register int  r1, r2, j, *p2;

    ox = px;			/* copy parameters		 */
    oy = py;
    sx = qx;
    sy = qy;
    abm = (char *) malloc (sx * sy * sizeof (*abm));
				/* allocate space		 */
    xbuf = (int *) malloc ((sx - 2) * sizeof (*xbuf));
				/* x-buffer			 */

    p3 = abm;			/* get result pointer		 */

    p1 = &pcb[oy][ox];		/*** pre-scan 		       ***/
    p1a = &pcb[oy + 1][ox];
    p2 = xbuf;
    r1 = tab1[255 & *p1++] << 1;
    r1 |= tab1[255 & *p1++];
    r2 = tab1[255 & *p1a++] << 1;
    r2 |= tab1[255 & *p1a++];
    *p3++ = NOGOD | NOGOD << 4;
    *p3++ = NOGOD | NOGOD << 4;
    for (i = 2; i < sx; i++) {	/* this scans 2 lines		 */
	*p3++ = NOGOD | NOGOD << 4;
	r1 = (0x6db6 & r1 << 1) | tab1[255 & *p1++];
	r2 = (0x6db6 & r2 << 1) | tab1[255 & *p1a++];
	*p2++ = tab2[r1 & 511] << 1 | tab2[r2 & 511] | r2 & 0x2400;
    }

    for (i = 2; i < sy; i++) {	/*** main - scan	      ***/
	p1 = &pcb[i + oy][ox];
	p2 = xbuf;
	r1 = tab1[255 & *p1++] << 1;
	r1 |= tab1[255 & *p1++];
	*p3++ = NOGOD | NOGOD << 4;
	for (j = 2; j < sx; j++) {
	    r1 = (0x6db6 & r1 << 1) | tab1[255 & *p1++];
	    r2 = (0xd9b6 & (*p2) << 1) | tab2[r1 & 511] | r1 & 0x2400;
	    *p2++ = r2;
	    *p3++ = tab3[(0x12 & r2 >> 10) | tab2[r2 & 511]];
	}
	*p3++ = NOGOD | NOGOD << 4;
    }

    for (i = 0; i < sx; i++)	/*** post - scan	       ***/
	*p3++ = NOGOD | NOGOD << 4;

    free (xbuf);		/* release x - buffer		 */
}

set_dir ()			/* set up directions		 */
{
    dir [8] = dir[0] = 1;
    dir [9] = dir[1] = 1 + sx;
    dir [10] = dir[2] = sx;
    dir [11] = dir[3] = sx - 1;
    dir [12] = dir[4] = -1;
    dir [13] = dir[5] = -sx - 1;
    dir [14] = dir[6] = -sx;
    dir [15] = dir[7] = 1 - sx;
}

abm_tst (x, y)
    int x, y;
{
    int     i, j;
    char   *t;

    if (!abm || x < ox || y < oy || x >= ox + sx || y >= oy + sy)
	return;

    printf ("ABM - Test at x=%d y=%d\n", x, y);

    for (i = y + 3; i >= y - 3; --i) {
	printf ("\t%d:  ", i);
	for (j = x - 4; j <= x + 4; ++j) {
	    if (i >= oy && i < oy + sy && j >= ox && j < ox + sx) {
		t = abm + (i - oy) * sx + j - ox;
		printf (" %2x", *t & 255);
	    }
	    else
		printf (" ..");
	}
	printf ("\n");
    }
}
