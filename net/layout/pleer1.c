/*******************************************************\
* 						        *
* 	PCB program				        *
* 						        *
* 	Lee Router section   (part 1: full maze run)    *
* 						        *
* 	(c) 1985		A. Nowatzyk	        *
* 						        *
\*******************************************************/

#include "pparm.h"
#include "pcdst.h"
#include "pleer.h"

maze_run (xs, ys)		/* run through the maze		 */
    int xs, ys;
{
/*****************************************************************\
* 								  *
*  Note: positions and directions refere to the auxilary bitmap.  *
*        They operate on pointer (eficiency hack).		  *
* 								  *
\*****************************************************************/
    static int  htt1[8] = {	/* home token table (side 1)	 */
	HDT + 4, HDT + 5, HDT + 6, HDT + 7, HDT + 0, HDT + 1, HDT + 2, HDT + 3
    };
    static int  htt2[8] = {	/* home token table (side 2)	 */
	(HDT + 4) << 4, (HDT + 5) << 4, (HDT + 6) << 4, (HDT + 7) << 4,
	(HDT + 0) << 4, (HDT + 1) << 4, (HDT + 2) << 4, (HDT + 3) << 4
    };
    static int dtab[16] = {
	1, 0, 1, 1,   1, 1, 1, 1,   0, 0, 0, 0,   0, 0, 0, 0 };
    int cd;
    register int   i, j, k;
    register char  *t, *t1;
    register unsigned *rtp;
    unsigned ort;

#ifdef debug
    int x, y;
#endif

    do {			/* *** main loop - let us run  *** */

	for ((rtp = hvrt, i = 0); i < hvrtc;(rtp++, i++)) {
				/* let us go h/v		 */
	    if (!(*rtp))
		continue;	/* dead entry, will be removed later */
	    if (drtc >= drtmax) /* need more space		 */
		add_space (&drt, drtc, &drtmax);

	    t = abm + (*rtp >> 8);
	    j = *rtp & rtdr;

	    if (!(*rtp & rtsb)) {
		k = (*t & MSK1) ^ HDT;
		if (dtab[MSK1 & *(t + dir[j + 1])] && k != ((j + 5) & 7))
				/* spawn ray in +1 direction	 */
		    drt[drtc++] = *rtp & ~rtdr | (j + 1) & rtdr;
		if (dtab[MSK1 & *(t + dir[j + 7])] && k != ((j + 3) & 7))
				/* spawn ray in -1 direction	 */
		    drt[drtc++] = *rtp & ~rtdr | (j + 7) & rtdr;

		t += dir[j];
		*rtp += dir[j] << 8;
		if (k = MSK1 & *t) {/* ray hit something	 */
		    if (!(k & HDT) && (k != NOGOD) &&
			    trm_chk (xs, ys, t,
				     (j + 4) & 7, s1b, (*rtp & rtvc) >> 4))
			return (1);
		    if (k != HOLE) {
		        hvrtcd++;/* increase deleted count	 */
		        *rtp = 0;}/* terminate this ray		 */
		    else {
		       	*t = (*t & MSK2) | htt1[j];
#ifdef debug
			if (ttt) {
			    color (resb, resb);
			    y = (int)(t-abm);
			    x = ox + y % sx;
			    y = oy + y / sx;
			    point (x, y);
			}
#endif
		    }}
		else {
		    *t |= htt1[j];/* mark square		 */
#ifdef debug
		    if (ttt) {
			color (resb, resb);
			y = (int)(t-abm);
			x = ox + y % sx;
			y = oy + y / sx;
			 point (x, y);
		    }
#endif
		}}
	    else {
		k = HDT ^ (*t & MSK2) >> 4;
		if (dtab[MSK1 & *(t + dir[j + 1]) >> 4] && k != ((j + 5) & 7))
				/* spawn ray in +1 direction	 */
		    drt[drtc++] = *rtp & ~rtdr | (j + 1) & rtdr;
		if (dtab[MSK1 & *(t + dir[j + 7]) >> 4] && k != ((j + 3) & 7))
				/* spawn ray in -1 direction	 */
		    drt[drtc++] = *rtp & ~rtdr | (j + 7) & rtdr;

		t += dir[j];
		*rtp += dir[j] << 8;
		if (k = MSK2 & *t) {/* ray hit something	 */
		    if (!(k & HDT << 4) && (k != NOGOD << 4) &&
			    trm_chk (xs, ys, t,
				     (j + 4) & 7, s2b, (*rtp & rtvc) >> 4))
			return (1);
		    if (k != HOLE << 4) {
		        hvrtcd++;/* increase deleted count	 */
		        *rtp = 0;}/* terminate this ray		 */
		    else {
		       *t = (*t & MSK1) | htt2[j];
#ifdef debug
			if (ttt) {
			    color (resb, resb);
			    y = (int)(t-abm);
			    x = ox + y % sx;
			    y = oy + y / sx;
			    point (x, y);
			}
#endif
		    }}
		else {
		    *t |= htt2[j];/* mark square		 */
#ifdef debug
		    if (ttt) {
			color (resb, resb);
			y = (int)(t-abm);
			x = ox + y % sx;
			y = oy + y / sx;
			 point (x, y);
		    }
#endif
		}
	    }
	}


	cd = hvrtc;		/* save count for balance expand */

	for ((rtp = drt, i = 0); i < drtc;(rtp++, i++)) {
				/* * let us go diagonally	* */
	    if (!(*rtp))	/* dead entry, will be removed later */
		continue;
	    if (hvrtc >= hvrtmax)
		add_space (&hvrt, hvrtc, &hvrtmax);

	    j = *rtp & rtdr;
	    t = abm + (*rtp >> 8);

	    if (!(*rtp & rtsb)) {
		k = (*t & MSK1) ^ HDT;
		if (dtab[MSK1 & *(t + dir[j + 1])] && k != ((j + 5) & 7))
				/* spawn ray in +1 direction	 */
		    hvrt[hvrtc++] = *rtp & ~rtdr | (j + 1) & rtdr;
		if (dtab[MSK1 & *(t + dir[j + 7])] && k != ((j + 3) & 7))
				/* spawn ray in -1 direction	 */
		    hvrt[hvrtc++] = *rtp & ~rtdr | (j + 7) & rtdr;

		t += dir[j];
		*rtp += dir[j] << 8;
		if (k = MSK1 & *t) {/* ray hit something	 */
		    if (!(k & HDT) && (k != NOGOD) &&
			    trm_chk (xs, ys, t,
				     (j + 4) & 7, s1b, (*rtp & rtvc) >> 4))
			return (1);
		    if (k != HOLE) {
		        drtcd++;/* increase deleted count	 */
		        *rtp = 0;}/* terminate this ray		 */
		    else {
		        *t = (*t & MSK2) | htt1[j];
#ifdef debug
			if (ttt) {
			    color (resb, resb);
			    y = (int)(t-abm);
			    x = ox + y % sx;
			    y = oy + y / sx;
			    point (x, y);
			}
#endif
		    }}
		else {
		    *t |= htt1[j];/* mark square		 */
#ifdef debug
		    if (ttt) {
			color (resb, resb);
			y = (int)(t-abm);
			x = ox + y % sx;
			y = oy + y / sx;
			 point (x, y);
		    }
#endif
		}}
	    else {
		k = HDT ^ (*t & MSK2) >> 4;
		if (dtab[MSK1 & *(t + dir[j + 1]) >> 4] && k != ((j + 5) & 7))
				/* spawn ray in +1 direction	 */
		    hvrt[hvrtc++] = *rtp & ~rtdr | (j + 1) & rtdr;
		if (dtab[MSK1 & *(t + dir[j + 7]) >> 4] && k != ((j + 3) & 7))
				/* spawn ray in -1 direction	 */
		    hvrt[hvrtc++] = *rtp & ~rtdr | (j + 7) & rtdr;

		t += dir[j];
	        *rtp += dir[j] << 8;
		if (k = MSK2 & *t) {/* ray hit something	 */
		    if (!(k & HDT << 4) && (k != NOGOD << 4) &&
			    trm_chk (xs, ys, t,
				     (j + 4) & 7, s2b, (*rtp & rtvc) >> 4))
			return (1);
		    if (k != HOLE << 4) {
		        drtcd++;/* increase deleted count	 */
		        *rtp = 0;}/* terminate this ray		 */
		    else {
		 	*t = (*t & MSK1) | htt2[j];
#ifdef debug
			if (ttt) {
			    color (resb, resb);
			    y = (int)(t-abm);
			    x = ox + y % sx;
			    y = oy + y / sx;
			    point (x, y);
			}
#endif
		    }}
		else {
		    *t |= htt2[j];/* mark square		 */
#ifdef debug
		    if (ttt) {
			color (resb, resb);
			y = (int)(t-abm);
			x = ox + y % sx;
			y = oy + y / sx;
			 point (x, y);
		    }
#endif
		}
	    }
	}

	for (rtp = hvrt + (i = cd); i < hvrtc; (rtp++, i++)) {
				/* let us go h/v		 */
	    if (!(*rtp))
		continue;	/* dead entry, will be removed later */
	    if (drtc >= drtmax)
		add_space (&drt, drtc, &drtmax);

	    t1 = t = abm + (*rtp >> 8);
	    ort = *rtp;
	    j = *rtp & rtdr;

	    if (!(*rtp & rtsb)) {
		t += dir[j];
		*rtp += dir[j] << 8;
		if (k = MSK1 & *t) {/* ray hit something	 */
		    if (!(k & HDT) && (k != NOGOD) &&
			    trm_chk (xs, ys, t,
				     (j + 4) & 7, s1b, (*rtp & rtvc) >> 4))
			return (1);
		    if (k != HOLE) {
		        hvrtcd++;/* increase deleted count	 */
		        *rtp = 0;}/* terminate this ray		 */
		    else {
		       *t = (*t & MSK2) | htt1[j];
#ifdef debug
			if (ttt) {
			    color (resb, resb);
			    y = (int)(t-abm);
			    x = ox + y % sx;
			    y = oy + y / sx;
			    point (x, y);
			}
#endif
		    }}
		else {
		    k = (*t1 & MSK1) ^ HDT;
		    if (dtab[MSK1 & *(t1 + dir[j + 1])] && k != ((j + 5) & 7))
				/* spawn ray in +1 direction	 */
			drt[drtc++] = ort & ~rtdr | (j + 1) & rtdr;
		    if (dtab[MSK1 & *(t1 + dir[j + 7])] && k != ((j + 3) & 7))
				/* spawn ray in -1 direction	 */
			drt[drtc++] = ort & ~rtdr | (j + 7) & rtdr;

		    *t |= htt1[j];/* mark square		 */
#ifdef debug
		    if (ttt) {
			color (resb, resb);
			y = (int)(t-abm);
			x = ox + y % sx;
			y = oy + y / sx;
			 point (x, y);
		    }
#endif
		}}
	    else {
		t += dir[j];
		*rtp += dir[j] << 8;
		if (k = MSK2 & *t) {/* ray hit something	 */
		    if (!(k & HDT << 4) && (k != NOGOD << 4) &&
			    trm_chk (xs, ys, t,
				     (j + 4) & 7, s2b, (*rtp & rtvc) >> 4))
			return (1);
		    if (k != HOLE << 4) {
		       hvrtcd++;/* increase deleted count	 */
		       *rtp = 0;}/* terminate this ray		 */
		    else {
			*t = (*t & MSK1) | htt2[j];
#ifdef debug
			if (ttt) {
			    color (resb, resb);
			    y = (int)(t-abm);
			    x = ox + y % sx;
			    y = oy + y / sx;
			    point (x, y);
			}
#endif
		    }}
		else {
		    k = HDT ^ (*t1 & MSK2) >> 4;
		    if (dtab[MSK1 & *(t1 + dir[j + 1]) >> 4] && k != ((j + 5) & 7))
				/* spawn ray in +1 direction	 */
			drt[drtc++] = ort & ~rtdr | (j + 1) & rtdr;
		    if (dtab[MSK1 & *(t1 + dir[j + 7]) >> 4] && k != ((j + 3) & 7))
				/* spawn ray in -1 direction	 */
		        drt[drtc++] = ort & ~rtdr | (j + 7) & rtdr;

		    *t |= htt2[j];/* mark square		 */
#ifdef debug
		    if (ttt) {
			color (resb, resb);
			y = (int)(t-abm);
			x = ox + y % sx;
			y = oy + y / sx;
			 point (x, y);
		    }
#endif
		}
	    }
	}

	for (i = 0; i < vhtc; i++)
	    if (--vht[i].c <= 0) { /* via hole gets trough	 */
		j = (vht[i].s == s1b) ? HDT << 4 : HDT;
		for (k = 0; k < 8; k++)
		    if (j & *(vht[i].t + dir[k]))
			break;
		if (k >= 8) {	/* this is an interesting hole	 */
#ifdef debug
  if (ttt) {
		x = (int) (vht[i].t - abm);
		y = oy + x / sx;
		x = ox + x % sx;
		if (x >= tx1 && x <= tx2 && y >= ty1 && y <= ty2)
		    printf("Hole at %3d,%3d to side %d retrieved\n",
			x, y, vht[i].s ^ vec);
  }
#endif
		    for (j = 0; j < 4; j++) {/* set up ray table */
#ifdef dfirst
		    drt[drtc++] =  ((int)(vht[i].t - abm)) << 8 |
				   (vht[i].n - 1) << 4 |
				   ((vht[i].s == s1b) ? rtsb : 0) |
				   (j + j + 1);
#else
		    hvrt[hvrtc++]= ((int)(vht[i].t - abm)) << 8 |
				   (vht[i].n - 1) << 4 |
				   ((vht[i].s == s1b) ? rtsb : 0) |
				   (j + j);
#endif
		    }
		}
		for (j = i + 1; j < vhtc; j++) {
		    vht[j - 1].t = vht[j].t;
		    vht[j - 1].s = vht[j].s;
		    vht[j - 1].n = vht[j].n;
		    vht[j - 1].c = vht[j].c;
	    	}
	        vhtc--;
	    }
    
	if (drtc < drtcd * 4) {	/* recover space if more than 25% lost */
	    for (j = i = 0; i < drtc; i++)
		if (drt[i]) 
		    drt[j++] = drt[i];
	    drtc = j;
	    drtcd = 0;		/* reset deleted count		 */
	}

	if (hvrtc < hvrtcd * 4) {/* recover space if more than 25% lost */
	    for (j = i = 0; i < hvrtc; i++)
		if (hvrt[i])
		    hvrt[j++] = hvrt[i];
	    hvrtc = j;
	    hvrtcd = 0;		/* reset deleted count		 */
	}
/*getcur(&x,&y);*/
/*tststr("after cleanup");*/
    } while ((drtc - drtcd + hvrtc - hvrtcd) || vhtc);
				/* until all rays died		 */

    return (0);			/* no connection found		 */
}

#ifdef debug
tststr(s)	/* test structure */
 char *s;
{
    int     i, j, k, x, y, y_key ();
    unsigned t;
k=0;
    qsort (drt, drtc, sizeof (*drt), y_key);
    qsort (hvrt, hvrtc, sizeof (*hvrt), y_key);

    for (i = 0; i < drtc; i++) {
	t = drt[i];
	for (j = i + 1; j < drtc; j++)
	    if (t != drt[j])
		break;
	j--;

	if (j > i) {
	    if (t) {
		if(!k) {    printf ("structure test - (%8x) %s\n",V.cnet, s);
k=1;}
		printf ("D: x/y= %3d,%3d s=%d d=%d  --  %3d times\n",
			ox + (t >> 8) % sx, oy + (t >> 8) / sx,
			(t & rtsb) ? s2b : s1b, t & rtdr, j - i +1);
	    }
if(t && j - i > 5) {
 printf("have fun\n");
 x=0;
}
	    i = j + 1;
	}
    }

    for (i = 0; i < hvrtc; i++) {
	t = hvrt[i];
	for (j = i + 1; j < hvrtc; j++)
	    if (t != hvrt[j])
		break;
	j--;

	if (j > i) {
	    if (t) {
		if(!k) {    printf ("structure test - (%8x) %s\n",V.cnet, s);
k=1;}
		printf ("HV:x/y= %3d,%3d s=%d d=%d  --  %3d times\n",
			ox + (t >> 8) % sx, oy + (t >> 8) / sx,
			(t & rtsb) ? s2b : s1b, t & rtdr, j - i+1);
	    }
	    i = j + 1;
	}
    }
}

y_key (a, b)
    unsigned *a, *b;
{
    return ((*a > *b) - (*a < *b));
}

#endif

add_space (p, n, max)		/* increase table space		 */
    int n, *max;
    unsigned **p;
{
    int     i;
    unsigned *t, *t1, *t2;

    i = *max + *max / 2;	/* add 50%			 */
    t1 = t = (unsigned *) malloc ((i + sftmrg) * sizeof (*t));
    *max = i;
    for ((t2 = *p, i = 0); i < n; i++)	/* copy stuff		 */
	*(t1++) = *(t2++);
    free (*p);			/* release old space		 */
    *p = t;
}

trm_chk (xs, ys, t, d, sd, vc)	/* termination check		 */
    int xs, ys, d, sd, vc;
    char *t;
/****************************************************************************\
* 									     *
*  Termination check: the maze-runner encountered an unusual object.	     *
*  such objects are:							     *
*    1. TERMs : a path was found (but there might be an s1b/s2b exception).  *
*    2. VTERMs: like 1, but a via-hole is necessary.			     *
*    3. HOLE  : the maze spawns to the other side.			     *
* 									     *
\****************************************************************************/
{
    int i, j, x, y, vt;
    int sx1, sy1, ox1, oy1, s1, d1;
    char *t1, *abm1;

    vt = 0;			/* no via hole default		 */

    j = (sd == s1b) ? *t & MSK1 : (*t & MSK2) >> 4;
    switch (j) {
	case VTERM: 
	case VTERM | HOLE: 

	    if (!C3)
		return 0;	/* allways fails if no via-holes */

	    i = (int) (t - abm);/* get real koordinates		 */
	    y = oy + i / sx;
	    x = ox + i % sx;

	    if ((i = (x | y) & 1)) {/* need alignment		 */
		if (x & 1) {	/* x is of grid			 */
		    if (((d + 1) & 7) < 4)
			x++;
		    else
			x--;
		}
		if (y & 1) {	/* y is of grid			 */
		    if (((d + 7) & 7) < 4)
			y++;
		    else
			y--;
		}
	    }

	    color (ishb | selb, ishb | selb);
	    if (pin (x, y))
		return (0);	/* pin allocation failed	 */

	    if (i) {		/* of wire pin, due to alignment */
		color (vec, vec);
		pxl (x, y);
	    }
	    vt = 1;		/* don't forget to remove grabage*/


	case TERM: 
	case TERM | HOLE: 
	    mz_done (t, d, sd);/* start at <xd,yd>		 */

	    get_vias (t, d, sd);
	    for (i = 0; i < vhptc; i++) {
		t1 = vhpt[i];
		j = (int) (t1 - abm);
		x = ox + j % sx;
		y = oy + j / sx;
		s1 = ((*t1 & MSK1) == HOLE) ? s2b : s1b;
		d1 = 7 & ((s1 == s1b) ? *t1 : *t1 >> 4);
		color (ishb | selb, ishb | selb);
		if (pin (x, y)) {
		    t1 = vhpt[i - 1];
		    d1 = 7 & ((s1 == s2b) ? *t1 : *t1 >>4);
		    if (i && fnc_tnnl (t1, d1, s1))
			mz_done (t1, d1, s1);	/* not a problem	*/
		    else {
			mz_undone (t, d, sd);
			return (0);	/* via-hole interaction problem */
		    }}
		else {
		    if ((*t1 & MSK1) == MSTART ||
			(*t1 & MSK2) == MSTART << 4)
			break;	/* hole on start wire exception	 */
		    if (mz_chk (t1, d1, s1)) {
				/* trouble			 */
			abm1 = abm;/* save aux bit map		 */
			sx1 = sx;
			sy1 = sy;
			ox1 = ox;
			oy1 = oy;
			j = s_route (xs, ys, x, y);
			abm = abm1;/* restore bitmap		 */
			sx = sx1;
			sy = sy1;
			ox = ox1;
			oy = oy1;
			set_dir ();
			if (!j)	/* recovery failed		 */
			    mz_undone (t, d, sd);
			return (j);
		    }
		    mz_done (t1, d1, s1);
		}
	    }

	    if (vt) 		/* (TERM-) via hole needs care	 */
		ckgp (x, y, sd);
	    return (1);		/* that's it			 */


	case HOLE: 		/* prepare for via - hole	 */
	    if (vhtc >= maxvht)
		err ("trm_chk: Via hole table too small - increase 'maxvht'",
		     vhtc, maxvht, 0, 0);
	    if (vc > 0 && *t == (HOLE | HOLE << 4)) {
		vht[vhtc].t = t;
		vht[vhtc].s = sd;
		vht[vhtc].c = ((d & 1) ? C5 : C4) + (C3 - vc) * C7;
		vht[vhtc++].n = vc;
#ifdef debug
   if (ttt) {
		x = (int) (t - abm);
		y = oy + x / sx;
		x = ox + x % sx;
		if (x >= tx1 && x <= tx2 && y >= ty1 && y <= ty2)
		    printf("Hole at %3d,%3d to side %d with delay=%d vc=%d entered\n",
			x, y, sd ^ vec, vht[vhtc - 1].c, vc);
   }
#endif
	    }
	    return (0);

	case MSTART:
	    return (0);		/* ignore start token		 */

	default:
	    err ("trm_chk: Unknown token", j, *t, sd, 0);
    }

    return 0;			/* to please lint		 */
}

fnc_tnnl (t, d, s)		/* fence tunnel			 */
    int d, s;
    char *t;
/************************************************************************\
* 									 *
*  This routine is an exception handler to recover from problems	 *
*  caused by fences: 2 via-holes (which are redundant) are placed	 *
*  too close to eachother. The hole that was already inserted will	 *
*  be removed and the 'wrong' side will be used for the wire.		 *
* 									 *
*  A 1 is returned if the hole was caused by the fence and is therefore	 *
*  redundant. A 0 indicates that a real hole was needed.		 *
* 									 *
\************************************************************************/
{
    int     i, j, x, y, x1, y1, d1;
    char    *t1;

    i = (int) (t - abm);	/* get real koordinates		 */
    y1 = y = oy + i / sx;
    x1 = x = ox + i % sx;
    t1 = t;
    d1 = d;
    j = (s == s2b) ? 0 : 4;

    do {			/* feasability test		 */
	x1 += dr[d1][0];
	y1 += dr[d1][1];
	t1 += dir[d1];

	if (HOLE == (i = (*t1) >> j & MSK1)) 
	    break;		/* exit on offending hole	 */

	if (pcb[y1][x1] & s) 	/* via is not redundant !	 */
	    return (0);

	d1 = i & 7;
    } while (i & HDT);

    color (0, ishb | selb);	/* ready to go !		 */
    dpin (x, y);
    color (0 , (s ^ vec) | selb);
    pxl (x, y);			/* inefficient, but seldom	 */

    do {			/* Loop back			 */

	*t = (s == s2b) ? (*t & MSK1) | (HDT | d) << 4  :
			  (*t & MSK2) | (HDT | d);

	x += dr[d][0];
	y += dr[d][1];
	t += dir[d];

	pxl (x, y);

	if (HOLE == (i = (*t) >> j & MSK1)) 
	    return (1);		/* exit on offending hole	 */

	d = i & 7;
    } while (i & HDT);
    err ("fnc_tnnl: illegal token", x, y, i, (int) (t - abm));

    return 0;			/* to please lint		 */
}

mz_done (t, d, s)		/* maze done - backtrace	 */
    int d, s;
    char *t;
{
    int     i, j, x, y, n, xl, yl, dl;

    i = (int) (t - abm);	/* get real koordinates		 */
    yl = y = oy + i / sx;
    xl = x = ox + i % sx;
    dl = d;

    color (s | selb, s | selb);

    j = (s == s1b) ? 0 : 4;

    do {			/* Loop back			 */
	x += dr[d][0];
	y += dr[d][1];
	t += dir[d];

	n = 0;
	if (d != dl) {
	     plt (xl, yl, x - dr[d][0], y - dr[d][1]);
	     xl = x;
	     yl = y;
	     dl = d;
	     n = 1;
	}

	if (MSTART == (i = (*t) >> j & MSK1) || i == HOLE) {
	    if (n)
		pxl (x, y);
	    else
		plt (xl, yl, x, y);
	    return;		/* exit on start token		 */
	}
	d = i & 7;
    } while (i & HDT);
    err ("mz_done: illegal token", x, y, i, (int) (t - abm));
}

mz_chk (t, d, s)		/* maze done - backtrace	 */
    int d, s;
    char *t;
/*******************************************************************\
* 								    *
*  Maze check: The result path is check for selected s1b and s2b    *
*  pixels on not-hole squares. a '1' is returned if this violation  *
*  is detected, a '0' is returned for a good path. Note: this is    *
*  just an other hack due to the lack of 2 marker/select bits.	    *
* 								    *
\*******************************************************************/
{
    int     i, j, x, y, p, c;

    i = (int) (t - abm);	/* get real koordinates		 */
    y = oy + i / sx;
    x = ox + i % sx;

    j = (s == s1b) ? 0 : 4;
    c = selb | (vec ^ s);

    p = pcb[y][x];
    if (!(p & ahb) && !(~p & c))
	return (1);		/* error			 */

    do {			/* Loop back			 */
	x += dr[d][0];
	y += dr[d][1];
	t += dir[d];

	p = pcb[y][x];
	if (!(p & ahb) && !(~p & c))
	    return (1);		/* error			 */

	if (MSTART == (i = (*t) >> j & MSK1) || i == HOLE)
	    return (0);		/* success			 */

	d = i & 7;
    } while (i & HDT);
    err ("mz_chk: illegal token", x, y, i, (int) (t - abm));

    return 0;			/* to plese lint		 */
}

mz_undone (t, d, s)		/* maze un-done - backtrace	 */
/**********************************************************************\
* 								       *
*  This routine un-does the effect of a mz_done call which may become  *
*  necessary to un-do long traces in L*_routes. The main problem is    *
*  the lack of bit-planes: 2 mark and 2 select bits are necessary to   *
*  avoid these hacks.						       *
* 								       *
\**********************************************************************/
    int d, s;
    char *t;
{
    int     i, j, x, y, n, xl, yl, dl, xs, ys;

    i = (int) (t - abm);	/* get real koordinates		 */
    ys = yl = y = oy + i / sx;
    xs = xl = x = ox + i % sx;
    dl = d;


    color (0, s | selb);

    j = (s == s1b) ? 0 : 4;

    do {			/* Loop back			 */
	x += dr[d][0];
	y += dr[d][1];
	t += dir[d];

	n = 1;
	if (d != dl) {
	     plt (xl, yl, x - dr[d][0], y - dr[d][1]);
	     xl = x;
	     yl = y;
	     dl = d;
	     n = 0;
	}

	i = *t >> j & MSK1;

	if (i == HOLE) {	/* time to switch sides		 */
	    j ^= 4;
	    i = *t >> j & MSK1;
	    plt (xl, yl, x, y);
	    xl = x;
	    yl = y;
	    dl = i & 7;
	    if (i != MSTART) {
		color (0, ishb | selb);
		dpin (x, y);	/* remove via hole		 */
		s ^= vec;	/* change color			 */
		color (0, s | selb);
	    }
	}

	if (MSTART == i) {
	    if (n)
		plt (xl, yl, x - dr[d][0], y - dr[d][1]);
	    i = 8;		/* assume safe to do ck_rdnb	 */
	    if (pcb[y][x] & ahb)
		for (i = 0; i < 8; i++)
		    if (pcb[y + dr[i][1]][x + dr[i][0]] & s)
			break;
	    if (i >= 8)		/* don't remove center of holes	 */
	    	ck_rdnb (x, y, s);
	    if (pcb[y][x] & ahb) {
		color (selb, selb);
		dpin (x, y);
	    }

	    if (pcb[ys][xs] & ahb) {
		color (selb, selb);
		dpin (xs, ys);
	    }
	    return;		/* exit on start token		 */
	}

	d = i & 7;
    } while (i & HDT);
    err ("mz_undone: illegal token", x, y, i, (int) (t - abm));
}

get_vias (t, d, s)		/* get list of via holes	 */
    int d, s;
    char *t;
/***********************************************************************\
* 								        *
*  The list of via holes is necessary to check for s1b-s2b problems,    *
*  which has to be done for each segment after the previous was added.  *
* 								        *
\***********************************************************************/
{
    int     i, j;

    vhptc = 0;			/* reset via hole counter	 */

    j = (s == s1b) ? 0 : 4;

    do {			/* Loop back			 */
	t += dir[d];

	i = *t >> j & MSK1;

	if (i == HOLE) {	/* time to switch sides		 */
	   j ^= 4;
	   i = *t >> j & MSK1;
	   vhpt[vhptc++] = t;
	}

	if (MSTART == i)
	    return;		/* done				 */

	d = i & 7;
    } while (i & HDT);
    err ("get_vias: illegal token", i, (int) (t - abm), 0, 0);
}

S_route (xs, ys, xd, yd)		/* full maze - run		 */
    int xs, ys, xd, yd;
/**************************************************************************\
* 									   *
*  This is a allmost unrestricted version of a maze-router. The routing	   *
*  area is confined to a rectangle that is C1 larger than the one defined  *
*  by <xs,ys> and <xd,yd>. Runing will start at <xs,ys>.		   *
*  This routind runs faster if <xd,yd> points to the larger object.	   *
*  s_route returns successfully if <xs,ys> is connected to an other	   *
*  part of its net. This need not be a connection to <xd,yd>, thus	   *
*  a successfull return does *not* guarantee a connection between	   *
*  <xs,ys> and <xd,yd>.							   *
* 									   *
\**************************************************************************/
{
    int     mz_vf (), mz_hf ();
    char   *t;
    int     i;
#ifdef debug
    int x, y;
#endif

    cr_gmaze (xs, ys, xd, yd, C1);/* create maze		 */

#ifdef debug
  if (ttt) {
    color (resb, resb);
    plts (ox, oy, ox+sx-1, oy);
    plts (ox+sx-1, oy, ox+sx-1, oy+sy-1);
    plts (ox+sx-1, oy+sy-1, ox, oy+sy-1);
    plts (ox, oy+sy-1, ox, oy);
  }
#endif

    fence (xs, ys, xd, yd);	/* insert fences		 */
    mz_hole ();			/* insert via hole candidates	 */

#ifdef debug
  if (ttt) {
    err_msg ("llc of test area");
    getcur(&tx1,&ty1);
    err_msg ("urc of test area");
    getcur(&tx2,&ty2);
    err_msg ("abort?");
    if (getcur (&x, &y) > 3) {	/* abort			 */
	free (abm);
	abm = 0;
	return (0);
    }
  }
#endif

    set_dir ();			/* prepare maze run		 */
    hvrt = (unsigned *) malloc ((maxrtl + sftmrg) * sizeof (*hvrt));
    drt = (unsigned *) malloc ((maxrtl + sftmrg) * sizeof (*drt));
    drtmax = hvrtmax = maxrtl;
    vhtc = drtc = drtcd = hvrtcd = hvrtc = 0;/* reset table counter */

    wtvf = mz_vf;		/* mark start			 */
    wthf = mz_hf;
    i = pcb[ys][xs];
    if (i & ahb)
	wtrace (xs, ys, vec);
    else if (i & selb) {
	if (i & s1b && !(i & resb))
	    wtrace (xs, ys, s1b);
	else if (i & s2b && i & resb)
	    wtrace (xs, ys, s2b);
	else {
	    free (abm);
	    return 0;		/* somthing is wrong		 */
	}
    }	    

    t = abm + (yd - oy) * sx + xd - ox;
    i = (*t & MSK1) == MSTART || (*t & MSK2) == MSTART << 4;

    if (!i)
	i = maze_run (xs, ys);	/* find the path		 */

#ifdef debug
    if (ttt && !i) {
	do {
	    err_msg ("select point");
	    getcur (&x, &y);
	    if (x > ox && y > oy && x < ox + sx - 1 && y < oy + sy - 1) {
		t = abm + (y - oy) * sx + x - ox;
		if (*t & HDT) {
		    get_vias (t, *t & 7, s1b);
		    printf ("S1-path: Path has %d via holes\n", vhptc);}
		if (*t & HDT << 4) {
		    get_vias (t, (*t >> 4) & 7, s2b);
		    printf ("S2-path: Path has %d via holes\n", vhptc);}
		if (!(*t & (HDT | HDT << 4)))
		    printf ("No path\n");}
	    else
		break;
	} while (1);
    }
#endif

    free (drt);			/* release memory		 */
    free (hvrt);
    free (abm);
    abm = 0;

    return (i);
}
RE_route (src, dst, xl, yl, xh, yh, sd)
    int xl, yl, xh, yh, sd;
    char *src, *dst;
/********************************************************************\
* 								     *
*   Re_route is similar to S_route, but is used to straight a wire   *
*   (see 'straight' in pplow.c): an existing trace is erased and     *
*   routed again. For this reason, the maze-size is precisely known  *
*   and no via-holes are necessary or desirable.		     *
* 								     *
\********************************************************************/
{
    int     mz_vf (), mz_hf ();
    char   *t;
    register int i, j, xs, ys, xd, yd;
    int sv_C3 = C3;

    C3 = 0;			/* no via - holes		 */

    xs = ((int) src - (int) pcb) % xmax;      /* long live ptr-s */
    ys = ((int) src - (int) pcb) / xmax;
    xd = ((int) dst - (int) pcb) % xmax;
    yd = ((int) dst - (int) pcb) / xmax;

    cr_maze (xl, yl, xh - xl + 1, yh - yl + 1);	/* create maze	 */

    set_dir ();			/* prepare maze run		 */
    hvrt = (unsigned *) malloc ((maxrtl + sftmrg) * sizeof (*hvrt));
    drt = (unsigned *) malloc ((maxrtl + sftmrg) * sizeof (*drt));
    drtmax = hvrtmax = maxrtl;
    vhtc = drtc = drtcd = hvrtcd = hvrtc = 0;/* reset table counter */

    wtvf = mz_vf;		/* mark start			 */
    wthf = mz_hf;
    wtrace (xs, ys, sd);

    j = (sd == s1b) ? 0 : rtsb;
    for (i = 0; i < hvrtc; i++)
	if ((hvrt[i] & rtsb) != j)
	    hvrt[i] = 0;	/* kill rays for wrong side	 */
    for (i = 0; i < drtc; i++)
	if ((drt[i] & rtsb) != j)
	    drt[i] = 0;		/* kill rays for wrong side	 */

    t = abm + (yd - oy) * sx + xd - ox;
    i = (*t & MSK1) == MSTART || (*t & MSK2) == MSTART << 4;

    if (!i)
	i = maze_run (xs, ys);	/* find the path		 */

    if (!i)
	err ("RE_route: there should be a path ??", xs, ys, xd, yd);

    free (drt);			/* release memory		 */
    free (hvrt);
    free (abm);
    abm = 0;
    C3 = sv_C3;			/* restore C3			 */
}

fence (x1, y1, x2, y2)		/* insert fences		 */
    int x1, y1, x2, y2;
{
    int     i, j, k;
    char *t;
#ifdef debug
    int xx, yy;
#endif

    if (x1 > x2) {		/* insure x1 <= x2		 */
	i = x1;
	x1 = x2;
	x2 = i;
    }
    if (y1 > y2) {		/* insure y2 <= y2		 */
	i = y1;
	y1 = y2;
	y2 = i;
    }

    k = sx / 2;

    x1 = (((x1 - C2 / 2) | 1) % C2) - (ox % C2);
    for (i = (x1 <= 0) ? x1 + C2 : x1; i < k; i += C2) {
	t = abm + i + sx;	/* insert y - fences		 */
#ifdef debug
  if (ttt) {
	yy = (int)(t - abm);
	xx = ox + yy % sx;
	yy = oy + yy / sx;
	color (mrkb, mrkb);
	plts (xx, yy, xx, yy + sy - 2);
  }
#endif
	for (j = 2; j < sy; (t += sx, j++))
	    if ((*t & MSK2) != TERM << 4)
		*t = (*t & MSK1) | NOGOD << 4;
    }

    x2 = (((x2 - 1 - C2 / 2) | 1) % C2) - ((ox + k) % C2);
    for (i = k + ((x2 < 0) ? x2 + C2 : x2); i < sx - 1; i += C2) {
	t = abm + i + sx;	/* insert y - fences		 */
#ifdef debug
  if (ttt) {
	yy = (int)(t - abm);
	xx = ox + yy % sx;
	yy = oy + yy / sx;
	color (mrkb, mrkb);
	plts (xx, yy, xx, yy + sy - 2);
  }
#endif
	for (j = 2; j < sy; (t += sx, j++))
	    if ((*t & MSK2) != TERM << 4)
		*t = (*t & MSK1) | NOGOD << 4;
    }

    k = sy / 2;

    y1 = (((y1 - C2 / 2) | 1) % C2) - (oy % C2);
    for (i = (y1 <= 0) ? y1 + C2 : y1; i < k; i += C2) {
	t = abm + i * sx + 1;	/* insert x - fences		 */
#ifdef debug
  if (ttt) {
	yy = (int)(t - abm);
	xx = ox + yy % sx;
	yy = oy + yy / sx;
	color (mrkb, mrkb);
	plts (xx, yy, xx + sx - 2, yy);
  }
#endif
	for (j = 2; j < sx; (t++, j++))
	    if ((*t & MSK1) != TERM)
		*t = (*t & MSK2) | NOGOD;
    }

    y2 = (((y2 - 1 - C2 / 2) | 1) % C2) - ((oy + k) % C2);
    for (i = k + ((y2 < 0) ? y2 + C2 : y2); i < sy - 1; i += C2) {
	t = abm + i * sx + 1;	/* insert x - fences		 */
#ifdef debug
   if (ttt) {
	yy = (int)(t - abm);
	xx = ox + yy % sx;
	yy = oy + yy / sx;
	color (mrkb, mrkb);
	plts (xx, yy, xx + sx - 2, yy);
   }
#endif
	for (j = 2; j < sx; (t++, j++))
	    if ((*t & MSK1) != TERM)
		*t = (*t & MSK2) | NOGOD;
    }
}

mz_hole()			/* insert via-hole candiates	 */
{
    int     x, y;
    char *t;

    for (y = oy + C6 - oy % C6; y < oy + sy - 1; y += C6)	
	for ((x = ox + C6 - ox % C6, t = abm + x - ox + (y - oy) * sx);
	     x < ox + sx - 1; (t += C6, x += C6))
	    if (((*t & MSK1) != NOGOD) && ((*t & MSK2) != NOGOD << 4) &&
		ck_pin (x, y)) {
#ifdef debug
  if (ttt) {
		color (mrkb, mrkb);
		point (x, y);
  }
#endif
		*t |= HOLE | (HOLE << 4);
	    }
}

mz_hf (x, y)			/* maze-run hole function	 */
    int x, y;
{
    register int i, j;
    register char   *t;

    if (x <= ox || y <= oy || x >= ox + sx - 1 || y >= oy + sy - 1)
	return (0);		/* outside of maze		 */

    t = abm + (x - ox + (y - oy) * sx);/* get start pointer	 */
    *t = MSTART | MSTART << 4;	/* mark start			 */
    for (i = 0; i < 4; i++) {	/* set up ray table		 */
#ifdef dfirst
	drt[drtc++] = j = ((int) (t - abm)) << 8 |
		    C3 << 4 |
		    i + i + 1;
	drt[drtc++] = j | rtsb;
#else
	hvrt[hvrtc++] = j = ((int) (t - abm)) << 8 |
		      C3 << 4 |
		      i + i;
	hvrt[hvrtc++] = j | rtsb;
#endif
    }
    return (0);
}

mz_vf (x1, y1, x2, y2)		/* maze-run set up vector 	 */
    int x1, y1, x2, y2;
{   int i, j, k, d, xl, yl, xh, yh;
    char *t;
    static int ct[9] = {5, 4, 3, 6, 8, 2, 7, 0, 1};

    xl = ox + 1;		/* active area of aux bit map	 */
    yl = oy + 1;
    xh = ox + sx - 2;
    yh = oy + sy - 2;

    if (y1 > y2) {		/* order y for y-band clipping	 */
	i = x1;
	x1 = x2;
	x2 = i;
	i = y1;
	y1 = y2;
	y2 = i;
    }
    i = ((x1 < x2) - (x2 < x1));

    if (y1 < yl) {
	if (y2 < yl)
	    return;
	x1 += (yl - y1) * i;
	y1 = yl;
    };

    if (y2 > yh) {
	if (y1 > yh)
	    return;
	x2 -= (y2 - yh) * i;
	y2 = yh;
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

    if (x1 < xl) {
	if (x2 < xl)
	    return;
	y1 += (xl - x1) * i;
	x1 = xl;
    }

    if (x2 > xh) {
	if (x1 > xh)
	    return;
	y2 -= (x2 - xh) * i;
	x2 = xh;
    }

    d = ct[((x1 < x2) - (x1 > x2) + 1) * 3 + (y1 < y2) - (y1 > y2) + 1];
    j = abs (x1 - x2) | abs (y1 - y2);/* length of vector	 */
    t = abm + (x1 - ox + (y1 - oy) * sx);/* get start pointer	 */

    if (wtsb == s1b)
	for (i = 0; i <= j; i++) {
	    if (!(pcb[y1][x1] & ahb)) {/* skip hole area	 */
		if (C3 && (*t & MSK2) == (HOLE | VTERM) << 4) {
		    *t = MSTART | HOLE << 4; 
		    for (k = 0; k < 4; k++) {
#ifdef dfirst
			drt[drtc++] = ((int) (t - abm)) << 8 |
				      (C3 - 1) << 4 |
				      rtsb |
				      k + k + 1;
#else
			hvrt[hvrtc++] = ((int) (t - abm)) << 8 |
				        (C3 - 1) << 4 |
				        rtsb |
				        k + k;
#endif
		    }}
		else if ((*t & MSK1) != MSTART)
		    *t = NOGOD << 4 | MSTART;/* mark start	 */
		if (d & 1) {	/* diagonal vector ?		 */
		    drt[drtc++] = ((int) (t - abm)) << 8 |
				  C3 << 4 |
				  (d + 2) & rtdr;
		    drt[drtc++] = ((int) (t - abm)) << 8 |
				  C3 << 4 |
				  (d + 6) & rtdr;
		}
		else {
		    hvrt[hvrtc++] = ((int) (t - abm)) << 8 |
				    C3 << 4 |
				    (d + 2) & rtdr;
		    hvrt[hvrtc++] = ((int) (t - abm)) << 8 |
				    C3 << 4 |
				    (d + 6) & rtdr;
		}
	    }
	    t += dir[d];
	    x1 += dr[d][0];
	    y1 += dr[d][1];
	}
    else
	for (i = 0; i <= j; i++) {
	    if (!(pcb[y1][x1] & ahb)) {/* skip hole area	 */
		if (C3 && (*t & MSK1) == (HOLE | VTERM)) {
		    *t = MSTART << 4 | HOLE;
		    for (k = 0; k < 4; k++) {
#ifdef dfirst
			drt[drtc++] = ((int) (t - abm)) << 8 |
				      (C3 - 1) << 4 |
				      k + k + 1;
#else
			hvrt[hvrtc++] = ((int) (t - abm)) << 8 |
				        (C3 - 1) << 4 |
				        k + k;
#endif
		    }}
		else if ((*t & MSK2) != MSTART << 4)
		    *t = NOGOD | MSTART << 4;/* mark start	 */
		if (d & 1) {	/* diagonal vector ?		 */
		    drt[drtc++] = ((int) (t - abm)) << 8 |
				  C3 << 4 |
				  rtsb |
				  (d + 2) & rtdr;
		    drt[drtc++] = ((int) (t - abm)) << 8 |
				  C3 << 4 |
				  rtsb |
				  (d + 6) & rtdr;
		}
		else {
		    hvrt[hvrtc++] = ((int) (t - abm)) << 8 |
				    C3 << 4 |
				    rtsb |
				    (d + 2) & rtdr;
		    hvrt[hvrtc++] = ((int) (t - abm)) << 8 |
				    C3 << 4 |
				    rtsb |
				    (d + 6) & rtdr;
		}
	    }
	    t += dir[d];
	    x1 += dr[d][0];
	    y1 += dr[d][1];
	}
}
