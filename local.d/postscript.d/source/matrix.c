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

Object AssignMatrix ();
extern Matrix identity;
static int PMatrix ();
static int PIdentMatrix ();
static int PDefaultMatrix ();
static int PCurrentMatrix ();
static int PSetMatrix ();
static int PTranslate ();
static int PScale ();
static int PRotate ();
static int PConcat ();
static int PConcatMatrix ();
static int PTransform ();
static int PDTransform ();
static int PITransform ();
static int PIDTransform ();
static int PInvertMatrix ();

InitMatrix ()
 {
 	InstallOp ("matrix",		PMatrix,	0, 1, 0, 0);
 	InstallOp ("initmatrix",	PInitMatrix,	0, 0, 0, 0);
 	InstallOp ("identmatrix",	PIdentMatrix,	1, 1, 0, 0, Array);
 	InstallOp ("defaultmatrix",	PDefaultMatrix,	1, 1, 0, 0, Array);
 	InstallOp ("currentmatrix",	PCurrentMatrix,	1, 1, 0, 0, Array);
 	InstallOp ("setmatrix",		PSetMatrix,	1, 0, 0, 0, Array);
 	InstallOp ("translate",		PTranslate,	0, 0, 0, 0);
 	InstallOp ("scale",		PScale,		0, 0, 0, 0);
 	InstallOp ("rotate",		PRotate,	0, 0, 0, 0);
 	InstallOp ("concat",		PConcat,	1, 0, 0, 0, Array);
 	InstallOp ("concatmatrix",	PConcatMatrix,	3, 1, 0, 0, Array, Array, Array);
 	InstallOp ("transform",		PTransform,	0, 0, 0, 0);
 	InstallOp ("dtransform",	PDTransform,	0, 0, 0, 0);
 	InstallOp ("itransform",	PITransform,	0, 0, 0, 0);
 	InstallOp ("idtransform",	PIDTransform,	0, 0, 0, 0);
 	InstallOp ("invertmatrix",	PInvertMatrix,	2, 1, 0, 0, Array, Array);
 }

Object MatrixFrom (m) Matrix m;
 {
 	return AssignMatrix (MakeArray ((Object *) Malloc (6 * sizeof (Object)), 6), m);
 }

int ExtractMatrix (pm, o) Matrix *pm; Object o;
 {
 	int i;
 	float temp[6];
 	
 	for (i = 0; i < 6; i++)
 	 {
 	 	Object item;
 	 	
 	 	item = getArray (o, i);
 	 	if (TypeOf (item) == Integer)
 	 		temp[i] = (float) BodyInteger (item);
 	 	else if (TypeOf (item) == Real)
 	 		temp[i] = BodyReal (item);
 	 	else
 	 		return FALSE;
 	 }
 	
 	*pm = NewMatrix (temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
 	return TRUE;
 }

static int PMatrix ()
 {
	return Push (OpStack, MatrixFrom (identity));
 }

int PInitMatrix ()
 {
 	gstate->CTM = gstate->device->default_matrix;
 	
 	return TRUE;
 }

static int PIdentMatrix (array) Object array;
 {
 	if (lengthArray (array) != 6)
 		return Error (PRangeCheck);
	VOID AssignMatrix (array, identity);
	return Push (OpStack, array);
 }

static int PDefaultMatrix (array) Object array;
 {
 	if (lengthArray (array) != 6)
 		return Error (PRangeCheck);
	VOID AssignMatrix (array, gstate->device->default_matrix);
	return Push (OpStack, array);
 }

static int PCurrentMatrix (array) Object array;
 {
 	if (lengthArray (array) != 6)
  		return Error (PRangeCheck);
	VOID AssignMatrix (array, gstate->CTM);
	return Push (OpStack, array);
 }

static int PSetMatrix (array) Object array;
 {
 	if (lengthArray (array) != 6)
   		return Error (PRangeCheck);
 	else if (!ExtractMatrix (&gstate->CTM, array))
    		return Error (PTypeCheck);
	else
		return TRUE;
 }

static int PTranslate ()
 {
 	Object tx, ty, mat;
 	float x, y;
 	
 	if (!OpCheck (2, 1))
 		return FALSE;
 	mat = Pop (OpStack);
	if (TypeOf (mat) == Array)
 		if (!wCheck (mat))
 		 	return Push (OpStack, mat), Error (PInvAccess);
 		else if (lengthArray (mat) != 6)
 		 	return Push (OpStack, mat), Error (PRangeCheck);
 		else
 		 {
 		 	ty = Pop (OpStack);
 		 	if (TypeOf (ty) == Integer)
 		 		y = (float) BodyInteger (ty);
 		 	else if (TypeOf (ty) == Real)
 		 		y = BodyReal (ty);
 		 	else
 		 		return Push (OpStack, ty), Push (OpStack, mat), Error (PTypeCheck);
 		 	tx = Pop (OpStack);
 		 	if (TypeOf (tx) == Integer)
 		 		x = (float) BodyInteger (tx);
 		 	else if (TypeOf (tx) == Real)
 		 		x = BodyReal (tx);
 		 	else
  		 		return Push (OpStack, tx), Push (OpStack, ty), Push (OpStack, mat), Error (PTypeCheck);
		 	
 		 	VOID AssignMatrix (mat, NewMatrix (1.0, 0.0, 0.0, 1.0, x, y));
 		 	
 		 	return Push (OpStack, mat);
 		 }
 	else if (TypeOf (mat) == Integer)
 		y = (float) BodyInteger (mat);
 	else if (TypeOf (mat) == Real)
 		y = BodyReal (mat);
 	else
 		return Push (OpStack, mat), Error (PTypeCheck);
 	
 	tx = Pop (OpStack);
 	if (TypeOf (tx) == Integer)
 		x = (float) BodyInteger (tx);
 	else if (TypeOf (tx) == Real)
 		x = BodyReal (tx);
 	else
 		return Push (OpStack, tx), Push (OpStack, mat), Error (PTypeCheck);
 	gstate->CTM = Translate (gstate->CTM, x, y);
 	return TRUE;
 }

static int PScale ()
 {
 	Object tx, ty, mat;
 	float x, y;
 	
 	if (!OpCheck (2, 1))
 		return FALSE;
 	mat = Pop (OpStack);
	if (TypeOf (mat) == Array)
 		if (!wCheck (mat))
 		 	return Push (OpStack, mat), Error (PInvAccess);
 		else if (lengthArray (mat) != 6)
 		 	return Push (OpStack, mat), Error (PRangeCheck);
 		else
 		 {
		 	ty = Pop (OpStack);
 		 	if (TypeOf (ty) == Integer)
 		 		y = (float) BodyInteger (ty);
 		 	else if (TypeOf (ty) == Real)
 		 		y = BodyReal (ty);
 		 	else
 		 		return Push (OpStack, ty), Push (OpStack, mat), Error (PTypeCheck);
 		 	tx = Pop (OpStack);
 		 	if (TypeOf (tx) == Integer)
 		 		x = (float) BodyInteger (tx);
 		 	else if (TypeOf (tx) == Real)
 		 		x = BodyReal (tx);
 		 	else
  		 		return Push (OpStack, tx), Push (OpStack, ty), Push (OpStack, mat), Error (PTypeCheck);
		 	
 		 	VOID AssignMatrix (mat, NewMatrix (x, 0.0, 0.0, y, 0.0, 0.0));
 		 	
 		 	return Push (OpStack, mat);
 		 }
 	else if (TypeOf (mat) == Integer)
 		y = (float) BodyInteger (mat);
 	else if (TypeOf (mat) == Real)
 		y = BodyReal (mat);
 	else
 		return Push (OpStack, mat), Error (PTypeCheck);
 	
 	tx = Pop (OpStack);
 	if (TypeOf (tx) == Integer)
 		x = (float) BodyInteger (tx);
 	else if (TypeOf (tx) == Real)
 		x = BodyReal (tx);
 	else
 		return Push (OpStack, tx), Push (OpStack, mat), Error (PTypeCheck);
 	gstate->CTM = Scale (gstate->CTM, x, y);
 	return TRUE;
 }

static int PRotate ()
 {
 	Object ang, mat;
 	float a;
 	
 	if (!OpCheck (1, 1))
 		return FALSE;
 	mat = Pop (OpStack);
	if (TypeOf (mat) == Array)
 		if (!wCheck (mat))
 		 	return Push (OpStack, mat), Error (PInvAccess);
 		else if (lengthArray (mat) != 6)
 		 	return Push (OpStack, mat), Error (PRangeCheck);
 		else
 		 {
		 	ang = Pop (OpStack);
 		 	if (TypeOf (ang) == Integer)
 		 		a = (float) BodyInteger (ang);
 		 	else if (TypeOf (ang) == Real)
 		 		a = BodyReal (ang);
 		 	else
 		 		return Push (OpStack, ang), Push (OpStack, mat), Error (PTypeCheck);
 		 	
 		 	VOID AssignMatrix (mat, NewMatrix (cos(a), sin(a), -sin(a), -cos(a), 0.0, 0.0));
 		 	
 		 	return Push (OpStack, mat);
 		 }
 	else if (TypeOf (mat) == Integer)
 		a = (float) BodyInteger (mat);
 	else if (TypeOf (mat) == Real)
 		a = BodyReal (mat);
 	else
 		return Push (OpStack, mat), Error (PTypeCheck);
 	
 	gstate->CTM = Rotate (gstate->CTM, Rad (a));
 	return TRUE;
 }

static int PConcat (mat) Object mat;
 {
 	Matrix m;
 	
 	if (!ExtractMatrix (&m, mat))
 		return Error (PTypeCheck);
 	gstate->CTM = MatMult (m, gstate->CTM);
 	return TRUE;
 }

static int PConcatMatrix (mat1, mat2, mat3) Object mat1, mat2, mat3;
 {
 	Matrix m1, m2;
 	
 	if (!ExtractMatrix (&m1, mat1) || !ExtractMatrix (&m2, mat2) ||
 			lengthArray (mat3) != 6)
 		return Error (PTypeCheck);
 	VOID AssignMatrix (mat3, MatMult (m1, m2));
 	VOID Push (OpStack, mat3);
 	return TRUE;
 }

static int Transfn (fn) Vector (*fn)();
 {
 	Matrix m;
 	Object tx, ty, mat;
 	Vector v;
 	float x, y;
 	int mflag = FALSE;
 	
 	m = gstate->CTM;
	if (!OpCheck (2, 1))
 		return FALSE;
 	
 	mat = Pop (OpStack);
	if (TypeOf (mat) == Array)
	 {
 		if (lengthArray (mat) != 6 || !ExtractMatrix (&m, mat))
 		 	return Push (OpStack, mat), Error (PTypeCheck);
 		mflag = TRUE;
 	 }
	else
 		VOID Push (OpStack, mat);
	
	if (!OpCheck (2, 1))
 		return Push (OpStack, mat), FALSE;
 	ty = Pop (OpStack);
	if (TypeOf (ty) == Integer)
		y = (float) BodyInteger (ty);
	else if (TypeOf (ty) == Real)
		y = BodyReal (ty);
	else
	 {
	 	VOID Push (OpStack, ty);
	 	if (mflag)
			VOID Push (OpStack, mat);
		return Error (PTypeCheck);
	 }
	tx = Pop (OpStack);
 	if (TypeOf (tx) == Integer)
 		x = (float) BodyInteger (tx);
 	else if (TypeOf (tx) == Real)
 		x = BodyReal (tx);
 	else
	 {
	 	VOID Push (OpStack, tx);
	 	VOID Push (OpStack, ty);
	 	if (mflag)
			VOID Push (OpStack, mat);
		return Error (PTypeCheck);
	 }
	
	v = (*fn) (NewVector (x, y, 1.0), m);
	 	
	return Push (OpStack, MakeReal (v.vx)), Push (OpStack, MakeReal (v.vy));
 }

static int PTransform ()
 {
 	return Transfn (Transform);
 }

static int PDTransform ()
 {
 	return Transfn (DTransform);
 }

static int PITransform ()
 {
 	return Transfn (ITransform);
 }

static int PIDTransform ()
 {
 	return Transfn (IDTransform);
 }

static int PInvertMatrix (mat1, mat2) Object mat1, mat2;
 {
 	Matrix m;
 	
 	if (!ExtractMatrix (&m, mat1) || lengthArray (mat2) != 6)
 		return Error (PTypeCheck);
 	VOID AssignMatrix (mat2, MatInvert (m));
 	VOID Push (OpStack, mat2);
 	return TRUE;
 }

Point IntToExt (p) HardPoint p;
 {
 	Vector v;
 	
 	v = Transform (NewVector ((float) p.hx, (float) p.hy, 1.0), MatInvert (gstate->CTM));
 	return NewPoint (v.vx, v.vy);
 }

HardPoint ExtToInt (p) Point p;
 {
	Vector v;
	
	v = Transform (NewVector (p.x, p.y, 1.0), gstate->CTM);
	return NewHardPoint (v.vx, v.vy);
 }

Matrix PointTranslate (m, p) Matrix m; Point p;
 {
 	return Translate (m, p.x, p.y);
 }

Matrix HardPointTranslate (m, p) Matrix m; HardPoint p;
 {
 	m.tx += p.hx;
 	m.ty += p.hy;
 	
 	return m;
 }

Object AssignMatrix (o, m) Object o; Matrix m;
 {
 	Object *vec = BodyArray (o);
 	
 	vec[0] = MakeReal (m.A);
 	vec[1] = MakeReal (m.B);
 	vec[2] = MakeReal (m.C);
 	vec[3] = MakeReal (m.D);
 	vec[4] = MakeReal (m.tx);
 	vec[5] = MakeReal (m.ty);
 	
 	return o;
 }
