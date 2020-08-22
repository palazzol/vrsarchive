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
#include "canon.h"
#include "wwinfo.h"

static void xxrasterop ();

static struct hardware *NewHardware ();

struct hardware *InitHardware ()
 {
	wwstartup (2);
	
	InitTransfer (wwask (ASKXPPI));
	
	return NULL;
 }

struct hardware *NewWindowHardware (width, height) int width, height;
 {
 	DevicePoint real_extent;
 	window *w;
 	
 	real_extent = NewDevicePoint (width, height);
 	
 	if (width == 0)
 		width = 1;
 	if (height == 0)
 		height = 1;
 	
 	if ((w = wwxget (boxbuild (0, 0, width, height), 2, "PostScript", 0)) == NULL)
 		return NULL;
 	return NewHardware (w->w_bm, real_extent, ISWIN);
 }

struct hardware *NewBitmapHardware (width, height) int width, height;
 {
 	DevicePoint real_extent;
 	bitmap *bm;
 	
 	real_extent = NewDevicePoint (width, height);
 	
 	if (width == 0)
 		width = 1;
 	if (height == 0)
 		height = 1;
 	if ((bm = bmget (width, height)) == NULL)
 		return NULL;
 	
 	return NewHardware (bm, real_extent, 0);
 }

#define DeviceBitmap(h) ((bitmap *) ((h)->hard.addr))

static void DestroyHard (dev) struct hardware *dev;
 {
 	bitmap *bm = DeviceBitmap (dev);
 	
 	if (bm)
 	 {
 	 	if (bm->bm_window)
 	 		wwfree (bm->bm_window);
 		else
 			bmfree (bm);
 	 }
 }

void DestroyHardware (dev) struct hardware *dev;
 {
 	if (dev == NULL)
 		return;
 	DestroyHard (dev);
 	if (dev->aux)
 		DestroyHardware (dev->aux);
 	Free ((char *) dev);
 }

static struct hardware *NewHardware (bm, extent, flags) bitmap *bm; DevicePoint extent; int flags;
 {
 	struct hardware *d = (struct hardware *) Malloc (sizeof (struct hardware));
 	
 	d->flags = flags;
 	d->hard.addr = (char *) bm;
 	d->aux = d->clip = NULL;
 	d->extent = extent;
 	
 	return d;
 }

struct hardware *HardwareFromString (st, width, height) char *st; int width, height;
 {
 	unsigned size = height * (width + 7) / 8;
 	char *s = Malloc (size + 4);
 	struct hardware *h;
 	
 	s[0] = height >> 8; s[1] = height;
 	s[2] = width >> 8;  s[3] = width;
 	Bcopy (s + 4, st, size);
 	
 	h = NewHardware (bmdecode (s, ENWWSTYLE), NewDevicePoint (width, height), 0);
 	Free ((char *) s);
 	
 	return h;
 }

char *StringFromHardware (dev) struct hardware *dev;
 {
 	int length;
 	
 	return bmencode (DeviceBitmap (dev), ENWWSTYLE, &length) + 4;
 }

void UpdateControl (h, flag) struct hardware *h; int flag;
 {
 	window *oldddwin = ddwin;
 	
 	if (h == NULL)
 		return;
 	ddwin = DeviceBitmap (h)->bm_window;
 	if (ddwin)
		wwstack (flag ? WWPOP : WWPUSHOFF);
	ddwin = oldddwin;
 }

void RasterTile (from, to, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint toPoint, extent;
	int rop;
 {
 	if (to == NULL || extent.dx == 0 || extent.dy == 0)
 		return;
 	bmclip (DeviceBitmap (to), boxbuild (toPoint.dx, toPoint.dy, toPoint.dx + extent.dx - 1, toPoint.dy + extent.dy - 1));
 	xxrasterop (DeviceBitmap (from), DeviceBitmap (from)->bm_box, DeviceBitmap (to), DeviceBitmap (to)->bm_box, rop);
 	bmclip (DeviceBitmap (to), noclipbox);
 }

void BitBlt (from, to, fromPoint, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, extent;
	int rop;
 {
	box frombox, tobox;
	
	if (to == NULL || extent.dx == 0 || extent.dy == 0)
 		return;
	
 	if (from == NULL)
 		rop = single_rop [rop];
 	else
 		frombox = boxbuild (fromPoint.dx, fromPoint.dy, fromPoint.dx + extent.dx - 1, fromPoint.dy + extent.dy - 1);
 	
  	tobox = boxbuild (toPoint.dx, toPoint.dy, toPoint.dx + extent.dx - 1, toPoint.dy + extent.dy - 1);
	
	xxrasterop ((from == NULL ? NULL : DeviceBitmap (from)), frombox, DeviceBitmap (to), tobox, rop);
 }

void BitBltLine (h, fromPoint, toPoint, rop) 
  	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	int rop;
 {
 	if (h == NULL)
 		return;
 	
	switch (single_rop [rop])
	 {
	 	case ROP_FALSE: 	rop = WWAND; 	break;
	 	case ROP_TRUE:  	rop = WWOR;  	break;
	 	case ROP_NOTDEST: 	rop = WWXOR; 	break;
	 	case ROP_DEST: 		return; 	break;
	 	
	 	default:
	 		fprintf (stderr, "illegal rasterop\n");
	 		exit (1);
	 }
	
	ddbm = DeviceBitmap (h);
 	dd->d_line = rop;
 	
	line (fromPoint.dx, fromPoint.dy, LNMOVEABS);
 	line (toPoint.dx, toPoint.dy, LNDRAWABS);
 }

void BitBltBlob (to, top, height, left, right, rop) struct hardware *to; int top, height, *left, *right, rop;
 {
 	int i, op, offset = top;
 	height += top;
 	switch (rop)
 	 {
 	 	case ROP_FALSE: 	op = BMCLEARALL; break;
 	 	case ROP_DEST: 		return;
 	 	case ROP_NOTDEST: 	op = BMNOTALL; break;
 	 	case ROP_TRUE: 		op = BMNOTALL | BMCLEARALL; break;
 	 }
 	ddbm = DeviceBitmap (to);
 	UpdateControl (to, FALSE);
 	for (i = top; i < height; i++)
 		bmbox (boxbuild (left[i - offset], i, right[i - offset], i), op);
 	UpdateControl (to, TRUE);
 }

static void xxrasterop (from, frombox, to, tobox, rop) bitmap *from, *to; box frombox, tobox; int rop;
 {
	switch (rop)
 	 {
 	 	case ROP_FALSE: 							    bmxbox (to, tobox, BMCLEARALL);	break;
 	 	case ROP_AND: 		bmxcopy (from, frombox, to, tobox, WWAND); 						break;
 	 	case ROP_ANDNOT:	bmxcopy (from, frombox, to, tobox, WWOR | WWNOT);   bmxbox (to, tobox, BMNOTALL); 	break;
	 	case ROP_SOURCE: 	bmxcopy (from, frombox, to, tobox, WWCOPY); 						break;
	 	case ROP_NOTAND: 	bmxcopy (from, frombox, to, tobox, WWAND | WWNOT); 					break;
 	 	case ROP_DEST: 													break;
 	 	case ROP_XOR:		bmxcopy (from, frombox, to, tobox, WWXOR); 						break;
  	 	case ROP_OR:		bmxcopy (from, frombox, to, tobox, WWOR); 						break;
  	 	case ROP_NOR:		bmxcopy (from, frombox, to, tobox, WWOR); 	    bmxbox (to, tobox, BMNOTALL); 	break;
  	 	case ROP_NXOR:		bmxcopy (from, frombox, to, tobox, WWXOR); 	    bmxbox (to, tobox, BMNOTALL); 	break;
	 	case ROP_NOTDEST: 							    bmxbox (to, tobox, BMNOTALL); 	break;
	 	case ROP_ORNOT:		bmxcopy (from, frombox, to, tobox, WWAND | WWNOT);  bmxbox (to, tobox, BMNOTALL);	break;
	 	case ROP_NOTSOURCE:	bmxcopy (from, frombox, to, tobox, WWCOPY | WWNOT); 					break;
  	 	case ROP_NOTOR:		bmxcopy (from, frombox, to, tobox, WWOR | WWNOT); 					break;
	 	case ROP_NAND:		bmxcopy (from, frombox, to, tobox, WWAND); 	    bmxbox (to, tobox, BMNOTALL); 	break;
 	 	case ROP_TRUE: 							bmxbox (to, tobox, BMCLEARALL | BMNOTALL); 	break;
 	 	
 	 	default:
 	 		fprintf (stderr, "Illegal rasterop %d (hex 0x%x)\n", rop, rop);
 	 		exit (1);
 	 }
 }
