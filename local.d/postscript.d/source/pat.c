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

#define MAXPATHELEMENTS 1500

static struct path_element *free_pelem = NULL;
 
Path NewPath ();
Point NewPoint (), IntToExt ();

HardPoint NewHardPoint (), ExtToInt ();

Path NewElem (type) enum pelem_type type;
 {
 	Path res = free_pelem;
 	
 	if (res != NULL)
 		free_pelem = res->next;
	else
	 	res = (Path) Malloc (sizeof (struct path_element));
 	res->ptype = type;

 	return res;
 }

void ElemFree (p) Path p;
 {
 	PanicIf (p == NULL, "ElemFree given NULL");
 	p->next = free_pelem;
 	free_pelem = p;
 }

Path ElemCopy (p) Path p;
 {
	Path res = NewElem (p->ptype);
	res->pe = p->pe;
 	return res;
 }

Path NewMove (type, point) enum pelem_type type; HardPoint point;
 {
 	Path res = NewElem (type);
 	
 	res->pe.point = point;
 	
 	return res;
 }

Path NewPath ()
 {
 	Path res = NewElem (EHeader);
 	
 	return res->next = res->last = res;
 }

PathFree (p) Path p;
 {
 	while (!EmptyPath (p))
 		PathDelete (p);
 	ElemFree (p);
 }

Path PathCopy (path) Path path;
 {
 	Path p, new = NewPath ();
 	
 	for (p = path->next; p != path; p = p->next)
 	 	PathInsert (new, ElemCopy (p));
 	return new;
 }

enum pelem_type CurEType (p) Path p;
 {
 	return p->last->ptype;
 }
	
int EmptyPath (p) Path p;
 {
 	return p->next == p;
 }

PathDelete (p) Path p;
 {
	Path res = p->last;
	
 	PanicIf (p->next == p, "delete from empty path");
 	ElemFree (PathRemove (res));
 	
 	return TRUE;
 }

Path PathRemove (p) Path p;
 {
 	p->last->next = p->next;
 	p->next->last = p->last;
 	
 	return p;
 }

PathInsert (p, i) Path p, i;
 {
 	PanicIf (i == NULL, "PathInsert given NULL");
 	i->next = p;
 	i->last = p->last;
 	p->last->next = i;
 	p->last = i;
 }

int MoveTo (p, np) Path p; HardPoint np;
 {
 	Path res = NewMove (EMove, np);
 	
 	if (res == NULL)
 		return Error (PLimitCheck);
 	if (!EmptyPath (p) && CurEType (p) == EMove)
 	 	PathDelete (p);
 	gstate->cp_defined = TRUE;
 	gstate->cp = np;
 	PathInsert (p, res);
 	return TRUE;
 }

int LineTo (p, np) Path p; HardPoint np;
 {
 	Path res = NewMove (ELine, np);
 	
 	if (res == NULL)
 		return Error (PLimitCheck);
  	gstate->cp = np;
 	PathInsert (p, res);
 	return TRUE;
 }

int CurveTo (p, p0, p1, p2) Path p; HardPoint p0, p1, p2;
 {
 	Path new = NewElem (ECurve);
 	
 	if (new == NULL)
 		return Error (PLimitCheck);
 	new->pe.curve.x0 = p0;
 	new->pe.curve.x1 = p1;
 	new->pe.curve.x2 = p2;
 	gstate->cp = p2;
 	PathInsert (p, new);
 	return TRUE;
 }

float Normalise (ang) float ang;
 {
 	while (ang < -PI || ang > PI)
 		if (ang < PI)
 			ang += 2 * PI;
 		else
 			ang -= 2 * PI;
	return ang;
  }

/*
 * In ArcBit, Y = 0 goes through P0, and the centre is at the origin.
 *
 */

static int ArcBit (p, dir, radius, ang) Path p; int dir; float radius, ang;
 {
 	HardPoint P3;
 	Point p3;
 	float x, y;
 	
 	gstate->CTM = Rotate (gstate->CTM, dir * ang / 2);
 	if (dir < 0)
 		gstate->CTM = MatMult (NewMatrix (1.0, 0.0, 0.0, -1.0, 0.0, 0.0), gstate->CTM);
 	
 	gstate->CTM = Rotate (gstate->CTM, ang / 2);
 	P3 = ExtToInt (NewPoint (radius, 0.0));
 	gstate->CTM = Rotate (gstate->CTM, -ang / 2);
 	
 	p3 = IntToExt (P3);
 	x = (4 * radius - p3.x) / 3;
 	y = p3.y * (radius / cos (ang / 2) - x) / p3.x;
	
 	return CurveTo (p, ExtToInt (NewPoint (x, -y)), ExtToInt (NewPoint (x, y)), P3);
 }

int Arc (p, dir, centre, radius, ang1, ang2) Path p; int dir; Point centre; float radius, ang1, ang2;
 {
 	Matrix old, cent;
 	int res;
 	float diff = dir > 0 ? ang2 - ang1 : ang1 - ang2;
 	float ang;
	
	while (diff < 0)
		diff += 2 * PI;
	old = gstate->CTM;
	cent = Translate (old, centre.x, centre.y);
 	for (ang = 0; ang <= diff - PI / 2; ang += PI / 2)
	 {
	 	gstate->CTM = Rotate (cent, ang1 + ang * dir);
 		if (!ArcBit (p, dir, radius, PI / 2))
 		 {
 		 	gstate->CTM = old;
 			return FALSE;
 		 }
 	 }
 	gstate->CTM = Rotate (cent, ang1 + ang * dir);
 	res = ArcBit (p, dir, radius, diff - ang);
 	gstate->CTM = old;
 	return res;
 }

ArcTo (path, cp, x1, y1, x2, y2, r, xt1, yt1, xt2, yt2) 
		Path path;
		HardPoint cp;
		float x1, y1, x2, y2, r;
		float *xt1, *yt1, *xt2, *yt2;
 {
 	Point p0;
 	float x0, y0, x3, y3, dist,
 		ang, anga, angb, absang, delta;
 	int dir;
 	
 	p0 = IntToExt (cp); x0 = p0.x, y0 = p0.y;
 	angb = atan2 (y2 - y1, x2 - x1);
 	if (angb <= 0)
 		angb += 2 * PI;
 	anga = atan2 (y0 - y1, x0 - x1);
 	if (anga <= 0)
 		anga += 2 * PI;
 	ang = angb - anga;
 	if (ang <= 0)
 		ang += 2 * PI;
 	if (ang >= 2 * PI)
 		ang -= 2 * PI;
 	if (ang >= PI)
 		ang -= 2 * PI;
 	dir = ang <= 0 ? 1 : -1;
 	dist = fabs (r / sin (ang / 2));
 	absang = ang / 2 + anga;
 	x3 = dist * cos (absang) + x1;
 	y3 = dist * sin (absang) + y1;
 	absang += PI;
 	delta = (ang + (ang > 0 ? - PI : PI)) / 2;
	*xt1 = x3 + r * cos (absang - delta);
	*yt1 = y3 + r * sin (absang - delta);
	*xt2 = x3 + r * cos (absang + delta);
	*yt2 = y3 + r * sin (absang + delta);
	LineTo (path, ExtToInt (NewPoint (*xt1, *yt1)));
	Arc (path,
		dir,
		NewPoint (x3, y3),
		r, absang - delta,
		absang + delta);
	return TRUE;
 }

int ClosePath (p) Path p;
 {
 	Path pp, res = NewMove (EClose, ExtToInt (NewPoint (0.0, 0.0)));
 	
 	if (res == NULL)
 		return Error (PLimitCheck);
 	if (!gstate->cp_defined)
 		return TRUE;
 	for (pp = p; pp->ptype != EMove; pp = pp->last)
 		;
 	PathInsert (p, res);
 	VOID MoveTo (p, pp->pe.point);
	return TRUE;
 }

Point NewPoint (x, y) float x, y;
 {
 	Point res;
 	
 	res.x = x; res.y = y;
 	return res;
 }

HardPoint NewHardPoint (x, y) float x, y;
 {
 	HardPoint res;
 	
 	res.hx = x; res.hy = y;
 	return res;
 }

HardPoint MoveHardPoint (a, b) HardPoint a, b;
 {
 	return NewHardPoint (a.hx + b.hx, a.hy + b.hy);
 }

Point MovePoint (a, b) Point a, b;
 {
 	return NewPoint (a.x + b.x, a.y + b.y);
 }

float PointX (p) Point p;
 {
 	return p.x;
 }

float PointY (p) Point p;
 {
 	return p.y;
 }

/*
 * This algorithm subdivides a Bezier curve.
 * The algorithm is taken from:
 *
 *	"The Beta2-split: A special case of the Beta-spline Curve and
 *	 Surface Representation." B. A. Barsky and A. D. DeRose. IEEE, 1985.
 *
 * A vector of control points W is transformed by two matrices into two
 * Vectors which describe the sub-polygons. This is done twice, once each
 * for the X and Y coordinates. The matrices required for the 'left' and
 * 'Right' sub-polygons are as follows:
 *
 *	 /  1    0    0    0  \			 / 1/8  3/8  3/8  1/8 \
 * W  =	|  1/2  1/2   0    0   |	W  =	|   0   1/4  1/2  1/4  |
 *  L	|  1/4  1/2  1/4   0   |	 R	|   0    0   1/2  1/2  |
 *	 \ 1/8  3/8  3/8  1/8 /			 \  0    0    0    1  /
 *
 * The algorithm subdivides the curve recursively until each sub-polygon
 * is deemed to be flat to within the required tolerance. Flatness is
 * defined as the greater distance of the inner control points (P1 & P2)
 * from the line through the outer ones (P0 & P3):
 *
 *						       P2
 *				       /-------------X
 *	      P1	   ------------    	     |\_
 *	       _X---------/			     |	\_
 *	     _/	|				     |	  \_
 *	   _/	|				     |	    \_
 *       _/	|				     |	      \_
 *     _/	| D1				  D2 |		\_
 *   _/		|				     |		  \_
 *  /		|				     |		    \
 * X-----------------------------------------------------------------X
 * P0
 * 							      P3
 */

static int IsFlat (x0, y0, x1, y1, x2, y2, x3, y3) float x0, y0, x1, y1, x2, y2, x3, y3;
 {
 	int flatness = gstate->flatness;
 	float sa, ca, y, O = y3 - y0, A = x3 - x0, H = sqrt (O*O + A*A);
	
	if (H == 0)
		return TRUE;
 	
 	sa = O / H, ca = A / H;
 	y = - sa * (x1 - x0) + ca * (y1 - y0);
 	if (y > flatness || y < -flatness)
 		return FALSE;
 	y =  - sa * (x2 - x0) + ca * (y2 - y0);
 	return y <= flatness && y >= -flatness;
 }

Bezier (x0, y0, x1, y1, x2, y2, x3, y3, flat) int (*flat)(); float x0, y0, x1, y1, x2, y2, x3, y3;
 {
 	if (IsFlat (x0, y0, x1, y1, x2, y2, x3, y3))
 		return (*flat) (x0, y0, x3, y3);
	return Bezier (x0, 					y0,
		(x0 + x1) / 2, 				(y0 + y1) / 2,
		(x0 + x2) / 4 + x1 / 2, 		(y0 + y2) / 4 + y1 / 2,
		(x0 + x3) / 8 + 3 * (x1 + x2) / 8, 	(y0 + y3) / 8 + 3 * (y1 + y2) / 8,
		flat) &&
	Bezier ((x0 + x3) / 8 + 3 * (x1 + x2) / 8, 	(y0 + y3) / 8 + 3 * (y1 + y2) / 8,
		(x1 + x3) / 4 + x2 / 2, 		(y1 + y3) / 4 + y2 / 2,
		(x2 + x3) / 2, 				(y2 + y3) / 2,
		x3, 					y3,
		flat);
 }

static int RevSeg (new, p, next) Path new, next; HardPoint p;
 {
 	switch (next->ptype)
 	 {
 	 	case EClose:
 	 	case EHeader:
 	 	case EMove:	return MoveTo (new, p);
 	 	case ELine:	return LineTo (new, p);
 	 	case ECurve:	return CurveTo (new, next->pe.curve.x1, next->pe.curve.x0, p);
 	 }
	Panic ("RevSeg: unknown pe type");
	return TRUE;
 }

Path ReversePath (path) Path path;
 {
  	Path p, new = NewPath (), pnew = new;
  	int closed = FALSE;
  	
 	for (p = path->last; p != path; p = p->last)
 	 switch (p->ptype)
 	  {
 	  	case EClose:
 	  		closed = TRUE;
 	  		break;
 	  	
 	  	case EMove:
   	  		if (!RevSeg (new, p->pe.point, p->next))
  	  		 {
  	  		 	PathFree (new);
  	  			return NULL;
  	  		 }
 	 		if (closed)
 	 		 {
 	 			if (!ClosePath (new))
  	  			 {
  	  			 	PathFree (new);
  	  				return NULL;
  	  			 }
  	  		 }
   	  		new = new->next;
 	 		closed = FALSE;
 	  		break;
  	  		
	  	case ELine:
 	  		if (!RevSeg (new, p->pe.point, p->next))
   	  		 {
  	  		 	PathFree (new);
  	  			return NULL;
  	  		 }

	  		break;
 	  	
 	  	case ECurve:
  	  		if (!RevSeg (new, p->pe.curve.x2, p->next))
   	  		 {
  	  		 	PathFree (new);
  	  			return NULL;
  	  		 }

	  		break;
	  	
	  	default:
	  		Panic ("ReversePath: unknown element type");
	  }
 	 PathFree (p);
 	 
 	 return pnew;
 }

Path path;

/*ARGSUSED*/
static int flatten (x0, y0, x1, y1) float x0, y0, x1, y1;
 {
	return LineTo (path, NewHardPoint (x1, y1));
 }

Path FlattenPath (arg) Path arg;
 {
 	Path p, new = NewPath ();
 	HardPoint lastp;

 	path = new;
 	for (p = arg->next; p != arg; p = p->next)
 	 switch (p->ptype)
 	  {
 	  	case EMove:
 	  		lastp = p->pe.point;
 	  		if (!MoveTo (new, lastp))
 	  		 {
 	  		 	PathFree (new);
 	  			return NULL;
 	  		 }
 	  		break;
 	  		
 	  	case ELine:
 	  		lastp = p->pe.point;
 	  		if (!LineTo (new, lastp))
 	  		 {
 	  		 	PathFree (new);
 	  			return NULL;
 	  		 }
 	  		break;
 	  		
 	  	case EClose:
 	  		if (!ClosePath (new))
  	  		 {
 	  		 	PathFree (new);
 	  			return NULL;
 	  		 }
	 		break;
 	 	
 	 	case ECurve:
 	 		if (!Bezier (lastp.hx, lastp.hy,
 	 				p->pe.curve.x0.hx, p->pe.curve.x0.hy,
 	 				p->pe.curve.x1.hx, p->pe.curve.x1.hy,
 	 				p->pe.curve.x2.hx, p->pe.curve.x2.hy,
 	 				flatten))
  	  		 {
 	  		 	PathFree (new);
 	  			return NULL;
 	  		 }
	 		lastp = p->pe.curve.x2;
 	 		break;
 	 	
 	 	default:
 	 		Panic ("Flattenpath discovers unknown path element type");
 	 		break;
 	  }
 	return path;
 }

int CloseAll (path) Path path;	/* closes all open portions of a path *in place* */
 {
	Path p;
	enum pelem_type last_type = EHeader;
	
	for (p = path->next; p != path; last_type = p->ptype, p = p->next)
	 if (p->ptype == EMove && last_type != EClose && last_type != EHeader)
	 	if (!ClosePath (p))
	 		return Error (PLimitCheck);
	 if (last_type == EMove)
	 	return PathDelete (p);
	 if (last_type == EHeader || last_type == EClose)
	 	return TRUE;
	 return ClosePath (p);
 }

SetPath (p, v) Path *p, v;
 {
 	PathFree (*p);
 	*p = v;
 }

Bound (left, right, top, bottom, p) float *left, *right, *top, *bottom; HardPoint p;
 {
	if (p.hx < *left) 	*left 	= p.hx;
	if (p.hx > *right) 	*right 	= p.hx;
	if (p.hy < *bottom) 	*bottom = p.hy;
	if (p.hy > *top) 	*top 	= p.hy;
 }

int PathBBox (left, right, top, bottom) float *left, *right, *top, *bottom;
 {
 	Path p;
 	
 	if (!gstate->cp_defined || EmptyPath (gstate->path))
 		return Error (PNoCurrentPoint);
 	
 	*left = *right = gstate->cp.hx;
 	*top = *bottom = gstate->cp.hy;
 	
 	for (p = gstate->path->next; p != gstate->path; p = p->next)
 	 switch (p->ptype)
 	  {
 	  	case EMove:
 	  	case ELine:
 	  		Bound (left, right, top, bottom, p->pe.point);
 	  		break;
 	  	case EClose:
 	  		break;
 	  	case ECurve:
 	  		Bound (left, right, top, bottom, p->pe.curve.x0);
 	  		Bound (left, right, top, bottom, p->pe.curve.x1);
 	  		Bound (left, right, top, bottom, p->pe.curve.x2); 
 	  		break;
 	  	case EHeader:
 	  		Panic ("PathBBox, header found.");
 	  		break;
 	  	default:
 	  		Panic ("PathBBox, unknown path element type.");
 	  		break;
 	  }
 	
 	return TRUE;
 }

UserBound (left, right, top, bottom, p) float *left, *right, *top, *bottom; Point p;
 {
	if (p.x < *left) 	*left 	= p.x;
	if (p.x > *right) 	*right 	= p.x;
	if (p.y < *bottom) 	*bottom = p.y;
	if (p.y > *top) 	*top 	= p.y;
 }
