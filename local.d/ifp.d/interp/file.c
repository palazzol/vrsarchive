
/****** file.c ********************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date: June 22, 1986          **/
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
#include "string.h"
#include "node.h"
#include "umax.h"
#include "inob.h"

extern char *strcpy (),*strcat (),*getenv ();

/*------------------------- Operating System Constants --------------------*
 *
 * Operating System Constants
 *
 * PATH_SEPARATOR = separator used for file names
 * EDITOR = path to editor
 * EdCommand = IFP command to invoke EDITOR
 *
 * Even operating systems without hierarchical file systems must
 * define a PATH_SEPARATOR, which is used still used internally.
 *
 * In the case of CTSS, we also have a constant USER_PATH.  This
 * a fictious path to the user's current directory. 
 */ 

#if OPSYS==UNIX
#define PATH_SEPARATOR '/'
#define EDITOR "/bin/vi"
char *EdCommand;
#endif

#if OPSYS==MSDOS
#define PATH_SEPARATOR '\\'
#define EDITOR "c:ed.exe"
char *EdCommand = "ed";
char DirPath [MAXPATH+1] = "c:dir.exe";
#endif

#if OPSYS==CTSS
#define PATH_SEPARATOR '/'
#define EDITOR "fred"
char *EdCommand = "fred";
#define USER_PATH "/usr"
#endif

/*-------------------- end of Operating System Constants --------------------*/

char RootPath [MAXPATH+1] = "";         /* Path to IFP's root */

char EditorPath [MAXPATH+1] = EDITOR;   /* value is default */

char FPprompt[16] = "ifp> ";   /* value is default */
NodePtr CurWorkDir = NULL;     /* Current working directory node */

/********************** Operating system file interface ***********************/

/*
 * PathTail
 *
 * Return the last component in a path name.
 * Returns pointer to "" if error occurs.
 */
private char *PathTail (Path)
   char *Path;
   {
      register char *T;

      if (*Path == PATH_SEPARATOR) Path++;
      while (*Path) {
	 for (T = Path; *T++ != PATH_SEPARATOR; )
	    if (!*T) return Path;
	 Path = T;
      }
      return Path;
   }

/*------------------------------ fopen hacks ------------------------------
 *
 * Both MSDOS and CTSS have problems with hierarchical file structure.
 * Thus we have to redefine the standard UNIX function "fopen" to allow
 * for these operating system's quirks.
 */
#if OPSYS==MSDOS

/*
 * We try to keep track of the current directory in CWDCache so we can avoid
 * superfluous calls to chdir.  Set the 0th character to '\0' to empty the
 * cache.
 */
char CWDCache [MAXPATH] = "";

/*
 * PathSplit
 *
 * Split a pathname into its directory and file parts.
 * Change directory to directory part.
 *
 * Input
 *      PathName = pathname
 *
 * Output
 *      NULL if error occurs, pointer to file name otherwise.
 */
char *PathSplit (PathName)      /* also used by command.c */
   char *PathName;
   {
      register char *S,*T;
      register int R;

      T = PathTail (PathName);
      if (T != &PathName [1]) {
	 T[-1] = '\0';
	 S = PathName;          /* Break string at path separator */
      } else S = "\\";
      if (strcmp (S,CWDCache)) {
	 R = chdir (S);
	 if (Debug & DebugFile) printf ("cache load: %d = ChDir (%s) for %s\n",R,S,T);
	 (void) strcpy (CWDCache,S);
      } else {
	 if (Debug & DebugFile) printf ("cache HIT!\n"); 
	 R = 0;
      }
      T[-1] = PATH_SEPARATOR;   /* Replace path separator */
      return R ? NULL : T;
   }

/*
 * DOSfopen
 *
 * Works as =`fopen' should.  The old Lattice C `fopen' would not take 
 * pathnames.  Even though the new compiler's will allow long names,
 * the fake fopen can take advantage of the current directory cache.
 */
private FILE *DOSfopen (FileName,Mode)
   char *FileName,*Mode;
   {
      char *T;

      return (T = PathSplit (FileName)) != NULL ? fopen (T,Mode) : NULL;
   }

#define fopen DOSfopen

#endif /* OPSYS==MSDOS */

#if OPSYS==CTSS
/*
 * CTSSfopen
 *
 * Tries for fake a real fopen.  CTSS does not support hierarchical file
 * structures, so CTSSfopen takes the tail of the path as the file name.
 */
private FILE *CTSSfopen (FileName,Mode)
   char *FileName,*Mode;
   {
      register char *T;

      if (Debug & DebugFile) printf ("CTSSfopen (%s,%s)\n",FileName,Mode);
      T = PathTail (FileName);
      if (T == &FileName[1]) return NULL;
      else {
	 T[-1] = '\0';
	 if (strcmp (FileName,USER_PATH)) return NULL;
	 else {
	    if (Debug & DebugFile) printf ("fopen (%s,%s)\n",T,Mode);
	    return fopen (T,Mode);
	 }
      }
   }
#define fopen CTSSfopen

#endif /* OPSYS==CTSS */

/*---------------------------- end of fopen hacks ----------------------------*/

/*
 * FormNPath
 *
 * Create the pathname for a given node.
 *
 * Input
 *      N = pointer to node
 *      PathLim = pointer to end of PathName buffer
 *
 * Output
 *      Pathname for node
 */
char *FormNPath (N,PathName,PathLim)
   register NodePtr N;
   char PathName[];
   register char *PathLim;
   {
      CharPtr U;
      register char *T;

      if (N->NodeParent == NULL) {
	 (void) strcpy (PathName,RootPath);
	 return &PathName [strlen (PathName)];
      } else {
	 T = FormNPath (N->NodeParent,PathName,PathLim);
	 if (T==NULL) return NULL;
	 else {
	    *T++ = PATH_SEPARATOR;
	    CPInit (&U,&N->NodeName);
	    (void) CPRead (&U,T,PathLim-T);
#if OPSYS==UNIX
	    T += strlen (T);
#endif
#if OPSYS==MSDOS || OPSYS==CTSS 
	    /* DOS and CTSS names limited to 8 characters */
	    {
	       int L;               
	       if ((L = strlen (T)) > 8) L = 8;
	       *(T += L) = '\0';
	    }
#endif
	    if (!CPRead (&U,T,PathLim-T)) return T;
	    else return NULL; /* U should be empty */
	 }
      }
   }


/*
 * FormPath
 *
 * Make UNIX or DOS pathname for node
 *
 * Input
 *      N = node or path list
 *      PathName = buffer to put pathname in.
 *      PathLim = pointer to end of buffer
 *
 * Output
 *      PathName = pathname if successful
 */
void FormPath (N,PathName,PathLim)
   register ObjectPtr N;
   char PathName[];
   char *PathLim;
   {
      register char *T;
      CharPtr U;
      register ListPtr P;
      register int K;

      switch (N->Tag) {

	 case LIST:
	    (void) strcpy (PathName,RootPath);
	    K = strlen (PathName);
	    PathLim -= K;
	    T = &PathName [K];
	    for (P = N->List; P!=NULL; P=P->Next) {
	       if (P->Val.Tag != STRING) return;
	       else {
		  CPInit (&U,&P->Val.String);
		  (void) CPRead (&U,T,PathLim-PathName);
#if OPSYS==UNIX
		  T += strlen (T);
#endif
#if OPSYS==MSDOS || OPSYS==CTSS 
		  /* DOS names are limited to 8 characters */
		  if ((K = strlen (T)) > 8) K = 8; 
		  *(T += K) = '\0';
#endif
		  /* T should always be <= PathLim */
		  if (T >= PathLim) return;
	       }
	    }
	    break;

	 case NODE:
	    (void) FormNPath (N->Node,PathName,PathLim);
	    break;

	 default:
	    break;
      }
   }

/*
 * ReadDef
 *
 * Read a definition node.  The definition node tag must be BOTTOM upon entry
 * when running UMAX.
 *
 * Input
 *      Caller = pointer to DEF node of caller
 *      Fun = object with tag NODE.  
 */
void ReadDef (Caller,Fun)
   NodePtr Caller;
   ObjectPtr Fun;
   {
      NodePtr N;
      char FileName[MAXPATH];
      FILE *DefFile;
      InDesc F;
      int C;


      if (NULL == FormNPath (N=Fun->Node,FileName,&FileName[MAXPATH])) 
	 DefError (Caller,Fun,"invalid name for function");
      else 
	 while (NULL != (DefFile = fopen (FileName,"r"))) {
	    InitIn (&F,N->NodeParent,DefFile,0);
	    C = InDef (&F,N->NodeName,&N->NodeData.NodeDef.DefCode);
	    (void) fclose (F.InFile);
	    if (C) goto exit;
	    printf ("Do you wish to edit %s ? ",FileName + strlen (RootPath));
	    while (1) {
	       for (C = getchar (); getchar ()!='\n';) continue;
	       if (C == 'y') {
		  ExecEdit (FileName); 
		  break;
	       }
	       if (C == 'n') goto exit;
	       printf ("Respond with y or n\n");
	    }
	 } 
exit:;
   }

/*
 * ReadImport
 *
 * Read the import file for a module node.
 *
 * Input
 *      M = pointer to module node
 */
void ReadImport (M)
   NodePtr M;
   {
      char *T;
      char FileName[MAXPATH];
      FILE *ImpFile;
      InDesc F;

      if (NULL != (T = FormNPath (M,FileName,&FileName[MAXPATH]))) {
	 *T++ = PATH_SEPARATOR;
	 (void) strcpy (T,"%IMPORT");
	 if (NULL != (ImpFile = fopen (FileName,"r"))) {
	    InitIn (&F,M,ImpFile,0);
	    InImport (&F,M);
	    (void) fclose (ImpFile);
	 }
      }
   }

#if OPSYS!=CTSS
/*
 * EnvGet
 *
 * Get value for environment variable.
 *
 * Input
 *      Key = enviroment variable name
 *      Value = default value for variable
 *      ValLim = length of Value buffer
 *
 * Output
 *      Value = value of enviroment variable, or default if not found.
 */
void EnvGet (Key,Value,ValLim)
   char *Key,*Value;
   int ValLim;
   {
      char *V;

      V = getenv (Key);
      if (V != NULL)
	 if (strlen (V) < ValLim) (void) strcpy (Value,V);
	 else fprintf (stderr,"Error: %s in enviroment is longer than %d\n",
		       Key,MAXPATH-3);
   }


/*
 * CWDGet
 *
 * Find pathname of current working directory (relative to FP root).
 *
 * Input
 *      PathLim = length of Path buffer (used by PCAT versions only)
 *
 * Output
 *      result = 1 if valid FP path, 0 otherwise
 *      Path = FP pathname if valid, undefined otherwise
 */
boolean CWDGet (Path,PathLim)
   register char *Path;
   int PathLim;
   {
#ifdef PCAT
      extern char *getcwd ();
      if (!getcwd (Path,PathLim-2)) return 0;
#else
#if S9000
      extern FILE *popen ();
      FILE *F;                          /* S9000 Xenix has no getwd! */
      F = popen ("/bin/pwd","r");
      fscanf (F,"%s",Path);
      pclose (F);
#else
      extern char *getwd();
      if (!getwd (Path)) return 0;
#endif /* S9000 */
#endif /* PCAT */

#if OPSYS==MSDOS
      (void) strcpy (Path,Path+2);              /* Delete drive name */
      if (Debug & DebugFile) printf ("CWD = '%s'\n",Path); 
      return 1;
#endif
#if OPSYS==UNIX
      {
	 register int K;
	 K = strlen (RootPath);
	 if (strncmp (Path,RootPath,K)) return 0;
	 else {
	    (void) strcpy (Path,&Path[K]);    /* Remove FP root path prefix */
	    return 1;
	 }
      }
#endif
   }
#endif /* OPSYS != CTSS */

/*
 * InitFile
 *
 * The DOS version is kludgy.  The problem is that DOSfopen changes 
 * the current directory, thus munging it before CWDGet is called.
 */
#if OPSYS==UNIX || OPSYS==CTSS
void InitFile ()
#endif
#if OPSYS==MSDOS
void InitFile (CWD)
   char *CWD;
#endif
   {
      Object X;
      InDesc F;

      if (Debug & DebugFile) printf ("enter InitFile\n");
#if OPSYS!=CTSS
      EnvGet ("EDITOR",EditorPath,MAXPATH);
#endif
      if (Debug & DebugFile) printf ("EditorPath = `%s'\n",EditorPath);
#if OPSYS==UNIX
      EdCommand = PathTail (EditorPath);
      if (!*EdCommand) {
	 fprintf (stderr,"\n * EDITOR environment variable not a full path.");
	 fprintf (stderr,"\n   Setting editor to '%s'.\n",EDITOR);
	 EdCommand = PathTail (strcpy (EditorPath,EDITOR));
      }
      EnvGet ("IFPprompt",FPprompt,sizeof (FPprompt));
#endif
#if OPSYS==MSDOS
      EnvGet ("IFPDIR",DirPath,MAXPATH);
      if (Debug & DebugFile) printf ("IFPDIR = '%s'\n",DirPath);
#endif

      /* Create dummy descriptor for scanning environment info */
      InitIn (&F,(NodePtr) NULL,(FILE *) NULL, -1); 

#if OPSYS==UNIX
      if (!CWDGet (F.InBuf,INBUFSIZE-1)) {
	 fprintf (stderr,"\n\n * Current directory not a IFP subdirectory.");
	 fprintf (stderr,  "\n   Setting current directory to IFP root.\n");
	 if (chdir (RootPath)) {
	    extern int errno;
	    perror (RootPath);
	    exit (errno);
	 } else F.InBuf[0] = '\0';
      }
#endif
#if OPSYS==MSDOS
      {
	 register char *T;
	 (void) strcpy (F.InBuf,CWD);
	 for (T=F.InBuf; *T; T++)
	    if (*T == PATH_SEPARATOR) *T = '/';
      }
#endif
#if OPSYS==CTSS
      (void) strcpy (F.InBuf,USER_PATH);
#endif
      if (F.InBuf[0]) {
	 (void) strcat (F.InPtr,"\n");
	 (void) InNode (&F,&X,NIL);
	 CurWorkDir = MakeNode (X.List,1);
      } else 
	 CurWorkDir = RootNode;
      if (Debug & DebugFile) printf ("exit InitFile\n");
   }


/************************* end of file.c *******************************/

