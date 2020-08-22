
#include <curses.h>
#include "tet.h"
/*********************************************************************/
/* Switch on type of piece, find out if I can move down */
/* If so, then do it and return 1 else return 0 */
/*********************************************************************/
AdvancePiece()
{
switch (Type) {
	/*  WHITE PIECES  */
	case W_TYPE	: 
		    if (IS_FREE(Column-1,Row+1) && 
			IS_FREE(Column,Row+2) &&
			IS_FREE(Column+1,Row+1)) 
				{
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column-1,Row+1,W_CHAR);
				PUTCH(Column,Row+2,W_CHAR);
				PUTCH(Column+1,Row+1,W_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case W_TYPE-1 :
		    if (IS_FREE(Column,Row+2) && 
			IS_FREE(Column+1,Row+1))
				{
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column,Row+2,W_CHAR);
				PUTCH(Column+1,Row+1,W_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case W_TYPE-2 :  
		    if (IS_FREE(Column-1,Row+1) && 	
			IS_FREE(Column,Row+1) && 
			IS_FREE(Column+1,Row+1))
				{
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column-1,Row+1,W_CHAR);
				PUTCH(Column,Row+1,W_CHAR);
				PUTCH(Column+1,Row+1,W_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case W_TYPE-3 :  
		    if (IS_FREE(Column-1,Row+1) && 	
			IS_FREE(Column,Row+2))
				{
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column-1,Row+1,W_CHAR);
				PUTCH(Column,Row+2,W_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;


	/*  RED PIECES  */
	case R_TYPE   : 
		    if (IS_FREE(Column-1,Row+2) && 
			IS_FREE(Column,Row+1) &&
			IS_FREE(Column+1,Row+1))
				{
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column-1,Row+2,R_CHAR);
				PUTCH(Column,Row+1,R_CHAR);
				PUTCH(Column+1,Row+1,R_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case R_TYPE-1 : 
		    if (IS_FREE(Column,Row+2) && 
			IS_FREE(Column+1,Row+2))
				{
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column+1,Row+1,NO_CHAR);
				PUTCH(Column,Row+2,R_CHAR);
				PUTCH(Column+1,Row+2,R_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case R_TYPE-2 : 
		    if (IS_FREE(Column-1,Row+1) && 
			IS_FREE(Column,Row+1) &&
			IS_FREE(Column+1,Row+1))
				{
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row-1,NO_CHAR);
				PUTCH(Column-1,Row+1,R_CHAR);
				PUTCH(Column,Row+1,R_CHAR);
				PUTCH(Column+1,Row+1,R_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case R_TYPE-3 : 
		    if (IS_FREE(Column-1,Row) && 
			IS_FREE(Column,Row+2))
				{
				PUTCH(Column-1,Row-1,NO_CHAR);
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column-1,Row,R_CHAR);
				PUTCH(Column,Row+2,R_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;


	/*  TAN PIECES  */
	case T_TYPE   :
	case T_TYPE-1 :
	case T_TYPE-2 :
	case T_TYPE-3 :
		    if (IS_FREE(Column,Row+2) && 
			IS_FREE(Column+1,Row+2))
				{
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column,Row+2,T_CHAR);
				PUTCH(Column+1,Row+2,T_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;


	/*  YELLOW PIECES  */
	case Y_TYPE   :
	case Y_TYPE-2 :  
		    if (IS_FREE(Column-1,Row+2) && 
			IS_FREE(Column,Row+2) &&
			IS_FREE(Column+1,Row+1))
				{
				PUTCH(Column-1,Row+1,NO_CHAR);
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column-1,Row+2,Y_CHAR);
				PUTCH(Column,Row+2,Y_CHAR);
				PUTCH(Column+1,Row+1,Y_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case Y_TYPE-1 :
	case Y_TYPE-3 : 
		    if (IS_FREE(Column,Row+1) && 
			IS_FREE(Column+1,Row+2))
				{
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column,Row+1,Y_CHAR);
				PUTCH(Column+1,Row+2,Y_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;


	/*  GREEN PIECES  */
	case G_TYPE   :
	case G_TYPE-2 : 
		    if (IS_FREE(Column-1,Row+1) && 
			IS_FREE(Column,Row+2) &&
			IS_FREE(Column+1,Row+2))
				{
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row+1,NO_CHAR);
				PUTCH(Column-1,Row+1,G_CHAR);
				PUTCH(Column,Row+2,G_CHAR);
				PUTCH(Column+1,Row+2,G_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case G_TYPE-1 :
	case G_TYPE-3 : 
		    if (IS_FREE(Column-1,Row+2) && 
			IS_FREE(Column,Row+1))
				{
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row+1,G_CHAR);
				PUTCH(Column-1,Row+2,G_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;


	/*  BLUE PIECES  */
	case B_TYPE   : 
		    if (IS_FREE(Column-1,Row+1) && 
			IS_FREE(Column,Row+1) &&
			IS_FREE(Column+1,Row+2))
				{
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column-1,Row+1,B_CHAR);
				PUTCH(Column,Row+1,B_CHAR);
				PUTCH(Column+1,Row+2,B_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case B_TYPE-1 : 
		    if (IS_FREE(Column,Row+2) && 
			IS_FREE(Column+1,Row))
				{
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column+1,Row-1,NO_CHAR);
				PUTCH(Column,Row+2,B_CHAR);
				PUTCH(Column+1,Row,B_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case B_TYPE-2 : 
		    if (IS_FREE(Column-1,Row+1) && 
			IS_FREE(Column,Row+1) &&
			IS_FREE(Column+1,Row+1))
				{
				PUTCH(Column-1,Row-1,NO_CHAR);
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column-1,Row+1,B_CHAR);
				PUTCH(Column,Row+1,B_CHAR);
				PUTCH(Column+1,Row+1,B_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case B_TYPE-3 : 
		    if (IS_FREE(Column-1,Row+2) && 
			IS_FREE(Column,Row+2))
				{
				PUTCH(Column-1,Row+1,NO_CHAR);
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column-1,Row+2,B_CHAR);
				PUTCH(Column,Row+2,B_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;


	/*  VIOLET PIECES  */
	case V_TYPE   :
	case V_TYPE-2 : 
		    if (IS_FREE(Column-1,Row+1) && 
			IS_FREE(Column,Row+1) && 
			IS_FREE(Column+1,Row+1) && 
			IS_FREE(Column+2,Row+1))
				{
				PUTCH(Column-1,Row,NO_CHAR);
				PUTCH(Column,Row,NO_CHAR);
				PUTCH(Column+1,Row,NO_CHAR);
				PUTCH(Column+2,Row,NO_CHAR);
				PUTCH(Column-1,Row+1,V_CHAR);
				PUTCH(Column,Row+1,V_CHAR);
				PUTCH(Column+1,Row+1,V_CHAR);
				PUTCH(Column+2,Row+1,V_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	case V_TYPE-1 :
	case V_TYPE-3 : 
		    if (IS_FREE(Column,Row+3)) 
				{
				PUTCH(Column,Row-1,NO_CHAR);
				PUTCH(Column,Row+3,V_CHAR);
				Row +=1;
				goto out;
				}
			else goto badout;

	default : 
		printf("Advance Piece: illegal piece Type=%d!!\n",Type); 
		exit(1);
	}
badout:
	return(0);
out:
	refresh();
	return(1);
}
