/*******************************************************\
* 						        *
* 	PCB program				        *
* 						        *
* 	Lee Router section  (common data structures)    *
* 						        *
* 	(c) 1985		A. Nowatzyk	        *
* 						        *
\*******************************************************/

#define maxrtl 10000		/* initial max ray table length	 */
#define sftmrg 2		/* safty margin (lazy check)	 */
#define maxvht 3000		/* length of via hole table	 */

#define MSK1  0x0f		/* nibble mask (LOB, side 1)	 */
#define MSK2  0xf0		/* nibble mask (HOB, side 2)	 */
#define VALID 0			/* valid square			 */
#define NOGOD 1			/* forbidden bit		 */
#define TERM  2			/* terminal square		 */
#define VTERM 3			/* terminal square on other side */
#define HOLE  4			/* hole bit (may go with 0,2,3)	 */
#define MSTART 5		/* start of maze		 */
#define HDT   8			/* home direction token		 */

/**** warning:  bit-defines are hardwired into several tables ****/

#define slahb 1			/* selected (any-) hole bit	 */
#define ivs1b 8			/* invalid side 1 bit		 */
#define ivs2b 64		/* invalid side 2 bit		 */
#define sls1b 512		/* selected side 1 bit		 */
#define sls2b 4096		/* selected side 2 bit		 */

#define maxvhpt 10		/* max. number of via holes/segm */

#ifdef mainleer			/* main router section ?	 */
#define cdext			/* yes: define structures	 */
#define initvl(x) = x;
#else
#define cdext extern		/* no : reference external	 */
#define initvl(x) ;
#endif

cdext int RT_sel initvl(1)	/* router-select: 0=full 1=confined	 */

cdext int C1 initvl(25)		/* boarder for S_route			 */
cdext int C2 initvl(10)		/* fence distance (must be even)	 */
cdext int C3 initvl(2)		/* max number of via holes		 */
cdext int C4 initvl(5)		/* penalty for HV via holes		 */
cdext int C5 initvl(10)		/* penalty for D via holes		 */
cdext int C6 initvl(8)		/* via hole alignment (must be even)	 */
cdext int C7 initvl(3)		/* via hole penalty progression		 */

cdext int K1 initvl(10)		/* small delta limit for single side run */
cdext int K2 initvl(8)		/* boarder size for single side runs	 */
cdext int K3 initvl(8)		/* boarder size for L - runs	 	 */
#define K3max 15
cdext int K4 initvl(50)		/* min length for Z - runs		 */
cdext int K5 initvl(16)		/* > K3 min progress for each segment	 */
cdext int K6 initvl(4)		/* max number of via holes in L* routes	 */
cdext int K7 initvl(0)		/* max level for L*_route retries	 */
#define maxvhct 260		/* >=(K3 +1)**2 via-hole cost table	 */
cdext int K8 initvl(-7)		/* rectangular via hole bonus	 */

/* #define debug	0 	*/	/* debuging option		 */
#ifdef debug
    cdext struct nlhd *testbp;	/* debuging stuff		 */
    cdext int tx1, ty1, tx2, ty2;
    cdext int ttt;
#endif

cdext int sx, sy;		/* size of temporary bit map	 */
cdext int ox, oy;		/* offset to temp. origin	 */
cdext char *abm initvl(0)	/* pointer to aux. Bit map	 */

/***********************************************************************\
* 								        *
*  Position <x,y> in the auxiliary bit map is position <x+ox,y+oy> in   *
*  the main bitmap (= pcb[y+oy][x+ox]). This referes to *(abm+x+y*sx).  *
* 								        *
\***********************************************************************/

cdext int     dir[16];		/* direction table (*2) 	 */

/**********************************************************************\
* 								       *
*  The ray tables keep track of the expanding wave the sweeps trough   *
*  the maze. Each entry represents a particular direction. If it hits  *
*  any obstacle, the ray terminates. as it propergates, it tries to    *
*  spawn new rays in directions +/- 1 of its own direction.	       *
*  A ray table entry has the following format:			       *
*  (it was compressed into a word because the number of rays can be    *
*  quite large)							       *
* 								       *
*         MSB                            LSB			       *
*          ========--------========--------			       *
*          ************************........ : offset in abm	       *
*          ............................*... : side bit: 0=s1b 1=s2b    *
*          .............................*** : direction (0-7)	       *
*          ........................****.... : Via hole count (0-15)    *
* 								       *
\**********************************************************************/
cdext unsigned *drt, *hvrt;	/* ray tables			 */
#define rtsb 0x08		/* side bit			 */
#define rtdr 0x07		/* direction bits		 */
#define rtvc 0xf0		/* via counter bits		 */
cdext int
	drtc, drtcd, drtmax,	/* ray tab cnt (diagonal)	 */
	hvrtc, hvrtcd, hvrtmax;	/* ray tab cnt (horizontal)	 */
struct vhtab {			/* via hole table		 */
	char *t;		/* koordinate			 */
	char s;			/* from side (= s1b or s2b)	 */
	char n;			/* wait counter			 */
	char c;			/* hole counter			 */
}   vht[maxvht];
cdext int vhtc;		/* counter for vht		 */

cdext char *vhpt[maxvhpt];	/* via hole pointer table	 */
cdext int vhptc;		/* number of via holes in segm	 */

#undef cdext
#undef initvl
