/***************************************************************\
*								*
*	PCB program						*
*								*
*	Bitmap I/O section					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include <pwd.h>
#include "pparm.h"
#include "pcdst.h"

#define bmapw 264		/* bitmap width (in bytes)	 */
#define BMF "/usr/vlsi/tmp/PCB"	/* bitmap file prefix		 */
#define SPF1 "/usr/spool/vpd/tmp"	/* spool file prefix 	 */
#define SPF2 "/usr/spool/vpd/dfa"	/* spool file prefix	 */
#define DAE "/usr/lib/vpd"	/* spool daemon			 */


pntbm ()			/* print bit map on versatek	 */
{
    char    bname[40], sname[40];
    int     pid, bch;
    static int  cnt = 0;
    FILE *sf, *fopen ();
    extern struct passwd  *getpwuid ();

    if (V.BSY > bmapw * 4) {
	printf ("Sorry, int Bitmap too wide for single page at this scale\n");
	return;
    }

    pid = getpid ();		/* get process id		 */
    sprintf (bname, "%s%d.%d", BMF, pid, ++cnt);
    bch = creat (bname, 0x1e4);
    if (bch < 0) {
	printf ("Could not open bitmap output file\n");
	return;
    };
    wbmap (bch);		/* write the bitmap		 */
    close (bch);		/* close bit map file		 */

    sprintf (sname, "%s%d.%d", SPF1, pid, cnt);
    sf = fopen (sname, "w");
    if (sf == nil) {
	printf ("Could not open spool entry file\n");
	return;
    };
    fprintf (sf, "L%s : PC-Board preview (%5.2f by %5.2f cm)       \n",
	     getpwuid (getuid ()) -> pw_gecos,
	     (V.BSX / rsun) * 0.254, (V.BSY / rsun) * 0.254);
    fprintf (sf, "B%d components with %d holes connected by %d nets\n",
	     V.ncp, V.nch, V.nnh);
    fprintf (sf, "BCreated by PCB V%d.%d\n", V.pver / 100, V.pver % 100);
    fprintf (sf, "C%s\nU%s\n", bname, bname);
    fclose (sf);

    sprintf (bname, "%s%d.%d", SPF2, pid, cnt);
    link (sname, bname);
    unlink (sname);
    if (vfork () == 0)
	execl (DAE, "vpd", 0);
}

wbmap(bch)				/* write Bit map		 */
    int bch;
{
    char   line0[bmapw], line1[bmapw], buf0[ymax], buf1[ymax],
           *z1, *p, t0, t1;
    register char *q0, *q1, *z0;
    register int k0, k1;
    int     i, j;

    static int btab[16] = { 0x46, 0x00, 0xce, 0x88,
			    0x56, 0x10, 0xfe, 0xf8,
			    0xf6, 0x20, 0xee, 0xa8,
			    0xf6, 0x30, 0xfe, 0xf8  };

#define getbits0  k0 = btab[*q0 | (k0 & 2) | !(*p & t0)]; *(q0++) = k0 & 0xc
#define getbits1  k1 = btab[*q1 | (k1 & 2) | !(*p & t1)]; *(q1++) = k1 & 0xc

    t0 = ahb | fb | s1b;	/* layers to be plotted (solid)	 */
    t1 = s2b;			/* layers to be plotted (shaded) */

    for (i = ymax / 4; i < bmapw; ++i)
	line0[i] = line1[i] = 0;	/* pad with 0's		 */
    for (i = 0; i < ymax; ++i)
	buf0[i] = buf1[i] = 0;

    for (i = 0; i < xmax; i++) {
	p = &pcb[0][i];
	q0 = &buf0[0];
	q1 = &buf1[0];
	z0 = &line0[0];
	z1 = &line1[0];
	k0 = k1 = 0;
	for (j = 0; j < ymax; j += 4) {
	    getbits0; getbits1; p += xmax;
	    *z0 = (0xc0 & (k0 << 2)) | (0x80 & (k1 << 2));
	    *z1 = (0xc0 & k0) | (0x40 & k1);
	    getbits0; getbits1; p += xmax;
	    *z0 |= (0x30 & k0) | (0x20 & k1);
	    *z1 |= (0x30 & (k0 >> 2)) | (0x10 & (k1 >> 2));
	    getbits0; getbits1; p += xmax;
	    *z0 |= (0x0c & (k0 >> 2)) | (0x08 & (k1 >> 2));
	    *z1 |= (0x0c & (k0 >> 4)) | (0x04 & (k1 >> 4));
	    getbits0; getbits1; p += xmax;
	    *(z0++) |= (0x03 & (k0 >> 4)) | (0x02 & (k1 >> 4));
	    *(z1++) |= (0x03 & (k0 >> 6)) | (0x01 & (k1 >> 6));
	};
	write (bch, line0, bmapw);
	write (bch, line1, bmapw);
    };

#undef getbits0
#undef getbits1

}

  
