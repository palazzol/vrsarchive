/***************************************************************\
*								*
*	PCB program						*
*								*
*	CIF output routines					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"

extern FILE *fwantread(), *fwantwrite();

extern int rot_m[4][4];	/* rotation matrix (in pplace)	 */
extern int pdr[8];	/* direction offsets (for ptrs)	 */

FILE *icf, *ocf;	/* Cif - I/O			 */

#define pinsym 1	/* ordinary pin symbol		 */
#define viasym 2	/* via - symbol			 */
#define pwrsym 3	/* power plane connection	 */
#define gndsym 4	/* ground plane connection	 */
#define tlhsym 5	/* tool hole symbol		 */
#define sigsym 6	/* signal pin: no pwr/gnd conn.	 */
#define pn1sym 7	/* pin #1 marking		 */
#define algsym 8	/* alignment symbol		 */

/**********************************************************\
* 							   *
* A note on coordinates:				   *
*    internally: 1 unit = 100mil/rsun			   *
*    CIF output: 1 unit = 0.5 mil for the main section	   *
* 		      = 1 mil for the symbol definition	   *
* 							   *
\**********************************************************/

#define almoff 32	/* offset for alignment marks 	 */
#define ucf 25		/* unit conversion factor	 */
#define pgd 9		/* pin guard zone displacement	 */
#define cmww 100	/* crop mark wire width		 */
#define coww 40		/* component outline wire width	 */
#define gzww 400	/* guard zone wire width	 */
#define sgww0 16	/* signal wire width (near hole) */
#define sgww1 26	/* signal wire width (elsewhere) */
#define min_dst 16	/* min distance between wires	 */
#define cpsb 1000	/* base for component symbols > ex_cifH	 */
#define chhgh 128	/* character hight		 */
#define chwdf 4		/* character width factor	 */
#define clm_off 50	/* offset of clm-s to boarder	 */
#define max_wcp 7	/* max. wire coordinate pairs in a line	*/

/********************************************************************\
* 								     *
*  Some obsolete stuff:						     *
*   dynamic labeling was used to generate the silk-screen lable for  *
*   a component based on its instantiation name. People prefered     *
*   the type definition name.					     *
* 								     *
*   Alignment marks are no longer required by mossis.		     *
* 								     *
*   (the code for tooling holes was entirely ripped out: external    *
*    cif symbols can do as well if required)			     *
* 								     *
\********************************************************************/
/* #define dyna_lab */	/* dynamic labeling		 */
/* #define align_mrk */	/* alignment marks 		 */

static int chdst[128][2];	/* character descriptor table	 */

cifout()			/* output a cif file		 */
{
    char    buf[82];
    int i, c1, c2, s, w, c;

    printf ("Start writing CIF-file\n");

    icf = fwantread (".", "/usr/agn/pcb/pcbsdef.CIF", buf,
	    "Symbol definition file:");
    ocf = fwantwrite (".", "pcb.cif", buf, "CIF output file:", 1);
    if (!ocf)
      err("-NO CIF output file written", 0, 0, 0, 0);
    c1 = c2 = 0;
    for (i = 0; i < 128; ++i)
	chdst[i][0] = (-1);

    while (fgets (buf, 82, icf)) {
	if (buf[0] == '%') {
	    switch (buf[1]) {
		case 'c': 	/* character descriptor	 */
		    i = sscanf (&buf[2], "%d%d%d", &c, &w, &s);
		    if ((i != 3) || (c<=0) || (c>127) || (w<0) || (w>300)) {
			printf ("Illegal character descriptor -- ignored\n");
			break;
		    };
		    if (chdst[c][0] >= 0) {
			printf (
			"Multiple character descriptor (%d) -- ignored\n", c);
			break;
		    };
		    chdst[c][0] = s;
		    chdst[c][1] = w * chwdf;
		    break;
		case 's': 	/* insert component symbols	 */
		    if (c1)
			err ("invalid pcbsdef.CIF - multiple %s", c1, 0, 0, 0);
		    c1 = 1;
		    ocps ();	/* output component symbols	 */
		    break;
		case 'p': 	/* insert PC-board symbol	 */
		    if (c2)
			err ("invalid pcbsdef.CIF - multiple %p", c2, 0, 0, 0);
		    c2 = 1;
		    ofps ();	/* output frame and components	 */
		    opgc ();	/* output pwr / gnd connections  */
		    osws ();	/* output signal wires		 */
		    ohfs ();	/* output hole 'fillet'-s	 */
		    oclm ();	/* output cmpnt location marks	 */
		    clean_up ();	/* remove temp. marks	 */
		    break;
		default: 
		    err ("invalid pcbsdef.CIF - unknown escape",
			(int)buf[1] & 255, 0, 0, 0);
	    };
	}
	else
	    fputs (buf, ocf);
    };

    if (!c1 || !c2)
	err ("invalid pcbsdef.CIF - missing %s or %p", c1, c2, 0, 0);

    fclose (icf);
    fclose (ocf);
}


ofps()				/* output frame components	 */
{
    int i;

#ifdef align_mrk
    scall (-almoff, -almoff, algsym);	/* alignment marks	 */
    scall (V.BSX + almoff, -almoff, algsym);
    scall (-almoff, V.BSY + almoff, algsym);
    scall (V.BSX + almoff, V.BSY + almoff, algsym);
#endif

    cmrk (-2, 18, -2, -2, 18, -2);	/* crop - marks		 */
    cmrk (V.BSX - 18, -2, V.BSX + 2, -2, V.BSX + 2, 18);
    cmrk (V.BSX + 2, V.BSY - 18, V.BSX + 2, V.BSY + 2, V.BSX - 18, V.BSY + 2);
    cmrk (18, V.BSY + 2, -2, V.BSY + 2, -2, V.BSY - 18);

    fputs (" L PN2;\n", ocf);	/* guard zone for inner layers	 */
    cwire_S (    0,     0, 0, 0, gzww);
    cwire_C (V.BSX,     0, 0, 0);
    cwire_C (V.BSX, V.BSY, 0, 0);
    cwire_C (    0, V.BSY, 0, 0);
    cwire_E (    0,     0, 0, 0);
    fputs (" L PN3;\n", ocf);
    cwire_S (    0,     0, 0, 0, gzww);
    cwire_C (V.BSX,     0, 0, 0);
    cwire_C (V.BSX, V.BSY, 0, 0);
    cwire_C (    0, V.BSY, 0, 0);
    cwire_E (    0,     0, 0, 0);

    for (i = 0; i < V.ncp; ++i)	/* scan components to call sym	 */
	if (strcmp (CP[i].name, DELETED))
	    dcp (&CP[i]);
}

ocps ()				/* output component symbols	 */
{
    register int i, j, k, x, y;
    int ext = 0;
    register struct pin *pp;
    char buf[inp_lnl], fname[80];
    FILE *ex_cif;

    for (j = 0; j < V.nty; ++j) {

	if (!strcmp (TY[j].name, DELETED))
	    continue;		/* skip deleted entries		 */

	if (TY[j].cif) {	/* skip external components	 */
	    ext = 1;
	    continue;
	}

	fprintf (ocf, "DS %d 1270 1;\n", j + cpsb);
	pp = TY[j].p;
	for (i = 0; i < TY[j].np; ++i) {
	    x = pp -> x;
	    y = pp -> y;
	    if (!i)
		scall (x, y, pn1sym);
	    switch (pp -> p) {
		case 0: 
		    scall (x, y, pinsym);
		    break;
		case 1: 
		    scall (x, y, pinsym);
		    scall (x, y, gndsym);
		    break;
		case 2: 
		case 3: 
		case 4: 
		    scall (x, y, pinsym);
		    scall (x, y, pwrsym);
		    break;
		default: 
		    err ("illegal pin type", pp -> p, i, 0, 0);
	    };
	    pp++;
	};

	fputs (" L PSSC;\n", ocf);	/* outline component	 */
	cwire_S (         -3,          -3, 0, 0, coww);
	cwire_C (TY[j].x + 3,          -3, 0, 0);
	cwire_C (TY[j].x + 3, TY[j].y + 3, 0, 0);
	cwire_C (         -3, TY[j].y + 3, 0, 0);
	cwire_E (         -3,          -3, 0, 0);
	if (TY[j].y >= rsun * 3 && TY[j].x >= rsun * 3)	/* add an IC mark */
	    fprintf (ocf, "  B 200 200 -50 %d;\n", (TY[j].y >> 1) * ucf);
	fputs (" L PF;\n", ocf);
	cwire_S (         -3,          -3, 0, 0, coww);
	cwire_C (TY[j].x + 3,          -3, 0, 0);
	cwire_C (TY[j].x + 3, TY[j].y + 3, 0, 0);
	cwire_C (         -3, TY[j].y + 3, 0, 0);
	cwire_E (         -3,          -3, 0, 0);
	if (TY[j].y >= rsun * 3 && TY[j].x >= rsun * 3)	/* add an IC mark */
	    fprintf (ocf, "  B 200 200 -50 %d;\n", (TY[j].y >> 1) * ucf);

#ifndef dyna_lab
	for (i = nmmax; i; i--)	/* truncate string if necessary	 */
	    if ((k = txt_len (TY[j].name, i)) <= (TY[j].x + 4) * ucf)
		break;
	x = ((TY[j].x + 4) * ucf - k) / 2 - 2 * ucf;/* center text */
	y = ((TY[j].y + 4) * ucf - chhgh) / 2 - 2 * ucf;
	ctxt (x, y, 0, TY[j].name, i);
#endif

	fputs ("DF;\n", ocf);
    }

    if (ext) {			/* external components present	 */
	printf ("List of external components:\n");
	for (i = 0; i < V.nty; i++)
	    if (TY[i].cif)
		printf ("\t'%s':\tcif-symbol#=%d\n",
			TY[i].name, CIF[TY[i].cif-1].symn);
	ex_cif = fwantread (".", "", fname, "External CIF-symbol file:");
	if (!ex_cif)
	    err ("Could not open external CIF-symbol file", 0, 0, 0, 0);
	while (fgets (buf, inp_lnl, ex_cif))
	   fputs (buf, ocf);
	fclose (ex_cif);
    }
}

dcp (C)				/* call a component symbol	 */
    struct comp *C;
{
    static int  rm[4][4] = { { 1,  1,  0,  1},
			     {-1,  1,  1,  0},
			     {-1, -1,  0, -1},
			     { 1, -1, -1,  0}  };
    union ptr_int {		/* allow arithmetic on pointer	*/
	unsigned i;
	struct type *ty;
    } p1, p2;
    register int i, r, x, y;

    x = C -> x;
    y = C -> y;
    r = C -> r;

    i = C -> ty -> cif;
    if (i) 			/* external symbol		 */
	i = CIF[i - 1].symn;
    else {			/* internal symbol		 */
	p1.ty = C -> ty;
	p2.ty = V.ty;
	i = (p1.i - p2.i) / sizeof (TY[0]) + cpsb;
    }

    fprintf (ocf, "C %d R%d %d T%d %d;\n", i, rm[r][3], rm[r][2],
	     x * ucf, y * ucf);

#ifdef dyna_lab
    i = (C -> ty -> y * ucf - chhgh) / 2;
    ctxt (C -> x * ucf + i * rm[C -> r][0],
          C -> y * ucf + i * rm[C -> r][1], C -> r, C -> name, nmmax);
#endif
}

ctxt (x, y, r, s, l)		/* create text			 */
    int x, y, r, l;
    char *s;
{
    while (*s && l--) {
	*s &= 127;
	if (chdst[*s][0] > 0) {
	    switch (r) {
		case 1: 
		    fprintf (ocf, "C %d R 0 1 T %d %d;\n",
			     chdst[*s][0], x, y);
		    y += chdst[*s][1];
		    break;
		case 2: 
		    fprintf (ocf, "C %d R -1 0 T %d %d;\n",
			     chdst[*s][0], x, y);
		    x -= chdst[*s][1];
		    break;
		case 3: 
		    fprintf (ocf, "C %d R 0 -1 T %d %d;\n",
			     chdst[*s][0], x, y);
		    y -= chdst[*s][1];
		    break;
		default: 
		    fprintf (ocf, "C %d T %d %d;\n",
			     chdst[*s][0], x, y);
		    x += chdst[*s][1];
		    break;
	    };
	};
	s++;
    };
}

txt_len (s, l)			/* text length function		 */
    char *s;
    int l;
{
    register int i = 0;

    while (*(s++) && l--)
	i += chdst[*s & 127][1];

    return i;
}


cmrk (x1, y1, x2, y2, x3, y3)		/* crop mark		 */
    int x1, y1, x2, y2, x3, y3;
{
    fputs (" L PC1;\n", ocf);
    cwire_S (x1, y1, 0, 0, cmww);
    cwire_C (x2, y2, 0, 0);
    cwire_E (x3, y3, 0, 0);
    fputs (" L PC4;\n", ocf);
    cwire_S (x1, y1, 0, 0, cmww);
    cwire_C (x2, y2, 0, 0);
    cwire_E (x3, y3, 0, 0);
    fputs (" L PSSC;\n", ocf);
    cwire_S (x1, y1, 0, 0, cmww);
    cwire_C (x2, y2, 0, 0);
    cwire_E (x3, y3, 0, 0);
    fputs (" L PSMC;\n", ocf);
    cwire_S (x1, y1, 0, 0, cmww);
    cwire_C (x2, y2, 0, 0);
    cwire_E (x3, y3, 0, 0);
    fputs (" L PD;\n", ocf);
    cwire_S (x1, y1, 0, 0, cmww);
    cwire_C (x2, y2, 0, 0);
    cwire_E (x3, y3, 0, 0);
    fputs (" L PF;\n", ocf);
    cwire_S (x1, y1, 0, 0, cmww);
    cwire_C (x2, y2, 0, 0);
    cwire_E (x3, y3, 0, 0);
}

scall (x, y, t)			/* symbol call		 */
    int x, y, t;
{
    fprintf (ocf, "C %d T %d,%d;\n", t, x * ucf, y * ucf);
}

#ifdef simple			/* simple wire routines		 */
osws ()				/* output signal wire stuff	 */
{
    int     i, chplt (), cvplt ();

    wtvf = cvplt;
    wthf = chplt;
    for (i = 0; i < V.nnh; ++i)	/* trace all nets		 */
	if (NH[i].l > 1)
	    wtrace (NH[i].lp -> c -> x, NH[i].lp -> c -> y, vec);
}

#else
osws ()				/* output signal wire stuff	 */
{
    int     chplt (), cvplt (), mrk_hole (), umrk_hole ();
    register int i, x, y;

    for (i = 0; i < V.nnh; ++i)	/* trace all nets		 */
	if (NH[i].l > 1) {
	    wtvf = nil;		/* mark all holes of this net	 */
	    wthf = mrk_hole;
	    x = NH[i].lp -> c -> x;
	    y = NH[i].lp -> c -> y;
	    wtrace (x, y, vec);
	    wtvf = cvplt;	/* plot the net			 */
	    wthf = chplt;
	    wtrace (x, y, vec);
	    wtvf = nil;		/* remove the hole marks	 */
	    wthf = umrk_hole;
	    wtrace (x, y, vec);
	}
}
#endif


#ifdef simple			/* simple wire routines		 */
cvplt (x1, y1, x2, y2)		/* output a wire segment	 */
    int x1, y1, x2, y2;
/******************************************************************\
* 								   *
*  Plot part of a signal wire from <x1,y1> to <x2,y2>. This draws  *
*  just a straight line on the desired side of the board.	   *
* 								   *
\******************************************************************/
{
    static int  s = 0;
    if (s != wtsb) {		/* side check			 */
	if (wtsb == s1b)
	    fputs (" L PC1;\n", ocf);
	else
	    fputs (" L PC4;\n", ocf);
	s = wtsb;
    };
    cwire_S (x1, y1, 0, 0, sgww1);
    cwire_E (x2, y2, 0, 0);
}

#else				/* conservative wire routine	 */
cvplt (x1, y1, x2, y2)		/* output a wire segment	 */
    int x1, y1, x2, y2;
/**********************************************************************\
* 								       *
*  A wire is drawn from <x1,y1> to <x2,y2> on the desired board side.  *
*  A minimal distance to holes is maintained. This calles for bends    *
*  in the wire path if it 1 raster unit apart from a hole.	       *
* 								       *
\**********************************************************************/
{
    register int i, j, t;
    register char *p;
    static int  s = 0;

    if (s != wtsb) {		/* side check			 */
	if (wtsb == s1b)
	    fputs (" L PC1;\n", ocf);
	else
	    fputs (" L PC4;\n", ocf);
	s = wtsb;
    }


    p = &pcb[(y1 + y2) >> 1][(x1 + x2) >> 1];
    for (i = 0, j = 0; i < 8; i++)	/* wide wire locator	 */
	if (*(p + pdr[i]) & wtsb)
	    j++;		/* count adjacent points	 */
    if (j > 6)
	wwb_trc ((x1 + x2) >> 1, (y1 + y2) >> 1, wtsb);


    if ((y2 < y1) || ((y2 == y1) && (x2 < x1))) {
	t = x1;			/* direction confined 0 to 3	 */
	x1 = x2;
	x2 = t;
	t = y1;
	y1 = y2;
	y2 = t;
    }

    if (x2 > x1) {
	if (y2 > y1)
	    plt_d1wire (x1, y1, x2, y2);	/* dir = 1	 */
	else
	    plt_hwire (x1, y1, x2, y2);}	/* dir = 0	 */
    else {
	if (x2 < x1)
	    plt_d2wire (x1, y1, x2, y2);	/* dir = 3	 */
	else
	    plt_vwire (x1, y1, x2, y2);		/* dir = 2	 */
    }
}

plt_d1wire (x1, y1, x2, y2)	/* plot diag. wire		 */
    int x1, y1, x2, y2;
/***********************************************************\
* 							    *
*  Plot a diagonal wire from <x1,y1> to <x2,y2>. The slope  *
*  is dx = 1, dy = 1.					    *
* 							    *
\***********************************************************/
{
    register int enable, nh1, nh2;

    nh1 = near_hl (x1, y1);
    nh2 = near_hl (x2, y2);
    enable = 3 <= (!nh1 + !nh2 + y2 - y1); /* attn: trick! */

    switch (nh1) {		/* check start point		 */
	case 0:
	    if (enable)
	        cwire_S (x1, y1, 0, 0, sgww1);
	    else
		cwire_S (x1, y1, 0, 0, sgww0);
	    break;
	case 1: 
	    cwire_S (x1, y1, 0, 1, sgww0);
	    if (enable) {
	        cwire_C (x1, y1, 1, 1);
		cwire_E (x1 + 1, y1 + 1, 0, 0);
		cwire_S (x1 + 1, y1 + 1, 0, 0, sgww1);}
	    else
		cwire_C (x1, y1, 1, 1);
	    break;
	case 4: 
	    cwire_S (x1, y1, 1, 0, sgww0);
	    if (enable) {
	        cwire_C (x1, y1, 1, 1);
		cwire_E (x1 + 1, y1 + 1, 0, 0);
		cwire_S (x1 + 1, y1 + 1, 0, 0, sgww1);}
	    else
		cwire_C (x1, y1, 1, 1);
	    break;
	default: 
	    err ("plt_d1wire: wire / hole collision", x1, y1, nh1, 0);
    }

    switch (nh2) {
	case 0: 
	    cwire_E (x2, y2, 0, 0);
	    break;
	case 2: 
	    if (enable) {
		cwire_E (x2 - 1, y2 - 1, 0, 0);
		cwire_S (x2 - 1, y2 - 1, 0, 0, sgww0);
	    }
	    cwire_C (x2, y2, -1, -1);
	    cwire_E (x2, y2, -1, 0);
	    break;
	case 3: 
	    if (enable) {
		cwire_E (x2 - 1, y2 - 1, 0, 0);
		cwire_S (x2 - 1, y2 - 1, 0, 0, sgww0);
	    }
	    cwire_C (x2, y2, -1, -1);
	    cwire_E (x2, y2, 0, -1);
	    break;
	default: 
	    err ("plt_d1wire: wire / hole intersection", x2, y2, nh2, 0);
    }
}


plt_d2wire (x1, y1, x2, y2)	/* plot diag. wire		 */
    int x1, y1, x2, y2;
/***********************************************************\
* 							    *
*  Plot a diagonal wire from <x1,y1> to <x2,y2>. The slope  *
*  is dx = -1, dy = 1.					    *
* 							    *
\***********************************************************/
{
    register int enable, nh1, nh2;

    nh1 = near_hl (x1, y1);
    nh2 = near_hl (x2, y2);
    enable = 3 <= (!nh1 + !nh2 + y2 - y1); /* attn: trick! */

    switch (nh1) {		/* check start point	 */
	case 0:
	    if (enable)
		cwire_S (x1, y1, 0, 0, sgww1);
	    else
		cwire_S (x1, y1, 0, 0, sgww0);
	    break;
	case 1: 
	    cwire_S (x1, y1, 0, 1, sgww0);
	    if (enable) {
	        cwire_C (x1, y1, -1, 1);
		cwire_E (x1 - 1, y1 + 1, 0, 0);
		cwire_S (x1 - 1, y1 + 1, 0, 0, sgww1);}
	    else
		cwire_C (x1, y1, -1, 1);
	    break;
	case 2: 
	    cwire_S (x1, y1, -1, 0, sgww0);
	    if (enable) {
		cwire_C (x1, y1, -1, 1);
		cwire_E (x1 - 1, y1 + 1, 0, 0);
		cwire_S (x1 - 1, y1 + 1, 0, 0, sgww1);}
	    else
		cwire_C (x1, y1, -1, 1);
	    break;
	default: 
	    err ("plt_d2wire: wire / hole collision", x1, y1, nh1, 0);
    }

    switch (nh2) {		/* check end point		 */
	case 0: 
	    cwire_E (x2, y2, 0, 0);
	    break;
	case 3:
	    if (enable) {
		cwire_E (x2 + 1, y2 - 1, 0, 0);
		cwire_S (x2 + 1, y2 - 1, 0, 0, sgww0);
	    }
	    cwire_C (x2, y2, 1, -1);
	    cwire_E (x2, y2, 0, -1);
	    break;
	case 4: 
	    if (enable) {
		cwire_E (x2 + 1, y2 - 1, 0, 0);
		cwire_S (x2 + 1, y2 - 1, 0, 0, sgww0);
	    }
	    cwire_C (x2, y2, 1, -1);
	    cwire_E (x2, y2, 1, 0);
	    break;
	default: 
	    err ("plt_d2wire: wire / hole intersection", x2, y2, nh2, 0);
    }
}

plt_hwire (x1, y1, x2, y2)	/* plot a horizontal wire	 */
    int x1, y1, x2, y2;
/*************************************************************\
* 							      *
*  Draw a horizontal wire from <x1,y1> to <x2,y2>. The slope  *
*  is: dx = 1, dy = 0;					      *
* 							      *
\*************************************************************/
{
    register int dy, nh1, nh2, wide;

    dy = 0;			/* default displacement		 */

    nh1 = near_hl (x1, y1);
    nh2 = near_hl (x1 + 1, y1);	/* look ahead			 */

    switch (nh1) {		/* check start point		 */
	case 0:
	    if (!nh2) {
		cwire_S (x1, y1, 0, 0, sgww1);
		wide = 1;}
	    else {
		cwire_S (x1, y1, 0, 0, sgww0);
		wide = 0;
	    }
	    break;
	case 1: 
	    cwire_S (x1, y1, 0, 1, sgww0);
	    dy = 1;
	    wide = 0;
	    break;
	case 3: 
	    cwire_S (x1, y1, 0, -1, sgww0);
	    dy = -1;
	    wide = 0;
	    break;
	case 4: 
	    cwire_S (x1, y1, 1, 0, sgww0);
	    wide = 0;
	    break;
	default: 
	    err ("plt_hwire: wire / hole collision", x1, y1, nh1, 0);
    }

    while (++x1 < x2) {		/* scan wire 			 */
	nh1 = nh2;
	nh2 = near_hl (x1 + 1, y1);
	switch (nh1) {
	    case 0:
		if (!wide && !nh2) {
		    if (dy) {	/* end any bend			 */
			cwire_C (x1 - 1, y1, 0, dy);
		    	dy = 0;
		    }
		    cwire_E (x1, y1, 0, 0);
		    cwire_S (x1, y1, 0, 0, sgww1);
		    wide = 1;}
		else if (dy) {
		    cwire_C (x1 - 1, y1, 0, dy);
		    cwire_C (x1, y1, 0, 0);
		    dy = 0;
		}
		break;
	    case 1: 
		if (dy != 1) {	/* start a bend up	 */
		    if (wide) {
			cwire_E (x1 - 1, y1, 0, dy);
			cwire_S (x1 - 1, y1, 0, dy, sgww0);
			wide = 0;}
		    else
			cwire_C (x1 - 1, y1, 0, dy);
		    cwire_C (x1, y1, 0, 1);
		    dy = 1;
		}
		break;
	    case 3: 
		if (dy != -1) {	/* start a bend down	 */
		    if (wide) {
			cwire_E (x1 - 1, y1, 0, dy);
			cwire_S (x1 - 1, y1, 0, dy, sgww0);
			wide = 0;}
		    else
			cwire_C (x1 - 1, y1, 0, dy);
		    cwire_C (x1, y1, 0, -1);
		    dy = -1;
		}
		break;
	    default: 
		err ("plt_hwire: illegal wire/hole realtion", x1,y1, nh1, dy);
	}
    }

    switch (nh2) {		/* check end point		 */
	case 0: 
	    if (dy != 0)	/* finish any bend		 */
		cwire_C (x2 - 1, y2, 0, dy);
	    cwire_E (x2, y2, 0, 0);
	    break;
	case 1: 
	    if (dy != 1) {	/* start an incomplete bend up	 */
		if (wide) {
		    cwire_E (x1 - 1, y1, 0, dy);
		    cwire_S (x1 - 1, y1, 0, dy, sgww0);}
		else
		    cwire_C (x1 - 1, y1, 0, dy);
	    }
	    cwire_E (x2, y2, 0, 1);
	    break;
	case 2: 
	    if (dy != 0)	/* finish any bend		 */
		cwire_C (x2 - 1, y2, 0, dy);
	    else if (wide) {
		cwire_E (x2 - 1, y2, 0, 0);
		cwire_S (x2 - 1, y2, 0, 0, sgww0);
	    }
	    cwire_E (x2, y2, -1, 0);
	    break;
	case 3: 
	    if (dy != -1) {	/* start an incomplete bend down */
		if (wide) {
		    cwire_E (x1 - 1, y1, 0, dy);
		    cwire_S (x1 - 1, y1, 0, dy, sgww0);}
		else
		    cwire_C (x1 - 1, y1, 0, dy);
	    }
	    cwire_E (x2, y2, 0, -1);
	    break;
	default: 
	    err ("plt_hwire: trouble at end point", x2, y2, nh2, dy);
    }
}

plt_vwire (x1, y1, x2, y2)	/* plot a vertcal wire		 */
    int x1, y1, x2, y2;
/**********************************************************\
* 							   *
*  Draw a vertcal wire from <x1,y1> to <x2,y2>. The slope  *
*  is: dx = 0, dy = 1;					   *
* 							   *
\**********************************************************/
{
    register int dx, nh1, nh2, wide;

    dx = 0;			/* default displacement		 */

    nh1 = near_hl (x1, y1);
    nh2 = near_hl (x1, y1 + 1);	/* look ahead			 */

    switch (nh1) {		/* check start			 */
	case 0:
	    if (!nh2) {
	        cwire_S (x1, y1, 0, 0, sgww1);
		wide = 1;}
	    else {
	        cwire_S (x1, y1, 0, 0, sgww0);
		wide = 0;
	    }
	    break;
	case 1: 
	    cwire_S (x1, y1, 0, 1, sgww0);
	    wide = 0;
	    break;
	case 2: 
	    cwire_S (x1, y1, -1, 0, sgww0);
	    dx = -1;
	    wide = 0;
	    break;
	case 4: 
	    cwire_S (x1, y1, 1, 0, sgww0);
	    dx = 1;
	    wide = 0;
	    break;
	default: 
	    err ("plt_vwire: wire / hole collision", x1, y1, nh1, dx);
    }

    while (++y1 < y2) {		/* scan wire 			 */
	nh1 = nh2;
	nh2 = near_hl (x1, y1 + 1);
	switch (nh1) {
	    case 0:
		if (!wide && !nh2) { 
		    if (dx) {	/* end any bend			 */
		        cwire_C (x1, y1 - 1, dx, 0);
			dx = 0;
		    }
		    cwire_E (x1, y1, 0, 0);
		    cwire_S (x1, y1, 0, 0, sgww1);
		    wide = 1;}
		else if (dx) {	/* end any bend			 */
		    cwire_C (x1, y1 - 1, dx, 0);
		    cwire_C (x1, y1, 0, 0);
		    dx = 0;
		}
		break;
	    case 2: 
		if (dx != -1) {	/* start a bend left		 */
		    if (wide) {
			cwire_E (x1, y1 - 1, dx, 0);
			cwire_S (x1, y1 - 1, dx, 0, sgww0);
			wide = 0;}
		    else
			cwire_C (x1, y1 - 1, dx, 0);
		    cwire_C (x1, y1, -1, 0);
		    dx = -1;
		}
		break;
	    case 4: 
		if (dx != 1) {	/* start a bend right		 */
		    if (wide) {
			cwire_E (x1, y1 - 1, dx, 0);
			cwire_S (x1, y1 - 1, dx, 0, sgww0);
			wide = 0;}
		    else
			cwire_C (x1, y1 - 1, dx, 0);
		    cwire_C (x1, y1, 1, 0);
		    dx = 1;
		}
		break;
	    default: 
		err ("plt_vwire: illegal wire/hole realtion", x1,y1, nh1, dx);
	}
    }

    switch (nh2) {		/* check end point		 */
	case 0: 
	    if (dx != 0)	/* finish any bend		 */
		cwire_C (x2, y2 - 1, dx, 0);
	    cwire_E (x2, y2, 0, 0);
	    break;
	case 2: 
	    if (dx != -1) {	/* start an incomplte bend left	 */
		if (wide) {
		    cwire_E (x2, y2 - 1, dx, 0);
		    cwire_S (x2, y2 - 1, dx, 0, sgww0);}
		else
		    cwire_C (x2, y2 - 1, dx, 0);
	    }
	    cwire_E (x2, y2, -1, 0);
	    break;
	case 3: 
	    if (dx != 0)	/* finish any bend		 */
		cwire_C (x2, y2 - 1, dx, 0);
	    else if (wide) {
		cwire_E (x2 - 1, y2, 0, 0);
		cwire_S (x2 - 1, y2, 0, 0, sgww0);
	    }
	    cwire_E (x2, y2, 0, -1);
	    break;
	case 4: 
	    if (dx != 1) {	/* start an incomplete bend up	 */
		if (wide) {
		    cwire_E (x2, y2 - 1, dx, 0);
		    cwire_S (x2, y2 - 1, dx, 0, sgww0);}
		else
		    cwire_C (x2, y2 - 1, dx, 0);
	    }
	    cwire_E (x2, y2, 1, 0);
	    break;
	default: 
	    err ("plt_vwire: trouble at end point", x2, y2, nh2, dx);
    }
}

on_hole (x, y)			/* on hole check		 */
    int x, y;
/**************************************************************\
* 							       *
*  Returns '1' if <x,y> is part of a hole that is not part     *
*  of the current net (unselected). It returns '0' otherwise.  *
*  This routine is not fooled by guard bits for via-holes.     *
* 							       *
\**************************************************************/
{
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

    return (!(pcb[y][x] & selb)); /* test for selection		 */
}

near_hl (x, y)			/* near hole test		 */
    int x, y;
/*******************************************************************\
* 								    *
*  The relation of <x,y> to the nearest hole (if any) is reported.  *
*  if no hole is present, a 0 is returned. values returned (Hex):   *
* 								    *
*         .  .  1  1  1  .  .					    *
*         .  .  .  .  .  .  .					    *
*         2  .  *  *  *  .  4					    *
*         2  .  *  *  *  .  4					    *
*         2  .  *  *  *  .  4					    *
*         .  .  .  .  .  .  .					    *
*         .  .  3  3  3  .  .					    *
* 								    *
\*******************************************************************/
{   int i;
    static int  Rx[4] = { 0, 2, 0,-2};
    static int  Ry[4] = {-2, 0, 2, 0};

    for (i = 0; i < 4; ++i)	/* look for nearby hole		 */
	if (on_hole (x + Rx[i], y + Ry[i]))
	    return (i + 1);

    return (0);			/* not near a hole		 */
}

mrk_hole (x, y)			/* mark a hole			 */
    int x, y;
{
    pcb[y][x] |= selb;
    return (0);
}

umrk_hole (x, y)		/* mark a hole			 */
    int x, y;
{
    pcb[y][x] &= ~selb;
    return (0);
}

#endif

chplt (x, y)		/* output via holes		 */
    int x, y;
{
    if (pcb[y][x] & ishb)
	scall (x, y, viasym);
    return (0);
}

static int real_x, real_y, cnt;	/* common to cwire_*		 */
cwire_S (x, y, dx, dy, w)	/* start a CIF - wire		 */
    int x, y, dx, dy, w;
/********************************************************************\
* 								     *
*  A CIF wire of width w is started at location <x,y>, which is	     *
*  given in raster units. A displacement <dx,dy> is applied to this  *
*  location.							     *
* 								     *
\********************************************************************/
{
    fprintf (ocf, " W %d: %d,%d",
	    w, real_x = x * ucf + dx * pgd, real_y = y * ucf + dy * pgd);
    cnt = 1;
}

cwire_C (x, y, dx, dy)		/* continue a CIF - wire	 */
    int x, y, dx, dy;
{   int     rx, ry;

    rx = x * ucf + dx * pgd;
    ry = y * ucf + dy * pgd;
    if (rx != real_x || ry != real_y) {
	real_x = rx;
	real_y = ry;
	if (++cnt < max_wcp)
	    fprintf (ocf, " %d,%d", rx, ry);
	else {
	    cnt = 0;
	    fprintf (ocf, "\n  %d,%d", rx, ry);
	}
    }
}

cwire_E (x, y, dx, dy)		/* and a CIF - wire	 */
    int x, y, dx, dy;
{   int rx, ry;

    rx = x * ucf + dx * pgd;
    ry = y * ucf + dy * pgd;
    if (rx != real_x || ry != real_y)
	fprintf (ocf, " %d,%d;\n", rx, ry);
    else
	fprintf (ocf, ";\n");
}

opgc ()
/*******************************************************************\
* 								    *
*  Output power/ground connections:				    *
* 								    *
*  Connections to the internal layers (VCC,GND ...) can be made by  *
*  2 different methods:						    *
*  a) by defining a pin to be VCC/GND/VEE/VTT in the type file.	    *
*     this is taken care off by the component symbol generation	    *
*     in 'ocps'.						    *
*  b) by connecting a pin to the special nets VCC/GND/VEE/VTT in    *
*     a given instantiation of the type: this is done here!	    *
* 								    *
\*******************************************************************/
{
    register int j, r, xp, yp;
    int i, x, y;
    register struct pin *pin;
    struct hole *hp;
    register struct nlhd *net;
    struct hole *fndh ();	

    for (i = 0; i < V.ncp; i++) {	/* scan all components	 */

	if (!strcmp (CP[i].name, DELETED))
	    continue;			/* skip deleted entries	 */

	if (CP[i].ty -> cif)
	    continue;			/* skip external cifs	 */

	x = CP[i].x;			/* get borad position	 */
	y = CP[i].y;
	r = CP[i].r;
	for (j = CP[i].ty -> np, pin = CP[i].ty -> p; j; j--, pin++) {
				/* scan component pins		 */
	    if (pin -> p)
		continue;	/* pwr/gnd is done in symbol def */

	    xp = x + (pin -> x) * rot_m[r][0] + (pin -> y) * rot_m[r][1];
	    yp = y + (pin -> x) * rot_m[r][2] + (pin -> y) * rot_m[r][3];

	    hp = fndh (xp, yp);	/* find hole			 */
	    if (!hp)
		err ("opgc: missing hole", xp, yp, 0, 0);

	    net = hp -> n -> nlp;	/* insert proper symbols */
	    if (net == &V.GND)		
		scall (xp, yp, gndsym);
	    else if (net == &V.VCC || net == &V.VEE || net == &V.VTT)
		scall (xp, yp, pwrsym);
	    else
		scall (xp, yp, sigsym);
	}
    }
}

ohfs ()
/****************************************************************\
* 								 *
*  Hole 'fillet'-s:						 *
*     In order to prevent demage to the board if components are	 *
*     replaced, extra cooper is added to the area where a wire	 *
*     enters a component hole.					 *
* 								 *
\****************************************************************/
{
    register int i, x, y;

    fprintf (ocf, "L PC1;\n");		/* side 1 first		 */
    for (i = 0; i < V.nch; i++)
	if (pcb[CH[i].y][CH[i].x] & s1b) {
	    x = CH[i].x;
	    y = CH[i].y;
	    fillet (x, y, s1b);
	}

    fprintf (ocf, "L PC4;\n");		/* now side 2		 */
    for (i = 0; i < V.nch; i++)
	if (pcb[CH[i].y][CH[i].x] & s2b) {
	    x = CH[i].x;
	    y = CH[i].y;
	    fillet (x, y, s2b);
	}
}

/* geometric constraints for fillet round-flash			 */

#define r0 72 	/* = (ucf*sqrt(2^2 + 4^2) - min_dst - via_hole_radius) * 2 */
#define r1 42	/* = (ucf*2 - (sgww1/2 + min_dst)) * 2		 */
#define r2 54	/* = (sqrt((ucf*2)^2 + ucf^2) - (sgww1/2 + min_dst)) * 2 */
#define r3 83	/* = (sqrt(2)*ucf*2 - (sgww1/2 + min_dst)) * 2	 */

fillet (x, y, s)			/* draw a 'fillet'	 */
    int x, y, s;
{
    static int pdx[16] = {2,2,2,1,0,-1,-2,-2,-2,-2,-2,-1,0,1,2,2};
    static int pdy[16] = {0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2,-2,-1};
    static int msk[8] = { 0x8003, 0x001f, 0x0038, 0x01f0,
			  0x0380, 0x1f00, 0x3800, 0xf001  };
    static int d[16] = {r1,r2,r3,r2,r1,r2,r3,r2,r1,r2,r3,r2,r1,r2,r3,r2};

    register int xx, yy, j, k, l, r;
    int i;

    for (i = 0; i < 16; i++) {	/* scan potential candidates	 */
	xx = x + pdx[i];
	yy = y + pdy[i];
	if (pcb[yy][xx] & s) {	/* found a wire			 */

	    r = r0;		/* default radius		 */

	    j = 0;		/* reset search mask		 */
	    for (k = 0; k < 8; k++) {
		l = pcb[yy + dr[k][1]][xx + dr[k][0]];
		if (l & ishb)
		    r = r1;
		if (l & (chb | s) && !(l & ishb))
		    j |= msk[k];
	    }
	    for (k = 0; k < 16; k++) {	/* find max legal radius */
		if (j & (1 << k))
		   continue;	/* skip this point		 */
		if (pcb[yy + pdy[k]][xx + pdx[k]] & (ahb | s)) {
		     if (r > d[k])
			r = d[k];
		}
	    }

	    fprintf (ocf, "R %d %d %d;\n", r, xx * ucf, yy * ucf);
	}
    }
}

oclm ()				/* output component location marks    */
{
    register int i, j, k, l, dis;
    char buf[10];

    dis = 5 * rsun;		/* distance to next mark		 */
    dis /= 2;

    buf[1] = 0;
    for (i = dis, j = 1, k = 0; i < V.BSX - 2 * rsun; i += dis) {
	if (j) {		/* character time */
	    j = 0;
	    buf[0] = 'A' + k++;
	    l = i * ucf - txt_len (buf, 2) / 2;
	    ctxt (l, clm_off, 0, buf, 2);
	    ctxt (l, V.BSY * ucf - chhgh - clm_off, 0, buf, 2);}
	else {
	    fputs ("L PSSC;\n", ocf);
	    cwire_S (i, 2, 0, 0, coww);
	    cwire_E (i, 5, 0, 0);
	    cwire_S (i, V.BSY - 2, 0, 0, coww);
	    cwire_E (i, V.BSY - 5, 0, 0);
	    fputs ("L PF;\n", ocf);
	    cwire_S (i, 2, 0, 0, coww);
	    cwire_E (i, 5, 0, 0);
	    cwire_S (i, V.BSY - 2, 0, 0, coww);
	    cwire_E (i, V.BSY - 5, 0, 0);
	    j = 1;
	}
    }

    for (i = dis, j = 1, k = 0; i < V.BSY - 2 * rsun; i += dis) {
	if (j) {		/* character time */
	    j = 0;
	    sprintf (buf, "%d", ++k);
	    l = i * ucf - txt_len (buf, 2) / 2;
	    ctxt (clm_off + chhgh, l, 1, buf, 10);
	    ctxt (V.BSX * ucf - clm_off, l, 1, buf, 10);}
	else {
	    fputs ("L PSSC;\n", ocf);
	    cwire_S (2, i, 0, 0, coww);
	    cwire_E (5, i, 0, 0);
	    cwire_S (V.BSX - 2, i, 0, 0, coww);
	    cwire_E (V.BSX - 5, i, 0, 0);
	    fputs ("L PF;\n", ocf);
	    cwire_S (2, i, 0, 0, coww);
	    cwire_E (5, i, 0, 0);
	    cwire_S (V.BSX - 2, i, 0, 0, coww);
	    cwire_E (V.BSX - 5, i, 0, 0);
	    j = 1;
	}
    }
}

clean_up ()		/* remove the marks for wide wire traces */
{
    register int i, j, ss;
    register char *p;

    ss = ~(selb | resb);	/* bits to be cleared		 */

    for (i = 0; i < V.BSY; i++) {
	p = &pcb[i][0];
	for (j = V.BSY; j; j--)
	    *p++ &= ss;
    }
}

wwb_trc (x, y, s)	/* wide wire boarder trace		 */
    int x, y, s;
/**********************************************************************\
* 								       *
*  Wide wires tend to have a rough boarder because 'wtrace' may scan   *
*  the wide wire un-parallel to its main direction. So 'wwb_trc'       *
*  scans the boarder and produce a wire that smoothes this roughness.  *
* 								       *
\**********************************************************************/
{
    register int ss = ~(s | ahb), i, j, k;
    register char *p, *ps;

    p = &pcb[y][x];
    while (*(p + 1) & ~ss) p++;	/* find boarder			 */

    if (!(*p & ~ss) || *p & resb)
	return;			/* trouble || already done: exit */

    for (i = 1; i < 8; i++)	/* find initial direction	 */
	if (*(p + pdr[i]) & ~ss)
	    break;

    ps = p;			/* remember start point		 */

    do {
	*p |= resb;		/* mark point			 */

	p += pdr[i];		/* move to next point		 */

	for (j = (i + 6) & 7, k = 7; k; k--, j = (j + 1) & 7)
	    if (!(*(p + pdr[(j + 7) & 7]) & ~ss) && *(p + pdr[j]) & ~ss)
		break;		/* find new direction		 */

	if (i != j) {		/* change in direction		 */
	    cvplt (((int) ps - (int) pcb) % xmax,
		   ((int) ps - (int) pcb) / xmax,
		   ((int) p  - (int) pcb) % xmax,
		   ((int) p  - (int) pcb) / xmax );
	    ps = p;
	    i = j;
	}
    } while (!(*p & resb));
}
