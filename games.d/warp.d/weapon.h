/* $Header: /home/Vince/cvs/games.d/warp.d/weapon.h,v 1.1 1987-07-26 10:21:27 vrs Exp $ */

/* $Log: not supported by cvs2svn $
 * Revision 7.0  86/10/08  15:18:20  lwall
 * Split into separate files.  Added amoebas and pirates.
 * 
 */

EXT int tractor INIT(0);

EXT int etorp;
EXT int btorp;

EXT OBJECT *isatorp[2][3][3];

EXT int aretorps;

void fire_torp();
void attack();
void fire_phaser();
int tract();
void weapon_init();
