/***************************************************************\
*								*
*	PCB program						*
*								*
*	Utility routines					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include "pparm.h"
#include "pcdst.h"
#include <stdio.h>

err (s, e1, e2, e3, e4)			/* error exit			*/
    char s[];
    int e1, e2, e3, e4;
{
    printf ("Error: %s -- exited\n", s);
    strncpy (V.errtxt, s, 80);
    V.err_info[0] = e1;
    V.err_info[1] = e2;
    V.err_info[2] = e3;
    V.err_info[3] = e4;
    if (s[0] != '-') {
	save (1);		/* save state for recovery	 */
	printf ("Data structure saved on 'pcb.ERR'\n");
    };
    finish ();
}

beep()			/* beep on the terminal		 */
{
    if (no_beep)
	return;
    putc (7, stdout);
    fflush (stdout);
}

pin(x, y)			/* enter pin to pcb and AED	 */
    int x, y;
{
    int     i, j, k;
    static int hx[8] = {3, 3, 0,-3,-3,-3, 0, 3};
    static int hy[8] = {0, 3, 3, 3, 0,-3,-3,-3};
    static int bx[12] = {5, 5, 5, 1, 0,-1,-5,-5,-5, 1, 0,-1};
    static int by[12] = {1, 0,-1, 5, 5, 5, 1, 0,-1,-5,-5,-5};
    static int xt[12] = {3, 3, 3, 1, 0,-1,-3,-3,-3, 1, 0,-1};
    static int yt[12] = {1, 0,-1, 3, 3, 3, 1, 0,-1,-3,-3,-3};

    for (i = x - 2; i <= x + 2; ++i)
	for (j = y - 2; j <= y + 2; ++j) {
	    k = pcb[j][i];
	    if ((k & (fb | chb | ishb)) || (!(k & selb) && k & vec) ||
		!(vec & ~k))
		return 1;	/* overlap error		 */
	}

    for (i = 0; i < 8; ++i)	/* special hole distance	 */
	if (pcb[y+hy[i]][x+hx[i]] & (ahb | fb))
	    return (1);		/* too close to hole		 */

    for (i = 0; i < 12; ++i)	/* check for small passages	 */
	if ((pcb[y+by[i]][x+bx[i]] & ahb) &&
	    ((pcb[y+yt[i]][x+xt[i]] & vec) && !(pcb[y+yt[i]][x+xt[i]] & selb)))
	    return (1);

    for (i = x - 1; i <= x + 1; ++i)
	for (j = y - 1; j <= y + 1; ++j)
	    pcb[j][i] = ccc | (ccm & pcb[j][i]);

    if (!batch)
	dot (x, y);		/* update AED 			 */

    for (i = 0; i < 12; ++i)	/* block small passages		 */
	if (pcb[y+by[i]][x+bx[i]] & ahb)
	    pxl (x + xt[i], y + yt[i]);

    return (0);
}

dot (x, y)			/* plot a 3 by 3 rectangle on AED*/

    int  x, y;
{
    int x1, y1, x2, y2;

    x1 = (x - 1 > fx) ? x - 1 : fx;
    x2 = (x < fx + 510) ? x + 1 : fx + 511;
    y1 = (y - 1 > fy) ? y - 1 : fy;
    y2 = (y < fy + 510) ? y + 1 : fy + 511;
    if ((x1 <= x2) && (y1 <= y2))
	rect (x1, y1, x2, y2);
}

dpin (x, y)		/* dot on pcb and AED (delete pin)	 */
    int x, y;
{
    int     i, j, s, t;
    static int  xt[12] = {3, 3, 3, 1, 0, -1, -3, -3, -3, 1, 0, -1};
    static int  yt[12] = {1, 0, -1, 3, 3, 3, 1, 0, -1, -3, -3, -3};

    s = (~ccm & ahb) ? pcb[y][x] & selb : 0;  /* select prot ?	 */

    for (i = 0; i < 12; ++i)/* remove guard zones		 */
	if (pcb[y + yt[i]][x + xt[i]] & ahb) {
	    t = ccc | (ccm & pcb[y + yt[i]][x + xt[i]]);
	    pcb[y + yt[i]][x + xt[i]] = (s && t & vec) ? t | s : t;
	    if (!batch)
		point (x + xt[i], y + yt[i]);
	}

    for (i = x - 1; i <= x + 1; ++i)
	for (j = y - 1; j <= y + 1; ++j) {
	    t = ccc | (ccm & pcb[j][i]);
	    pcb[j][i] = (t & vec) ? t | s : t;
	}

    if (!batch)
	dot (x, y);		/* update AED 			 */

    return 0;	/* this 0 is used, even if lint does not think so  :-) */
}

ck_pin (x, y)			/* check possibe via hole position */
    int x, y;
/*******************************************************************\
* 								    *
*  A 1 is returned if it is possible to insert a via-hole at <x,y>  *
* 								    *
\*******************************************************************/
{
    int     i, j, k;
    static int hx[8] = {3, 3, 0,-3,-3,-3, 0, 3};
    static int hy[8] = {0, 3, 3, 3, 0,-3,-3,-3};
    static int bx[12] = {5, 5, 5, 1, 0,-1,-5,-5,-5, 1, 0,-1};
    static int by[12] = {1, 0,-1, 5, 5, 5, 1, 0,-1,-5,-5,-5};
    static int xt[12] = {3, 3, 3, 1, 0,-1,-3,-3,-3, 1, 0,-1};
    static int yt[12] = {1, 0,-1, 3, 3, 3, 1, 0,-1,-3,-3,-3};

    for (i = x - 2; i <= x + 2; ++i)
	for (j = y - 2; j <= y + 2; ++j) {
	    k = pcb[j][i];
	    if ((k & (fb | chb | ishb)) || ((k & vec) && !(k & selb)) ||
		!(vec & ~k))
		return 0; 	/* overlap error		 */
	}

    for (i = 0; i < 8; ++i)	/* special hole distance	 */
	if (pcb[y+hy[i]][x+hx[i]] & (ahb | fb))
	    return (0);		/* too close to hole		 */

    for (i = 0; i < 12; ++i)	/* check for small passages	 */
	if ((pcb[y+by[i]][x+bx[i]] & ahb) &&
	    ((pcb[y+yt[i]][x+xt[i]] & vec) && !(pcb[y+yt[i]][x+xt[i]] & selb)))
	    return (0);

    return (1);			/* good place for a hole	 */
}

plt(x1, y1, x2, y2)			/* Plot a line		*/
    int x1, y1, x2, y2;
{
    int     dx, dy, n;

    if ((x1 != x2) && (y1 != y2) && (abs (x1 - x2) != abs (y1 - y2)))
	err ("invalid slope", x1, y1, x2, y2);

    if (!batch)
	plts (x1, y1, x2, y2);

    dx = (x1 < x2) - (x2 < x1);
    dy = (y1 < y2) - (y2 < y1);

    n = abs (x1 - x2);
    if (!n)
	n = abs (y2 - y1);

    while (n >= 0) {
	pcb[y1][x1] = ccc | (ccm & pcb[y1][x1]);
	x1 += dx;
	y1 += dy;
	n--;
    };
};

plts (x1, y1, x2, y2)		/* plot a line on the AED	 */
    int x1, y1, x2, y2;
{
    int     t;

    if (batch)
	return;

    if (y1 > y2) {		/* order y for y-band clipping	 */
	t = x1;
	x1 = x2;
	x2 = t;
	t = y1;
	y1 = y2;
	y2 = t;
    };
    t = ((x1 < x2) - (x2 < x1));

    if (y1 < fy) {
	if (y2 < fy)
	    return;
	x1 += (fy - y1) * t;
	y1 = fy;
    };

    if (y2 > (fy + 511)) {
	if (y1 > (fy + 511))
	    return;
	x2 -= (y2 - (fy + 511)) * t;
	y2 = fy + 511;
    };

    if (x1 > x2) {		/* order x for x-band clipping	 */
	t = x1;
	x1 = x2;
	x2 = t;
	t = y1;
	y1 = y2;
	y2 = t;
    };
    t = (y1 < y2) - (y2 < y1);

    if (x1 < fx) {
	if (x2 < fx)
	    return;
	y1 += (fx - x1) * t;
	x1 = fx;
    };

    if (x2 > (fx + 511)) {
	if (x1 > (fx + 511))
	    return;
	y2 -= (x2 - (fx + 511)) * t;
	x2 = fx + 511;
    };

    if (x1 <= x2)
	plts1 (x1, y1, x2, y2);
    else
	plts1 (x2, y2, x1, y1);
}

plts1 (x1, y1, x2, y2)		/* plot line on aed		 */
    int x1, y1, x2, y2;
{
    int     t;

    if ((x1 & 0xfffffe00) != (x2 & 0xfffffe00)) {
	t = (y2 > y1) - (y2 < y1);
	x1 &= 511;
	plts1 (0, y1 + (512 - x1) * t, x2 & 511, y2);
	x2 = 511;
	y2 = y1 + (511 - x1) * t;
    };

    if (y2 < y1) {	/* this could be optimized with more thinking */
	t = y1;
	y1 = y2;
	y2 = t;
	t = x1;
	x1 = x2;
	x2 = t;
    };

    if ((y1 & 0xfffffe00) != (y2 & 0xfffffe00)) {
	t = (x2 > x1) - (x2 < x1);
	y1 &= 511;
	plts1 (x1 + (512 - y1) * t, 0, x2, y2 & 511);
	y2 = 511;
	x2 = x1 + (511 - y1) * t;
    };

    move (x1, y1);
    draw (x2, y2);
}

menu (s, n)			/* menu function select		 */
    char *s[];
    int n;
/*********************************************************************\
* 								      *
* The <n> strings will be displayed on the screen. If the cursor      *
* is clicked (any key) on a string, the coresponding number <0..n-1>  *
* is returned. Otherwise, a -1 will be returned.		      *
* 								      *
\*********************************************************************/
{
    int     i, j, x, y;

    if (cz > 2)			/* No menu on large zooms	 */
	zoom (2);
    msg_off ();

    j = (cz == 1) ? 13 : 8;	/* line spaceing		 */

    color (resb, resb);

    for (i = 0; i < n; i++)
	atext (wx + 20 / cz, wy + 462 / cz - (i + 1) * j, s[i], 2 - cz);

    i = getcur (&x, &y);
    if (i < 0)
	err ("menu: couldn't read cursor", i, x, y, 0);

    color (0, resb);
    rect (wx + 20 / cz, wy + 452 / cz - n * j, wx + 300 / cz, wy + 462 / cz);

    if (x >= wx + 20 / cz  && x < wx + 160 &&
	y <= wy + 462 / cz && y > wy + 462 / cz - n * j) {
	msg_on ();
	return ((wy + 462 / cz - y) / j);}
    else {
	err_msg ("Nothing selected");
	return (-1);
    }
}

clp_plt (x0, y0, x1, y1, xl, yl, xh, yh)	/* clip plot	 */
    int x0, y0, x1, y1, xl, yl, xh, yh;
{
    float   a, b, c;
    int     t;

    if (batch)			/* redundant			 */
	return;

    if (xh < xl || yh < yl ||
	    ((x0 < xl && x1 < xl) || (x0 > xh && x1 > xh)) ||
	    ((y0 < yl && y1 < yl) || (y0 > yh && y1 > yh)))
	return;			/* nothing to plot		 */

    if (x0 == x1 && y0 == x1) {	/* just one point		 */
	point (x0, x1);
	return;
    }

    a = y0 - y1;		/* get vector equation		 */
    b = x1 - x0;
    c = y0 * x1 - x0 * y1;

    if (x0 > x1) {		/* insure x0 <= x1		 */
	t = x0;
	x0 = x1;
	x1 = t;
	t = y0;
	y0 = y1;
	y1 = t;
    }

    if (x0 < xl) {		/* xl - clip			 */
	x0 = xl;
	y0 = (b == 0.0) ? y0 : (c - a * (float) xl) / b;
    }

    if (x1 > xh) {		/* xh - clip			 */
	x1 = xh;
	y1 = (b == 0.0) ? y1 : (c - a * (float) xh) / b;
    }

    if (y0 > y1) {		/* insure y0 <= y1		 */
	t = x0;
	x0 = x1;
	x1 = t;
	t = y0;
	y0 = y1;
	y1 = t;
    }

    if (y0 > yh || y1 < yl)	/* out of area ?		 */
	return;

    if (y0 < yl) {		/* yl - clip			 */
	y0 = yl;
	x0 = (a == 0.0) ? x0 : (c - b * (float) yl) / a;
    }

    if (y1 > yh) {		/* yh - clip			 */
	y1 = yh;
	x1 = (a == 0.0) ? x1 : (c - b * (float) yh) / a;
    }

    if (x0 == x1 && y0 == x1)	/* draw result			 */
	point (x0, x1);
    else {
	move (x0, y0);
	draw (x1, y1);
    }
}

aed_plt (x0, y0, x1, y1)	/* plot on aed within window	 */
    int x0, y0, x1, y1;
{
    int     xl, yl, xh, yh;
    static int  msk = 0xfffffe00;

    xl = wx;
    xh = wx + 511 / cz;
    yl = wy;
    yh = wy + 482 / cz;

    if ((xl&msk) != (xh&msk)) {	/* need to split x		 */
	if ((yl&msk) != (yh&msk)) {/* need to split y		 */
	    clp_plt (x0, y0, x1, y1, xl, yl, (xl&msk) + 511, (yl&msk) + 511);
	    clp_plt (x0, y0, x1, y1, (xl&msk) + 512, yl, xh, (yl&msk) + 511);
	    clp_plt (x0, y0, x1, y1, xl, (yl&msk) + 512, (xl&msk) + 511, yh);
	    clp_plt (x0, y0, x1, y1, (xl&msk) + 512, (yl&msk) + 512, xh, yh);}
	else {
	    clp_plt (x0, y0, x1, y1, xl, yl, (xl&msk) + 511, yh);
	    clp_plt (x0, y0, x1, y1, (xl&msk) + 512, yl, xh, yh);
	}}
    else {
	if ((yl&msk) != (yh&msk)) {/* need to split y */
	    clp_plt (x0, y0, x1, y1, xl, yl, xh, (yl&msk) + 511);
	    clp_plt (x0, y0, x1, y1, xl, (yl&msk) + 512, xh, yh);}
	else
	    clp_plt (x0, y0, x1, y1, xl, yl, xh, yh);
    }
}


chg_color ()
/************************************************\
* 						 *
*  Change color: interactive changing of colors	 *
* 						 *
\************************************************/
{
    register int    i, j, bs, bl, x, y;
    int             ix, iy;
    char            *ln[ncolors + 1];
    struct color_tab t;

    ln[0] = "flip sides";
    for (i = 0; i < ncolors; i++)
	ln[i + 1] = Color_tab[i].name;

    if (0 <= (i = menu (ln, ncolors + 1))) {	/* change it	 */

        if (!i) {			/* just flip s1b/s2b	 */
	    t = Color_tab[CT_s1_n];
	    Color_tab[CT_s1_n] = Color_tab[CT_s2_n];
	    Color_tab[CT_s2_n] = t;
	    t = Color_tab[CT_s1_s];
	    Color_tab[CT_s1_s] = Color_tab[CT_s2_s];
	    Color_tab[CT_s2_s] = t;
	    ldcmp ();
	    return;}
        else
	    i--;

	j = (cz == 1) ? 13 : 8;	/* line spaceing		 */
	x = wx + 20 / cz;	/* x-start point		 */
	y = wy + 462 / cz;	/* y-start point		 */
	bs = wx + 128 / cz;	/* bar start			 */
	bl = (cz == 1) ? 255 : 127;	/* bar length		 */

 	msg_off ();
	color (resb, resb);

	draw_bar (x, y, "Red", Color_tab[i].r, bs, bl, j);
	draw_bar (x, y - j, "Green", Color_tab[i].g, bs, bl, j);
	draw_bar (x, y - j * 2, "Blue", Color_tab[i].b, bs, bl, j);

	while (getcur (&ix, &iy) < 4) {	/* change it		 */
	    if (ix < bs || ix > (bs + bl))
		break;
	    ix = (ix - bs) * cz;
	    iy = (iy - (y - 2 * j)) / j;
	    if (iy < 0 || iy > 2)
		break;
	    switch (iy) {
		case 0: 	/* blue				 */
		    Color_tab[i].b = ix;
		    new_bar (y - 2 * j, ix, bs, bl, j - 3);
		    break;
		case 1: 	/* Green			 */
		    Color_tab[i].g = ix;
		    new_bar (y - j, ix, bs, bl, j - 3);
		    break;
		case 2: 	/* red				 */
		    Color_tab[i].r = ix;
		    new_bar (y, ix, bs, bl, j - 3);
		    break;
	    }
	    ldcmp ();
	}

	color (0, resb);	/* clean it			 */
	rect (x, y - 2 * j, x + 384 / cz, y + j);
	msg_on ();
    }
}

draw_bar (x, y, s, v, bs, bl, dy)	/* draw a value bar	 */
    int x, y, v, bs, bl, dy;
    char *s;
{
    atext (x, y, s, 2 - cz);
    new_bar (y, v, bs, bl, dy - 3);
}

new_bar (y, v, bs, bl, dy)		/* plain bar		 */
    int y, v, bs, bl, dy;
{
    color (resb, resb);
    rect (bs, y, bs + bl, y + dy);
    v /= cz;
    if (v < (bl - 1) && v >= 0) {/* the value (v) should be in [0,255] */
	color (0, resb);
	rect (bs + v + 1, y + 1, bs + bl - 1, y + dy - 1);
	color (resb, resb);
    }
}

struct nlst *deq_NL ()		/* alocate a NL element		 */
{
    register struct nlst *p;

    if (V.enll) {		/* recycle old ones		 */
	p = V.enll;
	V.enll = p -> n;}
    else {
	if (V.nnl >= nlmax)
	    err ("Net list space too small: increase 'nlmax'", nlmax, 0, 0,0);
	p = &NL[V.nnl++];
    }

    return p;
}

	
