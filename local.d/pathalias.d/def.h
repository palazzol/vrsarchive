/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
#ifdef MAIN
static char	*h_sccsid = "@(#)def.h	7.1 (down!honey) 85/08/06";
#endif /*MAIN*/
#endif /*lint*/

#include <stdio.h>
#include <ctype.h>
#include "config.h"

typedef	long Cost;
typedef struct node node;
typedef struct link link;

#ifdef lint
#define vprintf fprintf
#else /*!lint -- this gives null effect warning*/
/* because it's there ... */
#define vprintf		!Vflag ? 0 : fprintf
#endif /*lint*/

/* scanner (yylex) states */
#define OTHER 0
#define COSTING 1
#define NEWLINE 2
#define PRIVATING 3

#define	isnetc(c)	((c)=='!' || (c)==':' || (c)=='@' || (c)=='%')

#define dirbits(c)	(c)

/* flags for n_flag */
#define ISPRIVATE  0x0001 /* this node invisible outside its definition file */
#define DEHASH	   0x0002 /* removed from hash table yet? */
#define ATSIGN	   0x0004 /* seen an at sign?  used for magic @/% rules */
#define MAPPED	   0x0008 /* done mapping this node */
#define	NDEAD	   0x0010 /* node is dead */
#define HASLEFT	   0x0020 /* route has a left side net character */
#define HASRIGHT   0x0040 /* route has a right side net character */
#define	NNET	   0x0080 /* node is a network name */
#define INDFS	   0x0100 /* used when removing net cycles */
#define DUMP	   0x0200 /* we have dumped this net's edges */
#define NDOMAIN	   0x0400 /* use .domain style addressing */
#define GATEWAYIN  0x0800 /* heaped via gatewayed net */
#define COLLISION  0x1000 /* collides with a private host name */

#define DEADNET(n) (((n)->n_flag & (NNET | NDEAD)) == (NNET | NDEAD))

/*
 * save some space in nodes -- there are > 10,000 allocated!
 *
 *	node	*n_net		others in this network (parsing)
 * 	node	*n_root		root of net cycle (mapping)
 *
 *	node	*n_private	other privates in this file (parsing)
 *	char	*n_path		path to this host (mapping)
 *		
 */

#define n_root n_unetroot.nu_root
#define n_net n_unetroot.nu_net

#define n_private n_uprivatepath.nu_private
#define n_path n_uprivatepath.nu_path

struct node {
	char	*n_name;	/* host name */
	link	*n_link;	/* head of adjacency list */
	node	*n_alias;	/* real node (when this node is an alias) */
	node	*n_aliaslink;	/* other aliases for this node */
	union {
	    node	*nu_root;	/* root of net cycle (mapping) */
	    node	*nu_net;	/* others in this network (parsing) */
	}	n_unetroot;
	union {
	    node	*nu_private;	/* other privates in this file (parsing) */
	    char	*nu_path;	/* path to this host (mapping) */
	}	n_uprivatepath;	
	Cost	n_cost;		/* cost to this host */
	short	n_tloc;		/* back ptr to heap/hash table */
	short	n_flag;		/* see manifests above */
};

#define	DEFNET	'!'			/* default network character */
#define	DEFDIR	LLEFT			/* host on left in default net */
#define	DEFCOST	((Cost) 4000)		/* default cost of a link */
#define	INF	((Cost) 30000000)	/* infinitely expensive link */

/* data structure for adjacency list representation */

/* flags for l_dir */

/*
 * there's an ugly dependency between the following manifests and the
 * variable Netchars = "!:^@%", defined in extern.c.  this saves 2
 * bytes per link (of which there are well over 20k).  this does not
 * mean i'm satsified with bad design.
 */
#define NETDIR(l)	((l)->l_flag & LDIR)
#define NETCHAR(l)	(Netchars[(l)->l_flag & LNETCHARS])

#define LNETCHARS	0x3
#define LBANG		0x0
#define LCOLON		0x1
#define LAT		0x2
#define LPERCENT	0x3

#define LDIR	0x8	/* 0 for left, 1 for right */
#define LRIGHT	0x0	/* user@host style */
#define LLEFT	0x8	/* host!user style */

#define LDEAD	0x10	/* this link is dead */
#define LDOMAIN 0x20	/* use host.domain.  i feel sick. */

struct link {
	link	*l_next;	/* rest of adjacency list */
	node	*l_to;		/* adjacent node */
	Cost	l_cost;		/* edge cost */
	char	l_flag;		/* right/left syntax */
};

node	*addnode(), *newnode(), **newtable(), *addprivate();
link	*addlink(), *lmerge(), *newlink();
char	*strsave(), *local();
void	pack();

#define STATIC static

extern node	*Home;
extern char	*Cfile;
extern int	Fcnt;
extern char	**Ifiles;
extern char	*ProgName;
extern int	Lineno;
extern node	**Table;
extern int	Tabsize;
extern char	*Netchars;
extern int	Vflag;
extern int	Cflag;
extern int	Iflag;
extern int	Ncount;
extern int	Lcount;
extern char	*Pathout;
extern char	*Graphout;
extern char	*Linkout;
extern node	*Private;
extern int	Hashpart;
extern int	Scanstate;
