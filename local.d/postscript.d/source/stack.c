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

#define   OPSTACKSIZE	500
#define EXECSTACKSIZE	250
#define DICTSTACKSIZE	 20

static StackOb	DStack, EStack, OStack;
	
Stack	DictStack = &DStack;
Stack	ExecStack = &EStack;
Stack	OpStack   = &OStack;

static int PPop (), PDup (), PExch (), PRoll (), PIndex (), PClear (), PCount (), PClearToMark (); 
static int PCountToMark (), PExecStack (), PCountDictStack (), PDictStack ();
static int PReverseStack ();

static StackOb MakeStack ();

InitStack ()
 {
	DStack	= MakeStack (DICTSTACKSIZE, PDictOverflow,	PDictUnderflow);
		VOID Push (DictStack, SysDict);
		
	OStack	= MakeStack (  OPSTACKSIZE, POpOverflow,	POpUnderflow);
	EStack	= MakeStack (EXECSTACKSIZE, PExecOverflow,	PExecUnderflow);
	
	Install ("mark", Marker);
	
	InstallOp ("pop",		PPop,		1, 0, 0, 0, Poly);
	InstallOp ("dup",		PDup,		1, 2, 0, 0, Poly);
	InstallOp ("exch",		PExch,		2, 2, 0, 0, Poly, Poly);
	InstallOp ("roll",		PRoll,		2, 0, 0, 0, Integer, Integer);
	InstallOp ("index",		PIndex,		1, 1, 0, 0, Integer);
	InstallOp ("clear",		PClear,		0, 0, 0, 0);
	InstallOp ("count",		PCount,		0, 1, 0, 0);
	InstallOp ("cleartomark",	PClearToMark,	0, 0, 0, 0);
	InstallOp ("counttomark",	PCountToMark,	0, 1, 0, 0);
	InstallOp ("execstack",		PExecStack,	1, 1, 0, 0, Array);
	InstallOp ("countdictstack",	PCountDictStack,0, 1, 0, 0);
	InstallOp ("dictstack",		PDictStack,	1, 1, 0, 0, Array);
	InstallOp ("reversestack",	PReverseStack,	1, 0, 0, 0, Integer);
 }

static StackOb MakeStack (size, over, under) int size; Object over, under;
 {
	StackOb res;
	
	res.stack_body = (Object *) Malloc ((unsigned) sizeof (Object) * size);
	res.stack_fill = 0; res.stack_size = size;
	res.overflow = over; res.underflow = under;
	
	return res;
 }

#ifdef notdef
int Push (stack, object) Stack stack; Object object;
 {
	if (stack->stack_fill != stack->stack_size)
		stack->stack_body[stack->stack_fill++] = object;
	else
		return FALSE;
	return TRUE;
 }

Object Pop (stack) Stack stack;
 {
	if (stack->stack_fill)
	 	return stack->stack_body[--stack->stack_fill];
	printf ("%s", stack == OpStack ? "OpStack" : stack == ExecStack ? "ExecStack" : "OtherStack");
	Panic (": Pop empty stack");
	return Nil; /* for lint */
 }

Object Top (stack) Stack stack;
 {
	if (stack->stack_fill)
	 	return stack->stack_body[stack->stack_fill - 1];
	printf ("%s", stack == OpStack ? "OpStack" : stack == ExecStack ? "ExecStack" : "OtherStack");
	Panic (": Top empty stack");
	return Nil; /* for lint */
 }

int Height (s) Stack s;
 {
 	return s->stack_fill;
 }

int MaxStack (s) Stack s;
 {
 	return s->stack_size;
 }
#endif

Object DictLookup (o) Object o;
 {
 	int i;
 	
 	for (i = DictStack->stack_fill - 1; i >= 0; i--)
 	 {
 	 	Object item;
 	 	
 	 	item = DictLoad (DictStack->stack_body[i], o);
 		if (TypeOf (item) != Condition)
 			return item;
 	 }
 	return Absent;
 }

Object Where (key) Object key;
 {
 	int i;
 	
 	for (i = DictStack->stack_fill - 1; i >= 0; i--)
 	 {
 	 	Object t;
 	 	
 	 	t = DictLoad (DictStack->stack_body[i], key);
  		if (TypeOf (t) != Condition)
 			return DictStack->stack_body[i];
 	 }
 	return Absent;
 }

static reverse (vec, n) Object *vec; int n;
 {
 	Object temp;
 	int i, lim = n / 2;
 	
 	for (i = 0; i < lim; i++)
 		temp = vec[i],
 		vec[i] = vec[n-1-i],
 		vec[n-1-i] = temp;
 }

int CountTo (t, s) Type t; Stack s;
 {
 	int i;
 	
 	Object *body = s->stack_body;
 	
 	for (i = Height (s) - 1; i >= 0; i--)
 		if (TypeOf (body[i]) == t)
 			return Height (s) - i - 1;
 	return -1;
 }

int ClearTo (t, s) Type t; Stack s;
 {
 	int count;
 	
 	if ((count = CountTo (t, s)) >= 0)
		s->stack_fill = s->stack_fill - (count + 1);
	else
		return FALSE;
	return TRUE;
 }

static int PCountDictStack ()
 {
 	return Push (OpStack, MakeInteger (DictStack->stack_fill));
 }

static int PDictStack (array) Object array;
 {
	Object *body = DictStack->stack_body;
	int i, l = Height (DictStack);
	
 	if (!wCheck (array))
 		return Error (PInvAccess);
 	if (l > lengthArray (array))
 		return Error (PRangeCheck);
	for (i = 0; i < l; i++)
 		putArray (array, i, body[i]);
 	return Push (OpStack, getIArray (array, 0, l));
 }

/*ARGSUSED*/
static int PPop (item) Object item;
 {
 	return TRUE;
 }

static int PDup (item) Object item;
 {
 	return Push (OpStack, item), Push (OpStack, item);
 }

static int PExch (a, b) Object a, b;
 {
 	return Push (OpStack, b), Push (OpStack, a);
 }

static int PRoll (size, count) Object size, count;
 {
	int s = BodyInteger (size), c = BodyInteger (count);
	Object *body = OpStack->stack_body + Height (OpStack) - s;
	
 	if (BodyInteger (size) < 0)
 		return Error (PRangeCheck);
 	if (BodyInteger (size) > Height (OpStack))
 		return Error (POpUnderflow);
 	if (BodyInteger (size) == 0)
 		return TRUE;
	if (c > 0)
	 {
	 	c = c % s;
		reverse (body, s - c);
		reverse (body + s - c, c);
	 }
	else
	 {
	 	c = (-c) % s;
		reverse (body, c);
		reverse (body + c, s - c);
	 }
	reverse (body, s);
	return TRUE;
 }

static int PIndex (index) Object index;
 {
 	if (BodyInteger (index) >= Height (OpStack) || BodyInteger (index) < 0)
 		return Error (PRangeCheck);
 	return Push (OpStack, OpStack->stack_body[Height (OpStack) - 1 - BodyInteger (index)]);
 }

static int PClear ()
 {
 	OpStack->stack_fill = 0;
 	return TRUE;
 }

static int PCount ()
 {
	return Push (OpStack, MakeInteger (Height (OpStack)));
 }

static int PClearToMark ()
 {
 	if (ClearTo (Mark, OpStack))
 		return TRUE;
 	else
 		return Error (PUnMatched);
 }

static int PCountToMark ()
 {
 	int count;
 	
 	if ((count = CountTo (Mark, OpStack)) >= 0)
 		return Push (OpStack, MakeInteger (count));
 	else
 		return Error (PUnMatched);
 }

static int PExecStack (array) Object array;
 {
	int i, l = Height (ExecStack);
 	Object *body = ExecStack->stack_body;
	
	if(!wCheck (array))
		return Error (PInvAccess);
	if (l > lengthArray (array))
		return Error (PRangeCheck);
	for (i = 0; i < l; i++)
		putArray (array, i, body[i]);
	VOID Push (OpStack, getIArray (array, 0, l));
	return TRUE;
 }

ReverseStack (stack, n) Stack stack; int n;
 {
 	Object *body = stack->stack_body;
 	
 	reverse (&body[Height (stack) - n], n);
 }

static int PReverseStack (n) Object n;
 {
 	if (BodyInteger (n) > Height (OpStack))
 		return Error (PRangeCheck);
 	ReverseStack (OpStack, BodyInteger (n));
 	return TRUE;
 }
