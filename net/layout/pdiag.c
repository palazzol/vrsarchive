/***************************************************************\
*								*
*	PCB program						*
*								*
*	Diagnostic routines					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"

test(x,y)
    int x, y;
{
    int     i, j;

    printf ("Test at x=%d y=%d\n", x, y);

    for (i = y + 3; i >= y - 3; --i) {
	printf ("\t%d:  ", i);
	for (j = -4; j <= 4; j++) {
	    if (i == y)
	    switch (j) {
		case 0: printf(">"); break;
		case 1: printf("<"); break;
		default: printf(" "); break;
	    }
	    else printf(" ");
	    printf("%2x",pcb[i][x + j] & 255);
	}
	printf ("\n");
    }
    abm_tst (x, y);
}

s_test(x,y,x1,y1,s)
    int x, y, x1, y1;
    char *s;
{
    int     i, j;
    char c;

    printf ("Test at x=%d y=%d   (..)=%s\n", x, y,s);

    for (i = y + 3; i >= y - 3; --i) {
	printf ("	%d:  ", i);
	for (j = -4; j <= 4; j++) {
	    c = ' ';
	    if (i == y1 && x + j == x1)
		    c = '(';
	    if (i == y1 && x + j == x1 + 1)
		    c = ')';
	    if (i == y && !j)
		    c = (c != ' ') ? '*' : '>';
	    if (i == y && j == 1)
		    c = (c != ' ') ? '*' : '<';

	    printf("%c%2x",c, pcb[i][x + j] & 255);
	}
	if(i == y1 && (x + j) == (x1+1))
	    printf(")");
	printf ("\n");
    }
    abm_tst (x, y);
}

tst1 ()
{
    int     x, y, sccm, sccc;
    sccm = ccm;
    sccc = ccc;
    getcur (&x, &y);
    test (x, y);
    ccc = sccc;
    ccm = sccm;
}

tst2 (x1, y1, x2, y2, c)
    int x1, y1, x2, y2, c;
{
    int     x, y;
    for (x = x1; x <= x2; ++x)
	for (y = y1; y <= y2; ++y)
	    pcb[y][x] = c;
}

verify ()			/* verify connectivity		 */
{
    int     i, j;

    for (nettgo = i = 0; i < V.nnh; ++i) {
	if (NH[i].l > 0) {
	    j = cchk (&NH[i]);
	    if (j != NH[i].f) {
		printf ("Net %d: f=%d cchk=%d\n", i, NH[i].f, j);
		NH[i].f = j;
	    }
	    nettgo += !NH[i].f;}
	else
	    printf ("Net %d is emty\n", i);
    }
    printf ("Done\n");
}

static char ksv_fn[100];	/* key stroke file name		 */

p_diagn ()			/* diagnostic menu		 */
{
    char buf[100];
    FILE *fwantwrite (), *fwantread ();
    int i, zm, win_x, win_y;
    static char *mtext[] = {
	"View",
	"Verify",
	"Clean",
	"Flip",
	"Keystroke on/off",
	"Replay"};

    switch (menu (mtext, 6)) {
	case 0:
	    tst1 ();
	    break;
	case 1:
	    verify ();
	    break;
	case 2:
	    clean ();
	    break;
	case 3:
	    flip_s();
	    break;
	case 4:
	    if (replay_fp)
		break;			/* ignore while replay		 */

	    if (save_fp) {
		err_msg ("Recording ended");
		fclose (save_fp);
		save_fp = 0;}
	    else {
		Ferr_msg ("Enter file name");
		save_fp = fwantwrite (".", "Keys.sav", ksv_fn, "New keystroke file:", 1);
		fprintf (save_fp, "%d %d %d %d\n", mgnm, cz, wx, wy);
		write_rpms ();
		err_msg ("Recording started");
	    }
	    break;
	case 5:
	    replay_fp = fwantread (".", "Keys.sav", buf, "Old keystroke file:");
	    if (4 !=fscanf (replay_fp,"%d%d%d%d", &i, &zm, &win_x, &win_y) ||
		i != mgnm) {
		err_msg ("Bad keystroke file");
		fclose (replay_fp);
		replay_fp = 0;}
	    else {
		zoom (zm);
		window (win_x, win_y);
		read_rpms ();
		err_msg ("Replaying keystrokes");
	    }
	    break;
    }
}

ksv_rwnd ()			/* keystroke file rewind	 */
{
    if (!save_fp)
	return;			/* not present			 */

    fclose (save_fp);
    save_fp = fopen (ksv_fn, "w");
    if (save_fp) {
	fprintf (save_fp, "%d %d %d %d\n", mgnm, cz, wx, wy);
	write_rpms ();}
    else {
	printf ("Keystroke file reset failed");
	beep ();
    }
}

clean ()			/* scan bitmap for wrong bits	 */
{
    register int i, j;
    struct nlhd *t;
    t = V.cnet;
    if (t)
	deseln (t);
    color (resb, resb);
    for (i = 0; i < V.BSX; i++)
	for (j = 0; j < V.BSY; j++)
	    if (pcb[j][i] & 0xe0) {
		point (i, j);
		pcb[j][i] &= 0x1f;
	    };
    Ferr_msg ("Trouble spots");
    window (0, 0);
    update (0, 0, 511, 511);
    fx = fy = 0;
    if (t)
	selnet (t);
}

flip_s ()			/* flip sides			 */
{
    register int i, j, k;

    for (i = 0; i < V.BSX; i++)
	for (j = 0; j < V.BSY; j++) {
	    k = pcb[j][i];
	    pcb[j][i] = (k & ~vec) | ((k & s2b) >> 1) | ((k & s1b) << 1);}

    update (0, 0, 511, 511);
    fx = fy = 0;
    window (0, 0);
}

qpat()
{
   TY[81].p = &PN[V.npn];

   PN[V.npn + 0].x = 0;
   PN[V.npn + 0].y = 0;
   PN[V.npn + 0].p = 0;

   PN[V.npn + 1].x = 0;
   PN[V.npn + 1].y = 12;
   PN[V.npn + 1].p = 0;

   PN[V.npn + 2].x = 0;
   PN[V.npn + 2].y = 24;
   PN[V.npn + 2].p = 0;

   PN[V.npn + 3].x = 12;
   PN[V.npn + 3].y = 24;
   PN[V.npn + 3].p = 0;

   PN[V.npn + 4].x = 12;
   PN[V.npn + 4].y = 12;
   PN[V.npn + 4].p = 0;

   PN[V.npn + 5].x = 12;
   PN[V.npn + 5].y = 0;
   PN[V.npn + 5].p = 0;

   V.npn += 6;
}
