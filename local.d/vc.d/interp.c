/*	SC	A Spreadsheet Calculator
 *		Expression interpreter and assorted support routines.
 *
 *		original by James Gosling, September 1982
 *		modified by Mark Weiser and Bruce Israel, 
 *			University of Maryland
 *
 *              More mods Robert Bond, 12/86
 */

#include <math.h>
#include <signal.h>
#include <setjmp.h>
#include <stdio.h>

#ifdef BSD42
#include <strings.h>
#include <sys/time.h>
#else
#include <time.h>
#ifndef SYSIII
#include <string.h>
#endif
#endif

#include <curses.h>
#include "sc.h"
#define DEFCOLDELIM ':'

extern char curfile[];

jmp_buf fpe_save;
int	exprerr;	/* Set by eval() and seval() if expression errors */

#ifdef SYSV3
void exit();
#endif
#define PI (double)3.14159265358979323846
#define dtr(x) ((x)*(PI/(double)180.0))
#define rtd(x) ((x)*(180.0/(double)PI))

double
dosum(minr, minc, maxr, maxc)
int minr, minc, maxr, maxc;
{
    double v;
    register r,c;
    register struct ent *p;

    v = 0;
    for (r = minr; r<=maxr; r++)
	for (c = minc; c<=maxc; c++)
	    if ((p = tbl[r][c]) && p->flags&is_valid)
		v += p->v;
    return v;
}

double
doprod(minr, minc, maxr, maxc)
int minr, minc, maxr, maxc;
{
    double v;
    register r,c;
    register struct ent *p;

    v = 1;
    for (r = minr; r<=maxr; r++)
	for (c = minc; c<=maxc; c++)
	    if ((p = tbl[r][c]) && p->flags&is_valid)
		v *= p->v;
    return v;
}

double
doavg(minr, minc, maxr, maxc)
int minr, minc, maxr, maxc;
{
    double v;
    register r,c,count;
    register struct ent *p;

    v = 0;
    count = 0;
    for (r = minr; r<=maxr; r++)
	for (c = minc; c<=maxc; c++)
	    if ((p = tbl[r][c]) && p->flags&is_valid) {
		v += p->v;
		count++;
	    }

    if (count == 0) 
	return ((double) 0);

    return (v / (double)count);
}

double
dostddev(minr, minc, maxr, maxc)
int minr, minc, maxr, maxc;
{
    double lp, rp, v, nd;
    register r,c,n;
    register struct ent *p;

    n = 0;
    lp = 0;
    rp = 0;
    for (r = minr; r<=maxr; r++)
	for (c = minc; c<=maxc; c++)
	    if ((p = tbl[r][c]) && p->flags&is_valid) {
		v = p->v;
		lp += v*v;
		rp += v;
		n++;
	    }

    if ((n == 0) || (n == 1)) 
	return ((double) 0);
    nd = (double)n;
    return (sqrt((nd*lp-rp*rp)/(nd*(nd-1))));
}

double
domax(minr, minc, maxr, maxc)
int minr, minc, maxr, maxc;
{
    double v;
    register r,c,count;
    register struct ent *p;

    count = 0;
    for (r = minr; r<=maxr; r++)
	for (c = minc; c<=maxc; c++)
	    if ((p = tbl[r][c]) && p->flags&is_valid) {
		if (!count) {
		    v = p->v;
		    count++;
		} else if (p->v > v)
		    v = p->v;
	    }

    if (count == 0) 
	return ((double) 0);

    return (v);
}

double
domin(minr, minc, maxr, maxc)
int minr, minc, maxr, maxc;
{
    double v;
    register r,c,count;
    register struct ent *p;

    count = 0;
    for (r = minr; r<=maxr; r++)
	for (c = minc; c<=maxc; c++)
	    if ((p = tbl[r][c]) && p->flags&is_valid) {
		if (!count) {
		    v = p->v;
		    count++;
		} else if (p->v < v)
		    v = p->v;
	    }

    if (count == 0) 
	return ((double) 0);

    return (v);
}

double
dotime(which, when)
int which;
double when;
{
	long time();

	static long t_cache;
	static struct tm *tp;
	long tloc;

	if (which == NOW) 
	    return (double)time((long *)0);

	tloc = (long)when;

	if (tloc != t_cache) {
	    tp = localtime(&tloc);
	    tp->tm_mon += 1;
	    tp->tm_year += 1900;
	    t_cache = tloc;
	}

	switch (which) {
		case HOUR: return((double)(tp->tm_hour));
		case MINUTE: return((double)(tp->tm_min));
		case SECOND: return((double)(tp->tm_sec));
		case MONTH: return((double)(tp->tm_mon));
		case DAY: return((double)(tp->tm_mday));
		case YEAR: return((double)(tp->tm_year));
	}
	/* Safety net */
	return (0.0);
}

double
doston(s)
char *s;
{
    char *strtof();
    double v;

    if (!s)
	return((double)0.0);

    (void)strtof(s, &v);
    xfree(s);
    return(v);
}

double
doeqs(s1, s2)
char *s1, *s2;
{
    double v;

    if (strcmp(s1, s2) == 0)
	v = 1.0;
    else
	v = 0.0;

    if (s1)
    	xfree(s1);

    if (s2)
    	xfree(s2);

    return(v);
}

double 
eval(e)
register struct enode *e;
{
    if (e==0) return 0;
    switch (e->op) {
	case '+':	return (eval(e->e.o.left) + eval(e->e.o.right));
	case '-':	return (eval(e->e.o.left) - eval(e->e.o.right));
	case '*':	return (eval(e->e.o.left) * eval(e->e.o.right));
	case '/':     {	double denom = eval (e->e.o.right);
			return denom ? eval(e->e.o.left) / denom : 0; }
	case '^':	return (pow(eval(e->e.o.left), eval(e->e.o.right)));
	case '<':	return (eval(e->e.o.left) < eval(e->e.o.right));
	case '=':	return (eval(e->e.o.left) == eval(e->e.o.right));
	case '>':	return (eval(e->e.o.left) > eval(e->e.o.right));
	case '&':	return (eval(e->e.o.left) && eval(e->e.o.right));
	case '|':	return (eval(e->e.o.left) || eval(e->e.o.right));
	case '?':	return eval(e->e.o.left) ? eval(e->e.o.right->e.o.left)
						: eval(e->e.o.right->e.o.right);
	case 'm':	return (-eval(e->e.o.right));
	case 'f':	return (eval(e->e.o.right));
	case '~':	return (eval(e->e.o.right) == 0.0);
	case 'k':	return (e->e.k);
	case 'v':	return (e->e.v.vp->v);
	case O_REDUCE('+'):
 	case O_REDUCE('*'):
 	case O_REDUCE('a'):
 	case O_REDUCE('s'):
	case O_REDUCE(MAX):
	case O_REDUCE(MIN):
	    {	register r,c;
		register maxr, maxc;
		register minr, minc;
		maxr = e->e.r.right.vp -> row;
		maxc = e->e.r.right.vp -> col;
		minr = e->e.r.left.vp -> row;
		minc = e->e.r.left.vp -> col;
		if (minr>maxr) r = maxr, maxr = minr, minr = r;
		if (minc>maxc) c = maxc, maxc = minc, minc = c;
	        switch (e->op) {
	            case O_REDUCE('+'): return dosum(minr, minc, maxr, maxc);
 	            case O_REDUCE('*'): return doprod(minr, minc, maxr, maxc);
 	            case O_REDUCE('a'): return doavg(minr, minc, maxr, maxc);
 	            case O_REDUCE('s'): return dostddev(minr, minc, maxr, maxc);
 	            case O_REDUCE(MAX): return domax(minr, minc, maxr, maxc);
 	            case O_REDUCE(MIN): return domin(minr, minc, maxr, maxc);
		}
	    }
	case ACOS:	 return (acos(eval(e->e.o.right)));
	case ASIN:	 return (asin(eval(e->e.o.right)));
	case ATAN:	 return (atan(eval(e->e.o.right)));
	case CEIL:	 return (ceil(eval(e->e.o.right)));
	case COS:	 return (cos(eval(e->e.o.right)));
	case EXP:	 return (exp(eval(e->e.o.right)));
	case FABS:	 return (fabs(eval(e->e.o.right)));
	case FLOOR:	 return (floor(eval(e->e.o.right)));
	case HYPOT:	 return (hypot(eval(e->e.o.left), eval(e->e.o.right)));
	case LOG:	 { double arg = eval(e->e.o.right);
			   return arg ? log(arg) : 0; }
	case LOG10:	 { double arg = eval(e->e.o.right);
			   return arg ? log10(arg) : 0; }
	case POW:	 return (pow(eval(e->e.o.left), eval(e->e.o.right)));
	case SIN:	 return (sin(eval(e->e.o.right)));
	case SQRT:	 return (sqrt(eval(e->e.o.right)));
	case TAN:	 return (tan(eval(e->e.o.right)));
	case DTR:	 return (dtr(eval(e->e.o.right)));
	case RTD:	 return (rtd(eval(e->e.o.right)));
	case RND:	 {
			    double temp;
			    temp = eval(e->e.o.right);
			    return(temp-floor(temp) < 0.5 ?
					     floor(temp) : ceil(temp));
			 }
	case HOUR:	 return (dotime(HOUR, eval(e->e.o.right)));
	case MINUTE:	 return (dotime(MINUTE, eval(e->e.o.right)));
	case SECOND:	 return (dotime(SECOND, eval(e->e.o.right)));
	case MONTH:	 return (dotime(MONTH, eval(e->e.o.right)));
	case DAY:	 return (dotime(DAY, eval(e->e.o.right)));
	case YEAR:	 return (dotime(YEAR, eval(e->e.o.right)));
	case NOW:	 return (dotime(NOW, (double)0.0));
	case STON:	 return (doston(seval(e->e.o.right)));
	case EQS:        return (doeqs(seval(e->e.o.right),seval(e->e.o.left)));
	default:	 error("Illegal Numeric Expression");
			 exprerr = 1;
			 return((double)0.0);
    }
}

/* 
 * Rules for string functions:
 * Take string arguments which they xfree.
 * All returned strings are assumed to be xalloced.
 */

char *
docat(s1, s2)
register char *s1, *s2;
{
    register char *p;

    p = xmalloc((unsigned)(strlen(s1)+strlen(s2)+1));
    (void) strcpy(p, s1);
    (void) strcat(p, s2);
    if (s1)
        xfree(s1);
    if (s2)
        xfree(s2);
    return(p);
}

char *
dodate(tloc)
long tloc;
{
    char *tp;
    char *p;

    tp = ctime(&tloc);
    tp[24] = 0;
    p = xmalloc((unsigned)25);
    (void) strcpy(p, tp);
    return(p);
}

char *
dofmt(fmtstr, v)
char *fmtstr;
double v;
{
    char buff[1024];
    char *p;

    if (!fmtstr)
	return(0);
    (void)sprintf(buff, fmtstr, v);
    p = xmalloc((unsigned)(strlen(buff)+1));
    (void) strcpy(p, buff);
    xfree(fmtstr);
    return(p);
}

char *
dosubstr(s, v1, v2)
char *s;
register int v1,v2;
{
    register char *s1, *s2;
    char *p;

    if (!s)
	return(0);

    if (v1 < 0 || v2 < v1 || strlen(s) <= (unsigned)v2 ) {
	xfree(s);
	p = xmalloc((unsigned)1);
	p[0] = 0;
	return(p);
    }
    s2 = p = xmalloc((unsigned)(v2-v1+2));
    s1 = &s[v1];
    for(; v1 <= v2; s1++, s2++, v1++)
	*s2 = *s1;
    *s2 = 0;
    xfree(s);
    return(p);
}

char *
seval(se)
register struct enode *se;
{
    register char *p;

    if (se==0) return 0;
    switch (se->op) {
	case O_SCONST: p = xmalloc((unsigned)(strlen(se->e.s)+1));
		     (void) strcpy(p, se->e.s);
		     return(p);
	case O_VAR:    {
			struct ent *ep;

			ep = se->e.v.vp;
			p = xmalloc((unsigned)(strlen(ep->label)+1));
			(void) strcpy(p, ep->label);
			return(p);
		     }
	case '#':    return(docat(seval(se->e.o.left), seval(se->e.o.right)));
	case 'f':    return(seval(se->e.o.right));
	case '?':    return(eval(se->e.o.left) ? seval(se->e.o.right->e.o.left)
					     : seval(se->e.o.right->e.o.right));
	case DATE:   return(dodate((long)(eval(se->e.o.right))));
	case FMT:    return(dofmt(seval(se->e.o.left), eval(se->e.o.right)));
	case SUBSTR: return(dosubstr(seval(se->e.o.left),
			    (int)eval(se->e.o.right->e.o.left) - 1,
			    (int)eval(se->e.o.right->e.o.right) - 1));
	default:
		     error("Illegal String Expression");
		     exprerr = 1;
		     return(0);
	}
}

#define MAXPROP 7

EvalAll () {
    int repct = 0;

    while (RealEvalAll() && (repct++ <= MAXPROP));
}

extern SIG_T quit();

SIG_T
eval_fpe(dummy) /* Trap for FPE errors in eval */
{
	longjmp(fpe_save, 1);
}

int 
RealEvalAll () {
    register i,j;
    int chgct = 0;
    register struct ent *p;

    (void) signal(SIGFPE, eval_fpe);
    for (i=0; i<=maxrow; i++)
	for (j=0; j<=maxcol; j++)
	    if ((p=tbl[i][j]) && p->expr) {
		if (p->flags & is_strexpr) {
		    char *v;
		    if (setjmp(fpe_save)) {
			error("Floating point exception %s", v_name(i,j));
			v = "";
		    } else {
		        v = seval(p->expr);
		    }
		    if (strcmp(v, p->label) != 0) {
			chgct++;
		        p->flags |= is_changed;
		    }
		    if(p->label)
			xfree(p->label);
		    p->label = v;
		} else {
		    double v;
		    if (setjmp(fpe_save)) {
			error("Floating point exception %s", v_name(i,j));
			v = 0.0;
		    } else {
		        v = eval (p->expr);
		    }
		    if (v != p->v) {
			p->v = v; chgct++;
			p->flags |= (is_changed|is_valid);
		    }
		}
	    }
    (void) signal(SIGFPE, quit);
    return(chgct);
}

struct enode *
new(op, a1, a2)
struct enode *a1, *a2;
{
    register struct enode *p;
    p = (struct enode *) xmalloc ((unsigned)sizeof (struct enode));
    p->op = op;
    p->e.o.left = a1;
    p->e.o.right = a2;
    return p;
}

struct enode *
new_var(op, a1)
struct ent_ptr a1;
{
    register struct enode *p;
    p = (struct enode *) xmalloc ((unsigned)sizeof (struct enode));
    p->op = op;
    p->e.v = a1;
    return p;
}

struct enode *
new_range(op, a1)
struct range_s a1;
{
    register struct enode *p;
    p = (struct enode *) xmalloc ((unsigned)sizeof (struct enode));
    p->op = op;
    p->e.r = a1;
    return p;
}

struct enode *
new_const(op, a1)
double a1;
{
    register struct enode *p;
    p = (struct enode *) xmalloc ((unsigned)sizeof (struct enode));
    p->op = op;
    p->e.k = a1;
    return p;
}

struct enode *
new_str(s)
char *s;
{
    register struct enode *p;

    p = (struct enode *) xmalloc ((unsigned)sizeof(struct enode));
    p->op = O_SCONST;
    p->e.s = s;
    return(p);
}

copy(dv1, dv2, v1, v2)
struct ent *dv1, *dv2, *v1, *v2;
{
    int minsr, minsc;
    int maxsr, maxsc;
    int mindr, mindc;
    int maxdr, maxdc;
    int vr, vc;
    int r, c;

    mindr = dv1->row;
    mindc = dv1->col;
    maxdr = dv2->row;
    maxdc = dv2->col;
    if (mindr>maxdr) r = maxdr, maxdr = mindr, mindr = r;
    if (mindc>maxdc) c = maxdc, maxdc = mindc, mindc = c;
    maxsr = v2->row;
    maxsc = v2->col;
    minsr = v1->row;
    minsc = v1->col;
    if (minsr>maxsr) r = maxsr, maxsr = minsr, minsr = r;
    if (minsc>maxsc) c = maxsc, maxsc = minsc, minsc = c;
    if (maxdr >= MAXROWS  || 
           maxdc >= MAXCOLS) {
	error ("The table can't be any bigger");
	return;
    }
    erase_area(mindr, mindc, maxdr, maxdc);
    if (minsr == maxsr && minsc == maxsc) {
	/* Source is a single cell */
	for(vr = mindr; vr <= maxdr; vr++)
	    for (vc = mindc; vc <= maxdc; vc++)
		copyrtv(vr, vc, minsr, minsc, maxsr, maxsc);
    } else if (minsr == maxsr) {
	/* Source is a single row */
	for (vr = mindr; vr <= maxdr; vr++)
	    copyrtv(vr, mindc, minsr, minsc, maxsr, maxsc);
    } else if (minsc == maxsc) {
	/* Source is a single column */
	for (vc = mindc; vc <= maxdc; vc++)
	    copyrtv(mindr, vc, minsr, minsc, maxsr, maxsc);
    } else {
	/* Everything else */
	copyrtv(mindr, mindc, minsr, minsc, maxsr, maxsc);
    }
    sync_refs();
}

copyrtv(vr, vc, minsr, minsc, maxsr, maxsc)
int vr, vc, minsr, minsc, maxsr, maxsc;
{
    register struct ent *p;
    register struct ent *n;
    register int sr, sc;
    register int dr, dc;

    for (dr=vr, sr=minsr; sr<=maxsr; sr++, dr++)
	for (dc=vc, sc=minsc; sc<=maxsc; sc++, dc++) {
	    n = lookat (dr, dc);
	    (void) clearent(n);
	    if (p = tbl[sr][sc]) {
		n -> v = p -> v;
		n -> flags = p -> flags;
		n -> expr = copye(p->expr, dr - sr, dc - sc);
		n -> label = 0;
		if (p -> label) {
		    n -> label = xmalloc ((unsigned)(strlen (p -> label) + 1));
		    (void) strcpy (n -> label, p -> label);
		}
	    }
	}
}

eraser(v1, v2)
struct ent *v1, *v2;
{
	FullUpdate++;
	flush_saved();
	erase_area(v1->row, v1->col, v2->row, v2->col);
	sync_refs();
}

moveto(v)
struct ent *v;
{
    currow = v->row;
    curcol = v->col;
}

fill (v1, v2, start, inc)
struct ent *v1, *v2;
double start, inc;
{
    register r,c;
    register struct ent *n;
    int maxr, maxc;
    int minr, minc;

    maxr = v2->row;
    maxc = v2->col;
    minr = v1->row;
    minc = v1->col;
    if (minr>maxr) r = maxr, maxr = minr, minr = r;
    if (minc>maxc) c = maxc, maxc = minc, minc = c;
    if (maxr >= MAXROWS) maxr = MAXROWS-1;
    if (maxc >= MAXCOLS) maxc = MAXCOLS-1;
    if (minr < 0) minr = 0;
    if (minr < 0) minr = 0;

    FullUpdate++;
    for (r = minr; r<=maxr; r++)
	for (c = minc; c<=maxc; c++) {
	    n = lookat (r, c);
	    (void) clearent(n);
	    n->v = start;
	    start += inc;
	    n->flags |= (is_changed|is_valid);
	}
}

let (v, e)
struct ent *v;
struct enode *e;
{
    double val;

    exprerr = 0;
    (void) signal(SIGFPE, eval_fpe);
    if (setjmp(fpe_save)) {
	error("Floating point exception %s", v_name(v->row, v->col));
	val = 0.0;
    } else {
	val = eval(e);
    }
    (void) signal(SIGFPE, quit);
    if (exprerr) {
	efree(e);
	return;
    }
    if (constant(e)) {
	v->v = val;
	if (!(v->flags & is_strexpr)) {
            efree (v->expr);
	    v->expr = 0;
	}
	efree(e);
        v->flags |= (is_changed|is_valid);
        changed++;
        modflg++;
	return;
    }
    efree (v->expr);
    v->expr = e;
    v->flags |= (is_changed|is_valid);
    v->flags &= ~is_strexpr;
    changed++;
    modflg++;
}

slet (v, se, flushdir)
struct ent *v;
struct enode *se;
int flushdir;
{
    char *p;

    exprerr = 0;
    (void) signal(SIGFPE, eval_fpe);
    if (setjmp(fpe_save)) {
	error("Floating point exception %s", v_name(v->row, v->col));
	p = "";
    } else {
	p = seval(se);
    }
    (void) signal(SIGFPE, quit);
    if (exprerr) {
	efree(se);
	return;
    }
    if (constant(se)) {
	label(v, p, flushdir);
	if (p)
	    xfree(p);
	efree(se);
	if (v->flags & is_strexpr) {
            efree (v->expr);
	    v->expr = 0;
	    v->flags &= ~is_strexpr;
	}
	return;
    }
    efree (v->expr);
    v->expr = se;
    v->flags |= (is_changed|is_strexpr);
    if (flushdir<0) v->flags |= is_leftflush;
    else v->flags &= ~is_leftflush;
    FullUpdate++;
    changed++;
    modflg++;
}

clearent (v)
struct ent *v; {
    if (!v)
	return;
    label(v,"",-1);
    v->v = 0;
    if (v->expr)
	efree(v->expr);
    v->expr = 0;
    v->flags |= (is_changed);
    v->flags &= ~(is_valid);
    changed++;
    modflg++;
}

constant(e)
register struct enode *e; {
    return e==0 || e->op == O_CONST || e->op == O_SCONST
	|| (e->op != O_VAR
	 && (e->op&~0177) != O_REDUCE(0)
	 && constant (e->e.o.left)
	 && constant(e->e.o.right)
	 && e->op != NOW);
}

efree (e)
register struct enode *e; {
    if (e) {
	if (e->op != O_VAR && e->op !=O_CONST && e->op != O_SCONST
		&& (e->op&~0177) != O_REDUCE(0)) {
	    efree(e->e.o.left);
	    efree(e->e.o.right);
	}
	if (e->op == O_SCONST && e->e.s)
	    xfree(e->e.s);
	xfree ((char *)e);
    }
}

label (v, s, flushdir)
register struct ent *v;
register char *s; {
    if (v) {
	if (flushdir==0 && v->flags&is_valid) {
	    register struct ent *tv;
	    if (v->col>0 && ((tv=lookat(v->row,v->col-1))->flags&is_valid)==0)
		v = tv, flushdir = 1;
	    else if (((tv=lookat (v->row,v->col+1))->flags&is_valid)==0)
		v = tv, flushdir = -1;
	    else flushdir = -1;
	}
	if (v->label) xfree((char *)(v->label));
	if (s && s[0]) {
	    v->label = xmalloc ((unsigned)(strlen(s)+1));
	    (void) strcpy (v->label, s);
	} else
	    v->label = 0;
	if (flushdir<0) v->flags |= is_leftflush;
	else v->flags &= ~is_leftflush;
	FullUpdate++;
	modflg++;
    }
}

decodev (v)
struct ent_ptr v; 
{
	register struct range *r;

	if (!v.vp) (void)sprintf (line+linelim,"VAR?");
	else if (r = find_range((char *)0, 0, v.vp, v.vp))
	    (void)sprintf(line+linelim, "%s", r->r_name);
	else
	    (void)sprintf (line+linelim, "%s%s%s%d",
			v.vf & FIX_COL ? "$" : "",
			coltoa(v.vp->col),
			v.vf & FIX_ROW ? "$" : "",
			v.vp->row);
	linelim += strlen (line+linelim);
}

char *
coltoa(col)
int col;
{
    static char rname[3];
    register char *p = rname;

    if (col > 25) {
	*p++ = col/26 + 'A' - 1;
	col %= 26;
    }
    *p++ = col+'A';
    *p = 0;
    return(rname);
}

decompile(e, priority)
register struct enode *e; {
    register char *s;
    if (e) {
	int mypriority;
	switch (e->op) {
	default: mypriority = 99; break;
	case '?': mypriority = 1; break;
	case ':': mypriority = 2; break;
	case '|': mypriority = 3; break;
	case '&': mypriority = 4; break;
	case '<': case '=': case '>': mypriority = 6; break;
	case '+': case '-': case '#': mypriority = 8; break;
	case '*': case '/': mypriority = 10; break;
	case '^': mypriority = 12; break;
	}
	if (mypriority<priority) line[linelim++] = '(';
	switch (e->op) {
	case 'f':	{ 
			    for (s="fixed "; line[linelim++] = *s++;);
			    linelim--;
			    decompile (e->e.o.right, 30);
			    break;
			}
	case 'm':	line[linelim++] = '-';
			decompile (e->e.o.right, 30);
			break;
	case '~':	line[linelim++] = '~';
			decompile (e->e.o.right, 30);
			break;
	case 'v':	decodev (e->e.v);
			break;
	case 'k':	(void)sprintf (line+linelim,"%.15g",e->e.k);
			linelim += strlen (line+linelim);
			break;
	case '$':	(void)sprintf (line+linelim, "\"%s\"", e->e.s);
			linelim += strlen(line+linelim);
			break;
	case O_REDUCE('+'):
			s = "@sum("; goto more;
	case O_REDUCE('*'):
			s = "@prod("; goto more;
	case O_REDUCE('s'):
			s = "@stddev("; goto more;
	case O_REDUCE(MAX):
			s = "@max("; goto more;
	case O_REDUCE(MIN):
			s = "@min("; goto more;
	case O_REDUCE('a'):
			s = "@avg("; /* fall though to more; */
	more:		{
			struct range *r;

			for (; line[linelim++] = *s++;);
			linelim--;
			if (r = find_range((char *)0, 0, e->e.r.left.vp,
						 e->e.r.right.vp)) {
			    (void)sprintf(line+linelim, "%s", r->r_name);
			    linelim += strlen(line+linelim);
			} else {
			    decodev (e->e.r.left);
			    line[linelim++] = ':';
			    decodev (e->e.r.right);
			}
			line[linelim++] = ')';
			break;
			}
	case ACOS:	s = "@acos("; goto more1;
	case ASIN:	s = "@asin("; goto more1;
	case ATAN:	s = "@atan("; goto more1;
	case CEIL:	s = "@ceil("; goto more1;
	case COS:	s = "@cos("; goto more1;
	case EXP:	s = "@exp("; goto more1;
	case FABS:	s = "@fabs("; goto more1;
	case FLOOR:	s = "@floor("; goto more1;
	case HYPOT:	s = "@hypot("; goto more2;
	case LOG:	s = "@ln("; goto more1;
	case LOG10:	s = "@log("; goto more1;
	case POW:	s = "@pow("; goto more2;
	case SIN:	s = "@sin("; goto more1;
	case SQRT:	s = "@sqrt("; goto more1;
	case TAN:	s = "@tan("; goto more1;
	case DTR:	s = "@dtr("; goto more1;
	case RTD:	s = "@rtd("; goto more1;
	case RND:	s = "@rnd("; goto more1;
	case HOUR:	s = "@hour("; goto more1;
	case MINUTE:	s = "@minute("; goto more1;
	case SECOND:	s = "@second("; goto more1;
	case MONTH:	s = "@month("; goto more1;
	case DAY:	s = "@day("; goto more1;
	case YEAR:	s = "@year("; goto more1;
	case DATE:	s = "@date("; goto more1;
	case STON:	s = "@ston("; goto more1;
	case FMT:	s = "@fmt("; goto more2;
	case EQS:	s = "@eqs("; goto more2;
	more1:		for (; line[linelim++] = *s++;);
			linelim--;
			decompile (e->e.o.right, 0);
			line[linelim++] = ')';
			break;
	more2:		for (; line[linelim++] = *s++;);
			linelim--;
			decompile (e->e.o.left, 0);
			line[linelim++] = ',';
			decompile (e->e.o.right, 0);
			line[linelim++] = ')';
			break;
	case NOW:	s = "@now"; goto more0;
	more0:		for (; line[linelim++] = *s++;);
			linelim--;
			break;
	case SUBSTR:	s = "@substr(";
			for (; line[linelim++] = *s++;);
			linelim--;
			decompile (e->e.o.left, 0);
			line[linelim++] = ',';
			decompile (e->e.o.right->e.o.left, 0);
			line[linelim++] = ',';
			decompile (e->e.o.right->e.o.right, 0);
			line[linelim++] = ')';
			break;

	default:	decompile (e->e.o.left, mypriority);
			line[linelim++] = e->op;
			decompile (e->e.o.right, mypriority+1);
			break;
	}
	if (mypriority<priority) line[linelim++] = ')';
    } else line[linelim++] = '?';
}

editv (row, col) {
    register struct ent *p;

    p = lookat (row, col);
    (void)sprintf (line, "let %s = ", v_name(row, col));
    linelim = strlen(line);
    if (p->flags & is_strexpr || p->expr == 0) {
	(void)sprintf (line+linelim, "%.15g", p->v);
	linelim += strlen (line+linelim);
    } else {
        editexp(row,col);
    }
}

editexp(row,col) {
    register struct ent *p;

    p = lookat (row, col);
    decompile (p->expr, 0);
    line[linelim] = 0;
}

edits (row, col) {
    register struct ent *p;

    p = lookat (row, col);
    (void)sprintf (line, "%sstring %s = ",
			((p->flags&is_leftflush) ? "left" : "right"),
			v_name(row, col));
    linelim = strlen(line);
    if (p->flags & is_strexpr && p->expr) {
	editexp(row, col);
    } else {
        (void)sprintf (line+linelim, "\"%s\"", p->label);
        linelim += strlen (line+linelim);
    }
}

printfile (fname, r0, c0, rn, cn)
char *fname;
{
    FILE *f;
    char pline[1000];
    int plinelim;
    int pid;
    int fieldlen, nextcol;
    register row, col;
    register struct ent **p;
    char ch, lin[100];

    if (strcmp(fname, curfile) == 0) {
	(void) move (0, 0);
	(void) clrtoeol ();
	(void) sprintf (lin,
		"Confirm that you want to destroy the data base: (y,n)");
	(void) addstr (lin);
	(void) refresh();
	ch = nmgetch();
	if (ch != 'y' && ch != 'Y') 
	    return;
    }

    f = openout(fname, &pid);

    if (f==0) {
	error ("Can't create %s", fname);
	return;
    }
    for (row=r0;row<=rn; row++) {
	register c = 0;
	pline[plinelim=0] = '\0';
	for (p = &tbl[row][col=c0]; col<=cn;
	        p += nextcol-col, col = nextcol, c += fieldlen) {

	    fieldlen = fwidth[col];
	    nextcol = col+1;
	    if (*p) {
		char *s;

		while (plinelim<c) pline[plinelim++] = ' ';
		plinelim = c;
		if ((*p)->flags&is_valid) {
		    (void)sprintf (pline+plinelim,"%*.*f",fwidth[col],
		                                precision[col], (*p)->v);
		    plinelim += strlen (pline+plinelim);
		}
		if (s = (*p)->label) {
		    int slen;
		    char *start, *last;
		    register char *fp;
		    struct ent *nc;

		    /* Figure out if the label slops over to a blank field */
		    slen = strlen(s);
		    while (slen > fieldlen && nextcol <= cn &&
			    !((nc = lookat(row,nextcol))->flags & is_valid) &&
			    !(nc->label)) {
			
			fieldlen += fwidth[nextcol];
			nextcol++;
		    }
		    if (slen > fieldlen)
			slen = fieldlen;
		    
		    /* Now justify and print */
		    start = (*p)->flags & is_leftflush ? pline + c
					: pline + c + fieldlen - slen;
		    last = pline + c + fieldlen;
		    fp = plinelim < c ? pline + plinelim : pline + c;
		    while (fp < start)
			*fp++ = ' ';
		    while (slen--)
			*fp++ = *s++;
		    if (!((*p)->flags & is_valid) || fieldlen != fwidth[col])
			while(fp < last)
			    *fp++ = ' ';
		    if (plinelim < fp - pline)
			plinelim = fp - pline;
		}
	    }
	}
	(void) fprintf (f,"%.*s\n",plinelim,pline);
    }

    closeout(f, pid);
}

tblprintfile (fname, r0, c0, rn, cn)
char *fname;
{
    FILE *f;
    char pline[1000];
    int pid;
    register row, col;
    register struct ent **p;
    char coldelim = DEFCOLDELIM;
    char ch, lin[100];

    if (strcmp(fname, curfile) == 0) {
	(void) move (0, 0);
	(void) clrtoeol ();
	(void) sprintf (lin,
		"Confirm that you want to destroy the data base: (y,n)");
	(void) addstr (lin);
	(void) refresh();
	ch = nmgetch();
	if (ch != 'y' && ch != 'Y') 
	    return;
    }

    f = openout(fname, &pid);

    if (f==0) {
	error ("Can't create %s", fname);
	return;
    }
    for (row=r0; row<=rn; row++) {
	for (p = &tbl[row][col=c0]; col<=cn; col++, p++) {
	    if (*p) {
		char *s;
		if ((*p)->flags&is_valid) {
		    (void) fprintf (f,"%.*f",precision[col],
				(*p)->v);
		}
		if (s = (*p)->label) {
	            (void) fprintf (f,"%s",s);
		}
	    }
	    (void) fprintf(f,"%c",coldelim);
	}
	(void) fprintf (f,"\n",pline);
    }

    closeout(f, pid);
}

struct enode *
copye (e, Rdelta, Cdelta)
register struct enode *e; {
    register struct enode *ret;
    if (e==0) ret = 0;
    else {
	ret = (struct enode *) xmalloc ((unsigned) sizeof (struct enode));
	ret->op = e->op;
	switch (ret->op) {
	case 'v':
		{
		    int newrow, newcol;
		    newrow=e->e.v.vf & FIX_ROW ? e->e.v.vp->row :
						 e->e.v.vp->row+Rdelta;
		    newcol=e->e.v.vf & FIX_COL ? e->e.v.vp->col :
						 e->e.v.vp->col+Cdelta;
		    ret->e.v.vp = lookat (newrow, newcol);
		    ret->e.v.vf = e->e.v.vf;
		    break;
		}
	case 'k':
		ret->e.k = e->e.k;
		break;
	case 'f':
		ret->e.o.right = copye (e->e.o.right,0,0);
		ret->e.o.left = 0;
 		break;
	case '$':
		ret->e.s = xmalloc((unsigned) strlen(e->e.s)+1);
		(void) strcpy(ret->e.s, e->e.s);
		break;
 	case O_REDUCE('+'):
 	case O_REDUCE('*'):
 	case O_REDUCE('a'):
 	case O_REDUCE('s'):
 	case O_REDUCE(MAX):
 	case O_REDUCE(MIN):
		{
		    int newrow, newcol;
		    newrow=e->e.r.left.vf & FIX_ROW ?e->e.r.left.vp->row :
						     e->e.r.left.vp->row+Rdelta;
		    newcol=e->e.r.left.vf & FIX_COL ?e->e.r.left.vp->col :
						     e->e.r.left.vp->col+Cdelta;
		    ret->e.r.left.vp = lookat (newrow, newcol);
		    ret->e.r.left.vf = e->e.r.left.vf;
		    newrow=e->e.r.right.vf & FIX_ROW ?e->e.r.right.vp->row :
						    e->e.r.right.vp->row+Rdelta;
		    newcol=e->e.r.right.vf & FIX_COL ?e->e.r.right.vp->col :
						    e->e.r.right.vp->col+Cdelta;
		    ret->e.r.right.vp = lookat (newrow, newcol);
		    ret->e.r.right.vf = e->e.r.right.vf;
		    break;
		}
	default:
		ret->e.o.right = copye (e->e.o.right,Rdelta,Cdelta);
		ret->e.o.left = copye (e->e.o.left,Rdelta,Cdelta);
		break;
	}
    }
    return ret;
}

/*
 * sync_refs and syncref are used to remove references to
 * deleted struct ents.  Note that the deleted structure must still
 * be hanging around before the call, but not referenced by an entry
 * in tbl.  Thus the free_ent, fix_ent calls in sc.c
 */

sync_refs () {
    register i,j;
    register struct ent *p;
    sync_ranges();
    for (i=0; i<=maxrow; i++)
	for (j=0; j<=maxcol; j++)
	    if ((p=tbl[i][j]) && p->expr)
		syncref(p->expr);
}


syncref(e)
register struct enode *e;
{
    if (e==0)
	return;
    else {
	switch (e->op) {
	case 'v':
		e->e.v.vp = lookat(e->e.v.vp->row, e->e.v.vp->col);
		break;
	case 'k':
		break;
	case '$':
		break;
 	case O_REDUCE('+'):
 	case O_REDUCE('*'):
 	case O_REDUCE('a'):
 	case O_REDUCE('s'):
 	case O_REDUCE(MAX):
 	case O_REDUCE(MIN):
 		e->e.r.right.vp = lookat ( e->e.r.right.vp->row,
 		                                  e->e.r.right.vp->col);
 		e->e.r.left.vp = lookat ( e->e.r.left.vp->row,
 		                                 e->e.r.left.vp->col);
		break;
	default:
		syncref(e->e.o.right);
		syncref(e->e.o.left);
		break;
	}
    }
}

hiderow(arg)
{
    register int r1;
    register int r2;

    r1 = currow;
    r2 = r1 + arg - 1;
    if (r1 < 0 || r1 > r2) {
	error("Invalid Range");
	return;
    }
    if (r2 > MAXROWS-2) {
	error("You can't hide the last row");
	return;
    }
    FullUpdate++;
    while (r1 <= r2)
	row_hidden[r1++] = 1;
}

hidecol(arg)
{
    register int c1;
    register int c2;

    c1 = curcol;
    c2 = c1 + arg - 1;
    if (c1 < 0 || c1 > c2) {
	error("Invalid Range");
	return;
    }
    if (c2 > MAXCOLS-2) {
	error("You can't hide the last col");
	return;
    }
    FullUpdate++;
    while (c1 <= c2)
	col_hidden[c1++] = 1;
}

showrow(r1, r2)
{
    if (r1 < 0 || r1 > r2) {
	error("Invalid Range");
	return;
    }
    if (r2 > MAXROWS-1) {
	r2 = MAXROWS-1;
    }
    FullUpdate++;
    while (r1 <= r2)
	row_hidden[r1++] = 0;
}

showcol(c1, c2)
{
    if (c1 < 0 || c1 > c2) {
	error("Invalid Range");
	return;
    }
    if (c2 > MAXCOLS-1) {
	c2 = MAXCOLS-1;
    }
    FullUpdate++;
    while (c1 <= c2)
	col_hidden[c1++] = 0;
}

/* Open the output file, setting up a pipe if needed */

FILE *
openout(fname, rpid)
char *fname;
int *rpid;
{
    int pipefd[2];
    int pid;
    FILE *f;

    while (*fname && (*fname == ' '))  /* Skip leading blanks */
	fname++;

    if (*fname != '|') {		/* Open file if not pipe */
	*rpid = 0;
	return(fopen(fname, "w"));
    }

    fname++;				/* Skip | */
    if ( pipe (pipefd) < 0) {
	error("Can't make pipe to child");
	*rpid = 0;
	return(0);
    }

    deraw();
    if ((pid=fork()) == 0)			  /* if child  */
    {
	(void) close (0);			  /* close stdin */
	(void) close (pipefd[1]);
	(void) dup (pipefd[0]);		  /* connect to pipe input */
	(void) execl ("/bin/sh", "sh", "-c", fname, 0);
	exit (-127);
    }
    else				  /* else parent */
    {
	*rpid = pid;
	f = fdopen (pipefd[1], "w");
	if (f == 0)
	{
	    (void) kill (pid, -9);
	    error ("Can't fdopen output");
	    (void) close (pipefd[1]);
	    *rpid = 0;
	    return(0);
	}
    }
    return(f);
}

closeout(f, pid)
FILE *f;
int pid;
{
    int temp;

    (void) fclose (f);
    if (pid) {
         while (pid != wait(&temp)) /**/;
	 (void) printf("Press <return> to continue");
	 (void) fflush(stdout);
	 (void) nmgetch();
	 goraw();
    }
}
