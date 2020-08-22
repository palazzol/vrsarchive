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

static int RealImage ();
static int RealImageMask ();

struct hardware *ScaleBitmap ();

InitImage ()
 {
 	InstallOp ("realimage",		RealImage,	5, 0, 0, 0, Integer, Integer, Integer, Array, String);
 	InstallOp ("realimagemask",	RealImageMask,	5, 0, 0, 0, Integer, Integer, Boolean, Array, String);
 }

static int RealImageMask (width, height, invert, mat, string) Object width, height, invert, mat, string;
 {
 	Matrix m, m2;
 	Vector origin, right, bottom;
 	int w = BodyInteger (width), h = BodyInteger (height);
 	int inv = BodyBoolean (invert), l = lengthString (string);
 	int dwidth, dheight;
 	unsigned char *s = BodyString (string);
 	
  	VOID GSave ();
	if (!ExtractMatrix (&m, mat))
 		return Error (PTypeCheck);
 	if (l != (w + 7) / 8 * h || w < 0 || h < 0)
 		return Error (PRangeCheck);
 	if (w == 0 || h == 0)
 		return TRUE;
 	m = MatInvert (m);
 	gstate->CTM = m2 = MatMult (m, gstate->CTM);
 	origin = Transform (NewVector (0.0, 0.0, 1.0), m2);
 	right  = Transform (NewVector ((float) w, 0.0, 1.0), m2);
 	bottom = Transform (NewVector (0.0, (float) h, 1.0), m2);
 	/* fprintf (stderr, "origin = (%g, %g), right = (%g, %g), bottom = (%g, %g)\n", origin.vx, origin.vy, right.vx, right.vy, bottom.vx, bottom.vy);
 	fprintf (stderr, "matrix == [%g %g %g %g %g %g]\n", m2.A, m2.B, m2.C, m2.D, m2.tx, m2.ty);*/
 	
 	dwidth = right.vx - origin.vx; dheight = bottom.vy - origin.vy;
 	
 	if (origin.vx == bottom.vx && origin.vy == right.vy &&
 			dwidth > 0 && dheight > 0/* && dwidth >= w && dheight >= h*/)
 	 {		/* simple orientation */
 	 	struct hardware *from, *to;
 	 	
 	 	from = HardwareFromString (s, w, h);
 	 	if (!inv)
 	 		BitBlt (from, from,
 	 			NewDevicePoint (0, 0), NewDevicePoint (0, 0),
 	 			HardwareExtent (from), ROP_NOTSOURCE);
 		if (dwidth != w || dheight != h)
 		 {
 	 		to = ScaleBitmap (from, (float) dwidth / w, (float) dheight / h);
 	 		DestroyHardware (from);
 	 	 }
 	 	else
 	 		to = from;
 	 	
 	 	Paint (to, gstate->device->dev,
 	 		NewDevicePoint (0, 0),
 	 		NewDevicePoint ((int) origin.vx, (int) origin.vy),
 	 		NewDevicePoint (dwidth, dheight),
 	 		gstate->colour);
 	 	DestroyHardware (to);
 	 }
 	else
 		DrawBitmap (s, w, h, inv);
	VOID GRestore ();
	
	return TRUE;
 }

DrawBitmap (s, width, height, inv) unsigned char *s; int width, height, inv;
 {
 	int w, h;
 	
 	VOID PNewPath ();
 	
 	for (h = 0; h < height; h++)
 		for (w = 0; w < width; w++)
 		 	if (((s [h * (width >> 3) + (w >> 3)] & (1 << (7 - (w & 0x7)))) != 0) == inv)
  		 	 	FillUnitBox (w, h);
 }

void Tile (t, h) struct hardware *t, *h;
 {
 	DevicePoint xt, xh;
 	int i, sw, sh;
 	
 	xt = HardwareExtent (t);
 	xh = HardwareExtent (h);
 	
  	BitBlt (t, h, NewDevicePoint (0, 0), NewDevicePoint (0, 0), xt, ROP_SOURCE);
 	
 	sw = xt.dx;
	for (i = sw;;)
 	 {
 		BitBlt (h, h, NewDevicePoint (0, 0), NewDevicePoint (i, 0), NewDevicePoint (sw, xt.dy), ROP_SOURCE);
 		if (i >= xh.dx)
 			break;
 		i += sw;
 		sw <<= 1;
 	 }
 	
 	sh = xt.dy;
	for (i = sh;;)
 	 {
 		BitBlt (h, h, NewDevicePoint (0, 0), NewDevicePoint (0, i), NewDevicePoint (xh.dx, sh), ROP_SOURCE);
 		if (i >= xh.dy)
 			break;
 		i += sh;
 		sh <<= 1;
 	 }
 }

void FillMask (h, mask, width) struct hardware *h; char mask; int width;
 {
 	struct hardware *m;
 	
 	mask <<= 8 - width;
 	
 	m = HardwareFromString (&mask, width, 1);
 	
 	Tile (m, h);
 	
 	DestroyHardware (m);
 }

struct hardware *UnpaddedHardwareFromString (s, w, h) unsigned char *s; int w, h;
 {
 	struct hardware *thin;
 	struct hardware *res;
 	int i;
 	
 	if (w & 7 == 0)
 		return HardwareFromString (s, w, h);
 	
 	thin = HardwareFromString (s, w * h, 1);
 	res = NewBitmapHardware (w, h);
 	
 	for (i = 0; i < h; i++)
 		BitBlt (thin, res, NewDevicePoint (i * w, 0), NewDevicePoint (0, i), NewDevicePoint (w, 1), ROP_SOURCE);
 	
 	DestroyHardware (thin);
 	return res;
 }

static void VerticalReverseImage (h) struct hardware *h;
 {
 	DevicePoint ex;
 	int i, n;
 	
 	ex = HardwareExtent (h);
 	
 	n = ex.dy;
 	
 	for (i = 0; i < n / 2; i++)
 		BitBlt (h, h, NewDevicePoint (0, i), NewDevicePoint (0, n - i - 1), NewDevicePoint (ex.dx, 1), ROP_XOR),
 		BitBlt (h, h, NewDevicePoint (0, n - i - 1), NewDevicePoint (0, i), NewDevicePoint (ex.dx, 1), ROP_XOR),
 		BitBlt (h, h, NewDevicePoint (0, i), NewDevicePoint (0, n - i - 1), NewDevicePoint (ex.dx, 1), ROP_XOR);
 }

static int RealImage (width, height, depth, mat, string) Object width, height, depth, mat, string;
 {
 	Matrix m, m2;
 	Vector origin, right, bottom;
 	int w = BodyInteger (width), h = BodyInteger (height);
 	int vrev = 0, dep = BodyInteger (depth), l = lengthString (string);
 	int dwidth, dheight;
 	unsigned char *s = BodyString (string);
 	
   	VOID GSave ();
	if (!ExtractMatrix (&m, mat))
 		return Error (PTypeCheck);
 	if (l != (w * dep + 7) / 8 * h || w < 0 || h < 0)
 		return Error (PRangeCheck);
 	if (w == 0 || h == 0)
 		return TRUE;
  	m = MatInvert (m);
 	gstate->CTM = m2 = MatMult (m, gstate->CTM);
 	origin = Transform (NewVector (0.0, 0.0, 1.0), m2);
 	right  = Transform (NewVector ((float) w, 0.0, 1.0), m2);
 	bottom = Transform (NewVector (0.0, (float) h, 1.0), m2);
 	
	dwidth = right.vx - origin.vx; dheight = bottom.vy - origin.vy;
	
	if (dheight < 0)
		origin.vy += dheight, right.vy += dheight, ++vrev, dheight = -dheight;
	
	/* fprintf (stderr, "origin.vx = %g, origin.vy = %g, bottom.vx = %g, right.vy = %g, dwidth = %d, dheight = %d, w = %d, h = %d\n",
			origin.vx, origin.vy, bottom.vx, right.vy, dwidth, dheight, w, h); */
 	
 	if (dep != 16 && (int) origin.vx == (int) bottom.vx && (int) origin.vy == (int) right.vy &&
 			dwidth > 0 && dheight > 0/* && dwidth >= w && dheight >= h */)
 	 {		/* simple orientation */
 	 	struct hardware *from , *to;
 	 	
 	 	if (dep == 1)
 	 	 {
 	 		from = HardwareFromString (s, w, h);
 	 		if (vrev)
 	 			VerticalReverseImage (from);
 			if (dwidth != w || dheight != h)
 			 {
 	 			to = ScaleBitmap (from, (float) dwidth / w, (float) dheight / h);
 	 			DestroyHardware (from);
 	 		 }
 	 		else
 	 			to = from;
 	 		
			Paint (to, gstate->device->dev,
				NewDevicePoint (0, 0), NewDevicePoint ((int) origin.vx, (int) origin.vy),
				NewDevicePoint (dwidth, dheight),
				NewGray (1.0));
			BitBlt (to, to,
				NewDevicePoint (0, 0), NewDevicePoint (0, 0),
				NewDevicePoint (dwidth, dheight),
				ROP_NOTSOURCE);
			Paint (to, gstate->device->dev,
				NewDevicePoint (0, 0), NewDevicePoint ((int) origin.vx, (int) origin.vy),
				NewDevicePoint (dwidth, dheight),
				NewGray (0.0));
 	 		DestroyHardware (to);
 	 	 }
 	 	else
 	 	 {
 	 		struct hardware *mask, *new, *temp;
 	 	 	int i, j;
 	 	 	
 	 	 	from = HardwareFromString (s, w * dep, h);
  	 		if (vrev)
 	 			VerticalReverseImage (from);
 	 	 	new = HardwareFromString (s, w, h);
	 	 	mask = NewBitmapHardware (w * dep, h);
 	 	 	temp = NewBitmapHardware (w * dep, h);
 	 	 	
 	 	 	for (i = 0; i < (1<<dep); i++)
 	 	 	 {
 	 	 	 	FillMask (mask, i, dep);
	 	 	 	
	 	 	 	BitBlt (from, mask, NewDevicePoint (0, 0), NewDevicePoint (0, 0), HardwareExtent (from), ROP_NXOR);
	 	 	 	BitBlt (NULL, temp, NewDevicePoint (0, 0), NewDevicePoint (0, 0), HardwareExtent (temp), ROP_TRUE);
 	 	 	 	for (j = 0; j < dep; j++)
 	 	 	 		BitBlt (mask, temp, NewDevicePoint (j, 0), NewDevicePoint (0, 0), HardwareExtent (mask), ROP_AND);
 	 	 	 	for (j = 0; j < w; j++)
 	 	 	 		BitBlt (temp, new, NewDevicePoint (j * dep, 0), NewDevicePoint (j, 0), NewDevicePoint (1, h), ROP_SOURCE);
 	 	 	 	to = ScaleBitmap (new, (float) dwidth / w, (float) dheight / h);
		  	 	Paint (to, gstate->device->dev,
		 	 		NewDevicePoint (0, 0),
		 	 		NewDevicePoint ((int) origin.vx, (int) origin.vy),
		 	 		NewDevicePoint (dwidth, dheight),
		 	 		NewGray ((float) i / ((1 << dep) - 1)));
		 	 	DestroyHardware (to);
	 	 	 }
 	 		DestroyHardware (from);
 	 		DestroyHardware (mask);
 	 		DestroyHardware (temp);
 	 		DestroyHardware (new);
 	 	 }
 	 }
 	else
 	 {
 	 	if (vrev)
 	 		origin.vy += dheight, right.vy += dheight, dheight = -dheight;
 		DrawColourBitmap (s, w, h, dep);
 	 }
	VOID GRestore ();
	
	return TRUE;
 }

static log2[] = { 0, 0, 1, 0, 2, 0, 0, 0, 3 };
static mask[] = { 0, 0x80, 0xc0, 0, 0xf0, 0, 0, 0, 0xff };

DrawColourBitmap (s, width, height, depth) unsigned char *s; int width, height, depth;
 {
 	int w, h, c;
 	
 	VOID PNewPath ();
 	
 	if (depth == 16)
 		for (h = 0; h < height; h++)
 			for (w = 0; w < width; w++)
 		 	 {
 		 	 	gstate->colour = NewColour (0.0, 0.0, (float) ((s [h * (width << 1) + (w << 1)] << 8) |
 		 	 						s [h * (width << 1) + (w << 1) + 1]) / (1 << 16));
 		 	 	FillUnitBox (w, h);
 		 	 }
 	else
 		for (c = 0; c < 1 << depth; c++)
 		 {
  		 	gstate->colour = NewColour (0.0, 0.0, (float) c / ((1 << depth) - 1));
			for (h = 0; h < height; h++)
 				for (w = 0; w < width; w++)
 				 {
 			 		int foo = 3 - log2 [depth];
 			 		int byte = s [h * (width >> foo) + (w >> foo)];
 			 		int shift = (w & ((1 << foo) - 1)) * depth;
 			 		if (c == ((byte & (mask [depth] >> shift)) >> (8 - (shift + depth))))
 		 	 			FillUnitBox (w, h);
 		 		 }
 		  }
 }

FillUnitBox (w, h) int w, h;
 {
 	VOID MoveTo (gstate->path, ExtToInt (NewPoint ((float ) w, (float ) h)));
	VOID LineTo (gstate->path, ExtToInt (NewPoint ((float ) w+1, (float ) h)));
	VOID LineTo (gstate->path, ExtToInt (NewPoint ((float ) w+1, (float ) h+1)));
	VOID LineTo (gstate->path, ExtToInt (NewPoint ((float ) w, (float ) h+1)));
	VOID ClosePath (gstate->path);
	VOID Fill ();
  }

struct hardware *ScaleBitmap (from, xscale, yscale) struct hardware *from; float xscale, yscale;
 {	 	 	
	struct hardware *middle, *middle2, *high, *high2;
	int i, w, h, dwidth, dheight, ixscale, iyscale;
	DevicePoint extent;
	
	extent = HardwareExtent (from);
	w = extent.dx; h = extent.dy;
	dwidth = w * xscale + 0.5; dheight = h * yscale + 0.5;
	
	if (dwidth > w)
	 {
		middle = NewBitmapHardware (dwidth, h);
		for (i = 0; i < w; i++)
			BitBlt (from, 	middle,
				NewDevicePoint (i, 0), NewDevicePoint ((int) (i*xscale), 0),
				NewDevicePoint (1, h),
				ROP_OR);
		middle2 = NewBitmapHardware (dwidth, h);
		ixscale = (int) ((float) dwidth / w + 0.5); 
		for (i = 0; i < ixscale + 1; i++)
			BitBlt (middle, middle2,
				NewDevicePoint (0, 0), NewDevicePoint (i, 0),
				NewDevicePoint (dwidth - ixscale + 1, h),
				ROP_OR);
		DestroyHardware (middle);
	 }
	else
	 {
	 	middle2 = NewBitmapHardware (dwidth, h);
		for (i = 0; i < dwidth; i++)
			BitBlt (from, 	middle2,
				NewDevicePoint ((int) (i/xscale), 0), NewDevicePoint (i, 0),
				NewDevicePoint (1, h),
				ROP_OR);
	 }
	
	if (dheight > h)
	 {
		high = NewBitmapHardware (dwidth, dheight);
		for (i = 0; i < h; i++)
			BitBlt (middle2, high,
				NewDevicePoint (0, i), NewDevicePoint (0, (int) (i*yscale)),
				NewDevicePoint (dwidth, 1),
				ROP_OR);
		DestroyHardware (middle2);
		
		high2 = NewBitmapHardware (dwidth, dheight);
		iyscale = (int) ((float) dheight / h + 0.5);
		for (i = 0; i < iyscale + 1; i++)
			BitBlt (high, 	high2, 
				NewDevicePoint (0, 0), NewDevicePoint (0, i),
				NewDevicePoint (dwidth, dheight - iyscale + 1),
				ROP_OR);
		DestroyHardware (high);
	 }
	else
	 {
		high2 = NewBitmapHardware (dwidth, dheight);
		for (i = 0; i < dheight; i++)
			BitBlt (middle2, high2,
				NewDevicePoint (0, (int) (i/yscale)), NewDevicePoint (0, i),
				NewDevicePoint (dwidth, 1),
				ROP_OR);
	 }
 	return high2;
 }
