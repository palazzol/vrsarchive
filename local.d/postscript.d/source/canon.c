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

int pixels_per_inch;

int single_rop [] =
 {
	ROP_FALSE, ROP_DEST, ROP_NOTDEST, ROP_TRUE,
	ROP_FALSE, ROP_DEST, ROP_NOTDEST, ROP_TRUE,
	ROP_FALSE, ROP_DEST, ROP_NOTDEST, ROP_TRUE,
	ROP_FALSE, ROP_DEST, ROP_NOTDEST, ROP_TRUE
 };

/*ARGSUSED*/
Matrix DeviceMatrix (width, height) int width, height;
 {
 	return NewMatrix (pixels_per_inch / 72.0, 0.0, 0.0, -pixels_per_inch / 72.0, 0.0, (float) height);
 }

void HardUpdate ()
 {
 }

int IsWindowHardware (h) struct hardware *h;
 {
 	return h->flags & ISWIN;
 }

#define IsWindowHardware(h) ((h)->flags & ISWIN)

DevicePoint HardwareExtent (h) struct hardware *h;
 {
 	if (h)
 		return h->extent;
 	else
 		return NewDevicePoint (0, 0);
 }

void SetClipHardware (h, clip) struct hardware *h, *clip;
 {
 	if (h)
		h->clip = clip;
 }
