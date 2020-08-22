/***************************************************************\
*								*
*	PCB program						*
*								*
*	Command loop section					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#include "pcdst.h"

#define iniCNTX 0		/* initial context		 */
#define nretry 2		/* retry limit for N_route	 */

int mv_wind(), zoom_in(), zoom_out(), s_menu(), c_menu();
int st_learn(), learn_sl(), learn_ov(), learn_ex(), lrn_wmv();
int w_zoomi(), w_zoomo(), wnd_urc(), wnd_llc(), exc_adel (), sel_adel();
int w_wmv(), ini_adel(), ini_art(), sel_art(), exc_art();

extern int net_sel(), 		/* manual edit functions	 */
    net_desel(), m_delete(), mr_delete(), 
    start_nt(), cont_s1(), cont_s2();

extern int exc_cmv(), cmv_plc(), cmv_rr(), cmv_rl(), cmv_sel();
extern int CPp_ini(), CPp_exc(), CPp_plc(), CPp_del();
extern int exc_plow(), plow_ini(), plow_src(), plow_dst();

static struct cntxdsc {		/* context descriptrors		 */
    char *name;			/* context name			 */
    int (*entcntx)();		/* enter context function	 */
    int (*excntx)();		/* exit context function	 */
    int (*mkfunc[8])();		/* mouse key functions		 */
} CNTX[] = {
   {"Start",   net_sel,  net_desel,
	      start_nt,    mv_wind,   start_nt,   m_delete,
	       zoom_in,     s_menu,   zoom_out,     c_menu  },
   {"Route", 	     0,  net_desel,
	       cont_s1,    mv_wind,    cont_s2,  mr_delete,
	       zoom_in,     s_menu,   zoom_out,     c_menu  },
   {"Edit",          0,          0,
		     0,	   mv_wind,	     0,   m_delete,
	       zoom_in,     s_menu,   zoom_out,     c_menu  },
   {"Learn",  st_learn,          0,
	      learn_sl,	   lrn_wmv,   learn_ex,   learn_ov,
	             0,          0,          0,          0  },
   {"AR-Del", ini_adel,          0,
	       wnd_urc,	     w_wmv,    wnd_llc,   exc_adel,
	       w_zoomi,   sel_adel,    w_zoomo,   sel_adel  },
   {"A-route", ini_art,          0,
	       wnd_urc,	     w_wmv,    wnd_llc,    exc_art,
	       w_zoomi,    sel_art,    w_zoomo,    sel_art  },
   {"CP_move",       0,          0,
	       cmv_sel,	   mv_wind,     cmv_plc,   exc_cmv,
	       zoom_in,     cmv_rl,    zoom_out,    cmv_rr  },
   {"Trace_mv", plow_ini,        0,
	      plow_src,	   mv_wind,    plow_dst,  exc_plow,
	       zoom_in,          0,    zoom_out,    c_menu  },
   {"CP_place",CPp_ini,          0,
	       CPp_exc,	   mv_wind,     CPp_plc,   CPp_del,
	       zoom_in,     s_menu,    zoom_out,    cmv_rr  }  };

cmd_loop()			/* command loop			 */
{
    int     nxtc, x, y, i;
    static int  context = iniCNTX;

    do {			/* context loop			 */
	if (CNTX[context].entcntx) {/* call enter function if present	*/
	    nxtc = (*CNTX[context].entcntx) (context);
	    if (nxtc != context) {/* context was rejected	 */
		context = nxtc;
		continue;
	    }
	}
	cntx_msg (CNTX[context].name);
	do {			/* loop within context		 */
	    i = getcur (&x, &y);
	    unprev ();		/* reset pending previews	 */
	    err_msg ("");	/* clear pending error messages	 */
	    if (i < 0)
		err ("couldn't read graphic tablet", i, 0, 0, 0);
	    nxtc = context;	/* default 			 */
	    if (CNTX[context].mkfunc[i])
		nxtc = (*CNTX[context].mkfunc[i]) (x, y, context);
	    else
		err_msg ("undefined key");
	} while (nxtc == context);
	if (CNTX[context].excntx)/* call exit function if present */
	    (*CNTX[context].excntx) (nxtc);
	context = nxtc;
    } while (context >= 0);
}

mv_wind (x, y, ctx)		/* move window			 */
    int x, y, ctx;
{
    window (x - 256 / cz, y - 256 / cz);
    return (ctx);		/* don't change context		 */
}

/*ARGSUSED*/
zoom_in (x, y, ctx)		/* increase zoom		 */
    int x, y, ctx;
{
    if (cz < 16)
	zoom (cz + 1);
    return (ctx);		/* don't change context		 */
}

/*ARGSUSED*/
zoom_out (x, y, ctx)		/* decrease zoom		 */
    int x, y, ctx;
{
    if (cz > 1)
	zoom (cz - 1);
    return (ctx);		/* don't change context		 */
}

quit ()				/* quit session			 */
{
    static char *mtext[] = {
	"Continue quit: no save",
	"Abort quit" };

    if (!menu (mtext, 2))
	finish ();
}

/*ARGSUSED*/
s_menu (x, y, ctx)		/* start menu			 */
    int x, y, ctx;
{
    static char *mtext[] = {
	"Plot preview",
	"CIF - Output",
	"Exit",
	"Quit (no save)",
	"Diagnostics",
	"Learn sequence",
	"Area Delete",
	"Area Route",
	"Route sequence",
	"Save Work",
	"Change color",
	"Move Component",
	"Expand wire",
	0,
	"Macro exec"
    };

    mtext[13] = mac_def ? "End MACdef" : "Start MACdef";

    switch (menu (mtext, 15)) {
	case 0:
	    Ferr_msg ("Busy: Plotting Bitmap");
	    pntbm ();
	    err_msg ("Done");
	    beep ();
	    break;
	case 1:
	    Ferr_msg ("Busy: Writing CIF file");
	    cifout ();
	    err_msg ("Done");
	    beep ();
	    break;
	case 2:
	    Ferr_msg ("Saving Bitmap & exit");
	    save (0);
	    finish ();
	    break;
	case 3:
	    quit ();
	    break;
	case 4:
	    p_diagn ();
	    break;
	case 5:
	    return (LEARN);
	case 6:
	    return (ADELE);
	case 7:
	    return (AROUTE);
	case 8:
	    seq_rt ();
	    return (net_sel (START));
	case 9:
	    Ferr_msg ("Busy: saving Bitmap");
	    save (0);
	    beep ();
	    err_msg ("Done");
	    break;
	case 10:
	    chg_color ();
	    break;
	case 11:
	    return (CMOVE);
	case 12:
	    return wide_wire (ctx);
	case 13:
	    mac_strt ();
	    break;
	case 14:
	    mac_exec ();
	    break;
    }

    return (ctx);
}

/*ARGSUSED*/
c_menu (x, y, ctx)		/* configuration menu		 */
    int x, y, ctx;
{
    static char *mtext[] = {
	0,				/* beep toggle		 */
	0,				/* window toggle	 */
	0,				/* H-route toggle	 */
	0,				/* straight toggle	 */
	"learn off",
	"Route Parameters",
	"Print statistics",
	"Wire desity",
	"Congestion area",
	"List",
	"Update"
    };

				/* update toggles		 */
    mtext[0] = (no_beep) ? "Beep on" : "Beep off";
    mtext[1] = (no_adjw) ? "Adjust window" : "Stable window";
    mtext[2] = (no_hrout) ? "Enable H-route" : "Disable H-route";
    mtext[3] = (st_reroute) ? "Move straight" : "Reroute straight";

    switch (menu (mtext, 11)) {
	case 0:
	    no_beep = (no_beep) ? 0 : 1;
	    break;
	case 1:
	    no_adjw = (no_adjw) ? 0 : 1;
	    break;
	case 2:
	    no_hrout = (no_hrout) ? 0 : 1;
	    break;
	case 3:
	    st_reroute = (st_reroute) ? 0 : 1;
	    break;
	case 4:
	    flush_lb ();
	    break;
	case 5:
	    Ferr_msg("See your Terminal");
	    get_rtprm ();
	    break;
        case 6:
	    pgen_stat ();
	    break;
	case 7:
	    dis_pro ();
	    break;
	case 8:
	    dis_cc ();
	    break;
	case 9:
	    P_list ();
	    break;
	case 10:
	    return P_update (ctx);
    }

    return (ctx);
}

static int ov_on = 0;		/* over-view display is on	 */

view_on ()			/* display a wire over-view	 */
{
    int     i, xl, yl, xh, yh, x0, y0, x1, y1;
    struct nlst *p1, *p2;

    if (ov_on)
	view_off ();

    xl = wx;			/* get visible area		 */
    xh = wx + 511 / cz;
    yl = wy;
    yh = wy + 482 / cz;

    msg_off ();
    color (resb, resb);

    for (i = 0; i < V.nnh; ++i)	/* scan through nets		 */
	if (NH[i].l && !NH[i].f) { /* only unfinished nets	 */
	    p1 = NH[i].lp;
	    while (p1) {
		x0 = p1 -> c -> x;
		y0 = p1 -> c -> y;
		if (x0 < xh && x0 > xl && y0 < yh && y0 > yl) {
		    p2 = p1 -> n;
		    while (p2) {/* display full lines		 */
			x1 = p2 -> c -> x;
			y1 = p2 -> c -> y;
			if (x0 < xh && x0 > xl && y0 < yh && y0 > yl)
			    aed_plt (x0, y0, x1, y1);
			p2 = p2 -> n;
		    }
		    p2 = NH[i].lp;
		    while (p2) {/* display of-screen lines	 */
			x1 = p2 -> c -> x;
			y1 = p2 -> c -> y;
			if (x0 >= xh || x0 <= xl || y0 >= yh || y0 <= yl)
			    aed_plt (x0, y0, x1, y1);
		        p2 = p2 -> n;
		    }
		}
		p1 = p1 -> n;
	    }
	}
    ov_on = 1;
}

view_off ()			/* turn off over view		 */
{
    if (ov_on) {
	color (0, resb);
	rect (wx, wy, wx + 511 / cz, wy + 482 / cz);
	msg_on ();
    }
    ov_on = 0;
}

static int lrn_cnt = 0;		/* learn counter		 */
static int lrnfull = 0;		/* max. number of valid entries	 */
static struct nlhd *lrn_buf[lrn_max];	/* learn buffer		 */

/*ARGSUSED*/
st_learn (x, y, ctx)		/* start learn mode		 */
    int x, y, ctx;
{
    lrn_cnt = 0;
    lrnfull = 0;
    return (LEARN);
}

learn_sl (x, y, ctx)		/* select a wire		 */
    int x, y, ctx;
{
    struct nlhd *p, *loc(), *fly();
    int i;

    if (lrn_cnt >= lrn_max) {
	view_off ();
	err_msg ("Learn buffer full");
	beep ();
	return (START);
    }

    p = loc (&x, &y);
    if (!p)
	p = fly (&x, &y);

    if (!p) {
	msg_on ();
	err_msg ("No net at cursor location");
	beep ();
	return (ctx);
    }

    if (p -> f) {
	msg_on ();
	err_msg ("Net is already done");
	return (ctx);
    }

    for (i = 0; i < lrn_cnt; ++i)
	if (p == lrn_buf[i]) {
	    msg_on ();
	    err_msg ("Net already selected");
	    beep ();
	    return (ctx);
	}

    lrn_buf[lrn_cnt++] = p;
    lrnfull = lrn_cnt;
    return (ctx);
}

lrn_wmv (x, y, ctx)		/* learn: move window		 */
    int x, y, ctx;
{
    if (ov_on)
	view_off ();
    return (mv_wind (x, y, ctx));
}

/*ARGSUSED*/
learn_ex (x, y, ctx)		/* learn: exit			 */
    int x, y, ctx;
{
    if (ov_on)
	view_off ();
    return (START);
}

/*ARGSUSED*/
learn_ov (x, y, ctx)		/* over-view on			 */
    int x, y, ctx;
{
    if (!ov_on)
	view_on ();
    return (ctx);
}

flush_lb ()			/* flush learn buffer		 */
{
    lrn_cnt = 0;
    lrnfull = 0;
}

struct nlhd *ck_lrnb ()		/* check learn buffer		 */
{
    while (lrn_cnt > 0 && lrn_buf[lrn_cnt - 1] -> f)
	lrn_cnt--;		/* skip finished nets		 */

    if (lrn_cnt <= 0)
	return (nil);		/* is empty			 */

    return (lrn_buf[--lrn_cnt]);
}

seq_rt ()			/* sequence route		 */
{
    struct nlhd *t, *ck_lrnb ();

    lrn_cnt = lrnfull;		/* rewind learn buffer		 */

    if (lrn_cnt <= 0)
	err_msg ("Learn a sequence first");
    else {
	Ferr_msg ("Busy routing");
	while ((t = ck_lrnb ())) {
	    N_route (t, nretry, 0, 0, V.BSX, V.BSY);
	    nettgo -= t -> f;
	}
	nets_msg (nettgo);
	err_msg ("Done");
	beep ();
    }
}

static struct mac_buf {		/* macro buffer			 */
    int x, y;			/* cursor coordinate		 */
    int ty;			/* entry type			 */
} *MB = 0;

static int mb_cnt = 0;		/* macro buffer count		 */
static int mb_max = 0;		/* max macro buffer size	 */
static int mb_sz  = 0;		/* current macro size		 */
static int org_x, org_y;	/* origin of definition		 */
static int min_x, min_y, max_x, max_y;	/* cursor confinement	 */
static int disp_x, disp_y;	/* displacement			 */

mac_strt ()			/* start a macro definition	 */
{
    if (mac_exp) {
	err_msg ("No macro nesting/recursion");
	return;			/* ignore call			 */
    }

    if (mac_def) {		/* terminate a macro		 */
	mb_sz--;		/* pop the termination command	 */
	if (mb_sz > 0)
	    err_msg ("Macro defined");
	else {
	    mb_sz = 0;
	    err_msg ("Empty macro");
	}
	mac_def = 0;  }
    else {			/* start a macro		 */
	if (mb_max <= 0) {	/* allocate initial buffer	 */
	    mb_max = 200;	/* raw guess			 */
	    MB = (struct mac_buf *) p_malloc(sizeof(struct mac_buf) * mb_max);
	}
	mb_cnt = 0;
	mb_sz = 0;		/* reset size			 */
	max_x = 0;		
	max_y = 0;
	min_x = V.BSX;
	min_y = V.BSY;
	disp_x = 0;
	disp_y = 0;
	err_msg ("Enter macro origin:");
	getcur (&org_x, &org_y);
	err_msg ("Recording macro");
	mac_def =1;
    }
}

mac_exec ()			/* macro execution		 */
{
    int x, y;
    register int i;

    if (mac_def) {
	err_msg ("No macro nesting/recursion");
	return;			/* ignore request		 */
    }

    if (!mb_max || mb_sz <= 0) {
	err_msg ("No macro defined");
	beep ();
	return;
    }

    err_msg ("Position origin");
    color (resb, resb);
    mac_hint ();
    i = getcur (&x, &y);
    color (0, resb);
    mac_hint ();
    if (i < 0 || i > 3) {
	err_msg ("Invalid origin");
	return;
    }

    disp_x = x - org_x;
    disp_y = y - org_y;
    if ((max_x + disp_x) >= V.BSX ||
	(min_x + disp_x) <= 0 ||
	(max_y + disp_y) >= V.BSY || 
	(min_y + disp_y) <= 0       ) {
	err_msg ("Part of macro leaves board");
	return;
    }

    mb_cnt = 0;
    mac_exp = 1;		/* start expansion		 */
}
mac_hint ()			/* give a hint on the macro	 */
{
    register int i;

    plts (org_x + disp_x + 5, org_y + disp_y,
	  org_x + disp_x - 5, org_y + disp_y );
    plts (org_x + disp_x, org_y + disp_y + 5,
	  org_x + disp_x, org_y + disp_y - 5);

    for (i = (mb_sz < 5) ? mb_sz : 5; i;)
	if (MB[--i].x >= 0)
	    point (MB[i].x + disp_x, MB[i].y + disp_y);
}

macdf_gc (x, y, i)		/* define macro			 */
    int x, y, i;
{
    if (!mac_def)
	err ("macdf_gc: no macro being defined", x, y, i, 0);

    if (mb_sz >= mb_max) {	/* time to expand space		 */
	mb_max += 1 + mb_max / 2;	/* add 50%		 */
	MB = (struct mac_buf *) p_realloc (MB,
		sizeof (struct mac_buf) * mb_max);
    }
    MB[mb_sz].x = x;
    MB[mb_sz].y = y;
    MB[mb_sz++].ty = i;

    if (min_x < x) min_x = x;
    if (max_x > x) max_x = x;
    if (min_y < y) min_y = y;
    if (max_y > y) max_y = y;
}

macdf_mn (i)			/* define macro			 */
    int i;
{
    if (!mac_def)
	err ("macdf_mn: no macro being defined", i, 0, 0, 0);

    if (mb_sz >= mb_max) {	/* time to expand space		 */
	mb_max += 1 + mb_max / 2;	/* add 50%		 */
	MB = (struct mac_buf *) p_realloc (MB,
		sizeof (struct mac_buf) * mb_max);
    }
    MB[mb_sz].x = -1;
    MB[mb_sz].y = -1;
    MB[mb_sz++].ty = i;
}

macex_gc (x, y)			/* macro expansion: get cursor	 */
    int *x, *y;
{
    if (!mac_exp)
	err ("macex_gc: no macro being expanded", 0, 0, 0, 0);

    if (MB[mb_cnt].x < 0)
	err ("macex_gc: sync error", mb_cnt, mb_sz, MB[mb_cnt].ty, 0);

    *x = MB[mb_cnt].x + disp_x;
    *y = MB[mb_cnt++].y + disp_y;

    if (mb_cnt >= mb_sz)	/* macro terminated		 */
	mac_exp = 0;

    return MB[mb_cnt - 1].ty;
}

macex_mn ()			/* macro exansion: menu		 */
{
    if (!mac_exp)
	err ("macex_mn: no macro being expanded", 0, 0, 0, 0);

    if (MB[mb_cnt++].x != -1)
	err ("macex_mn: sync error", mb_cnt, mb_sz, MB[mb_cnt].ty, 0);

    if (mb_cnt >= mb_sz)	/* macro terminated		 */
	mac_exp = 0;

    return MB[mb_cnt - 1].ty;
}
