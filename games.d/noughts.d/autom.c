# include "def.h"

/* Scans the tabel who defines the finite automat */

autom(){
    register int state, i, way;
    short int who, j;
    unsigned char *atm;
    char (*p)[17], (*t)[17];
    unsigned char *newpts;
    char *oldpts;
    unsigned char *newtht;
    char *oldtht;
    char max;

    for(way = 0; way < 4; way++){
        state = 0;
        atm = automat[0];
        p = tmppts[way];
        t = tmptht[way];
        for(i = 0; i < 17; i++){
            p[0][i] = 0;
            p[1][i] = 0;
            t[0][i] = 0;
            t[1][i] = 0;
        }
        for(i = 0; i < 17; i++){
            switch(hvlr[way][i]){
                case ' ':
                    state = atm[0];
                    break;
                case 'o':
                    state = atm[1];
                    who = 0;
                    break;
                case 'x':
                    state = atm[2];
                    who = 1;
                    break;
                default:
                    state = 0;
            }
            atm = automat[state];
            if(atm[3]){
                max = 3 * atm[3] + 4;
                for(j = 4; j < max; j += 3){
                    oldpts = &p[who][i - atm[j]];
                    newpts = &atm[j + 2];
                    oldtht = &t[who][i - atm[j]];
                    newtht = &atm[j + 1];
                    if(*newtht > *oldtht)
                        *oldtht = *newtht;
                    if(*newpts > *oldpts)
                        *oldpts = *newpts;
                }
            }
        }
    }
}
