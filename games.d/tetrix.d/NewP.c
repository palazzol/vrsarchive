
#include <curses.h>
#include "tet.h"
/*********************************************************************/
/* A new piece is created on the game board if possible */
/* returns 0 if unable to do it */
/*********************************************************************/
NewPiece()
{	
FallingDown = 0;			/* true when fall key is pressed */
Type = ((int)(mrand48() % 4) + 4) * 4;	/* random number 4 8 16 20 24 or 28 */
/* printf("DEBUG:NewPiece Type = %d\n",Type); */

switch (Type) {
	case W_TYPE : /* checked  */
	    if (IS_FREE(STARTCOL-1,STARTROW) && IS_FREE(STARTCOL,STARTROW) &&
		IS_FREE(STARTCOL+1,STARTROW) && IS_FREE(STARTCOL,STARTROW+1)) {
		PUTCH(STARTCOL-1,STARTROW,W_CHAR);
		PUTCH(STARTCOL,STARTROW,W_CHAR);
		PUTCH(STARTCOL+1,STARTROW,W_CHAR);
		PUTCH(STARTCOL,STARTROW+1,W_CHAR);
		}
	    else {
	return(0);
	}
	break;
	case R_TYPE : /* checked */
	    if (IS_FREE(STARTCOL-1,STARTROW) && IS_FREE(STARTCOL,STARTROW) &&
	       IS_FREE(STARTCOL+1,STARTROW) && IS_FREE(STARTCOL-1,STARTROW+1)) {
		PUTCH(STARTCOL-1,STARTROW,R_CHAR);
		PUTCH(STARTCOL,STARTROW,R_CHAR);
		PUTCH(STARTCOL+1,STARTROW,R_CHAR);
		PUTCH(STARTCOL-1,STARTROW+1,R_CHAR);
		}
	    else return(0);
	break;
	case T_TYPE : /* checked */
	    if (IS_FREE(STARTCOL,STARTROW) && IS_FREE(STARTCOL,STARTROW+1) &&
	       IS_FREE(STARTCOL+1,STARTROW) && IS_FREE(STARTCOL+1,STARTROW+1)) {
		PUTCH(STARTCOL,STARTROW,T_CHAR);
		PUTCH(STARTCOL,STARTROW+1,T_CHAR);
		PUTCH(STARTCOL+1,STARTROW,T_CHAR);
		PUTCH(STARTCOL+1,STARTROW+1,T_CHAR);
		}
	    else return(0);
	break;
	case Y_TYPE : /* checked */
	   if (IS_FREE(STARTCOL-1,STARTROW+1) && IS_FREE(STARTCOL,STARTROW+1) &&
		IS_FREE(STARTCOL,STARTROW) && IS_FREE(STARTCOL+1,STARTROW)) {
		PUTCH(STARTCOL-1,STARTROW+1,Y_CHAR);
		PUTCH(STARTCOL,STARTROW+1,Y_CHAR);
		PUTCH(STARTCOL,STARTROW,Y_CHAR);
		PUTCH(STARTCOL+1,STARTROW,Y_CHAR);
		}
	    else return(0);
	break;
	case G_TYPE : { /* checked */
	    if (IS_FREE(STARTCOL-1,STARTROW) && IS_FREE(STARTCOL,STARTROW) &&
	       IS_FREE(STARTCOL,STARTROW+1) && IS_FREE(STARTCOL+1,STARTROW+1)) {
		PUTCH(STARTCOL-1,STARTROW,G_CHAR);
		PUTCH(STARTCOL,STARTROW,G_CHAR);
		PUTCH(STARTCOL,STARTROW+1,G_CHAR);
		PUTCH(STARTCOL+1,STARTROW+1,G_CHAR);
		}
	    else return(0);
	break; }
	case B_TYPE : /* checked */
	    if (IS_FREE(STARTCOL-1,STARTROW) && IS_FREE(STARTCOL,STARTROW) &&
	       IS_FREE(STARTCOL+1,STARTROW) && IS_FREE(STARTCOL+1,STARTROW+1)) {
		PUTCH(STARTCOL-1,STARTROW,B_CHAR);
		PUTCH(STARTCOL,STARTROW,B_CHAR);
		PUTCH(STARTCOL+1,STARTROW,B_CHAR);
		PUTCH(STARTCOL+1,STARTROW+1,B_CHAR);
		}
	    else return(0);
	break;
	case V_TYPE : /* checked */
	    if (IS_FREE(STARTCOL-1,STARTROW) && IS_FREE(STARTCOL,STARTROW) &&
		IS_FREE(STARTCOL+1,STARTROW) && IS_FREE(STARTCOL+2,STARTROW)) {
		PUTCH(STARTCOL-1,STARTROW,V_CHAR);
		PUTCH(STARTCOL,STARTROW,V_CHAR);
		PUTCH(STARTCOL+1,STARTROW,V_CHAR);
		PUTCH(STARTCOL+2,STARTROW,V_CHAR);
		}
	    else return(0);
	break;
	default : printf("illegal piece Type=%d!!\n",Type); exit();
	}
refresh();
Row=STARTROW; Column=STARTCOL;	/* all pieces start at same point */
}
