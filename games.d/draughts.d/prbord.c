#include        "damdefs.h"

/* display routines for dam.c */
#define DELAY   7

extern int mvnr,game[],conv[],reconv[],bord[],*gp;
extern int timew,timeb,optg,playw,playb;
int ospeed;

int pb = 1;     /* nonzero if at begin of line */
int disp = 0;   /* nonzero if HP display desired */
int dirt = 0;   /* nonzero if garbage printed below board */
int prall = 1;  /* if nonzero the display is rewritten */

char prtarr[] = ".wzWZ?";
char *dsparr[] = {
        "\033&dJ  \0xxx",
        "\033&dJO \0xxx",
        "\033&dJX \0xxx",
        "O \0xxxxxxxxxx",
        "X \0xxxxxxxxxx",
        "? \0xxxxxxxxxx"
};

char *emparr[] = {
/* 0 */ "\033&dB  ",
/* 1 */ "\033&dJ  ",
/* 2 */ "\033&dJ%2d",
/* 3 */ "\033&dJ\016%2d\017",
/* 4 */ "\033&d@\016%2d\017",
        0 };

char *fularr[] = {
/* 0 */ "w ","z ","W ","Z ",
/* 1 */ "w ","z ","WW","ZZ",
/* 2 */ "w ","b ","W ","B ",
/* 3 */ "o ","x ","O ","X ",
/* 4 */ "\016V \017","\016$ \017","\016VV\017","\016$$\017",
/* 5 */ "\016[ \017","\016_ \017","\016[[\017","\016__\017",
/* 6 */ "\033&dJO ","\033&dJX ","O ","X ",
        0 };

/* cursor home and clear screen */
home(){
        printf("\033H\033J");
        dirt = 0;
}

/* position cursor */
int lastrow;
posit(l,c) int l,c; {
        printf("\033&a%dr%dC",l,c);
        lastrow = l;
        delay();
}

/* print string inverted (i.e. against white background) */
/* make sure no greek chars are printed */
whstr(s) char *s; {
        printf("\033&dB\017%s\033&d@",s);
        delay();
}

/* revert to black */
blstr(){
        printf("\033&d@");
        delay();
}

/* delay is necessary after changes of color or position */
delay(){
register int ct;
        ct = DELAY;
        if(ospeed == 300) ct = 1;
        while(ct--) putchar('\177');
}

int     col1 = 26;
int     col2 = 56;
#define COLW    14      /* width of column of moves */
#define ROW0    16      /* row of conversation below the board */

prmove(mvpt) int *mvpt; {
int fin,ct,inh;
        prmvnr();
        prnum(*mvpt++);
        fin = *mvpt++;
        ct = *mvpt++;
        inh = (ct>1 ? 1 : 0);
        if(!ct){
                putchar('-');
                prnum(fin);
        } else {
                putchar('x');
                prnum(fin);
                putchar('(');
                while(--ct){
                        prnum(*mvpt++);
                        putchar(',');
                }
                prnum(*mvpt++);
                putchar(')');
        }
        pb = mvnr&1;
        pb |= inh+optg;
        putchar(pb ? '\n' : '\t');
}

prnum(num) int num; {
        printf("%2d", reconv[ num & 0377 ]);
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
                        if(clr) clear(rw,cl,clr);
                        if(disp) posit(rw,cl);
                        printf("%2d.  ...\t", mvnr>>1);
                }
                if(disp) posit(rw, cl+COLW);
        } else {
                if(clr) clear(rw,cl,clr);
                if(disp) posit(rw, cl);
                printf("%2d. ", mvnr>>1);
        }
}

/* clear the line below the given line, and
   the given line only if it is line 1 */
clear(rw,cl,ct) int rw,cl,ct; {
register int i;
        if(!disp) return;
        if(rw == 1){
                posit(rw,cl);
                for(i=ct; i--; ) putchar(' ');
        }
        posit(rw+1,cl);
        for(i=ct; i--; ) putchar(' ');
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
        if(prall) {
                if(disp) home();
                pbundef();
        }
        if(!disp){
                if(!pb){
                        putchar('\n');
                        pb++;
                }
                putchar('\n');
        }
        if(disp){       /* write times above board */
                posit(1,5);
                printf("%s%4d \016:\017 %4d %s",
                        dsparr[3],timew,timeb,dsparr[4]);
                if(prall) printf("\n\n      \0161   2   3   4   5\017\n");
        }
        for(i=1; i<=50; i++){
                bdi = bord[conv[i]];
                if(disp && (prev_bord[conv[i]] == bdi)) continue;
                if((i/10)*10+1 == i)
                        if(!disp) putchar(' ');
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
                if(disp) {
                        /* first compute position */
                        yi = (i-1)/5;
                        xi = 4*(i - 5*yi) - 2;
                        if(yi&1) xi--; else xi++;
                        /* then display it there */
                        posit(yi+4, xi+2);
                        if(bdi) whstr(dsparr[bdi]);
                        else {
                                printf(dsparr[0],i);
                                blstr();
                        }
                        if((i%5 == 0) && (i%10 != 0))
                                printf("\016%2d\017\n \016%2d\017",i,i+1);
                } else {
                        putchar(prtarr[bdi]);
                        if(i%5 == 0) putchar('\n');
                        else putchar(' ');
                }
        }
        if(disp && prall)
                printf("\n  \016 46  47  48  49  50\017\n");
        if(prall) {
                prall = 0;
                if(disp){
                        rowdif = 0;
                        i = mvnr;       /* save temporarily */
                        mvnr = 1;
                        for(mp1 = game; mp1<gp; ){
                                mvnr++;
                                prmove(mp1);
                                mp1 += 2;
                                mp1 += *mp1++;
                        }
                        mvnr = i;
                }
        } else
        for(i = ROW0+dirt-2; i >= ROW0-1; i--)
                clear(i,0,col1);
        dirt = 0;
        if(disp) posit(ROW0,0);
        else putchar('\n');
        for(i=0; i<66; i++) prev_bord[i] = bord[i];
        if(prbwait) sleep(prbwait);
}

prposit(){
        prlist("w",WHITE);
        prlist("z",BLACK);
        prlist("wd",WHITE|DAM);
        prlist("zd",BLACK|DAM);
        printf(".\n");
}

prlist(s,c) char *s; int c; {
register int i;
int seen;
        seen = 0;
        for(i=1; i<=50; i++) if(bord[conv[i]] == c){
                if(!seen++) printf("%s: %d",s,i);
                else printf(",%d",i);
        }
        if(seen) printf("\n");
}

int fieldw,linpos;
putcr(c) char c; {
        putchar(c);
        linpos++;
        if(c == '\n') linpos = 0;
        else if(linpos == col1){
                putchar('\n');
                linpos = 0;
        }
        if(linpos == 0) dirt++;
}

printo(n) int n; {
        if(n){
                fieldw--;
                printo((n>>3) & 017777);
        }
        for(fieldw--; fieldw>0; fieldw--)
                putcr(' ');
        putcr('0' + (n & 07));
}

printd(n) int n; {
int a;
        if(a = n/10){
                fieldw--;
                printd(a);
        }
        for(fieldw--; fieldw > 0; fieldw--)
                putcr(' ');
        putcr('0' + n%10);
}

/*VARARGS1*/
pmesg(fmt,args) char *fmt; int *args; {
        int x;
        register char *s,c;
        register int *ap;

        ap = &args;
        if(disp && lastrow != ROW0) {
                posit(ROW0,0);
                linpos = 0;
        }
        while(c = *fmt++){
                fieldw = -1;
                if(c != '%') putcr(c);
                else {
                        c = *fmt++;
                        if(c >= '0' && c <= '9'){
                                fieldw = c-'0';
                                c = *fmt++;
                        }
                        switch(c){
                        case 'o':
                                printo(*ap++);
                                continue;
                        case 'd':
                                x = *ap++;
                                if(x < 0){
                                        putcr('-');
                                        printd(-x);
                                } else  printd(x);
                                continue;
                        case 's':
                                s = (char *) (*ap++);
                                while(c = *s++) putcr(c);
                                continue;
                        case 'c':
                                putcr(*ap++);
                                continue;
                        default:        /* in particular when c=0 */
                                error("bad string?");
                        }
                }
        }
}

extern int optf,crownmv,altermv,rdifmin;
outgame(){
int mvnr0;
register int *mp1;
        outfil();
        mvnr0 = mvnr;
        mvnr = 1;
        printf("***** dam%s version of %s *****\n",VERSION,DATE);
        printf("rdifmin = %d\n", rdifmin);
        if(crownmv) printf("crownmv = %d\n", crownmv);
        if(altermv) printf("altermv = %d\n", altermv);
        if(playw == USER) printf("user has white\n");
        if(playb == USER) printf("user has black\n");
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

int disp0,fterm;

outfil(){
        if(optf<0) {
                pmesg("no file\n");
                return;
        }
        fterm = dup(1);
        (void) close(1);
        if(dup(optf) != 1) error("f not 1 in outfil");
        disp0 = disp;
        disp = 0;
}

outterm(){
        disp = disp0;
        (void) close(1);
        if(dup(fterm) != 1) error("f not 1 in outterm");
        (void) close(fterm);
}

initterm(){}
