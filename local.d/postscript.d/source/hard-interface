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

struct hardware
 {
 	/*
 	 * Each driver is expected to provide its own definition of this structure.
 	 * It is only ever used as a pointer and is never dereferenced outside the driver.
 	 */
 	int pad;
 };

/*
 *	This file describes the interface that PostScript requires to the graphics system at Version 1.4.
 *	
 *	''Hardware'' in this context refers to a pointer to windows and/or bitmaps and is the lowest level
 *	of access that PostScript is interested in. Any Hardware parameter may be expected to be NULL.
 */

/*********************************** CREATION OF WINDOWS AND BITMAPS *******************/

struct hardware *InitHardware () {}
/*
 *	InitHardware () returns a default device which PostScript may use immediately (or NULL if not appropriate).
 *	Its size and shape are not defined. Most typically the user will want to start up another device
 *	before it is used anyway. No attempt will be made by PostScript to Destroy the resulting
 *	device.
 */

struct hardware *NewBitmapHardware (width, height) int width, height; {}

struct hardware *NewWindowHardware (width, height) int width, height; {}
/*
 *	NewBitmapHardware () is expected to create a new bitmap. Only one plane will be needed.
 *	
 *	NewWindowHardware () is expected to create a window on the screen. On a colour system this will
 *	be expected to support full colour.
 */

int IsWindowHardware (h) struct hardware *h; {}
/*
 *	IsWindowHardware () should return TRUE if the hardware is a window, FALSE otherwise.
 *	NULL is a window.
 */

void DestroyHardware (h) struct hardware *h; {}
/*	
 *	DestroyHardware () should release the resources required by the hardware, bitmap or window.
 *	This should cause a window device to vanish. NULL is not an error (does nothing).
 */


Matrix DeviceMatrix (width, height) int width, height; {}

/*
 *
 *	DeviceMatrix () should return a matrix appropriate to a device of the given height and width.
 *	For a typical display with a graphics origin at the top left of a window,
 *	an appropriate definition would be:
 *	
 *	Matrix DeviceMatrix (width, height) int width, height;
 *	 {
 *	 	return NewMatrix (PIXELS_PER_INCH / 72.0, 0.0, 0.0, -PIXELS_PER_INCH / 72.0, 0.0, (float) height);
 *	 }
 */

DevicePoint HardwareExtent (h) struct hardware *h; {}
/*	HardwareExtent () returns a DevicePoint describing the width and height of the argument.
 *	NULL has extent NewDevicePoint (0, 0).
 */

/******************************************* OUTPUT PRIMITIVES ******************************/	

void BitBlt (from, to, fromPoint, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint toPoint, fromPoint, extent;
	int rop;
 {}

void Paint (from, to, fromPoint, toPoint, extent, colour)
	struct hardware *from, *to;
	DevicePoint toPoint, fromPoint, extent;
	Colour colour;
 {}

/*	
 *	BitBlt () is a full function RasterOp. The 'rop' argument
 *	will have values as described in the header file hard.h. If the from argument is NULL it is taken to be
 *	a bitmap full of ones the shape of the fromPoint and extent. If the to argument is NULL, this is a no-op.
 *	
 *	Paint () is an addition to BitBlt. Bits that are set in the source are Painted into the destination
 *	in the given colour with a copying rasterop so that they replace pixels previously there. If the
 *	machine does not support colour windows, half-toning should be performed. 
 *	Colour values have hue, saturation and brightness components. on a black and white or greyscale
 *	system the brightness value will be a FP value between 0.0 (black) and 1.1 (white), which can be
 *	used as a grey level.
 *	
 *	Paint is expected to mask with the clip mask. BitBlt is not,
 */

void BitBltTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, rop)
	struct hardware *to;
	DevicePoint lefttop, leftbottom, righttop, rightbottom;
	int top, bottom, rop;
 {}

void PaintTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, colour)
	struct hardware *to;
	DevicePoint lefttop, leftbottom, righttop, rightbottom;
	int top, bottom;
	Colour colour;
 {}

/*
 * 	BitBltTrapezoid () and PaintTrapezoid () render a complete trapezoidal shape.
 *	The corners of the trapezoid may lie far outside the range of interesting scan-lines, but the slope
 *	of the line should be clipped by the top and bottom. The coordinates are half-open.
 */

void BitBltLine (h, fromPoint, toPoint, rop)
	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	int rop;
 {}

void PaintLine (h, fromPoint, toPoint, colour)
	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	Colour colour;
 {}

/*	
 *	BitBltLine () is expected to draw a line between the given points
 *	with the given RasterOp and colour masking.
 *	The line should be one pixel wide and half-open.
 *	[Thicker lines are done with BitBlt.]
 *	
 *	PaintLine () is expected to Paint a line by analogy with Paint and BitBlt.
 */

void BitBltBlob (to, top, height, left, right, rop)
	struct hardware *to;
	int top, height, *left, *right, rop;
 {}

 /*
  *	BitBltBlob () takes a set of pixel coordinates and fills the trapezon figure
  *	half open.
  */

void RasterTile (from, to, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint toPoint, extent;
	int rop;
 {}

/*
 *	RasterTile () replicates the whole of ``from'' over ``to'', but clipped by the
 *	rectangle bounded by ``toPoint'' and ``extent''.
 */

/*********************************** BRIGHTNESS TRANSFER FUNCTION ************************/

int TransferSize () {}

void SetTransfer (vec) float *vec; {}
/*	
 *	TransferSize () and SetTransfer () control the mapping function between user brightnesses
 *	and device brightnesses. The interface is expected to perform this mapping of brightnesses
 *	to a sufficient resolution. SetTransfer takes a table of floating point numbers between
 *	0 and 1. User brightnesses are scaled to the size of this table and mapped through it.
 *	The argument table given to SetTransfer () will be deleted after use. TransferSize () simply
 *	enquires the required size of the table.
 *	
 *	It may be appropriate to half-tone on a grayscale or colour device to improve rendering if it is not too
 *	expensive. TransferSize () returns the size of the pattern table.
 */

/********************************** BITMAP CONVERSION ********************************/

char *StringFromHardware (h) struct hardware *h; {}

struct hardware *HardwareFromString (s, width, height) char *s; int width, height; {}
/*	
 *	StringFromHardware () produces a string from its argument which describes the bitmap.
 *	The bitmap is returned in row-major order with the leftmost bit of each byte in the most significant
 *	position. Rows are padded to byte boundaries. Only single plane bitmaps are used.
 *	
 *	HardwareFromString () performs the inverse mapping, generating a bitmap from a set of bits, given
 *	a width and height. Only single plane bitmaps are used.
 */

/************************************* HALF-TONE SCREEN ***********************************/

int ScreenSize (freq, rotation) float freq, rotation; {}

void BuildScreen (freq, rotation, x, y) float freq, rotation, *x, *y; {}

void SetScreen (freq, rotation, thresh) float freq, rotation, *thresh; {}
/*
 *	ScreenSize () allows PostScript to determine how large an array of sample points to expect.
 *	It should return the length of the side of the sample square.
 *	
 *	BuildScreen () returns a set of sampling coordinates to PostScript to hand to the users spot-function
 *	
 *	SetScreen () allows PostScript to set the thresholds for each sample point so that half-tone bitmaps
 *	can be made.
 */

/************************************* CLIPPING **********************************************/

void SetClipHardware (h, clip) struct hardware *h, *clip; {}
/*	
 *	SetClipHardware sets hardware which is a clip mask for BitBlt. This mask should be ANDed with any output
 *	operation. If clip is NULL, masking will not be needed.
 */

/**************************************** UPDATE CONTROLS ******************************************/

void HardUpdate () {}
/*
 *	HardUpdate is a hook to allow devices which do output buffering to flush that buffering when appropriate.
 *	This allows an interactive user to see completed graphics between prompts (it is called as a side-effect
 *	of the PostScript flush operator). Typically is is a no-op.
 */

void UpdateControl (h, on) struct hardware *h; int on; {}
/*
 * 	This call can be used to enable batching of output operations. UpdateControl (h, FALSE) means ``start of
 *	batching'' UpdateControl (h, TRUE) means ``end of batching''. It is used to improve performance on machines
 *	where screen updates have a high locking overhead. It may be a no-op.
 *	The operation should nest if batching is already in progress: FALSE increments a counter,
 *	TRUE decrements a counter. Display changes are allowed when the counter is non-zero.
 */

/********************************** CANONICAL IMPLEMENTATION LIBRARY ******************************/

/*
 *	Some parts of the above interface can be supported by a canonical library.
 *	This library contains:

SetClipHardware
HardUpdate
IsWindowHardware
HardwareExtent

PaintTrapezoid
BitBltTrapezoid

Paint
PaintLine

DeviceMatrix
InitTransfer
TransferSize
SetTransfer
ScreenSize
BuildScreen
SetScreen

 *
 *	As the driver matures, the user may provide his own versions of the canonical routines.
 *	This leaves the following for implementation by the user.
 *

InitHardware
NewBitmapHardware
NewWindowHardware
DestroyHardware
HardwareFromString
StringFromHardware
UpdateControl
RasterTile
BitBlt
BitBltLine
BitBltBlob

 *	There is a pedagogical implementation in null.c
 *	
 *	There are a number of interface issues concerning the canonical driver.
 *	Firstly, a canonical struct hardware is defined, which contains a union of
 *	a char * and an int handle. The remainder are expected to use this to store
 *	device specific information.
 *
 *	InitTransfer() should be called during InitHardware with the number of pixels
 *	per inch on the display as an argument.
 */
