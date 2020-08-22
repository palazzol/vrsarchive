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
#include <stdio.h>
#include <signal.h>

#include "main.h"
#include "graphics.h"
#include "protocol.h"

#define CHUNK 500

struct hardware **hards, *get_hard ();
char *malloc (), *realloc ();
void set_hard ();

int nhard = 0;

Colour Black, White;

int monitor;

main (argc, argv) int argc; char **argv;
 {
 	int c;
 	struct hardware *res;
 	DevicePoint ex;
 	
 	signal (SIGINT, SIG_IGN);
 	
 	monitor = !strcmp (argv[0], "monitor");
 	
 	if (monitor)
 		fprintf (stderr, "Monitoring\n");
 	slave_protocol ();
  	Black = NewColour (0.0, 0.0, 0.0);
  	White = NewColour (0.0, 0.0, 1.0);
	
 	hards = (struct hardware **) malloc (sizeof (struct hardware *) * CHUNK);
 	nhard = CHUNK;
 
 	(void) set_hard (1, res = InitHardware ());
 	ex = HardwareExtent (res);
 	send_short (ex.dx);
 	send_short (ex.dy);
 	flush_protocol ();
 	
	while ((c = recv_byte ()) != 255)
	 {
	 	if (monitor)
	 		fprintf (stderr, "command = %d\n", c);
		switch (c)
		 {
		 	case NEW_WINDOW: 	DoNewWindow (); 	break;
		 	case NEW_BITMAP: 	DoNewBitmap (); 	break;
		 	case BITBLT:		DoBitBlt (); 		break;
		 	case PAINT:		DoPaint (); 		break;
		 	case SEND_BITMAP:	DoSendBitmap (); 	break;
		 	case GET_BITMAP:	DoGetBitmap (); 	break;
		 	case DESTROY_HARDWARE:	DoDestroyHardware (); 	break;
		 	case GET_MATRIX:	DoGetMatrix (); 	break;
		 	case LINE:		DoLine (); 		break;
		 	case PAINT_LINE:	DoPaintLine (); 	break;
		 	case GET_TRANSFERSIZE:	DoGetTransferSize (); 	break;
		 	case SET_TRANSFER:	DoSetTransfer (); 	break;
			case HARD_FLUSH:	HardUpdate ();		break;
			case SCREEN_SIZE:	DoScreenSize ();	break;
			case BUILD_SCREEN:	DoBuildScreen ();	break;
			case SET_SCREEN:	DoSetScreen ();		break;
			case SET_CLIP_HARDWARE:	DoSetClipHardware ();	break;
		 	case BITBLT_TRAPEZOID:	DoBitBltTrapezoid ();	break;
		 	case PAINT_TRAPEZOID:	DoPaintTrapezoid ();	break;
		 	case SET_UPDATE_CONTROL:DoSetUpdateControl ();	break;
		 	
		 	default:
		 		fprintf (stderr, "Unknown request type %d\n", c);
		 		exit (1);
		 }
	 }
 }

DoNewWindow ()
 {
 	int h = recv_hardware ();
 	int width = recv_short ();
 	int height = recv_short ();
 	
 	if (monitor)
 		fprintf (stderr, "%d = NewWindowHardware (%d, %d)\n", h, width, height);
	set_hard (h, NewWindowHardware (width, height));
 }

DoNewBitmap ()
 {
 	int h = recv_hardware ();
 	int width = recv_short ();
 	int height = recv_short ();
 	
 	if (monitor)
 		fprintf (stderr, "%d = NewBitmapHardware (%d, %d)\n", h, width, height);
 	set_hard (h, NewBitmapHardware (width, height));
 }

DoBitBlt ()
 {
 	int rop, f, t;
 	DevicePoint fromPoint, toPoint, extent;
 	struct hardware *from = get_hard (f = recv_hardware ());
 	struct hardware *to = get_hard (t = recv_hardware ());
	
	fromPoint = recv_point ();
	toPoint = recv_point ();
	extent = recv_point ();
	rop = recv_byte ();
	
 	if (monitor)
 		fprintf (stderr, "BitBlt (%d, %d, (%d, %d), (%d, %d), (%d, %d), %d)\n",
 		f, t,
 		fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy,
 		extent.dx, extent.dy,
 		rop);
	BitBlt (from, to, fromPoint, toPoint, extent, rop);
 }

DoPaint ()
 {
 	int f, t;
 	Colour colour;
 	DevicePoint fromPoint, toPoint, extent;
 	struct hardware *from = get_hard (f = recv_hardware ());
 	struct hardware *to = get_hard (t = recv_hardware ());
	
	fromPoint = recv_point ();
	toPoint = recv_point ();
	extent = recv_point ();
	colour = recv_colour ();
	
 	if (monitor)
 		fprintf (stderr, "Paint (%d, %d, (%d, %d), (%d, %d), (%d, %d), [%g, %g, %g])\n",
 		f, t,
 		fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy,
 		extent.dx, extent.dy,
 		colour.hue, colour.saturation, colour.brightness);
	Paint (from, to, fromPoint, toPoint, extent, colour);
 }

int size (width, height) int width, height;
 {
 	return (width + 7) / 8 * height;
 }

DoSendBitmap ()		/* actually receives this end */
 {
 	int h = recv_hardware ();
 	int width = recv_short ();
 	int height = recv_short ();
	int len = size (width, height);
 	char *s;
 	
 	recv_string ((s = malloc (len)), len);
 	if (monitor)
 		fprintf (stderr, "%d = HardwareFromString (<string>, %d, %d)\n", h, width, height);
 	set_hard (h, HardwareFromString (s, width, height));
 	free (s);
 }

DoGetBitmap ()
 {
 	struct hardware *h = get_hard (recv_hardware ());
 	char *s = StringFromHardware (h);
 	DevicePoint ex;
 	
 	ex = HardwareExtent (h);
 	
 	if (monitor)
 		fprintf (stderr, "StringFromHardware (%d)\n", h);
 	send_string (s, size (ex.dx, ex.dy));
 	flush_protocol ();
 	free (s);
 }

DoDestroyHardware ()
 {
 	int n = recv_hardware ();
 	
 	if (monitor)
 		fprintf (stderr, "DestroyHardware (%d)\n", n);
 	DestroyHardware (get_hard (n));
 	set_hard (n, NULL);
 }

DoGetMatrix ()
 {
	int width = recv_short ();
	int height = recv_short ();
	Matrix m;
	
 	if (monitor)
 		fprintf (stderr, "DeviceMatrix (%d, %d)\n", width, height);
	m = DeviceMatrix (width, height);
	send_float (m.A);
	send_float (m.B);
	send_float (m.C);
	send_float (m.D);
	send_float (m.tx);
	send_float (m.ty);
	flush_protocol ();
 }

DoLine ()
 {
 	DevicePoint fromPoint, toPoint;
 	int rop;
 	struct hardware *h = get_hard (recv_hardware ());
 	
	fromPoint = recv_point ();
	toPoint = recv_point ();
	rop = recv_byte ();
	
 	if (monitor)
 		fprintf (stderr, "BitBltLine ((%d, %d), (%d, %d), %d)\n",
 		fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy,
 		rop);
	BitBltLine (h, fromPoint, toPoint, rop);
 }

DoPaintLine ()
 {
 	DevicePoint fromPoint, toPoint;
 	Colour colour;
 	struct hardware *h = get_hard (recv_hardware ());
 	
	fromPoint = recv_point ();
	toPoint = recv_point ();
	colour = recv_colour ();
	
 	if (monitor)
 		fprintf (stderr, "PaintLine ((%d, %d), (%d, %d), [%g, %g, %g])\n",
 		fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy,
 		colour.hue, colour.saturation, colour.brightness);
	PaintLine (h, fromPoint, toPoint, colour);
 }

int recv_hardware ()
 {
 	return recv_short ();
 }

struct hardware *get_hard (h) int h;
 {
 	if (nhard <= h)
 	 {
 		fprintf (stderr, "%d is not a valid device\n", h);
 		return NULL;
 	 }
 	return hards [h];
 }

void set_hard (n, h) int n; struct hardware *h;
 {
 	if (nhard <= n)
		hards = (struct hardware **) realloc ((char *) hards, sizeof (struct hardware *) * (nhard += CHUNK));
 	hards [n] = h;
 }

int Max (a, b) int a, b;
 {
 	return a > b ? a : b;
 }

int Min (a, b) int a, b;
 {
 	return a < b ? a : b;
 }

Matrix NewMatrix (a, b, c, d, e, f) float a, b, c, d, e, f;
 {
 	Matrix res;
 	
 	res.A = a; res.B = b; res.C = c; res.D = d; res.tx = e; res.ty = f;
 	
 	return res;
 }

char *Malloc (i) unsigned i;
 {
 	return malloc (i);
 }

Free (s) char *s;
 {
 	free (s);
 }

unsigned char *Bcopy (to, from, count) unsigned char *from, *to; int count;
 {
 	unsigned char *res = to;
 	
 	while (count --)
 		*to++ = *from++;
 	
 	return res;
 }

PanicIf (flag, s) int flag; char *s;
 {
 	if (flag)
 		fprintf (stderr, "Viewer panic: %s\n", s),
 		exit (1);
 }

DevicePoint NewDevicePoint (x, y) int x, y;
 {
 	DevicePoint res;
 	
 	res.dx = x; res.dy = y;
 	
 	return res;
 }

Colour NewColour (h, s, b) float h, s, b;
 {
 	Colour res;
 	
 	res.hue = h;
 	res.saturation = s;
 	res.brightness = b;
 	
 	return res;
 }

DoGetTransferSize ()
 {
 	send_short (TransferSize ());
 	flush_protocol ();
 }

DoSetTransfer ()
 {
 	int i, size = TransferSize ();
 	float *val = (float *) malloc (sizeof (float) * size);
 	
 	for (i = 0; i < size; i++)
 		val [i] = recv_small ();
 	
 	SetTransfer (val);
 	Free ((char *) val);
 }

DoScreenSize ()
 {
 	float freq = recv_float (), rotation = recv_float ();
 	
 	send_short (ScreenSize (freq, rotation));
 	flush_protocol ();
 }

DoBuildScreen ()
 {
 	float freq = recv_float (), rotation = recv_float ();
 	int i, size = ScreenSize (freq, rotation);
 	float *x, *y;
 	
 	send_short (size);
 	x = (float *) malloc (sizeof (float) * size);
 	y = (float *) malloc (sizeof (float) * size);
 	
 	BuildScreen (freq, rotation, x, y);
 	for (i = 0; i < size; i++)
 	 	send_small (x[i]),
 	 	send_small (y[i]);
 	flush_protocol ();
 	free ((char *) x);
 	free ((char *) y);
 }

DoSetScreen ()
 {
 	float freq = recv_float (), rotation = recv_float ();
  	int i, size = ScreenSize (freq, rotation);
	float *th = (float *) malloc (sizeof (float) * size);
	
	for (i = 0; i < size; i++)
		th[i] = recv_small ();
	SetScreen (freq, rotation, th);
	free ((char *) th);
 }

DoSetClipHardware ()
 {
 	struct hardware *h = get_hard (recv_hardware ());
 	
 	if (monitor)
 		fprintf (stderr, "SetClipHardware ()\n");
	SetClipHardware (h, get_hard (recv_hardware ()));
 }

DoBitBltTrapezoid ()
 {
 	DevicePoint lefttop, leftbottom, righttop, rightbottom;
 	int top, bottom, rop;
 	struct hardware *h = get_hard (recv_hardware ());
 	
 	lefttop = recv_point ();
 	leftbottom = recv_point ();
 	righttop = recv_point ();
 	rightbottom = recv_point ();
 	top = recv_short ();
 	bottom = recv_short ();
 	rop = recv_byte ();
 	
 	if (monitor)
 		fprintf (stderr, "BitBltTrapezoid ()\n");
 	BitBltTrapezoid (h, lefttop, leftbottom, righttop, rightbottom, rop);
 }

DoPaintTrapezoid ()
 {
 	DevicePoint lefttop, leftbottom, righttop, rightbottom;
 	int top, bottom;
 	Colour colour;
 	struct hardware *h = get_hard (recv_hardware ());
 	
 	lefttop = recv_point ();
 	leftbottom = recv_point ();
 	righttop = recv_point ();
 	rightbottom = recv_point ();
 	top = recv_short ();
 	bottom = recv_short ();
 	colour = recv_colour ();
 	
  	if (monitor)
 		fprintf (stderr, "PaintTrapezoid ()\n");
	PaintTrapezoid (h, lefttop, leftbottom, righttop, rightbottom, top, bottom, colour);
 }

DoSetUpdateControl ()
 {
 	struct hardware *h = get_hard (recv_hardware ());
 	UpdateControl (h, recv_byte ());
 }
