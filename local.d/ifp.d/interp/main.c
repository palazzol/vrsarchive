
/****** main.c ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Jan 20, 1987          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

#include <stdio.h>
#include "struct.h"
#include "node.h"
#include "umax.h"
#include "cache.h"
#include "stats.h"

#if OPSYS!=CTSS
#endif

static char Version[] = "\nIllinois FP 0.5";
static char Author [] = " Arch D. Robison";
static char Date   [] = " Dec 5, 1986\n";

#if OPSYS==UNIX
#define OPSYSTEM "UNIX"
#endif
#if OPSYS==MSDOS
#define OPSYSTEM "MS-DOS"
#endif
#if OPSYS==CTSS
#define OPSYSTEM "CTSS"
#endif

boolean LongPathFlag = 0;

#ifdef COMPILE
boolean CompilerFlag = 0;       /* Enable compiler if set */
boolean RuleFlag = 0;           /* Display rules if set   */
#endif

private void Init ()
   {
      extern void D_arith (), D_form (), D_pred (), D_misc (), D_seq (), 
		  D_ss (), D_subseq (), D_string (), D_cray (), D_vector ();
      extern void InitString (), InitNode (), InitFile ();
      extern char RootPath[];                  /* from file.c */
#if OPSYS==MSDOS
      char CWD [64];
#endif
#if OPSYS==UNIX
      extern void EnvGet ();
#endif    
      if (Debug & DebugInit) printf ("enter Init\n");

      InitString ();
#if OPSYS==MSDOS
      CWDGet (CWD,MAXPATH);
#endif
#if OPSYS==UNIX
      EnvGet ("IFProot",RootPath,MAXPATH);      /* Check for RootPath */
#endif 
#if ECACHE
      InitCache ();
#endif

      InitNode ();
      D_arith ();
      D_form ();
      D_pred ();
      D_seq ();
      D_subseq ();
      D_misc ();
      D_ss ();
      D_string ();
#if OPSYS==MSDOS
      InitFile (CWD);
#endif
#if OPSYS==UNIX || OPSYS==CTSS
      InitFile ();
#endif
#ifdef COMPILE
      if (CompilerFlag) {
	 extern void InitSymTab (), InitCompiler ();
	 InitSymTab ();
	 InitCompiler ();
      }
#endif
#ifdef GRAPHICS
      InitDraw (); /* for CS9000 graphics only */
#endif
#if STATS
      printf (" (stats)");
#endif
      if (Debug & DebugInit) printf ("exit Init\n");
   }

extern void UserLoop ();

/*
 * GetOptions
 *
 * Process command line options.
 *
 * Input
 * 	argv = command line arguments
 *	argc = argument count
 */
private void GetOptions (argc,argv)
   int argc;
   char *argv[];
   {
      int k;
      char *P;

      for (k=1; k<argc; k++) 
	 if (*(P=argv[k]) == '-') 
	    while (*P && *++P)
	       switch (*P) {
#ifdef COMPILE
		  case 'c': CompilerFlag = 1; break;
		  case 'r': RuleFlag = 1; break;
#endif 
#if DEBUG
		  case 'd': 
		     while (*++P) {
		        extern char *index();
			static char Opt[] = DebugOpt;
		        char *t = index (Opt,*P);
			if (t != NULL) Debug |= 1 << (t-Opt);
			else printf ("[unknown option = -d%c] ",*P);
		     }
		     break; 
#endif /* DEBUG */
#if ECACHE
		  case 'e':
		     while (*++P)
			if (*P >= '0' && *P <= '2')
			   Cache[*P-'0'].Enable = 1;
			else
			   printf ("[unknown -e option = %c] ",*P);
		     break;
#endif /* ECACHE */
		  case 'l': LongPathFlag = 1; break;
		  default: 
		     printf ("[unknown option = %c] ",*P);
		     P = "";
		     break;
	       }
   }

main (argc, argv)
   int argc;
   char *argv[];
   {
      printf ("%s: (%s)",Version,OPSYSTEM);
      (void) fflush (stdout);
      GetOptions (argc,argv);
      Init ();
      printf ("\n\n");
      UserLoop ();
      Terminate();
      if (Debug & DebugInit) printf ("normal exit\n");
      exit (0);
   }

/************************** end of main.c **************************/

