#include        "damdefs.h"
#include	<curses.h>

/* display routines for dam.c */
#define DELAY   7

extern int mvnr,game[],conv[],reconv[],bord[],*gp;
extern int timew,timeb,optg,playw,playb;
extern unsigned sleep();

int pb = 1;     /* nonzero if at begin of line */

char *dsparr[] = {
        "  \0xxx",
        "o \0xxx",
        "x \0xxx",
        "O \0xxxxxxxxxx",
        "X \0xxxxxxxxxx",
        "? \0xxxxxxxxxx"
};

char *emparr[] = {
/* 0 */ "  ",
/* 1 */ "  ",
/* 2 */ "%2d",
/* 3 */ "%2d",
/* 4 */ "%2d",
        0 };

char *fularr[] = {
/* 0 */ "w ","z ","W ","Z ",
/* 1 */ "w ","z ","WW","ZZ",
/* 2 */ "w ","b ","W ","B ",
/* 3 */ "o ","x ","O ","X ",
/* 4 */ "V ","$ ","VV","$$",
/* 5 */ "[ ","_ ","[[","__",
/* 6 */ "O ","X ","OO","XX",
        0 };

/* cursor home and clear screen */
home(){
        clear();
        move(0,0);
}

/* position cursor */
posit(l,c) int l,c; {
        move(l,c);
}

int     col1 = 26;
int     col2 = 50;
#define COLW    14      /* width of column of moves */

prmove(mvpt) int *mvpt; {
int fin,ct,inh;
        prmvnr();
        prnum(*mvpt++);
        fin = *mvpt++;
        ct = *mvpt++;
        inh = (ct>1 ? 1 : 0);
        if(!ct){
                addch('-');
                prnum(fin);
        } else {
                addch('x');
                prnum(fin);
                addch('(');
                while(--ct){
                        prnum(*mvpt++);
                        addch(',');
                }
                prnum(*mvpt++);
                addch(')');
        }
        pb = mvnr&1;
        pb |= inh+optg;
        addch(pb ? '\n' : '\t');
}

prnum(num) int num; {
        printw("%2d", reconv[ num & 0377 ]);
}

/* define printing position before printing a move */
int rowdif;

prmvnr(){
int cl,rw,clr;
        clr = 0;
        rw = (mvnr>>1) + rowdif;
        if((mvnr&1) && pb) rw++;
        while(rw > 88) rw -= 44;
        if(rw > 66){
                rw -= 66;
                cl = col2;
                clr = 80-col2;
        } else if(rw > 44){
                rw -= 44;
                cl = col1;
                clr = col2-col1;
        } else if(rw > 22){
                rw -= 22;
                cl = col2;
        } else  cl = col1;
        if(mvnr&1){
                if(pb){
                        rowdif++;
                        if(clr) Clear(rw,cl,clr);
                        posit(rw,cl);
                        printw("%2d.  ...\t", mvnr>>1);
                }
                posit(rw, cl+COLW);
        } else {
                if(clr) Clear(rw,cl,clr);
                posit(rw, cl);
                printw("%2d. ", mvnr>>1);
        }
}

/* clear the line below the given line, and
   the given line only if it is line 1 */
Clear(rw,cl,ct) int rw,cl,ct; {
register int i;
        if(rw == 1){
                posit(rw,cl);
                for(i=ct; i--; ) addch(' ');
        }
        posit(rw+1,cl);
        for(i=ct; i--; ) addch(' ');
}

int prev_bord[66];      /* remember, and print differences only */
unsigned prbwait = 0;   /* delay after each prbord() */

pbundef(){      /* force printing of entire board */
register int i;
        for(i=0; i<66; i++) prev_bord[i] = -1;
}

prbord(){
register int i,*mp1;
int bdi,xi,yi;
	erase();
        pbundef();
        posit(1,5);
        printw("%s%4d : %4d %s",dsparr[3],timew,timeb,dsparr[4]);
        printw("\n\n     1   2   3   4   5\n");
        for(i=1; i<=50; i++){
                bdi = bord[conv[i]];
                if(prev_bord[conv[i]] == bdi) continue;
                switch(bdi){
                case EMPTY:
                        bdi = 0; break;
                case WHITE:
                        bdi = 1; break;
                case BLACK:
                        bdi = 2; break;
                case WHITE+DAM:
                        bdi = 3; break;
                case BLACK+DAM:
                        bdi = 4; break;
                default:
                        bdi = 5;
                }
                /* first compute position */
                yi = (i-1)/5;
                xi = 4*(i - 5*yi) - 2;
                if(yi&1) xi--; else xi++;
                /* then display it there */
                posit(yi+4, xi+2);
                printw(dsparr[bdi]);
                if((i%5 == 0) && (i%10 != 0))
                        printw("%2d\n%2d ",i,i+1);
        }
        printw("\n  46  47  48  49  50\n");
        i = mvnr;       /* save temporarily */
        mvnr = 1;
        rowdif = 0;
        for(mp1 = game; mp1<gp; ){
                mvnr++;
                prmove(mp1);
                mp1 += 2;
                mp1 += *mp1++;
        }
        mvnr = i;
        posit(ROW0,0);
	refresh();
        for(i=0; i<66; i++) prev_bord[i] = bord[i];
        if(prbwait) sleep(prbwait);
}

prposit(){
        prlist("w",WHITE);
        prlist("z",BLACK);
        prlist("wd",WHITE|DAM);
        prlist("zd",BLACK|DAM);
        printw(".\n");
}

prlist(s,c) char *s; int c; {
register int i;
int seen;
        seen = 0;
        for(i=1; i<=50; i++) if(bord[conv[i]] == c){
                if(!seen++) printw("%s: %d",s,i);
                else printw(",%d",i);
        }
        if(seen) printw("\n");
}

int fieldw;
printo(n) int n; {
        if(n){
                fieldw--;
                printo((n>>3) & 017777);
        }
        for(fieldw--; fieldw>0; fieldw--)
                addch(' ');
        addch('0' + (n & 07));
}

printd(n) int n; {
int a;
        if(a = n/10){
                fieldw--;
                printd(a);
        }
        for(fieldw--; fieldw > 0; fieldw--)
                addch(' ');
        addch('0' + n%10);
}

extern int optf,crownmv,altermv,rdifmin;
outgame(){
int mvnr0;
register int *mp1;
        outfil();
        mvnr0 = mvnr;
        mvnr = 1;
        printw("***** dam%s version of %s *****\n",VERSION,DATE);
        printw("rdifmin = %d\n", rdifmin);
        if(crownmv) printw("crownmv = %d\n", crownmv);
        if(altermv) printw("altermv = %d\n", altermv);
        if(playw == USER) printw("user has white\n");
        if(playb == USER) printw("user has black\n");
        for(mp1=game; mp1<gp; ){
                mvnr++;
                prmove(mp1);
                mp1 += 2;
                mp1 += *mp1++;
        }
        prbord();
        rdcomd("t");    /* %% recursive call */
        mvnr = mvnr0;
        outterm();
}

outboard(){
        outfil();
        prbord();
        outterm();
}

outposit(){
        outfil();
        prposit();
        outterm();
}

int fterm;

outfil(){
        if(optf<0) {
                mvprintw(ROW0,0,"No file\n");
                return;
        }
        fterm = dup(1);
        (void) close(1);
        if(dup(optf) != 1) error("f not 1 in outfil");
}

outterm(){
        (void) close(1);
        if(dup(fterm) != 1) error("f not 1 in outterm");
        (void) close(fterm);
}

initterm(){
	initscr();
	crmode();
	nl();
}
