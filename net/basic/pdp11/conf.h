/*
 * BASIC by Phil Cockcroft
 */
/*
 * Configuration file for a pdp11
 */
/*
 * hardware specific. Can't change MAXMEM upwards
 */

#define MAXMEM          (memp)0160000   /* max data address on a pdp11 */
#define MEMINC          1023            /* size of memory increments - 1 */

/*
 * various options.
 */

#define V7
#define UCB_NTTY
#define LKEYWORDS
#define LNAMES
#define RENUMB
#define SCOMMS
#define BERK

#ifdef  BERK
#define BLOCKSIZ        1024
#else
#define BLOCKSIZ        512
#endif

/*
 * terminal specific options
 */
#define DEFPAGE         80      /* default page width */
#define DEFLENGTH       24      /* default page length */
#define CTRLINT         03      /* ctrl -c - sig int */
#define CTRLQUIT        034     /* ctrl - \ FS sig quit */

/* #define V7     */    /* define for v7 */
/* #define SOFTFP */    /* define if not got fp hardware */
/* #define V6C    */    /* if got V6 compiler (no structure assignments ) */
/* #define BERK   */    /* define if got Berkley tty driver ( not v6 ) */
/* #define UCB_NTTY */  /* if got the new driver ..... */

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
/* #define SCOMMS    /* to allow shortened command names e.g. l. -> list */
		     /* this might cause some problems with overwriting of */
		     /* core but I think they have all been solved */
