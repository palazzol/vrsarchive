/*
    These routines implement all functions associated with the map and display
    thereof.

    (Thanks to Bob Hood of Rice for coming up with 
     the x-y and range algorithms.)

    Michael Caplinger, Rice University, March 1982.
*/

#include "ext.h"
#include <ctype.h>

int lastunit;


/* Initialize the map display, at the beginning of the game. */
init_screen() {

    int a, b;

    tc_setup();
    clear_screen();

    for(a = 1; a <= 28; a++) {
        for(b = 1; b <= 28; b++) {
            if(!off_map(a, b)) {
                disp_hex(a, b, '.');
            }
        }
    }
    disp_craters();
}

redraw_screen() {

    int a, b;

    clear_screen();

    for(a = 1; a <= 28; a++) {
        for(b = 1; b <= 28; b++) {
            if(!off_map(a, b)) {
                update_hex(a, b);
            }
        }
    }

    describe_action("Move", lastunit);
    disp_ogre_status(1);
}

/* 
    Convert a left and right hex pair (eg, the hex 2015 has an l_hex of 20 and
    an r_hex of 15) to x-y screen coordinates.
*/
to_xy(lhex, rhex, row, col)
char lhex, rhex, *row, *col;
{

    *row = (lhex - rhex) + 7;
    *col = 50 - (lhex + rhex);

}

/* Check to see if an lr pair is off the map. */
off_map(a, b)
char a, b;
{
    char row, col;

    to_xy(a, b, &row, &col);
    if(col < 0 || col > 38 || row < 0 || row > 14) return(TRUE);
    else return(FALSE);

}

/* Check to see if an lr pair is off the obstructed area of the map. */
off_obstructed(a, b)
char a, b;
{
    char row, col;

    to_xy(a, b, &row, &col);
    if(col < 10 || col > 38 || row < 0 || row > 14) return(TRUE);
    else return(FALSE);

}

/* Display a character at a given hex. */
disp_hex(a, b, c)
char a, b, c;
{
    char row, col;

    to_xy(a, b, &row, &col);

    movecur(row, col * 2 + 1);
    putchar(c);

}

/* 
    Display the contents of a hex.  If more than one item is in a hex,
    the following precedence applies:
        1) Ogre
        2) Defending units (by value)
        3) Craters (not that anything can be in a crater hex.)
*/
update_hex(a, b)
char a, b;
{

    int i;

    if(ogre.l_hex == a && ogre.r_hex == b) {
        disp_ogre();
        return;
    }

    for(i = 0; i < n_units; i++)
        if(unit[i].l_hex == a && unit[i].r_hex == b &&
            unit[i].status != DESTROYED) {
            disp_unit(i);
            return;
        }

    if(blocked(a, b)) {
        disp_hex(a, b, '*');
        return;
    }

    disp_hex(a, b, '.');

}

/* Display the ith unit. */
disp_unit(i)
int i;
{
    char a, b;

    a = unit[i].l_hex;
    b = unit[i].r_hex;

    switch(unit[i].status) {

        case OK:

            switch(unit[i].type) {

                case INFANTRY:
                    disp_hex(a, b, '0' + unit[i].attack);
                    break;

                default:
                    disp_hex(a, b, unit[i].type);
                    break;

            }
            break;

        case DISABLED:
            disp_hex(a, b, tolower(unit[i].type));
            break;

        case DESTROYED:
            disp_hex(a, b, '.');
            break;

    }

}

/* Display the Ogre. */
disp_ogre()
{
    char a, b;

    a = ogre.l_hex;
    b = ogre.r_hex;

    disp_hex(a, b, 'O');

}


/* Move the cursor to the specified hex on the screen. */
movecur_hex(a, b)
char a, b;
{
    char row, col;

    to_xy(a, b, &row, &col);

    movecur(row, col * 2 + 1);

}

/* Point at the ith unit with the cursor. */
movecur_unit(i)
int i;
{

    movecur_hex(unit[i].l_hex, unit[i].r_hex);

}

#define ABS(i) (((i) < 0) ? -(i) : (i))
#define BIGINT 32767

/* Calculate the range between 2 hexes. */
range(a1, b1, a2, b2)
char a1, b1, a2, b2;
{

    char    diff1, diff2, temp;
    int     subrange[3];
    int     min, i;
    int     rangesum;

    diff1 = a1 - b1;
    diff2 = a2 - b2;

    subrange[0] = ABS(a1 - a2);
    subrange[1] = ABS(b1 - b2);
    subrange[2] = ABS(diff1 - diff2);

    min = 0;
    for(i = 1; i < 3; i++)
        if(subrange[i] < subrange[min]) min = i;

    rangesum = subrange[min];

    temp = subrange[min]; subrange[min] = subrange[2]; subrange[2] = temp;

    min = 0;
    for(i = 1; i < 2; i++)
        if(subrange[i] < subrange[min]) min = i;

    rangesum += subrange[min];

    return(rangesum);

}

/*
    This is a hardwired set of craters, taken from the paper game's map.
*/
static struct {
    char l_hex;
    char r_hex;
} craters[] = {
    17, 16,
    19, 13,
    13, 18,
    14, 15,
    13, 15,
    15, 10,
    9,  15,
    10, 12,
    7,  14,
    11, 10,
    14, 7,
    12, 6,
    7,  10,
    8,  6,
    4,  9,
    9,  4,
    9,  3,
};

#define NCRATERS    (sizeof(craters) / 2 * sizeof(char))

/* Determine if a hex has a crater. */
blocked(a, b)
char a, b;
{
    int i;

    for(i = 0; i < NCRATERS; i++) 
        if(craters[i].l_hex == a && craters[i].r_hex == b) return(TRUE);

    return(FALSE);

}

/* Display the craters. */
disp_craters()
{
    int i;

    for(i = 0; i < NCRATERS; i++) 
        disp_hex(craters[i].l_hex, craters[i].r_hex, '*');

}

#include <stdio.h>

describe_action(action, i)
char *action;
int i;
{

    lastunit = i;

    switch(unit[i].type) {

        case HOWITZER:
            movecur(16, 0);
            eeol();
            printf("%s howitzer (%d/%d D%d M%d)", action,
                unit[i].attack, unit[i].range, 
                unit[i].defend, unit[i].moves_left);
            break;

        case MSLTANK:
            movecur(16, 0);
            eeol();
            printf("%s missile tank (%d/%d D%d M%d)", action,
                unit[i].attack, unit[i].range, 
                unit[i].defend, unit[i].moves_left);
            break;

        case GEV:
            movecur(16, 0);
            eeol();
            printf("%s GEV (%d/%d D%d M%d)", action,
                unit[i].attack, unit[i].range, 
                unit[i].defend, unit[i].moves_left);
            break;

        case HVYTANK:
            movecur(16, 0);
            eeol();
            printf("%s heavy tank (%d/%d D%d M%d)", action,
                unit[i].attack, unit[i].range, 
                unit[i].defend, unit[i].moves_left);
            break;

        case INFANTRY:
            movecur(16, 0);
            eeol();
            printf("%s infantry (%d/%d D%d M%d)", action,
                unit[i].attack, unit[i].range, 
                unit[i].defend, unit[i].moves_left);
            break;

    }

}
