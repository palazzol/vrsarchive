/*
 * Copyright (C) Rutherford Appleton Laboratory 1987, All Rights Reserved.
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
#include <pixrect/pixrect_hs.h>

int rop_map [] =
 {
 	PIX_SRC & PIX_NOT (PIX_SRC),
 	PIX_SRC & PIX_DST,
 	PIX_SRC & PIX_NOT (PIX_DST),
 	PIX_SRC,
 	PIX_NOT (PIX_SRC) & PIX_DST,
 	PIX_DST,
 	PIX_SRC ^ PIX_DST,
 	PIX_SRC | PIX_DST,
 	PIX_NOT (PIX_SRC | PIX_DST),
 	PIX_NOT (PIX_SRC ^ PIX_DST),
 	PIX_NOT (PIX_DST),
 	PIX_SRC | PIX_NOT (PIX_DST),
 	PIX_NOT (PIX_SRC),
 	PIX_NOT (PIX_SRC) | PIX_DST,
 	PIX_NOT (PIX_SRC & PIX_DST),
 	PIX_SRC | PIX_NOT (PIX_SRC)
 };

static struct hardware *NewHardware ();

static struct pixrect *screen;

char *malloc ();

struct hardware *InitHardware ()
 {
	screen = pr_open ("/dev/fb");
	PanicIf (screen == NULL, "could not open display");
	
	InitTransfer (82);	/* XXX */
	
	return NULL;
 }

struct hardware *NewWindowHardware (width, height) int width, height;
 {
 	DevicePoint real_extent;
 	struct pixrect *w;
 	
 	real_extent = NewDevicePoint (width, height);
 	
 	if ((w = pr_region (screen, 0, 0, width, height)) == NULL)
 		return NULL;
 	return NewHardware (w, real_extent, ISWIN);
 }

struct hardware *NewBitmapHardware (width, height) int width, height;
 {
 	DevicePoint real_extent;
 	struct pixrect *bm;
 	
 	real_extent = NewDevicePoint (width, height);
 	
 	if ((bm = mem_create (width, height, 1)) == NULL)
 	 {
 	 	fprintf (stderr, "mem_create (%d, %d, 1)\n", width, height);
 		Panic ("failed to create bitmap");
 	 }
 	
 	return NewHardware (bm, real_extent, 0);
 }

#define DeviceBitmap(h) ((struct pixrect *) ((h)->hard.addr))

static void DestroyHard (dev) struct hardware *dev;
 {
 	struct pixrect *bm = DeviceBitmap (dev);
 	
 	if (bm)
 		pr_destroy (bm);
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

static struct hardware *NewHardware (bm, extent, flags) struct pixrect *bm; DevicePoint extent; int flags;
 {
 	struct hardware *d = (struct hardware *) Malloc (sizeof (struct hardware));
 	
 	d->flags = flags;
 	d->hard.addr = (char *) bm;
 	d->aux = d->clip = NULL;
 	d->extent = extent;
 	
 	return d;
 }

struct hardware *HardwareFromString (s, width, height) unsigned char *s; int width, height;
 {
	int words = (width + 15) / 16;
	struct pixrect *pr = mem_create (width, height, 1);
	short *d = mpr_d (pr)->md_image;
	int odd = ((width + 7) / 8) & 1;
	int i, j;

	for (i = 0; i < height; i++)
	 {
		for (j = 0; j < words - odd; j++)
		 {
			short word = *s++;

			*d++ = (word << 8) | *s++;
		 }
		if (odd)
			*d++ = *s++ << 8;
	 }
	return NewHardware (pr, NewDevicePoint (width, height), 0);
 }

char *StringFromHardware (h) struct hardware *h;
 {
	int words = (h->extent.dx + 15) / 16;
	char *string = malloc ((h->extent.dx + 7) / 8 * h->extent.dy), *s = string;
	int i, j, odd = ((h->extent.dx + 7) / 8) & 1;
	short *d = mpr_d (DeviceBitmap (h))->md_image;

	for (i = 0; i < h->extent.dy; i++)
	 {
		for (j = 0; j < words - odd; j++)
		 {
			short word = *d++;

			*s++ = (word >> 8) & 0xFF;
			*s++ = word & 0xFF;
		 }
		if (odd)
			*s++ = (*d++ >> 8) & 0xFF;
	 }
	return string;
 }

void UpdateControl (h, flag) struct hardware *h; int flag;
 {
 }

void RasterTile (from, to, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint toPoint, extent;
	int rop;
 {
	struct pixrect *fr;
	
 	if (to == NULL || extent.dx == 0 || extent.dy == 0)
 		return;
 	if (from)
 		fr = DeviceBitmap (from);
 	else
 		fr = NULL;
	
 	pr_replrop (DeviceBitmap (to), toPoint.dx, toPoint.dy, extent.dx, extent.dy, rop_map [rop], fr, toPoint.dx, toPoint.dy);
 }

void BitBlt (from, to, fromPoint, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, extent;
	int rop;
 {
	struct pixrect *fr;
	
	if (to == NULL || extent.dx == 0 || extent.dy == 0)
 		return;
 		
 	if (from)
 		fr = DeviceBitmap (from);
 	else
 	 {
 		fr = NULL;
 		rop = single_rop [rop];
 	 }
	
	pr_rop (DeviceBitmap (to), toPoint.dx, toPoint.dy, extent.dx, extent.dy, rop_map [rop], fr, fromPoint.dx, fromPoint.dy);
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
	 	case ROP_FALSE: 	rop = PIX_NOT (PIX_SET); 	break;
	 	case ROP_TRUE:  	rop = PIX_SET;  	break;
	 	case ROP_NOTDEST: 	rop = PIX_NOT (PIX_SRC); 	break;
	 	case ROP_DEST: 		return; 	break;
	 	
	 	default:
	 		fprintf (stderr, "illegal rasterop\n");
	 		exit (1);
	 }
	
	pr_vector (DeviceBitmap (h), fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy, rop, ~0);
 }

void BitBltBlob (to, top, height, left, right, rop) struct hardware *to; int top, height, *left, *right, rop;
 {
 	int i, op, offset = top;
 	struct pixrect *bm;
 	height += top;
 	switch (rop)
 	 {
 	 	case ROP_FALSE: 	op = PIX_NOT (PIX_SET); break;
 	 	case ROP_DEST: 		return;
 	 	case ROP_NOTDEST: 	op = PIX_NOT (PIX_SRC); break;
 	 	case ROP_TRUE: 		op = PIX_SET; break;
 	 }
 	bm = DeviceBitmap (to);
 	rop = rop_map [rop];
 	UpdateControl (to, FALSE);
 	for (i = top; i < height; i++)
 		pr_rop (bm, left[i - offset], i, right[i - offset] - left[i - offset], 1, op, bm, 0, 0);
 	UpdateControl (to, TRUE);
 }
