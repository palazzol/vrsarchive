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
#include <graphics/grafix.h>
#include <graphics/window.h>
#include <stdio.h>

#include "orion.h"

int rop_map [16];

struct hardware
 {
	WindowPtr win;
	BitMapPtr bm;
 };

static int VirtualCol [256], nVirtual, transfer [256], bi_level [256];

static void InitRopMap (), InitVirtualColours ();
static struct hardware *NewHardware ();

char *malloc (), *getenv (), *strcpy (), *strcat ();

struct hardware *InitHardware ()
 {
	InitGraf ();
	
	InitVirtualColours ();
	InitRopMap ();
	
	return NewWindowHardware (1, 1);
 }

struct hardware *NewBitmapHardware (w, h) int w, h;
 {
 	BitMapPtr res = (BitMapPtr) malloc (sizeof (BitMap));
 	
 	*res = NewRBitMap (w, h, 1);
	return NewHardware ((WindowPtr) NULL, res);
 }

struct hardware *NewWindowHardware (w, h) int w, h;
 {
	Rect r;
	WindowPtr win;
	
	SetRect (&r, 0, 25, w, h);
	win = NewWindow (NIL, r, "PostScript", VISIBLE, documentProc, FRONT, NOGOAWAY, 0, 7);
	
	return NewHardware (win, &win->port.portBits);
 }

int IsWindowHardware (h) struct hardware *h;
 {
	return h->win != (WindowPtr) NULL;
 }

void DestroyHardware (dev) struct hardware *dev;
 {
	if (dev->win != (WindowPtr) NULL)
		/* DisposeWindow (dev->win) ? */;
	else
		DisposeRBitMap (dev->bm);
	Free ((char *) dev);
 }

static void InitRopMap ()
 {
	rop_map [ROP_FALSE]	= R_ZERO;			/*  0 0000 */
	rop_map [ROP_AND] 	= R_AND;			/* 11 1011 */
	rop_map [ROP_ANDNOT]	= R_SOURCE_AND_NOT_DEST;	/*  8 1000 */
	rop_map [ROP_SOURCE] 	= R_SOURCE;			/*  5 0101 */
	rop_map [ROP_NOTAND] 	= R_NOT_SOURCE_AND_DEST;	/*  7 0111 */
	rop_map [ROP_DEST]	= R_DEST;			/*  3 0011 */
	rop_map [ROP_XOR] 	= R_XOR;			/*  9 1001 */
	rop_map [ROP_OR] 	= R_OR;				/* 15 1111 */
	rop_map [ROP_NOR]	= R_NOR;			/*  6 0110 */
	rop_map [ROP_NXOR] 	= R_XNOR;			/* 12 1100 */
	rop_map [ROP_NOTDEST]	= R_NOT_DEST;			/*  2 0010 */
	rop_map [ROP_ORNOT]	= R_SOURCE_OR_NOT_DEST;		/* 14 1110 */
	rop_map [ROP_NOTSOURCE]	= R_NOT_SOURCE;			/*  4 0100 */
	rop_map [ROP_NOTOR] 	= R_NOT_SOURCE_OR_DEST;		/* 13 1101 */
	rop_map [ROP_NAND]	= R_NAND;			/* 10 1010 */
	rop_map [ROP_TRUE]	= R_ONE;			/*  1 0001 */
 }

static ColMap default_cm = {
	0, srcCopy,
	0, srcCopy,
	0, srcCopy,
	0, srcCopy,
	0, srcCopy,
	0, srcCopy,
	0, srcCopy,
	0, srcCopy
};

SCM (col, mode, from, to)
	int col, mode;
	ColMap *from, *to;
 {
	int i, imode = InverseMode (mode);

	for (i = 0; i < 8; i++)
	 {
		to->colMapEnt[i].plane = 0;
		to->colMapEnt[i].mode = (col & (1 << i)) ? mode : imode;
	 }
 }

UpdateHardware (h, r) struct hardware *h; Rect r;
 {
	if (IsWindowHardware (h))
	 {
		AddUpdate (h->win, r, 0xff);
		UpdateWindow (h->win);
	 }
 }

PrintRect (r) Rect r;
 {
	fprintf (stderr, "[(%d, %d) (%d, %d)]\n", left (r), top (r), width (r), height (r));
 }

SR (r, l, t, w, h) Rect *r; int l, t, w, h;
 {
	left (*r) = l;
	top (*r) = t;
	width (*r) = w;
	height (*r) = h;
 }

void BitBlt (from, to, fromPoint, toPoint, ex, rop, colour)
	struct hardware *from, *to;
	DevicePoint fromPoint, toPoint, ex;
	int rop;
	Colour colour;
 {
 	Rect sr, dr;
 	int col = HardColour (colour, (IsWindowHardware (to) ? transfer : bi_level));
 	ColMap cm;
 	
 	if (to->bm == NULL)
 		return;
 	SR (&sr, fromPoint.dx, fromPoint.dy, ex.dx, ex.dy);
 	SR (&dr, toPoint.dx, toPoint.dy, ex.dx, ex.dy);
 	SCM (col, rop_map [rop], &default_cm, &cm);
 	MapPlanes (from->bm, to->bm, &sr, &dr, &cm.colMapEnt[0]);
	UpdateHardware (to, dr);
 }

MapPlanes (sb, db, sr, dr, cm)
	BitMapPtr sb, db;
	Rect *sr, *dr;
	CMEPtr cm;
 {
	int i;
	BitMap sbm, dbm;
	sbm = *sb; dbm = *db;
	sbm.nPlanes = 1; dbm.nPlanes = 1;
	
	for (i = 0; i < db->nPlanes; i++)
	 {
	 	sbm.baseAddr = sbm.baseAddr + sbm.planeSize * (7 - cm[i].plane);
	 	CopyBits (&sbm, &dbm, *sr, *dr, cm[7 - i].mode);
	 	dbm.baseAddr += dbm.planeSize;
	 }
 }

/*ARGSUSED*/
Matrix DeviceMatrix (w, h) int w, h;
 {
	return NewMatrix (PIXELS_PER_INCH / 72.0, 0.0, 0.0, -PIXELS_PER_INCH / 72.0, 0.0, (float) h);
 }

DevicePoint HardwareExtent (h) struct hardware *h;
 {
	return NewDevicePoint (width (h->bm->bounds), height (h->bm->bounds));
 }

static struct hardware *NewHardware (win, bm) WindowPtr win; BitMapPtr bm;
 {
 	struct hardware *res = (struct hardware *) malloc (sizeof (struct hardware));
 	
 	res->win = win;
 	res->bm = bm;
 	
 	return res;
 }

static int HardColour (colour, transfer) Colour colour; int *transfer;
 {
	return VirtualCol [transfer [(int) ((nVirtual - 1) * colour.brightness + 0.5)]];
 }

int TransferSize ()
 {
 	return nVirtual;
 }

void SetTransfer (tran) float *tran;
 {
	int i;

	for (i = 0; i < nVirtual; i++)
		transfer [i] = (nVirtual - 1) * tran[i] + .5;
 }

static void InitVirtualColours ()
 {
	int i = 0, from, to, j;
	char name [BUFSIZ];
	FILE *fp;
	
	(void) strcpy (name, getenv ("HOME"));
	(void) strcat (name, "/.pscolourmap");
	
	PanicIf (((fp = fopen (name, "r")) == NULL && (fp = fopen ("/etc/.pscolourmap", "r")) == NULL),
		".pscolourmap not found");
	
	while (fscanf (fp, "%d %d\n", &from, &to) == 2)
		for (j = from; j <= to; j++)
			VirtualCol [i++] = j;
	fclose (fp);
	nVirtual = i;
	
	for (i = 0; i < nVirtual; i++)
	 {
	 	int b = 250 * (float) i / nVirtual + 0.5;
	 	
	 	SetCLTE (VirtualCol [i], b, b, b);
	 }
	
	for (i = 0; i < nVirtual; i++)
	 {
		transfer [i] = i;
		bi_level [i] = (i < nVirtual / 2) ? 0 : nVirtual - 1;
	 }
 }

struct hardware *HardwareFromString (s, w, h) char *s; int w, h;
 {
 	char *to;
 	int i, sw = (w + 7) / 8, dw;
 	
 	BitMapPtr bmp = (BitMapPtr) malloc (sizeof (BitMap));
 	
 	*bmp = NewRBitMap (w, h, 1);
 	to = bmp->baseAddr;
 	dw = bmp->rowBits / 8;
 	
 	for (i = 0; i < h; i++)
 	 {
 	 	(void) Bcopy (to, s, sw);
 	 	to += dw;
 	 	s += sw;
 	 }
 	
 	return NewHardware ((WindowPtr) NULL, bmp);
 }

char *StringFromHardware (d) struct hardware *d;
 {
	unsigned sw, sh, dw = d->bm->rowBits;
	char *res, *p, *q = d->bm->baseAddr;
	DevicePoint ex;
	int i;
	
	ex = HardwareExtent (d);
	sw = (ex.dx + 7) / 8;
	sh = ex.dy;
	p = res = malloc (sw * sh);
	
	for (i = 0; i < sh; i++)
	 {
	 	(void) Bcopy (p, q, sw);
	 	p += sw; q += dw;
	 }
	
	return res;
 }

void Line (h, fromPoint, toPoint, rop, colour)
	struct hardware *h;
	DevicePoint fromPoint, toPoint;
	int rop;
	Colour colour;
 {
	
 }

SetCLTE (col, r, g, b) int col, r, g, b;
 {
/*	fprintf (stderr, "%d = (%d, %d, %d)\n", col, r, g, b);	*/
 }

HardUpdate ()
 {
 }
