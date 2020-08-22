/*
 * This file is for use with compilers that don't have the capability to #define
 * symbols at compile-time (e.g. on the command line).  This file must be
 * #include'd before all other ck*.h files so that the symbols #define'd here
 * can be used for any subsequent conditional code.  Symbols should be #define'd
 * as 1 if TRUE and 0 if FALSE.  This implies, of course, that they be tested
 * with #if's, not #ifdef's and #ifndef's. 
 * 
 * This file is used by both C-Kermit and the Macintosh keyboard configuration
 * utility.  As such, it should not contain anything which is specific to either
 * program (e.g. extern's, typedef's, etc.), only the #define's necessary to
 * select the proper conditional code. 
 */ 

/*
 * The following #define is for compilation of the Macintosh version of C-Kermit
 * with the Megamax compiler. 
 */

#define MEGAMAX 0

#if MEGAMAX
#define MAC		/* Also #define MAC symbol for generic Macintosh code */
#endif

