/*	SC	A Spreadsheet Calculator
 *		Lexical analyser
 *
 *		original by James Gosling, September 1982
 *		modifications by Mark Weiser and Bruce Israel,
 *			University of Maryland
 *
 *              More mods Robert Bond, 12/86
 *
 */



#if defined(BSD42) || defined(BSD43)
#include <sys/ioctl.h>
#endif 

#include <curses.h>
#include <signal.h>
#include <setjmp.h>
#include "sc.h"
#include <ctype.h>

#ifdef BSD42
#include <strings.h>
#else
#ifndef SYSIII
#include <string.h>
#endif
#endif

#include "y.tab.h"

char *strtof();

jmp_buf wakeup;
jmp_buf fpe_buf;

struct key {
    char *key;
    int val;
};

struct key experres[] = {
#include "experres.h"
    0, 0};

struct key statres[] = {
#include "statres.h"
    0, 0};

yylex () {
    register char *p = line+linelim;
    int ret;
    while (isspace(*p)) p++;
    if (*p==0) ret = -1;
    else if (isalpha(*p)) {
	char *tokenst = p;
	register tokenl;
	register struct key *tblp;
	tokenl = 0;
	/*
	 * This picks up either 1 or 2 alpha characters (a column) or
	 * tokens with at least three leading alphas and '_' or digits
	 * (a function or token or command or a range name)
	*/
	while (isalpha(*p) || ((*p == '_') || isdigit(*p)) && (tokenl > 2)) {
	    p++;
	    tokenl++;
	}
	if (tokenl <= 2) {
	    register  col;  /* a COL is 1 or 2 char alpha (and not pi or ln!) */
	    if (tokenl == 2 && tokenst[0] == 'p' && tokenst[1] == 'i') {
		ret = K_PI;
	    } else if (tokenl == 2 && tokenst[0] == 'l' && tokenst[1] == 'n') {
		ret = K_LN;
	    } else {
		ret = COL;
		col = ((tokenst[0] & 0137) - 'A');
		if (p == tokenst+2)
		    col = (col + 1)*26 + ((tokenst[1] & 0137) - 'A');
		yylval.ival =  col;
	    }
	} else {
	    ret = WORD;
	    for (tblp = linelim ? experres : statres; tblp->key; tblp++)
		    if (((tblp->key[0]^tokenst[0])&0137)==0
		     && tblp->key[tokenl]==0) {
			register i = 1;
			while (i<tokenl && ((tokenst[i]^tblp->key[i])&0137)==0)
			    i++;
			if (i>=tokenl) {
			    ret = tblp->val;
			    break;
			}
		    }
	    if (ret==WORD) { 
		struct range *r;
		if (r = find_range(tokenst, tokenl,
				   (struct ent *)0, (struct ent *)0)) {
		    yylval.rval.left = r->r_left;
		    yylval.rval.right = r->r_right;
		    if (r->r_is_range)
		        ret = RANGE;
		    else
			ret = VAR;
		} else {
		    linelim = p-line;
		    yyerror ("Unintelligible word");
		}
	    }
	}
    } else if ((*p == '.') || isdigit(*p)) {
	double v = 0;
	int temp;
	char *nstart = p;
	if (*p != '.') {
	    do v = v*10 + (double)(*p-'0');
	    while (isdigit(*++p));
	}
	if (*p=='.' || *p == 'e' || *p == 'E') {
	    ret = FNUMBER;
	    p = strtof(nstart, &yylval.fval);
	} else {
	    /* A NUMBER must hold at least MAXROW and MAXCOL */
	    /* This is consistent with a short row and col in struct ent */
	    if (v > (double)32767 || v < (double)-32768) {
		ret = FNUMBER;
		yylval.fval = v;
	    } else {
		temp = (int)v;
		if((double)temp != v) {
		    ret = FNUMBER;
		    yylval.fval = v;
		} else {
		    ret = NUMBER;
		    yylval.ival = temp;
		}
	    }
	}
    } else if (*p=='"') {
	char *ptr;
        ptr = p+1;
        while(*ptr && *ptr++ != '"');
        ptr = xmalloc((unsigned)(ptr-p));
	yylval.sval = ptr;
	p += 1;
	while (*p && *p!='"') *ptr++ = *p++;
	*ptr = 0;
	if (*p) p += 1;
	ret = STRING;
    } else if (*p=='[') {
	while (*p && *p!=']') p++;
	if (*p) p++;
	linelim = p-line;
	return yylex();
    } else ret = *p++;
    linelim = p-line;
    return ret;
}

#ifdef SIMPLE

initkbd()
{}

kbd_again()
{}

resetkbd()
{}

nmgetch()
{
    return (getchar() & 0x7f);
}

#else /*SIMPLE*/

#if defined(BSD42) || defined (SYSIII) || defined(BSD43)

#define N_KEY 4

struct key_map {
    char *k_str;
    char k_val;
    char k_index;
}; 

struct key_map km[N_KEY];

char keyarea[N_KEY*10];

char *tgetstr();
char *getenv();
char *ks;
char ks_buf[20];
char *ke;
char ke_buf[20];

#ifdef TIOCSLTC
struct ltchars old_chars, new_chars;
#endif

char dont_use[] = {
    ctl('z'), ctl('r'), ctl('l'), ctl('b'), ctl('c'), ctl('f'), ctl('g'),
	ctl('['), ctl('h'), ctl('m'), ctl('j'), ctl('n'), ctl('p'), ctl('q'),
	ctl('s'), ctl('t'), ctl('u'), ctl('v'), ctl('e'), ctl('a'), ctl('i'),
	ctl('w'), 0,
};

charout(c)
int c;
{
	(void)putchar(c);
}

initkbd()
{
    register struct key_map *kp;
    register i,j;
    char *p = keyarea;
    char *ktmp;
    static char buf[1024]; /* Why do I have to do this again? */

    if (tgetent(buf, getenv("TERM")) <= 0)
	return;

    km[0].k_str = tgetstr("kl", &p); km[0].k_val = ctl('b');
    km[1].k_str = tgetstr("kr", &p); km[1].k_val = ctl('f');
    km[2].k_str = tgetstr("ku", &p); km[2].k_val = ctl('p');
    km[3].k_str = tgetstr("kd", &p); km[3].k_val = ctl('n');
    ktmp = tgetstr("ks",&p);
    if (ktmp)  {
	(void) strcpy(ks_buf, ktmp);
	ks = ks_buf;
	tputs(ks, 1, charout);
    }
    ktmp = tgetstr("ke",&p);
    if (ktmp)  {
	(void) strcpy(ke_buf, ktmp);
	ke = ke_buf;
    }

    /* Unmap arrow keys which conflict with our ctl keys   */
    /* Ignore unset, longer than length 1, and 1-1 mapped keys */

    for (i = 0; i < N_KEY; i++) {
	kp = &km[i];
	if (kp->k_str && (kp->k_str[1] == 0) && (kp->k_str[0] != kp->k_val))
	    for (j = 0; dont_use[j] != 0; j++)
	        if (kp->k_str[0] == dont_use[j]) {
		     kp->k_str = (char *)0;
		     break;
		}
    }


#ifdef TIOCSLTC
    (void)ioctl(fileno(stdin), TIOCGLTC, (char *)&old_chars);
    new_chars = old_chars;
    if (old_chars.t_lnextc == ctl('v'))
	new_chars.t_lnextc = -1;
    if (old_chars.t_rprntc == ctl('r'))
	new_chars.t_rprntc = -1;
    (void)ioctl(fileno(stdin), TIOCSLTC, (char *)&new_chars);
#endif
}

kbd_again()
{
    if (ks) 
	tputs(ks, 1, charout);

#ifdef TIOCSLTC
    (void)ioctl(fileno(stdin), TIOCSLTC, (char *)&new_chars);
#endif
}

resetkbd()
{
    if (ke) 
	tputs(ke, 1, charout);

#ifdef TIOCSLTC
    (void)ioctl(fileno(stdin), TIOCSLTC, (char *)&old_chars);
#endif
}

nmgetch() 
{
    register int c;
    register struct key_map *kp;
    register struct key_map *biggest;
    register int i;
    int almost;
    int maybe;

    static char dumpbuf[10];
    static char *dumpindex;

#ifdef SYSV3
    void atimeout();
#else
    int atimeout();
#endif

    if (dumpindex && *dumpindex)
	    return (*dumpindex++);

    c = getchar() & 0x7f;
    biggest = 0;
    almost = 0;

    for (kp = &km[0]; kp < &km[N_KEY]; kp++) {
	if (!kp->k_str)
	    continue;
	if (c == kp->k_str[kp->k_index]) {
	    almost = 1;
	    kp->k_index++;
	    if (kp->k_str[kp->k_index] == 0) {
		c = kp->k_val;
       	        for (kp = &km[0]; kp < &km[N_KEY]; kp++)
	            kp->k_index = 0;
	        return(c);
	    }
	}
	if (!biggest && kp->k_index)
	    biggest = kp;
        else if (kp->k_index && biggest->k_index < kp->k_index)
	    biggest = kp;
    }

    if (almost) { 

        (void) signal(SIGALRM, atimeout);
        (void) alarm(1);

	if (setjmp(wakeup) == 0) { 
	    maybe = nmgetch();
	    (void) alarm(0);
	    return(maybe);
	}

    }
    
    if (biggest) {
	for (i = 0; i<biggest->k_index; i++) 
	    dumpbuf[i] = biggest->k_str[i];
	dumpbuf[i++] = c;
	dumpbuf[i] = 0;
	dumpindex = &dumpbuf[1];
       	for (kp = &km[0]; kp < &km[N_KEY]; kp++)
	    kp->k_index = 0;
	return (dumpbuf[0]);
    }

    return(c);
}

#endif

#if defined(SYSV2) || defined(SYSV3)

initkbd()
{
    keypad(stdscr, TRUE);
}

kbd_again()
{
    keypad(stdscr, TRUE);
}

resetkbd()
{
    keypad(stdscr, FALSE);
}

nmgetch()
{
    register int c;

    c = getch();
    switch (c) {
    case KEY_LEFT:  c = ctl('b'); break;
    case KEY_RIGHT: c = ctl('f'); break;
    case KEY_UP:    c = ctl('p'); break;
    case KEY_DOWN:  c = ctl('n'); break;
    default:   c = c & 0x7f; 
    }
    return (c);
}

#endif /* SYSV2 || SYSV3 */

#endif /* SIMPLE */

#ifdef SYSV3
void
#endif
atimeout()
{
    longjmp(wakeup, -1);
}

int dbline;

/*VARARGS*/
void
debug (str)
char *str;
{
	(void) mvprintw (2+(dbline++%22),80-70,str);
	(void) clrtoeol();
}

SIG_T
fpe_trap(dummy)
{
    longjmp(fpe_buf, 1);
}

/*
 * This converts a floating point number of the form
 * [s]ddd[.d*][esd*]  where s can be a + or - and e is E or e.
 * to floating point. 
 * p is advanced.
 */

char *
strtof(p, res)
register char *p;
double *res;
{
    double acc;
    int sign;
    double fpos;
    int exp;
    int exps;
#ifdef SYSV3
    void quit();
#else
    int quit();
#endif

    (void) signal(SIGFPE, fpe_trap);
    if (setjmp(fpe_buf)) {
	error("Floating point exception\n");
	*res = 0.0; 
        (void) signal(SIGFPE, quit);
	return(p);
    }
    acc = 0.0;
    sign = 1;
    exp = 0;
    exps = 1;
    if (*p == '+')
        p++;
    else if (*p == '-') {
        p++;
        sign = -1;
    }
    while (isdigit(*p)) {
        acc = acc * 10.0 + (double)(*p - '0');
        p++;
    }
    if (*p == 'e' || *p == 'E') {
	    p++;
        if (*p == '+')
	    p++;
        else if (*p == '-') {
	    p++;
	    exps = -1;
        }
        while(isdigit(*p)) {
	    exp = exp * 10 + (*p - '0');
	    p++;
        }
    }
    if (*p == '.') {
	fpos = 1.0/10.0;
	p++;
	while(isdigit(*p)) {
	    acc += (*p - '0') * fpos;
	    fpos *= 1.0/10.0;
	    p++;
	}
    }
    if (*p == 'e' || *p == 'E') {
	exp = 0;
	exps = 1;
        p++;
	if (*p == '+')
	    p++;
	else if (*p == '-') {
	    p++;
	    exps = -1;
	}
	while(isdigit(*p)) {
	    exp = exp * 10 + (*p - '0');
	    p++;
	}
    }
    if (exp) {
	if (exps > 0)
	    while (exp--)
		acc *= 10.0;
	else
	    while (exp--)
		acc *= 1.0/10.0;
    }
    if (sign > 0)
        *res = acc;
    else
	*res = -acc;

    (void) signal(SIGFPE, quit);
    return(p);
}

help () {
    (void) move(1,0);
    (void) clrtobot();
    dbline = 0;
    debug ("Cursor:     ^n j next row       ^p k prev. row  ESC ^g erase cmd");
    debug ("            ^f l fwd col        ^b h back col    ^l ^r redraw screen");
    debug ("               w fwd to next       b back to next   ^e <dir> end");
    debug ("               0 col A             $ last col        g goto ");
    debug ("               ^ row 0             # last row");
    debug ("Cell:      \" < > enter label       = enter value     x clear cell");
    debug ("               c copy cell         m mark cell      ^t line 1 on/off");  
    debug ("              ^a type value       ^w type expr.     ^v type vbl name");
    debug ("Row, Col:  ar ac dup           ir ic insert      sr sc show");
    debug ("           dr dc delete        zr zc hide        pr pc pull");
    debug ("           vr vc value only        f format");
    debug ("Ranges:       /u undefine range   /s show ranges    /d define range");
    debug ("              /c copy range       /x clear range    /f fill range");
    debug ("              /v values only");
    debug ("File:          G get database      M merge database  T write tbl fmt");
    debug ("               P put database      W write listing   R run macros");
    debug ("               D define directory");
    debug ("Misc:        Q q quit             pm pull (merge)");
    debug ("Expr:      +-*/^ arithmetic     ?e:e conditional   & | booleans");
    debug ("           < = > relations     <= >= relations      != relations");
    debug ("                 @sum(range)         @avg(range)       @prod(range)");
    debug ("                 @func(e) - lots of other math functions");
    error("Hit any key to continue ");
    (void) refresh();
    (void) nmgetch();
    FullUpdate++;
    (void) move(1,0);
    (void) clrtobot();
}
