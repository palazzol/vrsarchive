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
#include <stdio.h>

#include "protocol.h"

struct hardware
 {
 	struct hardware *next;
 	int serial;
 	int width, height, iswin;
 };

static struct hardware *alloc_hard ();
char *malloc ();
void send_hardware ();
static void GetTransferSize ();

static int transfer_size;

struct hardware *InitHardware ()
 {
 	int width, height;
 	
 	master_protocol ();
 	Message ("InitHardware");
 	
 	width = recv_short (); height = recv_short ();
 	GetTransferSize ();
 	return alloc_hard (width, height, TRUE);
 }

static void GetTransferSize ()
 {
 	send_byte (GET_TRANSFERSIZE);
 	flush_protocol ();
	transfer_size = recv_short ();
 }

int TransferSize ()
 {
 	return transfer_size;
 }

void SetTransfer (tran) float *tran;
 {
	int i;
	
	send_byte (SET_TRANSFER);
	for (i = 0; i < transfer_size; i++)
		send_small (tran [i]);
 }

int ScreenSize (freq, rot) float freq, rot;
 {
 	send_byte (SCREEN_SIZE);
 	send_float (freq);
 	send_float (rot);
 	flush_protocol ();
 	
 	return recv_short ();
 }

void BuildScreen (freq, rotation, x, y) float freq, rotation, *x, *y;
 {
 	int i, size;
 	
 	send_byte (BUILD_SCREEN);
 	send_float (freq);
 	send_float (rotation);
 	flush_protocol ();
 	size = recv_short ();
 	
 	for (i = 0; i < size; i++)
 	 {
 		x[i] = recv_small ();
 		y[i] = recv_small ();
 	 }
 }

void SetScreen (freq, rotation, thresh) float freq, rotation, *thresh;
 {
 	int i, size = ScreenSize (freq, rotation);
 	send_byte (SET_SCREEN);
 	send_float (freq);
 	send_float (rotation);
 	
 	for (i = 0; i < size; i++)
 		send_small (thresh [i]);
 }

struct hardware *NewWindowHardware (width, height) int width, height;
 {
 	struct hardware *res = alloc_hard (width, height, TRUE);
	Message ("NewWindowHardware");
	send_byte (NEW_WINDOW);
	send_hardware (res);
	send_short (width);
	send_short (height);
	
	return res;
 }

struct hardware *NewBitmapHardware (width, height) int width, height;
 {
 	struct hardware *res = alloc_hard (width, height, FALSE);
 	
 	Message ("NewBitmapHardware");
 	send_byte (NEW_BITMAP);
 	send_hardware (res);
 	send_short (width);
 	send_short (height);
 	
 	return res;
 }

int IsWindowHardware (h) struct hardware *h;
 {
 	if (h == NULL)
 		return TRUE;
 	return h->iswin;
 }

void DestroyHardware (h) struct hardware *h;
 {
	if (h == NULL)
		return;
	Message ("DestroyHardware");
	send_byte (DESTROY_HARDWARE);
	send_hardware (h);
	free_hard (h);
 }

void BitBlt (from, to, fromPoint, toPoint, ex, rop)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, ex;
	int rop;
 {
 	if (to == NULL)
 		return;
 	Message ("BitBlt");
 	send_byte (BITBLT);
 	send_hardware (from);
 	send_hardware (to);
 	send_point (fromPoint);
 	send_point (toPoint);
 	send_point (ex);
 	send_byte (rop);
 }

void Paint (from, to, fromPoint, toPoint, ex, colour)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, ex;
	Colour colour;
 {
 	if (to == NULL)
 		return;
 	Message ("Paint");
 	send_byte (PAINT);
 	send_hardware (from);
 	send_hardware (to);
 	send_point (fromPoint);
 	send_point (toPoint);
 	send_point (ex);
 	send_colour (colour);
 }

Matrix DeviceMatrix (width, height) int width, height;
 {
 	Matrix m;
 	
 	Message ("DeviceMatrix");
 	send_byte (GET_MATRIX);
 	send_short (width);
 	send_short (height);
 	flush_protocol ();
 	m.A = recv_float ();
 	m.B = recv_float ();
 	m.C = recv_float ();
 	m.D = recv_float ();
 	m.tx = recv_float ();
 	m.ty = recv_float ();
 	
 	return m;
 }

DevicePoint HardwareExtent (h) struct hardware *h;
 {
	return h == NULL ? NewDevicePoint (0, 0) : NewDevicePoint (h->width, h->height);
 }

struct hardware *HardwareFromString (s, width, height) unsigned char *s; int width, height;
 {
	struct hardware *res = alloc_hard (width, height, FALSE);
	
	Message ("HardwareFromString");
	send_byte (SEND_BITMAP);
	send_hardware (res);
 	send_short (res->width);
 	send_short (res->height);
	send_string (s, hard_size (res));
	
	return res;
 }

char *StringFromHardware (h) struct hardware *h;
 {
  	unsigned len = hard_size (h);
  	char *s;
  	
  	Message ("StringFromHardware");
	send_byte (GET_BITMAP);
 	send_hardware (h);
 	flush_protocol ();
 	recv_string ((s = malloc (len)), len);
 	
 	return s;
 }

void Line (h, fromPoint, toPoint, rop)
	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	int rop;
 {
 	if (h == NULL)
 		return;
 	Message ("Line");
 	send_byte (LINE);
 	send_hardware (h);
 	send_point (fromPoint);
 	send_point (toPoint);
 	send_byte (rop);
 }

void PaintLine (h, fromPoint, toPoint, colour)
	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	Colour colour;
 {
 	if (h == NULL)
 		return;
 	Message ("PaintLine");
 	send_byte (PAINT_LINE);
 	send_hardware (h);
 	send_point (fromPoint);
 	send_point (toPoint);
 	send_colour (colour);
 }

static struct hardware *FreeList = NULL;
static int next_serial = 1;

static struct hardware *alloc_hard (width, height, iswin) int width, height, iswin;
 {
 	struct hardware *res;
 	
 	if (FreeList == NULL)
 	 {
 		res = (struct hardware *) malloc (sizeof (struct hardware));
 		res->serial = next_serial++;
 	 }
 	else
 	 {
 	 	res = FreeList;
 	 	FreeList = FreeList->next;
 	 }
 	res->width = width;
 	res->height = height;
 	res->iswin = iswin;
 	
 	return res;
 }

free_hard (h) struct hardware *h;
 {
 	if (h == NULL)
 		return;
 	h->next = FreeList;
 	FreeList = h;
 }

int size (width, height) int width, height;
 {
 	return (width + 7) / 8 * height;
 }

int hard_size (h) struct hardware *h;
 {
 	if (h == NULL)
 		return 0;
 	return size (h->width, h->height);
 }
	
void send_hardware (h) struct hardware *h;
 {
	send_short (h == NULL ? 0 : h->serial);
 }

void HardUpdate ()
 {
	send_byte (HARD_FLUSH);
 	flush_protocol ();
 }

void UpdateControl (h, flag) struct hardware *h; int flag;
 {
 	send_byte (SET_UPDATE_CONTROL);
 	send_hardware (h);
 	send_byte (flag);
 }

void SetClipHardware (h, clip) struct hardware *h, *clip;
 {
 	send_byte (SET_CLIP_HARDWARE);
 	send_hardware (h);
 	send_hardware (clip);
 }

void PaintTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, colour)
	struct hardware *to;
	DevicePoint lefttop, leftbottom, righttop, rightbottom;
	int top, bottom;
	Colour colour;
 {
 	if (to == NULL)
 		return;
 	Message ("PaintTrapezoid");
 	send_byte (PAINT_TRAPEZOID);
 	send_hardware (to);
 	send_point (lefttop);
 	send_point (leftbottom);
 	send_point (righttop);
 	send_point (rightbottom);
 	send_short (top);
 	send_short (bottom);
 	send_colour (colour);
 }

void BitBltTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, rop)
	struct hardware *to;
	DevicePoint lefttop, leftbottom, righttop, rightbottom;
	int top, bottom, rop;
 {
 	if (to == NULL)
 		return;
 	Message ("BitBltTrapezoid");
 	send_byte (BITBLT_TRAPEZOID);
 	send_hardware (to);
 	send_point (lefttop);
 	send_point (leftbottom);
 	send_point (righttop);
 	send_point (rightbottom);
 	send_short (top);
 	send_short (bottom);
 	send_byte (rop);
 }
