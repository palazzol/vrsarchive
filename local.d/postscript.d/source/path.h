/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

#define CBUTT	0
#define CROUND	1
#define CSQUARE	2

#define JMITRE	0
#define JROUND	1
#define JBEVEL	2

#define STROKE_FILL 0
#define STROKE_THIN 1

#define FILL_SINGLE		0
#define FILL_CACHE_DOUBLE	1
#define FILL_DOUBLE		2
#define FILL_DOUBLE_STROKE	3

typedef struct hard_point { float hx, hy; } HardPoint;

enum pelem_type { EHeader, EMove, ELine, EArc, ECurve, EClose };

struct path_element
 {
 	enum pelem_type ptype;
 	union {
  		HardPoint point;
		struct arc { int dir; HardPoint centre; float radius, from, to; } arc;
 		struct bezier { HardPoint x0, x1, x2; } curve;
 	} pe;
 	struct path_element *next, *last;
 };

typedef struct path_element *Path;

extern HardPoint NewHardPoint (), ExtToInt (), MoveHardPoint ();
extern Path NewPath (), PathCopy (), NewMove ();
extern int PNewPath (), PInitMatrix ();
extern Path NewClipPath (), PathRemove (), ReversePath (), FlattenPath ();
extern float Normalise (), PointX (), PointY ();
