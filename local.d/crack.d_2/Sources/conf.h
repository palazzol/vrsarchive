#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <signal.h>

/*
 * This switch is for my use - if defined, overrides the defaults below and
 * sets up my own.  It's simplest if you UNDEFINE AND IGNORE this switch.
 */

#define DEVELOPMENT_VERSION

/*
 * !!!! ---------- EVERYBODY ELSE START HERE ---------- !!!!
 */

/*
 * define this symbol if you are on a system where you don't have the
 * strchr() function in your standard library (usually this means you are on
 * a BSD based system) but instead, you DO have the equivalent index()
 * function.
 */

#undef INDEX_NOT_STRCHR

/*
 * What bytesex is your machine ? Select one of the two below, if you have
 * some really weird machine - otherwise the program should be able to work
 * it out itself.
 */

#undef BIG_ENDIAN
#undef LITTLE_ENDIAN

/* If you haven't selected one of the above options... */
#if	!defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)

/* Can we work out if we are little endian ? */
#if 	defined(vax) || defined(ns32000) || defined(sun386) || defined(i386) ||\
	defined(MIPSEL) || defined(BIT_ZERO_ON_RIGHT)
#define LITTLE_ENDIAN		/* YES */
#endif

/* Can we work out if we are bigendian ? */
#if	defined(sel) || defined(pyr) || defined(mc68000) || defined(sparc) ||\
	defined(is68k) || defined(tahoe) || defined(ibm032) ||\
	defined(ibm370) || defined(MIPSEB) || defined(__convex__) ||\
	defined(hpux) || defined(apollo) || defined (BIT_ZERO_ON_LEFT) || \
	defined(m68k) || defined(m88k)
#define BIG_ENDIAN		/* YES */
#endif

/* end of trying to guess things */
#endif

/* are we schitzophrenic ? */
#if	defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)
ERROR_BAD_BIT_ORDER;		/* YES */
#endif

/* are we still ignorant ? */
#if	!defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)
ERROR_NO_BIT_ORDER;		/* YES */
#endif

/*
 * If defined, this will insert code for verbose options
 */

#define CRACK_VERBOSE

/*
 * Define this if you have _toupper() - check your manpages
 */

#undef FAST_TOCASE

/*
 * define this if you are on a Sys V type system with a uname() system call
 * and have no gethostname() - it fakes up a BSD gethostname() so you can use
 * CRACK_NETWORK; see crack-port.c
 */

#undef CRACK_UNAME

/*
 * define this if you are running multiple Cracks over a network; if you
 * aren't using "rsh" to do this, edit "Scripts/RCrack" as well
 */

#undef CRACK_NETWORK

/*
 * Define this to search gecos entry for possible passwords - I RECOMMEND
 * THIS VERY VERY STRONGLY
 */

#define CRACK_GECOS

/*
 * Define this to make a dictionary pass of pluralised words
 */

#define CRACK_PLURAL

/*
 * Define this to make a dictionary pass of words with TRAILING 0's and 1's.
 * If you want to add other leading/trailing characters, edit the
 * "trailing_chars[]" string in "crack-pp.c"
 */

#define CRACK_TRAILING

/*
 * Define this to make a dictionary pass of words with LEADING 0's and 1's.
 * If you want to add other leading/trailing characters, edit the
 * "trailing_chars[]" string in "crack-pp.c"
 */

#define CRACK_LEADING

/*
 * Define this to print the guessed password if you crack it. This is useful
 * for interests sake, and you can find interesting correlations this way
 * too.
 */

/*
 * NOTE: DEFINING CRACK_PRINTOUT IS MANDATORY FOR "FEEDBACK" MODE TO WORK.
 */

#define CRACK_PRINTOUT

/*
 * Define this to print the ENCRYPTED VERSION of password if you crack it.
 * This is useful if you will want to doublecheck that the user HAS changed
 * his password, after you have warned him.
 */

#define CRACK_PRINTOUT2

/*
 * if defined, tries guesses < 5 chars long - this wastes time if you are
 * unlikely to have REALLY short passwords, try it once though.
 */

#define CRACK_SHORT_PASSWDS

/*
 * if defined, do sanity checks on the DES password - this gets round expiry
 * mechanisms appending extra characters to signify "things"
 */

#define CRACK_PW_VERIFY

/*
 * define this if you are using fcrypt() - you might not want to if fcrypt()
 * doesn't work properly
 */

#define FCRYPT

/*
 * The following symbols pertain ONLY to fcrypt() usage
 */

/*
 * if defined use builtin clearing in preference to using bzero(), for 4 or 8
 * byte long ints
 */

#define BUILTIN_CLEAR

/*
 * define if you have a bzero and do not want to use BUILTIN_CLEAR
 */

#define BZERO

/*
 * define this if you have a 4 byte "long_int" on RISC machines and want a
 * speedup - it should not hurt CISC machines either. Do NOT define it on a
 * 8-byte int machine...
 */

#undef FDES_4BYTE

/*
 * define this if you are on a Cray or something with an 8-byte int, to
 * enable Matthew Kaufman's fcrypt fix. I hope it works okay, cos I can't
 * test it - AEM.
 */

#undef FDES_8BYTE

/*
 * undef this if your compiler knows the fact that 6*x == x<<1 + x<<2
 */

#undef BRAINDEAD6

/*
 * Personal stuff only - I recommend you ignore stuff below this line, unless
 * you know what you're doing
 */

#ifdef DEVELOPMENT_VERSION
/*
 * for my personal use, this is my configuration on a DS5000/200
 */
#undef BRAINDEAD6

#ifndef BUILTIN_CLEAR
#define BUILTIN_CLEAR
#endif				/* BUILTIN_CLEAR */

#undef BZERO

#ifndef CRACK_GECOS
#define CRACK_GECOS
#endif				/* CRACK_GECOS */

#ifndef CRACK_NETWORK
#define CRACK_NETWORK
#endif				/* CRACK_NETWORK */

#ifndef CRACK_PLURAL
#define CRACK_PLURAL
#endif				/* CRACK_PLURAL */

#ifndef CRACK_PRINTOUT
#define CRACK_PRINTOUT
#endif				/* CRACK_PRINTOUT */

#ifndef CRACK_PRINTOUT2
#define CRACK_PRINTOUT2
#endif				/* CRACK_PRINTOUT2 */

#ifndef CRACK_PW_VERIFY
#define CRACK_PW_VERIFY
#endif				/* CRACK_PW_VERIFY */

#undef CRACK_SHORT_PASSWDS

#ifndef CRACK_TRAILING
#define CRACK_TRAILING
#endif				/* CRACK_TRAILING */

#undef CRACK_UNAME

#ifndef CRACK_VERBOSE
#define CRACK_VERBOSE
#endif				/* CRACK_VERBOSE */

#undef FAST_TOCASE

#ifndef FCRYPT
#define FCRYPT
#endif				/* FCRYPT */

#ifndef FDES_4BYTE
#define FDES_4BYTE
#endif				/* FDES_4BYTE */

#undef FDES_8BYTE

#endif				/* DEVELOPMENT VERSION */
