/*
 * BASIC by Phil Cockcroft
 */
/*
 * configuration file for Motorola 68000 systems
 */

/*
 * standard constants of a motorola 68000 processor
 */

#define MAXMEM  (memp)500000    /* maximum memory it is allowed */
#define MEMINC  8191            /* memory increment size -1 */
#define BLOCKSIZ 512            /* size of disk blocks */
#define MPORTABLE		/* must use portable memory allocation */
/*
 * could possibly not use this.
 * It would make it much faster if we didn't need to 
 * It is used to make the Fp routines portable.
 */
#define PORTABLE                /* must use the portable version of */
				/* the code */

/*
 * various options
 */

#define V7
#define BERK
#define LKEYWORDS
#define LNAMES
#define RENUMB
#define SCOMMS
#define MCBREAK

/*
 * various terminal options
 */

#define CTRLINT         03      /* the interupt character */
#define CTRLQUIT        034     /* the quit FS character */
#define DEFPAGE         80      /* default page width */


/* #define V7     */ /* define for v7 */
/* #define BERK   */ /* define if got Berkley tty driver ( not v6 ) */

/* #define MCBREAK   /* define if you want to always be in cbreak mode */
		     /* because the terminal driver is broken */

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
