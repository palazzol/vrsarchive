/***************************************************************\
*								*
*	PCB program						*
*								*
*	Reverse Engineering and Patch Department		*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"

extern rot_m[4][4];		/* rotation matrix (in pplace)	 */

FILE *fwantread (), *fwantwrite ();

extern float scalef;		/* scale factor			 */
static int brutal;		/* =1 remove old stuff		 */

P_list ()			/* list internal structures	 */
{
     static char *mtext[] = {
	"Type file",
	"Component file",
	"Netlist" };

    switch (menu (mtext, 3)) {
	case 0:
	    TY_list ();
	    break;
	case 1:
	    CP_list ();
	    break;
	case 2:
	    NL_list ();
	    break;
    }
}

/*ARGSUSED*/
P_update (ctx)			/* update internal structures	 */
     int ctx;
{
    static char *mtext[] = {
	"Type file",
	"Component file",
	"Netupdate" };

    if (V.cnet)
	deseln (V.cnet);

    switch (menu (mtext, 3)) {
	case 0:
	    TY_update ();
	    break;
	case 1:
	    CP_update ();
	    break;
	case 2:
	    NL_update ();
	    break;
    }

    return net_sel (START);
}

TY_list ()			/* list type file		 */
{
    FILE *opf, *fwantwrite ();
    static char *ptype[] = { "GND", "Vcc", "Vee", "Vtt" };
    char fname[80];
    register int i, j, k;
    register struct pin *pin;
    float cf = 1.0 / rsun;

    opf = fwantwrite (".", "test.ty", fname, "Type def. output file:", 1);
    if (!opf) {
	err_msg ("File open problem");
	return;
    }
    Ferr_msg ("Busy: Listing types");

    fputs ("*types:\n", opf);

    for (i = 0; i < V.nty; i++) {

	if (!strcmp (TY[i].name, DELETED))
	    continue;

	fprintf (opf, "%s:\n", TY[i].name);
	if (TY[i].cif) {		/* external cif-symbol	 */
	    j = TY[i].cif - 1;
	    fprintf (opf, "EXTcif: %d\n", CIF[j].symn);
	    fprintf (opf, "Box: %.3f,%.3f\n",
		(float) TY[i].x * cf, (float) TY[i].y * cf);
	    do {
		if (CIF[j].blk)
		    fprintf (opf, "Block: %.3f,%.3f %.3f,%.3f\n",
			(float) CIF[j].xl * cf, (float) CIF[j].yl * cf,
			(float) CIF[j].xh * cf, (float) CIF[j].yh * cf );}
	    while (CIF[j++].flg);
	}
	for (pin = TY[i].p, j = 1; TY[i].np >= j; pin++, j++) {
	    if (k = pin -> p)
		fprintf (opf, "%.3f,%.3f\t%s\n",
		    (float) (pin -> x) * cf, (float) (pin -> y) * cf,
		    ptype[--k]);
	    else
		fprintf (opf, "%.3f,%.3f\tpin%d\n",
		    (float) (pin -> x) * cf, (float) (pin -> y) * cf, j);
	}
	fputs ("\n", opf);
    }
    fclose (opf);
    err_msg ("Done");
    beep ();
}

CP_list ()			/* list component file		 */
{
    FILE *opf, *fwantwrite ();
    char fname[80];
    register int i;
    float cf = 1.0 / rsun;

    opf = fwantwrite (".", "test.cp", fname, "Component output file:", 1);
    if (!opf) {
	err_msg ("File open problem");
	return;
    }
    Ferr_msg ("Busy: Listing components");

    fputs ("*components:\n", opf);

    for (i = 0; i < V.ncp; i++) {

	if (!strcmp (CP[i].name, DELETED))
	    continue;

	if (CP[i].unplaced)
	    fprintf (opf, "%s\t%s\t\t; ** unplaced **\n",
		CP[i].name, CP[i].ty -> name);
	else
	    fprintf (opf, "%s\t%s\t%.3f,%.3f\t%d\n",
		CP[i].name, CP[i].ty -> name, (float) CP[i].x * cf,
		 (float) CP[i].y * cf, CP[i].r);
    }

    fclose (opf);
    err_msg ("Done");
    beep ();
}

static int nl_type;		/* 0: Multiwire   1: DP		 */
static FILE *opf;

NL_list ()			/* list netlist			 */
{
    FILE *fwantwrite ();
    char fname[80];
    register int i;

    if (!placed) {
	err_msg ("Place components first");
	return;
    }

    opf = fwantwrite (".", "test.nl", fname, "Netlist output file:", 1);
    if (!opf) {
	err_msg ("File open problem");
	return;
    }
    Ferr_msg ("Busy: Printing Netlist");

    nl_type = NH[0].name[0] != '#';	/* decide on format	 */

    if (nl_type)
	fputs ("DPnetlist:\n", opf);
    else
	fputs ("Cnetlist:\n", opf);

    pnt_net (&V.GND, 1);		/* print special nets	 */
    pnt_net (&V.VCC, 1);
    pnt_net (&V.VEE, 1);
    pnt_net (&V.VTT, 1);

    for (i = 0; i < V.nnh; i++)		/* print normal nets	 */
	pnt_net (&NH[i], 0);

    fclose (opf);
    err_msg ("Done");
    beep ();
}

pnt_net (net, sp)		/* print a net			 */
    struct nlhd *net;
    int sp;
{
    register int i;
    register struct nlst *p;
    static int nc;		/* net counter for MW-format	 */

    if (!(net -> l))		/* skip empty nets		 */
	return;

    nc++;
    if (sp)			/* reset net counter		 */
	nc = 0;

    for (p = net -> lp, i = 0; p; p = p -> n, i++) {/* scan net	 */

	if (!(i & 7)) {		/* denote net			 */

	    if (nl_type) {	/* DP-type names		 */
		if (sp) {
		    if (i)
			fputc ('\n', opf);
		    fprintf (opf, "!%s\\", net -> name);}
		else if (!i)
		    fprintf (opf, "%s\\", net -> name);}

	    else {		/* Multi-wire names		 */
		if (i)
		    fputc ('\n', opf);
		if (sp)
		    fprintf (opf, "C%s\t", net -> name);
		else
		    fprintf (opf, "%d\t", nc);
	    }
	}

	fprintf (opf, " %s-%d", CP[p -> c -> cpi].name, p -> c -> pn + 1);
    }

    fputc ('\n', opf);
}

static struct type new_ty;	/* temporary place for a type def	 */
static struct pin *new_pin = 0;
static struct cif *new_cif = 0;
static int npn_max = 100, ncf_max = 50, lcnt, cif_cnt;
static FILE *inp;

TY_update ()
/**************************************************************************\
* 									   *
*  Update type file:							   *
*   The stored type definitions are compared those supplied by the	   *
*   file. Updates will be made only if the particular type is not in use.  *
* 									   *
\**************************************************************************/
{
    char    fname[80], *flgs, buf[80];
    register int i;
    float getfloat ();

    inp = fwantread (".", "test.ty", fname, "New type definition file:");
    if (!inp) {
        err_msg ("Open failed");
	return;
    }

    scalef = getfloat ("Scale factor for pin coordinates", 0.01, 100.0, 1.0);
    brutal = getbool ("Brutal update", 0);

    fgets (buf, inp_lnl, inp);
    if (strncmp (buf, "*types:", 7)) {
	printf ("Type-def file doesn't start with '*types:'");
	err_msg ("Format error");
	beep ();
	fclose (inp);
	return;
    }

    flgs = (char *) malloc (sizeof (char) * tymax);
    for (i = 0; i < V.nty; flgs[i++] = 0);

    lcnt = 1;			/* reset line counter		 */
    Ferr_msg ("Busy: type update");

    while (i = get_ty ()) {
	if (i < 0)
	    printf ("Skipping defective type definition\n");
	else {
	    for (i = 0; i < V.nty; i++)	/* try to find old def	 */
		if (!strncmp (TY[i].name, new_ty.name, nmmax))
		    break;

	    if (i >= V.nty) {
		printf ("Adding '%s'\n", new_ty.name);
		add_TY ();	/* add new type			 */
		flgs[V.nty - 1] = 1;}
	    else {
		if (flgs[i]) {
		    printf ("Line%d: '%s' multiply defined\n",
			lcnt, new_ty.name);
		    continue;
		}
		flgs[i] = 1;	/*  this entry is referenced	 */
		if (neq_TY (&TY[i])) {
		    if (replace_TY (&TY[i]))
			printf ("New definition for '%s' loaded\n",
			    new_ty.name);
		    else
			printf ("Cannot update '%s': in use\n",
			    new_ty.name);
		}
	    }
	}
    }

    for (i = 0; i <V.nty; i++)
	if (!flgs[i] && strcmp (DELETED, TY[i].name)) {
	    strncpy (buf, TY[i].name, nmmax);
	    if (del_TY (&TY[i]))
		printf ("Type '%s' deleted\n", buf);
	    else
		printf ("Cannot delete '%s': in use\n", buf);
	}		

    free (flgs);		/* clean up & exit		 */
    fclose (inp);
    err_msg ("Done");
    beep ();
}

get_ty ()			/* get a new type		 */
{
    char    buf[inp_lnl], nm[nmmax + 2], ga[nmmax + 2];
    register int j, k, ix, iy, error = 0;
    int i;
    float   x, y, x1, y1;
    static char pln[4][4] = {"GND", "VCC", "VEE", "VBB"};

    if(!fgets (buf, inp_lnl, inp))
	return 0;		/* no more types		 */

    rms (buf);
    lcnt++;			/* count lines for error msg	 */

    for (j = 0; (j < nmmax) && (buf[j] != ':'); j++)
	new_ty.name[j] = buf[j];
    if (buf[j] == ':') {
	if (buf[j + 1] != '\0')
	    printf ("Line %d: warning: garbage ignored\n", lcnt);
	new_ty.name[j] = '\0'; }
    else
	if (buf[j + 1] != ':')
	    printf ("Line %d: warning: missing ':'\n", lcnt);

    new_ty.x = 0;
    new_ty.y = 0;
    new_ty.np = 0;
    new_ty.cif = 0;
    new_ty.p = nil;

    cif_cnt = 0;

    while (fgets (buf, inp_lnl, inp)) {	/* read type body	 */
	rms (buf);
	lcnt++;			/* count lines			 */

	if (!strncmp (buf, "EXTCIF: ", 8)) { /*** EXTCIF       ***/
	    j = sscanf (&buf[8], "%d", &i);

	    if (j != 1 || i < ex_cifL || i > ex_cifH || new_ty.cif) {
		printf ("Line %d: illegal 'EXTcif'\n", lcnt);
		error = 1;}
	    else {
		if (!new_cif) 	/* need to allocate cif tab	 */
		    new_cif = (struct cif *) malloc
				(sizeof (struct cif) * ncf_max);
		else if (cif_cnt >= ncf_max) {	/* too small	 */
		    ncf_max += 10 + ncf_max / 2;
		    new_cif = (struct cif *) realloc
				(new_cif, sizeof (struct cif) * ncf_max);
		}

		new_cif[cif_cnt].symn = i;
		new_cif[cif_cnt].flg = 0;
		new_cif[cif_cnt].blk = 0;
		new_ty.cif = 1;
	    }}

	else if (!strncmp (buf, "BOX: ", 5)) {  /*** BOX ***/
	    j = sscanf (&buf[5], "%f %f", &x, &y);
	    if (j != 2 || x < 0 || y < 0) {
		printf ("Line %d: invalid 'Box' statement\n", lcnt);
		error = 1;}
	    else {
		ix = x * rsun * scalef + 0.5;
		iy = y * rsun * scalef + 0.5;
		if (new_ty.x < ix) new_ty.x = ix;
		if (new_ty.y < iy) new_ty.y = iy;
	    }}

	else if (!strncmp (buf, "BLOCK: ", 7)) { /*** BLOCK ***/
	    j = sscanf (&buf[7], "%f %f %f %f", &x, &y, &x1, &y1);
	    ix = x1 * rsun * scalef + 0.5;
	    iy = y1 * rsun * scalef + 0.5;
	    if (j != 4 || x < 0 || x1 < 0 || y < 0 || y1 < 0 ||
		x > x1 || y > y1 || !new_ty.cif ||
		ix > new_ty.x || iy > new_ty.y) {
		printf ("Line %d: invalid 'Block' statement\n", lcnt);
		error = 1;}
	    else {
		if (new_cif[cif_cnt].blk) {	/* need additional record */
		    new_cif[cif_cnt].flg = 1;
		    if (cif_cnt >= ncf_max) {
			ncf_max += 10 + ncf_max / 2;
			new_cif = (struct cif *) realloc
				(new_cif, sizeof (struct cif) * ncf_max);
		    }
		    new_cif[++cif_cnt].flg = 0;
		    new_cif[cif_cnt].symn = new_cif[0].symn;
		}
		new_cif[cif_cnt].blk = 1;
		new_cif[cif_cnt].xh = ix;
		new_cif[cif_cnt].yh = iy;
		new_cif[cif_cnt].xl = x * rsun * scalef + 0.5;
		new_cif[cif_cnt].yl = y * rsun * scalef + 0.5;
	    }}

	else {			/*** normal pin statement      ***/
	    j = sscanf (buf, "%f %f %s %s", &x, &y, nm, ga);
	    if (buf[0] == '\0')
		break;		/* end of one definition	 */

	    k = 0;		/* default type			 */
	    switch (j) {
		case 1: 
		    printf ("Line %d: missing y - line ignored\n", lcnt);
		    error = 1;
		    break;
		case 4: 
		    printf ("Line %d: grabage '%s..' ignored\n", lcnt, ga);
		    error = 1;
		    break;
		case 3: 
		    for (k = 0; k < 4; ++k)
			if (!strncmp (pln[k], nm, nmmax))
			    break;
		    k = (k + 1) % 5;
		case 2:
		    if (!new_pin)
			new_pin = (struct pin *) malloc (
					sizeof (struct pin) * npn_max);
		    else if (new_ty.np >= npn_max) {
			npn_max += 10 + npn_max / 2;
			new_pin = (struct pin *) realloc (new_pin,
					sizeof (struct pin) * npn_max);
		    }

		    new_ty.p = new_pin;
		    ix = x * rsun * scalef + 0.5;
		    iy = y * rsun * scalef + 0.5;
		    if (new_ty.x < ix) new_ty.x = ix;
		    if (new_ty.y < iy) new_ty.y = iy;
		    new_pin[new_ty.np].x = ix;
		    new_pin[new_ty.np].y = iy;
		    new_pin[new_ty.np++].p = k;
	    }
	}
    }

    if (new_ty.y > new_ty.x)
	printf ("Warning for '%s': y>x (text is aligned to the x-axis)\n",
		 new_ty.name);

    return error ? -1 : 1;
}

add_TY ()			/* add new type definition	 */
{
    register int i;

    if (V.ncif + cif_cnt >= cifmax)
	err ("EXTcif table too small: increase 'cifmax'", cifmax, 0, 0, 0);
    if (V.npn + new_ty.np > pnmax)
	err ("Pin table too small: increase 'pnmax'", pnmax, 0, 0, 0);
    if (V.nty > tymax)
	err ("Type table too small: increase 'tymax'", tymax, 0, 0, 0);

    new_ty.p = &PN[V.npn];	/* adjust pointer and index	 */

    if (new_ty.cif)
	new_ty.cif = V.ncif + 1;

    TY[V.nty++] = new_ty;	/* enter type			 */

    for (i = 0; i < new_ty.np; i++)	/* copy pins		 */
	PN[V.npn++] = new_pin[i];

    if (new_ty.cif) {		/* copy cif records		 */
	for (i = 0; i <= cif_cnt; i++)
	    CIF[V.ncif++] = new_cif[i];
    }
}

neq_TY (old)
    struct type *old;
/************************************************************************\
* 									 *
*  .NE. types:								 *
*   This function comapres 2 type definitions and returns a '1' if they	 *
*   differ. The primary reason of difference is printed.		 *
* 									 *
*   Note: the second type is hardwired to 'new_ty'			 *
* 									 *
\************************************************************************/
{
    register int i, j;

    if (new_ty.np != old -> np) {
	printf ("Type '%s' - number of pins: old=%d new=%d\n",
	    new_ty.name, old -> np, new_ty.np);
	return 1;
    }

    for (i = 0; i < new_ty.np; i++) {
	if (new_pin[i].x != (old -> p)[i].x ||
	    new_pin[i].y != (old -> p)[i].y   ) {
	    printf ("Type '%s' - pin%d has new position\n", new_ty.name, ++i);
	    return 1;
	}
	if (new_pin[i].p != (old -> p)[i].p) {
	    printf ("Type '%s' - pin%d has new type\n", new_ty.name, ++i);
	    return 1;
	}
    }

    if (!new_ty.cif != !old -> cif) {
	printf ("Type '%s' - type: old=%s new=%s\n", new_ty.name,
	    old -> cif ? "EXTcif" : "normal",
	    new_ty.cif ? "EXTcif" : "normal");
	return 1;
    }

    if (new_ty.cif) {
	i = 0; 
	j = old -> cif - 1;
	do {
	    if (new_cif[i].symn != CIF[j].symn) {
		printf ("Type '%s' - CIF number: old=%d new=%d\n",
		    new_ty.name, CIF[j].symn, new_cif[i].symn);
		return 1;
	    }
	    if (new_cif[i].blk != CIF[j].blk) {
		printf ("Type '%s' - different number of blocks\n",
		    new_ty.name);
		return 1;
	    }
	    if (new_cif[i].blk && (
		new_cif[i].xl != CIF[j].xl ||
		new_cif[i].xh != CIF[j].xh ||
		new_cif[i].yl != CIF[j].yl ||
		new_cif[i].yh != CIF[j].yh    )) {
		printf ("Type '%s' - different block dimensions\n",
		    new_ty.name);
		return 1;
	    }
	    j++;}
        while (new_cif[i++].flg);
    }

    return 0;			/* they match			 */
}

replace_TY (old)
    struct type *old;
/**************************************************************************\
* 									   *
*  Replace a type definition:						   *
*   The old type definition <old> is replaced by the new one in "new_ty".  *
*   A '1' is returned if the operation succeeds. A returned '0' indicates  *
*   that the type old type is in use and that no change was made.	   *
* 									   *
\**************************************************************************/
{
    register int i, j, new_p = 0, new_c = 0;
    register struct pin *p;

    for (i = 0; i < V.ncp; i++)
	if (CP[i].ty == old) {		/* is in use!		 */
	    if (brutal) {
		printf ("Deleting component '%s'\n", CP[i].name);
		del_CP (&CP[i]);}
	    else
	        return 0;
	}

    if (old -> np < new_ty.np)
	new_p = 1;			/* re-allocate pins	 */

    j = 0;				/* count old CIF records */
    if (old -> cif) {
	i = old -> cif - 1;
	do j++;
	while (CIF[i++].flg);
    }

    if (cif_cnt > j || !old -> cif)
	new_c = 1;			/* re-allocate cif stuff */

    if (new_c && V.ncif + cif_cnt > cifmax)
	err ("EXTcif table too small: increase 'cifmax'", cifmax, 0, 0, 0);
    if (new_p && V.npn + new_ty.np > pnmax)
	err ("Pin table too small: increase 'pnmax'", pnmax, 0, 0, 0);

    new_ty.p = new_p ? &PN[V.npn] : old -> p;
				/* adjust pointer and index	 */
    if (new_ty.cif)
	new_ty.cif = new_c ? V.ncif + 1 : old -> cif;

    *old = new_ty;		/* enter type			 */

    if (new_p) {		/* copy pins			 */
	for (i = 0; i < new_ty.np; i++)
	    PN[V.npn++] = new_pin[i];}
    else {
	for (i = 0, p = new_ty.p; i < new_ty.np; i++)
	    *(p++) = new_pin[i];
    }

    if (new_ty.cif) {		/* copy cif records		 */
	for (i = 0, j = new_ty.cif - 1; i <= cif_cnt; i++)
	    CIF[j++] = new_cif[i];
	if (new_c)
	    V.ncif = j;
    }

    return 1;
}    

del_TY (old)
    struct type *old;
/*************************************************************\
* 							      *
*  Delete a type definition:				      *
*   The type definition will be made inaccessable (the input  *
*   filter 'rms' does not pass lower case characters. The     *
*   space is not freed! A returned '0' indicates that the     *
*   delete failed because the Type definition is in use.      *
* 							      *
\*************************************************************/
{
    register int i;

    for (i = 0; i < V.ncp; i++)
	if (CP[i].ty == old)		/* is in use!		 */
	    return 0;

    strcpy (old -> name, DELETED);	/* Note: lower case letters	*/

    return 1;
}

static struct comp new_cp;

CP_update ()			/* update component file	 */
{
    char    buf[80], *flgs;
    float   getfloat ();
    register int i;

    inp = fwantread (".", "test.cp", buf, "Component definition file:");
    if (!inp) {
        err_msg ("open failed");
	return;
    }

    fgets (buf, inp_lnl, inp);
    if (strncmp (buf, "*components:", 12)) {
	err_msg ("Format error");
	printf ("Component-def file doesn't start with '*components:'\n");
	beep ();
	fclose (inp);
	return;
    }

    scalef = getfloat ("Scale factor for componnet coordinates",
			0.01, 100.0, 1.0);
    brutal = getbool ("Brutal update", 0);

    flgs = (char *) malloc (sizeof (char) * cpmax);
    for (i = 0; i < V.ncp; flgs[i++] = 0);

    lcnt = 1;			/* reset line counter		 */

    Ferr_msg ("Busy: updating components");

    while (i = get_CP ()) {
	if (i < 0) {
	    printf ("Defect component skipped\n");
	    continue;
	}
	for (i = 0; i < V.ncp; i++)
	    if (!strncmp (CP[i].name, new_cp.name, nmmax))
		break;
	if (i >= V.ncp) {	/* add new component		 */
	    printf ("Adding '%s'\n", new_cp.name);
	    add_CP ();
	    flgs[V.ncp - 1] = 1;}
	else {
	    if (flgs[i]) {
		printf ("Line %d: '%s' multiple defined\n",
		    lcnt, new_cp.name);
		continue;
	    }
	    flgs[i] = 1;	/* flag reference		 */

	    if (neq_CP (&CP[i])) {
		if (replace_CP (&CP[i]))
		    printf ("New definition for '%s' loaded\n", new_cp.name);
		else
		    printf ("Cannot update '%s': in use\n", new_cp.name);
	    }
	}
    }

    for (i = 0; i <V.ncp; i++)
	if (!flgs[i] && strcmp (CP[i].name, DELETED)) {
	    strncpy (buf, CP[i].name, nmmax);
	    if (del_CP (&CP[i]))
		printf ("Component '%s' deleted\n", buf);
	    else
		printf ("Cannot delete '%s': in use\n", buf);
	}		

    placed = 0;			/* revaluate net location	 */

    free (flgs);		/* clean up & exit		 */
    fclose (inp);
    err_msg ("Done");
    beep ();
}

get_CP ()
/*********************************************************************\
* 								      *
*  Get a new component						      *
*   A returned '-1' indicates a some trouble, '1' success and '0' EOF *
* 								      *
\*********************************************************************/
{
    char    buf[inp_lnl], t[nmmax + 2];
    float   x, y;
    register int i, j;
    int r;

    if (!fgets (buf, inp_lnl, inp))
	return 0;		/* done				*/

    rms (buf);
    lcnt++;			/* count lines			 */

    j = sscanf (buf, "%s %s %f %f %d", new_cp.name, t, &x, &y, &r);

    if (j != 5 && j != 2) {	/*** argument check	       ***/
	printf ("Line %d error: invalid argument count\n", lcnt);
	return -1;
    }

    for (i = 0; i < V.nty; i++)	/*** get type	      	       ***/
	if (!strncmp (t, TY[i].name, nmmax))
	    break;
    if (i >= V.nty) {
	printf ("Line %d error: undefined Type '%s'\n", lcnt, t);
	return -1;
    }
	
    new_cp.ty = &TY[i];
    new_cp.unplaced = 1;	/* not yet placed		 */

    if (5 == j) {		/* explicily placed component	 */
	if ((r < 0) || (r > 3)) {
	    printf ("Line %d error: invalid rotation - line ignored\n",lcnt);
	    return -1;
	}
	new_cp.x = rsun * x * scalef + 0.5;
	new_cp.y = rsun * y * scalef + 0.5;
	new_cp.r = r;
        new_cp.unplaced = 0;	/* a wild assumption ...	 */
    }

    return 1;
}

add_CP ()
/**********************************************************************\
* 								       *
*  Try to add a new component					       *
*   The only error condition is lack of table space. If the componnet  *
*   does not fit, it will handled as unplaced.			       *
* 								       *
\**********************************************************************/
{
    register int i;

    if (V.ncp >= cpmax)
	err ("Component table too small: increase 'cpmax'", cpmax, 0, 0, 0);
    if (V.nch + new_cp.ty -> np > chmax)
	err ("Component hole table too small: increase 'chmax'", chmax,0,0,0);

    for (i = new_cp.ty -> np; i;) {	/* allocate holes	 */
	CH[V.nch].x = 0;
	CH[V.nch].y = 0;
	CH[V.nch].pn = --i;
	CH[V.nch].cpi = V.ncp;
	CH[V.nch++].n = nil;
    }

    CP[V.ncp++] = new_cp;	/* insert component		 */

    if (!new_cp.unplaced) {	/* try to make it happen	 */
	CP[V.ncp - 1].unplaced = 1;
	i = new_cp.r;
	if (!C_place (&CP[V.ncp - 1], new_cp.x, new_cp.y, i))
	    printf ("Line %d: cannot place '%s' - kept as unplaced\n",
		lcnt, new_cp.name);
    }

    CH_update ();
}

neq_CP (old)
   struct comp *old;
/************************************************************************\
* 									 *
*  .NE. new component							 *
*    a '1' is returned if the new componet "new_cp" differs from <old>.	 *
*    The first difference is printed.					 *
* 									 *
\************************************************************************/
{
    if (new_cp.ty != old -> ty) {
	printf ("Component '%s' - type difference: old='%s' new='%s'\n",
	    new_cp.name, old -> ty -> name, new_cp.ty -> name);
	return 1;
    }

    if (!new_cp.unplaced && old -> unplaced) {
	printf ("Component '%s' - status difference: old is unplaced\n",
	    new_cp.name);
	return 1;
    }

    if (!new_cp.unplaced && (
	new_cp.x != old -> x ||
	new_cp.y != old -> y ||
	new_cp.r != old -> r   )) {
	printf ("Component '%s' - different position\n", new_cp.name);
	return 1;
    }

    return 0;			/* they match			 */
}

replace_CP (old)
    struct comp *old;
/**************************************************************************\
* 									   *
*  Try to replace a component						   *
*   If the componnet has connections to any net, the replace attempt will  *
*   have no effect and a '0' is returned. A '1' indicates a successfull	   *
*   replacement, but the new component may be 'unplaced'		   *
* 									   *
\**************************************************************************/
{
    register int i, j, own, cc;
    register struct hole *hp, *hp1;
    struct nlhd *net;

    own = ((int) old - (int) CP) / sizeof (struct comp); /* get own index */
    for (i = V.nch, hp = CH; i; i--, hp++)	/* 'use' check	 */
	if (hp -> n && hp -> cpi == own) {
	    if (brutal) {
		net = hp -> n -> nlp;
		printf ("Deleting net '%s'\n", net -> name);
		del_NH (net);}
	    else
	        return 0;	/* component is referenced	 */
	}

    /***** ready to replace it				     *****/

    if (!(old -> unplaced)) {	/* un-place component first	 */
	if (!C_unplace (old))
	    err ("replace_CP: unplace failed with no nets", own, 0, 0, 0);
    }

    cc = V.nch + new_cp.ty -> np - old -> ty -> np; 
				/* expected new hole count	 */
    if (cc > chmax)
	err ("Hole table too small: increase 'chmax'", chmax, 0, 0, cc);

    for (i = new_cp.ty -> np, j = V.nch, hp = CH; i && j; j--, hp++)
	if (hp -> cpi == own) {	/* found a pin			 */
	    hp -> x = 0;
	    hp -> y = 0;
	    hp -> pn = --i;
	    hp -> n = nil;	/* x, y and n should not change here */
	}
    if (i) {			/* need to allocate more pins */
	while (i) {
	    CH[V.nch].x = 0;
	    CH[V.nch].y = 0;
	    CH[V.nch].pn = --i;
	    CH[V.nch].cpi = own;
	    CH[V.nch++].n = nil;
	}}
    else if (j) {		/* there may be dangling pins */
	for (hp1 = hp; j; j--, hp++) {
	    if (hp -> cpi != own)
		*(hp1++) = *hp;
	    else
		V.nch--;
	}
    }

    if (cc != V.nch)		/* something went wrong		 */
	err ("remove_CP: inconsistent hole count", cc, V.nch, own, 0);

    *old = new_cp;		/* insert new component		 */

    if (!new_cp.unplaced) {	/* try to make it happen	 */
	old -> unplaced = 1;
	i = new_cp.r;
	if (!C_place (old, new_cp.x, new_cp.y, i))
	    printf ("Line %d: cannot place '%s' - kept as unplaced\n",
		lcnt, new_cp.name);
    }

    CH_update ();

    return 1;			/* success !			 */
}

del_CP (old)
    struct comp *old;
/*******************************************************************\
* 								    *
*  Delete component:						    *
*   The holes for the componen <old> are removed and the CP entry   *
*   is marked as deleted iff <old> is not in use. '1' indicates	    *
*   sucessfull deletion and '0' signals that no delete took place.  *
*   The as in 'del_TY' the space is not freed (so far).		    *
* 								    *
\*******************************************************************/
{
    register int i, j, own, cc;
    register struct hole *hp, *hp1;
    struct nlhd *net;

    own = ((int) old - (int) CP) / sizeof (struct comp); /* get own index */
    for (i = V.nch, hp = CH; i; i--, hp++)	/* 'use' check	 */
	if (hp -> n && hp -> cpi == own) {
	    if (brutal) {
		net = hp -> n -> nlp;
		printf ("Deleting net '%s'\n", net -> name);
		del_NH (net);}
	    else
	        return 0;	/* component is referenced	 */
	}

    /***** ready to delete it				     *****/

    if (!(old -> unplaced)) {	/* un-place component first	 */
	if (!C_unplace (old))
	    err ("del_CP: unplace failed with no nets", own, 0, 0, 0);
    }

    cc = V.nch - old -> ty -> np; /* expected new hole count	 */

    for (j = V.nch, hp1 = hp = CH; j; j--, hp++) {
	if (hp -> cpi != own)
	    *(hp1++) = *hp;
	else
	    V.nch--;
    }

    if (cc != V.nch)		/* something went wrong		 */
	err ("del_CP: inconsistent hole count", cc, V.nch, own, 0);

    strcpy (old -> name, DELETED);	/* make it inaccessable  */
    old -> ty = 0;		/* free types			 */
    old -> unplaced = 0;	/* do not fool place functions	 */

    CH_update ();

    return 1;			/* all set			 */
}

static int mode;		/* =0: Multiwire =1: DP netlist	 */
static struct nlhd new_nh;	/* temprary net list head	 */
static struct hole **new_ht = 0;/* temporary hole table		 */
static int ht_max = 100;	/* max size of new_ht		 */
static int ht_cnt;		/* allocation counter		 */
static struct nlhd *power_n;	/* != 0 : power net processing	 */
static int net_cnt;		/* net counter (MW format)	 */

NL_update ()			/* update netlist		 */
{
    char    buf[80];
    float   getfloat ();
    register int i, err = 0;
    register struct nlst *p, *q;

    inp = fwantread (".", "test.nl", buf, "Netlist file:");
    if (!inp) {
        err_msg ("Open failed");
	return;
    }

    mode = 0;			/* assume 'Multiwire' mode		 */
    fgets (buf, inp_lnl, inp);
    if (strncmp (buf, "Cnetlist:", 9) &&
	(mode = 1, strncmp (buf, "DPnetlist:", 10))) {
	err_msg ("Format error");
	printf ("Component-def file doesn't start with '*components:'\n");
	beep ();
	fclose (inp);
	return;
    }

    for (i = 0; i < V.nnh; NH[i++].ref = 0);	/* clear ref-flags */

    for (p = V.GND.lp; p; p = p -> n)	/* clear GND net	 */
	p -> mk = 0;
    for (p = V.VCC.lp; p; p = p -> n)	/* clear VCC net	 */
	p -> mk = 0;
    for (p = V.VEE.lp; p; p = p -> n)	/* clear VEE net	 */
	p -> mk = 0;
    for (p = V.VTT.lp; p; p = p -> n)	/* clear VTT net	 */
	p -> mk = 0;

    new_nh.x1 = xmax;		/* initialize prototype		 */
    new_nh.y1 = ymax;
    new_nh.x2 = 0;
    new_nh.y2 = 0;
    new_nh.f = 0;
    new_nh.l = 0;
    new_nh.lp = 0;
    new_nh.ref = 1;

    net_cnt = 0;		/* reset MW net counter		 */
    lcnt = 1;			/* reset line counter		 */

    Ferr_msg ("Busy: updating Netlist");

    while (i = get_NH ()) {
	if (i < 0) {
	    printf ("Invalid net: skipped\n");
	    err = 1;		/* prevent delete phase		 */
	    continue;
	}
	i = ident_NH ();	/* identify net			 */
	if (i < 0) { 		/* add new net			 */
	    printf ("Adding net '%s'\n", new_nh.name);
	    add_NH ();}
	else {
	    if (ref_chk ()) {
		printf ("Line %d: '%s' shorted to other net\n",
		    lcnt, new_nh.name);
		continue;
	    }
	    if (neq_NH (&NH[i])) 
		replace_NH ();
	}
    }

    if (err)
	printf ("Not deleting old net because of earlier errors\n");
    else {
	for (i = 0; i <V.nch; i++)
	    if (!NH[i].ref && NH[i].l) {
		printf ("Net '%s' deleted\n", NH[i].name);
		del_NH (&NH[i]);
	    }

	for (q = 0, p = V.GND.lp; p;) {
	    if (!(p -> mk)) {	/* remove node			 */
		printf ("Removing GND connection '%s-%d'\n",
		    CP[p -> c -> cpi].name, p -> c -> pn + 1);
		if (q)
		    q -> n = p -> n;	/* unlink it		 */
		else
		    V.GND.lp = p -> n;
		p -> n = V.enll;
		V.enll = p;
		p -> c -> n = 0;
		V.GND.l--;
		p = (q) ? q : V.GND.lp;}
	    else {
		q = p;
		p = p -> n;
	    }
	}

	for (q = 0, p = V.VCC.lp; p;) {
	    if (!(p -> mk)) {	/* remove node			 */
		printf ("Removing VCC connection '%s-%d'\n",
		    CP[p -> c -> cpi].name, p -> c -> pn + 1);
		if (q)
		    q -> n = p -> n;	/* unlink it		 */
		else
		    V.VCC.lp = p -> n;
		p -> n = V.enll;
		V.enll = p;
		p -> c -> n = 0;
		V.VCC.l--;
		p = (q) ? q : V.VCC.lp;}
	    else {
		q = p;
		p = p -> n;
	    }
	}

	for (q = 0, p = V.VEE.lp; p;) {
	    if (!(p -> mk)) {	/* remove node			 */
		printf ("Removing VEE connection '%s-%d'\n",
		    CP[p -> c -> cpi].name, p -> c -> pn + 1);
		if (q)
		    q -> n = p -> n;	/* unlink it		 */
		else
		    V.VEE.lp = p -> n;
		p -> n = V.enll;
		V.enll = p;
		p -> c -> n = 0;
		V.VEE.l--;
		p = (q) ? q : V.VEE.lp;}
	    else {
		q = p;
		p = p -> n;
	    }
	}

	for (q = 0, p = V.VTT.lp; p;) {
	    if (!(p -> mk)) {	/* remove node			 */
		printf ("Removing VTT connection '%s-%d'\n",
		    CP[p -> c -> cpi].name, p -> c -> pn + 1);
		if (q)
		    q -> n = p -> n;	/* unlink it		 */
		else
		    V.VTT.lp = p -> n;
		p -> n = V.enll;
		V.enll = p;
		p -> c -> n = 0;
		V.VTT.l--;
		p = (q) ? q : V.VTT.lp;}
	    else {
		q = p;
		p = p -> n;
	    }
	}
		
    }

    flush_stat ();
    placed = 0;			/* reevaluate net loactions	 */

    for (i = 0, nettgo = 0; i < V.nnh; i++)
	nettgo += !NH[i].f && NH[i].l;	/* count unfinished nets */
    nets_msg (nettgo);

    fclose (inp);
    err_msg ("Done");
    beep ();
}

get_NH ()
/*************************************************************\
* 							      *
*  Get a net:						      *
*   returns:  1: a correct net was read into "new_NH/new_NL"  *
* 	      0: EOF was encountered			      *
* 	     -1: the net description was in error	      *
* 							      *
\*************************************************************/
{
    char    buf[inp_lnl];
    register int i, j, ntc = 0;
    int k;
    static int preload = 0;

    ht_cnt = 0;			/* reset node count		 */

    do {
	if (!preload) {
	    if (!fgets (buf, inp_lnl, inp))
	        return mode ? 0 : ntc;	/* end of file		 */

	    rms (buf);
	    lcnt++;
	}
	preload = 0;		/* hack for Multiwre cont. records */

	if (mode) {
/*******************************************************************\
* 								    *
*  DP netlist reader						    *
* 								    *
*  Warning: I am too tired to completly rewrite the reader now, so  *
*           there is the limitation that a net has to fit into the  *
* 	    line buffer (buf).					    *
* 								    *
\*******************************************************************/
	    for (j = 0; buf[j] && buf[j] != '\\'; j++);
	    if (j && !buf[j]) {
		printf ("Line %d error: DP-name without '\\'\n", lcnt);
		return -1;
	    }

	    if (!j)
		continue;	/* skip empty lines		 */

	    if (buf[0] == '!') {/* power net (gnd, vcc, vee, vtt) */
		if      (!strncmp ("GND", &buf[1], 3))
		    power_n = &V.GND;
		else if (!strncmp ("VCC", &buf[1], 3)) 
		    power_n = &V.VCC;
		else if (!strncmp ("VTT", &buf[1], 3))
		    power_n = &V.VTT;
		else if (!strncmp ("VEE", &buf[1], 3))
		    power_n = &V.VEE;
	        else {
		    printf ("Line %d error: unknown power net\n", lcnt);
		    return -1;
		}
		strcpy (new_nh.name, power_n -> name); /* for err-msg only */
	    } else {		/* normal net			 */
		power_n = nil;
		buf[j] = 0;
		strncpy (new_nh.name, buf, nmmax);
	    }

	    for (j++; (j < inp_lnl) && (buf[j] != '\0'); ++j)
		if (buf[j] == ' ') {
		    if (add_NL (&buf[j + 1]))
			return -1;	/* some error in net	 */
		}

	    if (!power_n)
	        return 1;	/* net completed		 */

	    continue;		/* hide power-nets		 */
	}
	
	if (buf[0] == 'C') {	/* Multiwire net list processing */
	    j = 3;
	    if (!strncmp ("GND", &buf[1], 3))
		power_n = &V.GND;
	    else if (!strncmp ("VCC", &buf[1], 3)) 
		power_n = &V.VCC;
	    else if (!strncmp ("VTT", &buf[1], 3))
		power_n = &V.VTT;
	    else if (!strncmp ("VEE", &buf[1], 3))
		power_n = &V.VEE;
	    else
		continue;	/* normal comment		 */
	    strcpy (new_nh.name, power_n -> name);
	} else {
	    power_n = nil;
	    if (!sscanf (buf, "%d", &k)) {
		printf ("Line %d error: invalid net number\n", lcnt);
		return - 1;}
	    else if (k == -1)	/* Multiwire end ?		 */
		return 0;
	    else if ((k < net_cnt) || (k <= 0)) {
		printf ("Line %d error: net number %d out of sequence\n",
		    lcnt, k);
		return -1;}
	    else {
		if (k > net_cnt + 1)
		    printf("Line %d warning: net %d missing, next is %d\n",
			 lcnt, net_cnt + 1, k);
		if (k > net_cnt) {	/* start a new net	 */
		    if (ntc) {
			preload = 1;
			return 1;	/* got a net		 */
		    }
		    ntc = 1;
		    sprintf (new_nh.name, "#%d", net_cnt = k);
		}
	    }
	    j = 0;
	}

	for (j++; (j < inp_lnl) && (buf[j] != '\0'); ++j)
	    if (buf[j] == ' ') {
		if (add_NL (&buf[j + 1]))
		    return -1;	/* some error in net		 */
	    }

    } while (1);

    return -1;			/* keep lint happy		 */
}

add_NL (s)			/* add a node to a net list	*/
    char *s;
{
    register int i, j, l, x, y, r;
    int          k, x1, y1;
    struct hole  *hp, *fndh ();
    struct nlst  *p, *deq_NL ();
    struct nlhd  *net;

    for (i = 0; s[i] != '-'; ++i) /* isolate component name	*/
	if ((s[i] == '\0') || (i > nmmax)) {
	    printf ("Line %d error: missing pin number\n", lcnt);
	    return 1;
	}
    s[i] = '\0';		/* terminate string temporarily */

    for (j = 0; j < V.ncp; j++)	/* find componet definition	*/
	if (!strncmp (s, CP[j].name, nmmax))
	    break;
    if (j >= V.ncp) {
	printf ("Line %d error: reference to undefined component\n", lcnt);
	return 1;
    }

    if ((!sscanf (&s[i + 1], "%d", &k)) || (k <= 0) || (k > CP[j].ty -> np)) {
	printf("Line %d error: '%s-%d': pin-number out of range\n", lcnt,s,k);
	return 1;
    }					/* check pin number		*/
    --k;				/* pin number must start at 0	*/

    if ((CP[j].ty -> p + k) -> p)
	printf ("Line %d warning: '%s-%d' is a power/gnd pin\n", lcnt, s,k+1);
    s[i] = '*';				/* this is a hack (as usual)	*/

    if (CP[j].unplaced) {		/* need direct search		*/
	hp = nil;
	for (l = 0; l < V.nch; l++)
	    if (CH[j].cpi == j && CH[j].pn == k) {
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
	err ("add_NL: failed to find hole", x1, y1, x, y);

    if (power_n) {		
/************************************************************************\
* 									 *
*  New power net handler:						 *
* 									 *
*   case a: touching the old & correct powernet -> mark node		 *
*   case b: touching an old & !correct powernet -> move node, mark it	 *
*   case c: touching an old net (!power) -> delete old net, add a power	 *
* 					  node, mark it			 *
*   case d: touching an new net (!power) -> serious error		 *
*   case e: pin is in no net -> add a power node, mark it		 *
* 									 *
\************************************************************************/
	if (hp -> n) {		/* case a-d			 */
	    net = hp -> n -> nlp;

	    if (net == power_n) {			/*** CASE A ***/
		if (hp -> n -> mk) 
		    printf ("Line %d: multiple '%s' connection\n",
			lcnt, power_n -> name);	/* no need to abort */
		hp -> n -> mk = 1;
		return 0;
	    }

	    if (net < &NH[0] || net > &NH[V.nnh - 1]) { /*** CASE B ***/
		if (pcb[y1][x1] & vec)
		    err ("add_NL: power net with trace", x1, y1, lcnt, 0);
		if (hp -> n -> mk) {
		    printf ("Line %d: '%s' to '%s' short\n", lcnt,
			hp -> n -> nlp -> name, power_n -> name);
		    return 1;
		}
		if (net -> lp == hp -> n)	/* first in old list	 */
		    net -> lp = hp -> n -> n;
		else {
		    for (p = net -> lp; p; p = p -> n)
			if (p -> n == hp -> n) { /* found predecessor	 */
			    p -> n = hp -> n -> n;
			    break;
			}
		    if (p)
			err ("add_NL: unlink failed - ptr error",
				 x1, y1, lcnt, 0);
		}
		hp -> n -> n = power_n -> lp;
		power_n -> lp = hp -> n;	/* linked in new net	 */
		hp -> n -> nlp = power_n;
		hp -> n -> mk = 1;
		power_n -> l++;			/* update net length	 */
		net -> l--;
		return 0;
	    }

	    if (hp -> n -> nlp -> ref) {		/*** CASE D ***/
		printf ("Line %d: Error - net '%s' shorted to '%d'\n",
		    lcnt, hp -> n -> nlp -> name, power_n -> name);
		return 1;
	    }

	    del_NH (hp -> n -> nlp);			/*** CASE C ***/
	}

					     /*** CASE E + (C cont) ***/
	p = deq_NL ();		/* get an NL - node		 */

	p -> c = hp;		/* add node to power net	 */
	p -> n = power_n -> lp;
	p -> nlp = power_n;
	power_n -> lp = p;
	p -> mk = 1;
	hp -> n = p;
	power_n -> l++;		/* update net length		 */

	p -> mk = 1;		/* mark it			 */
	return 0;
    }

    if (!new_ht) 		/* need to allocate space	 */
	new_ht = (struct hole **) malloc (sizeof (struct hole *) * ht_max);
    else if (ht_max <= ht_cnt) {
	ht_max += 50;
	new_ht = (struct hole **) realloc (new_ht, 
			sizeof (struct hole *) * ht_max);
    }

    for (i = 0; i < ht_cnt; i++)
	if (new_ht[i] == hp) {
	    printf ("Line %d: multiple node in net\n", lcnt);
	    return 1;
	}

    new_ht[ht_cnt++] = hp;	/* remember hole		 */

    return 0;
}

ident_NH ()
/*********************************************************\
* 							  *
*  Identify the new net:				  *
*   the first match is reported. '-1' indicates no match  *
* 							  *
\*********************************************************/
{
    register int i;

    for (i = 0; i < ht_cnt; i++)
	if (new_ht[i] -> n) 
	   return ((int) (new_ht[i] -> n -> nlp) - (int) NH) /
			 sizeof (struct nlhd);

    return -1;
}

ref_chk ()
/************************************************************************\
* 									 *
*  reference check:							 *
*   a '1' is return if the new net has a node in common with a net that	 *
*   is already referenced. '0' is returned otherwise.			 *
* 									 *
\************************************************************************/
{
    register int i;

    for (i = 0; i < ht_cnt; i++)
	if (new_ht[i] -> n && new_ht[i] -> n -> nlp -> ref)
	   return 1;

    return 0;
}

neq_NH (net)
   struct nlhd *net;
/*************************************************************\
* 							      *
*  .NE. old net:					      *
*   A '1' is return if the new net differs from the old one.  *
* 							      *
\*************************************************************/
{
    register int i;
    register struct nlst *p;

    if (net -> l != ht_cnt)
	return 1;		/* different length	 */

    for (p = net -> lp; p; p = p -> n)
	p -> mk = 0;		/* clear marks of old net */

    for (i = 0; i < ht_cnt; i++) {
	if (!(new_ht[i] -> n) || new_ht[i] -> n -> nlp != net)
	    return 1;		/* referes to different net */
	new_ht[i] -> n -> mk = 1;
    }

    for (p = net -> lp; p; p = p -> n)
	if (!(p -> mk))
	    err ("new_NH: ptr screw up in internal structure",
		((int) net - (int) NH) / sizeof (struct nlhd), lcnt, 0, 0);

    net -> ref = 1;		/* sucessfully referenced */

    return 0;			/* nets match		 */
}

replace_NH ()
/*******************************************\
* 					    *
*  Replace the old net(s) with the new one  *
* 					    *
\*******************************************/
{
    register int i;

    printf ("The new net '%s' replaces", new_nh.name);

    for (i = 0; i < ht_cnt; i++)
	if (new_ht[i] -> n) {	/* delete the old nets	 */
	   printf (" '%s'", new_ht[i] -> n -> nlp -> name);
	   del_NH (new_ht[i] -> n -> nlp);
	}

    printf ("\n");

    add_NH ();
}

add_NH ()		/* add new net			 */
{
    register int i, j;
    register struct nlst *p;

    for (i = 0; i < V.nnh; i++)	/* look for a old header */
	if (!NH[i].l)
	   break;
    if (i >= V.nnh) {	/* not found: add new entry	 */
	if (V.nnh >= nhmax)
	    err ("Net table too small: increase 'nhmax'", nhmax, 0,0,0);
	i = V.nnh++;
    }

    NH[i] = new_nh;	/* copy prototype		 */
    NH[i].l = ht_cnt;

    for (j = 0; j < ht_cnt; j++) {	/* add nodes	 */
	p = deq_NL ();
	p -> n = NH[i].lp;
	NH[i].lp = p;
	p -> c = new_ht[j];
	p -> nlp = &NH[i];
	p -> mk = 0;
	new_ht[j] -> n = p;
    }
}

del_NH (net)		/* delete net			 */
    struct nlhd *net;
{
    register int x, y;
    register struct nlst *p, *q;
    int delh (), delv ();

    if (!(net -> l))
	err ("del_NH: deleting deleted net ???",
	   ((int) net - (int) NH) / sizeof (struct nlhd), lcnt, 0, 0);

    wthf = delh;	/* renmove old geometry		 */
    wtvf = delv;
    for (p = net -> lp; p; p = p -> n) {
	q = p;
	x = p -> c -> x;
	y = p -> c -> y;
	wtrace (x, y, vec);
	ckgp (x, y, s1b);
	ckgp (x, y, s2b);
	p -> c -> n = 0;	/* free holes		 */
    }

    q -> n = V.enll;	/* enter nodes to free list	 */
    V.enll = net -> lp;

    net -> lp = 0;
    net -> ref = 0;
    net -> l = 0;	/* net gone !!			 */
}
