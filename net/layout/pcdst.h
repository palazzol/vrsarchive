/***************************************************************\
*								*
*	PCB program						*
*								*
*	Common data structures					*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#ifdef mainpgm				/* main program ?	*/
#define cdext				/* define data struct.	*/
#else
#define cdext extern			/* declare data struct. */
#endif

cdext char pcb[ymax][xmax];		/* pcb bit map		*/

cdext struct comp {			/* component desc.	*/
	char name[nmmax];		/* incarnation name	*/
	struct type *ty;		/* type			*/
	int  x, y;			/* board position	*/
	unsigned r:2;			/* rotation 0-3 (*90 dg)*/
	unsigned unplaced:1;		/* =1 if not placed	*/
} CP[cpmax];

cdext struct type {			/* type decriptor	*/
	char name[nmmax];		/* type name		*/
	int x, y;			/* component box	*/
	short np;			/* number of pins	*/
	short cif;			/* 0 or (index to cif)+1*/
	struct pin *p;			/* start of pin list	*/
} TY[tymax];

cdext struct pin {			/* pin descr.		*/
	unsigned p:3;			/* pin type:		*/
					/*    0 : normal	*/
					/*    1 : ground	*/
					/*    2 : Vcc		*/
					/*    3 : Vee		*/
					/*    4 : Vbb		*/
	int x, y;			/* location		*/
} PN[pnmax];

cdext struct hole {			/* pcb hole		*/
	unsigned short x, y;		/* hole position	*/
	unsigned short pn;		/* pin number	[0..n]	*/
	unsigned short cpi;		/* component index	*/
	struct nlst *n;			/* net pointer		*/
} CH[chmax];

cdext struct nlhd {			/* net list heads	*/
	char name[nmmax];		/* net name		*/
	int l;				/* length of list	*/
	struct nlst *lp;		/* list pointer		*/
	int x1, y1, x2, y2;		/* list confinment	*/
	unsigned f:1;			/* flag: 1:done 0:not d.*/
	unsigned ref:1;			/* referenced (update)	*/
} NH[nhmax];

cdext struct nlst {			/* net list		*/
	struct hole *c;			/* component hole	*/
	struct nlst *n;			/* next list element	*/
	struct nlhd *nlp;		/* net list back pointer*/
	int mk;				/* mark for con. check	*/
} NL[nlmax];

cdext struct cif {			/* external cif descrip.*/
	int symn;			/* cif-symbol number	*/
	int xl, yl, xh, yh;		/* off-limit area	*/
	unsigned flg:1;			/* =1 for extenension	*/
	unsigned blk:1;			/* =1 if block present	*/
} CIF[cifmax];

cdext struct vtin {			/* vital information	*/
	int ncp;			/* number of components */
	int nty;			/* number of types	*/
	int npn;			/* number of pins	*/
	int nch;			/* number of com. holes */
	int nnh;			/* number of nets	*/
	int nnl;			/* total net list length*/
	int pver;			/* program version	*/
	int x, y;			/* size of used bit-map */
	struct comp *cp;		/* CP start pointer	*/
	struct type *ty;		/* TY start pointer	*/
	struct pin  *pn;		/* PN start pointer	*/
	struct hole *ch;		/* CH start pointer	*/
	struct nlhd *nh;		/* NH start pointer	*/
	struct nlst *nl;		/* NL start pointer	*/
	struct nlhd *cnet;		/* current net		*/
	struct nlhd *enhl;		/* empty NH list	*/
	struct nlst *enll;		/* empty NL list	*/
        int BSX, BSY;			/* board size		*/
	int reserved[8];		/* reserved for future use */
	char errtxt[82];		/* famous last words	*/
	int err_info[4];		/* some explanation	*/
	struct nlhd GND, VCC, VTT, VEE;	/* power nets		*/
	int resrv1;			/* reserved for future use */
	int resrv2;			/* reserved for future use */
	int ncif;			/* #of cif records	*/
	struct vtin *own;		/* own address          */
} V 
#ifdef mainpgm
	= {0, 0, 0, 0, 0, 0, pversion, xmax, ymax,
	   CP, TY, PN, CH, NH, NL, 0, 0, 0,
	   0, 0,			/* BSX, BSY		*/
	   0, 0, 0, 0, 0, 0, 0, 0,
	   "", 0, 0, 0, 0,		/* error info		*/
	   {"Gnd", 0, 0, xmax, ymax, 0, 0, 1},
	   {"Vcc", 0, 0, xmax, ymax, 0, 0, 1},
	   {"Vee", 0, 0, xmax, ymax, 0, 0, 1},
	   {"Vtt", 0, 0, xmax, ymax, 0, 0, 1},
	   0, 0, 0, &V
	}
#endif
;

/* The next variables are not realy common, but usefull		*/
cdext int	cx, cy,			/* current x/y position */
		wx, wy,			/* current window pos.  */
		fx, fy,			/* current frame pos.	*/
		cz,			/* current zoom factor  */
		ccc,			/* current color code	*/
		ccm;			/* complement color mask*/

cdext int	wtsb;			/* wire trace side bit	*/
cdext int (*wthf)(), (*wtvf)();	        /* wire trace function pointer	*/
cdext int nettgo;			/* nets to go (status info)	*/
cdext int no_beep			/* beep on/off switch		*/
#ifdef mainpgm
	= 0
#endif
		;
cdext int no_adjw			/* adjw on/off switch		*/
#ifdef mainpgm
	= 1
#endif
		;
cdext int st_reroute			/* straight by reroute		*/
#ifdef mainpgm
	= 0
#endif
		;
cdext int no_hrout			/* home route on/off switch	*/
#ifdef mainpgm
	= 0
#endif
		;
cdext int rt_str			/* routing strategy		*/
#ifdef mainpgm
	= 0
#endif
		;
cdext int batch				/* batch mode indicator		*/
#ifdef mainpgm
	= 0
#endif
		;
	
cdext int dr[8][2] 			/* wire directions		*/
#ifdef mainpgm
		=	{ { 1,  0},
			  { 1,  1},
			  { 0,  1},
			  {-1,  1},
			  {-1,  0},
			  {-1, -1},
			  { 0, -1},
	 		  { 1, -1}  }
#endif
;

#define ncolors 13 		/* number of color layers		 */
cdext struct color_tab {	/* guess what: this is the color table	 */
     int Z, O;			/* off/on bit mask			 */
     int r, g, b;		/* red, green, blue color intensity	 */
     char *name;		/* layer name				 */
} Color_tab[ncolors]
#ifdef mainpgm
	    = {	{0x00, 0x20, 255, 255, 255, "Text"},
		{0x00, 0x10,   0, 255, 255, "Frame & ext. cif symbols"},
		{0x80, 0x04,   0,   0, 200, "Componet pins"},
		{0x80, 0x08, 190, 190,   0, "Via holes"},
		{0x80, 0x01,   0, 190,   0, "Component side"},
		{0x80, 0x02, 190,   0,   0, "Solder side"},
		{0x00, 0x04, 190,   0, 190, "*Componet pins"},
		{0x00, 0x08, 255, 255,   0, "*Via holes"},
		{0x00, 0x01,   0, 255,   0, "*Component side"},
		{0x00, 0x02, 255,   0,   0, "*Solder side"},
		{0x00, 0x40,   0, 190, 200, "Cursor"},
		{0x80, 0x00, 100, 120, 130, "Background"},
		{0x00, 0x00, 100, 120, 200, "Trouble"}
	      }
#endif
;
#define CT_s1_n 4 		/* index to side 1 (normal)	 */
#define CT_s1_s 8		/* index to side 1 (selected)	 */
#define CT_s2_n 5		/* index to side 2 (normal)	 */
#define CT_s2_s 9		/* index to side 2 (selected)	 */

cdext int top_side		/* visable top side (to select stuff)	*/
#ifdef mainpgm
			= s1b
#endif
;

cdext char placed		/* =1: components are placed	 */
#ifdef mainpgm
			= 0
#endif
;

cdext FILE *save_fp		/* keystroke save file		 */
#ifdef mainpgm
			= 0
#endif
;

cdext FILE *replay_fp		/* keystroke replay file	 */
#ifdef mainpgm
			= 0
#endif
;

cdext int mac_exp		/* macro expansion flag		*/
#ifdef mainpgm
			= 0
#endif
;

cdext int mac_def		/* macro definition flag	*/
#ifdef mainpgm
			= 0
#endif
;

cdext int msg_svd		/* message save delay flag	 */
#ifdef mainpgm
			= 0
#endif
;

#undef cdext
