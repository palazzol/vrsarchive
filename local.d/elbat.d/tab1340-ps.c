/*	This is an example of a source file for an nroff driver table.
 *	It is a very fancy description of a Toshiba P1340 in the high
 *	quality proportional spacing font.
*/

#include	<sys/types.h>	/* Req'd only for termio.h	*/
#include	<termio.h>	/* Req'd only for bset, breset	*/

#define INCH 240

struct {
	int bset;
	int breset;
	int Hor;
	int Vert;
	int Newline;
	int Char;
	int Em;
	int Halfline;
	int Adj;
	char *twinit;
	char *twrest;
	char *twnl;
	char *hlr;
	char *hlf;
	char *flr;
	char *bdon;
	char *bdoff;
	char *iton;
	char *itoff;
	char *ploton;
	char *plotoff;
	char *up;
	char *down;
	char *right;
	char *left;
	char *codetab[256-32];
	char *zzz;
	} t = {
/*bset    */		0,
/*breset  */		ONLCR | OCRNL | ONLRET,
/*Hor     */		INCH / 60,
/*Vert    */		INCH / 48,
/*Newline */		INCH / 6,
/*Char    */		INCH / 120,
/*Em      */		INCH / 10,
/*Halfline*/		INCH / 12,
/*Adj     */		INCH / 60,
/*twinit  */		"\033*3",
/*twrest  */		"\033*0",
/*twnl    */		"\r\n",
/*hlr     */		"\033D",
/*hlf     */		"\033U",
/*flr     */		"\033D\033D",
/*bdon    */		"\033I",
/*bdoff   */		"\033J",
/*iton    */		"\033I",
/*itoff   */		"\033J",
/*ploton  */		"\033\063",
/*plotoff */		"\033\064",
/*up      */		"",
/*down    */		"",
/*right   */		" ",
/*left    */		"\b",
/* space */         	"\012 ",
/* ! */             	"\006!",
/* " */             	"\011\"",
/* # */             	"\014#",
/* $ */             	"\013$",
/* % */             	"\013%",
/* & */             	"\013&",
/* ' close */       	"\005'",
/* ( */             	"\007(",
/* ) */             	"\007)",
/* * */             	"\011*",
/* + */             	"\013+",
/* , */             	"\005,",
/* - hyphen */      	"\013-",
/* . */             	"\005.",
/* / */             	"\011/",
/* 0 */             	"\213\060",
/* 1 */             	"\213\061",
/* 2 */             	"\213\062",
/* 3 */             	"\213\063",
/* 4 */             	"\213\064",
/* 5 */             	"\213\065",
/* 6 */             	"\213\066",
/* 7 */             	"\213\067",
/* 8 */             	"\213\070",
/* 9 */             	"\213\071",
/* : */             	"\006:",
/* ; */             	"\006;",
/* < */             	"\012<",
/* = */             	"\013=",
/* > */             	"\012>",
/* ? */             	"\011?",
/* @ */             	"\013@",
/* A */             	"\214A",
/* B */             	"\213B",
/* C */             	"\213C",
/* D */             	"\213D",
/* E */             	"\213E",
/* F */             	"\213F",
/* G */             	"\213G",
/* H */             	"\214H",
/* I */             	"\211I",
/* J */             	"\213J",
/* K */             	"\214K",
/* L */             	"\213L",
/* M */             	"\214M",
/* N */             	"\214N",
/* O */             	"\213O",
/* P */             	"\213P",
/* Q */             	"\213Q",
/* R */             	"\214R",
/* S */             	"\213S",
/* T */             	"\213T",
/* U */             	"\213U",
/* V */             	"\214V",
/* W */             	"\214W",
/* X */             	"\214X",
/* Y */             	"\214Y",
/* Z */             	"\213Z",
/* [ */             	"\007[",
/* \ */             	"\011\\",
/* ] */             	"\007]",
/* ^ */             	"\011^",
/* _ dash */        	"\014\033\063\033D\033\064_\033\063\033U\033\064",
/* ` open */        	"\011'",
/* a */             	"\213a",
/* b */             	"\213b",
/* c */             	"\213c",
/* d */             	"\213d",
/* e */             	"\213e",
/* f */             	"\211f",
/* g */             	"\213g",
/* h */             	"\214h",
/* i */             	"\211i",
/* j */             	"\211j",
/* k */             	"\214k",
/* l */             	"\211l",
/* m */             	"\214m",
/* n */             	"\214n",
/* o */             	"\213o",
/* p */             	"\214p",
/* q */             	"\214q",
/* r */             	"\213r",
/* s */             	"\213s",
/* t */             	"\211t",
/* u */             	"\214u",
/* v */             	"\214v",
/* w */             	"\214w",
/* x */             	"\214x",
/* y */             	"\214y",
/* z */             	"\213z",
/* { */             	"\007{",
/* | */             	"\005|",
/* } */             	"\007}",
/* ~ */             	"\011~",
/* narrow sp */     	"\000\0",
/* hyphen */        	"\013-",
/* bullet */        	"\000\0",
/* square */        	"\000\0",
/* 3/4 em */        	"\013-",
/* rule */          	"\014_",
/* 1/4 */           	"\037\061/4",
/* 1/2 */           	"\037\061/2",
/* 3/4 */           	"\037\063/4",
/* minus */         	"\013-",
/* fi */            	"\222fi",
/* fl */            	"\222fl",
/* ff */            	"\222ff",
/* ffi */           	"\233ffi",
/* ffl */           	"\233ffl",
/* degree */        	"\000\0",
/* dagger */        	"\000\0",
/* section */       	"\000\0",
/* foot mark */     	"\000\0",
/* acute accent */  	"\005'",
/* grave accent */  	"\011`",
/* underrule */     	"\014_",
/* slash (longer) */	"\011/",
/* half narrow space */	"\000\0",
/* unpaddable space */	"\012 ",
/* alpha */         	"\000\0",
/* beta */          	"\000\0",
/* gamma */         	"\000\0",
/* delta */         	"\000\0",
/* epsilon */       	"\000\0",
/* zeta */          	"\000\0",
/* eta */           	"\000\0",
/* theta */         	"\000\0",
/* iota */          	"\000\0",
/* kappa */         	"\000\0",
/* lambda */        	"\000\0",
/* mu */            	"\000\0",
/* nu */            	"\000\0",
/* xi */            	"\000\0",
/* omicron */       	"\000\0",
/* pi */            	"\000\0",
/* rho */           	"\000\0",
/* sigma */         	"\000\0",
/* tau */           	"\000\0",
/* upsilon */       	"\000\0",
/* phi */           	"\000\0",
/* chi */           	"\000\0",
/* psi */           	"\000\0",
/* omega */         	"\000\0",
/* Gamma */         	"\000\0",
/* Delta */         	"\000\0",
/* Theta */         	"\013-\b0",
/* Lambda */        	"\000\0",
/* Xi */            	"\000\0",
/* Pi */            	"\000\0",
/* Sigma */         	"\000\0",
/* Tau */           	"\000\0",
/* Upsilon */       	"\000\0",
/* Phi */           	"\000\0",
/* Psi */           	"\000\0",
/* Omega */         	"\000\0",
/* square root */   	"\000\0",
/* terminal sigma */	"\000\0",
/* root en */       	"\000\0",
/* >= */            	"\014>\b_",
/* <= */            	"\014<\b_",
/* identically equal */	"\015_\b=",
/* equation minus */	"\013-",
/* approx = */      	"\013~\b_",
/* approximates */  	"\011\033U~\033D",
/* not equal */     	"\013=\b/",
/* right arrow */   	"\025->",
/* left arrow */    	"\025<-",
/* up arrow */      	"\000\0",
/* down arrow */    	"\000\0",
/* eqn equals */    	"\013=",
/* multiply */      	"\014x",
/* divide */        	"\011/",
/* plus-minus */    	"\014-\b+",
/* cup (union) */   	"\000\0",
/* cap (intersection) */ "\000\0",
/* subset of */     	"\000\0",
/* superset of */   	"\000\0",
/* improper subset */	"\000\0",
/* improper superset */	"\000\0",
/* infinity */      	"\027oo",
/* pt deriv */      	"\000\0",
/* gradient */      	"\000\0",
/* not */           	"\000\0",
/* integral */      	"\000\0",
/* proportional to */	"\007\033U~\033D\b~",
/* empty set */     	"\012\060\b/",
/* member of */     	"\000\0",
/* equation plus */ 	"\013+",
/* registration mk */	"\000\0",
/* copyright mk */  	"\000\0",
/* box rule */      	"\005|",
/* cent sign */     	"\000\0",
/* dbl dagger */    	"\000\0",
/* right hand */    	"\025=>",
/* left hand */     	"\025<=",
/* math *  */       	"\011*",
/* bell system sign */	"\000\0",
/* or (was star) */ 	"\005|",
/* circle */        	"\013o",
/* left top of big curly */	"\000\0",
/* left bottom of big curly */	"\000\0",
/* right top of big curly */	"\000\0",
/* right bottom of big curly */	"\000\0",
/* left center of big curly */	"\000\0",
/* right center of big curly */	"\000\0",
/* bold vertical rule */	"\000\0",
/* left bottom of big bracket */	"\000\0",
/* right bottom of big bracket */	"\000\0",
/* left top of big bracket */	"\000\0",
/* right top of big bracket */	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
/* ??? */           	"\000\0",
};
