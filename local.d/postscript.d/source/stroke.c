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

int PStrokePath ();

int in_stroke = FALSE;

int Stroke ();

InitStroke ()
 {
 	InstallOp ("strokepath",	PStrokePath,	0, 0, 0, 0);
 	InstallOp ("stroke",		Stroke,		0, 0, 0, 0);
 }

int Stroke ()
 {
 	int res;
 	
 	if (!PFlattenPath ())
 		return FALSE;
 	if (EmptyPath (gstate->path))
 		return TRUE;
 	if (gstate->device->dev == NULL)
 		return PNewPath ();
 	if (ThinStroke ())
 		return TRUE;
 	in_stroke = TRUE;
 	res = PStrokePath () && Fill ();
	VOID PNewPath ();
	in_stroke = FALSE;
	
	return res;
 }

float Magnitude (v) Vector v;
 {
 	return sqrt (v.vx * v.vx + v.vy * v.vy);
 }

static void Dash ()
 {
 	Matrix m;
 	Path p, new = NewPath (), last_move, last_dash;
 	HardPoint move, here, next;
 	Vector v, unit;
 	int marking, start_marking = TRUE, index, start_index = 0;
 	float dash_left, start_dash_left = gstate->dash_offset;
 	float umag, dmag;
 	
 	m = gstate->CTM;
 	m = NewMatrix (m.A, m.B, m.C, m.D, 0.0, 0.0);
 	
 	while (start_dash_left > gstate->dash_array [start_index])
 	 {
 	 	start_dash_left -= gstate->dash_array [start_index];
 	 	if (++start_index == gstate->dash_length)
 	 		start_index = 0;
 	 	start_marking = !start_marking;
 	 }
 	start_dash_left = gstate->dash_array [start_index] - start_dash_left;
 	
 	for (p = gstate->path->next; p != gstate->path; p = p->next)
 	 switch (p->ptype)
 	  {
 	  	case EMove:
 	  		index = start_index;
 	  		marking = start_marking;
 	  		dash_left = start_dash_left;
 	  		here = move = p->pe.point;
 	  		VOID MoveTo (new, here);
 	  		last_move = last_dash = new->last;
 	  		break;
 	  	
 	  	case ELine:
 	  	case EClose:
 	  		next = p->ptype == ELine ? p->pe.point : move;
 	  		v = NewVector (next.hx - here.hx, next.hy - here.hy, 1.0);
 	  		dmag = Magnitude (v);
 	  		if (dmag == 0)
 	  		 {
 	  		 	here = next;
 	  			break;
 	  		 }
 	  		umag = Magnitude (ITransform (v, m));
 	  		unit = NewVector (v.vx / umag, v.vy / umag, 1.0);
 	  		while (umag > dash_left)
 	  		 {
 	  		 	here.hx += unit.vx * dash_left;
 	  		 	here.hy += unit.vy * dash_left;
 	  		 	(*(marking ? LineTo : MoveTo)) (new, here);
 	  		 	if (!marking)
 	  		 		last_dash = new->last;
 	  		 	marking = !marking;
 	  		 	umag -= dash_left;
 	  		 	if (++index == gstate->dash_length)
 	  		 		index = 0;
 	  		 	dash_left = gstate->dash_array [index];
 	  		 }
 	  		if (p->ptype == ELine)
 	  		 {
 	  		 	dash_left -= umag;
 	  		 	if (marking)
 	  		 		VOID LineTo (new, next);
 	  		 }
 	  		else
 	  		 {
 	  		 	if (marking)
 	  		 	 {
 	  		 	 	if (start_marking)
 	  		 	 		if (last_dash == last_move)
 	  		 	 			ClosePath (new);
 	  		 	 		else
 	  		 	 		 {
	  		 	 		 /*	LineTo (new, move);	*/
 	  		 	 			MoveChunk (last_move, last_dash, new->last);
 	  		 	 		 }
 	  		 	 	else
 	  		 	 		VOID LineTo (new, move);
 	  		 	 }
 	  		 }
 	  		here = next;
 	  		break;
 	  	
 	  	default:
 	  		Panic ("Dash: unknown Path Element type");
 	  }
 	PathFree (gstate->path);
 	gstate->path = new;
 	gstate->cp = new->next->pe.point;
 }

MoveChunk (dest, begin, end) Path dest, begin, end;
 {
	dest->ptype = ELine;
	end->next->last = begin->last;
	begin->last->next = end->next;
	dest->last->next = begin;
	
	end->next = dest;
	begin->last = dest->last;
	dest->last = end;
 }

/*
 * is 'b' to the left of 'a' ? if 'a' carries into 'b' ?
 *
 *			   /
 *			  /
 *			 b
 *			/
 *		       /
 * --------- a -------o
 * 
 */

leftof (a, b) float a, b;
 {
	return Normalise (b - a) > 0;
 }

Miter (new, last_angle, angle, width) Path new; float last_angle, angle, width;
 {
 	Matrix old;
 	float diff;
	
	old = gstate->CTM;
	if (leftof (last_angle, angle))
	 {
	 	diff = angle - last_angle;
		VOID MoveTo (new, ExtToInt (NewPoint (0.0, 0.0)));
		gstate->CTM = Rotate (gstate->CTM, -diff);
		VOID LineTo (new, ExtToInt (NewPoint (0.0, -width / 2)));
		gstate->CTM = Rotate (gstate->CTM, diff / 2);
		VOID LineTo (new, ExtToInt (NewPoint (0.0, -(width / 2) / cos (diff / 2))));
		gstate->CTM = old;
		VOID LineTo (new, ExtToInt (NewPoint (0.0, -width / 2)));
	 }	
	else
	 {
	 	diff = last_angle - angle;
		VOID MoveTo (new, ExtToInt (NewPoint (0.0, 0.0)));
		VOID LineTo (new, ExtToInt (NewPoint (0.0, width / 2)));
		gstate->CTM = Rotate (gstate->CTM, diff / 2);
		VOID LineTo (new, ExtToInt (NewPoint (0.0, (width / 2) / cos (diff / 2))));
		gstate->CTM = Rotate (gstate->CTM, diff / 2);
		VOID LineTo (new, ExtToInt (NewPoint (0.0, width / 2)));
		gstate->CTM = old;
	 }
	ClosePath (new);
 }

Bevel (new, last_angle, angle, width) Path new; float last_angle, angle, width;
 {
 	Matrix old;
 	float diff;
	
	old = gstate->CTM;
	if (leftof (last_angle, angle))
	 {
	 	diff = angle - last_angle;
		VOID MoveTo (new, ExtToInt (NewPoint (0.0, 0.0)));
		gstate->CTM = Rotate (gstate->CTM, -diff);
		VOID LineTo (new, ExtToInt (NewPoint (0.0, -width / 2)));
		gstate->CTM = old;
		VOID LineTo (new, ExtToInt (NewPoint (0.0, -width / 2)));
	 }
	else
	 {
	 	diff = last_angle - angle;
		VOID MoveTo (new, ExtToInt (NewPoint (0.0, 0.0)));
		VOID LineTo (new, ExtToInt (NewPoint (0.0, width / 2)));
		gstate->CTM = Rotate (gstate->CTM, diff);
		VOID LineTo (new, ExtToInt (NewPoint (0.0, width / 2)));
		gstate->CTM = old;
	 }
	ClosePath (new);
 }

BeginCap (new, width) Path new; float width;
 {
	switch (gstate->line_cap)
	 {
	 	case CBUTT: break;
	 	
	 	case CSQUARE:
	 		VOID MoveTo (new, ExtToInt (NewPoint (0.0, -width / 2)));
	 		VOID LineTo (new, ExtToInt (NewPoint (0.0, width / 2)));
	 		VOID LineTo (new, ExtToInt (NewPoint (-width / 2, width / 2)));
	 		VOID LineTo (new, ExtToInt (NewPoint (-width / 2, -width / 2)));
	 		ClosePath (new);
	 		break;
	 		
	 	case CROUND:
	 		VOID MoveTo (new, ExtToInt (NewPoint (0.0, width / 2)));
	 		Arc (new, 1, NewPoint (0.0, 0.0), width / 2, PI / 2, 3 * PI / 2);
	 		ClosePath (new);
	 		break;
	 		
	 	default:
	 		Panic ("BeginCap  - unknown line cap encountered");
	 }
 }

LineJoin (new, width, last_angle, angle) Path new; float width, last_angle, angle;
 {
 	float ang, sa;
 	
	switch (gstate->line_join)
	 {
	 	case JMITRE:
	 		ang = Normalise (leftof (last_angle, angle) ? angle - last_angle : last_angle - angle);
	 		ang = ang < 0 ? -ang : ang;
	 		ang = ang > PI / 2 ? PI - ang : ang;
	 		sa = sin (ang / 2);
	 		if (sa != 0 && 1 / sa <= gstate->miter_limit && 1 / sa >= -gstate->miter_limit)
	 			Miter (new, last_angle, angle, width);
	 		else
	 			Bevel (new, last_angle, angle, width);
	 		
	 		break;
	 	
	 	case JROUND:
	 		VOID MoveTo (new, ExtToInt (NewPoint (width / 2, 0.0)));
	 		Arc (new, 1, NewPoint (0.0, 0.0), width / 2, 0.0, 2 * PI);
	 		ClosePath (new);
	 		break;
	 	
	 	case JBEVEL: Bevel (new, last_angle, angle, width); break;
	 	
	 	default:
	 		Panic ("LineJoin  - unknown line join encountered");
	 }
 }

EndCap (new, width, length) Path new; float width, length;
 {
	switch (gstate->line_cap)
	 {
	 	case CBUTT: break;
	 	
	 	case CSQUARE:
	 		VOID MoveTo (new, ExtToInt (NewPoint (length, -width / 2)));
	 		VOID LineTo (new, ExtToInt (NewPoint (length + width / 2, -width / 2)));
	 		VOID LineTo (new, ExtToInt (NewPoint (length + width / 2, width / 2)));
	 		VOID LineTo (new, ExtToInt (NewPoint (length, width / 2)));
	 		ClosePath (new);
	 		break;
	 		
	 	case CROUND:
	 		VOID MoveTo (new, ExtToInt (NewPoint (length, -width / 2)));
	 		Arc (new, 1, NewPoint (length, 0.0), width / 2, -PI / 2, PI / 2);
	 		ClosePath (new);
	 		break;
	 		
	 	default:
	 		Panic ("StrokeLineEnd  - unknown line cap encountered");
	 }
 }

static float move_angle;
static Matrix move_matrix;

float LineSegment (p, new, ehere, enow, width, last_angle, last_type) Path p, new; Point ehere, enow; float width, last_angle; enum pelem_type last_type;
 {
 	float angle = atan2 (enow.y - ehere.y, enow.x - ehere.x),
 		length = sqrt ((enow.y - ehere.y) * (enow.y - ehere.y) + (enow.x - ehere.x) * (enow.x - ehere.x));
	Matrix old;
	
	old = gstate->CTM;
	
	gstate->CTM = Rotate (Translate (gstate->CTM, ehere.x, ehere.y), angle);
	VOID MoveTo (new, ExtToInt (NewPoint (0.0, -width / 2)));
	VOID LineTo (new, ExtToInt (NewPoint (length, -width / 2)));
	VOID LineTo (new, ExtToInt (NewPoint (length, width / 2)));
	VOID LineTo (new, ExtToInt (NewPoint (0.0, width / 2)));
	ClosePath (new);
	
	if (last_type == EMove)
	 {
	 	move_angle = angle;
		move_matrix = gstate->CTM;
	 }
	else if (last_type == ELine)
		LineJoin (new, width, last_angle, angle);
	
	if (p->ptype == EClose)
	 {
		if (last_type == ELine)
		 {
		 	gstate->CTM = move_matrix;
			LineJoin (new, width, angle, move_angle);
		 }
	 }
	else if (p->next->ptype == EMove || p->next->ptype == EHeader)
	 {
		EndCap (new, width, length);
	 	gstate->CTM = move_matrix;
	 	BeginCap (new, width);
	 }
	
	gstate->CTM = old;
	return angle;
 }

int PStrokePath ()
 {
 	Path p, new = NewPath ();
 	HardPoint prev, here, move;
 	enum pelem_type last_type = EHeader;
 	float angle, last_angle, width = gstate->line_width;
 	
 	PFlattenPath ();
 	if (gstate->dash_length != 0)
 		Dash ();
 	for (p = gstate->path->next; p != gstate->path; last_type = p->ptype, p = p->next)
 	 {
	 	switch (p->ptype)
	 	 {
	 	  	case EMove:
			 	prev = here;
			 	move = here = p->pe.point;
			 	break;
			 	
	 	  	case EClose:
	 	  		if (last_type == EMove)
	 	  			break;
				angle = LineSegment (p, new, IntToExt (here), IntToExt (move), width, last_angle, last_type);
			 	prev = here;
			 	here = move;
			 	last_type = EHeader;
			 	break;
			 	
	 	  	case ELine:
				angle = LineSegment (p, new, IntToExt (here), IntToExt (p->pe.point), width, last_angle, last_type);
			 	prev = here;
			 	here = p->pe.point;
	 	  		break;
	 	  		
	 	  	default:
	 	  		Panic ("unknown path element type in StrokePath");
	 	 }
	 	last_type = p->ptype;
	 	last_angle = angle;
	 }
 	PathFree (gstate->path);
 	gstate->path = new;
 	return TRUE;
 }

int ThinStroke ()
 {
 	Path p;
 	Vector v;
 	HardPoint here, prev;
 	
 	if (stroke_method != STROKE_THIN)
 		return FALSE; /* not used - get better results with area fill */
 	v = Transform (NewVector (gstate->line_width, gstate->line_width, 0.0), gstate->CTM);
 	if (fabs (v.vx) > 1.1 || fabs (v.vy) > 1.1)
 		return FALSE;
 	if (gstate->dash_length != 0)
 		Dash ();
 	for (p = gstate->path->next; p != gstate->path; p = p->next)
 	 switch (p->ptype)
 	  {
 	  	case EMove:
 	  		here = prev = p->pe.point;
 	  		break;
 	  	
 	  	case ELine:
 	  		DevicePaintLine (gstate->device, prev, p->pe.point, gstate->colour);
 	  		prev = p->pe.point;
 	  		break;
 	  	
 	  	case EClose:
 	  		DevicePaintLine (gstate->device, prev, here, gstate->colour);
  	  		prev = here;
	  		break;
 	  		
	 	default:
	 	  	Panic ("unknown path element type in ThinStroke");
 	  }
	VOID PNewPath ();
	
	return TRUE;
 }
