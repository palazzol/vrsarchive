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



#include <curses.h>
#include <signal.h>
#include <setjmp.h>
#include "sc.h"
#include <ctype.h>
#include "y.tab.h"

char *strtof();

jmp_buf wakeup;

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

#define ctl(x) ('x'&037)

yylex () {
    register char *p = line+linelim;
    int ret = -1;
    while (isspace(*p)) p++;
    if (*p==0) ret = -1;
    else if (isalpha(*p)) {
	char *tokenst = p;
	register tokenl;
	register struct key *tbl;
	while (isalpha(*p)) p++;
	if (p-tokenst <= 2) { /* a COL is 1 or 2 char alpha */
	    register  col;
	    ret = COL;
	    col = ((tokenst[0] & 0137) - 'A');
	    if (p == tokenst+2)
		col = (col + 1)*26 + ((tokenst[1] & 0137) - 'A');
	    yylval.ival =  col;
	} else {
	    ret = WORD;
	    tokenl = p-tokenst;
	    for (tbl = linelim ? experres : statres; tbl->key; tbl++)
		    if (((tbl->key[0]^tokenst[0])&0137)==0
		     && tbl->key[tokenl]==0) {
			register i = 1;
			while (i<tokenl && ((tokenst[i]^tbl->key[i])&0137)==0)
			    i++;
			if (i>=tokenl) {
			    ret = tbl->val;
			    break;
			}
		    }
	    if (ret==WORD) { 
		linelim = p-line;
		yyerror ("Unintelligible word");
	    }
	}
    } else if ((*p == '.') || isdigit(*p)) {
	register long v = 0;
	char *nstart = p;
	if (*p != '.') {
	    do v = v*10 + (*p-'0');
	    while (isdigit(*++p));
	}
	if (*p=='.' || *p == 'e' || *p == 'E') {
	    ret = FNUMBER;
	    p = strtof(nstart, &yylval.fval);
	} else {
            if((int)v != v)
            {
                ret = FNUMBER;
                yylval.fval = v;
            }
            else
            {
 
                ret = NUMBER;
                yylval.ival = v;
            }
	}
    } else if (*p=='"') {
	/* This storage is never freed.  Oh well.  -MDW */
	char *ptr;
        ptr = p+1;
        while(*ptr && *ptr++ != '"');
        ptr = (char *)malloc((unsigned)(ptr-p));
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

#ifndef KEY_LEFT
#define N_KEY 4

struct key_map {
    char *k_str;
    char k_val;
    char k_index;
}; 

struct key_map km[N_KEY];

char keyarea[N_KEY*10];

char *tgetstr();

initkbd()
{
    char buf[1024]; /* Why do I have to do this again? */
    char *p = keyarea;
    char *ks;
    int i;

    if (tgetent(buf, getenv("TERM")) <= 0)
	return;

    km[0].k_str = tgetstr("kl", &p); km[0].k_val = ctl(b);
    km[1].k_str = tgetstr("kr", &p); km[1].k_val = ctl(f);
    km[2].k_str = tgetstr("ku", &p); km[2].k_val = ctl(p);
    km[3].k_str = tgetstr("kd", &p); km[3].k_val = ctl(n);
    ks = tgetstr("ks",&p);
    if (ks) 
	printf("%s",ks);
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

    void timeout();

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

        signal(SIGALRM, timeout);
        alarm(1);

	if (setjmp(wakeup) == 0) { 
	    maybe = nmgetch();
	    alarm(0);
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

#else /*KEY_LEFT*/

initkbd()
{
    keypad(stdscr, TRUE);
}

nmgetch()
{
    register int c;

    c = getch();
    switch (c) {
    case KEY_LEFT:  c = ctl(b); break;
    case KEY_RIGHT: c = ctl(f); break;
    case KEY_UP:    c = ctl(p); break;
    case KEY_DOWN:  c = ctl(n); break;
    default:   c = c & 0x7f; 
    }
    return (c);
}

#endif /*KEY_LEFT*/

void
timeout()
{
    longjmp(wakeup, -1);
}

int dbline;

debug (fmt, a, b, c) {
	mvprintw (2+(dbline++%22),80-60,fmt,a,b,c);
	clrtoeol();
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

    return(p);
}

help () {
    move(2,0);
    clrtobot();
    dbline = 0;
    debug ("                 Cursor cmds:");
    debug ("  ^n j next row       ^p k prev. row      ^g erase cmd");
    debug ("  ^f l fwd col        ^b h back col       ^r redraw screen");
    debug ("   0 $ first, end col");
    debug ("                 Cell cmds:");
    debug (" \" < > enter label       = enter value     x clear cell");
    debug ("     c copy cell         m mark cell      ^t line 1 on/off");  
    debug ("    ^a type value       ^e type expr.     ^v type vbl name");
    debug ("                 Row, Column cmds:");
    debug (" ar ac dup           ir ic insert      sr sc show");
    debug (" dr dc delete        zr zc hide        pr pc pull");
    debug (" vr vc value only        f format");
    debug ("                 File cmds:");
    debug ("     G get database      M merge database  T write tbl fmt");
    debug ("     P put database      W write listing");
    debug ("                 Misc. cmds:");
    debug (" ^c, q quit              / copy region    pm pull (merge)");
    debug ("                 Expression Operators");
    debug ("  +-*/ arithmetic     ?e:e conditional   & | booleans");
    debug (" < = > relations     <= >= relations      != relations");
    debug ("       @sum(v1:v2)         @avg(v1:v2)       @prod(v1:v2)");
}
