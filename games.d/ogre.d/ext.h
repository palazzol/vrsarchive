#include "ogre.h"

#ifdef MAIN
UNIT unit[N_UNITS];
OGRE ogre;
int n_units;

#else
extern UNIT unit[N_UNITS];
extern OGRE ogre;
extern int n_units;
#endif

#ifdef lint
#  define SHORTNAMES
#endif

#ifdef SHORTNAMES
/*
 *	gets rid of variable/routine name length problems. (psk)
*/
#  define	disp_ogre		DSP1
#  define	disp_ogre_status	DSP2
#  define	init_ogre		INIT1
#  define	init_ogre_attack	INIT2
#  define	movecur_hex		MV1
#  define	movecur_unit		MV2
#  define	move_ogre		MVOGR1
#  define	move_ogre1		MVOGR2
#  define	update_odds		UPDT1
#  define	update_hex		UPDT2
#endif
