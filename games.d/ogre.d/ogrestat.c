/*
    Handle the Ogre status display.
*/

#include "ext.h"

disp_ogre_status(redraw)

/* If redraw is false, the display is not touched if nothing has changed. */
int redraw;
{
    static OGRE last;

    /*
        The Ogre status display occupies the bottom 6 lines of the display.
    */

    /*               0        1         2         3         4
                     1234567890123456789012345678901234567890       */

    if(redraw || last.main_bats != ogre.main_bats) {
        movecur(18, 0);
        eeol();
        if(ogre.main_bats > 0)
            printf("Main Batteries:      %d (4/3 D4)", ogre.main_bats);
    }
    if(redraw || last.sec_bats != ogre.sec_bats) {
        movecur(19, 0);
        eeol();
        if(ogre.sec_bats > 0)
            printf("Secondary Batteries: %d (3/2 D3)", ogre.sec_bats);
    }
    if(redraw || last.missiles != ogre.missiles) {
        movecur(20, 0);
        eeol();
        if(ogre.missiles > 0)
            printf("Missiles:            %d (6/5 D3)", ogre.missiles);
    }
    if(redraw || last.ap != ogre.ap) {
        movecur(21, 0);
        eeol();
        if(ogre.ap > 0)
            printf("Anti-personnel:     %2d (1/1 D1)", ogre.ap);
    }
    if(redraw || last.treads != ogre.treads) {
        movecur(22, 0);
        eeol();
        if(ogre.treads > 0)
            printf("Treads:             %2d (1/* D1)", ogre.treads);
    }
    if(redraw || last.movement != ogre.movement) {
        movecur(23, 0);
        eeol();
        printf("Movement:            %d", ogre.movement);
    }
    copy(&last, &ogre, sizeof(last));
}

copy(to, from, size)
OGRE *to, *from;
int size;
{
    union {
        OGRE *o;
        char *c;
    } f, t;			/* All this to shut up lint		*/
    int i;

    f.o = from;
    t.o = to;
    for(i = 0; i < size; i++) t.c[i] = f.c[i];

}
