
#include <curses.h>
#include "tet.h"
/*********************************************************************/
/* Switch on type of piece, find out if I can rotate */
/* If so, then do it */
/*********************************************************************/
Rotate()
{
switch (Type) {
	/*  WHITE PIECES  */
	case W_TYPE   :  /* checked */
		if (IS_FREE(Column,Row-1)) {
			PUTCH(Column-1,Row,NO_CHAR);
			PUTCH(Column,Row-1,W_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case W_TYPE-1 :  /* checked */
		if (IS_FREE(Column-1,Row)) {
			PUTCH(Column,Row+1,NO_CHAR);
			PUTCH(Column-1,Row,W_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case W_TYPE-2 :  /* checked */
		if (IS_FREE(Column,Row+1)) {
			PUTCH(Column+1,Row,NO_CHAR);
			PUTCH(Column,Row+1,W_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case W_TYPE-3 :  /* checked */
		if (IS_FREE(Column+1,Row)) {
			PUTCH(Column,Row-1,NO_CHAR);
			PUTCH(Column+1,Row,W_CHAR);
			Type = W_TYPE;
			}
		else goto beepout;
		goto out;

	/*  RED PIECES  */
	case R_TYPE   : /* checked */
		if (IS_FREE(Column,Row-1) && IS_FREE(Column,Row+1) && 
		    IS_FREE(Column+1,Row+1)) {
			PUTCH(Column-1,Row,NO_CHAR);
			PUTCH(Column-1,Row+1,NO_CHAR);
			PUTCH(Column+1,Row,NO_CHAR);
			PUTCH(Column,Row-1,R_CHAR);
			PUTCH(Column,Row+1,R_CHAR);
			PUTCH(Column+1,Row+1,R_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case R_TYPE-1 : /* checked */
		if (IS_FREE(Column-1,Row) && IS_FREE(Column+1,Row-1) && 
		    IS_FREE(Column+1,Row)) {
			PUTCH(Column,Row-1,NO_CHAR);
			PUTCH(Column,Row+1,NO_CHAR);
			PUTCH(Column+1,Row+1,NO_CHAR);
			PUTCH(Column-1,Row,R_CHAR);
			PUTCH(Column+1,Row-1,R_CHAR);
			PUTCH(Column+1,Row,R_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case R_TYPE-2 : /* checked  */
		if (IS_FREE(Column-1,Row-1) && IS_FREE(Column,Row-1) && 
		    IS_FREE(Column,Row+1)) {
			PUTCH(Column-1,Row,NO_CHAR);
			PUTCH(Column+1,Row-1,NO_CHAR);
			PUTCH(Column+1,Row,NO_CHAR);
			PUTCH(Column-1,Row-1,R_CHAR);
			PUTCH(Column,Row-1,R_CHAR);
			PUTCH(Column,Row+1,R_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case R_TYPE-3 : /* checked */
		if (IS_FREE(Column-1,Row) && IS_FREE(Column-1,Row+1) && 
		    IS_FREE(Column+1,Row)) {
			PUTCH(Column-1,Row-1,NO_CHAR);
			PUTCH(Column,Row-1,NO_CHAR);
			PUTCH(Column,Row+1,NO_CHAR);
			PUTCH(Column-1,Row,R_CHAR);
			PUTCH(Column-1,Row+1,R_CHAR);
			PUTCH(Column+1,Row,R_CHAR);
			Type = R_TYPE;
			}
		else goto beepout;
		goto out;

	/*  TAN PIECES  */
	case T_TYPE   :
	case T_TYPE-1 :
	case T_TYPE-2 :
	case T_TYPE-3 : goto out;

	/*  YELLOW PIECES  */
	case Y_TYPE   :
	case Y_TYPE-2 :  /* checked */
		if (IS_FREE(Column,Row-1) && IS_FREE(Column+1,Row+1)) {
			PUTCH(Column-1,Row+1,NO_CHAR);
			PUTCH(Column,Row+1,NO_CHAR);
			PUTCH(Column,Row-1,Y_CHAR);
			PUTCH(Column+1,Row+1,Y_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case Y_TYPE-1 :
	case Y_TYPE-3 : /* checked */
		if (IS_FREE(Column-1,Row+1) && IS_FREE(Column,Row+1)) {
			PUTCH(Column,Row-1,NO_CHAR);
			PUTCH(Column+1,Row+1,NO_CHAR);
			PUTCH(Column-1,Row+1,Y_CHAR);
			PUTCH(Column,Row+1,Y_CHAR);
			Type = Y_TYPE;
			}
		else goto beepout;
		goto out;

	/*  GREEN PIECES  */
	case G_TYPE   :
	case G_TYPE-2 : /* checked */
		if (IS_FREE(Column-1,Row+1) && IS_FREE(Column,Row-1)) {
			PUTCH(Column,Row+1,NO_CHAR);
			PUTCH(Column+1,Row+1,NO_CHAR);
			PUTCH(Column-1,Row+1,G_CHAR);
			PUTCH(Column,Row-1,G_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case G_TYPE-1 :
	case G_TYPE-3 : /* checked */
		if (IS_FREE(Column,Row+1) && IS_FREE(Column+1,Row+1)) {
			PUTCH(Column-1,Row+1,NO_CHAR);
			PUTCH(Column,Row-1,NO_CHAR);
			PUTCH(Column,Row+1,G_CHAR);
			PUTCH(Column+1,Row+1,G_CHAR);
			Type = G_TYPE;
			}
		else goto beepout;
		goto out;


	/*  BLUE PIECES  */
	case B_TYPE   : /* checked */
		if (IS_FREE(Column,Row-1) && IS_FREE(Column,Row+1) && 
		    IS_FREE(Column+1,Row-1)) {
			PUTCH(Column-1,Row,NO_CHAR);
			PUTCH(Column+1,Row,NO_CHAR);
			PUTCH(Column+1,Row+1,NO_CHAR);
			PUTCH(Column,Row-1,B_CHAR);
			PUTCH(Column,Row+1,B_CHAR);
			PUTCH(Column+1,Row-1,B_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case B_TYPE-1 : /* checked */
		if (IS_FREE(Column-1,Row-1) && IS_FREE(Column-1,Row) && 
		    IS_FREE(Column+1,Row)) {
			PUTCH(Column,Row-1,NO_CHAR);
			PUTCH(Column,Row+1,NO_CHAR);
			PUTCH(Column+1,Row-1,NO_CHAR);
			PUTCH(Column-1,Row-1,B_CHAR);
			PUTCH(Column-1,Row,B_CHAR);
			PUTCH(Column+1,Row,B_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case B_TYPE-2 : /* checked  */
		if (IS_FREE(Column-1,Row+1) && IS_FREE(Column,Row-1) && 
		    IS_FREE(Column,Row+1)) {
			PUTCH(Column-1,Row-1,NO_CHAR);
			PUTCH(Column-1,Row,NO_CHAR);
			PUTCH(Column+1,Row,NO_CHAR);
			PUTCH(Column-1,Row+1,B_CHAR);
			PUTCH(Column,Row-1,B_CHAR);
			PUTCH(Column,Row+1,B_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case B_TYPE-3 : /* checked */
		if (IS_FREE(Column-1,Row) && IS_FREE(Column+1,Row) && 
		    IS_FREE(Column+1,Row+1)) {
			PUTCH(Column-1,Row+1,NO_CHAR);
			PUTCH(Column,Row-1,NO_CHAR);
			PUTCH(Column,Row+1,NO_CHAR);
			PUTCH(Column-1,Row,B_CHAR);
			PUTCH(Column+1,Row,B_CHAR);
			PUTCH(Column+1,Row+1,B_CHAR);
			Type = B_TYPE;
			}
		else goto beepout;
		goto out;

	/*  VIOLET PIECES  */
	case V_TYPE   :
	case V_TYPE-2 : /* checked */
		if (IS_FREE(Column,Row-1) && IS_FREE(Column,Row+1) && 
		    IS_FREE(Column,Row+2)) {
			PUTCH(Column-1,Row,NO_CHAR);
			PUTCH(Column+1,Row,NO_CHAR);
			PUTCH(Column+2,Row,NO_CHAR);
			PUTCH(Column,Row-1,V_CHAR);
			PUTCH(Column,Row+1,V_CHAR);
			PUTCH(Column,Row+2,V_CHAR);
			Type--;
			}
		else goto beepout;
		goto out;
	case V_TYPE-1 :
	case V_TYPE-3 : /* checked */
		if (IS_FREE(Column-1,Row) && IS_FREE(Column+1,Row) && 
		    IS_FREE(Column+2,Row)) {
			PUTCH(Column,Row-1,NO_CHAR);
			PUTCH(Column,Row+1,NO_CHAR);
			PUTCH(Column,Row+2,NO_CHAR);
			PUTCH(Column-1,Row,V_CHAR);
			PUTCH(Column,Row,V_CHAR);
			PUTCH(Column+1,Row,V_CHAR);
			PUTCH(Column+2,Row,V_CHAR);
			Type = V_TYPE;
			}
		else goto beepout;
		goto out;
	default : 
		printf("illegal piece Type=%d!!\n",Type); 
		exit();
	}
beepout:
	if (Beep) beep();
out:
	refresh();
}

