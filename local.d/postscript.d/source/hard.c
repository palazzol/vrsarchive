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

struct hardware *InitHardware ()
 {
	InitTransfer (1);
	
	return NULL;
 }

static struct hardware *NewHardware (bits, width, height, flags) char *bits; int width, height, flags;
 {
 	struct hardware *d = (struct hardware *) Malloc (sizeof (struct hardware));
 	
 	d->hard.addr = bits;
 	d->flags = flags;
 	d->aux = d->clip = NULL;
 	d->extent = NewDevicePoint (width, height);
 	
 	return d;
 }

struct hardware *NewWindowHardware (width, height) int width, height;
 {
 	return NewHardware (NULL, width, height, ISWIN);
 }

struct hardware *NewBitmapHardware (width, height) int width, height;
 {
 	return NewHardware (NULL, width, height, 0);
 }

static void DestroyBits (b) char *b;
 {
 }

void DestroyHardware (dev) struct hardware *dev;
 {
 	if (dev == NULL)
 		return;
 	DestroyBits (dev->hard.addr);
 	if (dev->aux)
 		DestroyHardware (dev->aux);
 	Free ((char *) dev);
 }

struct hardware *HardwareFromString (st, width, height) char *st; int width, height;
 {
 	Free (st);
 	return NewHardware (NULL, width, height, 0);
 }

char *StringFromHardware (dev) struct hardware *dev;
 {
 	return Malloc ((dev->extent.dx + 7) / 8 *dev->extent.dy);
 }

void UpdateControl (h, flag) struct hardware *h; int flag;
 {
 }

void RasterTile (from, to, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint toPoint, extent;
	int rop;
 {
 }

void BitBlt (from, to, fromPoint, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, extent;
	int rop;
 {
 }

void BitBltLine (h, fromPoint, toPoint, rop) 
  	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	int rop;
 {
 }

void BitBltBlob (to, top, height, left, right, rop) struct hardware *to; int top, height, *left, *right, rop;
 {
 }
