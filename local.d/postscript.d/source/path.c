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

#define PATHFORALLDEPTH 500
	
static int PCurrentPoint ();
static int PMoveTo ();
static int PRMoveTo ();
static int PLineTo ();
static int PRLineTo ();
static int PArc ();
static int PArcN ();
static int PArcTo ();
static int PCurveTo ();
static int PRCurveTo ();
static int PClosePath ();
static int PPathBBox ();
static int PReversePath ();
static int PPathForAll ();

static int PathForAll ();
static int PPathProc ();

int PFlattenPath ();

Object OpPathForAll;

InitPath ()
 {
 	OpPathForAll = MakeOp ("(oppathforall)", PathForAll, 0, 6, 7, 7);
 	
 	InstallOp ("newpath",		PNewPath,	0, 0, 0, 0);
 	InstallOp ("currentpoint",	PCurrentPoint,	0, 2, 0, 0);
  	InstallOp ("moveto",		PMoveTo,	2, 0, 0, 0, Float, Float);
	InstallOp ("rmoveto",		PRMoveTo,	2, 0, 0, 0, Float, Float);
 	InstallOp ("lineto",		PLineTo,	2, 0, 0, 0, Float, Float);
 	InstallOp ("rlineto",		PRLineTo,	2, 0, 0, 0, Float, Float);
 	InstallOp ("arc",		PArc,		5, 0, 0, 0, Float, Float, Float, Float, Float);
 	InstallOp ("arcn",		PArcN,		5, 0, 0, 0, Float, Float, Float, Float, Float);
 	InstallOp ("arcto",		PArcTo,		5, 4, 0, 0, Float, Float, Float, Float, Float);
 	InstallOp ("curveto",		PCurveTo,	6, 0, 0, 0, Float, Float, Float, Float, Float, Float);
 	InstallOp ("rcurveto",		PRCurveTo,	6, 0, 0, 0, Float, Float, Float, Float, Float, Float);
 	InstallOp ("closepath",		PClosePath,	0, 0, 0, 0);
 	InstallOp ("pathbbox",		PPathBBox,	0, 4, 0, 0);
 	InstallOp ("flattenpath",	PFlattenPath,	0, 0, 0, 0);
 	InstallOp ("reversepath",	PReversePath,	0, 0, 0, 0);
 	InstallOp ("pathforall",	PPathForAll,	4, 0, 0, 6, Array, Array, Array, Array);
 	InstallOp ("pathproc",		PPathProc,	0, 1, 0, 0);
	gstate->path = NewPath ();
	gstate->clip = NewPath ();
 }

int PNewPath ()
 {
 	gstate->cp_defined = FALSE;
 	PathFree (gstate->path);
 	gstate->path = NewPath ();
 	return TRUE;
 }

static int PCurrentPoint ()
 {
 	Point cp;
 	
	if (!gstate->cp_defined)
 		return Error (PNoCurrentPoint);
 	cp = IntToExt (gstate->cp);
 	VOID Push (OpStack, MakeReal (PointX (cp)));
 	VOID Push (OpStack, MakeReal (PointY (cp)));
 	return TRUE;
 }

static int PMoveTo (x, y) Object x, y;
 {
 	return MoveTo (gstate->path, ExtToInt (NewPoint (BodyReal (x), BodyReal (y))));
 }

static int PRMoveTo (x, y) Object x, y;
 {
	if (!gstate->cp_defined)
 		return Error (PNoCurrentPoint);
 	return MoveTo (gstate->path, ExtToInt (MovePoint (NewPoint (BodyReal (x), BodyReal (y)), IntToExt (gstate->cp))));
 }

static int PLineTo (x, y) Object x, y;
 {
	if (!gstate->cp_defined)
 		return Error (PNoCurrentPoint);
	return LineTo (gstate->path, ExtToInt (NewPoint (BodyReal (x), BodyReal (y))));
 }


static int PRLineTo (x, y) Object x, y;
 {
	if (!gstate->cp_defined)
 		return Error (PNoCurrentPoint);
 	return LineTo (gstate->path, ExtToInt (MovePoint (NewPoint (BodyReal (x), BodyReal (y)), IntToExt (gstate->cp))));
 }

static int PArc (x, y, rad, ang1, ang2) Object x, y, rad, ang1, ang2;
 {
 	Point centre, start;
 	float a1 = Rad (BodyReal (ang1)), a2 = Rad (BodyReal (ang2));
 	float r = BodyReal (rad);
 	
 	start = NewPoint (cos (a1) * r, sin (a1) * r);
 	centre = NewPoint (BodyReal (x), BodyReal (y));
 	if (!(*(EmptyPath (gstate->path) ? MoveTo : LineTo)) (gstate->path, ExtToInt (MovePoint (centre, start))))
		return FALSE;
 	return Arc (gstate->path, 1, centre, r, a1, a2);
 }

static int PArcN (x, y, rad, ang1, ang2) Object x, y, rad, ang1, ang2;
 {
 	Point centre, start;
 	float a1 = Rad (BodyReal (ang1)), a2 = Rad (BodyReal (ang2));
 	float r = BodyReal (rad);
 	
 	start = NewPoint (cos (a1) * r, sin (a1) * r);
 	centre = NewPoint (BodyReal (x), BodyReal (y));
 	if (!(*(EmptyPath (gstate->path) ? MoveTo : LineTo)) (gstate->path, ExtToInt (MovePoint (centre, start))))
		return FALSE;
 	return Arc (gstate->path, -1, centre, r, a1, a2);
 }

static int PArcTo (X1, Y1, X2, Y2, rad) Object X1, Y1, X2, Y2, rad;
 {
 	float xt1, xt2, yt1, yt2;
 	if (!gstate->cp_defined)
 		return Error (PNoCurrentPoint);
 	
	return ArcTo (gstate->path, gstate->cp,
			BodyReal (X1), BodyReal (Y1), BodyReal (X2), BodyReal (Y2), BodyReal (rad),
 			&xt1, &yt1, &xt2,  &yt2) &&
 		Push (OpStack, MakeReal (xt1)) &&
		Push (OpStack, MakeReal (yt1)) &&
		Push (OpStack, MakeReal (xt2)) &&
		Push (OpStack, MakeReal (yt2));
 }

static int PCurveTo (x0, y0, x1, y1, x2, y2) Object x0, y0, x1, y1, x2, y2;
 {
	if (!gstate->cp_defined)
 		return Error (PNoCurrentPoint);
 	return CurveTo (gstate->path,
 			ExtToInt (NewPoint (BodyReal (x0), BodyReal (y0))),
 			ExtToInt (NewPoint (BodyReal (x1), BodyReal (y1))),
 			ExtToInt (NewPoint (BodyReal (x2), BodyReal (y2))));
 }

static int PRCurveTo (x0, y0, x1, y1, x2, y2) Object x0, y0, x1, y1, x2, y2;
 {
 	Point cp;
 	
	if (!gstate->cp_defined)
 		return Error (PNoCurrentPoint);
 	cp = IntToExt (gstate->cp);
 	return CurveTo (gstate->path,
 			ExtToInt (MovePoint (cp, NewPoint (BodyReal (x0), BodyReal (y0)))),
 			ExtToInt (MovePoint (cp, NewPoint (BodyReal (x1), BodyReal (y1)))),
 			ExtToInt (MovePoint (cp, NewPoint (BodyReal (x2), BodyReal (y2)))));
 }

static int PClosePath ()
 {
 	return ClosePath (gstate->path);
 }

int PFlattenPath ()
 {
 	Path res;
 	
 	if ((res = FlattenPath (gstate->path)) == NULL)
 		return Error (PLimitCheck);
 	SetPath (&gstate->path, res);
	return TRUE;
 }

static int PReversePath ()
 {
	Path res;
 	
 	if ((res = ReversePath (gstate->path)) == NULL)
 		return Error (PLimitCheck);
  	SetPath (&gstate->path, res);
 	return TRUE;	
 }

static Path pathforall [PATHFORALLDEPTH], *pathp = pathforall;
static int path_depth = 0;

static int PPathForAll (move, line, curve, close) Object move, line, curve, close;
 {
 	Path res;
 	
 	if ((res = PathCopy (gstate->path)) == NULL || path_depth == PATHFORALLDEPTH - 1)
 		return Error (PLimitCheck);
 	*++pathp = res;
 	++path_depth;
 	VOID Push (ExecStack, Nil);
 	VOID Push (ExecStack, move);
 	VOID Push (ExecStack, line);
 	VOID Push (ExecStack, curve);
 	VOID Push (ExecStack, close);
 	VOID Push (ExecStack, OpPathForAll);
 	return TRUE;
 }

static int PathForAll ()
 {
 	Path res;
 	Object move, line, curve, close, fn;
 	Point p;
 	
 	close = Pop (ExecStack);
 	curve = Pop (ExecStack);
 	line = Pop (ExecStack);
 	move = Pop (ExecStack);
 	if (EmptyPath (*pathp))
 	 {
 	 	PathFree (*pathp--);
 	 	--path_depth;
 	 	VOID Pop (ExecStack);
 		return TRUE;
 	 }
 	res = (*pathp)->next;
 	switch (res->ptype)
 	 {
 	 	case EMove:
 	 		p = IntToExt (res->pe.point);
 	 		VOID Push (OpStack, MakeReal (p.x));
 	 		VOID Push (OpStack, MakeReal (p.y));
 	 		fn = move;
 	 		break;
 	 	
 	 	case ELine:
  	 		p = IntToExt (res->pe.point);
 	 		VOID Push (OpStack, MakeReal (p.x));
 	 		VOID Push (OpStack, MakeReal (p.y));
 	 		fn = line;
 	 		break;
 	 	
	 	case ECurve:
	 		p = IntToExt (res->pe.curve.x0);
 	 		VOID Push (OpStack, MakeReal (p.x));
 	 		VOID Push (OpStack, MakeReal (p.y));
	 		p = IntToExt (res->pe.curve.x1);
 	 		VOID Push (OpStack, MakeReal (p.x));
 	 		VOID Push (OpStack, MakeReal (p.y));
	 		p = IntToExt (res->pe.curve.x2);
 	 		VOID Push (OpStack, MakeReal (p.x));
 	 		VOID Push (OpStack, MakeReal (p.y));
	 		fn = curve;
	 		break;
	 	
	 	case EClose:
	 		fn = close;
	 		break;
	 	
	 	default:
	 		Panic ("OpPathForAll - encounters unknown path element");
	 }
	VOID Push (ExecStack, move);
	VOID Push (ExecStack, line);
	VOID Push (ExecStack, curve);
	VOID Push (ExecStack, close);
	VOID Push (ExecStack, OpPathForAll);
	VOID Push (ExecStack, fn);
	
	PathDelete (res->next);
	return TRUE;
 }

static int PPathProc ()
 {
 	Object *body, *p;
 	int sum = 0;
 	Path pa;
 	Point po;
 	
 	for (pa = gstate->path->next; pa != gstate->path; pa = pa->next)
 		switch (pa->ptype)
 		 {
 		 	case EMove: case ELine: sum += 3; break;
 		 	case ECurve: sum += 7; break;
 		 	case EClose: ++sum; break;
 		 }
 	
	p = body = (Object *) Malloc ((unsigned) sizeof (Object) * sum);
 	for (pa = gstate->path->next; pa != gstate->path; pa = pa->next)
 		switch (pa->ptype)
 		 {
 		 	case EMove:
 		 		po = IntToExt (pa->pe.point);
  		 		*p++ = MakeReal (po.x);
 		 		*p++ = MakeReal (po.y);
  		 		*p++ = Cvx (NameFrom ("moveto"));
		 		break;
 		 	
		 	case ELine:
		 		po = IntToExt (pa->pe.point);
  		 		*p++ = MakeReal (po.x);
 		 		*p++ = MakeReal (po.y);
  		 		*p++ = Cvx (NameFrom ("lineto"));
		 		break;
 		 	
 		 	case ECurve:
 		 		po = IntToExt (pa->pe.curve.x0);
  		 		*p++ = MakeReal (po.x);
 		 		*p++ = MakeReal (po.y);
 		 		po = IntToExt (pa->pe.curve.x1);
  		 		*p++ = MakeReal (po.x);
 		 		*p++ = MakeReal (po.y);
		 		po = IntToExt (pa->pe.curve.x2);
  		 		*p++ = MakeReal (po.x);
 		 		*p++ = MakeReal (po.y);
		 		*p++ = Cvx (NameFrom ("curve"));
		 		break;
 		 	
 		 	case EClose:
 		 		*p++ = Cvx (NameFrom ("closepath"));
 		 		break;
 		 }
 	return Push (OpStack, Cvx (MakeArray (body, sum)));
 }

static int PPathBBox ()
 {
 	Point right_top, left_bottom, left_top, right_bottom;
 	float left, right, top, bottom, uleft, uright, utop, ubottom;
 	
 	if (!PathBBox (&left, &right, &top, &bottom))
 		return FALSE;
 	
 	left_bottom	= IntToExt (NewHardPoint (left, bottom));
 	right_bottom	= IntToExt (NewHardPoint (right, bottom));
 	right_top 	= IntToExt (NewHardPoint (right, top));
 	left_top	= IntToExt (NewHardPoint (left, top));
 	
 	uleft = uright = left_bottom.x; utop = ubottom = left_bottom.y;
 	UserBound (&uleft, &uright, &utop, &ubottom, left_top);
 	UserBound (&uleft, &uright, &utop, &ubottom, right_top);
 	UserBound (&uleft, &uright, &utop, &ubottom, right_bottom);
 	
 	VOID Push (OpStack, MakeReal (uleft));
  	VOID Push (OpStack, MakeReal (ubottom));
 	VOID Push (OpStack, MakeReal (uright));
	VOID Push (OpStack, MakeReal (utop));
 	
	return TRUE;
 }
