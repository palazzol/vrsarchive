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

static struct cache
 {
	struct cache *cache_next, *cache_prev;
	int fid;
	Matrix mat;					/* character -> device */
	int width, height, swidth, sheight; 	/* device coords */
 } cache_header = { &cache_header, &cache_header },
 	*cache_ring = &cache_header;

static struct char_table {
	Object 		char_key;
	struct cache 	*char_cache;
	
	struct device 	*char_device;
	Point 		char_width;		    /* character coords */
} *CharTable;

static int bsize = 0, bmax = 0;
static int msize = 0, mmax = 0;
static int csize = 0, cmax = 0;

static int maxbits = 0;

static int CacheStatus ();
static int PSetCharWidth ();
static int SetCacheLimit ();
static int PCurrentCharWidth ();
static int PSetCacheDevice ();
static int SetCache ();
static int SaveCurrentFont ();

int SetCacheDevice ();
static struct cache *NewCache (), *CacheRemove ();

InitCache ()
 {
 	InstallOp ("cachestatus",	CacheStatus,		0, 7, 0, 0);
 	InstallOp ("setcachedevice",	PSetCacheDevice,	6, 0, 0, 0, Float, Float, Float, Float, Float, Float);
 	InstallOp ("setcharwidth",	PSetCharWidth,		2, 0, 0, 0, Float, Float);
 	InstallOp (".currentcharwidth",	PCurrentCharWidth,	0, 2, 0, 0);
 	InstallOp ("setcachelimit",	SetCacheLimit,		1, 0, 0, 0, Integer);
 	InstallOp ("setcache",		SetCache,		3, 0, 0, 0, Integer, Integer, Integer);
	InstallOp ("savecurrentfont",	SaveCurrentFont,		0, 0, 0, 0);
 }

static InitCharTable ()
 {
 	int i;
 	
 	CharTable = (struct char_table *) Malloc ((unsigned) (sizeof (struct char_table) * cmax));
 	for (i = 0; i < cmax; i++)
 		CharTable[i].char_cache = NULL;
 }

static struct char_table *CharAddition (table, cache, key, size)
		struct char_table *table;
		struct cache *cache;
		Object key;
		int size;
 {
 	int i, h = (BodyInteger (key) + (int) cache) % size;
 	
	for (i = h;;)
	 {
	 	if (table[i].char_cache == NULL)
		 {
			table[i].char_key = key;
			table[i].char_cache = cache;
			
			return &table[i];
		 }
		if (++i == size)
			i = 0;
		if (i == h)
			return NULL;
	 }
 }

CharStore (key, width, dev) Object key; Point width; struct device *dev;
 {
 	struct char_table *p;
 	
	while (csize == cmax)
		DestroyCache (CacheRemove (cache_ring->cache_prev));
 	
	if (p = CharAddition (CharTable, gstate->show->ccache, key, cmax))
	 {
		p->char_width = width;
		p->char_device = dev;
		++csize;
	 }
	else
		Panic ("dictionary full in CharStore");
 }

static struct char_table *CharFind (table, cache, key, size)
		struct char_table *table;
		struct cache *cache;
		Object key;
		int size;
 {
 	int i, h = (BodyInteger (key) + (int) cache) % size;
 	
	for (i = h;;)
	 {
/*	 	if (table[i].char_cache == NULL)
	 		return NULL;
*/	 	if (table[i].char_cache == cache && Equal (key, table[i].char_key))
			return &table[i];
		if (++i == size)
			i = 0;
		if (i == h)
			break;
	 }
	return NULL;
 }

static struct char_table *CharLoad (key) Object key;
 {
 	return CharFind (CharTable, gstate->show->ccache, key, cmax);
 }

static FlushChars (p) struct cache *p;
 {
 	int i;
 	
	for (i = 0; i < cmax; i++)
	 	if (CharTable[i].char_cache == p)
	 	 {
	 		CharTable[i].char_cache = NULL;
	 		--csize;
	 	 }
 }

/* convert bounding box to equivalent one in device coordinates */
DeviceBBox (left, right, top, bottom, llx, urx, ury, lly) float *left, *right, *top, *bottom, llx, urx, ury, lly;
 {
 	HardPoint ll;
 	
 	ll = ExtToInt (NewPoint (llx, lly));
 	
 	*left = *right = ll.hx; *top = *bottom = ll.hy;
 	
 	Bound (left, right, top, bottom, ExtToInt (NewPoint (urx, lly)));
 	Bound (left, right, top, bottom, ExtToInt (NewPoint (llx, ury)));
 	Bound (left, right, top, bottom, ExtToInt (NewPoint (urx, ury)));
 }

int EqVector (a, b) Vector a, b;
 {
 	return (int) a.vx == (int) b.vx && (int) a.vy == (int) b.vy;
 }

static int PCurrentCharWidth ()
 {
 	VOID Push (OpStack, MakeReal (gstate->show->Width.x));
 	VOID Push (OpStack, MakeReal (gstate->show->Width.y));
 	return TRUE;
 }

static int PSetCharWidth (wx, wy) Object wx, wy;
 {
 	if (!gstate->show->InShow)
 		return Error (PUndefined);
 	SetCharWidth (NewPoint (BodyReal (wx), BodyReal (wy)));
 	return TRUE;
 }

SetCharWidth (width) Point width;
 {
 	gstate->show->Width = width;
 }

static int PSetCacheDevice (wx, wy, llx, lly, urx, ury)  Object wx, wy, llx, lly, urx, ury;
 {
 	if (!gstate->show->InShow)
 		return Error (PUndefined);
 	return SetCacheDevice (gstate->show->CharName,
 				NewPoint (BodyReal (wx), BodyReal (wy)),
 				BodyReal (llx), BodyReal (lly),
 				BodyReal (urx), BodyReal (ury));
 }

static int CharTooBig (width, height) float width, height;
 {
	return width * height > maxbits * 8;
 }

int SetCacheDevice (name, char_width, llx, lly, urx, ury)
		Object name; Point char_width; float llx, lly, urx, ury;
 {
 	float left, right, top, bottom, width, height;
 	struct device *new_char;
 	struct cache *ccache = gstate->show->ccache;
 	
 	SetCharWidth (char_width);
 	if (!ccache)
 		return TRUE;
 	DeviceBBox (&left, &right, &top, &bottom, llx, urx, ury, lly);
 	width = right - left; height = top - bottom;
 	
 	if (CharTooBig (width, height))
 		return TRUE;
 	
 	new_char = NewCacheDevice (ccache->mat, ccache->width, ccache->height, ccache->swidth, ccache->sheight);
 	CharStore (name, char_width, LinkDevice (new_char));
 	SetDevice (new_char);
 	
 	PathFree (gstate->clip);
 	gstate->clip = NewClipPath (0.0, width, height, 0.0);
 	gstate->CTM.tx = ccache->swidth;
 	gstate->CTM.ty = ccache->sheight;
 	VOID MoveTo (gstate->path, ExtToInt (NewPoint (0.0, 0.0)));
 	
 	return TRUE;
 }

printMatrix (m) Matrix m;
 {
	printf ("[%g %g]\n", m.A, m.B);
	printf ("[%g %g]\n", m.C, m.D);
	printf ("[%g %g]\n", m.tx, m.ty);
 }

#ifdef notdef
int EqFont (m, n, bbox) Matrix m, n; float *bbox;
 {
 	Vector morigin, norigin, mright, nright, mbottom, nbottom;
 	
 	morigin = Transform (NewVector (bbox [BBOX_LEFT], bbox [BBOX_TOP], 1.0), m);
 	norigin = Transform (NewVector (bbox [BBOX_LEFT], bbox [BBOX_TOP], 1.0), n);
 	mright  = DiffVector (Transform (NewVector (bbox [BBOX_RIGHT], bbox [BBOX_TOP], 1.0), m), morigin);
 	nright  = DiffVector (Transform (NewVector (bbox [BBOX_RIGHT], bbox [BBOX_TOP], 1.0), n), norigin);
 	mbottom = DiffVector (Transform (NewVector (bbox [BBOX_LEFT],  bbox [BBOX_BOTTOM], 1.0), m), morigin);
 	nbottom = DiffVector (Transform (NewVector (bbox [BBOX_LEFT],  bbox [BBOX_BOTTOM], 1.0), n), norigin);
 	
 	return EqVector (mbottom, nbottom) && EqVector (mright, nright);
 }
#endif

int EqFont (m, n) Matrix m, n;
 {
	return m.A == n.A && m.B == n.B && m.C == n.C && m.D == n.D;
 }

extern float fontbboxCache[];
extern int fidCache;

struct cache *SearchCache (m, fid) Matrix m; int fid;
 {
 	struct cache *p;
 	
 	for (p = cache_ring->cache_next; p != cache_ring; p = p->cache_next)
 		if (p->fid == fid && EqFont (m, p->mat))
 		 {
 		 	if (cache_ring->cache_next != p)
 				CacheInsert (cache_ring->cache_next, CacheRemove (p));
 			return cache_ring->cache_next;
 		 }
 	
 	return NULL;
 }

int FindCache ()
 {
 	HardPoint origin;
 	float left, right, top, bottom;
 	int width, height, swidth, sheight;
 	
 	if ((gstate->show->ccache = SearchCache (gstate->CTM, fidCache)) != NULL)
 		return TRUE;
 	Message ("building a new cache");
	DeviceBBox (&left, &right, &top, &bottom,
			fontbboxCache[BBOX_LEFT], fontbboxCache[BBOX_RIGHT], fontbboxCache[BBOX_BOTTOM], fontbboxCache[BBOX_TOP]);
 	
 	width = right - left; height = top - bottom;
 	if (CharTooBig ((float) width, (float) height))
 	 {
 	 	Message ("character too big to cache");
		return TRUE;
	 }
 	
 	if (msize == mmax)
 		DestroyCache (CacheRemove (cache_ring->cache_prev));
	
	origin = ExtToInt (NewPoint (0.0, 0.0));
	swidth = origin.hx - left; sheight = origin.hy - bottom;
 	
 	CacheInsert (cache_ring->cache_next,
 			NewCache (fidCache,
 				gstate->CTM,
 				width, height,
 				swidth, sheight));
 	gstate->show->ccache = cache_ring->cache_next;
 	
 	LoadDiskCache (gstate->CTM, fontbboxCache);
 	
 	return TRUE;
 }

int CacheShow (name, cp) Object name; HardPoint cp;
 {
 	struct char_table *b;
 	struct cache *ccache = gstate->show->ccache;
 	DevicePoint from, to, extent;
 	
 	if (!ccache)
 		return FALSE;
 	gstate->show->CharName = name;
 	if ((b = CharLoad (name)) == NULL)
 		return FALSE;
 	/* SetCharWidth (b->char_width); */
 	gstate->show->Width = b->char_width;
 	
 	from.dx = from.dy = 0;
 	to.dx = cp.hx - ccache->swidth;
 	to.dy = cp.hy - ccache->sheight;
	extent.dx = ccache->width; extent.dy = ccache->height;
	
	Paint (b->char_device->dev, gstate->device->dev, from, to, extent, gstate->colour);
 	
	return TRUE;
 }

static struct cache *NewCache (fid, m, width, height, swidth, sheight)
	int fid; Matrix m; int width, height, swidth, sheight;
 {
 	struct cache *res = (struct cache *) Malloc (sizeof (struct cache));
 	
 	m.tx = m.ty = 0.0;
 	res->fid	= fid;
 	res->mat	= m;
 	res->width	= width;
 	res->height	= height;
 	res->swidth	= swidth;
 	res->sheight	= sheight;
	
 	return res;
 }

static DestroyCache (p) struct cache *p;
 {
	FlushChars (p);
 	Free ((char *) p);
 }

static CacheInsert (where, cache) struct cache *where, *cache;
 {
 	cache->cache_next = where; cache->cache_prev = where->cache_prev;
 	where->cache_prev->cache_next = cache;
 	where->cache_prev = cache;
 	++msize;
 }

static struct cache *CacheRemove (item) struct cache *item;
 {
 	item->cache_prev->cache_next = item->cache_next;
 	item->cache_next->cache_prev = item->cache_prev;
 	--msize;
 	
 	return item;
 }

static int SetCacheLimit (mb) Object mb;
 {
 	int b = BodyInteger (mb);
 	if (b < 0)
 		return Error (PRangeCheck);
 	maxbits = b;
 	return TRUE;
 }

static int SetCache (bitmap, fonts, chars) Object bitmap, fonts, chars;
 {
 	int b = BodyInteger (bitmap);
 	int f = BodyInteger (fonts);
 	int c = BodyInteger (chars);
 	
 	if (b < 0 || f < 0 || c < 0)
 		return Error (PRangeCheck);
 	bmax = b; mmax = f; cmax = c;
 	InitCharTable ();
 	return TRUE;
 }

static int CacheStatus ()
 {
 	VOID Push (OpStack, MakeInteger (bsize));
  	VOID Push (OpStack, MakeInteger (bmax));
 	VOID Push (OpStack, MakeInteger (msize));
 	VOID Push (OpStack, MakeInteger (mmax));
 	VOID Push (OpStack, MakeInteger (csize));
 	VOID Push (OpStack, MakeInteger (cmax));
 	VOID Push (OpStack, MakeInteger (maxbits));
 	
 	return TRUE;
}

LoadDiskCache (m, bbox) Matrix m; float *bbox;
 {
 	Object font_name;
 	FILE *fp;
 	Vector origin, right, bottom;
 	char disk_name [BUFSIZE], *bits, mess[BUFSIZ];
/* 	struct hardware *screen = gstate->device->dev; */
 	
 	Message ("Attempting to find a cache on disk");
 	font_name = DictLoad (gstate->font, FontName);
 	if (TypeOf (font_name) != Name)
 		return;
 	origin = Transform (NewVector (bbox [BBOX_LEFT], bbox [BBOX_TOP], 1.0), m);
 	right  = DiffVector (Transform (NewVector (bbox [BBOX_RIGHT], bbox [BBOX_TOP], 1.0), m),    origin);
 	bottom = DiffVector (Transform (NewVector (bbox [BBOX_LEFT],  bbox [BBOX_BOTTOM], 1.0), m), origin);
 	VOID sprintf (disk_name,
 		"%s/cache/%.*s/%3d%3d%3d%3d",
 		library,
 		lengthName (font_name), BodyName (font_name),
 		(int) right.vx, (int) right.vy, (int) bottom.vx, (int) bottom.vy);
 	Message (disk_name);
 	if ((fp = fopen (disk_name, "r")) == NULL)
 		return;
 	Message ("found a disk cache");
 	VOID GSave ();
 	bits = Malloc ((unsigned) maxbits);
 	
  	sprintf (mess, "width = %d, height = %d\n", gstate->show->ccache->width, gstate->show->ccache->height);
 	Message (mess);
	sprintf (mess, "computed length = %d", gstate->show->ccache->height * ((gstate->show->ccache->width + 7) / 8));
 	Message (mess);
 	
 	while (!feof (fp))
 	 {
 	 	char char_name [BUFSIZE];
 	 	struct hardware *newdev;
 	 	int width, height, length;
 	 	
 	 	if (fscanf (fp, "%s %d %d %d\n",
 	 			char_name,
 	 			&width, &height,
 	 			&length) == EOF)
 	 		break;
	  	VOID fread (bits, (unsigned) length, 1, fp);
 	 	VOID getc (fp);
 	 	newdev = HardwareFromString (bits, gstate->show->ccache->width, gstate->show->ccache->height);
 /*	 	VOID BitBlt (newdev, screen,
 	 			NewDevicePoint (0, 0), NewDevicePoint (200, 200),
 	 			NewDevicePoint (gstate->show->ccache->width, gstate->show->ccache->height),
 	 			ROP_SOURCE);
*/	 	
 	 	VOID SetCacheDevice (NameFrom (char_name),
 	 			NewPoint ((float) width, (float) height),
 	 			bbox [BBOX_LEFT],  bbox [BBOX_TOP],
 	 			bbox [BBOX_RIGHT], bbox [BBOX_BOTTOM]);
	 	
	 	VOID BitBlt (newdev, gstate->device->dev,
 	 			NewDevicePoint (0, 0), NewDevicePoint (0, 0),
 	 			NewDevicePoint (gstate->show->ccache->width, gstate->show->ccache->height),
 	 			ROP_SOURCE);
 	 	DestroyHardware (newdev);
 	 }
 	Free (bits);
 	VOID GRestore ();
 	VOID fclose (fp);
 	Message ("load completed");
 }

static int SaveCurrentFont ()
 {
 	Matrix fm, m;
 	Vector origin, right, bottom;
 	Object font_name;
 	FILE *fp;
 	char disk_name [BUFSIZE];
 	int i;
 	float bbox[4];
 	struct cache *ccache;
 	
 	ExtractMatrix (&fm, DictLoad (gstate->font, FontMatrix));
 	m = MatMult (fm, gstate->CTM);
 	ExtractBBox (bbox, DictLoad (gstate->font, FontBBox));
 	font_name = DictLoad (gstate->font, FontName);
 	
 	origin = Transform (NewVector (bbox [BBOX_LEFT], bbox [BBOX_TOP], 1.0), m);
 	right  = DiffVector (Transform (NewVector (bbox [BBOX_RIGHT], bbox [BBOX_TOP], 1.0), m),    origin);
 	bottom = DiffVector (Transform (NewVector (bbox [BBOX_LEFT],  bbox [BBOX_BOTTOM], 1.0), m), origin);
  	
	if ((ccache = SearchCache (m, BodyFontID (DictLoad (gstate->font, Fid)))) == NULL)
 		return Error (PInvFont);
 	
  	Message (disk_name);
	VOID sprintf (disk_name,
 		"%s/cache/%.*s/%3d%3d%3d%3d",
 		library,
 		lengthName (font_name), BodyName (font_name),
 		(int) right.vx, (int) right.vy, (int) bottom.vx, (int) bottom.vy);
 	
 	if ((fp = fopen (disk_name, "w")) == NULL)
 		return Error (PInvFileAccess);
 	
 	for (i = 0; i < cmax; i++)
 	 if (CharTable[i].char_cache == ccache)
 	  {
 	  	Object char_name;
 	  	char *bits;
 	  	DevicePoint extent;
 	  	int length;
 	  	
 	  	char_name = CharTable[i].char_key;
 	  	extent = HardwareExtent (CharTable[i].char_device->dev);
 	  	length = (extent.dx + 7) / 8 * extent.dy;
 	 	bits = StringFromHardware (CharTable[i].char_device->dev);
	 	VOID fprintf (fp,
 	 		"%.*s %d %d %d\n",
 	 		lengthName (char_name), BodyName (char_name),
 	 		(int) CharTable [i].char_width.x,
 	 		(int) CharTable [i].char_width.y,
 	 		length);
 	 	VOID fwrite (bits, (unsigned) length, 1, fp);
 	 	putc ('\n', fp);
/*  	 	Free (bits);*/
	 		
 	  }
 	
 	VOID fclose (fp);
 	
 	return TRUE;
 }
