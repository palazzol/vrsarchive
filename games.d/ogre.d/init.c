#include "ext.h"

void qsort();

static char a, b;
static int cp_set;
static int infantry_points, armor_points;

init_units(mark)
{
    int unitcmp();

	init_screen();

    a = 10;
    b = 10;

    switch(mark) {

        case 3:
            armor_points = 10;
            infantry_points = 18;
            break;

        case 5: 
            armor_points = 18;
            infantry_points = 27;
            break;
    }

    n_units = 0;
    cp_set = FALSE;

    while(armor_points > 0 || infantry_points > 0 || !cp_set) {
        movecur(16, 0);
        eeol();
        printf("left to place: %d armor, %d infantry%s",
            armor_points, infantry_points,
            (cp_set) ? "." : ", CP");
        getunit();
    }

    /* sort the units so lower the i, the more valuable the unit. */
    qsort( (char *) unit, n_units, sizeof(UNIT), unitcmp);


}

getunit()
{
    char    no_new, bad_char;
    char    olda, oldb;
    char    dir;

    no_new = TRUE;
    bad_char = FALSE;

    movecur_hex(a, b);

    while(no_new) {

        olda = a;
        oldb = b;

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
                break;

            case CP:
                if(cp_set) {
                    bad_char = TRUE;
                }
                else {
                    add_unit(a, b, dir);
                    no_new = FALSE;
                    cp_set = TRUE;
                }
                break;

            case HVYTANK:
            case MSLTANK:
            case GEV:
                if(occupied(a, b) || blocked(a, b) || armor_points == 0) {
                    bad_char = TRUE;
                    break;
                }
                add_unit(a, b, dir);
                no_new = FALSE;
                armor_points--;
                break;
    
    	    case '1':
                if(occupied(a, b) || blocked(a, b) || infantry_points < 1) {
                    bad_char = TRUE;
                    break;
                }
                add_unit(a, b, dir);
                no_new = FALSE;
                infantry_points -= 1;
                break;

	    case '2':
                if(occupied(a, b) || blocked(a, b) || infantry_points < 2) {
                    bad_char = TRUE;
                    break;
                }
                add_unit(a, b, dir);
                no_new = FALSE;
                infantry_points -= 2;
                break;

            case INFANTRY:
	    case '3':
                if(occupied(a, b) || blocked(a, b) || infantry_points < 3) {
                    bad_char = TRUE;
                    break;
                }
                add_unit(a, b, dir);
                no_new = FALSE;
                infantry_points -= 3;
                break;
    
            case HOWITZER:
                if(occupied(a, b) || blocked(a, b) || armor_points <= 1) {
                    bad_char = TRUE;
                    break;
                }
                add_unit(a, b, dir);
                no_new = FALSE;
                armor_points -= 2;
                break;
    
            default:
                bad_char = TRUE;
                break;
    
        }
    
        if(off_obstructed(a, b)  || bad_char)
        {

            a = olda;
            b = oldb;
            bad_char = FALSE;
    
        }

        else {

            movecur_hex(a, b);

        }

    }

}

add_unit(aa, bb, c)
char aa, bb, c;
{
    int i;

    i = n_units;
    n_units++;

    switch(c) {

        case CP:
            unit[i].type = CP;
            unit[i].attack = 0;
            unit[i].range = 0;
            unit[i].defend = 0;
            unit[i].movement = 0;
            break; 

        case HVYTANK:
            unit[i].type = HVYTANK;
            unit[i].attack = 4;
            unit[i].range = 2;
            unit[i].defend = 3;
            unit[i].movement = 3;
            break; 

        case MSLTANK:
            unit[i].type = MSLTANK;
            unit[i].attack = 3;
            unit[i].range = 4;
            unit[i].defend = 2;
            unit[i].movement = 2;
            break; 

        case GEV:
            unit[i].type = GEV;
            unit[i].attack = 2;
            unit[i].range = 2;
            unit[i].defend = 2;
            unit[i].movement = 4;
            break; 

        case HOWITZER:
            unit[i].type = HOWITZER;
            unit[i].attack = 6;
            unit[i].range = 8;
            unit[i].defend = 1;
            unit[i].movement = 0;
            break; 

        case INFANTRY:
	case '3':
            unit[i].type = INFANTRY;
            unit[i].attack = 3;
            unit[i].range = 1;
            unit[i].defend = 1;
            unit[i].movement = 2;
            break; 

	case '1':
            unit[i].type = INFANTRY;
            unit[i].attack = 1;
            unit[i].range = 1;
            unit[i].defend = 1;
            unit[i].movement = 2;
            break; 

	case '2':
            unit[i].type = INFANTRY;
            unit[i].attack = 2;
            unit[i].range = 1;
            unit[i].defend = 1;
            unit[i].movement = 2;
            break; 

    }

    unit[i].range_to_ogre = 0;
    unit[i].fired = 0;
    unit[i].status = OK;
    unit[i].moves_left = 0;
    unit[i].l_hex = aa;
    unit[i].r_hex = bb;

    disp_unit(i);

}

occupied(aa, bb)
char aa, bb;
{
    int i;

    for(i = 0; i < n_units; i++)
        if(unit[i].status != DESTROYED &&
           unit[i].l_hex == aa &&
           unit[i].r_hex == bb) return(TRUE);

    return(FALSE);

}

init_ogre(mark)
{

    ogre.l_hex = rand() % 7 + 22; /* 22 - 28 */
    ogre.r_hex = 50 - ogre.l_hex;

    switch(mark) {

        case 3:
            ogre.treads = 45;
            ogre.init_treads = 45;
            ogre.movement = 3;
            ogre.missiles = 2;
            ogre.main_bats = 1;
            ogre.sec_bats  = 4;
            ogre.ap = 8;
            break;

        case 5:
            ogre.treads = 60;
            ogre.init_treads = 60;
            ogre.movement = 3;
            ogre.missiles = 5;
            ogre.main_bats = 2;
            ogre.sec_bats  = 6;
            ogre.ap = 10;
            break;

    }


    disp_ogre();

}

unitcmp(u1, u2)
UNIT *u1, *u2;
{
    int cmp;

    switch(u1 -> type) {

        case CP:

            switch(u2 -> type) {

                case CP: 
                    cmp = 0;
                    break;

                default:
                    cmp = -1;
                    break;

            }

            break;

        case HOWITZER:
            switch(u2 -> type) {

                case CP: 
                    cmp = 1;
                    break;

                case HOWITZER:
                    cmp = 0;
                    break;

                default:
                    cmp = -1;
                    break;

            }

            break;

        case HVYTANK:
            switch(u2 -> type) {

                case CP:
                case HOWITZER:
                    cmp = 1;
                    break;

                case HVYTANK:
                    cmp = 0;
                    break;

                default:
                    cmp = -1;
                    break;

            }

            break;

        case MSLTANK:
            switch(u2 -> type) {

                case CP:
                case HOWITZER:
                case HVYTANK:
                    cmp = 1;
                    break;

                case MSLTANK:
                    cmp = 0;
                    break;

                default:
                    cmp = -1;
                    break;

            }

            break;

        case GEV:
            switch(u2 -> type) {

                case INFANTRY:
                    cmp = -1;
                    break;

                case GEV:
                    cmp = 0;
                    break;

                default:
                    cmp = 1;
                    break;

            }

            break;

        case INFANTRY:
            switch(u2 -> type) {

                case INFANTRY: 
                    cmp = 0;
                    break;

                default:
                    cmp = 1;
                    break;

            }

            break;

        }

    return(cmp);

}
