
/****** cache.h *******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP                         Version: 0.1                 **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison       Date: July 29, 1986          **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

#define ECACHE 0	/* Implement expression cache if defined */

#if ECACHE

/*
 * The expression cache can be turned on selectively for expressions with
 * primitive functions, user-defined functions, or PFOs.
 *
 * Cache[i].Enable = 0/1 to turn off/on cache for expression type i in [0..2]. 
 */
#define CachePrim  0
#define CacheUser  1
#define CachePFO   2
#define CacheTotal 3

typedef struct {
   boolean Enable;
   int Looks;		/* Number of looks into cache */
   int Hits;		/* Number of successful looks */
   int Evictions;	/* Number of evictions	      */
   char *Name;		/* "Prim", "User", "PFO", etc.*/	
} CacheRec;

extern CacheRec Cache[];

#if DEBUG
extern void PrintCache ();
#endif

/*
 * The expression cache is implemented as a hash table.  It
 * associates outputs with <input,function> pairs.
 */

#define CACHE_SIZE 1024 /* Must be power of 2 */

/*
 * EC_Fun.Tag = BOTTOM iff that cache entry is empty
 */
typedef struct {
   Object EC_In, EC_Out;
   NodePtr EC_Fun;
} CacheEntry;

extern CacheEntry ECache[];
extern int HashOb ();
extern void ShowCache (); /* Show cache statistics                */

/*
 * CheckCache
 *
 * Parameter
 *      T = &Cache[i] where i is type of function to be cached.
 *	A = call to "apply" with appropriate arguments.
 */
#define CheckCache(T,A)							\
   if ((T)->Enable) {							\
      CacheEntry *C;							\
      extern int TraceDepth;						\
									\
      (T)->Looks++;							\
      C = &ECache [(HashOb(InOut) + (long) F->Node) * 0x9B & CACHE_SIZE-1]; \
      if (ApplyFun == C->EC_Fun && ObEqual (InOut,&C->EC_In)) {		\
	 if (Debug & DebugCache) PrintCache ("Hit!",C);			\
	 (T)->Hits++;							\
	 if (Trace|SaveTrace) printf ("IBID\n");			\
	 RepObject (InOut,&C->EC_Out);					\
      } else {								\
	 if (C->EC_Fun != NULL) {					\
	    (T)->Evictions++;						\
	    if (Debug & DebugCache) PrintCache ("Evict",C);		\
	 }								\
	 C->EC_Fun = NULL;						\
	 RepObject (&C->EC_In,InOut);					\
	 {A;}								\
	 C->EC_Fun = F->Node;						\
	 RepObject (&C->EC_Out,InOut);					\
	 if (Debug & DebugCache) PrintCache ("Load",C);			\
      }									\
   } else {A;}

#else

#define CheckCache(T,A) {A;}
#define ClearCache()

#endif

/***************************** end of cache.h ****************************/

