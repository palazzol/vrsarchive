#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#ifdef USG
#include <fcntl.h>
#endif
#include <signal.h>
#include <sgtty.h>
#include "at.h"

#define NEWSVERSION	 "B UNSW 1.1 19 Sep 1984"

/* Things that very well may require local configuration */

#define TIMEZONE	"EST"		/* name of time zone */

#define DFLTSUB "general,general.all"	/* default subscription list	*/
#define	ADMSUB	"general"		/* Mandatory subscription list */
#define MODGROUPS "mod.all,all.mod,all.announce"	/* Moderated groups */
#define DFLTGRP	"general"		/* default newsgroup (for postnews) */
/* #define MANGRPS	 1		/* if you have mandatory subscriptions
					   tailored per-person (uses
					   getclasses()) */
/*#define OZ	1*/			/* if on Australian network, used
					   in readnews to get correct return
					   address */
/*#define AUSAM	1*/			/* hashed passwd file, locked files */
#if AUSAM
#include <passwd.h>
#else
#include <pwd.h>
#endif

#ifdef vax
/* #define NETPATH	1		/* if you have path finding program
					   /bin/netpath */
#endif
/*#define UNSWMAIL 1*/			/* if you have UNSW "mail" which
					   allows "-s subject -i include_file"
					   arguments */
#define NETID "utstat"
#ifndef NETID
#define NETID "utstat"			/* else define it here */
#endif

#ifndef NETID
#include <table.h>			/* UNSW only */
#endif

/* #define MC "/usr/bin/p"			/* pager */
#define UUNAME "/usr/bin/uuname"
#define RNEWS	"exec rnews"		/* rnews for uurec to fork */
#define POSTNEWS "/usr/bin/inews"
#define CHOWN	"/etc/chown"		/* pathname of chown command */
#define SHELL	"/bin/sh"		/* if not bourne shell see postnews.c */
#define MKDIR	"/bin/mkdir"
#define MAIL	"/bin/mail"
#if UNSWMAIL
#define FASTMAIL	"/bin/mail"
#else
#define FASTMAIL	MAIL
#endif

#define HELP	"/usr/lib/news/help.readnews"		/* Help text */
#define SEQ	"/usr/lib/news/seq"		/* Next sequence number */
#define SYS	"/usr/lib/news/sys"		/* System subscription lists */
#define ACTIVE	"/usr/lib/news/active"		/* Active newsgroups */
#define HISTORY "/usr/lib/news/history"		/* Current articles */

#define MYDOMAIN "uucp"			/* Local domain */
#define MYORG	"U. of Toronto Statistics" /* My organization */
#define NEWSROOT "news"			/* news editor */

/* Things you might want to change */

#define NEWSRC  ".newsrc"		/* name of .newsrc file */
#define	PAGESIZE 24			/* lines on screen */
#define ARTICLES "articles"		/* default place to save articles */
#define NEGCHAR	'!'			/* newsgroup negation character	*/
#define NEGS	"!"			/* ditto (string) */
#define BADGRPCHARS "/#!"		/* illegal chars in group name */
#define BUFLEN	256			/* standard buffer size */
#define ED	"/bin/ed"		/* default, if $EDITOR not set */

/* Things you probably won't want to change */

#define	NGSEPCHAR ','	/* delimit character in news group line		*/
#define NGSEPS	","	/* ditto */
#define PSEPS "!"	/* separator in Path: */
#define PSEPCHAR '!'	/* ditto */
#define PATHPREF "..!"	/* prefix for addresses worked out from Path: */
#define TRUE	1
#define FALSE	0

#ifndef F_SETFD
#ifdef F_SETFL
#define F_SETFD F_SETFL		/* SETFL becomes SETFD (close on exec arg
				   to fcntl) */
#endif
#endif

typedef enum booltype { false = 0, true } bool;
typedef enum applytype { stop, next, nextgroup, searchgroup } applycom;
typedef applycom (*apcmfunc)();
typedef enum pheadtype { printing, passing, making } pheadcom;

/*
 * header structure
 */
typedef struct header {
	/* mandatory fields */
	char	*h_relayversion;
	char	*h_postversion;
	char	*h_from;
	char	*h_date;
	char	*h_newsgroups;
	char	*h_subject;
	char	*h_messageid;
	char	*h_path;
	/* optional fields */
	char	*h_replyto;
	char	*h_sender;
	char	*h_followupto;
	char	*h_datereceived;
	char	*h_expires;
	char	*h_references;
	char	*h_control;
	char	*h_distribution;
	char	*h_organisation;
	char	*h_lines;
	/* any we don't recognise */
	char	*h_others;
} header;

/*
 * internal structure for active file
 */
typedef struct active active;
struct active {
	char	*a_name;
	short	a_seq;
	short	a_low;
	active	*a_next;
};

/*
 * internal struct for newsrc file
 */
typedef struct newsrc newsrc;
struct newsrc {
	char	*n_name;
	bool	n_subscribe;
	short	n_last;
	newsrc	*n_next;
};

char	*strrchr(), *strchr(), *strcat(), *strcpy(), *strpbrk();
char	*itoa(), *convg(), *ngsquash(), *ttoa(), *mgets(), *rconvg();
char	*newstr(), *newstr2(), *newstr3(), *newstr4(), *newstr5(), *catstr();
char	*catstr2(), *bsearch(), *mtempnam(), *newstr6();
char	*getunique(), *getretaddr(), *getsubject();
FILE	*fopenl(), *fopenf();
char	*memset(), *myalloc(), *myrealloc();
long	time(), atol(), atot();
int	strpcmp();
active	*readactive();
char *getenv();

#define NIL(type)	((type *) 0)
#define NEW(type)	((type *) myalloc(sizeof(type)))
#define CMP(a, b)	(*(a) != *(b) ? *(a) - *(b) : strcmp(a, b))
#define CMPN(a, b, n)	(*(a) != *(b) ? *(a) - *(b) : strncmp(a, b, n))

/* bw 9/15/84 */
#define uid_t int
#define strchr index
#define strrchr rindex
