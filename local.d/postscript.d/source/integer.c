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
#include <signal.h>
#include "main.h"

static Object OpFor;
	
static int For (), PFor (), Eq (), Lt (), Le (), Gt (), Ge (), Not (), And (), Or (), Xor (), BitShift ();
static int Abs (), Add (), Sub (), Mul (), Div (), Mod (), Neg ();
static int Exp (), Sqrt (), Sin (), Cos (), Atan (), Ln (), Log (), Identity ();
static int Cvr (), Cvs (), EqEq ();

static int WordSize, Word2, LowMask;

InitInteger ()
 {
 	unsigned word;
 	
 	OpFor = MakeOp ("(forinteger)", For, 0, 1, 5, 6);
 	
	TypeInstallOp (Integer, "cvi", 		Identity,	1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "cvr", 		Cvr,		1, 1, 0, 0, Integer);
  	TypeInstallOp (Integer, "cvs", 		Cvs,		2, 1, 0, 0, Integer, String);
	TypeInstallOp (Integer, "==", 		EqEq,		1, 0, 0, 0, Integer);
 	TypeInstallOp (Integer, "for", 		PFor, 		4, 0, 0, 6, Integer, Integer, Integer, Array);
  	TypeInstallOp (Integer, "eq", 		Eq,		2, 1, 0, 0, Integer, Integer);
  	TypeInstallOp (Integer, "lt", 		Lt,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "le", 		Le,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "gt", 		Gt,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "ge", 		Ge,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "not", 		Not,		1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "and", 		And,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "or", 		Or,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "xor", 		Xor,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "bitshift", 	BitShift,	2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "abs", 		Abs,		1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "add", 		Add,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "sub", 		Sub,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "mul", 		Mul,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "div", 		Div,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "mod", 		Mod,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "neg", 		Neg,		1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "exp", 		Exp,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "sqrt", 	Sqrt,		1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "sin", 		Sin,		1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "cos", 		Cos,		1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "atan", 	Atan,		2, 1, 0, 0, Integer, Integer);
 	TypeInstallOp (Integer, "ln", 		Ln,		1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "log", 		Log,		1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "ceiling", 	Identity,	1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "floor", 	Identity,	1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "round", 	Identity,	1, 1, 0, 0, Integer);
 	TypeInstallOp (Integer, "truncate", 	Identity,	1, 1, 0, 0, Integer);
 	
 	word = -1;
 	PanicIf (word != ~0, "Need 2's complement machine! Sorry.");
 	
 	for (WordSize = 0; word != 0; WordSize++)
 		word >>= 1;
 	Word2 = WordSize / 2;
 	LowMask = (1<<Word2) - 1;
}

int StrictMul (a, b) int a, b;
 {
 	int atop, abot, btop, bbot, sum, signed;
 	
 	signed = (a < 0) != (b < 0);
 	a = a < 0 ? -a : a;
 	b = b < 0 ? -b : b;
 	abot = a & LowMask;
 	bbot = b & LowMask;
 	atop = a >> Word2;
 	btop = b >> Word2;
 	sum = abot * btop + atop * bbot;
 	sum += ((unsigned) (abot * bbot) >> Word2);
 	sum = ((unsigned) sum >> Word2) + atop * btop;
 	if (sum != 0 || a * b < 0)
 		kill (getpid (), SIGFPE);
 	return signed ? -a * b : a * b;
 }

int StrictAdd (a, b) int a, b;
 {
	if ((a < 0) == (b < 0) && (a < 0) != (a + b < 0))
		kill (getpid (), SIGFPE);
	return a + b;
 }

int BodyInteger (item) Object item;
 {
 	return item.u.Integer;
 }

static int Body (item) Object item;
 {
 	return item.u.Integer;
 }

Object MakeInteger (i) int i;
 {
 	Object res;
 	
 	res = MakeObject (Integer);
 	res.u.Integer = i;
 	
 	return res;
 }

static Object Make (i) int i;
 {
 	Object res;
 	
 	res = MakeObject (Integer);
 	res.u.Integer = i;
 	
 	return res;
 }

static int Cvr (item) Object item;
 {
	return Push (OpStack, RealInteger (item));
 }

static int Cvs (v, string) Object v, string;
 {
	int length;
	char buf [BUFSIZE];
 	
	VOID sprintf (buf, "%d", BodyInteger (v));
	if ((length = strlen (buf)) > lengthString (string))
		return Error (PRangeCheck);
	VOID Bcopy (BodyString (string), buf, length);
	return Push (OpStack, getIString (string, 0, length));
 }

static int EqEq (v) Object v;
 {
	printf ("%d", BodyInteger (v));
	return TRUE;
 }

Object IntReal (o) Object o;
 {
 	return Make ((int) BodyReal (o));
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
	 	VOID Push (ExecStack, Make (StrictAdd (Body (current), Body (increment))));
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

static int Not (integer) Object integer;
 {
 	 return Push (OpStack, Make (~ Body (integer)));
 }

static int And (a, b) Object a, b;
 {
 	return Push (OpStack, Make (Body (a) & Body (b)));
 }

static int Or (a, b) Object a, b;
 {
 	return Push (OpStack, Make (Body (a) | Body (b)));
 }

static int Xor (a, b) Object a, b;
 {
 	return Push (OpStack, Make (Body (a) ^ Body (b)));
 }

static int BitShift (a, b) Object a, b;
 {
 	if (Body (b) > 0)
 		return Push (OpStack, Make ((int) ((unsigned) Body (a) << Body (b))));
 	else
 		return Push (OpStack, Make ((int) ((unsigned) Body (a) >> (-Body (b)))));
 }

static int Abs (v) Object v;
 {
 	return Push (OpStack, Make (Body (v) >= 0 ? Body (v) : -Body (v)));
 }

static int Add (a, b) Object a, b;
 {
 	return Push (OpStack, Make (StrictAdd (Body (a), Body (b))));
 }

static int Sub (a, b) Object a, b;
 {
 	return Push (OpStack, Make (StrictAdd (Body (a), -Body (b))));
 }

static int Mul (a, b) Object a, b;
 {
 	return Push (OpStack, Make (StrictMul (Body (a), Body (b))));
 }

static int Div (a, b) Object a, b;
 {
 	if (Body (b) == 0)
 		return Error (PUnResult);
 	return Push (OpStack, MakeReal ((float) Body (a) / (float) Body (b)));
 }

static int Mod (a, b) Object a, b;
 {
 	if (Body (b) == 0)
 		return Error (PUnResult);
 	return Push (OpStack, Make (Body (a) % Body (b)));
 }

static int Neg (a) Object a;
 {
	return Push (OpStack, Make (-Body (a)));
 }

static int Sqrt (v) Object v;
 {
  	if (Body (v) < 0)
 		return Error (PUnResult);
	return Push (OpStack, MakeReal ((float) sqrt ((double) Body (v))));
 }

static int Exp (a, b) Object a, b;
 {
	return Push (OpStack, MakeReal ((float) pow ((double) Body (a), (double) Body (b))));
 }

static int Identity (v) Object v;
 {
 	return Push (OpStack, v);
 }

static int Sin (v) Object v;
 {
 	return Push (OpStack, MakeReal ((float) sin (Rad ((double) Body (v)))));
 }

static int Cos (v) Object v;
 {
 	return Push (OpStack, MakeReal ((float) cos (Rad ((double) Body (v)))));
 }

static int Atan (a, b) Object a, b;
 {
 	return Push (OpStack, MakeReal ((float) Deg (atan2 ((double) Body (a), (double) Body (b)))));
 }

static int Ln (v) Object v;
 {
 	return Push (OpStack, MakeReal ((float) log ((double) Body (v))));
 }

static int Log (v) Object v;
 {
 	return Push (OpStack, MakeReal ((float) log10 ((double) Body (v))));
 }
