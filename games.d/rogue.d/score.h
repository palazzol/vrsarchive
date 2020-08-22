/*
 * Score file structure
 *
 * @(#)score.h	6.2 (Berkeley) 5/28/84
 */

struct sc_ent {
	char sc_name[MAXSTR];
	unsigned int sc_flags;
	unsigned int sc_uid;
	unsigned short sc_monster;
	unsigned short sc_score;
	unsigned short sc_level;
};

typedef struct sc_ent SCORE;
