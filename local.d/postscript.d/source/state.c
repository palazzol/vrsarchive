/*
 * Copyright (C) Rutherford Appleton Laboratory 1987
 * 
 * This source may be copied, distributed, altered or used, but not sold for profit
 * or incorporated into a product except under licence from the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to the source
 * made by persons other than the author should be marked as such.
 * 
 *	Crispin Goswell @ Rutherford Appleton Laboratory caag@uk.ac.rl.vd
 */
#include "main.h"
#include "graphics.h"

extern int InitGraphics ();
int ErasePage ();
static int CopyPage ();

static int SetLineWidth ();
static int GetLineWidth ();
static int SetLineCap ();
static int GetLineCap ();
static int SetLineJoin ();
static int GetLineJoin ();
static int SetDash ();
static int GetDash ();
static int SetFlat ();
static int GetFlat ();
static int SetMiterLimit ();
static int GetMiterLimit ();
static int SetHSB ();
static int GetHSB ();
static int SetRGB ();
static int GetRGB ();

static int PSetRealScreen ();
static int PBuildScreen ();
static int PScreenSize ();
static int GetScreen ();

static int PSetRealTransfer ();
static int PGetTransfer ();
static int PTransferSize ();

static int SetGray ();
static int GetGray ();

static int SetFillMethod ();
static int GetFillMethod ();
static int SetStrokeMethod ();
static int GetStrokeMethod ();

int fill_method = 1, stroke_method = 0;

InitState ()
 {
  	InstallOp ("erasepage",		ErasePage,	0, 0, 0, 0);
 	InstallOp ("copypage",		CopyPage,	0, 0, 0, 0);
	InstallOp ("setlinewidth",	SetLineWidth,	1, 0, 0, 0, Float);
 	InstallOp ("currentlinewidth",	GetLineWidth,	0, 1, 0, 0);
 	InstallOp ("setlinecap",	SetLineCap,	1, 0, 0, 0, Integer);
 	InstallOp ("currentlinecap",	GetLineCap,	0, 1, 0, 0);
 	InstallOp ("setlinejoin",	SetLineJoin,	1, 0, 0, 0, Integer);
 	InstallOp ("currentlinejoin",	GetLineJoin,	0, 1, 0, 0);
 	InstallOp ("setdash",		SetDash,	2, 0, 0, 0, Array, Float);
 	InstallOp ("currentdash",	GetDash,	0, 2, 0, 2);
 	InstallOp ("setflat",		SetFlat,	1, 0, 0, 0, Float);
 	InstallOp ("currentflat",	GetFlat,	0, 1, 0, 0);
 	InstallOp ("setmiterlimit",	SetMiterLimit,	1, 0, 0, 0, Float);
 	InstallOp ("currentmiterlimit",	GetMiterLimit,	0, 1, 0, 0);
 	
 	InstallOp ("setgray",		SetGray,	1, 0, 0, 0, Float);
 	InstallOp ("currentgray",	GetGray,	0, 1, 0, 0);
 	InstallOp ("sethsbcolor",	SetHSB,		3, 0, 0, 0, Float, Float, Float);
 	InstallOp ("currenthsbcolor",	GetHSB,		0, 3, 0, 0);
 	InstallOp ("setrgbcolor",	SetRGB,		3, 0, 0, 0, Float, Float, Float);
 	InstallOp ("currentrgbcolor",	GetRGB,		0, 3, 0, 0);
 	
  	InstallOp ("screensize",	PScreenSize,	2, 1, 0, 0, Float, Float);
  	InstallOp ("buildscreen",	PBuildScreen,	2, 2, 0, 0, Float, Float);
  	InstallOp ("setrealscreen",	PSetRealScreen,	4, 0, 0, 0, Float, Float, Array, Array);
	InstallOp ("currentscreen",	GetScreen,	0, 3, 0, 0);
	
 	InstallOp ("setrealtransfer",	PSetRealTransfer,	2, 0, 0, 0, Array, Array);
 	InstallOp ("currenttransfer",	PGetTransfer,	0, 1, 0, 0);
 	InstallOp ("transfersize",	PTransferSize,	0, 1, 0, 0);
 	
 	InstallOp ("setfillmethod",	SetFillMethod,	1, 0, 0, 0, Integer);
 	InstallOp ("currentfillmethod",	GetFillMethod,	0, 1, 0, 0);
 	InstallOp ("setstrokemethod",	SetStrokeMethod,	1, 0, 0, 0, Integer);
 	InstallOp ("currentstrokemethod",GetStrokeMethod,	0, 1, 0, 0);
 	InstallOp ("initgraphics",	InitGraphics,	0, 0, 0, 0);

	VOID InitGraphics ();
 }

int InitGraphics ()
 {
 	VOID PInitMatrix ();		/* initmatrix */
 	VOID InitClip ();		/* initclip */
 	VOID PNewPath ();		/* newpath */
 	
 	gstate->colour = NewColour (0.0, 0.0, 0.0);	/* 0 setgray */
 	gstate->line_width = 1;		/* 1 setlinewidth */
 	gstate->line_cap = CBUTT;	/* setlinecap */
 	gstate->line_join = JMITRE;	/* setlinejoin */
 	gstate->dash_length = 0;	/* [] 0 setdash */
 	gstate->dash_offset = 0;	
 	gstate->miter_limit = 10;	/* 10 setmiterlimit */
 	InitShowContext ();
 	
 	return TRUE;
 }

int ErasePage ()
 {
	Paint (NULL, gstate->device->dev,
		NewDevicePoint (0, 0), NewDevicePoint (0, 0),
		HardwareExtent (gstate->device->dev),
		White);
 	return TRUE;
 }

static int CopyPage ()
 {
 	return TRUE;
 }

static int SetLineWidth (width) Object width;
 {
 	gstate->line_width = BodyReal (width);
 	return TRUE;
 }

static int GetLineWidth ()
 {
 	return Push (OpStack, MakeReal (gstate->line_width));
 }

static int SetLineCap (cap) Object cap;
 {
 	int v = BodyInteger (cap);
 	
 	if (v < 0 || v > 2)
 		return Error (PRangeCheck);
 	gstate->line_cap = v;
 	return TRUE;
 }

static int GetLineCap ()
 {
 	return Push (OpStack, MakeInteger (gstate->line_cap));
 }

static int SetLineJoin (join) Object join;
 {
 	int v = BodyInteger (join);
 	
 	if (v < 0 || v > 2)
 		return Error (PRangeCheck);
 	gstate->line_join = v;
 	return TRUE;
 }

static int GetLineJoin ()
 {
  	return Push (OpStack, MakeInteger (gstate->line_join));
 }

static int SetDash (array, offset) Object array, offset;
 {
 	Object *v = BodyArray (array);
	int i, length = lengthArray (array);
	float sum = 0;
	
 	if (length > MAXDASH)
  		return Error (PLimitCheck);
  	if (length == 0)
  	 {
  	 	gstate->dash_length = 0;
  	 	return TRUE;
  	 }
  	for (i = 0; i < length; i++)
  	 {
  	 	float val;
  	 	
  		if (TypeOf (v[i]) == Real)
  			val = BodyReal(v[i]);
  		else if (TypeOf (v[i]) == Integer)
  			val = (float) BodyInteger (v[i]);
  		else
    			return Error (PLimitCheck);
 	 	if (val < 0)
 	 		return Error (PRangeCheck);
 	 	sum += val;
    	 }
  	if (sum == 0)
 		return Error (PRangeCheck);
   	 
 	gstate->dash_length = length;
 	gstate->dash_offset = BodyReal (offset);
 	for (i = 0; i < length; i++)
 	 {
	 	if (TypeOf (v[i]) == Integer)
 	 		gstate->dash_array [i] = BodyInteger (v[i]);
 	 	else
 	 		gstate->dash_array [i] = BodyReal (v[i]);
 	 }
 	return TRUE;
 }

static int GetDash ()
 {
 	int i;
 	if (!OpCheck (0, gstate->dash_length + 1))
 		return FALSE;
 	VOID Push (OpStack, Marker);
 	for (i = 0; i < gstate->dash_length; i++)
 		VOID Push (OpStack, MakeReal (gstate->dash_array[i]));
 	VOID Push (ExecStack, MakeReal (gstate->dash_offset));
 	VOID Push (ExecStack, Cvx (Rbracket));
 	return TRUE;
 }

static int SetFlat (flat) Object flat;
 {
 	gstate->flatness = BodyReal (flat);
 	return TRUE;
 }

static int GetFlat ()
 {
 	return Push (OpStack, MakeInteger (gstate->flatness));
 }

static int SetMiterLimit (miter) Object miter;
 {
 	float m = BodyReal (miter);
 	
 	if (m < 1)
 		return Error (PRangeCheck);
 	gstate->miter_limit = m;
 	return TRUE;
 }

static int GetMiterLimit ()
 {
 	return Push (OpStack, MakeReal (gstate->miter_limit));
 }

static int SetGray (gray) Object gray;
 {
 	float g = BodyReal (gray);
 	
 	if (g < 0 || g > 1)
 		return Error (PRangeCheck);
	gstate->colour = NewGray (g);
 	
	return TRUE;
 }

static int GetGray ()
 {
 	return Push (OpStack, MakeReal (Brightness (gstate->colour)));
 }

static int SetHSB (hue, sat, bright) Object hue, sat, bright;
 {
 	float h = BodyReal (hue), s = BodyReal (sat), b = BodyReal (bright);
 	if (h < 0 || h > 1 || s < 0 || s > 1 || b < 0 || b > 1)
 		return Error (PRangeCheck);
 	gstate->colour = NewHSBColour (h, s, b);
 	return TRUE;
 }

static int SetRGB (red, green, blue) Object red, green, blue;
 {
 	float R = BodyReal (red), G = BodyReal (green), B = BodyReal (blue);
 	
 	if (R < 0 || R > 1 || G < 0 || G > 1 || B < 0 || B > 1)
 		return Error (PRangeCheck);
 	
 	gstate->colour = NewRGBColour (R, G, B);
 	return TRUE;
 }

static int GetHSB ()
 {
 	float h, s, b;
 	
 	ColourHSB (gstate->colour, &h, &s, &b);
 	VOID Push (OpStack, MakeReal (h));
 	VOID Push (OpStack, MakeReal (s));
 	VOID Push (OpStack, MakeReal (b));
 	
 	return TRUE;
 }

static int GetRGB ()
 {
 	float R, G, B;
	
	ColourRGB (gstate->colour, &R, &G, &B);
 	VOID Push (OpStack, MakeReal (R));
 	VOID Push (OpStack, MakeReal (G));
 	VOID Push (OpStack, MakeReal (B));
 	
	return TRUE;
 }

static int PScreenSize (freq, rot) Object freq, rot;
 {
 	return Push (OpStack, MakeInteger (ScreenSize (BodyReal (freq), BodyReal (rot))));
 }

static int PBuildScreen (freq, rot) Object freq, rot;
 {
 	float f = BodyReal (freq), r = BodyReal (rot);
 	int i, ss = ScreenSize (f, r);
 	float	*x = (float *) Malloc (sizeof (float) * ss),
 		*y = (float *) Malloc (sizeof (float) * ss);
 	
 	Object *px = (Object *) Malloc (sizeof (Object) * ss),
  		*py = (Object *) Malloc (sizeof (Object) * ss);
	
 	BuildScreen (f, r, x, y);
 	
 	for (i = 0; i < ss; i++)
 	 {
 	 	px [i] = MakeReal (x[i]);
 	 	py [i] = MakeReal (y[i]);
 	 }
 	Free ((char *) x);
 	Free ((char *) y);
 	
 	return Push (OpStack, MakeArray (px, ss)) && Push (OpStack, MakeArray (py, ss));
 }

static int PSetRealScreen (freq, rot, spot, thresh) Object freq, rot, spot, thresh;
 {
 	float f = BodyReal (freq), r = BodyReal (rot);
 	int i, ss = ScreenSize (f, r);
 	float *th = (float *) Malloc (sizeof (float) * ss);
 	
 	gstate->screen.frequency 	= f;
 	gstate->screen.rotation 	= r;
 	gstate->screen.spot_function 	= spot;
 	
 	if (lengthArray (thresh) != ss)
 	 {
 	 	free ((char *) th);
 	 	return Error (PRangeCheck);
 	 }
 	
 	for (i = 0; i < ss; i++)
 	 {
 	 	Object a;
 	 	
 	 	a = getArray (thresh, i);
 	 	if (TypeOf (a) != Real)
 	 	 {
 	 	 	Free ((char *) th);
 	 		return Error (PTypeCheck);
 	 	 }
 	 	
 		th[i] = BodyReal (a);
 	 }
 	if (gstate->screen.count == 1)	/* allows 0 to mean 'not assigned yet' */
 		Free ((char *) gstate->screen.thresh);
 	gstate->screen.thresh = th;
 	gstate->screen.count = 1;
 	
 	SetScreen (f, r, th);
 	
 	return TRUE;
 }

static int GetScreen ()
 {
 	VOID Push (OpStack, MakeReal (gstate->screen.frequency));
 	VOID Push (OpStack, MakeReal (gstate->screen.rotation));
 	VOID Push (OpStack, gstate->screen.spot_function);
 	return TRUE;
 }

static int PSetRealTransfer (transfer, values) Object transfer, values;
 {
 	Object v;
 	int i, size = TransferSize ();
 	float *val;
 	
 	if (lengthArray (values) != size)
 		return Error (PRangeCheck);
 	
 	val = (float *) Malloc (sizeof (float) * size);
 	for (i = 0; i < size; i++)
 	 {
 	 	v = getArray (values, i);
 	 	if (TypeOf (v) == Real)
 	 		val [i] = BodyReal (v);
 	 	else
 	 	 {
 	 	 	Free ((char *) val);
 	 	 	return Error (PTypeCheck);
 	 	 }
 	 }
 	
 	if (gstate->transfer.tcount == 1)
 	 	Free ((char *) gstate->transfer.tran);
 	gstate->transfer.transfn = transfer;
 	gstate->transfer.tran = val;
 	gstate->transfer.tcount = 1;
 	
 	SetTransfer (val);
 	
 	return TRUE;
 }

static int PGetTransfer ()
 {
 	return Push (OpStack, gstate->transfer.transfn);
 }

static int PTransferSize ()
 {
	return Push (OpStack, MakeInteger (TransferSize ()));
 }

static int SetFillMethod (method) Object method;
 {
 	fill_method = BodyInteger (method);
 	return TRUE;
 }

static int GetFillMethod ()
 {
 	return Push (OpStack, MakeInteger (fill_method));
 }

static int SetStrokeMethod (method) Object method;
 {
 	stroke_method = BodyInteger (method);
 	return TRUE;
 }

static int GetStrokeMethod ()
 {
 	return Push (OpStack, MakeInteger (stroke_method));
 }
