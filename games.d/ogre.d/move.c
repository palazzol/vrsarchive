/*
    Move the defender's units.

    Michael Caplinger, Rice University, March 1982.
*/

#include "ext.h"

move_def()
{
    int i;

    for(i = 0; i < n_units; i++)
        if(unit[i].status == OK) {
            if(unit[i].moves_left > 0) describe_action("Move", i);
            while(unit[i].moves_left > 0 && unit[i].status == OK) 
                getmove(i);
        }

}

getmove(i)
int i;
{

    char    nomove, bad_char;
    char    a, b, dir;
    char    olda, oldb;

    nomove = TRUE;

    while(nomove) {
    
        a = unit[i].l_hex;
        b = unit[i].r_hex;

        movecur_hex(a, b);

        bad_char = FALSE;
    
        dir = getchar();
    
        switch(dir) {
    
            case RIGHT:
                a--;
                b--;
                break;
    
            case UPRIGHT:
                a--;
                break;
    
            case DOWNRIGHT:
                b--;
                break;
    
            case LEFT:
                a++;
                b++;
                break;
    
            case UPLEFT:
                b++;
                break;
    
            case DOWNLEFT:
                a++;
                break;
    
            case SIT:
            case ' ':
                unit[i].moves_left = 0;
                return;
    
            case REDRAW:
                redraw_screen();
                bad_char = TRUE;
                break;

            default:
                bad_char = TRUE;
                break;
    
        }

        /* Rule 5.02 */
        /*
            Note that the no-stacking rule can be violated by SITting on an
            occupied hex, and that the enforcement of no-stacking below is
            TOO stringent.
        */

        if(off_map(a, b) || 
            (occupied(a, b) && unit[i].moves_left == 1) ||
            blocked(a, b) ||
            bad_char)
        {

            bad_char = FALSE;
    
        }
    
        else {
            /* display move */

            olda = unit[i].l_hex;
            oldb = unit[i].r_hex;
            unit[i].l_hex = a;
            unit[i].r_hex = b;

            update_hex(olda, oldb);

            nomove = FALSE;
            unit[i].moves_left -= 1;

            def_ram(i);

            update_hex(unit[i].l_hex, unit[i].r_hex);

        }

    }

}
