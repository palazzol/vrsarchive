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

static int Copy ();

int PolyFirst (), PolySecond (), PolyThird (), PolyPair ();

InitPoly ()
 {
 	Lbracket = Cvx (NameFrom("["));
	Rbracket = Cvx (NameFrom("]"));
	
	InstallOp ("token",		PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("copy",		Copy, 		0, 0, 0, 0);
	InstallOp ("length",		PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("forall",		PolySecond,	2, 2, 0, 0, Poly, Poly);
	InstallOp ("get",		PolySecond,	2, 2, 0, 0, Poly, Poly);
	InstallOp ("put",		PolyThird,	3, 3, 0, 0, Poly, Poly, Poly);
	InstallOp ("getinterval",	PolyThird,	3, 3, 0, 0, Poly, Poly, Poly);
	InstallOp ("putinterval",	PolyThird,	3, 3, 0, 0, Poly, Poly, Poly);
	InstallOp ("signature",		PolyFirst,	1, 0, 0, 0, Poly);
 }

Object Parse (f) Object f;
 {
 	Object res;
 	int c;
 	
	for (;;)
	 switch (c = Getch (f))
	  {
	 	default:  Ungetch (f, c);
	 		res = ParseId (f);
	 		if (TypeOf (res) == Integer || TypeOf (res) == Real)
	 			return res;
	 		else
	 			return Cvx (res);
	 	
		case '/': return ParseId (f);
		case '{': return Cvx (ParseArray (f));
		case '}': return True;
		case '[': return Lbracket;
		case ']': return Rbracket;
		case '<': return ParseHexString (f);
		case '(': return ParseString (f);
		case ')': return Absent;
		
	 	case EOF: return False;
	 	
	 	case ' ':
	 	case '\t':
 		case '\n':
	 		/* nothing */
	 	    continue;
	 	
 		case '%':
 			while ((c = Getch (f)) != '\n' && c != EOF)
 				;
 			Ungetch (f, c);
 		    continue;
	  }
 }

/*
 * The following are a few of the polymorphic generic routines which actually get called
 * by user PostScript.
 * They call type-checked routines on behalf of the types of their arguments.
 *
 */

int PolyFirst (arg1) Object arg1;	/* type dictionary choice determined by top of stack */
 {
  	if (!Apply (TypeOf (arg1)))
 		return Error (PTypeCheck);
	return Push (OpStack, arg1);
 }

int PolySecond (arg1, arg2) Object arg1, arg2;	/* type dictionary choice determined by first from top of stack */
 {
 	if (!Apply (TypeOf (arg1)))
 		return Error (PTypeCheck);
 	return Push (OpStack, arg1), Push (OpStack, arg2);
 }

int PolyThird (arg1, arg2, arg3) Object arg1, arg2, arg3;	/* type dictionary choice determined by second from top of stack */
 {
 	if (!Apply (TypeOf (arg1)))
 		return Error (PTypeCheck);
 	return Push (OpStack, arg1), Push (OpStack, arg2), Push (OpStack, arg3);
 }

int PolyPair (arg1, arg2) Object arg1, arg2;
 {
	if (TypeOf (arg1) == Integer && TypeOf (arg2) == Real)
 		arg1 = RealInteger (arg1);
 	else if (TypeOf (arg2) == Integer && TypeOf (arg1) == Real)
 		arg2 = RealInteger (arg2);
 	if (TypeOf (arg1) != TypeOf (arg2))
		return Error (PTypeCheck);
 	if (!Apply (TypeOf (arg1)))
 		return Error (PTypeCheck);
	return Push (OpStack, arg1), Push (OpStack, arg2);
 }

int Apply (type) Type type;
 {
 	Object fn;
 	
 	fn = Lookup (type, Self);
 	if (TypeOf (fn) == Condition)
 		return Error (PTypeCheck);
 	else
 		return Push (ExecStack, fn);
 }

static int Copy ()	/* any1 . . . anyn N	--- any1 . . . anyn any1 . . . anyn */
		/* other1  other2	--- subother2 */
 {
	Object object1, object2;
	int h = Height (OpStack);
	
	object2 = Pop (OpStack);
	if (h == 0)
		return Error (POpUnderflow);
	else if (TypeOf (object2) == Integer)
	 {
	  	int n = BodyInteger (object2);
	  	
	 	if (n < 0 || n >= h)
			return Error (PRangeCheck);
	 	else if (h - 1 + n > MaxStack (OpStack))
			return Error (POpOverflow);
	 	else
	 	 {
	 	 	int i;
	 	 	
	 	 	for (i = h - 1 - n; i < h; i++)
	 	 		OpStack->stack_body[i+n] = OpStack->stack_body[i];
	 	 	OpStack->stack_fill += n;
	 	 	return TRUE;
	 	 }
 	 }
 	object1 = Top (OpStack);
 	VOID Push (OpStack, object2);
 	if (TypeOf (object1) != TypeOf (object2))
		return Error (PTypeCheck);
	else if (!rCheck (object1) || !wCheck (object2))
		return Error (PInvAccess);
	else
 		return Apply (TypeOf (object1));
 }
