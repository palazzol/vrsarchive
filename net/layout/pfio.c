/***************************************************************\
*								*
*	PCB program						*
*								*
*	File I/O section					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"

FILE	*inp;			/* read/write channels 		 */

extern	FILE *fwantread();
extern  int rot_m[4][4];	/* rotation matrix		 */
extern	wantread(), wantwrite();
char *getstr(); 

float scalef;			/* sacle factor			 */
static inp_errs = 0;		/* input errors			 */


rdnl ()				/* read a netlist		 */
{
    register int i, j;

    if (getbool ("Do you want to read a checkpoint file?",1))
	rsto ();
    else {
        specs ();		/* promt for board specifications*/
	rdty ();		/* read types			 */
	rdcp ();		/* read components		 */
	rdnh ();		/* read net list		 */

	if (inp_errs)
	    err ("-Please correct earlier errors and try again", 0, 0, 0, 0);
    }
    for (j = 0, i = 0; i < V.nnh; i++)
	j += !NH[i].f;
    nettgo = j;
    nets_msg (nettgo);
}

rdty ()				/* read the type definition file */
{
    char    buf[inp_lnl], nm[nmmax + 2], ga[nmmax + 2];
    char    fname[40];
    int     i, j, k, ix, iy;
    float   x, y, x1, y1;
    static char pln[4][4] = {"GND", "VCC", "VEE", "VBB"};

    inp = fwantread (".", "test.ty", fname, "Type definition file:");
    if (!inp)
        err("-Failed to read type definition file", 0, 0, 0, 0);

    fgets (buf, inp_lnl, inp);
    if (strncmp (buf, "*types:", 7))
	err ("-Type-def file doesn't start with '*types:'", 0, 0, 0, 0);
    i = 1;

    while (fgets (buf, inp_lnl, inp)) {
	rms (buf);
	i++;			/* count lines for error msg	 */

	if (V.nty >= tymax)
	    err ("type table too small - increase 'tymax'", V.nty, i, 0, 0);

	for (j = 0; (j < nmmax) && (buf[j] != ':'); j++)
	    TY[V.nty].name[j] = buf[j];
	if (buf[j] == ':') {
	    if (buf[j + 1] != '\0')
		printf ("Line %d: warning: garbage ignored\n", i);
	    TY[V.nty].name[j] = '\0'; }
	else
	    if (buf[j + 1] != ':')
		printf ("Line %d: warning: missing ':'\n", i);

	for (j = 0; j < V.nty; j++)	/* kind of slow, but ... */
	    if (!strncmp (TY[V.nty].name, TY[j].name, nmmax))
		break;
	if (j < V.nty) {
	    printf ("Line %d: error: '%s' is already defined\n", 
		i, TY[V.nty].name);
	    inp_errs = 1;
	}

	TY[V.nty].x = 0;
	TY[V.nty].y = 0;
	TY[V.nty].np = 0;
	TY[V.nty].cif = 0;
	TY[V.nty].p = nil;

	while (fgets (buf, inp_lnl, inp)) {	/* read type body */
	    rms (buf);
	    i++;			/* count lines		 */

	    if (!strncmp (buf, "EXTCIF: ", 8)) { /*** EXTCIF ***/
		j = sscanf (&buf[8], "%d", &k);

		if (j != 1 || k < ex_cifL || k > ex_cifH || TY[V.nty].cif) {
		    printf ("Line %d: illegal 'EXTcif'\n", i);
		    inp_errs = 1;}
		else {
		    if (V.ncif >= cifmax)
			err ("EXTcif table too small: increase 'cifmax'",
			    V.ncif, i, 0, 0);
		    CIF[V.ncif].symn = k;
		    CIF[V.ncif].flg = 0;
		    CIF[V.ncif].blk = 0;
		    TY[V.nty].cif = V.ncif + 1;
		}}

	    else if (!strncmp (buf, "BOX: ", 5)) {  /*** BOX ***/
		j = sscanf (&buf[5], "%f %f", &x, &y);
		if (j != 2 || x < 0 || y < 0) {
		    printf ("Line %d: invalid 'Box' statement\n", i);
		    inp_errs = 1;}
		else {
		    ix = x * rsun * scalef + 0.5;
		    iy = y * rsun * scalef + 0.5;
		    if (TY[V.nty].x < ix)
			TY[V.nty].x = ix;
		    if (TY[V.nty].y < iy)
			TY[V.nty].y = iy;
		}}

	    else if (!strncmp (buf, "BLOCK: ", 7)) { /*** BLOCK ***/
		j = sscanf (&buf[7], "%f %f %f %f", &x, &y, &x1, &y1);
		ix = x1 * rsun * scalef + 0.5;
		iy = y1 * rsun * scalef + 0.5;
		if (j != 4 || x < 0 || x1 < 0 || y < 0 || y1 < 0 ||
		    x > x1 || y > y1 || !TY[V.nty].cif ||
		    ix > TY[V.nty].x || iy > TY[V.nty].y) {
		    printf ("Line %d: invalid 'Block' statement\n", i);
		    inp_errs = 1;}
		else {
		    if (CIF[V.ncif].blk) {	/* need additional record */
		        CIF[V.ncif].flg = 1;
			if (V.ncif >= cifmax)
			    err ("EXTcif table too small: increase 'cifmax'",
				V.ncif, i, 0, 0);
			CIF[++V.ncif].flg = 0;
			CIF[V.ncif].symn = CIF[V.ncif - 1].symn;
		    }
		    CIF[V.ncif].blk = 1;
		    CIF[V.ncif].xh = ix;
		    CIF[V.ncif].yh = iy;
		    CIF[V.ncif].xl = x * rsun * scalef + 0.5;
		    CIF[V.ncif].yl = y * rsun * scalef + 0.5;
		}}

	    else {		/*** normal pin statement	 ***/
		j = sscanf (buf, "%f %f %s %s", &x, &y, nm, ga);
		if (buf[0] == '\0')
		    break;	/* exit */

		k = 0;		/* default type			 */
	        switch (j) {
		case 1: 
		    printf ("Line %d: warning: missing y - line ignored\n",i);
		    break;
		case 4: 
		    printf ("Line %d: warning: grabage '%s...' ignored\n", i, ga);
		case 3: 
		    for (k = 0; k < 4; ++k)
			if (!strncmp (pln[k], nm, nmmax))
			    break;
		    k = (k + 1) % 5;
		case 2: 
		    if (V.npn >= pnmax)
			err ("pin table too small - increase 'pnmax'", V.npn, 0, 0, 0);
		    if (TY[V.nty].p == nil)
			TY[V.nty].p = &PN[V.npn];
		    (TY[V.nty].np)++;
		    ix = x * rsun * scalef + 0.5;
		    iy = y * rsun * scalef + 0.5;
		    if (TY[V.nty].x < ix)
			TY[V.nty].x = ix;
		    if (TY[V.nty].y < iy)
			TY[V.nty].y = iy;
		    PN[V.npn].x = ix;
		    PN[V.npn].y = iy;
		    PN[V.npn].p = k;
		    V.npn++;
		}
	    }
	}
	if (TY[V.nty].y > TY[V.nty].x)
	    printf ("Warning for '%s': y>x (text is aligned to the x-axis)\n",
		TY[V.nty].name);
	if (TY[V.nty].cif)
	    V.ncif++;
	V.nty++;
    }

    fclose (inp);
    printf("Type file: %d types with a total of %d pins defined\n",
	   V.nty, V.npn);
}

rms (b)
    char *b;
/******************************************************************\
* 								   *
*  remove stuff:						   *
*    The character string <b> is reformatted:			   *
*    1. [a-z] -> [A-Z]						   *
*    2. Comment are removed					   *
*    3. Multiple field seperators (' ', '\t' and ',') are removed  *
*    4. Field seperators will become ' '			   *
*    5. Field seperators before '\0' are removed		   *
*    6. Fileds are truncated to 'nmmax' characters		   *
* 								   *
\******************************************************************/
{
    register int i, j, k, s;

    s = 1;
    for (i = j = k = 0; i < inp_lnl; ++i) {
	if (('a' <= b[i]) && ('z' >= b[i]))
	    b[i] -= 'a' - 'A';	/* case folding 		*/
	switch (b[i]) {
	    case ' ': 
	    case '\t': 		/* single tab or space separator */
		if (!s)
		    b[j++] = ' ';
		s = 1;
		k = 0;
		break;
	    case ';': 		/* ignor comments		*/
	    case '\n':
	    case '\0':
		j -= s && j;	/* remove seperators at EOL	*/
		b[j] = '\0';
		i = inp_lnl;
		break;
	    case ',': 
		b[j++] = ' ';
		s = 1;
		k = 0;
		break;
	    default: 
		if (k > nmmax) {
		    b[j++] = ' ';
		    k = 0;}
		else {
		    s = 0;
		    b[j++] = b[i];
		    k++;}
		break;
	}
    }
}

rdcp()				/* read component file */
{
    char    buf[inp_lnl], t[nmmax + 2];
    float   x, y;
    register int i, j, k, ix, iy;
    int r;

    inp = fwantread (".", "test.cp", buf, "Component definition file:");
    if (!inp)
      err("-Failed to read component definition file", 0, 0, 0, 0);
    fgets (buf, inp_lnl, inp);

    if (strncmp (buf, "*components:", 12))
	err ("component-def file doesn't start with '*components:'", 0, 0, 0, 0);

    j = 1;

    while (fgets (buf, inp_lnl, inp)) {
	rms (buf);
	j++;			/* count lines			 */

	if (V.ncp >= cpmax)
	    err ("Component table too small: increase 'cpmax'", V.ncp, j,0,0);

	k = sscanf (buf, "%s %s %f %f %d", CP[V.ncp].name, t, &x, &y, &r);

	if (k != 5 && k != 2) {		/*** argument check    ***/
	    printf ("Line %d error: invalid argumnet count\n", j);
	    inp_errs = 1;
	    continue;
	}

	for (i = 0; i < V.ncp; i++)	/*** multi-def check  ***/
	    if (!strncmp (CP[i].name, CP[V.ncp].name, nmmax))
		break;
	if (i < V.ncp) {
	    printf ("Line %d error: multiple defined '%s'\n",
		j, CP[V.ncp].name);
	    inp_errs = 1;
	    continue;
	}

	for (i = 0; i < V.nty; i++)	/*** get type	       ***/
	    if (!strncmp (t, TY[i].name, nmmax))
		break;
	if (i >= V.nty) {
	    printf ("Line %d error: undefined Type '%s'\n", j, t);
	    inp_errs = 1;
	    continue;
	}
	CP[V.ncp].ty = &TY[i];
	CP[V.ncp].unplaced = 1;		/* not yet placed	 */

	if (V.nch + TY[i].np > chmax)
	    err ("Hole table too small, increase 'chmax'\n", chmax, j, 0, 0);

	for (i = TY[i].np; i;) {	/* allocate holes	 */
	    CH[V.nch].x = 0;
	    CH[V.nch].y = 0;
	    CH[V.nch].pn = --i;
	    CH[V.nch].cpi = V.ncp;
	    CH[V.nch++].n = nil;
	}

	if (5 == k) {		/* explicily placed component	 */
	    if ((r < 0) || (r > 3)) {
	        printf ("Line %d error: invalid rotation - line ignored\n",j);
		inp_errs = 1;
		continue;
	    }
	    ix = rsun * x * scalef + 0.5;
	    iy = rsun * y * scalef + 0.5;
	    if (!C_place (&CP[V.ncp], ix, iy, r))
		printf ("Line %d warning: invalid location, place it later\n", j);
	}
	V.ncp++;
    }

    CH_update ();		/* sort holes			 */

    fclose (inp);
    printf ("Component file: %d components with %d pins allocated\n",
	    V.ncp, V.nch);
}

rdnh()				/* read net list		*/
{
    char    buf[inp_lnl];
    register int i, j;
    int k;
    register int mode = 0;		/* =0 Multiwire, =1 DP	*/
    register struct nlhd *hptr;

    inp = fwantread (".", "test.nl", buf, "Net list file:");
    if (!inp)
        err ("-Failed to read Net list file", 0, 0, 0, 0);
    fgets (buf, inp_lnl, inp);

    if (strncmp (buf, "Cnetlist:", 9) &&
	(mode = 1, strncmp (buf, "DPnetlist:", 10)))
	err ("-component-def file doesn't start with 'C/DPnetlist:'",
		 0, 0, 0, 0);

    i = 1;
    while (fgets (buf, inp_lnl, inp)) {
	rms (buf);
	i++;

	if (mode) {
/*******************************************************************\
* 								    *
*  DP netlist reader						    *
* 								    *
*  Warning: I am too tired to completly rewrite the reader now, so  *
*           there is the limitation that a net has to fit into the  *
* 	  line buffer (buf).					    *
* 								    *
\*******************************************************************/
	    for (j = 0; buf[j] && buf[j] != '\\'; j++);
	    if (j && !buf[j]) {
		printf ("Line %d error: DP-name without '\\'\n", i);
		inp_errs = 1;
		continue;
	    }

	    if (!j)
		continue;	/* skip empty lines		 */

	    if (buf[0] == '!') {/* power net (gnd, vcc, vee, vtt) */
		if      (!strncmp ("GND", &buf[1], 3))
		    hptr = &V.GND;
		else if (!strncmp ("VCC", &buf[1], 3)) 
		    hptr = &V.VCC;
		else if (!strncmp ("VTT", &buf[1], 3))
		    hptr = &V.VTT;
		else if (!strncmp ("VEE", &buf[1], 3))
		    hptr = &V.VEE;
	        else {
		    printf ("Line %d error: unknown power net\n");
		    inp_errs = 1;
		    continue;
		}}
	    else {		/* normal net		 */
		if (V.nnh >= nhmax)
		    err ("Netlist table too small - increase 'nhmax'",
			    V.nnh, 0, 0, 0);
		buf[j] = 0;
		strncpy (NH[V.nnh].name, buf, nmmax);
		NH[V.nnh].l = 0;
		NH[V.nnh].lp = nil;
		NH[V.nnh].x1 = xmax;
		NH[V.nnh].y1 = ymax;
		NH[V.nnh].x2 = 0;
		NH[V.nnh].y2 = 0;
		NH[V.nnh].f = 0;
		hptr = &NH[V.nnh];
		V.nnh++;
	    }
	    for (j++; (j < inp_lnl) && (buf[j] != '\0'); ++j)
		if (buf[j] == ' ')
		    addn (&buf[j + 1], hptr, i);

	    continue;		/* next net			 */
	}

	
	if (buf[0] == 'C') {	/* Multiwire net list processing */
	    j = 3;
	    if (!strncmp ("GND", &buf[1], 3))
		hptr = &V.GND;
	    else if (!strncmp ("VCC", &buf[1], 3)) 
		hptr = &V.VCC;
	    else if (!strncmp ("VTT", &buf[1], 3))
		hptr = &V.VTT;
	    else if (!strncmp ("VEE", &buf[1], 3))
		hptr = &V.VEE;
	    else
		continue;	/* normal comment		 */
	} else {
	    if (!sscanf (buf, "%d", &k)) {
		printf ("Line %d error: invalid net number\n", i);
		inp_errs = 1;
		continue;}
	    else if (k == -1)	/* Multiwire end ? */
		break;
	    else if ((k < V.nnh) || (k <= 0)) {
		printf ("Line %d error: net number %d out of sequence\n",
		    i, k);
		inp_errs = 1;
		continue;}
	    else {
		if (k > V.nnh + 1)
		    printf("Line %d warning: net %d missing, next is %d\n",
			 i, V.nnh + 1, k);
		while (k > V.nnh) {/* start a new net list */
		    if (V.nnh >= nhmax)
			err ("Netlist table too small - increase 'nhmax'",
			    V.nnh, 0, 0, 0);
		    sprintf (NH[V.nnh].name, "#%d", V.nnh + 1);
		    NH[V.nnh].l = 0;
		    NH[V.nnh].lp = nil;
		    NH[V.nnh].x1 = xmax;
		    NH[V.nnh].y1 = ymax;
		    NH[V.nnh].x2 = 0;
		    NH[V.nnh].y2 = 0;
		    NH[V.nnh].f = 0;
		    hptr = &NH[V.nnh];
		    V.nnh++;
		}
	    }
	    j = 0;
	}

	for (j++; (j < inp_lnl) && (buf[j] != '\0'); ++j)
	    if (buf[j] == ' ')
		addn (&buf[j + 1], hptr, i);

    }
    fclose (inp);
    printf ("Netlist: %d nets with %d nodes allocated\n", V.nnh, V.nnl);
}

addn(s,net,ln)		/* add a node to a net list	*/
    char s[];
    struct nlhd *net;
    int ln;
{
    int          k, x1, y1;
    register int i, j, l, r, x, y;
    struct hole  *hp, *fndh ();

    for (i = 0; s[i] != '-'; ++i)	/* isolate component name	*/
	if ((s[i] == '\0') || (i > nmmax)) {
	    printf ("Line %d error: missing pin number\n", ln);
	    inp_errs = 1;
	    return;
	}
    s[i] = '\0';		/* terminate string temporarily */

    for (j = 0; j < V.ncp; j++)	/* find componet definition	*/
	if (!strncmp (s, CP[j].name, nmmax))
	    break;
    if (j >= V.ncp) {
	printf ("Line %d error: reference to undefined component\n", ln);
	inp_errs = 1;
	return;
    }

    if ((!sscanf (&s[i + 1], "%d", &k)) || (k <= 0) || (k > CP[j].ty -> np)) {
	printf("Line %d error: '%s-%d': pin-number out of range\n", ln, s, k);
	inp_errs = 1;
	return;
    }					/* check pin number		*/
    --k;				/* pin number must start at 0	*/
    if ((CP[j].ty -> p + k) -> p)
	printf ("Line %d warning: '%s-%d' is a power/gnd pin\n", ln, s, k+1);
    s[i] = '*';				/* this is a hack		*/

    if (CP[j].unplaced) {		/* need direct search		*/
	hp = nil;
	for (l = 0; l < V.nch; l++)
	    if (CH[l].cpi == j && CH[l].pn == k) {
		hp = &CH[l];
		break;
	    }
    } else {
	x = (CP[j].ty -> p + k) -> x;	/* locate hole entry in CH	*/
 	y = (CP[j].ty -> p + k) -> y;
	r = CP[j].r;
	x1 = CP[j].x + rot_m[r][0] * x + rot_m[r][1] * y;
	y1 = CP[j].y + rot_m[r][2] * x + rot_m[r][3] * y;
	hp = fndh (x1, y1);
    }

    if (hp == nil)
	err ("addn: failed to find hole", x1, y1, j, i);

    if (hp -> n != nil) {		/* multiple net node ??		*/
	printf ("Line %d error: '%s-%d' already used in other net\n",
		ln, s, k + 1);
	inp_errs = 1;
	return;
    }

    if (V.nnl >= nlmax)
	err ("net node table too small - increase 'nlmax'", V.nnl, 0, 0, 0);

    NL[V.nnl].c = hp;			/* enter node to tables		*/
    NL[V.nnl].n = net -> lp;
    NL[V.nnl].nlp = net;
    hp -> n = net -> lp = &NL[V.nnl];
    net -> l++;
    V.nnl++;
}

struct hole *fndh (x,y)			/* find hole			*/
    int x, y;
{
    register int i, j, k, xx = x, yy = y;

    i = 0;
    j = V.nch;
    while (i <= j) {
	k = (i + j) >> 1;
	if (xx > CH[k].x) {
	    i = k + 1;
	    continue; }
	if (xx < CH[k].x) {
	    j = k - 1;
	    continue; }
	if (yy > CH[k].y) {
	    i = k + 1;
	    continue; }
	if (yy < CH[k].y) {
	    j = k - 1;
	    continue; }
	return &CH[k];
    }

    return  nil;
}

save (ety)				/* save vital data structure	*/
    int ety;
{
    int     i, och;
    char    fn[30];
    static char fname[2][8] = { "pcb.SAV", "pcb.ERR"};
    struct {int r, g, b;} crc;		/* color record			*/
    struct color_tab t;
    
    och = wantwrite (".", &fname[ety][0], fn, "Checkpoint file name:", 0);

    if (!ety)
	ksv_rwnd ();			/* reset keystroke file		*/

    write (och, &V, sizeof (V));	/* write head information	*/

    for (i = 0; i < ymax; ++i)
	write (och, &pcb[i][0], xmax);
    for (i = 0; i < V.ncp; ++i)
	write (och, &CP[i], sizeof (CP[0]));
    for (i = 0; i < V.nty; ++i)
	write (och, &TY[i], sizeof (TY[0]));
    for (i = 0; i < V.npn; ++i)
	write (och, &PN[i], sizeof (PN[0]));
    for (i = 0; i < V.nch; ++i)
	write (och, &CH[i], sizeof (CH[0]));
    for (i = 0; i < V.nnh; ++i)
	write (och, &NH[i], sizeof (NH[0]));
    for (i = 0; i < V.nnl; ++i)
	write (och, &NL[i], sizeof (NL[0]));
    if (top_side != s1b) {	/* ensure normal order		 */
	    t = Color_tab[CT_s1_n];
	    Color_tab[CT_s1_n] = Color_tab[CT_s2_n];
	    Color_tab[CT_s2_n] = t;
	    t = Color_tab[CT_s1_s];
	    Color_tab[CT_s1_s] = Color_tab[CT_s2_s];
	    Color_tab[CT_s2_s] = t;
    }
    for (i = 0; i < ncolors; ++i) {
	crc.r = Color_tab[i].r;
	crc.g = Color_tab[i].g;
	crc.b = Color_tab[i].b;
	write (och, &crc, sizeof (crc));
    }
    if (top_side != s1b) {	/* restore current order	 */
	    t = Color_tab[CT_s1_n];
	    Color_tab[CT_s1_n] = Color_tab[CT_s2_n];
	    Color_tab[CT_s2_n] = t;
	    t = Color_tab[CT_s1_s];
	    Color_tab[CT_s1_s] = Color_tab[CT_s2_s];
	    Color_tab[CT_s2_s] = t;
    }
    for (i = 0; i < V.ncif; ++i)
	write (och, &CIF[i], sizeof (struct cif));

    i = mgnm;			/* mark the end of the grabage	*/
    write (och, &i, sizeof (int));
    close (och);
}

rsto()				/* Restore Checkpointed state	*/
{
    int     i, ich, old;
    char    fn[30];
    struct {int r, g, b;} crc;		/* color record			*/

    ich = wantread (".", "pcb.SAV", fn, "Checkpoint file name:");

    read (ich, &V, sizeof (V));	/* read head information	 */

    old = 0;
    if (V.pver != pversion) {
	if (V.pver == 123) 	/* conversion V 1.23 -> current	 */
	    old = 1;
	else
	    err ("-program version mismatch", V.pver, pversion, 0, 0);
    }

    if (V.errtxt[0] != '\0') {
	printf ("Warning: previous pcb run died with error message:\n");
	printf ("\t\"%s\"\n", V.errtxt);
	printf (" error-info: %d %d %d %d\n",
	    V.err_info[0], V.err_info[1], V.err_info[2], V.err_info[3]);
	V.errtxt[0] = '\0';
    }
    if ((V.x != xmax) || (V.y != ymax))
	err ("-different bit map size", V.x, V.y, xmax, ymax);
    if (V.ncp > cpmax)
	err ("-CP table too small - increase 'cpmax'", V.ncp, cpmax, 0, 0);
    if (V.nty > tymax)
	err ("-TY table too small - increase 'tymax'", V.nty, tymax, 0, 0);
    if (V.npn > pnmax)
	err ("-PN table too small - increase 'pnmax'", V.npn, pnmax, 0, 0);
    if (V.nch > chmax)
	err ("-CH table too small - increase 'chmax'", V.nch, chmax, 0, 0);
    if (V.nnh > nhmax)
	err ("-NH table too small - increase 'nhmax'", V.nnh, nhmax, 0, 0);
    if (V.nnl > nlmax)
	err ("-NL table too small - increase 'nlmax'", V.nnl, nlmax, 0, 0);
    if (V.ncif > cifmax)
	err ("-CIF table too small - increase 'cifmax'", V.ncif, cifmax,0, 0);

    for (i = 0; i < ymax; ++i)
	read (ich, &pcb[i][0], xmax);
    for (i = 0; i < V.ncp; ++i)
	read (ich, &CP[i], sizeof (CP[0]));
    for (i = 0; i < V.nty; ++i)
	read (ich, &TY[i], sizeof (TY[0]));
    for (i = 0; i < V.npn; ++i)
	read (ich, &PN[i], sizeof (PN[0]));
    for (i = 0; i < V.nch; ++i)
	read (ich, &CH[i], sizeof (CH[0]));
    for (i = 0; i < V.nnh; ++i)
	read (ich, &NH[i], sizeof (NH[0]));
    for (i = 0; i < V.nnl; ++i)
	read (ich, &NL[i], sizeof (NL[0]));
    for (i = 0; i < ncolors; i++) {
	read (ich, &crc, sizeof (crc));
	Color_tab[i].r = crc.r;
	Color_tab[i].g = crc.g;
	Color_tab[i].b = crc.b;
    }
    for (i = 0; i < V.ncif; ++i)
	    read (ich, &CIF[i], sizeof (struct cif));

    read (ich, &i, sizeof (int));	/* consistency check	 */
    if (i != mgnm)
	err ("-failed to find magic number in saved file", i, mgnm, 0, 0);

    close (ich);

    if ( (V.cp != CP) || (V.ty != TY) || (V.pn != PN) ||
         (V.ch != CH) || (V.nh != NH) || (V.nl != NL) || (V.own != &V))
	radjptr ();

    if (old)			
	convf123 ();		/* convert from version 123	 */

    if (!batch) {
	ldcmp ();
	update (0, 0, 511, 511);
	fx = fy = 0;
	window (0, 0);
    }

    if (V.cnet)			/* deselect pending nets	 */
	deseln (V.cnet);
}

radjptr ()			/* readjust pointer		 */
/************************************************************************\
* 									 *
*  If the saved file was created with an older (but similar) version of	 *
*  this program, the saved pointer may have changed due to the new	 *
*  complilation. This routine readjusts the old pointer so that they	 *
*  correspont to the new locations.					 *
* 									 *
\************************************************************************/
{
    union ptr_int {		/* allow arithmetic on pointer	*/
	unsigned i;
	struct type *ty;
	struct pin  *pn;
	struct hole *ch;
	struct nlhd *nh;
	struct nlst *nl;
        struct vtin *vi;
    } p1, p2;
    unsigned tyof, pnof, chof, nhof, nlof, viof, nh_l, nh_h;
    register int i;

    printf("Warning: pcb has been recomplied since the file was saved\n");

    p1.ty = V.ty;
    p2.ty = &TY[0];
    tyof = p2.i - p1.i;		/* get TY offset		 */

    p1.pn = V.pn;
    p2.pn = &PN[0];
    pnof = p2.i - p1.i;		/* get PN offset		 */

    p1.ch = V.ch;
    p2.ch = &CH[0];
    chof = p2.i - p1.i;		/* get CH offset		 */

    p1.nl = V.nl;
    p2.nl = &NL[0];
    nlof = p2.i - p1.i;		/* get NL offset		 */

    p1.nh = V.nh;
    p2.nh = &NH[0];
    nhof = p2.i - p1.i;		/* get NH offset		 */

    p1.vi = V.own;
    p2.vi = &V;
    viof = p2.i - p1.i;		/* get NH offset		 */

    if (tyof) {
	for (i = 0; i < V.ncp; ++i) {
	    p1.ty = CP[i].ty;
	    p1.i = p1.i ? p1.i + tyof : nil;
	    CP[i].ty = p1.ty;
	}
    }

    if (pnof) {
	for (i = 0; i < V.nty; ++i) {
	    p1.pn = TY[i].p;
	    p1.i = p1.i ? p1.i + pnof : nil;
	    TY[i].p = p1.pn;
	}
    }

    if (viof || chof || nlof || nhof) {/* adjust netlist ptr''s	 */
	p1.nh = V.nh;
	nh_l = p1.i;
	p1.nh = V.nh + V.nnh;
	nh_h = p1.i;
	for (i = 0; i < V.nnl; ++i) {
	    p1.ch = NL[i].c;
	    p1.i = p1.i ? p1.i + chof : nil;
	    NL[i].c = p1.ch;

	    p1.nl = NL[i].n;
	    p1.i = p1.i ? p1.i + nlof : nil;
	    NL[i].n = p1.nl;

	    p1.nh = NL[i].nlp;
	    if (p1.i >= nh_l && p1.i <= nh_h)
		p1.i += nhof;
	    else
	        p1.i = p1.i ? p1.i + viof : nil;
	    NL[i].nlp = p1.nh;
	}
    }

    if (nlof) {
	for (i = 0; i < V.nch; ++i) {
	    p1.nl = CH[i].n;
	    p1.i = p1.i ? p1.i + nlof : nil;
	    CH[i].n = p1.nl;
	}
	for (i = 0; i < V.nnh; ++i) {
	    p1.nl = NH[i].lp;
	    p1.i = p1.i ? p1.i + nlof : nil;
	    NH[i].lp = p1.nl;
	}
	if (V.GND.lp) {
	    p1.nl = V.GND.lp;
	    p1.i += nlof;
	    V.GND.lp = p1.nl;
	}
	if (V.VCC.lp) {
	    p1.nl = V.VCC.lp;
	    p1.i += nlof;
	    V.VCC.lp = p1.nl;
	}
	if (V.VEE.lp) {
	    p1.nl = V.VEE.lp;
	    p1.i += nlof;
	    V.VEE.lp = p1.nl;
	}
	if (V.VTT.lp) {
	    p1.nl = V.VTT.lp;
	    p1.i += nlof;
	    V.VTT.lp = p1.nl;
	}
    }
    if (nhof || nlof) {
	p1.nh = V.cnet;
	if (p1.i >= nh_l && p1.i <= nh_h)
	    p1.i += nhof;
	else
	    p1.i = p1.i ? p1.i + viof : nil;
	V.cnet = p1.nh;
	p1.nh = V.enhl;
	p1.i = p1.i ? p1.i + nhof : nil;
	V.enhl = p1.nh;
	p1.nl = V.enll;
	p1.i = p1.i ? p1.i + nlof : nil;
	V.enll = p1.nl;
    }

    V.own = &V;
    V.cp = &CP[0];
    V.ty = &TY[0];
    V.pn = &PN[0];
    V.ch = &CH[0];
    V.nh = &NH[0];
    V.nl = &NL[0];		/* done ! (hopefully)	 */

/* the next is a temporary fix to correct an old ptr- problem */
/*
    for (i = 0; i < V.nnh; i++)
        if (NH[i].l) {int j; struct nlst *p;
	    for (p = NH[i].lp, j = 0; p; p = p -> n, j++)
		if (p -> nlp != &NH[i]) {
		    printf("#");
		    p -> nlp = &NH[i];
		}
	    if (j != NH[i].l)
		err ("radjptr: inconsistent length", i, j, NH[i].l, 0);
	}
*/

}

convf123 ()
/**************************************************************\
* 							       *
*  Convert from version 1.23:				       *
* 							       *
*  Changes: a) Tool holes are no longer needed -> remove them  *
*           b) The hole-structure received a reference to the  *
*              originating hole.			       *
* 							       *
\**************************************************************/
{
    register int j, r, xp, yp;
    int i, k, x, y;
    register struct pin *pin;
    register struct hole *hp;
    struct nlst *net;
    struct hole *fndh ();	

    struct oldhole {
	int x, y;		/* coordinates		 */
	struct nlst *n;		/* net pointer		 */
    } *oh;

    printf ("Updating to PCB version 1.24\n");

    V.pver = pversion;		/* update pgm version	 */

    color (0, fb);
    for (i = 0; i < 4; i++)	/* remove tool holes	 */
	if (V.reserved[i]) {
	    x = V.reserved[i];
	    y = V.reserved[i + 4];
	    for (j = x - 3; j <= x + 3; j++)
		for (k = y - 3; k <= y + 3; k++)
		    pxl (j, k);
	}
    for (i = 0; i < 8; i++)
	V.reserved[i] = 0;

    for (i = 0; i < V.nch; i++) { /* change hole structure */
	oh = (struct oldhole *) &CH[i];
	x = oh -> x;
	y = oh -> y;
	net = oh -> n;
	CH[i].x = x;
	CH[i].y = y;
	CH[i].n = net;
	CH[i].pn = 0;
	CH[i].cpi = 0;
    }

    for (i = 0; i < V.ncp; i++) {	/* scan all components	 */
	if (CP[i].unplaced)
	    err ("Hey: V1.23 had no unplaced components!", i, 0, 0, 0);
	x = CP[i].x;			/* get borad position	 */
	y = CP[i].y;
	r = CP[i].r;
	k = CP[i].ty -> np;
	for (j = 0, pin = CP[i].ty -> p; j < k; j++, pin++) {
				/* scan component pins		 */

	    xp = x + (pin -> x) * rot_m[r][0] + (pin -> y) * rot_m[r][1];
	    yp = y + (pin -> x) * rot_m[r][2] + (pin -> y) * rot_m[r][3];

	    hp = fndh (xp, yp);	/* find hole			 */
	    if (!hp)
		err ("convt124: missing hole", xp, yp, 0, 0);
	    hp -> pn = j;
	    hp -> cpi = i;
	}
    }
}

specs()				/* promt for bord specs	 */
{
    float   x, y, getfloat();

    x = getfloat ("Width of board in cm:",0.0,100.0,0.0);
    y = getfloat ("Higth of board in cm:",0.0,100.0,0.0);
    scalef = getfloat ("Scale factor for pin coordinates:",0.01,100.0,1.0);

    if ((x < 1.0) || (y < 1.0))
	err ("-Use a pencil and paper for such a board", x, y, 0, 0);
    x *= 3.937007874;		/* step by step (rounding problem) */
    y *= 3.937007874;
    x *= rsun;
    y *= rsun;
    V.BSX = 0.5 + x;
    V.BSY = 0.5 + y;
    if ((V.BSX > xmax - 4) || (V.BSY > ymax - 4)) {
	printf ("Recompile me with 'xmax'>%d and 'ymax'>%d\n",
		V.BSX + 3, V.BSY + 3);
	err ("-bitmap too small", V.BSX, V.BSY, xmax - 4, ymax - 4);
    }

    color (fb, fb);		/* plot frame			 */
    plt (2, 1, V.BSX + 1, 1);
    plt (1, 2, 1, V.BSY + 1);
    plt (V.BSX + 2, 2, V.BSX + 2, V.BSY + 1);
    plt (2, V.BSY + 2, V.BSX + 1, V.BSY + 2);
}
