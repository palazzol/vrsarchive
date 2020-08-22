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

static int Xvalue (ax, ay, bx, by, cy) int ax, ay, bx, by, cy;
 {
 	return bx + (cy - by) * (ax - bx) / (float) (ay - by);
 }

void BitBltTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, rop)
	struct hardware *to;
	DevicePoint lefttop, leftbottom, righttop, rightbottom;
	int top, bottom, rop;
 {
 	int i, j, temp;
 	static int left [1024], right [1024];
 	
 	int 	ltx = Xvalue (lefttop.dx, 	lefttop.dy, 	leftbottom.dx, 	leftbottom.dy, 	top),
 		rtx = Xvalue (righttop.dx, 	righttop.dy, 	rightbottom.dx, rightbottom.dy, top),
 		lbx = Xvalue (lefttop.dx, 	lefttop.dy, 	leftbottom.dx, 	leftbottom.dy, 	bottom),
 		rbx = Xvalue (righttop.dx, 	righttop.dy, 	rightbottom.dx, rightbottom.dy, bottom);
 	
 	if (ltx == lbx && rtx == rbx)
 	 {
 	 	if (rtx < ltx)
 	 		temp = rtx, rtx = ltx, ltx = temp;
 	 	
 		BitBlt ((struct hardware *) NULL, to,
 			NewDevicePoint (0, 0), NewDevicePoint (ltx, top),
 			NewDevicePoint (rtx - ltx + 1, bottom - top + 1), rop);
 		
 		return;
 	 }
 	
 	for (i = top, j = 0; i <= bottom; i++, j++)
 	 {
 	 	int 	lx = Xvalue (lefttop.dx, 	lefttop.dy, 	leftbottom.dx, leftbottom.dy,  i),
 			rx = Xvalue (righttop.dx, 	righttop.dy, 	rightbottom.dx,rightbottom.dy, i);
 		
 		if (rx < lx)
 			temp = rx, rx = lx, lx = temp;
 		
 		left [j] = lx; right [j] = rx;
 	 }
 	BitBltBlob (to, top, bottom - top, left, right, single_rop [rop]);
 }
