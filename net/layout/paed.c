/***************************************************************\
*								*
*	PCB program						*
*								*
*	AED related routines					*
*								*
*	(c) 1985	A. Nowatzyk				*
*								*
\***************************************************************/

#include <stdio.h>
#include <sgtty.h>
#include <time.h>
#include "pparm.h"
#include "pcdst.h"

#define ALP  6				/* enter alpha mode     */
#define DFR  111			/* draw filled rectangle*/
#define DVA  65				/* draw to absolute pos */
#define ERS  126			/* erase		*/
#define ESC  27				/* escape to command exe*/
#define ETC  51				/* enable graphic tablet*/
#define MOV  81				/* move to absolute pos */
#define RCP  106			/* read cursor address  */
#define SAP  94				/* set alpha parameters */
#define SBL  52				/* set blink		*/
#define SBR  98				/* set baud rate	*/
#define SCC  99				/* set cursor color     */
#define SCP  93				/* set cursor parameters*/
#define SCT  75				/* set color table	*/
#define SEC  67				/* set color		*/
#define SEN  71				/* set encoding scheme  */
#define SHO  102			/* set horizontal offset*/
#define SVO  101			/* set vertical offset  */
#define SWM  76				/* set write mask	*/
#define SZR  69				/* set zoom registers	*/
#define WPX  84				/* write pixel		*/
#define XYZ  64				/* cursor response char */

FILE	*fpr, *fp;			/* read/write channels  */
extern	FILE *fwantread();
	
char aed[11]="/dev/ttyxx";		/* AED 512  terminal	*/

#define trm_lud "/usr/vlsi/lib/caesar"	/* terminal look up dir */
#define trm_luf "displays"		/* terminal look up file*/
int  litout = LLITOUT;			/* Terminal I/O         */
int  sgflags, localmode;		/*      wizzardry       */
struct sgttyb sgttyb;                   /*        ...		*/
int ldisc = NTTYDISC;			/* 	    ...		*/

init()					/* initialize environement */
{
    if (batch)
	return;				/* batch doesn't need the aed	*/

    if (!select_AED())
	getstr ("Which AED are you using", "none", aed);

    fp = fopen (aed, "w");
    if (fp == NULL)
	err ("-cannot write to AED", 0, 0, 0, 0);
    fpr = fopen (aed, "r");
    if (fpr == NULL)
	err ("-cannot read from AED", 0, 0, 0, 0);

    ioctl(fileno(fp), TIOCSETD, (char *) &ldisc);
    ioctl(fileno(fp), TIOCLGET, (char *) &localmode);
    ioctl(fileno(fp), TIOCLBIS, (char *) &litout);
    gtty(fileno(fp), &sgttyb);
    sgflags = sgttyb.sg_flags;
    sgttyb.sg_flags = (sgttyb.sg_flags &
	~(RAW | CBREAK | ECHO | LCASE)) | EVENP | ODDP | CRMOD;

    sgttyb.sg_ispeed = EXTA;		/* this is 19200 baud		 */
    sgttyb.sg_ospeed = EXTA;
    putc (ESC, fp);			/* switch to interpreter mode 	 */
    putc (ESC, fp);
    putc (SBR, fp);			/* switch to 19200 baud		 */
    putc ('6', fp);
    fflush (fp);    

    stty(fileno(fp), &sgttyb);
    ioctl(fileno(fp), TIOCEXCL, (char *) &sgttyb);

    putc ('5', fp);			/* AUX-baud rate = 9600 	*/

    putc (SEN, fp);			/* set up encoding scheme	*/
    fputs ("18D8N", fp);

    putc (SCP, fp);			/* select cursor shape		*/
    putc ('L', fp);
    putc (0, fp);
    putc (mrkb, fp);			/* mark bit isn't used on aed	*/

    putc (SCC, fp);			/* select cursor color		*/
    putc (255, fp);
    putc (255, fp);
    putc (0, fp);

    color (0, all);			/* erase screen			*/
    putc (12, fp); putc (ESC, fp);
    fflush (fp);			/* wired timing problem		*/
/*    putc (ERS, fp);  */

    putc (ETC, fp);			/* disable graphic tablet	*/
    putc (0, fp);
    wx = wy = fx = fy = cx = cy = 0;
    cz = 1;
    zoom (1);
    ldcmp ();				/* load color map		*/
}

select_AED() /* select AED terminal 5 or 6 depends where you are */
{
    char   *ttynm,
           *ttyname (), buf[80];
    char    nm[30];

    ttynm = ttyname (0);
    fp = fwantread (trm_lud, trm_luf, buf, "CRT-AED association file:");
    while (fgets (buf, 80, fp)) {
	sscanf (buf, "%s %s", nm, aed);
	if (!strcmp (nm, ttynm)) {
	    fclose (fp);
	    return (1);
	}
    }
    fclose (fp);
    return (0);

}



finish()				/* clean up the mess		*/
{
    if (!batch) {

	putc (ESC, fp);			/* switch to 9600 baud		 */
	putc (SBR, fp);
	putc ('5', fp);
	fflush (fp);    

	sgttyb.sg_flags = sgflags;
	sgttyb.sg_ispeed = B9600;
	sgttyb.sg_ospeed = B9600;
	stty (fileno (fp), &sgttyb);
	ioctl (fileno (fp), TIOCNXCL, (char *) & sgttyb);
	ioctl (fileno (fp), TIOCLSET, (char *) & localmode);

	putc ('5', fp);			/* aux speed = 9600		 */
	fflush (fp);
    }

    exit (0);				/* ich mag nicht mehr !		*/
}

mvorg (x, y)				/* move display origin		*/
    int x, y;
{
    if (batch)
	return;

    y += 482/cz;
    putc (SHO, fp);
    putc (3 & (x >> 8), fp);
    putc (255 & x, fp);
    putc (SVO, fp);
    putc (3 & (y >> 8), fp);
    putc (255 & y, fp);
}

color (t, m)				/* set up color type and mask	*/
    int t, m;
{

    if (!batch) {			/* inform AED			*/
	putc (SEC, fp);
	putc (t, fp);
	putc (SWM, fp);
	putc (m, fp);
    }

    if (!t && m & s2b)
	m |= resb;			/* s2b erase: erase 'resb' also */
    else if (m == (selb | s2b) && t & s2b) {
	m |= resb;			/* paint selected s2b: add 'resb' */
	t |= resb;
    }

    ccc = t & 255 & m;			/* set current color code	*/
    ccm = ~m & 255;			/* set complement color mask	*/
}

move(x,y)				/* move to position		*/
    int x,y;
{
    putc (MOV, fp);
    xy20 (cx = x, cy = y);
}

pxl (x, y)				/* plot a point on AED and pcb  */
    int x, y;
{
    pcb[y][x] = ccc | (pcb[y][x] & ccm);
    if (!batch)
        point (x, y);
}

point (x, y)				/* plot a point on AED		*/
    int x,y;
{
    if (!batch && (x >= fx) && (y >= fy) && (x < fx + 512) && (y < fy + 512)) {
	move (x, y);
	putc (WPX, fp);
	putc (ccc, fp);
    }
}

draw(x,y)				/* draw to position		*/
    int x,y;
{
    putc (DVA, fp);
    xy20 (cx = x, cy = y);
}

xy20(x,y)				/* send converted coordinates	*/
  int x,y;
{
    putc (((x >> 4) & 0x10) | ((y >> 8) & 0x01), fp);
/*  putc (((x >> 4) & 0xf0) | ((y >> 8) & 0x0f), fp); */
    putc (x & 255, fp);
    putc (y & 255, fp);
}

c_rect (x1, y1, x2, y2)
    int x1, y1, x2, y2;
{
    register int t;

    if (x1 > x2) {t = x1; x1 = x2; x2 = t;}
    if (y1 > y2) {t = y1; y1 = y2; y2 = t;}

    x1 = (x1 < fx) ? fx : x1;		/* clipping			 */
    x2 = (x2 > fx + 511) ? fx + 511 : x2;
    y1 = (y1 < fy) ? fy : y1;
    y2 = (y2 > fy + 511) ? fy + 511 : y2;

    if (x1 > x2 || y1 > y2)
	return;				/* nothing left			 */
    rect (x1, y1, x2, y2);
}

rect (x1,y1,x2,y2)			/* draw a filled rectangle	*/
   int x1,y1,x2,y2;
/* assumption: x1 <= x2  and y1 <= y2	 */
{
    if (batch)
	return;

    if ((x1 & 0xfffffe00) != (x2 & 0xfffffe00)) {
	rect (0, y1, x2 & 511, y2);
	x1 &= 511;
	x2 = 511;
    }
    if ((y1 & 0xfffffe00) != (y2 & 0xfffffe00)) {
	rect (x1, 0, x2, y2 & 511);
	y1 &= 511;
	y2 = 511;
    }
    putc (MOV, fp);
    xy20 (x1, y1);
    putc (DFR, fp);
    xy20 (cx = x2, cy = y2);
}

zoom(n)					/* n-Zoom			*/
    int n;
{
    register int i;
    if (batch)
	return;
    msg_off ();
    i = cz;
    cz = (n > 15) ? 15 : (n < 1) ? 1 : n;
    putc (SZR, fp);
    putc (cz, fp);
    putc (cz, fp);
    i = 256 / i - 256 / cz;
    window (wx + i, wy + i);		/* update AED window		*/
}

ldcmp ()				/* load color map		*/
{
/****************************************************************\
* 								 *
*  The color table is priority ordered: first come, first used.	 *
*  It is always only one line in use. This line is choosen by	 *
*  matching the key: zero and one mask.				 *
* 								 *
\****************************************************************/
    register int     i, j;

    if (batch)
	return;

    top_side = Color_tab[CT_s1_n].O & vec;

    putc (SCT, fp);
    putc (0, fp);
    putc (0, fp);
    for (i = 0; i < 256; i++) {
	for (j = 0; j < ncolors; j++)
	    if (!(Color_tab[j].Z & i) && !(Color_tab[j].O & ~i)) {
		putc (Color_tab[j].r, fp);
		putc (Color_tab[j].g, fp);
		putc (Color_tab[j].b, fp);
		break;
	    }
    }

    for (i = 132; i < 136; ++i) {	/* select blink			*/
	putc (SBL, fp);
	putc (i, fp);
	putc (255, fp);
	putc (0, fp);
	putc (255, fp);
	putc (10, fp);
	putc (10, fp);
    }
}

window (x, y)				/* set window origin to x,y	*/
    int x, y;
{
    register int     t, tx, ty;

/*    printf ("in PAED.c: *t window: x=%d y=%d\n", x, y);    */
    if (batch)
	return;
                                                          
    msg_off ();

    if ((x + 512 / cz) > V.BSX + 3)
	x = V.BSX - 509 / cz;
    if ((y + 482 / cz) > V.BSY + 3)
	y = V.BSY - 479 / cz;
    if (x < 0)				/* Don't move window over limits */
	x = 0;
    if (y < 0)
	y = 0;

    wx = x;			/* some book keeping		 */
    wy = y;
    tx = fx;
    ty = fy;

    t = ovs;
    if (cz == 1)
	t = 0;			/* no overshot if unzoomed */

    if ((x > fx + 511) || (x + 512 / cz <= fx))
				/* its time for a new x alignment */
	tx = x + 256 / cz - 256;
    else {			/* need to shift the frame in x ? */
	if (x < fx)
	    tx = x - t;
	if (x + 512 / cz > fx + 512)
	    tx = x + 512 / cz + t - 512;
    }

    if (tx + 512 > V.BSX + 3)
	tx = V.BSX - 509;
    if (tx < 0)			/* check boundaries		 */
	tx = 0;

    if ((y > fy + 511) || (y + 512 / cz <= fy))
				/* its time for a new y alignment */
	ty = y + 256 / cz - 256;
    else {			/* need to shift the frame in y ? */
	if (y < fy)
	    ty = y - t;
	if (y + 512 / cz > fy + 512)
	    ty = y + 512 / cz + t - 512;
    }

    if (ty + 512 > V.BSY + 3)
	ty = V.BSY - 509;
    if (ty < 0)			/* clamp to boundaries		 */
	ty = 0;

    mvorg (x, y);		/* change the display origin 	 */

    if ((tx != fx) || (ty != fy))/* move AED frame if necessary  */
	frame (tx, ty);

    msg_on ();
}

frame (x, y)			/* adjust the bit map, held in the AED */
    int x, y;
{
/* printf ("in PAED.c: *t frame: x=%d y=%d\n", x, y); */
    if ((x + 512 <= fx) || (x - 512 >= fx) ||
	    (y + 512 <= fy) || (y - 512 >= fy))/* no overlap */
	update (x, y, x + 511, y + 511);

    else
	if (x == fx) {		/* simple transition */
	    if (y > fy)
		update (x, fy + 512, x + 511, y + 511);
	    else
		if (y < fy)
		    update (x, y, x + 511, fy - 1);
	}
	else
	    if (y == fy) {	/* simple transition */
		if (x > fx)
		    update (fx + 512, y, x + 511, y + 511);
		else
		    if (x < fx)
			update (x, y, fx - 1, y + 511);
	    }

	    else {		/* partial overlap */
		if (x > fx) {
		    if (y > fy) {
			update (x, fy + 512, x + 511, y + 511);
			update (fx + 512, y, x + 511, fy + 511);
		    }
		    else {
			update (x, y, x + 511, fy - 1);
			update (fx + 512, fy, x + 511, y + 511);
		    }
		}
		else {
		    if (y > fy) {
			update (x, y, fx - 1, y + 511);
			update (fx, fy + 512, x + 511, y + 511);
		    }
		    else {
			update (x, y, fx - 1, y + 511);
			update (fx, y, x + 511, fy - 1);
		    }
		}
	    }

    fx = x;
    fy = y;
}

update (x1, y1, x2, y2)		/* update a sub-bit map */
    int x1, y1, x2, y2;
{
    register int     tx, ty;

/* printf("in PAED.c: *t update: x1=%d y1=%d x2=%d y2=%d\n",x1,y1,x2,y2);  */

    if ((x1 > x2) || (y1 > y2))
	err ("Kill the programmer", x1, x2, y1, y2);

    tx = x2 & 0xfffffe00;
    ty = y2 & 0xfffffe00;
    if (x1 < tx) {		/* 2 x-partitions */
	if (y1 < ty) {		/* 2 y-partitions */
	    qupdate (tx, ty, x2, y2);
	    qupdate (x1, y1, tx - 1, ty - 1);
	    qupdate (x1, ty, tx - 1, y2);
	    qupdate (tx, y1, x2, ty - 1);
	}
	else {
	    qupdate (tx, y1, x2, y2);
	    qupdate (x1, y1, tx - 1, y2);
	}
    }
    else {
	if (y1 < ty) {		/* 2 y-paritions */
	    qupdate (x1, ty, x2, y2);
	    qupdate (x1, y1, x2, ty - 1);
	}
	else
	    qupdate (x1, y1, x2, y2);
    }
}

qupdate (x1, y1, x2, y2)	/* update contiguos sub map */
int     x1, y1, x2, y2;
{
    register int i, t, u;
    int j;
    register char   *a, *b, *c;

/* printf("in PAED.c: *t qupdate: x1=%d y1=%d x2=%d y2=%d\n",x1,y1,x2,y2);  */

    color (0, all);
    rect (x1, y1, x2, y2);	/* erase area */

    for (j = y1; j <= y2; ++j) {
/*
 *	Note: This loop is tricky. It consumes most cpu-time of the
 *	frame update process, because it has to examin up to 0.25 Mbyte
 *	for possible picture elements. It has to deal with various
 *	boarder conditions.
 *      Warning: think twice before messing arround with this!
 */
	b = &pcb[j][x1];	/* main start address of line scan */

	if (j <= y1) {		/* first line execption handler */
	    if (j >= y2)
		a = &pcb[0][0];
	    else
		a = &pcb[j + 1][x1];
	    for (i = x1 + 1; i < x2; i++) {
		t = *(b + 1) & vec & ~(*b | *(b + 2) | *a | *(a+1) | *(a+2));
		if (t) {
		    color (t, t);
		    point (i, y1);
		}
	    }
	    b = &pcb[j][x1];
	    c = &pcb[0][0];
	}
	else
	    c = &pcb[j - 1][x1];

	if (j >= y2) {		/* last line exception handler	 */
	    for (i = x1 + 1; i < x2; i++) {
		t = *(b + 1) & vec & ~(*b | *(b + 2) | *c | *(c+1) | *(c+2));
		if (t) {
		    color (t, t);
		    point (i, y2);
		}
	    }
	    a = &pcb[0][0];
	    b = &pcb[j][x1];
	    if (j <= y1)
		c = &pcb[0][0];
	    else
		c = &pcb[j - 1][x1]; }
	else
	    a = &pcb[j + 1][x1];

	if (*b) {		/* handle first pixel execptions */
	    t = *b & rec & ~(*c);
	    if (t)
		scanr (t, x1, j, x2, y2);
	    u = *b & vec;
	    if (u) {
		t = u & *(b + 1);
		if (t)
		    scanv1 (t, x1, j, x2);
		t = u & *(a + 1);
		if (t)
		    scanv2 (t, x1, j, x2, y2);
		t = u & *a & ~(*c);
		if (t)
		    scanv3 (t, x1, j, y2);
		t = u & ~(*a | *c | *(a + 1) | *(c + 1) | *(b + 1));
		if (t) {	/* capture boarder points */
		    color (t, t);
		    point (x1, j);
		}
	    }
	}
	a++;
	b++;
	c++;

	for (i = x1 + 1; i < x2; ++i) {	/* main loop ************ */
	    if (*b) {
		t = *b & rec & ~(*(b - 1)) & ~(*c);
		if (t)
		    scanr (t, i, j, x2, y2);
		u = *b & vec;
		if (u) {
		    t = u & *(b + 1) & ~(*(b - 1));
		    if (t)
			scanv1 (t, i, j, x2);
		    t = u & *(a + 1) & ~(*(c - 1));
		    if (t)
			scanv2 (t, i, j, x2, y2);
		    t = u & *a & ~(*c);
		    if (t)
			scanv3 (t, i, j, y2);
		    t = u & *(a - 1) & ~(*(c + 1));
		    if (t)
			scanv4 (t, i, j, x1, y2);
		}
	    }
	    a++;
	    b++;
	    c++;
	}

	if (*b) {		/* handle last pixel execptions	 */
	    t = *b & rec & ~(*(b - 1)) & ~(*c);
	    if (t)
		scanr (t, x2, j, x2, y2);
	    u = *b & vec;
	    if (u) {
		t = u & *a & ~(*c);
		if (t)
		    scanv3 (t, x2, j, y2);
		t = u & *(a - 1);
		if (t)
		    scanv4 (t, x2, j, x1, y2);
		t = u & ~(*(a) | *c | *(a - 1) | *(c - 1) | *(b - 1));
		if (t) {	/* capture boarder points */
		    color (t, t);
		    point (x2, j);
		}
	    }
	}

    }
}

scanr (b, x1, y1, x2, y2)		/* find outlines of rectangles */
    char b;				/* bit map (s) */
    int x1, y1, x2, y2;
{
    register int     i, j, x, y;

/*printf("in PAED.c: *t scanr: b=%d x1=%d y1=%d x2=%d y2=%d\n",b,x1,y1,x2,y2);
 */
    j = pcb[y1][x1] & selb;
    for (i = 1; i & 127; i += i)	
	if (i & b) {
	    for (x = x1; x <= x2; ++x)
		if ((pcb[y1][x] & i) == 0)
		    break;
	    for (y = y1; y <= y2; ++y)
		if ((pcb[y][x1] & i) == 0)
		    break;
	    color (i | j, i | j);
	    rect (x1, y1, --x, --y);
	}
}

scanv1 (b, x1, y1, x2)			/* find vectors 		*/
    char b;				/* bit map (s) */
    int x1, y1, x2;
{
    register int i, j, k, l, m;

/* printf ("in PAED.c: *t scanv1: b=%d x1=%d y1=%d x2=%d\n",b,x1,y1,x2); */
    k = pcb[y1][x1] & selb;
    for (i = 1; i & 127; i += i)	
	if (i & b) {
	    j = x1;
	    m = k;
	    while (((l = pcb[y1][j]) & i) && (j <= x2))
		(m &= l, ++j);
	    color (i | m, i | m);
	    move (x1, y1);
	    draw (j - 1, y1);
	}
}

scanv2 (b, x1, y1, x2, y2)		/* find vectors 		*/
    char b;				/* bit map (s) */
    int x1, y1, x2, y2;
{
    register int i, j, k, l, m;

    k = pcb[y1][x1] & selb;
    for (i = 1; i & 127; i += i)	
	if (i & b) {
	    j = 0;
	    m = k;
	    while (((l = pcb[y1+j][x1+j]) & i) && 
		   ((x1+j) <= x2) && ((y1+j)<=y2))
		(m &= l, ++j);
	    j--;
	    if ((j>2) || ( !(pcb[y1][x1+1]&i) && !(pcb[y1+1][x1] & i))) {
	        color (i | m, i | m);
	        move (x1, y1);
	        draw (x1 + j, y1 + j);}
	}
}

scanv3 (b, x1, y1, y2)			/* find vectors 		*/
    char b;				/* bit map (s) */
    int x1, y1, y2;
{
    register int     i, j, k, l, m;

/* printf ("in PAED.c: *t scanv3: b=%d x1=%d y1=%d y2=%d\n",b,x1,y1,y2);  */
    k = pcb[y1][x1] & selb;
    for (i = 1; i & 127; i += i)	
	if (i & b) {
	    j = y1;
	    m = k;
	    while (((l = pcb[j][x1]) & i) && (j <= y2))
		(m &= l, ++j);
	    color (i | m, i | m);
	    move (x1, y1);
	    draw (x1, j - 1);
	}
}

scanv4 (b, x1, y1, x2, y2)		/* find vectors 		*/
    char b;				/* bit map (s) */
    int x1, y1, x2, y2;
{
    register int     i, j, k, l, m;

    k = pcb[y1][x1] & selb;
    for (i = 1; i & 127; i += i)	
	if (i & b) {
	    j = 0;
	    m = k;
	    while (((l = pcb[y1+j][x1-j]) & i) &&
		   ((x1-j) >= x2) && ((y1+j)<=y2))
		(m &= l, ++j);
	    j--;
	    if ((j>2) || ( !(pcb[y1][x1-1]&i) && !(pcb[y1+1][x1] & i))) {
	        color (i | m, i | m);
	        move (x1, y1);
	        draw (x1 - j, y1 + j);}
	}
}

getcur (x, y)				/* get cursor			*/
    int *x, *y;
/********************************************************************\
* 								     *
*  getcur returns the real coordinates of the cursor and a code	     *
*  for the mouse-key:						     *
*    0: yellow	1: white	2: blue		3: green	     *
*    4: yellow	5: white	6: blue  	7: green	     *
*  The second line applies if the cursor was not within the window.  *
*  In case of trouble, a -1 is returned.			     *
* 								     *
\********************************************************************/
{
    char    buf[20];
    int     i, j;

    if (batch)
	err ("getcur: not allowed in batch mode", 0, 0, 0, 0);

    putc (ETC, fp);		/* enable cursor */
    putc (XYZ, fp);
    color (0, all);		/* allow cursor insertion */
    fflush (fp);		/* flush buffer */

    do {
	if (NULL == fgets (buf, 19, fpr))
	    return (-1);	/* some hard problem */
    } while (buf[0] != XYZ);

    sscanf (&(buf[1]), "%d", &i);
    if (NULL == fgets (buf, 19, fpr))
	return (-1);
    sscanf (buf, "%d", x);
    if (NULL == fgets (buf, 19, fpr))
	return (-1);
    sscanf (buf, "%d", y);

    putc (ETC, fp);		/* disable cursor */
    putc (0, fp);

    if (i < 16) {
	if (*x < (fx & 511))
	    *x += 512;
	*x += fx & 0xfffffe00;
	if (*y < (fy & 511))
	    *y += 512;
	*y += fy & 0xfffffe00;
	if (*x < wx || *y < wy ||
	    *x >= (wx + 512 / cz) || *y >= (wy + 482 / cz) ||
	    *x <= 2 || *y <= 2 || *x >= V.BSX - 1 || *y >= V.BSY - 1 )
	    i += 16;
    }

    for (j = (i & 16) ? 4 : 0; i; j++) {
	if (i & 1)
	    return (j);
	i >>= 1;
    }
    err ("getcur: something went wrong", i, j, 0, 0);

    return -1;			/* just to please lint		 */
}

atext (x, y, s, t)
/**************************************************************************\
* 									   *
*  Print the text string 's' at position <x,y> with small (t=0) or	   *
*  large (t=1) font.							   *
* 									   *
*  Note: The AED refuses to print a character over its frame buffer edge.  *
*        It will be placed entirly on the other side. This causes some	   *
*        random space inserted where x % 512 == 0. (cosmetic problem)	   *
* 									   *
\**************************************************************************/
    int x, y, t;
    char s[];
{
    register int i;
    static int  ot = -1;

    if (batch)
	return;

  /*  printf("in PAED.c: atext\n");   */
    move (x, y);		/* position text		 */

    t &= 1;
    if (ot != t) {		/* adjust character type	 */
	ot = t;
	putc (SAP, fp);
	putc ('1', fp);		/* normal size			 */
	if (t) {
	    putc ('7', fp);	/* font				 */
	    putc (8, fp);	/* horizontal spacing		 */
	    putc (0, fp); }	/* vertical spacing		 */
	else {
	    putc ('5', fp);
	    putc (6, fp);
	    putc (0, fp);
	}
	putc ('L', fp);		/* link to cusor position	 */
    }

    putc (ALP, fp);		/* enter alpha mode		 */
    if (!ot) {			/* case folding for small font	 */
	for (i = 0; s[i]; i++)
	    putc (((s[i] >= 'a') && (s[i] <= 'z')) ?
			s[i] - ('a' - 'A') : s[i], fp);}
    else
	fputs (s, fp);		/* print text			 */

    putc (ESC, fp);		/* return to graphics mode	 */
}

static char msg_buf[42];	/* status message buffer (40char)*/
static int msg_cond = 0;	/* =1 : message is on screen	 */
static int msg_cnt = 0;		/* number of characters in msg	 */

msg_off ()			/* turn message off		 */
{
    if (batch)
	return;

    if (msg_cond) {		/* is there anything to turn off?*/
	msg_cond = 0;
	color (0, resb);
	switch (cz) {
	    case 1:		/* un - zoomed			 */
		rect (wx, wy + 470, wx + 320, wy + 482);
		break;
	    case 2: 		/* zoom = 2			 */
		rect (wx, wy + 233, wx + 240, wy + 241);
		break;
	    default: 		/* ????  - erase it all		 */
		printf ("msg_off: should be off at this zoom\n");
		rect (wx, wy, wx + 512 / cz, wy + 482 / cz);
		break;
	}
    }
}

msg_on ()			/* switch messages on		 */
{
    if (batch)
	return;

    if (!msg_cond && msg_cnt > 0 && cz >= 1 && cz <= 2) {
	msg_cond = 1;		/* only if it makes sense !	 */
	color (resb, resb);
	if (cz == 1)		/* zoom=1 : large font		 */
	    atext (wx + 1, wy + 474, msg_buf, 1);
	else			/* zoom=2 : small font		 */
	    atext (wx + 1, wy + 235, msg_buf, 0);
    }
}

msg_update ()			/* update the on screen message	 */
/********************************************************\
* 							 *
*  An incremental update version may save some traffic.	 *
* 							 *
\********************************************************/
{
    msg_off ();
    msg_on ();
}

cntx_msg (s)			/* insert context name (8c max)	 */
    char *s;
{   int     i;

    if (batch)			/* pretty redundant		 */
	return;

    for (i = 0; i < 8 && *s; i++)
	msg_buf[i] = *s++;	/* copy text			 */

    if (msg_cnt <= 8) {		/* context only ?		 */
	msg_cnt = i;
	msg_buf[i] = 0;}
    else
	while (i < 8)		/* no: fill with blanks		 */
	    msg_buf[i++] = ' ';
    msg_update ();
}

nets_msg (n)			/* n nets to go			 */
    int n;
{
    if (batch)
	return;

    while (msg_cnt < 8)
	msg_buf[msg_cnt++] = ' ';
    sprintf (&msg_buf[8], " %4d", n);
    if (msg_cnt <= 14) {
	msg_cnt = 14;
	msg_buf[13] = 0;}
    else
	msg_buf[13]= ' ';
}

Ferr_msg (s)			/* forced display error messages */
    char *s;
{
    err_msg (s);
    if (!batch)
        fflush (fp);		/* flush display buffer		 */
}

err_msg (s)			/* display error messages	 */
    char *s;
{   int i;
    long clock, time ();
    char *ctime ();

    if (batch) {
	clock = time (0);
	printf ("PCB: %s -- %s", s, ctime (&clock));
	return;
    }

    if (!s) {			/* nil: clear string		 */
	if (msg_cnt > 14) {
	    msg_cnt = 14;
	    msg_buf[13] = 0;
	}}
    else {
	for (i = 0; i < 14; i++)
	    if (!msg_buf[i])
		while (i < 14)
		    msg_buf[i++] = ' ';
	for (i = 14; i < 40 && *s; i++)
	    msg_buf[i] = *s++;
	msg_buf[i] = 0;
	msg_cnt = i;
    }
    msg_update ();
}

