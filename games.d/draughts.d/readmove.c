#include        <stdio.h>
#include        "damdefs.h"

/* read move; the intended format is
        nn-mm
   or
        nnxmm
   or
        nnxmm(q1,...,qt)
 */

extern int      dirt,prall,possct,captmax,captct,timew,timeb,timemax;
extern long     lseek();
extern int      optp,rdif,disp,playw,playb,me,optg,optu,optf;
extern unsigned prbwait;
extern int      rdifmin,altermv,fplevel,rdmin;
extern int      *mp,*mpf,conv[],bord[],moves[],mvnr,*findmove();
extern char     *dsparr[],*emparr[],*fularr[],prtarr[];

int valstone[5] = {
        EMPTY,WHITE,BLACK,WHITE|DAM,BLACK|DAM
};

#define LINSIZ  82
char line[LINSIZ];              /* terminal input line */
char ofile[LINSIZ];             /* output filename */
char ch, *ach;
#define ERRCHAR '\177'

rdlin(){
        register char *lp;
        register int c;

        lp = line;
        while((c = getchar()) != '\n'){
                if(c == EOF) error("end of input");
                if(lp == line+LINSIZ-1){
                        lp--;
                        c = ERRCHAR;
                }
                *lp++ = c;
        }
        *lp = 0;
        ach = line;
}

newchar(){
        return(ch = *ach++);
}

answer(){
ask:
        rdlin();
        switch(newchar()){
        case '!':
                rdcomd(line);
                goto ask;
        case 'j':
        case 'y':
                return(1);
        case 'n':
                return(0);
        default:
                pmesg("answer 'y' or 'n'\n");
                goto ask;
        }
}

skipspaces(){
        while((ch == ' ') || (ch == '\t')) newchar();
}

rnum(){
int num;
        num = 0;
        skipspaces();
        while(digit()) {
                num = 10*num + (ch - '0');
                newchar();
        }
        return(num);
}

digit(){
        return((ch >= '0') && (ch <= '9'));
}

letter(){
register char c;
        c = ch|040;
        return((c >= 'a') && (c <= 'z'));
}

readnum(){
int     num;
        num = rnum();
        skipspaces();
        if((num < 0) || (num > 50)) num = 0;
        return(conv[num]);
}

check(cc) char cc; {
        if(ch != cc){
                putcr('\'');
                putsym(ch);
                pmesg("' where '");
                putsym(cc);
                pmesg("' expected\n");
                ch = ERRCHAR;           /* some error occurred */
        } else if(ch) newchar();
}

putsym(cc) char cc; {
        if(!cc) pmesg("\\n");
        else if(cc < ' ') pmesg("\\0%o",cc);
        else putcr(cc);
}

/*
 * a alter      * k             * u uit eval
 * b black      * l             * v value
 * c crown      * m move back   * w white
 * d define     * n no HP use   * x exit
 * e end        * o output      * y you
 * f file,fp    * p print       * z zwart
 * g (game)     * q             * = remise
 * h halt       * r read,rdepth * I I play
 * i info       * s sleep       * : (debug)
 * j            * t time        * \0 (no random)
 */

int     *youmv = 0;

rdcomd(ac) char *ac; {
register char *sp,*sp2;
register int i;
int ct;
        if(*ac == '!') ac++;
        ach = ac;       /* read from argstring instead of array line */
        switch(newchar()){
        case 'w':
                playw = PDP;
                break;
        case 'b':
        case 'z':
                playb = PDP;
                break;
        case 'I':
                newchar();
                if(ch == 'w') playw = USER;
                else if(ch == 'b' || ch == 'z') playb = USER;
                else playw = playb = USER;
                break;
        case 'y':
                youmv = findmove(me);
                break;
        case 'h':
                while(1){
                        rdlin();
                        if(*ach == '.') break;
                        rdcomd(ach);
                }
                break;
        case 'p':
                newchar();
                if(ch == 'b'){
                        pbundef();
                        newchar();
                } else  prall++;
                if(digit()) optp = rnum();
                prbord();
                break;
        case 'm':
                newchar();
                if(i = readcol()){
                        me = i;
                        mpf = 0;
                        break;
                }
                if(ch == '-'){
                        newchar();
                        i = mvnr-rnum()-2;
                } else  i = rnum();
                check(0);
                if(ch == ERRCHAR) goto ask;
                if(i<0 || i+2>mvnr){
                        pmesg("impossible\n");
                        goto ask;
                }
                backup(i);
                prbord();
                break;
        case 'n':
                if(disp) home();
                disp = 0;
                break;
        case 'a':
                /*
                 * awk<nr>: alter field <nr> into white king
                 * a<nr>: make the nonempty field <nr> empty
                 */
                newchar();
                ct = readstone();
        alp:
                i = readnum();
                if(!i){
                        pmesg("bad field\n");
                        goto ask;
                }
                if((!ct) && (bord[i] == EMPTY)){
                        pmesg("empty already\n");
                        goto ask;
                }
                bord[i] = valstone[ct];
                altermv = mvnr;
                mpf = 0;
                if(ch == ','){
                        newchar();
                        goto alp;
                }
                check(0);
                if(ch == ERRCHAR) goto ask;
                break;
        case 'f':
                if(newchar() == 'p'){
                        if(!newchar()){
                                pmesg("freepath values:\n");
                                pmesg("w: %d\n",freepath(WHITE));
                                pmesg("z: %d\n",freepath(BLACK));
                                break;
                        }
                        fplevel = rnum()-1;
                        break;
                }
                if(letter()){
                        pmesg("What?\n");
                        goto ask;
                }
                skipspaces();
                sp = ofile;
                while(*sp++ = ch){
                        if(sp > ofile+LINSIZ) goto ask;
                        newchar();
                }
                if(optf >= 0) (void) close(optf);
                optf = open(ofile,1);
                if(optf < 0) optf=creat(ofile,0666);
                else (void) lseek(optf, 0L, 2);
                if(optf < 0) pmesg("cannot create %s\n",ofile);
                break;
        case 'o':
                if(newchar() == 'g') outgame();
                else if(ch == 'b') outboard();
                else if(ch == 'p') outposit();
                else {
                        pmesg("what?");
                        goto ask;
                }
                break;
        case 'r':
                newchar();
                /*
                 * r: set rdif
                 * rm: set rdmin
                 * rf: read pos from file
                 * rt: read pos from terminal
                 */
                if(ch == 'f') readfile();
                else if(ch == 't') readpos();
                else if(ch == 'm') {
                        newchar();
                        rdmin = rnum();
                        pmesg("rdmin = %d\n",rdmin);
                }
                else {
                        rdif = rnum();
                        pmesg("rdif = %d\n",rdif);
                        if(mvnr == 1)rdifmin = rdif;
                        else if(rdif<rdifmin)rdifmin = rdif;
                }
                break;
        case 's':
                newchar();
                prbwait = rnum();
                if(prbwait>30) prbwait = 30;
                pmesg("sleep after prbord for %d seconds\n",
                        prbwait);
                break;
        case 't':
                newchar();
                if(digit()){
                        timemax = rnum();
                        pmesg("allowed time per move: %d sec\n",timemax);
                }
                pmesg("time used:\n");
                pmesg("  W %4d sec\n",timew);
                pmesg("  B %4d sec\n",timeb);
                break;
        case 'c':
                /* this does not count for a move */
                crown(me);
                break;
        case 'd':
                pbundef();      /* force printing new stones */
                newchar();
                skipspaces();
                if(!ch){
                        disp = 1;
                        break;
                }
                if((i = readstone()) || (ch == '"')){
                        skipspaces();
                        check('"');
                        if(ch == ERRCHAR) goto ask;
                        sp = dsparr[i];
                        while(ch != '"'){
                                if(sp - dsparr[i] < 12) *sp++ = ch;
                                else {
                                        pmesg("string too long\n");
                                        goto fins;
                                }
                                if(!ch){
                                        pmesg("bad string\n");
                                        goto fins;
                                }
                                newchar();
                        }
                        newchar();
                        check(0);
                fins:
                        *sp++ = 0;
                        break;
                } else if(ch == 'e'){
                        newchar();
                        i = rnum();
                        for(ct=0; ct++!=i; ) if(!emparr[ct]){
                                pmesg("we dont have such empty fields\n");
                                goto ask;
                        }
                        sp = dsparr[0];
                        sp2 = emparr[i];
                        while(*sp++ = *sp2++);
                } else {
                        i = 4*rnum();
                        for(ct=0; ct++!=i; ) if(!fularr[ct]){
                                pmesg("this set of stones is not available\n");
                                goto ask;
                        }
                        for(ct=0; ct<4; ct++){
                                sp = dsparr[ct+1];
                                sp2 = fularr[i+ct];
                                while(*sp++ = *sp2++);
                        }
                }
                break;
        case 'u':
                if(newchar() == '\0') prsteval();
                else optu = rnum();
                break;
        case 'v':
                newchar();
                i = rnum();
                check(',');
                if(ch != ERRCHAR){
                        ct = rnum();
                        check(0);
                }
                if(ch == ERRCHAR) goto ask;
                setstrat(i,ct);
                break;
        case 'e':
                pmesg("I win\n");
                reset();
        case '=':
                remise();
                break;
        case 'i':
                if(disp) home();
                prinfo();
                prall++;
                break;
        case 'x':
                error("the game is over");
        case '?':
                switch(newchar()){
                case 'f':
                        if(newchar() != 'p') goto deflt;
                        i = fplevel;
                        break;
                case 'r':
                        i = rdif;
                        break;
                case 's':
                        i = prbwait;
                        break;
                deflt:
                default:
                        pmesg("unknown param\n");
                        goto ask;
                }
                pmesg("%c: %d\n",ch,i);
                break;
        default:
                if(ch)pmesg("unknown command\n");
                goto ask;
        }
        return;
ask:
        return;
}

int *
readmove(color) int color; {
        register int *mp1;      /* essential ! */
        register int *mp2,*ump,*ump0,ct,f1,f2;
        int umove[22];

listmoves:
        mpf = moves;
        move(me);
        if(possct == 0){
                pmesg("you lose\n");
                reset();
        } else if(possct == 1 && !optg){
                pmesg(" forced:\n");
                prmove(mpf);
                /*  return(mpf);  */
        }
ask:
        if(!optg) pmesg("?");
        rdlin();
        dirt++;
        newchar();
        skipspaces();
        /* read options */
        if(ch == '!'){
                rdcomd(ach);
                if(mp1 = youmv){
                        youmv = 0;
                        return(mp1);
                }
                if(!mpf) goto listmoves; /* the position has changed */
                goto ask;
        }
        /* if line does not start with a digit and (optg) the ignore */
        if(!digit()) if(optg) goto ask;
        /* read actual moves */
        ump0 = ump = &umove[0];
        while(ch){
                if(ump-ump0 >= 22) ump--;
                if((*ump++ = readnum()) == 0) {
                        if(optg) goto ask; /* ignore bad lines */
                        pmesg("bad field\n");
                        goto ask;
                }
                switch(ump - ump0){
                case 1:
                        if(ch == '.') {
                                do newchar();
                                while(ch == '.' || ch == ' ' || ch == '\t');
                                ump--;
                        }
                        else if(ch)
                                check(captmax ? 'x' : '-');
                        break;
                case 2:
                        if(!captmax) check(0);
                        else if(ch) check('(');
                        break;
                default:
                        if(ch == ',') {
                                newchar();
                                break;
                        }
                        check(')');
                        if(ch == ERRCHAR) goto ask;
                        check(0);
                }
                if(ch == ERRCHAR) goto ask;     /* some error occurred */
        }
        if(ump == ump0) {
                if(possct == 1) return(mpf);
                else goto ask;
        }
        captct = (ump - ump0) - 2;
        if(captct > 0){
                if(captct != captmax){
                        pmesg("incorrect number of captured stones\n");
                        goto ask;
                }
        } else if(captct == -1) {
                /* incomplete move, see whether it is to or from */
                if(bord[umove[0]] == EMPTY){    /* to */
                        umove[1] = umove[0];
                        umove[0] = -1;
                } else  umove[1] = -1;          /* from */
                captct = 0;
        }
        /* now determine which move he selected from the list
           created by 'move(c);'
         */
        ump = 0;
        for(mp1 = mpf; mp1 < mp; mp1 += ct){
                ump0 = &umove[0];
                mp2 = mp1;
                f1 = *mp1++;
                f2 = *mp1++;
                ct = *mp1++;
                if(((f1 & 0177) ^ *ump0++) > 0) continue;
                if(((f2 & 0177) ^ *ump0++) > 0) continue;
                if(captct) while(ct--)
                        if((*mp1++ & 0177) != *ump0++) continue;
                if(ump){
                        pmesg("ambiguous move\n");
                        goto ask;
                } else ump = mp2;
        }
        if(!ump) {
                pmesg("illegal move\n");
                goto ask;
        }
        if(!optg) prmove(ump);
        return(ump);
}

/* read w, z, W, Z, wd, bk and return index (0 by default) */
readstone(){
int c;
        switch(ch|040){
        case 'w':
                c = 1;
                break;
        case 'b':
        case 'z':
                c = 2;
                break;
        default:
                return(0);
        }
        if(ch & 040){
                newchar();
                if(ch == 'd' || ch == 'k') goto dam;
        } else {
        dam:
                newchar();
                c += 2;
        }
        return(c);
}

readcol(){
        switch(ch){
        case 'w':
                newchar();
                return(WHITE);
        case 'z':
        case 'b':
                newchar();
                return(BLACK);
        default:
                return(0);
        }
}

/*
 * Read position; each line looks like
 * w: 31-48,49,50
 * the description ends with a dot.
 */
int rdbord[66];

readpos(){
register int i,j;
int c;
        rdbord[0] = -1; /* remember that readpos was executed */
        for(i=1; i<66; i++) rdbord[i] = EMPTY;
        do {
                rdlin();
                if(newchar() == '.') break;
                if(!(i = readstone())){
                        pmesg("bad colour, ");
                        goto bad;
                }
                c = valstone[i];
                check(':');
                if(ch == ERRCHAR) goto bad;
        nxt:
                if(!(i = readnum())){
                        pmesg("bad field, ");
                        goto bad;
                }
                if(ch == '-'){
                        newchar();
                        if(!(j = readnum())){
                                pmesg("bad 2nd field, ");
                                goto bad;
                        }
                } else j = i;
                while(i <= j) rdbord[i++] = c;
                if(ch){
                        if(ch == '.') break;
                        check(',');
                        if(ch == ERRCHAR){
                                pmesg("bad separator, ");
                                goto bad;
                        }
                        goto nxt;
                }
        } while(1);
        for(i=6; i<60; i++)
                if(bord[i] != EDGE) bord[i] = rdbord[i];
        mpf = 0;
        prbord();
        return;
bad:
        pmesg("old board retained\n");
        return;
}

readfile(){
int f,f0;
        /* ach points to the f of 'rf' */
        newchar();
        skipspaces();
        ach--;
        f0 = dup(0);
        (void) close(0);
        f = open(ach,0);
        if(f < 0) pmesg("cannot open %s\n",ach);
        else if(f) error("f nonzero");
        else {
                readpos();
                (void) close(f);
        }
        if(f = dup(f0)) error("df nonzero");
        (void) close(f0);
}
