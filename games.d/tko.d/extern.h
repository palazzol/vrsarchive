/*
Copyright (c) 1979  Mike Gancarz
Permission is hereby granted to freely distribute, copy, modify and otherwise
use this software for nonprofit purposes as long as this copyright appears
in the file and remains intact.
*/

/** boxing action card **/
struct action{
	byte	acrp;		/* ring position */
	char	acrpchk;	/* check ring position */
	byte	accf;		/* control factor */
	char	achand;		/* left- or right-hand punch */
	byte	accutk;		/* check for cuts or knockdowns */
	byte	acpunch;	/* punch number */
	byte	ackd;		/* knockdown attempt */
	byte	ackdr;		/* knockdown rating */
	byte	ackor;		/* knockout rating */
	char	acj1;		/* judge 1 scoring */
	char	acj2;		/* judge 2 scoring */
	char	acref;		/* referee scoring */
};
extern struct action act[81];

/** boxer table **/
struct boxer {
	char	fname[NAMSIZ];	/* boxer's first name */
	char	mname[NAMSIZ];	/* boxer's middle name */
	char	lname[NAMSIZ];	/* boxer's last name */
	byte	rating;		/* overall rating */
	char	style;		/* style */
	byte	weight;		/* weight class */
	byte	cfb;		/* control factor against a boxer */
	byte	cfs;		/* control factor against a slugger */
	byte	hp;		/* hit power */
	byte	ki;		/* killer instinct */
	byte	kdr1;		/* knockdown rating 1 */
	byte	kdr2;		/* knockdown rating 2 */
	byte	kor;		/* knockout rating */
	byte	co;		/* cut opponent rating */
	byte	ch;		/* cut himself rating */
	byte	tko;		/* technical knockout rating */
	byte	agg;		/* aggressiveness rating */
	byte	end;		/* 10-round endurance */
	byte	def;		/* defense rating */
	byte	fi;		/* fighting inside strategy available */
	byte	fo;		/* fighting outside strategy available */
	byte	cu;		/* cover-up strategy available */
	byte	ko;		/* knockout strategy available */
	byte	pl;		/* punches landed upper limit */
	byte	pm;		/* punches missed upper limit */
	byte	cl;		/* clinching upper limit */
	byte	foul;		/* foul rating */
	byte	jab3;		/* 3-point jab upper limit */
	byte	jab2;		/* 2-point jab upper limit */
	byte	hook3;		/* 3-point hook upper limit */
	byte	hook2;		/* 2-point hook upper limit */
	byte	cross3;		/* 3-point cross upper limit */
	byte	cross2;		/* 2-point cross upper limit */
	byte	combo3;		/* 3-point combination upper limit */
	byte	combo2;		/* 2-point combination upper limit */
	byte	upper3;		/* 3-point uppercut upper limit */
	byte	icor;		/* in corner/on ropes adjustment factor */
};
extern struct boxer bx[];

/** run-time statistics **/
struct rtstat {
	int	bxptr;		/* pointer to bx[] structure offset */
	byte	ccf;		/* current control factor */
	byte	chp;		/* current hit power */
	byte	ckdr1;		/* current knockdown rating 1 */
	byte	cagg;		/* current aggressiveness rating */
	int	cend;		/* amount of endurance left */
	byte	cdef;		/* current defense rating */
	byte	cpl;		/* current punches landed upper limit */
	byte	cnpm;		/* current number of punches missed */
	byte	cnpts;		/* current number of points */
	byte	catko1;		/* auto-tko score from previous round */
	byte	catko2;		/* auto-tko score from previous round */
	byte	cncpts;		/* current number of cut points */
	byte	pncpts;		/* previous number of cut points */
	byte	cpv3;		/* current punch value of 3-point punches */
	byte	cpv2;		/* current punch value of 2-point punches */
	byte	cj1scr;		/* current judge #1 score */
	byte	cj2scr;		/* current judge #2 score */
	byte	crscr;		/* current referee score */
	byte	ckdflg;		/* current knockdown flag */
};
extern struct rtstat rt[2];

extern byte kdtab[200];
extern byte cuttab[90];
extern byte atko[15];

extern SIG_T savegm();

extern int nboxers;	/* number of boxers in the names file */
extern int nrounds;	/* number of rounds in the fight */
extern int round;	/* current round number */
extern int a;		/* boxer a  bx[] offset */
extern int b;		/* boxer b bx[] offset */
extern int fagg;	/* fight aggressor */
extern int offense;	/* fighter on offense */
extern int actcnt;	/* boxing action card counter */
extern int ccard;	/* current boxing action card */
extern int lcard;	/* last boxing action card */
extern byte killer;	/* killer instinct flag */
extern byte kdcount;	/* knockdown count */
extern byte deck;	/* which half of boxing act. card deck is being used */
extern char *wc[12];	/* weight classes */
extern byte w;		/* weight class for this fight */
extern int speed;	/* speed at which the fight will take place */
extern char temp[LINE];	/* temporary string buffer */
extern int cused[80];	/* boxing action cards already used this round */
extern char ptype[20];	/* verbal description of type of punch landed */
