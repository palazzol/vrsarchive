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

static int NullDevice ();
static int FrameDevice ();
static int GrabBits ();

Colour Black = { 0.0, 0.0, 0.0 }, White = { 0.0, 0.0, 1.0 };

struct device *NewWindowDevice (), *NewBitmapDevice (), *NewCacheDevice ();

InitDevices ()
 {
 	struct hardware *h;
 	DevicePoint extent;
 	
	InstallOp ("framedevice",	FrameDevice, 		4, 0, 0, 0, Array, Integer, Integer, Array);
	InstallOp ("nulldevice",	NullDevice,		0, 0, 0, 0);
	InstallOp ("grabbits",		GrabBits,		4, 0, 0, 0, Float, Float, Float, Float);
	
	extent = HardwareExtent (h = InitHardware ());
 	gstate->device = NULL; gstate->clipdevice = NULL;
	SetDevice (LinkDevice (DeviceFrom (h, DeviceMatrix (extent.dx, extent.dy))));
 }

static int GrabBits (x, y, w, h) Object x, y, w, h;
 {
 	HardPoint origin, corner, extent;
 	int width, height, size;
 	char *s;
 	struct hardware *dev;
 	
 	origin = ExtToInt (NewPoint (BodyReal (x), BodyReal (y)));
 	corner = ExtToInt (NewPoint (BodyReal (x) + BodyReal (w), BodyReal (y) + BodyReal (h)));
 	width = corner.hx - origin.hx;
 	height = corner.hy - origin.hy;
 	
 	fprintf (stderr, "width = %d, height = %d\n", width, height);
 	
 	extent = NewHardPoint ((float) width, (float) height);
 	
 	dev = NewBitmapHardware (width, height);
 	
 	BitBlt (gstate->device->dev, dev,
 		NewDevicePoint ((int) origin.hx, (int) origin.hy),
 		NewDevicePoint (0, 0),
 		NewDevicePoint ((int) extent.hx, (int) extent.hy),
 		ROP_SOURCE);
 	s = StringFromHardware (dev);
 	DestroyHardware (dev);
 	size = (width + 7) / 8 * height;
 	
 	Push (OpStack, MakeString (s, size));
 	Free (s);
 	Push (OpStack, MakeInteger (width));
 	Push (OpStack, MakeInteger (height));
 	
 	return TRUE;
 }

static int NullDevice ()
 {
 	SetDevice (
 		NewDevice (
 			NewClipPath (0.0, 0.0, 0.0, 0.0),
 			NewMatrix (1.0, 0.0, 0.0, 1.0, 0.0, 0.0),
 			(struct hardware *) NULL));
 	return TRUE;
 }

/*ARGSUSED*/
static int FrameDevice (mat, width, height, proc) Object mat, width, height, proc;
 {
 	Matrix m;
 	
 	if (lengthArray (mat) != 6 || !ExtractMatrix (&m, mat))
 		return Error (PTypeCheck);
 	if (BodyInteger (width) < 0 || BodyInteger (height) < 0)
 		return Error (PRangeCheck);
 	SetDevice (NewWindowDevice (BodyInteger (width) * 8, BodyInteger (height), m));
 	ErasePage ();
 	
	return TRUE;
 }

struct device *DeviceFrom (h, m) struct hardware *h; Matrix m;
 {
 	DevicePoint extent;
 	
 	extent = HardwareExtent (h);
 	return NewDevice (
 			NewClipPath (0.0, (float) extent.dx, (float) extent.dy, 0.0),
 			m,
 			h);
 }

struct device *NewWindowDevice (width, height, m) int width, height; Matrix m;
 {
 	return DeviceFrom (NewWindowHardware (width, height), m);
 }

struct device *NewBitmapDevice (width, height, m) int width, height; Matrix m;
 {
 	return DeviceFrom (NewBitmapHardware (width, height), m);
 }

struct device *NewCacheDevice (m, width, height, swidth, sheight) Matrix m; int width, height, swidth, sheight;
 {
 	return NewDevice (
 			NewClipPath (0.0, (float) width, (float) height, 0.0),
 			NewMatrix (m.A, m.B, m.C, m.D, (float) swidth, (float) height - sheight),
 			NewBitmapHardware (width, height));
 }

Path NewClipPath (left, right, top, bottom) float left, right, top, bottom;
 {
 	Path p = NewPath ();
 	HardPoint cp;
 	int cp_def = gstate->cp_defined;
 	
 	cp = gstate->cp;
 	VOID MoveTo (p, NewHardPoint (left, 	bottom));
 	VOID LineTo (p, NewHardPoint (right, bottom));
 	VOID LineTo (p, NewHardPoint (right, top));
 	VOID LineTo (p, NewHardPoint (left,  top));
 	ClosePath (p);
 	
 	gstate->cp = cp; gstate->cp_defined = cp_def;
 	
 	return p;
 }

struct device *NewDevice (clip, m, dev) Path clip; Matrix m; struct hardware *dev;
 {
 	struct device *res = (struct device *) Malloc (sizeof (struct device));
 	
 	res->link_count = 0;
 	res->default_clip = clip;
 	res->default_matrix = m;
 	res->dev = dev;
 	
 	return res;
 }

SetDevice (d) struct device *d;
 {
 	UnlinkDevice (gstate->device);
 	gstate->device = LinkDevice (d);
 	gstate->CTM = d->default_matrix;
 	InitClip ();
 }

int IsCache (d) struct device *d;
 {
	return !IsWindowHardware (d->dev);
 }

struct device *LinkDevice (d) struct device *d;
 {
 	if (d)
 		++d->link_count;
 	
	return d;
 }

UnlinkDevice (d) struct device *d;
 {
 	if (d == NULL)
 		return;
 	if (--d->link_count != 0)
		return;
	if (d->dev != NULL)
 		DestroyHardware (d->dev);	
 	Free ((char *) d);
 }

struct device *UniqueDevice (d) struct device *d;
 {
 	struct device *res;
 	DevicePoint ex;
 	
 	if (d && d->link_count == 1)
 		return d;
 	ex = HardwareExtent (gstate->device->dev);
 	res = LinkDevice (NewBitmapDevice (ex.dx, ex.dy, gstate->CTM));
 	/*if (d)
 	 {
 	 	BitBlt (d->dev, res->dev, NewDevicePoint (0, 0), NewDevicePoint (0, 0), ex, ROP_SOURCE);
 		UnlinkDevice (d);
 	 }
 	else
 		*/BitBlt (NULL, res->dev, NewDevicePoint (0, 0), NewDevicePoint (0, 0), ex, ROP_FALSE);
 	
 	return res;
 }

DevicePoint NewDevicePoint (x, y) int x, y;
 {
 	DevicePoint res;
 	
 	res.dx = x; res.dy = y;
 	
 	return res;
 }

DevicePoint HardToDevice (p) HardPoint p;
 {
 	return NewDevicePoint ((int) p.hx, (int) p.hy);
 }

DevicePaintLine (d, fromPoint, toPoint, colour)
	struct device *d;
	HardPoint fromPoint, toPoint;
	Colour colour;
 {
 	PaintLine (d->dev, HardToDevice (fromPoint), HardToDevice (toPoint), colour);
 }

/*
void ColourLine (h, fromPoint, toPoint, rop, colour) 
  	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	int rop;
	Colour colour;
 {
 	int x1 = fromPoint.dx, y1 = fromPoint.dy, x2 = toPoint.dx, y2 = toPoint.dy;
 	int col = IsWindowHardware (h) ? HardColour (colour) : HardBlack;
	
 	if (col == HardBlack)
 	 {
 	 	ddbm = h->bm;
 	 	dd->d_line = rop_map [rop];
		line (x1, y1, LNMOVEABS);
 		line (x2, y2, LNDRAWABS);
	 }
 	else
 	 {
 	 	box b;
 	 	
 	 	b = boxbuild (Min (x1, x2), Min (y1, y2), Max (x1, x2), Max (y1, y2));
 	 	
 	 	NeedAux (h);
 	 	GraySync (h, col);
 	 	bmxcopy (h->bm, b, h->bm, b, WWXOR);
 	 	
 	 	ddbm = h->aux;
 	 	dd->d_line = WWOR;
 		line (x1, y1, LNMOVEABS);
 		line (x2, y2, LNDRAWABS);
 		
 	 	bmxcopy (h->aux,  b, h->bm,  b, WWAND | WWNOT);
 	 	bmxcopy (h->gray, b, h->aux, b, WWAND);
  		bmxcopy (h->aux,  b, h->bm,  b, rop_map [rop]);
	 	
	 }
 }
*/

/*
void ColourBitBlt (from, to, fromPoint, toPoint, extent, rop, colour)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, extent;
	int rop;
	Colour colour;
 {
 	box frombox, tobox;
 	int col = IsWindowHardware (to) ? HardColour (colour) : HardBlack;
 	int op = rop_map [rop];
 	
 	
 	frombox = boxbuild (fromPoint.dx, fromPoint.dy,
 		fromPoint.dx + extent.dx - 1, fromPoint.dy + extent.dy - 1);
 	tobox = boxbuild (toPoint.dx, toPoint.dy,
 		toPoint.dx + extent.dx - 1, toPoint.dy + extent.dy - 1);
 	
	if (to == NULL)
 		return;
 	
 	if (rop == ROP_FALSE)
		bmxcopy (to->bm, tobox, to->bm, tobox, WWXOR);
 	else if (rop == ROP_TRUE)
		if (col == HardBlack)
 			bmxcopy (to->bm, tobox, to->bm, tobox, WWXOR | WWNOT);
 		else
		 {
 			GraySync (to, col);
			bmxcopy (to->gray, tobox, to->bm, tobox, WWCOPY);
		 }
	else if (col == HardBlack)
		bmxcopy (from->bm, frombox, to->bm, tobox, op);
	else
	 {
 		GraySync (to, col);
		NeedAux (to);
		bmxcopy (to->gray, tobox, to->aux, tobox, WWCOPY);
		bmxcopy (from->bm, frombox, to->aux, tobox, WWAND);
		bmxcopy (to->aux, tobox, to->bm, tobox, op);
	 }
 }
*/
