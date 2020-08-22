
/* Written 12:01 pm  Jan  8, 1985 by gwyn@brl-tgr in uiucdcsb:net.unix */
/*
	symbol -- software character generator subroutine

	last edit:	26-Nov-1984	D A Gwyn
			18-Mar-1985     A D Robison - hacked for GSI card
						      rehacked for console

Function:

	This routine plots an ASCII character string as vector strokes.


Calling sequence:

	void ConSymbol (
		     char *string,	   // -> NUL-terminated string
		     int transform [2][3]; // text transformation //
		   );

The characters are on a 6 high by 4 wide grid.  The transform is scaled
by 1024 and transforms from the character grid onto console device coordinates.
E.g. the unit transform is {{1024,0,0},{0,1024,0}}.

*/

/*
				STROKE TABLES

	The stroke[] table contains encodings for all vector strokes
	needed to draw each character at a standard size.  Actual plot
	output is of course properly positioned, scaled, and rotated.
	To keep code size small, variable-length entries are used; each
	character stroke sequence is terminated by a 0 datum.  Pointers
	to the appropriate data for every character is stored into
	sstroke[] during a one-time initialization.

	The prototypes are constrained to a 4 x 6 unit area, except for
	occasional descenders up to 2 units below the baseline.  All
	visible strokes should be "basic" vectors (in directions that
	are integral multiples of 45 degrees) for best overall results
	on most devices, especially with small character height.  The
	first 16 "control" characters are plotted as non-standard extra
	symbols, the next 16 produce Calcomp "centered plotting symbols"
	(not centered here!), and the final 96 characters are plotted as
	corresponding ASCII graphics (DEL plots as a grid).

	A prototype stroke is encoded as 8 bits SVXXXYYY:
		S	= 0 if YYY is correct as is
			  1 if YYY needs to have 2 subtracted
		V	= 0 if stroke is invisible (move)
			  1 if stroke is visible (draw)
		XXX	= final X coord of stroke (0..4)
		YYY	= final Y coord of stroke (0..6)
*/

/* bit masks for fields in stroke vector */

#define	S	0200
#define	V	0100
#define	XXX	0070
#define	YYY	0007

#define	XJUST	3			/* bits to the right of XXX */

/* stroke vectors for all characters */

static char	stroke[] =
	{
/*NUL*/	0003, 0105, 0123, 0143, 0141, 0121, 0125, 0,
/*SOH*/	0006, 0115, 0112, 0142, 0022, 0121, 0141, 0140, 0120, 0013,
	0133, 0034, 0114, 0015, 0126, 0,
/*STX*/	0021, 0125, 0105, 0103, 0123, 0141, 0143, 0,
/*ETX*/	0012, 0114, 0034, 0104, 0106, 0126, 0124, 0033, 0113, 0021,
	0141, 0042, 0122, 0120, 0,
/*EOT*/	0005, 0125, 0134, 0145, 0143, 0023, 0125, 0015, 0113, 0,
/*ENQ*/	0011, 0131, 0142, 0144, 0135, 0115, 0104, 0102, 0111, 0012,
	0114, 0134, 0133, 0113, 0023, 0132, 0,
/*ACK*/	0011, 0131, 0142, 0144, 0135, 0115, 0104, 0102, 0111, 0034,
	0114, 0112, 0132, 0,
/*BEL*/	0021, 0122, 0142, 0133, 0134, 0124, 0125, 0024, 0114, 0113,
	0102, 0122, 0,
/*BS */	0012, 0103, 0114, 0003, 0143, 0,
/*HT */	0003, 0143, 0034, 0143, 0132, 0,
/*LF */	0012, 0121, 0132, 0021, 0125, 0,
/*VT */	0021, 0125, 0014, 0125, 0134, 0,
/*FF */	0012, 0121, 0132, 0021, 0125, 0014, 0125, 0134, 0,
/*CR */	0012, 0103, 0114, 0003, 0143, 0034, 0143, 0132, 0,
/*SO */	0004, 0124, 0126, 0106, 0104, 0014, 0112, 0142, 0034, 0130, 0,
/*SI */	0021, 0123, 0013, 0115, 0025, 0105, 0003, 0123, 0141, 0143, 0,
/*DLE*/	0023, 0125, 0145, 0141, 0101, 0105, 0125, 0,
/*DC1*/	0023, 0125, 0135, 0144, 0142, 0131, 0111, 0102, 0104, 0115,
	0125, 0,
/*DC2*/	0023, 0124, 0142, 0102, 0124, 0,
/*DC3*/	0021, 0125, 0003, 0143, 0,
/*DC4*/	0001, 0145, 0005, 0141, 0,
/*NAK*/	0023, 0125, 0143, 0121, 0103, 0125, 0,
/*SYN*/	0021, 0125, 0143, 0103, 0125, 0,
/*ETB*/	0001, 0145, 0105, 0141, 0,
/*CAN*/	0005, 0145, 0101, 0141, 0,
/*EM */	0023, 0121, 0005, 0123, 0145, 0,
/*SUB*/	0023, 0145, 0034, 0132, 0141, 0032, 0112, 0101, 0012, 0114,
	0105, 0014, 0134, 0,
/*ESC*/	0001, 0145, 0025, 0121, 0041, 0105, 0003, 0143, 0,
/*FS */	0001, 0141, 0105, 0145, 0101, 0,
/*GS */	0021, 0125, 0,
/*RS */	0023, 0125, 0024, 0142, 0102, 0124, 0021, 0122, 0144, 0104,
	0122, 0,
/*US */	0023, 0143, 0,
/*SP */	0,
/* ! */	0020, 0121, 0022, 0126, 0,
/* " */	0014, 0116, 0036, 0134, 0,
/* # */	0010, 0116, 0036, 0130, 0042, 0102, 0004, 0144, 0,
/* $ */	0002, 0111, 0131, 0142, 0133, 0113, 0104, 0115, 0135, 0144,
	0026, 0120, 0,
/* % */	0001, 0145, 0025, 0114, 0105, 0116, 0125, 0032, 0141, 0130,
	0121, 0132, 0,
/* & */	0040, 0104, 0105, 0116, 0125, 0124, 0102, 0101, 0110, 0120,
	0142, 0,
/* ' */	0014, 0136, 0,
/* ( */	0030, 0112, 0114, 0136, 0,
/* ) */	0010, 0132, 0134, 0116, 0,
/* * */	0001, 0145, 0025, 0121, 0041, 0105, 0,
/* + */	0021, 0125, 0003, 0143, 0,
/* , */	0211, 0120, 0121, 0,
/* - */	0003, 0143, 0,
/* . */	0020, 0121, 0,
/* / */	0001, 0145, 0,
/* 0 */	0001, 0145, 0136, 0116, 0105, 0101, 0110, 0130, 0141, 0145, 0,
/* 1 */	0010, 0130, 0020, 0126, 0115, 0,
/* 2 */	0005, 0116, 0136, 0145, 0144, 0100, 0140, 0,
/* 3 */	0001, 0110, 0130, 0141, 0142, 0133, 0144, 0145, 0136, 0116,
	0105, 0023, 0133, 0,
/* 4 */	0030, 0136, 0024, 0102, 0142, 0,
/* 5 */	0001, 0110, 0130, 0141, 0143, 0134, 0114, 0103, 0106, 0146, 0,
/* 6 */	0002, 0113, 0133, 0142, 0141, 0130, 0110, 0101, 0105, 0116,
	0136, 0145, 0,
/* 7 */	0006, 0146, 0145, 0112, 0110, 0,
/* 8 */	0013, 0102, 0101, 0110, 0130, 0141, 0142, 0133, 0113, 0104,
	0105, 0116, 0136, 0145, 0144, 0133, 0,
/* 9 */	0001, 0110, 0130, 0141, 0145, 0136, 0116, 0105, 0104, 0113,
	0133, 0144, 0,
/* : */	0020, 0121, 0023, 0124, 0,
/* ; */	0211, 0120, 0121, 0023, 0124, 0,
/* < */	0030, 0103, 0136, 0,
/* = */	0002, 0142, 0044, 0104, 0,
/* > */	0010, 0143, 0116, 0,
/* ? */	0005, 0116, 0136, 0145, 0144, 0122, 0021, 0120, 0,
/* @ */	0031, 0133, 0124, 0113, 0112, 0121, 0131, 0142, 0144, 0135,
	0115, 0104, 0101, 0110, 0130, 0,
/* A */	0104, 0126, 0144, 0140, 0042, 0102, 0,
/* B */	0130, 0141, 0142, 0133, 0144, 0145, 0136, 0106, 0100, 0003,
	0133, 0,
/* C */	0045, 0136, 0116, 0105, 0101, 0110, 0130, 0141, 0,
/* D */	0130, 0141, 0145, 0136, 0106, 0100, 0,
/* E */	0003, 0133, 0046, 0106, 0100, 0140, 0,
/* F */	0106, 0146, 0033, 0103, 0,
/* G */	0022, 0142, 0141, 0130, 0110, 0101, 0105, 0116, 0136, 0145, 0,
/* H */	0106, 0046, 0140, 0043, 0103, 0,
/* I */	0010, 0130, 0020, 0126, 0016, 0136, 0,
/* J */	0001, 0110, 0130, 0141, 0146, 0,
/* K */	0106, 0046, 0102, 0013, 0140, 0,
/* L */	0006, 0100, 0140, 0,
/* M */	0106, 0124, 0146, 0140, 0,
/* N */	0106, 0005, 0141, 0040, 0146, 0,
/* O */	0010, 0130, 0141, 0145, 0136, 0116, 0105, 0101, 0110, 0,
/* P */	0106, 0136, 0145, 0144, 0133, 0103, 0,
/* Q */	0010, 0130, 0141, 0145, 0136, 0116, 0105, 0101, 0110, 0022,
	0140, 0,
/* R */	0106, 0136, 0145, 0144, 0133, 0103, 0013, 0140, 0,
/* S */	0001, 0110, 0130, 0141, 0142, 0133, 0113, 0104, 0105, 0116,
	0136, 0145, 0,
/* T */	0020, 0126, 0006, 0146, 0,
/* U */	0006, 0101, 0110, 0130, 0141, 0146, 0,
/* V */	0006, 0102, 0120, 0142, 0146, 0,
/* W */	0006, 0100, 0122, 0140, 0146, 0,
/* X */	0101, 0145, 0146, 0006, 0105, 0141, 0140, 0,
/* Y */	0020, 0123, 0105, 0106, 0046, 0145, 0123, 0,
/* Z */	0040, 0100, 0101, 0145, 0146, 0106, 0013, 0133, 0,
/* [ */	0030, 0110, 0116, 0136, 0,
/* \ */	0005, 0141, 0,
/* ] */	0010, 0130, 0136, 0116, 0,
/* ^ */	0004, 0126, 0144, 0,
/* _ */	0201, 0341, 0,
/* ` */	0016, 0134, 0,
/* a */	0003, 0114, 0134, 0143, 0140, 0042, 0112, 0101, 0110, 0130,
	0141, 0,
/* b */	0106, 0001, 0110, 0130, 0141, 0143, 0134, 0114, 0103, 0,
/* c */	0043, 0134, 0114, 0103, 0101, 0110, 0130, 0141, 0,
/* d */	0043, 0134, 0114, 0103, 0101, 0110, 0130, 0141, 0040, 0146, 0,
/* e */	0002, 0142, 0143, 0134, 0114, 0103, 0101, 0110, 0130, 0141, 0,
/* f */	0010, 0115, 0126, 0136, 0145, 0034, 0104, 0,
/* g */	0201, 0310, 0330, 0341, 0144, 0041, 0130, 0110, 0101, 0103,
	0114, 0134, 0143, 0,
/* h */	0106, 0003, 0114, 0134, 0143, 0140, 0,
/* i */	0020, 0124, 0114, 0025, 0126, 0,
/* j */	0201, 0310, 0330, 0341, 0144, 0045, 0146, 0,
/* k */	0106, 0044, 0100, 0022, 0140, 0,
/* l */	0020, 0126, 0116, 0,
/* m */	0104, 0003, 0114, 0123, 0120, 0040, 0143, 0134, 0123, 0,
/* n */	0104, 0003, 0114, 0134, 0143, 0140, 0,
/* o */	0010, 0130, 0141, 0143, 0134, 0114, 0103, 0101, 0110, 0,
/* p */	0001, 0110, 0130, 0141, 0143, 0134, 0114, 0103, 0004, 0300, 0,
/* q */	0041, 0130, 0110, 0101, 0103, 0114, 0134, 0143, 0044, 0340, 0,
/* r */	0104, 0003, 0114, 0134, 0143, 0,
/* s */	0001, 0110, 0130, 0141, 0132, 0112, 0103, 0114, 0134, 0143, 0,
/* t */	0004, 0134, 0015, 0111, 0120, 0130, 0141, 0,
/* u */	0004, 0101, 0110, 0130, 0141, 0040, 0144, 0,
/* v */	0004, 0102, 0120, 0142, 0144, 0,
/* w */	0004, 0101, 0110, 0121, 0022, 0121, 0130, 0141, 0144, 0,
/* x */	0144, 0004, 0140, 0,
/* y */	0201, 0310, 0330, 0341, 0144, 0004, 0101, 0110, 0130, 0141, 0,
/* z */	0004, 0144, 0100, 0140, 0,
/* { */	0030, 0121, 0122, 0113, 0124, 0125, 0136, 0,
/* | */	0020, 0126, 0,
/* } */	0010, 0121, 0122, 0133, 0124, 0125, 0116, 0,
/* ~ */	0005, 0116, 0134, 0145, 0,
/*DEL*/	0140, 0146, 0106, 0100, 0010, 0116, 0026, 0120, 0030, 0136, 0
	};

/* pointers to start of stroke data for each character */

static char	*sstroke[128] = {(char *) 0};

			/* CONSTANTS */
#define	CHSPAC	6			/* prototype text spacing */
#define	ASCMASK	0177			/* 7-bit ASCII mask */
#define void int

			/* GLOBAL DATA */

static int (*T)[3];			/* text transformation */

			/* ENTRY POINT */

void ConSymbol (string,transform)
   char *string;		/* -> NUL-terminated string */
   int	transform[2][3];	/* text transformation */
   {
      register char *sp;	/* -> stroke data */
      register int cornx;	/* proto X of cell corner */
      register int c;	        /* char from ASCII string    */
				/* also used for stroke data */

      /* initialize starting stroke pointers upon first entry only */

      if (!sstroke[0]) {
	 sp = stroke;
	 for (c = 0; c < 128; ++c) {
	    sstroke [c] = sp;		 	 /* starts here */
	    while (*sp++) continue; 		 /* 0 terminates the data */
	 }
      }

      T = transform;

      /* look up strokes for each character and plot them */

      for (cornx = 0; c = *string++; cornx += CHSPAC) {

	 sp = sstroke [c & ASCMASK]; /* -> stroke data */

	 plot (cornx,0,0,0); 	/* get to character cell LLC */

	 /* draw the strokes starting at LLC */

	 while (c = *sp++ )	/* get stroke */
	    plot (cornx + ((c & XXX) >> XJUST),
		  (c & YYY) - ((c & S) ? 2 : 0),
		  (c & V),
		  (*sp & V));	/* move or draw */
      }
   }

/* transform prototype coordinates to actual plot coordinates */
#define map(x,y,n) ((T[n][0] * x + T[n][1]*y + T[n][2]) + 512 >> 10);

static void plot (dx,dy,vis,NextVis)	/* plot adjusted stroke */
   int dx,dy;				/* unrot pos rel to text LLC */
   int vis;			        /* nonzero => visible */
   int NextVis;				/* Is the next stroke visible? */
   {
      static int oldposx=0,oldposy=0;
      static int olddx,olddy;
      static int oldValid;
      int posx,posy;

      if (vis && !oldValid) {
	 oldposx = map (olddx,olddy,0);
	 oldposy = map (olddx,olddy,1);
	 VI_AMove (oldposx,oldposy);
      }
      posx = map (dx,dy,0);
      posy = map (dx,dy,1);

      /* no arithmetic overflow checking is done */

      if (vis) VI_RLine (posx-oldposx,posy-oldposy);

      oldValid = vis;
      olddx = dx;
      olddy = dy;
      oldposx = posx;
      oldposy = posy;
   }

/* End of text from uiucdcsb:net.unix */
