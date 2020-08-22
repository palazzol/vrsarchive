/*
    This file contains routines to collect attack orders from the player,
    and display his odds of success for each target.  It calls the
    routines in "resolve.c" to determine the outcomes of the attacks.

    Michael Caplinger, Rice University, March 1982.
*/

#include "ext.h"

static OGRE allocated;

#define PASS        'p'
#define NOPASS      '\0'
#define RESOLVE     'r'
#define MISSILE     'm'
#define MAIN        'b'
#define SECONDARY   's'
#define AP          'a'
#define TREAD       't'

attack_def()
{
    char moreunits;
    int  i;

    moreunits = TRUE;
    zero(&allocated, sizeof(allocated));
    init_def_attack();

    /*
        The "fired" element of each unit description is here used as a
        Boolean to keep track of who has fired.
    */

    while(moreunits) {

        moreunits = FALSE;

        for(i = 0; i < n_units; i++) {

            if(unit[i].status == OK &&
                !unit[i].fired &&
                unit[i].attack > 0  &&
                unit[i].range_to_ogre <= unit[i].range) {

                    describe_action("Fire", i);

                    if(get_target(i) == PASS) moreunits = TRUE;
                    else unit[i].fired = TRUE;

            }
        }
    }
    ogre_resolve(&allocated);
}

get_target(i)
int i;
{

    char    action, invalid;

    movecur_unit(i);

    do {

        invalid = FALSE;
        action = getchar();
    
        switch(action) {
    
            case PASS:
                return(PASS);
    
            case MISSILE:
                if(ogre.missiles > 0) {
                    allocated.missiles += unit[i].attack;
                    update_odds(action);
                }
                else {
                    invalid = TRUE;
                }
                break;
    
            case MAIN:
                if(ogre.main_bats > 0) {
                    allocated.main_bats += unit[i].attack;
                    update_odds(action);
                }
                else {
                    invalid = TRUE;
                }
                break;
    
            case SECONDARY:
                if(ogre.sec_bats > 0) {
                    allocated.sec_bats += unit[i].attack;
                    update_odds(action);
                }
                else {
                    invalid = TRUE;
                }
                break;
    
            case AP:
                if(ogre.ap > 0) {
                    allocated.ap += unit[i].attack;
                    update_odds(action);
                }
                else {
                    invalid = TRUE;
                }
                break;
    
            case TREAD:
                if(ogre.treads > 0) {
                    allocated.treads += unit[i].attack;
                    update_odds(action);
                }
                else {
                    invalid = TRUE;
                }
                if(invalid) break;

                /* TREAD has to be resolved immediately. */
                ogre_resolve(&allocated);
                zero(&allocated, sizeof(allocated));
                break;

            case RESOLVE:
                ogre_resolve(&allocated);
                zero(&allocated, sizeof(allocated));
                return(PASS);
		/* NOTREACHED */
                break;

            default:
                invalid = TRUE;
                break;
    
        }

    } while(invalid);

    return(NOPASS);

}


zero(area, size)
OGRE *area;
int  size;
{
    union {
        OGRE *o;
        char *a;
    } p;			/* All this to shut lint up!	*/
    int i;

    p.o = area;
    for(i = 0; i < size; i++) p.a[i] = '\0';

}

update_odds(weapon)
char weapon;
{

    char *odd_str();

    switch(weapon) {
        case MAIN:
            movecur(18, 40);
            eeol();
            printf("%d/%d (%s)", allocated.main_bats, DEF_MAIN,
                odd_str(allocated.main_bats, DEF_MAIN));
            break;
        case SECONDARY:
            movecur(19, 40);
            eeol();
            printf("%d/%d (%s)", allocated.sec_bats, DEF_SECONDARY,
                odd_str(allocated.sec_bats, DEF_SECONDARY));
            break;
        case MISSILE:
            movecur(20, 40);
            eeol();
            printf("%s", odd_str(allocated.missiles, DEF_MISSILES));
            break;
        case AP:
            movecur(21, 40);
            eeol();
            printf("%s", odd_str(allocated.ap, DEF_AP));
            break;
        case TREAD:
            movecur(22, 40);
            eeol();
            printf("1/1 (%d)", allocated.treads);
            break;
    }
}
