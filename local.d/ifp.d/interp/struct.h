
/****** struct.h ******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.5                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date:   Aug 4, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/*
 * There are some preprocessor variables which must be defined either
 * here or in the cc command.  The following options are not available
 * in the public domain release:
 *
 * 	ARRAYS, COMPILE, UMAX, VECTOR, OUTBERKELEY
 *
 * Some of the code for these options are removed from the source by 
 * unifdef(1), so the source may look strange in places.  (E.g. degenerate 
 * switch statements).
 *
 * The preprocessor variables are listed below.
 *
 * OPSYS (UNIX, MSDOS, CTSS) - specifies operating system
 * PCAT - for compiling on PC/ATs
 * SQUEEZE - put space at a premium
 * DEBUG - incorporate interpreter debugging spy points
 * DUMP - incoporate dump command for debugging (see debug.c)
 * REFCHECK - incorporate reference checking command (see apply.c)
 * COMPILE - incorporate IFP compiler (see C_comp.h)
 * ARRAYS - incorporate array representation of sequences
 * VECTOR - define APL-style vector operations (must define ARRAYS also)
 * UMAX - make parallel version for Encore Multimax
 *
 * There are also preprocessor variables which may be turned on or off
 * in the following files:
 *
 *      ECACHE in cache.h - implement expression cache
 *      STATS in stats.h - collect run time statistics
 *      FETCH in node.h - implement "fetch" functional form
 *	OUTBERKELEY in outberkely.h - implement routine to print functions in
 *				      Berkeley FP format.
 *
 * WARNING: Some of the compiling options may interfere with each other.
 *          Some options have not been tested for many revisions, so
 *	    new bugs may creep out of the woodwork!
 */

#define UMAX 0		/* Must not enable ARRAYS, ECACHE, or STATS if set */
#define DUMP 0
#define ARRAYS 0	/* Must also define VECTOR=1 if set */
#define VECTOR 0
#define DEBUG 0
#define REFCHECK 0

/*
 * Possible values for OPSYS preprocessor variable.
 */
#define UNIX  10
#define MSDOS 11
#define CTSS  12

#define OPSYS UNIX

#if OPSYS==CTSS
/*
 * PARAMBUG is defined to indicate that the C compiler can not
 * take the address (&) of parameter variables correctly.
 * When this bug is removed from the CRAY C compiler, this define
 * and dependent code should be removed.
 */
#define PARAMBUG 1
#endif
 
#if OPSYS==MSDOS || OPSYS==CTSS 
#define MAXPATH 65     /* Maximum pathname length allowed (in characters) */
#endif

#if OPSYS==UNIX
#define MAXPATH 256    /* Maximum pathname length allowed (in characters) */
#endif

#if OPSYS==CTSS
#define index strchr
#endif

#ifdef PCAT
#define index strchr
#endif

/********** Fundamental Data Structures and Constants **********/


#define private static
#define forward extern  /* for forward definitions which are not external */
typedef int boolean;
typedef long FPint;
typedef int FPboolean;

typedef short ushort;

/********************** MACHINE DEPENDENT CONSTANTS **********************/

/* These two definitions assume two's complement arithmetic! */
#define FPMaxInt (((FPint) 1 << 8 * sizeof(FPint) - 1) - 1)
#define MaxInt   (((  int) 1 << 8 * sizeof(  int) - 1) - 1)

#ifdef SQUEEZE

/* Maximum floating point value representable by an FPfloat */
typedef float FPfloat;

#define MAXFLOAT 1e38
#define LNMAXFLOAT 88.7

#define CompTol (1e-6)

#else

typedef double FPfloat;

/* Maximum floating point value representable by an FPfloat */
#define MAXFLOAT 1.8e308
#define LNMAXFLOAT 710.37     /* ln (MAXFLOAT) */

#define CompTol (1e-8)

#endif

/* if abs(A),abs(B) are both < MAXFACTOR then A*B will fit in FPInt */
#define MAXFACTOR 0xB504L

/****************** end of machine dependent constants *********************/

/********************************* Strings *********************************/

/*
 * StrCell
 *
 * Each string is segmented into a linked list.  The first record of the
 * linked list contains the reference count for the string.
 * The string is terminated by a segment with a null StrNext field or
 * a '\0', whichever comes first.  The empty string is represented
 * by a null pointer.  Segments have '\0' as their first character iff
 * they are in the free string list.
 */

/*
 * StrHeadLen is the maximum number of characters which can be contained in
 * the first segment of a string list.  
 */
#if OPSYS==CTSS
#define StrHeadLen 8	/* For 64-bit ushort and 64-bit pointer */
#else 
#define StrHeadLen 10	/* For 16-bit ushort and 32-bit pointer */
#endif

#define StrTailLen (StrHeadLen + sizeof (ushort))

typedef struct StrCell {
   struct StrCell *StrNext;
   union {
      char StrVar1 [StrTailLen];
      struct {
	 char StrV1F1 [StrHeadLen];
	 ushort StrV1F2;
      } StrVar2;
   } StrUni1;
} StrCell;

typedef StrCell *StrPtr;

#define StrChar StrUni1.StrVar1
#define SRef StrUni1.StrVar2.StrV1F2

/****************************** Sequences ******************************/

/*
 * Sequences are guaranteed not to have cycles by the definition of FP.
 * Note that function representation lists may have a cycle, but the cycle
 * will always contain a function name as a member.  Cycle will be broken
 * when the function definition is deleted.
 */

/* Object Tags */
#define BOTTOM  0
#define BOOLEAN 1
#define INT     2
#define FLOAT   3
#define LIST    4
#define STRING  5
#define NODE    6
#define CODE    7
#define JOIN	8

/* Bitmasks for PairTest */
#define NUMERIC ((1<<FLOAT)|(1<<INT))
#define ATOMIC (NUMERIC | (1<<BOOLEAN) | (1<<STRING))


#define MAXTAG  7
#define SEQUENCE (1<<LIST)


/* Tag checking expressions dependent upon tag value assignments above */
#define Scalar(Tag) ((Tag) < 4)
#define Numeric(Tag) (((Tag)&~1)^2==0)
#define NotNumPair(Tag1,Tag2) ((((Tag1)^2)|((Tag2)^2))&~1) 
#define IntPair(Tag1,Tag2) ((Tag1+Tag2) == 4)
 
typedef struct CodeCell {
   int (*CodePtr) ();           /* (*CodePtr) (InOut,CodeParam) */
   int CodeParam;
} CodeCell;

typedef union {
   FPfloat _Float;
   FPint _Int;
   FPboolean _Bool;
   struct ListCell *_List;
   StrPtr _String;
   struct NodeDesc *_Node;
   CodeCell _Code;
} ObUnion;              

#define Float Data._Float
#define Int Data._Int
#define Bool Data._Bool
#define List Data._List
#define String Data._String
#define Node Data._Node
#define Code Data._Code

/*
 * Note on ARRAYS structures.  Cells with the ARRAY tag use the List field
 * to point to an array descriptor list.  The first element of the list
 * uses the APtr field, subsequent elements use the ADim field.
 */

/*
 * Object 
 *
 * An Object is a union which stores an IFP object.  The _LRef field is not 
 * logically part of an * object, but rather part of a ListCell.  We get much 
 * better packing by including it in Object, since it fits in a 32-bit word 
 * along with the Tag field.
 *
 * Likewise, for the UMAX version the _LRefLock field is physically part
 * of Object though it should be part of ListCell.
 *
 * Note that P->Val = Q->Val will transfer the reference count!
 */  
typedef struct {
   ObUnion Data;
   ushort _LRef;
   char Tag;   /* BOTTOM,BOOLEAN,INT,FLOAT,LIST,STRING,NODE,CODE,ARRAY */
} Object;

/*
 * ListCell
 *
 * Sequences are represented as linked lists of objects.  Each ListCell
 * also contains a reference count (hidden in the Object field).  The
 * value stored in the reference count is offset by -1.  The rationale is
 * that reference counts are always compared against one, and comparing
 * against zero is faster on some machines.  
 */ 
typedef struct ListCell {
   Object Val;			/* Value of first element of sequence (CAR) */
   struct ListCell *Next;	/* Pointer tail of sequence (CDR) 	    */
} ListCell;

#define LRef Val._LRef
#define LRefOne 0       	/* value of LRef for reference count of 1 */


/*
 * Most of the code uses subsets of the alphabet for certain types.
 * For example, P,Q, and R are usually ListPtr.
 */
typedef ListCell *ListPtr;	/* e.g. P,Q,R */
typedef ListPtr *MetaPtr;	/* e.g. A,B,C */
typedef Object *ObjectPtr;	/* e.g. X,Y,Z */

#define NIL ((ListPtr) NULL)	/* empty list */

/******************************* Definitions ******************************/

/*
 * DefDesc
 *
 * DefFlags = subset of {TRACE,RESOLVED}.
 * DefCode = code for definition - BOTTOM if not resident.
 */
typedef struct DefDesc {
   char DefFlags;
   Object DefCode;
} DefDesc;

typedef DefDesc *DefPtr;

#define TRACE  1       /* Print input and output.                      */
#define RESOLVED 4     /* Mark bit used by reference checker           */

/*
 * All compiled FP functions have the following form:
 *
 *   void F (InOut,CodeParam)
 *      ObjectPtr InOut;
 *      int CodeParam;
 *      {...};
 *
 * F replaces *InOut with the result of applying F to *InOut.
 * CodeParam  is optional.
 */


/******************************* Modules *******************************/

/*
 * Modules are stored as lists of nodes.  Each node has a pointer to
 * its next sibling and its parent node.
 */
typedef struct {                /* Module node descriptor */
   struct NodeDesc *FirstChild;
} ModDesc;

/******************************** Imports ******************************/

/*
 * Definition nodes are imported with IMPORT nodes.  An import node in a
 * module points to a definition node elsewhere.
 */
typedef struct {
   Object ImpDef;     /* Can be path list or node */
} ImpDesc;

/******************************** Nodes ********************************/

#define NEWNODE 0 /* Values for NodeType */
#define MODULE 1
#define DEF 2
#define IMPORT 3

/*
 * NodeDesc
 *
 * See the top of node.c for the description of how these are linked together
 * to form the function/module tree.
 *
 * NRef = reference count (references by objects)
 * NodeNext = pointer to next sibling (or parent).
 * NodeType = type of node (DEF, MODULE, IMPORT)
 * NodeName = print name of node.
 */
typedef union {
   DefDesc NodeDef;     /* if DEF    */
   ModDesc NodeMod;     /* if MODULE */
   ImpDesc NodeImp;     /* if IMPORT */
} NDunion;

typedef struct NodeDesc {
   struct NodeDesc *NodeSib;
   struct NodeDesc *NodeParent;
   StrPtr NodeName;
   short NRef;
   char NodeType;
   NDunion NodeData;
} NodeDesc;

typedef struct NodeDesc *NodePtr;

/*----------------- exception handling: see except.c -----------------*/

/* values for SysError, 0 == no error */

#define INTERNAL     1     /* Inexplicable internal error  */
#define NO_LIST_FREE 2	   /* Ran out of list cell storage */
#define NO_STR_FREE  3	   /*  "   "  " string  "     "    */
#define NO_NODE_FREE 4	   /*  "   "  "  node   "      "   */

extern short SysError;     /* An error occurred if SysError != 0 */
extern short SysStop;      /* Stop evaluation if != 0            */

/*------------ debugging the interpreter: see debug.c ----------------*/

/*
 * The interpreter may be compiled with internal spy points.  These spy 
 * points print internal information on stdout.  To include the spy * points, 
 * the interpreter must be compiled with #define DEBUG 1.  To turn on a spy 
 * point when running ifp, use the command line option '-d' followed by the 
 * appropriate letters.  The letters are defined by ``DebugOpt'' below.  
 * For example,
 *
 *	ifp -dar
 *
 * will turn on spy points related to memory allocation (a) and 
 * reference counts (r).
 */
#define DebugParse   	(1<<0)	/* parser		*/
#define DebugAlloc 	(1<<1)	/* memory allocation 	*/
#define DebugFile	(1<<2)	/* file io 		*/
#define DebugRef	(1<<3)	/* reference counts 	*/
#define DebugInit	(1<<4)	/* initialization	*/
#define DebugCache	(1<<5)	/* expression cache	*/
#define DebugXDef	(1<<6)	/* extended definitions */
#define DebugHyper	(1<<7)	/* hypercube		*/
#define DebugUMax	(1<<8)  /* multimax		*/
#define DebugSemaphore  (1<<9)  /* semaphores		*/
#define DebugFreeList   (1<<10) /* free list		*/
#define DebugExpQueue   (1<<11) /* expression queue	*/

#define DebugOpt "pafricxhusle"	/* option letters for above */

#if DEBUG
extern int Debug;	/* Bit-set of enabled spy points */
#else
#define Debug 0		/* Turn spy points into dead code */
#endif

/*--------------------------------------------------------------------*/

extern NodePtr CurWorkDir; 	/* Current working directory */
extern NodePtr SysDef ();

extern void DelLPtr ();         /* Delete a list pointer */
extern ListPtr CopyLPtr ();     /* Copy a list pointer */

extern void Rot3 ();            /* list pointer rotation */

extern long ListLength ();              /* from list.c */
extern void CopyObject ();
extern ListPtr Repeat ();
extern void NewList ();
extern void RepTag ();
extern boolean RepObject ();
extern void RepLPtr ();
extern void CopyTop ();
extern void Copy2Top ();
extern void RepBool ();

extern void Apply ();                   /* from apply.c */
extern NodePtr ApplyFun;

extern void NodeExpand ();

extern void ExecEdit (), ReadImport (); /* from file.c */

extern void OutObject (), OutList ();   /* from outob.c */
extern void OutString (), OutNode ();
extern void OutForm (), OutFun ();      /* from outfun.c */
extern void OutPretty ();

extern void InitIn (), InBlanks ();	/* from inob.c */

extern void ReadDef (), DelImport ();
extern void InImport (); 

extern int InError();			/* from error.c */
extern void DefError (), IntError ();
extern void FunError (), FormError ();
extern char ArgNotSeq[], ArgObSeq[], ArgSeqOb[], ArgNull[], ArgBottom[];

extern NodePtr PrimDef ();
extern char *malloc();

#define ArrayEnd(A) (A+(sizeof(A)/sizeof A[0])) 


/************************** end of struct.h **************************/
