
/****** command.c *****************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:  Jan 28, 1987          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/*************************** Command Interpreter **************************/


#include <stdio.h>
#include <errno.h>
#include "struct.h"
#include "node.h"
#include "umax.h"
#include "inob.h"
#include "cache.h"
#include "stats.h"

#if OPSYS==UNIX
#include <strings.h>
#include <sys/wait.h>
#endif

#if OPSYS==MSDOS
#include "/usr/include/dos/spawn.h"     /* Full name so lint can find it */
#include "/usr/include/dos/string.h"
#endif

extern char EditorPath [],*EdCommand;
extern char *getenv ();

extern boolean RefCheck ();        /* from apply.c */

#if OPSYS==UNIX
extern fork (),execl ();
#endif

InDesc UserIn;

/*
 * ReadNode
 */
private NodePtr ReadNode (U)
   InDesc *U;
   {
      Object S;

      if (!InNode (U,&S,NIL)) return NULL;
      LinkPath (&S,DEF);
      if (S.Tag == NODE) return S.Node;
      else {
	 printf ("Error: ");
	 OutString (S.String);
	 printf (" not defined\n");
	 return NULL;
      }
   }

#if REFCHECK
/*
 * ShowRefCheck
 */
void ShowRefCheck ()
   {
      Object F;
      register InDesc *U;

      U = &UserIn;
      F.Tag = BOTTOM;

      (void) InComp (U,&F,NIL);
      (void) RefCheck ((NodePtr) NULL,&F);
      RepTag (&F,BOTTOM);
   }
#endif


/*
 * ShowApply
 */
private void ShowApply (OutGraph)
   int OutGraph;
   {
      Object X,F;
      register InDesc *U;

      U = &UserIn;
      X.Tag = BOTTOM;
      F.Tag = BOTTOM;
      if (InObject (U,&X)) {

	 if (!IsTok (U,":")) (void) InError (U,"colon expected");
	 else {
	    (void) InComp (U,&F,NIL);
	    if (Debug & DebugFile) {
	       printf ("Object = "); OutObject (&X); printf ("\n");
	       printf ("Function = "); OutFun (&F,MaxInt); printf ("\n");
	    }

	    if (*U->InPtr) (void) InError (U,"extra character on line");
	    else {
	       U->InPtr++;
	          ClearCache ();
	          Apply (&X,&F);
#ifdef GRAPHICS
	          if (OutGraph) DrawObject (&X);
	          else OutPretty (&X,0);
#else
	          OutPretty (&X,0);
	          printf ("\n");
#endif
	    }
	 }
      }
      RepTag (&X,BOTTOM);
      RepTag (&F,BOTTOM);
   }

/*
 * ExecFile
 *
 * Execute a file
 *
 * Input
 *      Prog = program to be executed
 *      Arg  = argument string
 */
void ExecFile (Prog,Arg)
   char *Prog,*Arg;
   {
      if (Debug & DebugFile) printf ("ExecFile (%s,%s)\n",Prog,Arg);
#if OPSYS==UNIX
      if (fork ()) (void) wait ((union wait *)NULL);
      else {
	 if (Debug & DebugFile) printf ("prepare to flush\n");
	 (void) fflush (stdout);
	 execl (Prog,Prog,Arg,(char *)NULL);
	 perror (Prog);
	 exit (1);
      }
#endif
#if OPSYS==MSDOS
      if (spawnl (P_WAIT,Prog,Prog,Arg,(char *)NULL)) perror (Prog);
#endif
   }

void ExecEdit (FileName)
   char *FileName;
   {
      if (Debug & DebugFile) printf ("ExecEdit (%s)\n",FileName);
#if OPSYS==UNIX
      ExecFile (EditorPath,FileName);
#endif
#if OPSYS==MSDOS
      {
	 extern char *PathSplit ();
	 char *T;
	 T = PathSplit (FileName);
	 if (T != NULL) ExecFile (EditorPath,T);
      }
#endif
   }

/*
 * EditRm
 *
 * Action depends on ``Edit'' flag:
 *
 * Edit
 *     Apply the user's editor to a function or import file.  If a function,
 *     delete the function definition from memory. If %IMPORT file, reread it.
 *
 * !Edit
 *     Remove a function definition or %IMPORT file.
 */
private void EditRm (U,Edit)
   register InDesc *U;
   boolean Edit;
   {
      Object N;
      char Buf[MAXPATH+1];
      static char *Import = "%IMPORT";
   
      if (Debug & DebugFile) printf ("EditRm (%s,%d)\n",U->InPtr,Edit);

      if (IsTok (U,Import)) {

	 if (Edit) ExecFile (EditorPath,Import);
	 else 
	    if (unlink (Import)) perror (Import);
	 DelImport (U->InDefMod);
	 ReadImport (U->InDefMod);

      } else {

	 N.Tag = BOTTOM;
	 (void) InNode (U,&N,NIL);
	 LinkPath (&N,DEF);

	 /* Kill old source code definition */
	 if (N.Tag == NODE)
	    switch (N.Node->NodeType) {
	       case DEF:
		  RepTag (&N.Node->NodeData.NodeDef.DefCode,BOTTOM);
		  break;
	       case MODULE:
		  break;
	    }

	 FormPath (&N,Buf,&Buf[MAXPATH]);
	 RepTag (&N,BOTTOM);
	 if (Edit) ExecEdit (Buf);
	 else
	    if (unlink (Buf)) perror (Buf);
      }
   }

#if OPSYS==UNIX
/*
 * Shell
 *
 * Execute a shell command
 */
void Shell (U)
   register InDesc *U;
   {
      if (Debug & DebugFile) printf ("Shell: '%s'\n",U->InPtr);
      if (fork ()) (void) wait ((union wait *)NULL);
      else {
	 (void) fflush (stdout);
	 execl ("/bin/sh","sh","-c",U->InPtr,(char *)NULL);
      }
   }
#endif
#if OPSYS==MSDOS
/*
 * ChDirToCWD
 *
 * Set DOS current working directory to IFP current working directory.
 *
 * This procedure is a necessary KLUDGE because the current directory
 * cache mechanism changes the current working directory all over the place.
 */
void ChDirToCWD ()
   {
      char Buf[MAXPATH];
      extern char *FormNPath ();

      (void) FormPath (CurWorkDir,Buf,&Buf[MAXPATH]);
      chdir (Buf);
   }

/*
 * Directory
 *
 * Show the current directory
 */
void Directory (U)
   register InDesc *U;
   {
      extern char DirPath[];

      ChDirToCWD ();
      ExecFile (DirPath,U->InPtr);
   }
#endif

/*
 * SetDepth
 *
 * Set function printing depth used for printing.
 */
SetDepth (U)
   register InDesc *U;
   {
      Object X;
      FPint N;
      extern int TraceDepth;

      X.Tag = BOTTOM;
      (void) InObject (U,&X);
      if (GetFPInt (&X,&N) || N < 0 || N > MaxInt)
	 printf ("Error: depth must be integer in range 0..%d\n",MaxInt);
      else TraceDepth = N;
   }


/*
 * SetTrace
 *
 * Set or reset function trace flags.
 */
private void SetTrace (U)
   register InDesc *U;
   {
      NodePtr N;
      int T;       /* phone home */

      if (IsTok (U,"on")) T=1;
      else if (IsTok (U,"off")) T=0;
      else {
	 printf ("trace [on|off] f1 f2 f3 ... \n");
	 return;
      }
      while (*U->InPtr) {
	 N = ReadNode (U);
	 if (N != NULL) {
	    if (T) N->NodeData.NodeDef.DefFlags |= TRACE;
	    else   N->NodeData.NodeDef.DefFlags &= ~TRACE;
	 } else break;
      }
   }

#if DUMP
extern void DumpNode();
#endif

void UserLoop ()
   {
      register InDesc *U;
      int N;

      U = &UserIn;
      while (1) {
	 extern char FPprompt [], *gets();
	 extern void ResetExcept();
#if OPSYS==MSDOS
	 extern char CWDCache [];
	 CWDCache [0] = '\0';		/* Clear current directory cache */
#endif
	 ResetExcept ();
	 if (Debug & DebugAlloc) {
	    extern ListPtr FreeList;
	    printf ("length (FreeList) = %ld\n",ListLength (FreeList));
	 }
	 printf ("%s",FPprompt);
	 (void) fflush (stdout);
	 InitIn (U,CurWorkDir,stdin,-1);

	 /* Copy prompt so that error message '^' will point correctly. */
	 U->InPtr += N = strlen (strcpy (U->InPtr,FPprompt));
	 (void) fgets (U->InPtr, INBUFSIZE-N, stdin);

	 if (!*U->InPtr || IsTok (U,"exit")) {
#if OPSYS==MSDOS
	    ChDirToCWD ();
#endif
	    return;
	 }
	 else if (IsTok (U,"depth")) SetDepth (U);
	 else if (IsTok (U,"show")) ShowApply (0);
#if HYPERCUBE
	 else if (IsTok (U,"send")) {
	    Object X;
	    ForkFP ();
	    InObject (U,&X); 
	    OutBinObject (&X);
	 }
#endif
#if COMPILE
	 else if (CompilerFlag && IsTok (U,"c")) Compile (U);
#endif
#if REFCHECK
	 else if (IsTok (U,"check")) ShowRefCheck ();
#endif
#if ECACHE
	 else if (IsTok (U,"cache")) ShowCache ();
#endif
#if STATS
	 else if (IsTok (U,"stats")) ShowStats ();
#endif
	 else if (IsTok (U,"trace")) SetTrace (U);
	 else if (IsTok (U,EdCommand)) EditRm (U,1);
#if DUMP
	 else if (IsTok (U,"dump")) DumpNode (CurWorkDir,0);
#endif
#ifdef GRAPHICS
	 else if (IsTok (U,"graph")) ShowApply (1);
#endif
	 /* else if (IsTok (U,"test")) Test (U); */
#if OPSYS==UNIX
	 else if (IsTok (U,"rm")) EditRm (U,0);
	 else Shell (U);
#endif
#if OPSYS==MSDOS
	 else if (IsTok (U,"del")) EditRm (U,0);
	 else if (IsTok (U,"dir")) Directory (U);
#endif
#if OPSYS==MSDOS || OPSYS==CTSS
	 else printf ("Unknown command: %s\n",U->InPtr);
#endif
      }
   }


/************************** end of command.c **************************/
