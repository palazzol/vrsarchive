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

Object True, False;
static Object OpNot;
static int Cvs (), And (), Or (), Not (), Xor ();
static int PEq (), PNeq (), EqBoolean ();

InitBoolean ()
 {
	True = 	MakeObject (Boolean);
	False = MakeObject (Boolean);
	
 	True.u.Boolean = TRUE;
	False.u.Boolean = FALSE;
	
	Install ("true",	True);
	Install ("false",	False);
  	
 	TypeInstallOp (Boolean, "cvs", 	Cvs,  		2, 1, 0, 0, Boolean, String);
 	TypeInstallOp (Boolean, "and", 	And,  		2, 1, 0, 0, Boolean, Boolean);
 	TypeInstallOp (Boolean, "or", 	Or,  		2, 1, 0, 0, Boolean, Boolean);
 	TypeInstallOp (Boolean, "xor", 	Xor,  		2, 1, 0, 0, Boolean, Boolean);
 	TypeInstallOp (Boolean, "not", 	Not,  		1, 1, 0, 0, Boolean);
 	TypeInstallOp (Boolean, "eq", 	EqBoolean,  	2, 1, 0, 0, Boolean, Boolean);
 	
	OpNot = Lookup (Boolean, NameFrom ("not"));
	
 	InstallOp ("eq",	PEq,  		2, 1, 0, 0, Poly, Poly);
 	InstallOp ("ne",	PNeq, 		2, 1, 0, 0, Poly, Poly);
 	InstallOp ("ge", 	PolyPair, 	2, 2, 0, 0, Poly, Poly);
 	InstallOp ("gt", 	PolyPair, 	2, 2, 0, 0, Poly, Poly);
 	InstallOp ("le", 	PolyPair, 	2, 2, 0, 0, Poly, Poly);
 	InstallOp ("lt", 	PolyPair, 	2, 2, 0, 0, Poly, Poly);
 	InstallOp ("not", 	PolyFirst, 	1, 1, 0, 0, Poly);
 	InstallOp ("and", 	PolyPair, 	2, 2, 0, 0, Poly, Poly);
 	InstallOp ("or", 	PolyPair, 	2, 2, 0, 0, Poly, Poly);
 	InstallOp ("xor", 	PolyPair, 	2, 2, 0, 0, Poly, Poly);
 	InstallOp ("bitshift", 	PolyPair, 	2, 2, 0, 0, Poly, Poly);
 }

Object MakeBoolean (b) int b;
 {
	return b ? True : False;
 }

int BodyBoolean (b) Object b;
 {
 	return b.u.Boolean;
 }

static int Cvs (v, string) Object v, string;
 {
 	char *choice = BodyBoolean (v) ? "true" : "false";
 	int length = strlen (choice);
 	
	if (lengthString (string) < length)
		return Error (PRangeCheck);
	VOID Bcopy (BodyString (string), choice, length);
	return Push (OpStack, getIString (string, 0, length));
 }

static int PEq (a, b) Object a, b;	/* any any --- boolean */
 {
	if (!rCheck (a) || !rCheck (b))
	 	return Error (PInvAccess);
	if (TypeOf (a) == Name && TypeOf (b) == String)
 		a = StringName (a);
 	else if (TypeOf (b) == Name && TypeOf (a) == String)
 		b = StringName (b);
	else if (TypeOf (a) == Real && TypeOf (b) == Integer)
			b = RealInteger (b);
	else if (TypeOf (a) == Integer && TypeOf (b) == Real)
			a = RealInteger (a);
 	if (TypeOf (a) == TypeOf (b))
 	 {
 	 	VOID Push (OpStack, a);
 	 	VOID Push (OpStack, b);
  		return Apply (TypeOf (a));
  	 }
 	else
 		return Push (OpStack, False);
 }

static int PNeq (a, b) Object a, b;		/* any any --- boolean */
 {
 	if (!rCheck (a) || !rCheck (b))
 	 	return Error (PInvAccess);
	if (TypeOf (a) == Name && TypeOf (b) == String)
 		a = StringName (a);
 	else if (TypeOf (b) == Name && TypeOf (a) == String)
 		b = StringName (b);
	else if (TypeOf (a) == Real && TypeOf (b) == Integer)
			b = RealInteger (b);
	else if (TypeOf (a) == Integer && TypeOf (b) == Real)
			a = RealInteger (a);
 	if (TypeOf (a) == TypeOf (b))
  	 {
 	 	VOID Push (OpStack, a);
 	 	VOID Push (OpStack, b);
 	 	VOID Push (ExecStack, OpNot);
 	 	Self = NameFrom ("eq");
  		return Apply (TypeOf (a));
  	 }
 	else
 		return Push (OpStack, True);
 }

static int EqBoolean (a, b) Object a, b;
 {
  	return Push (OpStack, MakeBoolean (BodyBoolean (a) == BodyBoolean (b)));
 }

static int Not (bool) Object bool;
 {
 	return Push (OpStack, MakeBoolean (!BodyBoolean (bool)));
 }

static int And (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (BodyBoolean (a) && BodyBoolean (b)));
 }

static int Or (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (BodyBoolean (a) || BodyBoolean (b)));
 }

static int Xor (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (BodyBoolean (a) != BodyBoolean (b)));
 }
