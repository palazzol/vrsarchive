/*
 * BASIC by Phil Cockcroft
 */
/*
 * configuration file for pyramid systems (BSD unix only)
 */

/*
 * memory sizes.
 */

#define MAXMEM  (memp)1000000   /* max amount of memory */
#define MEMINC  8191            /* sizeof memory increments -1 */

/*
 * various options
 */

#define V7                      /* must be defined */
#define BERK                    /* must be defined */
#define UCB_NTTY                /* must be defined */
#define LKEYWORDS
#define LNAMES
#define RENUMB
#define SCOMMS
#define VFORK
#define BLOCKSIZ        1024
#define BSD42
#define ALIGN4
#define MPORTABLE
#define PORTABLE

/*
 * terminal dependent stuff
 */

#define CTRLINT         03      /* sig int control c */
#define CTRLQUIT        034     /* sig quit FS */
#define DEFPAGE         80      /* default terminal width */
#define DEFLENGTH       24      /* default number of lines on screen */

/* #define VAX_ASSEM */ /* if you want to use assembler in various routines*/
			/* this only works for the vax */

/* #define NOEDIT    /* define if don't want editing ever ! */
		     /* NB basic -e will still turn on editing */
		     /* basic -x will still turn off editing */

/* #define LKEYWORDS /* define this if you want to have variable names which*/
		     /* contain commands this is like the later versions of */
		     /* microsoft but not like the orignal version */
		     /* it wastes more space since you have to have some */
		     /* spaces in to distinguish keywords */

/* #define RENUMB    /* define if you want to put the code for renumbering */
		     /* in. It works but is very wasteful of space. If you */
		     /* are short of space then don't use it. */

/* #define LNAMES    /* define if you want long variables names. This only */
		     /* slows it down by a small fraction */

/* #define _BLOCKED  /* This is a switch to allow block mode files */
		     /* don't define it here look below for where it is done*/
		     /* in the file handling bits */

/* #define BSD42     /* if useing a 4.2 system */

/* #define SCOMMS    /* to allow shortened command names e.g. l. -> list */
		     /* this might cause some problems with overwriting of */
		     /* core but I think they have all been solved */
/* #define VFORK     /* if your system supports vfork() */
