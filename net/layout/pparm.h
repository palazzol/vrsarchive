/***************************************************************\
*								*
*	PCB program						*
*								*
*	Program parameter definitions				*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#define pversion 124			/* program version 1.24 */
#define mgnm 3341			/* magic number		*/
#define xmax 1280			/* max. X size +2	*/
#define ymax 1024			/* max. Y size +2	*/
#define ovs  50				/* overshot for windows */
#define rsun 8				/* raster unit		*/
#define savcnt 25			/* auto save period	*/
#define pvws 200			/* preview window size	*/
#define delrmax 20			/* max. delete recursion*/

#define nmmax 15			/* name length		*/
#define cpmax 500	/* < 64K */	/* component limit	*/
#define tymax 500			/* type limit		*/
#define pnmax 2000			/* pin definition limit */
#define chmax 10000			/* component hole limit */
#define nhmax 4000			/* net limit		*/
#define nlmax 8000			/* net length limit	*/
#define wbmax 100			/* current chain buffer	*/
#define lrn_max 200			/* learn buffer size	*/
#define cifmax 500			/* ex CIF symtab size	*/

#define ex_cifL 500			/* range for External	*/
#define ex_cifH 999			/*   cif descriptions	*/

#define inp_lnl 1024			/* input line length	*/

#define cp_align(x) ((x+2) & 0xfffffffc)/* align a component co */
#define cp_alchk(x) (x & 3)		/* CP align check: 1=err*/

#define nil 0				/* used for pointers	*/
#define DELETED "deleted"		/* used to invalidate TY, CP */

#define s1b  1				/* side 1 bit		*/
#define s2b  2				/* side 2 bit		*/
#define chb  4				/* component hole bit   */
#define ishb 8				/* inter side hole bit  */
#define fb   16				/* frame bit		*/
#define resb 32				/* spare: may need 2 mrk*/
#define mrkb 64				/* mark for wire trace	*/
#define selb 128			/* select bit		*/
/****************************************************************************\
* 									     *
*  Usage of bits:							     *
* 		    -- in pcb --       		-- on the AED --	     *
*     s1b:            side1 trace bit		same			     *
*     s2b:	    side2 trace bit		same			     *
*     chb:	    component hole outline	same			     *
*     ishb:	    via-hole outline		same			     *
*     fb:		    restricted areas (frame..)	same		     *
*     resb:	    side disambiguation		text & graphic overlay	     *
*     mrkb:	    temp marker for traces	cursor plane		     *
*     selb:	    select marker		highlight objects	     *
* 									     *
*  Conventions:								     *
*     'selb' is used to select nets: V.cnet will have a pointer to	     *
* 	   the selected net or is '0'. Only one net can be selected at	     *
* 	   a time and all points that are selected should eventually be	     *
* 	   connected electrically. Selected object may not be adjacents	     *
* 	   to unselected ones (design rule). If V.cnet is '0' no select	     *
* 	   bits should be present.					     *
*     'mrkb' is used as temporary marker within certain functions (wtrace,   *
* 	   adj_sort). It must be cleared when those functions are done.	     *
*     'resb' is set only along with 'selb' if 's2b' is present as part of    *
* 	   the selection process to disambiguate between a selected	     *
* 	   's1b' and a selected 's2b' when they are both set.		     *
* 									     *
*  Note: this particular use of the bit-planes is a serious design mistake,  *
*        but it is too late to fix that. A better bit assignment would pack  *
*        'fb', 'ishb' and 'chb' into 2 bit (along with background there are  *
*        4 mutual exclusive possibilities) and would use 3 bits for each     *
*        signal plane: a presence bit ('s1b','s2b'), a select bit ('sel1b',  *
*        'sel2b') and a mark bit ('mrk1b', 'mrk2b'). This would get rid of   *
*        many constaints and special cases: a vastly cleaner solution.	     *
* 									     *
\****************************************************************************/
#define all  255			/* all bit planes	*/
#define vec  3	 /* = s1b|s2b		   vectored bit planes  */
#define ahb  12  /* = chb|ishb		   any hole bit plane	*/
#define rec  28	 /* = chb|ishb|fb	   rectangular b planes */

#define START 0				/* Start context	*/
#define ROUTE 1				/* Route context	*/
#define EDIT 2				/* Edit context		*/
#define LEARN 3				/* Learn context	*/
#define ADELE 4				/* Area delete context	*/
#define AROUTE 5			/* Area route		*/
#define CMOVE 6				/* component move	*/
#define PLOW 7				/* plow context		*/
#define PLACE 8				/* component place cntx */
