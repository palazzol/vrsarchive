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

static Object OpFor;

static int For (), PFor (), Eq (), Lt (), Le (), Gt (), Ge (), Identity ();
static int Abs (), Add (), Sub (), Mul (), Div (), Neg (), EqEq (), Cvi (), Cvs ();
static int Exp (), Sqrt (), Sin (), Cos (), Atan (), Ln (), Log (), Ceiling (), Floor (), Round (), Truncate ();

InitReal ()
 {
 	OpFor = MakeOp ("(forreal)", For, 0, 1, 5, 6, Real, Real, Real, Array);
 	
	TypeInstallOp (Real, "cvr", 	Identity,	1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "cvi", 	Cvi,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "cvs", 	Cvs,		2, 1, 0, 0, Real, String);
	TypeInstallOp (Real, "==", 	EqEq,		1, 0, 0, 0, Real);
 	TypeInstallOp (Real, "for", 	PFor,		4, 0, 0, 6, Real, Real, Real, Array);
  	TypeInstallOp (Real, "eq", 	Eq,		2, 1, 0, 0, Real, Real);
  	TypeInstallOp (Real, "lt", 	Lt,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "le", 	Le,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "gt", 	Gt,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "ge", 	Ge,		2, 1, 0, 0, Real, Real);
	TypeInstallOp (Real, "abs", 	Abs,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "add",	Add,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "sub", 	Sub,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "mul", 	Mul,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "div", 	Div,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "neg", 	Neg,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "exp", 	Exp,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "sqrt", 	Sqrt,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "sin", 	Sin,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "cos", 	Cos,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "atan",	Atan,		2, 1, 0, 0, Real, Real);
 	TypeInstallOp (Real, "ln", 	Ln,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "log",	Log,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "ceiling", Ceiling,	1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "floor", 	Floor,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "round", 	Round,		1, 1, 0, 0, Real);
 	TypeInstallOp (Real, "truncate",Truncate,	1, 1, 0, 0, Real);
 }

Object MakeReal (f) float f;
 {
 	Object res;
 	
 	res = MakeObject (Real);
 	res.u.Real = f;
 	
 	return res;
 }

static Object Make (f) float f;
 {
 	Object res;
 	
 	res = MakeObject (Real);
 	res.u.Real = f;
 	
 	return res;
 }

static int Identity (item) Object item;
 {
	return Push (OpStack, item);
 }

static int Cvi (item) Object item;
 {
 	return Push (OpStack, IntReal (item));
 }

static int Cvs (v, string) Object v, string;
 {
 	char buf [BUFSIZ];
 	int length;
 	
	VOID sprintf (buf, "%g", BodyReal (v));
	if ((length = strlen (buf)) > lengthString (string))
		return Error (PRangeCheck);
	VOID Bcopy (BodyString (string), buf, length);
	return Push (OpStack, getIString (string, 0, length));
 }

static int EqEq (v) Object v;
 {
	printf ("%g", BodyReal (v));
	return TRUE;
 }

float BodyReal (item) Object item;
 {
 	return item.u.Real;
 }

static float Body (item) Object item;
 {
	return item.u.Real;
 }

Object RealInteger (o) Object o;
 {
 	return Make ((float) BodyInteger (o));
 }

static int PFor (initial, increment, limit, proc) Object initial, increment, limit, proc;
 {
	VOID Push (ExecStack, Nil);
	VOID Push (ExecStack, increment);
	VOID Push (ExecStack, limit);
	VOID Push (ExecStack, initial);
	VOID Push (ExecStack, proc);
	VOID Push (ExecStack, OpFor);
	
	return TRUE;
 }

static int For () 
 {
	Object current, limit, increment, proc;
	
	proc	  = Pop (ExecStack);
	current   = Pop (ExecStack);
	limit	  = Pop (ExecStack);
	increment = Pop (ExecStack);
	
	if (Body (increment) > 0
				? Body (current) > Body (limit)
				: Body (current) < Body (limit))
	 	VOID Pop (ExecStack);
	else
	 {
	 	VOID Push (ExecStack, increment);
	 	VOID Push (ExecStack, limit);
	 	VOID Push (ExecStack, Make (Body (current) + Body (increment)));
	 	VOID Push (ExecStack, proc);
	 	VOID Push (ExecStack, OpFor);
	 	VOID Push (ExecStack, proc);
	 	VOID Push (OpStack, current);
	 }
	return TRUE;
 }

static int Eq (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (Body (a) == Body (b)));
 }

static int Lt (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (Body (a) < Body (b)));
 }

static int Le (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (Body (a) <= Body (b)));
 }

static int Gt (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (Body (a) > Body (b)));
 }

static int Ge (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (Body (a) >= Body (b)));
 }

static int Abs (v) Object v;
 {
  	return Push (OpStack, Make (Body (v) >= 0 ? Body (v) : -Body (v)));
 }

static int Add (a, b) Object a, b;
 {
 	return Push (OpStack, Make (Body (a) + Body (b)));
 }

static int Sub (a, b) Object a, b;
 {
 	return Push (OpStack, Make (Body (a) - Body (b)));
 }

static int Mul (a, b) Object a, b;
 {
 	return Push (OpStack, Make (Body (a) * Body (b)));
 }

static int Div (a, b) Object a, b;
 {
 	return Push (OpStack, Make (Body (a) / Body (b)));
 }

static int Neg (a) Object a;
 {
	return Push (OpStack, Make (-Body (a)));
 }

static int Sqrt (v) Object v;
 {
 	return Push (OpStack, Make ((float) sqrt ((double) Body (v))));
 }

static int Exp (a, b) Object a, b;
 {
	return Push (OpStack, Make ((float) pow ((double) Body (a), (double) Body (b))));
 }

static int Ceiling (v) Object v;
 {
 	return Push (OpStack, Make ((float) ceil ((double) Body (v))));
 }

static int Floor (v) Object v;
 {
 	return Push (OpStack, Make ((float) floor ((double) Body (v))));
 }

static int Round (v) Object v;
 {
	return Push (OpStack, Make (Body (v) - floor ((double) Body (v)) >= 0.5
 					? ceil ((double) Body (v))
 					: floor ((double) Body (v))));
 }

static int Truncate (v) Object v;
 {
 	return Push (OpStack, Make (Body (v) < 0 ? ceil (Body (v)) : floor (Body (v))));
 }

static int Atan (a, b) Object a, b;
 {
 	return Push (OpStack, Make ((float) Deg (atan2 (Body (a), Body (b)))));
 }

static int Cos (v) Object v;
 {
 	return Push (OpStack, Make (cos (Rad (Body (v)))));
 }

static int Sin (v) Object v;
 {
 	return Push (OpStack, Make (sin (Rad (Body (v)))));
 }

static int Ln (v) Object v;
 {
 	return Push (OpStack, Make (log (Body (v))));
 }

static int Log (v) Object v;
 {
 	return Push (OpStack, Make (log10 (Body (v))));
 }
