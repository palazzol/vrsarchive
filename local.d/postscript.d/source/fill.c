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

#define MAXPATHELEMENTS	15000

/*
 * Trapezoid decomposition algorithm:
 * 
 *	Edges are characterised by a line description, with topY, bottomY, topX, bottomX, startingY
 * 	Build edge lists from path.
 *	Sort edges on top Y value.
 *	find topmost interestingY value, which will be the minimum topY. This becomes interestingY.
 *	loop
 *		examine edge list from here to end. Move edges with topY value less than interestingY into
 *			interesting edge set, computing intersections for each as you go. This should establish next
 *			interestingY value, nextY.
 *		form the pairings for all edges in the interesting set, according to the fill rule at interestingY.
 *			to do this, while inside, keep a pointer to the last up-transition edge, and pair it with the
 *			next down-transition edge.
 *			for any edges which have changed pairing, emit a trapezoid from their startingY to interestingY,
 *			(if they were paired) and set their startingY to interestingY
 *		break if (ninteresting == 0 && nedges_left == 0)
 *		interestingY := nextY
 *	pool
 */		

static struct edge
 {
	int topX, topY, bottomX, bottomY; short dir;
	struct edge *pair;
	int startingY, where, up, clip;
	int name;
 };

int names = 0;

extern int in_stroke;

int InitClip ();
static int Clip ();
static int EOClip ();
int Fill ();
static int EOFill ();
static int PClipPath ();

InitFill ()
 {
 	InstallOp ("initclip",		InitClip,	0, 0, 0, 0);
 	InstallOp ("clip",		Clip,		0, 0, 0, 0);
 	InstallOp ("eoclip",		EOClip,		0, 0, 0, 0);
 	InstallOp ("fill",		Fill,		0, 0, 0, 0);
 	InstallOp ("eofill",		EOFill,		0, 0, 0, 0);
 	InstallOp ("clippath",		PClipPath,	0, 0, 0, 0);
 }

static struct hardware *output_device;
static Colour output_colour;

static int FillIt ();
static int EoRule (), NwRule ();

static void EmitTrapezoid (left, right, top, bottom) struct edge *left, *right; int top, bottom;
 {
 	struct edge *temp;
 	
 	if (left->topX > right->topX || left->bottomX > right->bottomX)
 		temp = left, left = right, right = temp;
 	PaintTrapezoid (output_device,
 		NewDevicePoint (left->topX, left->topY),
 		NewDevicePoint (left->bottomX, left->bottomY),
 		NewDevicePoint (right->topX, right->topY),
 		NewDevicePoint (right->bottomX, right->bottomY),
 		top, bottom,
 		output_colour);
 }

int Fill ()
 {
	int res;
	
	if (gstate->device->dev == NULL)
		return PNewPath ();
	
	output_device = gstate->device->dev;
	output_colour = gstate->colour;
 	res = FillIt (gstate->path, NwRule, gstate->clip, NwRule, EmitTrapezoid);
 						/* XXX needs optimising */
 	
	VOID PNewPath ();
	
	return res;
 }

static int EOFill ()
 {
	int res;
	
	if (gstate->device->dev == NULL)
		return PNewPath ();
	
 	output_device = gstate->device->dev;
	output_colour = gstate->colour;
	res = FillIt (gstate->path, EoRule, gstate->clip, NwRule, EmitTrapezoid);
 						/* XXX needs optimising */
 	
	VOID PNewPath ();
	
	return res;
 }

static Path output_path;

static void PathTrapezoid (left, right, top, bottom) struct edge *left, *right; int top, bottom;
 {
 	int ll, ul, lr, ur;
 	struct edge *temp;
 	
 	if (left->topX > right->topX || left->bottomX > right->bottomX)
 		temp = left, left = right, right = temp;
 	
 	ll = Xvalue (left->topX, left->topY, left->bottomX, left->bottomY, bottom);
 	ul = Xvalue (left->topX, left->topY, left->bottomX, left->bottomY, top);
 	lr = Xvalue (right->topX, right->topY, right->bottomX, right->bottomY, bottom);
 	ur = Xvalue (right->topX, right->topY, right->bottomX, right->bottomY, top);
 	
 	MoveTo (output_path, NewHardPoint ((float) ll, (float) bottom));
 	LineTo (output_path, NewHardPoint ((float) lr, (float) bottom));
 	LineTo (output_path, NewHardPoint ((float) ur, (float) top));
 	LineTo (output_path, NewHardPoint ((float) ul, (float) top));
 	ClosePath (output_path);
 }

int InitClip ()
 {
 	PathFree (gstate->clip);
 	gstate->clip = PathCopy (gstate->device->default_clip);
 	
 	UnlinkDevice (gstate->clipdevice);
  	gstate->clipdevice = NULL;
  	
 	SetClipHardware (gstate->device->dev, NULL);
	
	return TRUE;
 }

static int Clip ()
 {
	int res;
	
	
	output_path = NewPath ();
 	res = FillIt (gstate->path, NwRule, gstate->clip, NwRule, PathTrapezoid);
 						/* XXX - needs optimising */
 	
 	PathFree (gstate->clip);
 	gstate->clip = output_path;
 	
 	if (res)
 	 {
  		gstate->clipdevice = UniqueDevice (gstate->clipdevice);
  		SetClipHardware (gstate->device->dev, gstate->clipdevice->dev);
  		
		output_device = gstate->clipdevice->dev;
 		output_colour = NewColour (0.0, 0.0, 0.0);
 		res = FillIt (gstate->clip, NwRule, gstate->device->default_clip, NwRule, EmitTrapezoid);
 	 }
	return res;
 }

static int EOClip ()
 {
	int res;
	
	output_path = NewPath ();
	res = FillIt (gstate->path, EoRule, gstate->clip, NwRule, PathTrapezoid);
 						/* XXX - needs optimising */
 	
 	PathFree (gstate->clip);
 	gstate->clip = output_path;
 	
 	if (res)
 	 {
 		gstate->clipdevice = UniqueDevice (gstate->clipdevice);
    		SetClipHardware (gstate->device->dev, gstate->clipdevice->dev);
   		
		output_device = gstate->clipdevice->dev;
 		output_colour = NewColour (0.0, 0.0, 0.0);
 		res = FillIt (gstate->clip, NwRule, gstate->device->default_clip, NwRule, EmitTrapezoid);
 	 }
	return res;
 }

static int PClipPath ()
 {
 	PathFree (gstate->path);
 	gstate->path = PathCopy (gstate->clip);
 	gstate->cp_defined = TRUE;
 	gstate->cp = gstate->path->last->pe.point;
 	
 	return TRUE;
 }

/****************************/

static struct edge edge [MAXPATHELEMENTS];

static struct edge *interesting [MAXPATHELEMENTS];

static int nedges, here, ninteresting;

static int interestingY, nextY;

static int infinity;

static int FillIt (path_a, rule_a, path_b, rule_b, emitfn)
	Path path_a, path_b; int (*rule_a)(), (*rule_b)(); void (*emitfn)();
 {
	Path new;
	static int edgecmp ();
	static void Trapezoids (), BuildEdgeList ();
	
	if (EmptyPath (path_a) || EmptyPath (path_b))
		return TRUE;
	
	path_a = FlattenPath (path_a);
	path_b = FlattenPath (path_b);
	
	if (!CloseAll (path_a) || !CloseAll (path_b))
	 {
		PathFree (path_a);
		PathFree (path_b);
		
		return FALSE;
	 }
	
	nedges = 0;
	BuildEdgeList (path_a, TRUE);
	BuildEdgeList (path_b, FALSE);
 	qsort ((char *) edge, (unsigned) nedges, sizeof (struct edge), edgecmp);
	PathFree (path_a);
	PathFree (path_b);
	Trapezoids (rule_a, rule_b, emitfn);
	
	return TRUE;
 }

static void AddLowest ();

static void Trapezoids (rule_a, rule_b, emitfn) int (*rule_a)(), (*rule_b)(); void (*emitfn)();
 {
	static void FindInfinity (), AddInteresting ();
	
	nextY = edge[0].topY;
	AddLowest (nextY);
	FindInfinity ();
	
	here = 0;
	ninteresting = 0;
	names = 0;
	
	while (here != nedges || ninteresting)
	 {
	 	static void RemoveEdges ();
	 	
	 	interestingY = nextY;
	 	ProcessEdges (rule_a, rule_b, emitfn);
	 	
	 	AddInteresting ();
	 	
	 	RemoveEdges (interestingY, emitfn);
	 	
	 	ProcessEdges (rule_a, rule_b, emitfn);
	 }
 }

ProcessEdges (rule_a, rule_b, emitfn) int (*rule_a)(), (*rule_b)(); void (*emitfn)();
 {
	struct edge *up_edge;
	int i, count_a = 0, count_b = 0;
	static void RemoveEdges ();
	
	for (i = 0; i < ninteresting; i++)
	 {
	 	static void Emit ();
	 	int d_a = 0, d_b = 0;
	 	
	 	if (interesting[i]->clip)
	 		d_a = interesting[i]->dir;
	 	else
	 		d_b = interesting[i]->dir;

	 	if (UpEdge (count_a, count_b, d_a, d_b, rule_a, rule_b))
	 		up_edge = interesting[i];
	 	else if (DownEdge (count_a, count_b, d_a, d_b, rule_a, rule_b))
	 	 	ThisBit (up_edge, interesting[i], interestingY, emitfn);
	 	else
	 	 	NotThisBit (interesting[i], interestingY, emitfn);
	 	
	 	count_a += d_a;
	 	count_b += d_b;
	 }
	if (count_a || count_b)
		fprintf (stderr, "count_a = %dcount_b = %d\n", count_a, count_b);
	PanicIf (count_a || count_b, "something wrong in area fill");
 }

ThisBit (left, right, where, emitfn) struct edge *left, *right; int where; void (*emitfn)();	
 {
 	if (left->pair != right || right->up)
 	 {
 	 	if (left->pair != NULL)
 		 {
 			(*emitfn) (left, left->pair, left->startingY, left->where);
 			left->pair->startingY = left->pair->where;
  			left->pair->pair->startingY = left->pair->pair->where;
  			left->pair->pair = NULL;
		 }
 		if (right->pair != NULL)
 		 {
 			(*emitfn) (right, right->pair, right->startingY, right->where);
 			right->pair->startingY = right->pair->where;
  			right->pair->pair->startingY = right->pair->pair->where;
  			right->pair->pair = NULL;
		 }
 		
  		left->pair = right;
 		right->pair = left;
 		left->startingY = right->startingY = where;
	 }
 	left->where = right->where = where;
 	left->up = TRUE; right->up = FALSE;
 }

NotThisBit (edge, where, emitfn) struct edge *edge; int where; void (*emitfn)();	
 {
 	if (edge->pair != NULL)
 	 {
 		(*emitfn) (edge, edge->pair, edge->startingY, where);
 		edge->pair->startingY = where;
 		edge->pair->where = where;
 		edge->pair->pair = NULL;
 		edge->pair = NULL;
	 }
  	edge->startingY = where;
 	edge->where = where;
 }

static void RemoveEdges (interestingY, emitfn) int interestingY; void (*emitfn)();
 {
 	int i, j = 0;
 	
 	for (i = 0; i < ninteresting; i++)
		if (interesting [i]->bottomY > interestingY)
 			interesting [j++] = interesting [i];
 		else
 			NotThisBit (interesting[i], interestingY, emitfn);
 	ninteresting = j;
 }

static int UpEdge (count_a, count_b, inc_a, inc_b, rule_a, rule_b) int count_a, count_b, inc_a, inc_b, (*rule_a) (), (*rule_b) ();
 {
 	return (*rule_b)(count_b + inc_b) && !(*rule_a) (count_a) && (*rule_a) (count_a + inc_a) ||
 		(*rule_a)(count_a + inc_a) && !(*rule_b) (count_b) && (*rule_b) (count_b + inc_b);
 }

static int DownEdge (count_a, count_b, inc_a, inc_b, rule_a, rule_b) int count_a, count_b, inc_a, inc_b, (*rule_a) (), (*rule_b) ();
 {
 	return (*rule_b)(count_b + inc_b) && (*rule_a) (count_a) && !(*rule_a) (count_a + inc_a) ||
 		(*rule_a)(count_a + inc_a) && (*rule_b) (count_b) && !(*rule_b) (count_b + inc_b);
 }

static int EoRule (n) int n;
 {
	return n & 1;
 }

static int NwRule (n) int n;
 {
	return n;
 }

static float Yintersect (a, b) struct edge *a, *b;
 {
 	float
 		num =   (a->bottomX * a->topY - a->bottomY * a->topX) * (b->topY - b->bottomY) -
 			(b->bottomX * b->topY - b->bottomY * b->topX) * (a->topY - a->bottomY),
 		denom = (a->bottomX - a->topX) * (b->topY - b->bottomY) -
 			(b->bottomX - b->topX) * (a->topY - a->bottomY);
 	
 	if (denom == 0)
 	 {
 		return infinity;
 	 }
	return num / denom;
 }

static int Xvalue (ax, ay, bx, by, cy) int ax, ay, bx, by, cy;
 {
 	return bx + (cy - by) * (ax - bx) / (float) (ay - by);
 }

static int intercmp (aa, bb) char *aa, *bb;
 {
 	struct edge *a = *(struct edge **) aa, *b = *(struct edge **) bb;
 	int sign;
	
	sign = Xvalue (a->topX, a->topY, a->bottomX, a->bottomY, interestingY + 1) -
		     Xvalue (b->topX, b->topY, b->bottomX, b->bottomY, interestingY + 1);
	if (sign == 0)
		return a->bottomX - b->bottomX;
	return sign;
 }

static void AddInteresting ()
 {
	int i;
	 	
	nextY = infinity;
	for (; here < nedges && edge[here].topY <= interestingY; here++) /* look at each new interesting edge */
	 {
	 	int i, n;
	 	
		for (i = 0; i < ninteresting; i++) /* look at all possible intersections */
		 {
		 	int inter = Yintersect (&edge[here], interesting[i]);
		 	
		 	if (inter >= interestingY && inter <= edge[here].bottomY && inter <= interesting[i]->bottomY)
		 		AddLowest (inter);
		 }
		n = ninteresting++;
		interesting[n] = &edge[here];
		interesting[n]->pair = NULL;
		interesting[n]->up = FALSE;
		interesting[n]->startingY = interesting[n]->where = edge[here].topY;
		interesting[n]->name = names++;
	 }
	i = NextLowest (interestingY);
	if (i)
		nextY = i;
	if (here != nedges && edge[here].topY < nextY)
		nextY = edge[here].topY;
	for (i = 0; i < ninteresting; i++)
	 {
		if (interesting[i]->topY > interestingY && interesting[i]->topY < nextY)
			nextY = interesting[i]->topY;
		if (interesting[i]->bottomY > interestingY && interesting[i]->bottomY < nextY)
			nextY = interesting[i]->bottomY;
	 }
	qsort ((char *) interesting, (unsigned) ninteresting, sizeof (struct edge *), intercmp);
 }

static void FindInfinity ()
 {
 	int i;
 	
	infinity = edge[0].topY;
	for (i = 0; i < nedges; i++)
		if (edge[i].bottomY > infinity)
			infinity = edge[i].bottomY;
 }

/***************************/

static int edgecmp (a, b) char *a, *b;
 {
 	struct edge *aa = (struct edge *) a, *bb = (struct edge *) b;
 	
 	return aa->topY - bb->topY;
 }

static int AddEdge (e, from, to, clip) struct edge *e; HardPoint from, to; int clip;
 {
 	int dir = 1;
 	HardPoint temp;
 	
 	if ((int) (from.hy) == (int) (to.hy))
 		return 0;
 	if ((int) (from.hy) > (int) (to.hy))
 	 {
 		temp = from; from = to; to = temp;
 		dir = -dir;
 	 }
 	e->topX = from.hx;
  	e->topY = from.hy;
	e->bottomX = to.hx;
 	e->bottomY = to.hy;
	e->dir = dir;
	e->clip = clip;
	
 	return 1;
 }

static void BuildEdgeList (path, clip) Path path; int clip;
 {
 	Path p;
 	HardPoint move, here;
 	
 	for (p = path->next; p != path; p = p->next)
 	 switch (p->ptype)
 	  {
 	  	case EMove: move = here = p->pe.point; break;
 	  	case ELine:
 	  		nedges += AddEdge (&edge[nedges], here, p->pe.point, clip);
 	  		here = p->pe.point;
 	  		break;
 	  	
 	  	case EClose:
 	  		nedges += AddEdge (&edge[nedges], here, move, clip);
 	  		here = move;
 	  		break;
 	  }
 }

/* keep list of interesting entries to come */

struct lowest
 {
	struct lowest *higher;
	int e;
 } *lowest_free = NULL, *lowest = NULL;

static int NextLowest (y) int y;
 {
	int res;
	struct lowest *p;
	
	for (p = lowest; p && p->e <= y; p = lowest)	/* delete any which are now irrelevent */
	 {
	 	lowest = p->higher;
	 	p->higher = lowest_free;
	 	lowest_free = p;
	 }
	
	if (lowest == NULL)
		return 0;
	res = lowest->e;
	
	return res;
 }

static void AddLowest (e) int e;
 {
 	struct lowest *res, *p, *q;
 	
 	for (p = lowest; p && p->e < e; q = p, p = p->higher)
 		;
 	
 	if (p && p->e == e)
 		return;
	if (lowest_free == NULL)
 		res = (struct lowest *) Malloc (sizeof (struct lowest));
 	else
 		res = lowest_free, lowest_free = lowest_free->higher;
 	
  	res->e = e;
 	res->higher = p;
 	if (p != lowest)
 		q->higher = res;
 	else
 		lowest = res;
 }
