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

static NeedAux (h) struct hardware *h;
 {
 	DevicePoint extent;
 	
 	if (h->aux)
 		return;
 	extent = HardwareExtent (h);
 	h->aux = NewBitmapHardware (extent.dx, extent.dy);
 }

static void RasterThreeAdd (from, aux, to, fromPoint, toPoint, extent, rop)	/* depends on aux */
	struct hardware *from, *aux, *to;
	DevicePoint fromPoint, toPoint, extent;
	int rop;
 {
 	BitBlt (from, aux, fromPoint, toPoint, extent, ROP_AND);
 	BitBlt (aux, to, toPoint, toPoint, extent, rop);
 }

static void RasterFour (from, mask, aux, to, fromPoint, toPoint, extent, rop)
	struct hardware *from, *mask, *aux, *to;
	DevicePoint fromPoint, toPoint, extent;
	int rop;
 {
 	if (mask == NULL)
 		BitBlt (from, to, fromPoint, toPoint, extent, rop);
 	else if (from == NULL)
 		BitBlt (mask, to, toPoint, toPoint, extent, rop);
 	else
 	 {
 	 	BitBlt (from, aux, fromPoint, toPoint, extent, ROP_SOURCE);
 	 	RasterThreeAdd (mask, aux, to, toPoint, toPoint, extent, rop);
 	 }
 }

void Paint  (from, to, fromPoint, toPoint, extent, colour)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, extent;
	Colour colour;
 {
  	int col;
  	
 	if (to == NULL)
 		return;
 	
 	UpdateControl (to, FALSE);
 	
 	col = IsWindowHardware (to) ? HardColour (colour) : 0;
 	
 	if (to->clip )
 		NeedAux (to);	
 	if (col == 0)
 		if (to->clip == NULL && from != NULL)
 		 	BitBlt (from, to, fromPoint, toPoint, extent, ROP_OR);
 		else
			RasterFour (from, to->clip, to->aux, to, fromPoint, toPoint, extent, ROP_OR);
	else if (col == TransferSize () - 1)
		RasterFour (from, to->clip, to->aux, to, fromPoint, toPoint, extent, ROP_NOTAND);
 	else
 	 { 
 	 	struct hardware *gray = GraySync (col);
  		NeedAux (to);
	 	if (from)
	 	 {
	 		BitBlt (from, to->aux, fromPoint, toPoint, extent, ROP_SOURCE);
	 		if (to->clip)
	 			BitBlt (to->clip, to->aux, toPoint, toPoint, extent, ROP_AND);
	 		BitBlt (to->aux, to, toPoint, toPoint, extent, ROP_NOTAND);
	 		RasterTile (gray, to->aux, toPoint, extent, ROP_AND);
	 		BitBlt (to->aux, to, toPoint, toPoint, extent, ROP_OR);
	 	 }
	 	else
	 	 {
	 	 	if (to->clip)
	 	 	 {
	 	 		BitBlt (to->clip, to->aux, toPoint, toPoint, extent, ROP_SOURCE);
	 			BitBlt (to->aux, to, toPoint, toPoint, extent, ROP_NOTAND);
	 			RasterTile (gray, to->aux, toPoint, extent, ROP_AND);
	 			BitBlt (to->aux, to, toPoint, toPoint, extent, ROP_OR);
	 		 }
	 		else
	 		 	RasterTile (gray, to, toPoint, extent, ROP_SOURCE);
	 	 	
	 	 }
 	 }
 	UpdateControl (to, TRUE);
 }

void PaintLine (h, fromPoint, toPoint, colour)
	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	Colour colour;
 {
 	int col;
 	
 	if (h == NULL)
 		return;
 	
 	col = IsWindowHardware (h) ? HardColour (colour) : 0;
 	
 	UpdateControl (h, FALSE);
 	
 	if (h->clip == NULL && (col == 0 || col == TransferSize () - 1))
	 	BitBltLine (h, fromPoint, toPoint, (col == 0 ? ROP_TRUE : ROP_FALSE));
	else
 	 {
		int mx = Min (fromPoint.dx, toPoint.dx), my = Min (fromPoint.dy, toPoint.dy),
			Mx = Max (fromPoint.dx, toPoint.dx), My = Max (fromPoint.dy, toPoint.dy);
  		DevicePoint orig, ex;
  		
  		orig = NewDevicePoint (mx, my);
  		ex = NewDevicePoint (Mx - mx + 1, My - my + 1);
  		
		NeedAux (h);
 		BitBlt ((struct hardware *) NULL, h->aux, orig, orig, ex, ROP_FALSE);
 		
 		BitBltLine (h->aux, fromPoint, toPoint, ROP_TRUE);
		
		BitBlt (h->clip, h->aux, orig, orig, ex, ROP_AND);
		BitBlt (h->aux, h, orig, orig, ex, ROP_NOTAND);
		
		if (col != 0 && col != TransferSize () - 1)
		 {
		 	struct hardware *gray = GraySync (col);
		 	RasterTile (gray, h->aux, orig, ex, ROP_AND);
		 }
		BitBlt (h->aux, h, orig, orig, ex, ROP_OR);
	 }
	UpdateControl (h, TRUE);
 }

void PaintTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, colour)
	struct hardware *to;
	DevicePoint lefttop, leftbottom, righttop, rightbottom;
	int top, bottom;
	Colour colour;
 {
  	int col;
  	
 	if (to == NULL)
 		return;
 	
 	col = IsWindowHardware (to) ? HardColour (colour) : 0;
 	
 	UpdateControl (to, FALSE);
 	
 	if (to->clip)
 		NeedAux (to);	
 	if (col == 0)
		BitBltTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, ROP_TRUE);
	else if (col == TransferSize () - 1)
		BitBltTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, ROP_FALSE);
 	else
 	 {
 	 	DevicePoint origin, extent;
 	 	struct hardware *gray;
 	 	
 	 	int left = lefttop.dx < leftbottom.dx ? lefttop.dx : leftbottom.dx,
 	 		right = righttop.dx > rightbottom.dx ? righttop.dx : rightbottom.dx;
 	 	
 	 	if (righttop.dx < left)
 	 		left = righttop.dx;
 	 	if (rightbottom.dx < left)
 	 		left = rightbottom.dx;
 	 	if (lefttop.dx > right)
 	 		right = lefttop.dx;
 	 	if (leftbottom.dx > right)
 	 		right = leftbottom.dx;
 	 	origin = NewDevicePoint (left, top);
 	 	extent = NewDevicePoint (right - left, bottom - top);
 	 	
 	 	gray = GraySync (col);
  		NeedAux (to);
 		BitBlt ((struct hardware *) NULL, to->aux, origin, origin, extent, ROP_FALSE);
		BitBltTrapezoid (to->aux, lefttop, leftbottom, righttop, rightbottom, top, bottom, ROP_TRUE);
 		if (to->clip)
 			BitBlt (to->clip, to->aux, origin, origin, extent, ROP_AND);
 		BitBlt (to->aux, to, origin, origin, extent, ROP_NOTAND);
 		RasterTile (gray, to->aux, origin, extent, ROP_AND);
 		BitBlt (to->aux, to, origin, origin, extent, ROP_OR);
 	 }
  	UpdateControl (to, TRUE);
 }
