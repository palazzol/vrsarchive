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

static int PCvs (), PCvrs (), PCvn (), PType (), PCvlit(), PxCheck ();
int PCvx ();

Object type;

InitProperty ()
 {
 	type = NameFrom ("type");
 	
 	InstallOp ("type",		PType,		1, 1, 0, 0, Poly);
 	InstallOp ("cvlit",		PCvlit,		1, 1, 0, 0, Poly);
 	InstallOp ("cvn",		PCvn,		1, 1, 0, 0, String);
 	InstallOp ("cvrs",		PCvrs,		3, 1, 0, 0, Integer, Integer, String);
 	InstallOp ("cvs",		PCvs,		2, 1, 0, 0, Poly, String);
 	InstallOp ("cvx",		PCvx,		1, 1, 0, 0, Poly);
 	InstallOp ("xcheck",		PxCheck,	1, 1, 0, 0, Poly);
	InstallOp ("cvi",		PolyFirst, 	1, 1, 0, 0, Poly);
	InstallOp ("cvr",		PolyFirst, 	1, 1, 0, 0, Poly);
	InstallOp ("readonly",		PolyFirst, 	1, 1, 0, 0, Poly);
	InstallOp ("rcheck",		PolyFirst, 	1, 1, 0, 0, Poly);
	InstallOp ("wcheck",		PolyFirst, 	1, 1, 0, 0, Poly);
	InstallOp ("executeonly",	PolyFirst, 	1, 1, 0, 0, Poly);
 }

static int PType (item) Object item;
 {
 	return Push (OpStack, Lookup (TypeOf (item), type));
 }

static int PCvs (item, string) Object item, string;
 {
 	int l;
 	Object t;
 	
 	t = Lookup (TypeOf (item), NameFrom ("cvs"));
	if (TypeOf (t) == Condition)
		if (lengthString (string) < (l = strlen ("--nostringval--")))
			return Error (PRangeCheck);
		else
		 {
		 	putIString (string, 0, StringFrom ("--nostringval--"));
		 	return Push (OpStack, getIString (string, 0, l));
		 }
	else
	 {
		VOID Push (OpStack, item);
		VOID Push (OpStack, string);
 		return Apply (TypeOf (item));
 	 }
 }

static int PCvlit (item) Object item;
 {
 	return Push (OpStack, Cvlit (item));
 }

static int PCvn (string) Object string;
 {
 	return Push (OpStack, SameFlags (string, Cvn (string)));
 }

static int PCvrs (num, base, string) Object num, base, string;
 {
 	unsigned n = BodyInteger (num);
	unsigned char buf [BUFSIZE], *p = buf, *q = BodyString (string);
	int b, length;
 	
 	if (!wCheck (string))
 		return Error (PInvAccess);
 	else if ((b = BodyInteger (base)) < 2 || b > 36)
 		return Error (PRangeCheck);
 	do {
 		int dig_val = n % b;
 		
 		n /= b;
 		*p++ = dig_val >= 10 ? 'A' + dig_val - 10 : '0' + dig_val;
 	} while (n != 0);
 	
 	if ((length = p - buf) > lengthString (string))
 		return Error (PRangeCheck);
 	
 	while (--p >= buf)
 		*q++ = *p;
 	return Push (OpStack, getIString (string, 0, length));
 }

/*ARGSUSED*/
int NoStringVal (v, string) Object v, string;
 {
 	char *mess = "--nostringval--";
 	int length = strlen (mess);
 	
	if (lengthString (string) < length)
		return Error (PRangeCheck);
	VOID Bcopy (BodyString (string), mess, length);
	return Push (OpStack, getIString (string, 0, length));
 }

int PCvx (item) Object item;
 {
 	return Push (OpStack, Cvx (item));
 }

static int PxCheck (item) Object item;
 {
 	return Push (OpStack, MakeBoolean (xCheck (item)));
 }
