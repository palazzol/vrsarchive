/*
 * parsed form of the SYSFILE
 */
struct system {
	char *sy_name;		/* machine name */
	char *sy_excl;		/* exclusion list of machines */
	char *sy_ngs;		/* newsgroup subscription list */
	char *sy_distr;		/* distribution list */
	char *sy_cmd;		/* command to transmit articles */
	unsigned sy_lochops;	/* flags Ln value: local hops */
	char sy_flags;		/* ornaments, encoded as bits */
	struct system *sy_next;	/* link to next system */
};

/* sy_flags bits */
#define FLG_IHAVEOLD	(1<<0)		/* N: broken I-have/send-me proto */
/* #define FLG_PERM	(1<<1)		/* U: %s is a permanent file name */
#define FLG_BATCH	(1<<2)		/* F: sy_cmd is batch filename */
#define FLG_LOCAL	(1<<3)		/* L: send local articles only */
#define FLG_IHAVE	(1<<4)		/* I: new I-have/send-me proto */
#define FLG_MOD		(1<<5)		/* m: send moderated groups only */
#define FLG_UNMOD	(1<<6)		/* u: send unmoderated groups only */
#define FLG_SZBATCH	(1<<7)		/* f: like F, but include byte count */
/* imports from system */
extern struct system *oursys(), *nextsys();
