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
#include "X/Xlib.h"

#include "main.h"
#include "graphics.h"

#define TRANSFER_SIZE	256
#define XMAX		65535

static int transfer [TRANSFER_SIZE + 1];

static unsigned char reverse [0x100];

struct hardware
 {
 	Window win;
 	struct bitmap *bm;
 	int width, height;
 };

struct bitmap
 {
 	short *data;
 	Bitmap xbm;
 	int width, height;
 };

struct hardcolour
 {
 	int pixel;
 	Pixmap tile;
 };

struct screen
 {
 	float val;
 	int sx, sy;
 	Pixmap shade;
 } *screen = NULL;

static int screen_size, screen_side;

char *malloc ();

static Display *theDisplay;
static float magnification;

static void InitTransfer ();
static struct hardcolour HardColour ();

struct hardware *InitHardware ()
 {
	if ((theDisplay = XOpenDisplay ("")) == NULL)
	 {
	 	fprintf (stderr, "Could not open display\n");
	 	exit (1);
	 }
	magnification = DisplayHeight () / (11.0 * 72);
	XSetDisplay (theDisplay);
	InitReversal ();
	InitRopMap ();
	InitTransfer ();
	
	return 0;
 }

static struct hardware *NewHardware (win, bm, width, height) Window win; struct bitmap *bm; int width, height;
 {
 	struct hardware *res = (struct hardware *) malloc (sizeof (struct hardware));
 	
 	res->win = win;
 	res->bm = bm;
 	res->width = width;
 	res->height = height;
 	
 	return res;
 }

SetClipHardware (bm, clip) struct hardware *bm, *clip;
 {
 }

int hard_size (width, height) int width, height;
 {
	return ((width + 15) / 16) * height;
 }

static struct bitmap *NewBM (width, height) int width, height;
 {
 	struct bitmap *res = (struct bitmap *) malloc (sizeof (struct bitmap));
 	
 	res->data = (short *) calloc (hard_size (width, height), sizeof (short));
 	res->width = width;
 	res->height = height;
	res->xbm = 0;
 	
 	return res;
 }

static void DestroyBM (bm) struct bitmap *bm;
 {
 	free ((char *) bm->data);
 	if (bm->xbm != 0)
 		XFreeBitmap (bm->xbm);
 	free ((char *) bm);
 }

static Bitmap NeedBitmap (bm) struct bitmap *bm;
 {
 	if (bm->xbm == 0)
	 {
 		bm->xbm = XStoreBitmap (bm->width, bm->height, bm->data);
	 }
 	return bm->xbm;
 }

static OODBitmap (bm) struct bitmap *bm;
 {
 	if (bm->xbm != 0)
 	 {
 		XFreeBitmap (bm->xbm);
 		bm->xbm = 0;
 	 }
 }

struct hardware *NewWindowHardware (width, height) int width, height;
 {
 	Window win = XCreateWindow (RootWindow, 0, 0, width, height, 1, BlackPixmap, WhitePixmap);
 	
 	if (win == 0)
 		return NULL;
 	XMapWindow (win);
 	
 	return NewHardware (win, NULL, width, height);
 }

struct hardware *NewBitmapHardware (width, height) int width, height;
 {
 	return NewHardware (0, NewBM (width, height), width, height);
 }

void DestroyHardware (h) struct hardware *h;
 {
 	if (IsWindowHardware (h))
 		XDestroyWindow (h->win);
 	else
 		DestroyBM (h->bm);
 	free ((char *) h);
 }

Matrix DeviceMatrix (width, height) int width, height;
 {
 	return NewMatrix (magnification, 0.0, 0.0, -magnification, 0.0, (float) height);
 }

DevicePoint HardwareExtent (h) struct hardware *h;
 {
 	return h == NULL ? NewDevicePoint (0, 0) : NewDevicePoint (h->width, h->height);
 }

int IsWindowHardware (h) struct hardware *h;
 {
 	return h->win != 0;
 }

static int single_rop [16];

static InitRopMap ()
 {
	single_rop [ROP_FALSE] 	= single_rop [ROP_NOTAND] = single_rop [ROP_NOR]     = single_rop [ROP_NOTSOURCE] = ROP_FALSE;
	single_rop [ROP_AND] 	= single_rop [ROP_DEST]   = single_rop [ROP_NXOR]    = single_rop [ROP_NOTOR] 	  = ROP_DEST;
	single_rop [ROP_ANDNOT] = single_rop [ROP_XOR]	  = single_rop [ROP_NOTDEST] = single_rop [ROP_NAND] 	  = ROP_NOTDEST;
	single_rop [ROP_SOURCE] = single_rop [ROP_OR] 	  = single_rop [ROP_ORNOT]   = single_rop [ROP_TRUE] 	  = ROP_TRUE;
 }

static int Xvalue (ax, ay, bx, by, cy) int ax, ay, bx, by, cy;
 {
 	return bx + (cy - by) * (ax - bx) / (float) (ay - by);
 }

static void PaintBlob (to, top, bottom, left, right, col)
	struct hardware *to;
	int top, bottom, *left, *right;
	struct hardcolour col;
 {
 	int i, op, offset = top;
 	
	if (col.tile == 0)
  		for (i = top; i <= bottom; i++)
 	 		XPixSet (to->win, left[i - offset], i, right[i - offset] - left[i - offset] + 1, 1, col.pixel);
 	else
 		for (i = top; i <= bottom; i++)
 			XTileSet (to->win, left[i - offset], i, right[i - offset] - left[i - offset] + 1, 1, col.tile);
 }

void PaintTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, colour)
	struct hardware *to;
	DevicePoint lefttop, leftbottom, righttop, rightbottom;
	int top, bottom;
	Colour colour;
 {
 	struct hardcolour col;
 	int i, j, temp; char c;
 	static int left [1024], right [1024];
 	
 	int 	ltx = Xvalue (lefttop.dx, 	lefttop.dy, 	leftbottom.dx, 	leftbottom.dy, 	top),
 		rtx = Xvalue (righttop.dx, 	righttop.dy, 	rightbottom.dx, rightbottom.dy, top),
 		lbx = Xvalue (lefttop.dx, 	lefttop.dy, 	leftbottom.dx, 	leftbottom.dy, 	bottom),
 		rbx = Xvalue (righttop.dx, 	righttop.dy, 	rightbottom.dx, rightbottom.dy, bottom);
 	
 	col = HardColour (colour);
 	
 	if (ltx == lbx && rtx == rbx)
 	 {
 	 	if (rtx < ltx)
 	 		temp = rtx, rtx = ltx, ltx = temp;
 	 	
 		if (col.tile == 0)
 	 		XPixSet (to->win, ltx, top, rtx - ltx + 1, bottom - top + 1, col.pixel);
 		else
			XTileSet (to->win, ltx, top, rtx - ltx + 1, bottom - top + 1, col.tile);
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
 	PaintBlob (to, top, bottom, left, right, col);
 }

static void RasterBlob (to, top, bottom, left, right, rop)
	struct hardware *to;
	int top, bottom, *left, *right, rop;
 {
 	int i, op, offset = top;
 	
 	switch (rop)
 	 {
 	 	case ROP_FALSE: 	op = GXclear; break;
 	 	case ROP_DEST: 		return;
 	 	case ROP_NOTDEST: 	op = GXinvert; break;
 	 	case ROP_TRUE: 		op = GXset; break;
 	 }
 	for (i = top; i <= bottom; i++)
 		XCopyArea (to->win, 0, 0, left[i - offset], i, right[i - offset] - left[i - offset] + 1, 1, op, ~0);
 }

static void RasterTrapezoid (to, lefttop, leftbottom, righttop, rightbottom, top, bottom, rop)
	struct hardware *to;
	DevicePoint lefttop, leftbottom, righttop, rightbottom;
	int top, bottom, rop;
 {
 	int i, j, temp; char c;
 	static int left [1024], right [1024];
 	
 	int 	ltx = Xvalue (lefttop.dx, 	lefttop.dy, 	leftbottom.dx, 	leftbottom.dy, 	top),
 		rtx = Xvalue (righttop.dx, 	righttop.dy, 	rightbottom.dx, rightbottom.dy, top),
 		lbx = Xvalue (lefttop.dx, 	lefttop.dy, 	leftbottom.dx, 	leftbottom.dy, 	bottom),
 		rbx = Xvalue (righttop.dx, 	righttop.dy, 	rightbottom.dx, rightbottom.dy, bottom);
 	
 	if (ltx == lbx && rtx == rbx)
 	 {
 	 	int op;
 	 	
 	 	if (rtx < ltx)
 	 		temp = rtx, rtx = ltx, ltx = temp;
 	 	
 		switch (rop)
 		 {
 	 		case ROP_FALSE: 	op = GXclear; break;
 	 		case ROP_DEST: 		return;
 	 		case ROP_NOTDEST: 	op = GXinvert; break;
 	 		case ROP_TRUE: 		op = GXset; break;
 		 }
 		XCopyArea (to->win, 0, 0, ltx, top, rtx - ltx + 1, bottom - top + 1, op, ~0);
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
 	RasterBlob (to, top, bottom, left, right, single_rop [rop]);
 }

void Paint (from, to, fromPoint, toPoint, extent, colour)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, extent;
	Colour colour;
 {
 	if (to == NULL)
 		return;
 	if (from && IsWindowHardware (from))
 	 {
 	 	fprintf (stderr, "driver for Paint cannot use Window as source\n");
 	 	exit (1);
 	 }
 	if (IsWindowHardware (to))
 	 {
 	 	struct hardcolour col;
 	 	
 	 	col = HardColour (colour);
 	 	if (col.tile == 0)
 	 		if (from == NULL)
 	 			XPixSet (to->win, toPoint.dx, toPoint.dy, extent.dx, extent.dy, col.pixel);
 	 		else
			 {
				Pixmap pm = XMakePixmap (NeedBitmap (from->bm), AllPlanes, 0);
				
  	 			XPixmapPut (to->win, fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy, extent.dx, extent.dy, pm, GXandInverted, AllPlanes);
  	 			XFreePixmap (pm);
 				pm = XMakePixmap (NeedBitmap (from->bm), col.pixel, 0);
				
  	 			XPixmapPut (to->win, fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy, extent.dx, extent.dy, pm, GXor, AllPlanes);
  	 			XFreePixmap (pm);
  	 		 }
	 	else
 	 		if (from == NULL)
  	 			XTileSet (to->win, toPoint.dx, toPoint.dy, extent.dx, extent.dy, col.tile);
 	 		else
			 {
				Pixmap pm = XMakePixmap (NeedBitmap (from->bm), BlackPixel, WhitePixel);
				
  	 			XPixmapPut (to->win, fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy, extent.dx, extent.dy, pm, GXandInverted, AllPlanes);
  	 			XFreePixmap (pm);
  	 		 }
	 }
 	else
 	 {
  	 	fprintf (stderr, "driver for Paint cannot use Window as source\n");
 	 	exit (1);
	 }
 }

void BitBlt (from, to, fromPoint, toPoint, extent, rop)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, extent;
	int rop;
 {
 	if (to == NULL)
 		return;
 	if (IsWindowHardware (to))
 	 {
 	 	fprintf (stderr, "driver for BitBlt cannot use Window as Destination\n");
 	 	exit (1);
 	 }
 	else
 	 {
 	 	if (rop == ROP_SOURCE && fromPoint.dx == 0 && fromPoint.dy == 0 && toPoint.dx == 0 && toPoint.dy == 0)
 	 	 {
 	 	 	DevicePoint exfrom, exto;
 	 	 	
 	 	 	exfrom = HardwareExtent (from); exto = HardwareExtent (to);
 	 	 	if (exfrom.dx == extent.dx && exfrom.dy == extent.dy && exto.dx == extent.dx && exto.dy == extent.dy)
 	 	 	 {
 	 	 	 	Bcopy ((char *) (to->bm->data), (char *) (from->bm->data), sizeof (short) * hard_size (to->width, to->height));
 	 	 	 	OODBitmap (to->bm);
 	 	 	 	
 	 	 	 	return;
 	 	 	 }
 	 	 		
 	 	 }
 	 }
 	fprintf (stderr, "driver for BitBlt can't cope with general case\n");
 	exit (1);
 }

void PaintLine (h, fromPoint, toPoint, colour) struct hardware *h; DevicePoint fromPoint, toPoint; Colour colour;
 {
	struct hardcolour col;

	col = HardColour (colour);

	if (col.tile == screen[0].shade)
		col.pixel = BlackPixel,
		col.tile = 0;
	else if (col.tile == screen[screen_size].shade)
		col.pixel == WhitePixel,
		col.tile = 0;
	else
		;
	if (!IsWindowHardware (h))
	 {
		fprintf (stderr, "driver for PaintLine cannot cope with bitmaps\n");
		return;
	 }
	if (col.tile == 0)
		XLine (h->win, fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy, 1, 1, col.pixel, GXcopy, AllPlanes);
	else
	 {
		fprintf (stderr, "driver for PaintLine cannot cope with textured lines\n");
		XLine (h->win, fromPoint.dx, fromPoint.dy, toPoint.dx, toPoint.dy, 1, 1, col.pixel, GXcopy, AllPlanes);
	 }
 }

void Line (h, fromPoint, toPoint, rop) struct hardware *h; DevicePoint fromPoint, toPoint; Colour rop;
 {
 }

struct hardware *HardwareFromString (s, width, height) unsigned char *s; int width, height;
 {
	int words = (width + 15) / 16;
	struct bitmap *BM = NewBM (width, height);
	short *d = BM->data;
	int odd = ((width + 7) / 8) & 1;
	int i, j;

	for (i = 0; i < height; i++)
	 {
		for (j = 0; j < words - odd; j++)
		 {
			short word = reverse [*s++];

			*d++ = word | (reverse [*s++] << 8);
		 }
		if (odd)
			*d++ = reverse [*s++];
	 }
	return NewHardware (0, BM, width, height);
 }

char *StringFromHardware (h) struct hardware *h;
 {
	int words = (h->width + 15) / 16;
	char *string = malloc ((h->width + 7) / 8 * h->height), *s = string;
	int i, j, odd = ((h->width + 7) / 8) & 1;
	short *d = h->bm->data;

	for (i = 0; i < h->height; i++)
	 {
		for (j = 0; j < words - odd; j++)
		 {
			short word = *d++;

			*s++ = reverse [word & 0xFF];
			*s++ = reverse [(word >> 8) & 0xFF];
		 }
		if (odd)
			*s++ = reverse [*d++ & 0xFF];
	 }
	return string;
 }

InitReversal ()
 {
	int i, j;

	for (i = 0; i < 0x100; i++)
	 {
		unsigned char r = 0;

		for (j = 0; j < 8; j++)
			r |= (1 & (i >> j)) << (7 - j);
		reverse [i] = r;
	 }
 }

void HardUpdate ()
 {
 	XFlush ();
 }

static struct hardcolour HardColour (colour) Colour colour;
 {
 	struct hardcolour res;
	int col;
 	
	if (DisplayPlanes () != 1)
	 {
		Color def;
		float r, g, b;

		ColourRGB (colour, &r, &g, &b);
		def.red = XMAX * r;
		def.green = XMAX * g;
		def.blue = XMAX * b;

		XGetHardwareColor (&def);

		res.tile = 0;
		res.pixel = def.pixel;

		return res;
	 }
	
	col = transfer [(int) (TRANSFER_SIZE * colour.brightness + .5)];
	
 	res.tile = screen[(int) (col * (float) screen_size / TRANSFER_SIZE + 0.5)].shade;
 	
 	return res;
 }

static void InitTransfer ()
 {
 	int i;
 	
 	for (i = 0; i <= TRANSFER_SIZE; i++)
 		transfer [i] = i;
 }

int TransferSize ()
 {
	return TRANSFER_SIZE;
 }

void SetTransfer (tran) float *tran;
 {
 	int i;
 	
 	for (i = 0; i < TRANSFER_SIZE; i++)
 		transfer [i] = (TRANSFER_SIZE - 1) * tran[i] + .5;
 }

int ScreenSize (freq, rot) float freq, rot;
 {
 	int size = magnification * 72 / freq + 0.5;
 	
 	return size * size;
 }

void BuildScreen (freq, rotation, x, y) float freq, rotation, *x, *y;
 {
 	int size = magnification * 72 / freq + 0.5;
 	int i, j;
 	
 	for (i = 0; i < size; i++)
 		for (j = 0; j < size; j++)
 			*x++ = (2 * i - size + 1) / (float) size,
 			*y++ = (2 * j - size + 1) / (float) size;
 }

static sgn (a) float a;
 {
	if (a == 0)
		return 0;
	else if (a < 0)
		return -1;
	else
		return 1;
 }

static int screen_cmp (a, b) char *a, *b;
 {
 	struct screen *aa = (struct screen *) a, *bb = (struct screen *) b;
 	
 	return sgn (aa->val - bb->val);
 }

void SetScreen (freq, rotation, thresh) float freq, rotation, *thresh;
 {
 	struct bitmap *temp;
 	int i, j, size = magnification * 72 / freq + 0.5;
 	struct screen *p;
 	
 	if (screen)
 	 {
 	 	for (i = 0; i < screen_size; i++)
 	 		XFreePixmap (screen [i].shade);
 	 	free ((char *) screen);
 	 }
 	p = screen = (struct screen *) malloc (((screen_size = size * size) + 1) * sizeof (struct screen));
 	screen_side = size;
 	for (i = 0; i < size; i++)
 		for (j = 0; j < size; j++)
 		 {
 		 	p->val = *thresh++;
 		 	p->sx = i;
 		 	p->sy = j;
 		 	++p;
 		 }
 	qsort ((char *) screen, (unsigned) screen_size, sizeof (struct screen), screen_cmp);
 	
 	temp = NewBM (size, size);
 	
 	for (i = 0; i < screen_size; i++)
 	 {
 	 	screen [i].shade = XMakePixmap (NeedBitmap (temp), WhitePixel, BlackPixel);
 	 	SetBit (temp, screen[i].sx, screen[i].sy);
 	 	OODBitmap (temp);
 	 }
 	screen[screen_size].shade = XMakePixmap (NeedBitmap (temp), WhitePixel, BlackPixel);
 	OODBitmap (temp);
 }

SetBit (bm, x, y) struct bitmap *bm; int x, y;
 {
 	int words = ((bm->width + 15) / 16);
 	
 	OODBitmap (bm);
 	bm->data[y * words + (x >> 4)] |= 1 << (x & 0xF);
 }

UpdateControl (h, i) struct hardware *h; int i;
 {
 }
