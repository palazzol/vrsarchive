/* Copyright (C) A.E. Brouwer, Amsterdam, 1977,1985 */
/* Interactive draugths playing program */
/* Version 1.0, AEB, Easter monday 1977 */
/* Version 2.0, AEB&CLP, 770514 */
/* Version 3.0, AEB&CLP, 771203 */
/* Linted and converted to BSD4.2, AEB, 850725 */

#include        <setjmp.h>
#include        <signal.h>
#include        "damdefs.h"
#include	<curses.h>

#define MSIZE   3000
#define GSIZE   1000

/*
   Numbering of the board:

          00  01  02  03  04  05
        05  06  07  08  09  10
          11  12  13  14  15  16
        16  17  18  19  20  21
          22  23  24  25  26  27
        27  28  29  30  31  32
          33  34  35  36  37  38
        38  39  40  41  42  43
          44  45  46  47  48  49
        49  50  51  52  53  54
          55  56  57  58  59  60
        60  61  62  63  64  65

   where the fields 0-5, 16, 27, 38, 49, 60-65
   constitute the edge of the board.
   Note: the fields with numbers divisible by 5
   form the main diagonal (see 'value(c)').
   Conversion tables:
 */

int conv[51] = { 0,
         6, 7, 8, 9,10,
        11,12,13,14,15,
        17,18,19,20,21,
        22,23,24,25,26,
        28,29,30,31,32,
        33,34,35,36,37,
        39,40,41,42,43,
        44,45,46,47,48,
        50,51,52,53,54,
        55,56,57,58,59
};
int reconv[66] = {
         0,  0,  0,  0,  0,  0,
           1,  2,  3,  4,  5,
         6,  7,  8,  9, 10,  0,
          11, 12, 13, 14, 15,
        16, 17, 18, 19, 20,  0,
          21, 22, 23, 24, 25,
        26, 27, 28, 29, 30,  0,
          31, 32, 33, 34, 35,
        36, 37, 38, 39, 40,  0,
          41, 42, 43, 44, 45,
        46, 47, 48, 49, 50,  0,
           0,  0,  0,  0,  0
};

int bord[66];
int game[GSIZE];
int *gp  = &game[0];
int moves[MSIZE];
int playb,playw;
int mvnr = 1;           /* odd means black's move */
int endgame = 0;        /* endgame strategy */
int fouragnst1 = 0;     /* four kings against one */
int lasthap = 1;        /* draw if nothing happens for too long */
int drawdel = DRAWWT;   /* 3 or 10 or DRAWWT */
int me   = WHITE;       /* player to move */
int optp = 1;           /* frequency of prbord() */
int optg = 0;           /* nonzero if game */
int optu = 0;   /* print evaluation */
                        /* bit0: static evaluation of position
                           bit1: static evaluation of each move
                           bit2: evaluation of position
                           bit3: evaluation of each move
                         */
int optf = -1;  /* file descriptor of ofile */
int userand = 1;
int rdif = 100;         /* cannot be much larger */
int rdifmin = 100;      /* min used during this game */
int rdmin = 1;          /* minimum recursion depth */
int rdmax = 10;         /* maximum recursion depth */
int fplevel = 1;        /* level of search for free paths */
int captct,captmax, *mpf,*mp0,*mp,possct;
extern int *findmove(), *readmove();
extern long lseek();
extern unsigned sleep();
int debug,crownmv,altermv;
int rescct;
int wpct,bpct,wkct,bkct;        /* global */
int timeb,timew;        /* time used by b/w (in seconds) */
int timemax  = 10;      /* nr of seconds allowed per move */

extern int value2(),value(),evalue();
int (*avalue)() = value;
int (*stratw)(),(*stratb)();
int (*(strategies[]))() = { value, value2, evalue };

extern char line[];
extern int rdbord[];

setstrat(w,b) int w,b; {
register int i;
        i = sizeof(strategies)/2;
        if(w >= i || b >= i){
                mvprintw(ROW0,0,"No such strategies\n");
                return;
        }
        stratw = strategies[w];
        stratb = strategies[b];
        mvprintw(ROW0,0,"Strategies %d(w),%d(b)\n",w,b);
}

intrup(){
        (void) signal(SIGINT,SIG_IGN);          /* no more interrupts */
        mvprintw(ROW0,0,"?");
        rdlin();
        rdcomd(line);
        (void) signal(SIGINT,intrup);
        if(!mpf){
                mvprintw(ROW0,0,"You cannot change the board during an interrupt\n");
                reset();
        }
}

int intflg = 0; /* interrupt while non-interruptable */

intrdp(){
        intflg++;
        (void) signal(SIGINT,intrdp);
}

jmp_buf env;

main(argc,argv)
int argc;
char **argv;
{
        int p;
        char *ac;

        initterm();
        printw("Dam %s Version %s\n",VERSION,DATE);

        optf = open(".outdam",1);       /* use it if it exists */
        if(optf >= 0) (void) lseek(optf, 0L, 2);        /* seek eof */

        setstrat(0,1);

        if(argc == 1){
                printw("Do you want info(y/n)? ");
                if(answer()) prinfo();
        }
        if(signal(SIGINT, SIG_IGN) != SIG_IGN)
                (void) signal(SIGINT, intrup);
        if(setjmp(env)) ; /* Program was reset */
        if(mvnr > 1){
                prbord();
                rdcomd("t");    /* report time used */
                if(optf > 0) outgame();
                printw("Another game? ");
                if(!answer()) {
			endwin();
			exit(0);
		}
        }
        sleep(2);
        init();
        /* 2nd time here argc will be one */
        while(--argc) {
                ac = *++argv;
                if(*ac == '-') ac++;
                switch(*ac){
                case 'p':
                        optp = atoi(++ac);
                        break;
                case 'g':
                        optg++;
                        break;
                case ':':
                        debug++;
                        break;
                case '\0':
                        /* do not use random generator */
                        userand = 0;
                        printw("No random generator used\n");
                        break;
                default:
                        rdcomd(ac);
                }
        }
        argc++;
        init2();
        while(1){
                mvnr++;
                if(optu&3) prsteval();
                timebeg();

                p = ((me == WHITE) ? playw : playb);
                if(p == USER) domove(readmove());
                else domove(findmove(me));

                if(me == WHITE) timew += timedif();
                else timeb += timedif();

                if(optp) if(mvnr%optp == 0) prbord();

                me = COL-me;
        }
}

reset(){
        longjmp(env,1);
}

init(){
register int i;
        /* check correctness of conv and reconv */
        for(i=1; i<=50; i++)
                if(reconv[conv[i]] != i)
                        error("conv error");
        /* fill edge of the board */
        for(i=0; i<66; i++)
                if(reconv[i] == 0) bord[i] = EDGE;
        /* initial board position */
        for(i=1; i<=20; i++)
                bord[conv[i]] = BLACK;
        for(i=21; i<=30; i++)
                bord[conv[i]] = EMPTY;
        for(i=31; i<=50; i++)
                bord[conv[i]] = WHITE;
        me = WHITE;     /* white to move */
        rdbord[0] = 0;  /* not from file */
        timew = timeb = 0;
        lasthap = mvnr =1;
        endgame = fouragnst1 = 0;
        crownmv = altermv = 0;
        mpf = &moves[0];
        gp = &game[0];
}

init2(){
        if(!playb && !playw){
                printw("Do you want me to play black? ");
                playb = answer()+1;
                printw("Do you want me to play white? ");
                playw = answer()+1;
        } else {
                if(!playb) playb = USER;
                if(!playw) playw = USER;
        }

        if(!mpf) mpf = moves; else
        if(optp)prbord();
}

/* fatal error routine */
error(s) char *s; {
        printw("%s\n",s);
	endwin();
        exit(0);
}

/* create list of all possible moves */

Move(c) int c; {
register int i;
int c0;

        possct = 0;
        captmax = captct = 0;
        mp = mp0 = mpf;
        for(i=6; i<60; i++) if(((c0 = bord[i]) & COL) == c) {
                bord[i] = EMPTY;
                mp += 3;
                *mp0 = i+c0;
                capt(c0,i);
                mp -= 3;
                bord[i] = c0;
        }
        if(!captmax) {
                if(c == BLACK) {
                for(i=6; i<60; i++) if(((c0 = bord[i]) & COL) == c) {
                        if(c0 == BLACK) {
                                hmove(c0,i,5);
                                hmove(c0,i,6);
                        } else {
                                dmove(c0,i,-6);
                                dmove(c0,i,-5);
                                dmove(c0,i,5);
                                dmove(c0,i,6);
                        }
                }} else
                for(i=59; i>5; i--) if(((c0 = bord[i]) & COL) == c) {
                        if(c0 == WHITE) {
                                hmove(c0,i,-5);
                                hmove(c0,i,-6);
                        } else {
                                dmove(c0,i,-6);
                                dmove(c0,i,-5);
                                dmove(c0,i,5);
                                dmove(c0,i,6);
                        }
                }
        }
        if(mp > &moves[MSIZE]) error("overflow moves array");
}

/* king move in specified direction */
dmove(c,pos,dir) int c,pos,dir; {
register int i;
        for(i = pos+dir; bord[i] == EMPTY; i += dir){
                *mp++ = pos+c;
                *mp++ = i+c;
                *mp++ = 0;
                possct++;
        }
}

/* piece move */
hmove(c,pos,dir) int c,pos,dir; {
register int i;
        if(bord[i = pos+dir] == EMPTY){
                *mp++ = pos + c;
                *mp++ = prom(c,i);
                *mp++ = 0;
                possct++;
        }
}

/* partial capture in specified direction */
hcapt(c,pos,dir) int c,pos,dir; {
register int i,j;
int c1;
        if(c&DAM){
                for(i=pos+dir; (c1=bord[i]) == EMPTY; i += dir);
                if((c1 & MASK) != DAM+COL-c) return;
                j = i;
                bord[i] |= CAPT;
                captct++;
                *mp++ = i+c1;
                while(bord[j += dir] == EMPTY)
                        capt(c,j);
                bord[i] = c1;
                captct--;
                mp--;
        } else {
                c1 = bord[i = pos+dir];
                if((c1 & MASK) != COL-c) return;
                if(bord[j=i+dir] != EMPTY) return;
                bord[i] |= CAPT;
                captct++;
                *mp++ = i+c1;
                capt(c,j);
                bord[i] = c1;
                captct--;
                mp--;
        }
}

capt(c,pos) int c,pos; {
register int *mp1;
        hcapt(c,pos,-6);
        hcapt(c,pos,-5);
        hcapt(c,pos,5);
        hcapt(c,pos,6);
        if(captct == captmax){
                if(!captct) return;
                possct++;
                *++mp0 = prom(c,pos);
                *++mp0 = captct;
                mp0 -= 2;
                mp1 = mp;
                do { *mp++ = *mp0++; } while (mp0 != mp1);
                return;
        } else if(captct > captmax){
                captmax = captct;
                possct = 1;
                mp1 = mpf;
                *mp1++ = *mp0;
                *mp1++ = prom(c,pos);
                *mp1++ = captct;
                mp0 += 3;
                do { *mp1++ = *mp0++; } while(mp0 != mp);
                mp0 = mpf;
                mp = mp1;
                do { *mp1++ = *mp0++; } while(mp0 != mp);
                mp = mp1;
                return;
        }
}

prom(c,pos) int c,pos; {
        if(c&WHITE) {
                if(pos < 11) c |= DAM;
        } else  if(pos > 54) c |= DAM;
        return(pos+c);
}

/* execute actual move */
domove(mvpt) int *mvpt; {
register int i;
register int *mp1;
int ct,ctd,cte;
int difflist[66];
        /* store move in array game */
        mp1 = mvpt;
        *gp++ = *mp1++;
        *gp++ = *mp1++;
        *gp++ = ct = *mp1++;
        while(ct--) *gp++ = *mp1++;
        if(gp+4 > &game[GSIZE]) error("overflow game array");

        /* change board position */
        mp1 = mvpt;
        bord[((*mp1++) & 0377)] = EMPTY;
        i = *mp1 & VAL;
        bord[((*mp1++) & 0377)] = i;
        ct = *mp1++;
        if(ct || !(*mvpt & DAM)) lasthap = mvnr;
        while(ct--) bord[((*mp1++) & 0377)] = EMPTY;

        /* check for repetition of moves */
        ct = cte = 0;
        ctd = mvnr-lasthap;
        if(ctd > 3){
                for(i=0; i<66; i++) difflist[i]=0;
                mp1 = gp;
                while(ctd--){   /* go back one move */
                        if(*--mp1) error("capture after lasthap");
                        i = *--mp1;
                        if(i & (difflist[i & 0377] ^= (i&COL)))
                                ct++;
                        else    ct--;
                        i = *--mp1;
                        if(i & (difflist[i & 0377] ^= (i&COL)))
                                ct++;
                        else    ct--;
                        if(difflist[0] ^= 1) ct++;
                        else ct--;
                        if(!ct) cte++;
                }
                if(cte == 2){
                        mvprintw(ROW0,0,"\nDraw by repetition of moves\n");
                        reset();
                } else if(cte == 1) {
                        mvprintw(ROW0,0,"\nThis is the 2nd occurrence\
 of this position\n");
                }
        }
        /* check for three kings against one */
        pdistr();
        if((!wpct) && (!bpct) && (wkct+bkct == 4) && ((wkct&bkct) == 1)){
                drawdel = 10;
                ct = (wkct==1 ? WHITE : BLACK);
                /* single king on main diagonal? */
                for(i=10; i<60; i += 5)
                        if(bord[i] & ct){
                                drawdel = 3;
                                break;
                        }
        }
        if(mvnr-lasthap > drawdel*2){
                mvprintw(ROW0,0,"\nDraw since nothing happened for %d moves\n",
                        drawdel);
                reset();
        }
        if(wkct+bkct && wpct+bpct<12) {
                if(!endgame) setstrat(2,2);             /* %% */
                endgame++;
        }
        if(endgame && ((wkct > 3) && (bkct < 2) ||
                        (bkct > 3) && (wkct < 2)))
                fouragnst1++;
}

/* he asks for remise, shall we accept it? */
/* very stupid algorithm %% */
remise(){
        if(mvnr-lasthap > drawdel*2){
                mvprintw(ROW0,0,"I accept\n");
                reset();
        } else {
                mvprintw(ROW0,0,"I reject\n");
        }
}

pdistr(){       /* find distribution of white and black pieces */
register int i;
        wpct = wkct = bpct = bkct = 0;
        for(i=6; i<60; i++) switch(bord[i] & VAL){
        case WHITE:
                wpct++;
                break;
        case BLACK:
                bpct++;
                break;
        case WHITE|DAM:
                wkct++;
                break;
        case BLACK|DAM:
                bkct++;
                break;
        default:
                break;
        }
}

crown(c) int c; {
register int i,v;
int pct,kct,kseen,save_i[2],sk;
        pct = kct = kseen = 0;
        for(i=1; i<=50; i++){
                v = bord[conv[i]] & VAL;
                if(v == EMPTY) continue;
                if(v & c){
                        if(kseen++ || (!(v & DAM))) goto err;
                        else sk = i;
                } else {
                        if(v & DAM) kct++;
                        else if(pct == 2) goto err;
                        else save_i[pct++] = i;
                }
        }
        if((kct + pct == 3) && pct){
                drawdel = ((sk/5)*5 == sk ? 3 : 10);
                lasthap = crownmv = mvnr;
                mvprintw(ROW0,0,"%d pieces crowned to king\n",pct);
                mvprintw(ROW0,0,"It will be a draw in %d moves\n",drawdel);
                while(pct--){
                        bord[conv[save_i[pct]]] |= DAM;
                }
        } else {
        err:
                mvprintw(ROW0,0,"Crowning my pieces to a king is allowed only\n");
                mvprintw(ROW0,0,"if I have 3 pieces among which 1 or 2 kings\n");
        }
}

int rdepth;
int res0,res1,res2;

/* allowed difference with optimal move */
#define Q       30

int*
findmove(c) int c; {
int     r,res;
struct mvv {
        int *mvp,val0,val1;
} mvvals[MVMAX], *mvg;
register struct mvv *mvvp;
register int *mp1;

        rescct = 0;
        Move(c);
        if(!possct){
                mvprintw(ROW0,0,"I lost\n");
                reset();
        }
        if(possct == 1){
                prmove(mpf);
                return(mpf);
        }
        avalue = (me == WHITE ? stratw : stratb);
        rdepth = 0;
        res2 = -1;
        /* first look to a short depth */
        rdif -= 40;
        rdmin--;
        res = -GIANT;
        mvvp = mvvals;
        for(mp1 = mpf; mp1<mp; ){
                r = result(mp1,res-Q,GIANT);
                if(r < -GIANT) r = -GIANT;
                if(r > res) res = r;
                mvvp->mvp = mp1;
                mvvp++->val0 = r;
                if(mvvp >= mvvals+MVMAX-1){
                        mvprintw(ROW0,0,"Error in findmove: MVMAX too small\n");
                        break;
                }
                mp1 += 2;
                mp1 += (*mp1++);
        }
        /* end of list */
        mvvp->mvp = 0;

        /* sort mvvals */
        for(mvvp=mvvals; mp1=(mvvp+1)->mvp; mvvp++){
                res1 = (mvvp+1)->val0;
        test:
                if(res1 > (res0 = mvvp->val0)){
                        /* switch */
                        (mvvp+1)->mvp = mvvp->mvp;
                        (mvvp+1)->val0 = res0;
                        mvvp -> mvp = mp1;
                        mvvp -> val0 = res1;
                        if(mvvp > mvvals){
                                mvvp--;
                                goto test;
                        }
                }
        }
        /* short term result */
        res0 = res;
        /* now look to full depth */
        mvvp = mvvals;
        rdif += 40;
        rdmin++;
        res = -GIANT-1;         /* force assignment to mvg */
        while(mp1 = mvvp->mvp){
                r = result(mp1,res-Q,GIANT);
                if(r < -GIANT) r = -GIANT;
                if(r > res){
                        res = r;
                        mvg = mvvp;
                /*      gdct = 1;       */
                }
        /*      else if(r == res)if(random(++gdct)==0) mvg=mvvp; %% */
                mvvp++ -> val1 = r;
        }
        /* long term result */
        res1 = res;

        /* final result good? */
        if(res1 >= res0-Q) goto ret;

        /* or short term result not worse than for the others? */
        if(mvg->val0 >= res0-30) goto ret;

        /* this seems a bad move at first sight;
           see whether it is really necessary */
        rdif += (mp-mpf);
        res2 = result(mvg->mvp, res1-PVAL, res1+PVAL);
        rdif -= (mp-mpf);
        if(res2 >= res1-Q) goto ret;

        /* a bad move after all, take the one that seems best */
        r = -GIANT-1;
        mvvp = mvvals;
        while(mvvp->val0 >= res0-Q){
                if(mvvp->val1 > r){
                        r = mvvp->val1;
                        mvg = mvvp;
                }
                mvvp++;
        }
ret:
        if(optu&8){
                /* print value of each move */
                mvvp = mvvals;
                while(mp1 = mvvp->mvp){
                        prmove(mp1);
                        printw(mvvp == mvg ? "####" : "****");
                        printw(" %d %d\n",mvvp->val0,mvvp->val1);
                        mvvp++;
                }
                addch('\n');
        }
        prmove(mvg->mvp);
        if(optu&4) preval(mvg->val1);
        return(mvg->mvp);
}

preval(r) int r; {
        mvprintw(ROW0,0,"value: %d\n",r);
        mvprintw(ROW0,0,"res[i]: %d %d %d\n",res0,res1,res2);
        mvprintw(ROW0,0,"#calls: %d\n",rescct);
}

/* deliver either evaluation or answer '<a' or '>b' */
result(mvpt,a,b) int *mvpt,a,b; {
register int i;
register int *mp1;
int c,ct0,ct,r,res,*mpf0,fp;
        rescct++;
        mp1 = mvpt;
        c = *mp1 & COL;
        bord[((*mp1++) & 0377)] = EMPTY;
        i = *mp1 & VAL;
        bord[((*mp1++) & 0377)] = i;
        ct0 = ct = *mp1++;
        while(ct--) bord[((*mp1++) & 0377)] = EMPTY;

        if(!ct0) rdepth++;
        fp = (rdepth/2 == fplevel ? freepath(c) : 0);
        a += fp;
        b += fp;

        if(ct0 || (rdepth<rdmin)) goto go_on;
        if((mp-moves > rdif) || (rdepth >= rdmax)) goto eval;
        goto go_on;

eval:
        res = (*avalue)(c);
        goto unmove;

go_on:
        mpf0 = mpf;
        mpf = mp;
        Move(COL-c);
        res = b+1;
        for(mp1=mpf; mp1<mp; ){
                r = -result(mp1,-res,-a);
                if(r < res){
                        res = r;
                        if(res < a) break;
                }
                mp1 += 2;
                mp1 += (*mp1++);
        }
        mp = mpf;
        mpf = mpf0;

unmove:
        mp1 = mvpt;
        i = *mp1++;             /* be careful; perhaps start and
                                   final position are the same! */
        bord[((*mp1++) & 0377)] = EMPTY;
        bord[i & 0377] = i & VAL;
        ct = *mp1++;
        while(ct--){
                i = *mp1 & VAL;
                bord[((*mp1++) & 0377)] = i;
        }
        
        if(!ct0) rdepth--;
        return(res-fp);
}

/* evaluation of position after recursion */
int info[66] = {
         0,  0,  0,  0,  0,  0,
           1,  1,  1,  1,  1,
         2,  2,  2,  2,  2,  0,
           3,  3,  3,  3,  3,
         4,  4,  4,  4,  4,  0,
           5,  5,  5,  5,  5,
         6,  6,  6,  6,  6,  0,
           7,  7,  7,  7,  7,
         8,  8,  8,  8,  8,  0,
           9,  9,  9,  9,  9,
        10, 10, 10, 10, 10,  0,
           0,  0,  0,  0,  0
};

/* valuations for the columns: index is fldnr%11 */
/* weight center */
int valar1[11] = {
        0, 2, 4, 3, 1, UNDEF, 1, 3, 4, 2, 0
};

/* weight distribution */
int valar2[11] = {
        -5, -3, -1, 2, 4, UNDEF, -4, -2, 1, 3, 5
};

value(c){
int sum,v,oc;
register int d,*bp,*ip;
        oc = COL - c;
        sum = 0;
        ip = &info[6];
        for(bp = &bord[6]; bp < &bord[60]; ){
                d = *bp++ & VAL;
                v = *ip++;
                if(d == EMPTY) continue;
                if(d == EDGE) continue;
                if(d&DAM){
                        v = DVAL;
                        /* increase value if on main diagonal */
                        if((bp-bord)%5 == 1) v += DMVAL;
                } else {
                        if(d == WHITE) v = 11-v;
                        v += PVAL;
                }
                if(d & oc) v = -v;
                sum += v;
        }
        return(sum);
}

/* evaluation of position after recursion */
/* value of tripods : the value of the field they point to */
int info2[66] = {
         0,  0,  0,  0,  0,  0,
           1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  0,
           5,  7,  8,  6,  2,
         2,  7,  9,  8,  6,  0,
           7,  9, 10,  8,  2,
         2,  9, 11, 10,  8,  0,
           2,  2,  2,  2,  2,
         2,  2,  2,  2,  2,  0,
           2,  2,  2,  2,  2,
         2,  2,  2,  2,  2,  0,
           0,  0,  0,  0,  0
};

int vcentrw,vcentrb,vequidw,vequidb,vtempw,vtempb;

value2(c){
int sum,v,oc;
register int d,*bp,i;
        oc = COL - c;
        sum = 0;
        vcentrw = vcentrb = vequidw = vequidb = vtempw = vtempb = 0;
        for(bp = bord; bp < &bord[6]; bp++){
                sum += trip(5,BLACK,bp);
                sum += trip(6,BLACK,bp);
        }
        if(c == WHITE) sum = -sum;
        for(i=6; i<60; (i++,bp++)){
                d = *bp & VAL;
                if(d == EMPTY) continue;
                if(d == EDGE) {
                        v = trip(-5,WHITE,bp);
                        v += trip(-6,WHITE,bp);
                        v -= trip(5,BLACK,bp);
                        v -= trip(6,BLACK,bp);
                        sum += (c == WHITE ? v : -v);
                        continue;
                }
                if(d&DAM){
                        v = DVAL;
                        /* increase value if on main diagonal */
                        if((i/5)*5 == i) v += DMVAL;
                } else {
                        v = PVAL;
                        if(d == WHITE){
                                vtempw += info[65-i];
                                v += trip(-5,WHITE,bp);
                                v += trip(-6,WHITE,bp);
                                vequidw += valar2[i%11];
                                vcentrw += valar1[i%11];
                        } else {
                                vtempb += info[i];
                                v += trip(5,BLACK,bp);
                                v += trip(6,BLACK,bp);
                                vequidb += valar2[i%11];
                                vcentrb += valar1[i%11];
                        }
                }
                if(d & oc) v = -v;
                sum += v;
        }
        for( ;bp < &bord[66]; bp++){
                v = trip(-5,WHITE,bp);
                v += trip(-6,WHITE,bp);
                sum += (c == WHITE ? v : -v);
        }
        if(vequidw < 0) vequidw = -vequidw;
        if(vequidb < 0) vequidb = -vequidb;

        v = vcentrw-vcentrb-vequidw+vequidb+vtempw-vtempb;
        sum += (c == WHITE ? v : -v);
        return(sum);
}

evalue(c){
int sum,v,oc,i;
register int d,*bp,*ip;
        oc = COL-c;
        sum = 0;
        ip = &info[6];
        for(bp = &bord[6]; bp < &bord[60]; ){
                d = *bp++ & VAL;
                v = *ip++;
                if(d == EMPTY) continue;
                if(d == EDGE) continue;
                /* if opponent has a king then traps are valuable */
                if((d==WHITE && bkct) || (d==BLACK && wkct))
                        v += trap(-5,d&COL,bp)
                           + trap(-6,d&COL,bp)
                           + trap( 5,d&COL,bp)
                           + trap( 6,d&COL,bp);
                if(d&DAM) {
                        v = DVAL;
                        if((bp-bord)%5 == 1) v += DMVAL;
                        if(fouragnst1){
                                i = (bp-bord)%6;
                                if(i <= 1) v += DMVAL;
                                switch(bp-bord){
                                case 6+1:
                                case 11+1:
                                case 54+1:
                                case 59+1:
                                        v += DMVAL/2;
                                }
                        }
                } else {
                        if(d == WHITE) v = 11-v;
                        v += PVAL;
                }
                if(d & oc) v = -v;
                sum += v;
        }
        return(sum);
}

prsteval(){
int rdmax0,rdmin0;
register int *mp1;
        if(optu&1) printw("Static eval: %d\n",value2(me));
        if(optu&2){
                rdmax0 = rdmax;
                rdmin0 = rdmin;
                rdmax = rdmin = 1;
                rdepth = 0;
                avalue = value2;
                Move(me);
                for(mp1=mpf; mp1<mp; ){
                        printw("%4d  ", result(mp1,-GIANT,GIANT));
                        printw("(%2d %2d %2d / %2d %2d %2d)     ",
                                vtempw-vtempb,vcentrw,vequidw,
                                vtempb-vtempw,vcentrb,vequidb   );
                        prmove(mp1);
                        if((mvnr&1)==0) addch('\n');  /* %% */
                        mp1 += 2;
                        mp1 += (*mp1++);
                }
                printw("\n");
                rdmax = rdmax0;
                rdmin = rdmin0;
        }
}

/* test for X XY where Y=X or Y=Edge */
trap(dir,col,nbp) int dir,col,*nbp; {
register int *bp,w,v;
        bp = nbp+dir;
        if(*bp != EMPTY) return(0);
        bp += dir;
        if(((w = *bp)&COL) != col) return(0);
        bp += dir;
        if(!(*bp & col)) return(0);
        /* value is number of empty fields covered by trap */
        v = 0;
        bp = nbp-dir;
        while(*bp == EMPTY){
                v += TPVAL;
                bp -= dir;
        }
        /* extra bonus if base of trap is a king */
        if(w & DAM) v += (v/2);
        return(v);
}

trip(dir,col,nbp) int dir,col,*nbp; {
register int *bp;
int tv1,tv2;
        tv2 = 0;
        bp = nbp+dir;
        if(*bp != col) return(0);
        bp += dir;
        if(*bp != col) return(0);
        tv1 = (col == WHITE ? info2[65-(bp+dir-bord)] : info2[bp+dir-bord]);
        if(bp[dir] == col) tv1 >>= 1;
        if(dir < 0) dir = -dir;
        dir = 11-dir;           /* interchange 5 and 6 */
        if(bp[dir] == col && ((bp[dir+dir] & col) == 0)) tv2++; else
        if(bp[-dir]== col && ((bp[-dir-dir]& col) == 0)) tv2++;
        return(tv2 ? 2 : tv1);
}

/* check for free path (for oc) to a king */
int freect,frlngt,flth; /* 'real' free path */
int freect2,frlngt2,fbcct;      /* free paths discarding backward captures */
int fpval[11] = {
        0,              /* 0 cannot occur */
        PVAL,           /* 1 is already seen as a king in recursion %% */
        PVAL,           /* 2 */
        PVAL-10,        /* 3 */
        PVAL-20,        /* 4 */
        PVAL/2,         /* 5 */
        30,30,          /* 6,7 */
        15,15,          /* 8,9 */
        0               /* 10, i.e. no free paths */
};

int fpval2[11] = {
        0, 0,           /* 0,1 cannot occur */
        PVAL/2,         /* 2 */
        PVAL/3,         /* 3 */
        10,10,          /* 4,5 */
        5,5,            /* 6,7 */
        2,2,            /* 8,9 */
        0               /* 10 */
};

freepath(c) int c; {
register int *bp,*nbp,i;
int nbord[66],oc;

        oc = COL-c;

        /* copy */
        bp = bord;
        nbp = nbord;
        while(bp < &bord[66]) *nbp++ = *bp++;

        /* swell */
        for(nbp = &nbord[6]; nbp < &nbord[60]; nbp++){
                if((*nbp & COL) == c){
                        if(*nbp & DAM) {
                                dmark(nbp, -6);
                                dmark(nbp, -5);
                                dmark(nbp, 5);
                                dmark(nbp, 6);
                        } else {
                                if(c == WHITE){
                                        nbp[-6] |= FLAGS;
                                        nbp[-5] |= FLAGS;
                                        nbp[5] |= FLAG;
                                        nbp[6] |= FLAG;
                                } else {
                                        nbp[-6] |= FLAG;
                                        nbp[-5] |= FLAG;
                                        nbp[5] |= FLAGS;
                                        nbp[6] |= FLAGS;
                                }
                        }
                        *nbp = EDGE;
                }
        }

        /* but note: points on the border are always safe */
        nbp = nbord;
        for(i = 6; i<10; i++) nbp[i] &= VAL;
        for( ; i<65; i += 10){
                nbp[i++] &= VAL;
                nbp[i] &= VAL;
        }
        for(i = 56; i<60; i++) nbp[i] &= VAL;

        /* determine strong component of last line */
        freect = flth = freect2 = fbcct = 0;
        frlngt = frlngt2 = 10;  /* shortest freepath: +inf */
        if(c == WHITE){
                for(nbp = &nbord[55]; nbp < &nbord[60]; nbp++)
                        reach(oc,nbp,-6);
        } else  for(nbp = &nbord[6]; nbp < &nbord[11]; nbp++)
                        reach(oc,nbp,5);

        if(debug){              /* %% */
                mvprintw(ROW0,0,"fct: %d, fct2: %d, fl: %d, fl2: %d,\
flth: %d=0, fbcct: %d=0\n",
                        freect,freect2,frlngt,frlngt2,flth,fbcct);
        }
        if(freect) return(fpval[frlngt]);
        return(fpval2[frlngt2]);
}

dmark(nbp,d) int *nbp,d; {
register int *bp;
        bp = nbp + d;
        while((*bp & COL) == EMPTY){
                *bp = FLAGS;
                bp += d;
        }
}

reach(oc,nbp,d) int oc,*nbp,d; {
register int *bp,v;
        bp = nbp;
        v = *bp & VAL;
        if(v == EDGE) return;
        if(v == oc) {
                if(fbcct == 0){
                        freect++;
                        if(frlngt > flth) frlngt=flth;
                } else {
                        freect2++;
                        if(frlngt2 > flth) frlngt2=flth;
                }
        }
        if(v != EMPTY) return;  /* %% */
        if((*bp & FLAG) == 0){
                flth++;
                reach(oc,bp+d,d);
                reach(oc,bp+d+1,d);
                flth--;
                if(fbcct == 0) *bp = EDGE;
                else *bp |= FLAG2;      /* dont look for semifree paths */
        } else if((*bp & FLAG2) == 0){
                fbcct++;
                flth++;
                reach(oc,bp+d,d);
                reach(oc,bp+d+1,d);
                flth--;
                fbcct--;
                *bp = EDGE;     /* dont return to this field */
        } else  *bp = EDGE;
}

/* construct position after move k */
backup(k) int k; {
register int i,*gp0;
int mvno,c,ct;
        /* the validity of k was checked by the caller */
        if(rdbord[0])
                for(i=6; i<60; i++){
                        if(bord[i] != EDGE) bord[i] = rdbord[i];
                }
        else {
                for(i=1; i<=20; i++) bord[conv[i]] = BLACK;
                for(i=21;i<=30; i++) bord[conv[i]] = EMPTY;
                for(i=31;i<=50; i++) bord[conv[i]] = WHITE;
        }
        gp0 = game;
        mvno = mvnr;
        mvnr = 1;
        while(k--){
                mvnr++;
                c = *gp0 & COL;
                if(mvnr == crownmv) crown(c); /* yields output %% */
                bord[(*gp0++) & 0377] = EMPTY;
                i = *gp0 & VAL;
                bord[(*gp0++) & 0377] = i;
                ct = *gp0++;
                while(ct--) bord[(*gp0++) & 0377] = EMPTY;
        }
        mvnr++;
        if((mvno+mvnr) & 1) me = COL-me;
        mpf = 0;        /* the position has changed */
        gp = gp0;
        if(lasthap > mvnr) lasthap = mvnr;
        if(crownmv > mvnr) crownmv = 0;
}

/*
 * Implemented rules:
 *
 * a) If the same position occurs thrice (with the same
 *    player to move) it is a draw.
 * b) If a player has 3 pieces among which at least one
 *    king and his opponent has one king only, his opponent
 *    may crown all these pieces (which action does not
 *    count for a move). Immediately rule c) becomes applicable.
 * c) If the position is 3 kings against 1 king then after
 *      (single king on main diagonal ? 3 : 10)
 *    moves the game ends in a draw.
 *
 * In addition to these we use for PDP against PDP play the rule:
 *
 * z) If during DRAWWT moves no piece has been captured and only
 *    kings have moved it is a draw.
 *
 */
