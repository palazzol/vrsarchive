#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/structs.h,v 1.3 1987-12-25 20:51:21 vrs Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/structs.h,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/structs.h,v 1.3 1987-12-25 20:51:21 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  87/10/09  11:12:49  11:12:49  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: structs.h
 *
 * Struct Defs for TREK73
 *
 */

#include "defines.h"

struct phaser {
	struct ship *target;	/* who we're aimed at */
	float bearing;		/* direction aimed (if no target) */
	int drain;		/* Drain from engines (to if negative) */
	short load;		/* energy in phasers */
	char status;		/* Damaged, etc. */
} ;

struct tube {
	struct ship *target;	/* who we're aimed at */
	float bearing;		/* direction aimed (if no target) */
	int load;		/* energy in tubes */
	char status;		/* Damaged, etc */
} ;

struct shield {
	float eff;		/* efficiency from 0-1 */
	float drain;		/* Actual drain from engines */
	float attemp_drain;	/* Attempted drain from engines */
} ;

#define MAXWEAPONS 11
#define SHIELDS 4

struct ship {
	char name[30];			/* name of ship */
	char class[3];			/* Type of ship */
	int x, y;			/* location */
	float warp;			/* warp speed */
	float newwarp;			/* for speed changes */
	float course;			/* 0-360 */
	float newcourse;		/* for course changes */
	struct ship *target;		/* who we're pursuing */
	float relbear;			/* Relative bearing to keep */
	struct phaser phasers[MAXWEAPONS];	/* phaser banks */
	int p_spread;			/* phaser spread */
	int p_percent;			/* phaser firing percentage */
	int p_blind_left;		/* phaser blind angle, left side */
	int p_blind_right;		/* phaser blind angle, right side */
	struct tube tubes[MAXWEAPONS];		/* torpedo tubes */
	int t_prox;			/* proximity delay */
	int t_delay;			/* time delay to detonation */
	int t_lspeed;			/* launch speed */
	int t_blind_left;		/* tube blind angle, left side */
	int t_blind_right;		/* tube blind angle, right side */
	struct shield shields[SHIELDS]; /* shields */
	int probe_status;		/* Probe launcher status */
	float eff;			/* efficiency */
	float regen;			/* regeneration (energy per turn) */
	float energy;			/* amount of effective energy */
	float pods;			/* max energy level */
	int complement;			/* crew left alive */
	int status[MAXSYSTEMS];		/* damage percentage of these systems */
	float delay;			/* how long 'till we blow up? */
	int id;				/* Unique identifier */
	int num_phasers;		/* Number of phasers */
	int num_tubes;			/* Number of tubes */
	float orig_max;			/* Maximum original warp */
	float max_speed;		/* Maximum warp */
	float deg_turn;			/* Degrees per warp turn */
	float ph_damage;		/* Damage divisor from phasers */
	float tu_damage;		/* Damage divisor from tubes */
	int cloaking;			/* Cloaking device status */
	int cloak_energy;		/* Energy needed to run cloak */
	int cloak_delay;		/* Time until you can recloak */
	int (*strategy)();		/* Which strategy to use */
	struct pos {			/* Last known position (before cloak) */
		int x,y;		/* Coordinates */
		float warp;		/* Warp speed */
		int range;		/* Distance to ship */
		float bearing;		/* Bearing */
		float course;		/* Course */
	} position ;
	int p_firing_delay;		/* Delay in segments to fire phasers */
	int t_firing_delay;		/* Delay in segments to fire torpedos */
} ;

/*
 * note that probes act like torpedos
 * but have targets; torps only have
 * courses
 */
struct torpedo {
	struct ship *from;	/* pointer to ship they're from */
	int x, y;		/* coords of object */
	float course;		/* where it's going */
	float speed;		/* how fast we're moving */
	float newspeed;		/* what our target speed is */
	struct ship *target;	/* who we're aimed at */
	int fuel;		/* how many antimatter pods it has */
	float timedelay;	/* seconds until detonation */
	int prox;		/* proximity fuse */
	int id;			/* Unique identifier */
	int type;		/* torpedo, probe, or engineering */
} ;

/*
 * the list of what's in space -- depending on the type, we use
 * differing parts of the union (data) structure.  it's a linked
 * list of all the stuff in space.
 */
struct list {
	int type;			/* Type of pointer kept */
	struct list *back, *fwd;	/* Pointers fore and aft */
	union {
		struct torpedo *tp;	/* Hold a ptr to a torpedo */
		struct ship *sp;	/* ...or to a ship */
	} data;
} ;

struct cmd {
	int (*routine)();		/* Routine to call */
	char *code_num;			/* Code number of command */
	char *explanation;		/* Explanation of command */
	int turns;			/* Does it take a turn? */
} ;

/*
 * for the phaser and anti-matter damage lists
 */

/*
 * When you are hit, for each system, you roll a roll-sided die.
 * If this number is less than the actual hit you took, then the
 * system has been damaged.  Historically, this system would be
 * completely damaged, but here we simply add more damage to that
 * system.  See damage.c for more details
 */
struct dam {
	int roll;		/* How many sides on the die */
	char *mesg;		/* Message to print out */
};

/*
 * How to allocate damage.
 * For all but crew, the amount of a hit that gets through is divided
 * by the field and that amount is added or subtracted from the
 * relevant stat.  The number for weapon is the number of weapons that
 * have been damaged.
 */
struct damage {
	float eff;			/* Damage to efficiency */
	float fuel;			/* Loss of fuel capacity */
	float regen;			/* Loss of regeneration */
	float crew;			/* Crew casualties */
	float weapon;			/* Chance of weapons damage */
	struct dam stats[S_NUMSYSTEMS];	/* For each system */
};

struct ship_stat {
	char abbr[4];		/* Abbreviation */
	int class_num;		/* Index into array */
	int num_phaser;		/* Number of phasers */
	int num_torp;		/* Number of tubes */
	int o_warpmax;		/* Own max speed */
	int e_warpmax;		/* Enemy max speed */
	float o_eff;		/* Own efficiency */
	float e_eff;		/* Enemy efficiency */
	float regen;		/* Regeneration */
	float energy;		/* Starting fuel */
	float pods;		/* Max pods */
	int o_crew;		/* Own crew */
	int e_crew;		/* Enemy crew */
	float ph_shield;	/* Divisor for phaser damage */
	float tp_shield;	/* Divisor for torp damage */
	int turn_rate;		/* Degrees per warp-second */
	int cloaking_energy;	/* Energy to run cloaking device */
	int t_blind_left;	/* Start of tube blind area left */
	int t_blind_right;	/* Start of tube blind area right */
	int p_blind_left;	/* Start of phaser blind area left */
	int p_blind_right;	/* Start of phaser blind area right */
	/* Must change to absolute time */
	int p_firing_delay;	/* Delay in segments to fire phasers */
	int t_firing_delay;	/* Delay in segments to fire torpedos */
};

struct race_info {
	char race_name[30];	/* Name of the race */
	char empire_name[30];	/* What they call themselves */
	int id;			/* Identifier number */
	int surrender;		/* Chance they will accept a surrender */
	int surrenderp;		/* Chance they will surrender to you */
	int corbomite;		/* Chance they fall for a corbomite bluff */
	int defenseless;	/* Chance they fall for a defenseless ruse */
	int attitude;		/* Attitude factor for strategies */
	char *ship_names[MAXESHIPS];	/* Ship names */
	char *ship_types[MAXSHIPCLASS];	/* Ship types */
	char *captains[MAXECAPS];	/* Some exemplary captains */
};
