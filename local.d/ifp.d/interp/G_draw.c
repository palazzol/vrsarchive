
/****** G_draw.c ******************************************************/
/**                                                                  **/
/**                    University of Illinois                        **/
/**                                                                  **/
/**                Department of Computer Science                    **/
/**                                                                  **/
/**   Tool: IFP      			Version: 0.5		     **/
/**                                                                  **/
/**   Author:  Arch D. Robison          Date:   May 1, 1985          **/
/**                                                                  **/
/**   Revised by: Arch D. Robison	Date: June 18, 1986	     **/
/**                                                                  **/
/**   Principal Investigators: Prof. R. H. Campbell                  **/
/**                            Prof. W. J. Kubitz                    **/
/**                                                                  **/
/**                                                                  **/
/**------------------------------------------------------------------**/
/**   (C) Copyright 1987  University of Illinois Board of Trustees   **/
/**                       All Rights Reserved.                       **/
/**********************************************************************/

/******************** FP Graphics Interface Module ********************/

/*
 * The fp interpreter must be compiled with the -DGRAPHICS option to use
 * the graphics interface.  The interface is specific to the PC/RT.
 *
 * There are no graphics primitives in FP itself, rather FP is used to
 * calculate a display list.  The display list is then fed to DrawObject,
 * which draws the picture specified by the display list.
 *
 * The display list has the following structure:
 *
 *     display-list == < {display-list} > | polyline | color | transform | text
 *     polyline == < "line" { < x y > } >
 *     color == < "color" color-index display-list >
 *     text == <"text" print-atom size ["center"]>
 *     transform = <"trans" t-matrix display-list >
 *     t-matrix = <<Txx Txy Txo> <Tyx Tyy Tyo>>
 *
 * The polyline structure specifies a sequence of points.  Adjacent points
 * are connected with line segments.
 *
 * The color structure draws the display-list in the color specified by
 * the color index (0..15).  The color applies to all parts of the
 * subordinate display-list which are not subordinate to a color structure
 * within.  
 *
 * The transform structure draws the display-list as transformed by the
 * t-matrix.  Transforms may be nested.
 *
 * The text structure draws a string with the lower-left corner at (0,0).
 * Each character is drawn in a 1.0 by 1.0 box (including spacing).
 */

#include <stdio.h>
#include "struct.h"
#include "string.h"

#define NKey 4
StrPtr SKey[4],SCenter;

private short ScreenDim[2];

private void GraphError (InOut,Message)
   ObjectPtr InOut;
   char *Message;
   {
      VI_Term ();
      printf ("%s\n",Message);
      OutObject (InOut);
      printf ("\n");
      RepTag (InOut,BOTTOM);
   }

void InitDraw ()
   {
      printf (" (RT/PC graphics)");
      fflush (stdout);
      SKey[0] = MakeString ("line");
      SKey[1] = MakeString ("trans");
      SKey[2] = MakeString ("color");
      SKey[3] = MakeString ("text");
      SCenter = MakeString ("center");
   }

typedef double Transform [2][3];

Transform TransDefault = {
   {800,0,0},
   {0,800,0}
};

forward void DrOb(), DrawText(), DrawTrans(), DrawColor (), PolyLine();

void DrawObject (InOut)
   ObjectPtr InOut;
   {
      if (InOut->Tag == BOTTOM) return;

      VI_Init(ScreenDim,ScreenDim+1);
      VI_Force();
      VI_Color (0);
      VI_Tile (ScreenDim[0],ScreenDim[1],1,1,"\0");
      DrOb (InOut,TransDefault,1);
      if (InOut->Tag != BOTTOM) {
	 while (getchar () != '\n') continue;
	 VI_Term ();
      }
   }

/*
 * DrOb
 *
 * Draw object Inout with transform T and in color Color.
 */
private void DrOb (InOut,T,Color)
   register ObjectPtr InOut;
   Transform T;
   int Color;
   {
      register int K;
      register ListPtr P;

      if (InOut->Tag != LIST)
	 GraphError (InOut,"DrOb: invalid display object");
      else {
	 P = InOut->List;
	 if (P == NULL || P->Val.Tag == LIST)
	    for (; P!=NULL; P=P->Next) DrOb (&P->Val,T,Color);
	 else if (P->Val.Tag != STRING)
	    GraphError (InOut,"DrOb: first element must be string");
	 else {
	    for (K=0; K<NKey; K++)
	       if (!StrComp (SKey [K],P->Val.String)) break;
	    switch (K) {
	       case 0:
		  PolyLine (P->Next,T,Color);
		  break;
	       case 1:
		  DrawTrans (P->Next,T,Color);
		  break;
	       case 2:
		  DrawColor (P->Next,T);
		  break;
	       case 3:
		  DrawText (P->Next,T,Color);
		  break;
	       default:
		  GraphError (InOut,"DrOb: unknown drawing command");
		  break;
	    }
	 }
      }
   }

private void GetCoor (P,T,X,Y)
   register ListPtr P;
   Transform T;
   int *X,*Y;
   {
      extern short sddmul ();
      register ListPtr Q;
      double Xf,Yf;

      if (!PairTest (&P->Val,NUMERIC,NUMERIC))
	 GraphError (&P->Val,"GetCoor: numeric pair expected\n");
      else {
	 Q = P->Val.List;
	 GetDouble (&Q->Val,&Xf);
	 GetDouble (&Q->Next->Val,&Yf);
	 *X = Xf * T[0][0] + Yf * T[0][1] + T[0][2];
	 *Y = Xf * T[1][0] + Yf * T[1][1] + T[1][2];
      }
   }

private void DrawText (P,T,Color)
   register ListPtr P;
   Transform T;
   int Color;
   {
      char Buf[256];
      CharPtr U;
      int S[2][3];
      int i,j,N3;
      double Size;
      boolean Center;

      if (P!=NULL) {
	 switch (P->Val.Tag) {
	    default: return;
	    case STRING:
	       CPInit (&U,&P->Val.List);
	       (void) CPRead (&U,Buf,256);
	       break;
	    case BOOLEAN:
	       (void) sprintf (Buf,P->Val.Bool ? "t" : "f");
	       break;
	    case INT:
	       (void) sprintf (Buf,"%ld",P->Val.Int);
	       break;
	    case FLOAT:
	       (void) sprintf (Buf,"%g",P->Val.Float);
	       break;
	 }
	 Size = 1.0;
	 Center = 0;
	 if (NULL != (P=P->Next)) {
	    GetDouble (&P->Val,&Size);
	
	    if (NULL != (P=P->Next) && P->Val.Tag == STRING &&
		!StrComp (P->Val.String,SCenter)) {
	       Center = 1;
	       N3 = 3*strlen (Buf);
	    }
	 }

	 Size /= 6.0;

	 for (i=0; i<2; i++)
	    for (j=0; j<3; j++)
	       S[i][j] = (int) ((j<2 ? Size * T[i][j] : T[i][j]) * (1 << 10));
	
	 if (Center) {
	    S[0][2] -= N3 * S[0][0] + 3 * S[0][1];
	    S[1][2] -= N3 * S[1][0] + 3 * S[1][1];
	 }

	 ConSymbol (Buf,S);
      }
   }

private void PolyLine (P,T,Color)
   register ListPtr P;
   Transform T;
   int Color;
   {
      int X,Y;

      VI_Color (!Color);
      if (P != NULL) {
	 GetCoor (P,T,&X,&Y);
	 VI_AMove (X,Y);
	 while (NULL != (P=P->Next)) {
	    GetCoor (P,T,&X,&Y);
	    VI_ALine (X,Y);
	 }
      }
   }

boolean GetTrans (X,T)
   ObjectPtr X;
   Transform T;
   {
      register ListPtr P,Q;
      register int i,j;

      if (!PairTest (X,1<<LIST,1<<LIST)) return 0;
      else {
	 P = X->List;
	 for (i=0; i<2; P=P->Next,i++) {
	    Q = P->Val.List;
	    for (j=0; j<3; Q=Q->Next,j++) {
	       if (Q == NULL) return 0;
	       if (GetDouble (&Q->Val,&T[i][j])) return 0;
	    }
	 }
	 return 1;
      }
   }

private void DrawTrans (P,T,Color)
   register ListPtr P;
   Transform T;
   int Color;
   {
      Transform R,S;
      int i,j;

      if (P!=NULL)
	 if (!GetTrans (&P->Val,R))
	    GraphError (&P->Val,"DrawTrans: not a transform");
	 else {
	    for (i=0; i<2; i++) {
	       for (j=0; j<3; j++)
		  S[i][j] = T[i][0] * R[0][j] + T[i][1] * R[1][j];
	       S[i][2] += T[i][2];
	    }
	 }
	 if (NULL != (P=P->Next)) DrOb (&P->Val,S,Color);
   }

private void DrawColor (P,T)
   register ListPtr P;
   Transform T;
   {
      int Color;

      if (P!=NULL) {
	 switch (P->Val.Tag) {
	    case INT:
	       Color = P->Val.Int;
	       break;
	    case FLOAT:
	       Color = (int) (P->Val.Float + 0.5);
	       break;
	    default:
	       GraphError (&P->Val,"DrawColor: not a color");
	 }
	 if (P->Next != NULL) DrOb (&P->Next->Val,T,Color);
      }
   }

/***************************** end of G_draw.c *******************************/

