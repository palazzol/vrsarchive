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

/*
 * This file implements the flow control operators of PostScript
 *
 * All operations are implemented using the ExecStack and without recursion.
 * This is to allow error recovery to operate cleanly.
 *
 * Actual control is acheived by stacking continuation operators for the looping
 * operators, declared thus:
 *
 */

static Object OpLoop, OpRepeat, OpStopped;

/*
 * "loop", for example, stacks the following:
 *	proc
 *	OpLoop
 *	proc
 *	Nil
 *
 * OpLoop is the continuer, proc is the body code, passed as a parameter.
 *
 * Nil is placed at the bottom by each of the looping constructs, which are 'exit'able.
 * "exit" pops the exec stack until it encounters a Nil.
 *
 * A Marker is placed on the ExecStack to enclose a Stopped context.
 * A File is placed on the ExecStack to enclose a Run context.
 * A Nil is placed on the ExecStack to enclose a Looping construct.
 * "stop" will unwind the stack down to the Marker, ignoring any Nil or File it might find.
 * "exit" will baulk if it finds a Marker or File before a Nil.
 *
 */

static int PExec (), PIf (), PIfElse (), PRepeat (), PFor (), PLoop ();
static int PExit (), PCountExecStack ();
static int PQuit (), PStart ();	
int PStop (), PStopped ();
static int Repeat (), Loop (), Stopped ();

InitControl ()
 {
 	OpRepeat 	= MakeOp ("(repeat)",	Repeat,	0, 0, 3, 2);
 	OpLoop	 	= MakeOp ("(loop)",	Loop,	0, 0, 1, 2);
 	OpStopped	= MakeOp ("(stopped)",	Stopped,0, 1, 1, 0);
 	
	InstallOp ("exec",	PExec,		1, 1, 0, 1, Poly);
  	InstallOp ("if",	PIf,		2, 0, 0, 1, Boolean, Array);
 	InstallOp ("ifelse",	PIfElse,	3, 0, 0, 1, Boolean, Array, Array);
 	InstallOp ("repeat",	PRepeat,	2, 0, 0, 4, Integer, Array);
	InstallOp ("for",	PFor,		4, 0, 0, 6, Poly, Poly, Poly, Array);
 	InstallOp ("loop",	PLoop,		1, 0, 0, 4, Array);
	InstallOp ("exit",	PExit,		0, 0, 0, 0);
 	InstallOp ("stop",	PStop,		0, 1, 0, 0);
 	InstallOp ("stopped",	PStopped,	1, 1, 0, 3, Array);
 	InstallOp ("quit",	PQuit, 		0, 0, 0, 0);
 	InstallOp ("start",	PStart, 	0, 0, 0, 0);
 	InstallOp ("countexecstack",
 				PCountExecStack,0, 1, 0, 0);
 }

static int PExec (item) Object item;
 {
	VOID Push (OpStack, item);
	if (xCheck (item))
	 {
 		Object fn;
 		
		fn = Lookup (TypeOf (item), Self);
		if (TypeOf (fn) != Condition)
			VOID Push (ExecStack, fn);
	 }
	return TRUE;
 }

static int PIf (bool, proc) Object bool, proc;
 {
	if (!xCheck (proc))
		return Error (PTypeCheck);
	if (BodyBoolean (bool))
		VOID Push (ExecStack, proc);
	return TRUE;
 }

static int PIfElse (bool, proc1, proc2) Object bool, proc1, proc2;
 {
	if (!xCheck (proc1) || !xCheck (proc2))
	 	return Error (PTypeCheck);
	VOID Push (ExecStack, BodyBoolean (bool) ? proc1 : proc2);
	return TRUE;
 }

static int PRepeat (count, proc) Object count, proc;
 {
	if (!xCheck (proc))
		return Error (PTypeCheck);
	else if (BodyInteger (count) < 0)
		return Error (PRangeCheck);
	
	VOID Push (ExecStack, Nil);
	VOID Push (ExecStack, proc);
	VOID Push (ExecStack, count);
	VOID Push (ExecStack, OpRepeat);
	
	return TRUE;
 }

static int Repeat ()
 {
 	int count;

	count = BodyInteger (Pop (ExecStack));
 	
 	if (count != 0)
 	 {
 	 	Object proc;
 	 	
 	 	proc = Top (ExecStack);
 	 	VOID Push (ExecStack, MakeInteger (count - 1));
 	 	VOID Push (ExecStack, OpRepeat);
 	 	VOID Push (ExecStack, proc);
 	 }
 	else
 	 {
 		VOID Pop (ExecStack);
 		VOID Pop (ExecStack);
 	 }
	return TRUE;
 }

static int PFor (initial, increment, limit, proc) Object initial, increment, limit, proc;
 {
 	int integers = 0, reals = 0, res;
 	
 	if (!xCheck (proc))
 		return Error (PTypeCheck);
	
 	if (TypeOf (initial) == Integer || TypeOf (initial) == Real)
 		integers += TypeOf (initial) == Integer,
 		reals += TypeOf (initial) == Real;
 	
 	if (TypeOf (increment) == Integer || TypeOf (increment) == Real)
 		integers += TypeOf (increment) == Integer,
 		reals += TypeOf (increment) == Real;
 	
 	if (TypeOf (limit) == Integer || TypeOf (limit) == Real)
 		integers += TypeOf (limit) == Integer,
 		reals += TypeOf (limit) == Real;
 		
 	if (integers + reals == 3 && integers != 3 && reals != 3)
 	 {
 	 	if (TypeOf (initial) == Integer)
 	 		initial = RealInteger (initial);
 	 	if (TypeOf (increment) == Integer)
 	 		increment = RealInteger (increment);
 	 	if (TypeOf (limit) == Integer)
 	 		limit = RealInteger (limit);
 	 }
 	if (res = Apply (TypeOf (initial)))
 	 {
  		VOID Push (OpStack, initial);
 	 	VOID Push (OpStack, increment);
 		VOID Push (OpStack, limit);
		VOID Push (OpStack, proc);
	 }
 	return res;
 }
 
static int PLoop (proc) Object proc;
 {
 	if (!xCheck (proc))
 		return Error (PTypeCheck);
	
 	VOID Push (ExecStack, Nil);
	VOID Push (ExecStack, proc);
	VOID Push (ExecStack, OpLoop);
	VOID Push (ExecStack, proc);
	
	return TRUE;
 }

static int Loop ()	 /* proc --- */
 {
 	Object proc;
 	
 	proc = Top (ExecStack);
	VOID Push (ExecStack, OpLoop);
	VOID Push (ExecStack, proc);
	return TRUE;
 }

static int PExit ()
 {
 	int n, m, p;
 	
 	if ((n = CountTo (Null, ExecStack)) < 0
 			|| (m = CountTo (Mark, ExecStack)) >= 0 && m < n	/* don't cross Stopped Context */
 			|| (p = CountTo (File, ExecStack)) >= 0 && p < n)	/* don't cross Run context */
 		return Error (PInvExit);
 	VOID ClearTo (Null, ExecStack);
 	return TRUE;
 }

int PStop ()	/* --- */
 {
 	Object item;
 	
 	for (;;)
 	 {
 		if (Height (ExecStack) == 0)
 		 {
 		 	if (interactive)
 				return Push (ExecStack, Cvx (NameFrom ("executive")));
 			else
 				return TRUE;
 		 }
 		item = Pop (ExecStack);
 		if (item.type == Mark)
 		 	return Push (OpStack, True);
 	 }
 }

static int Stopped ()
 {
	VOID Pop (ExecStack);
	VOID Push (OpStack, False);
	return TRUE;
 }

int PStopped (proc) Object proc;
 {
	VOID Push (ExecStack, Marker);
	VOID Push (ExecStack, OpStopped);
	VOID Push (ExecStack, proc);
	return TRUE;
 }

static int PCountExecStack ()
 {
	VOID Push (OpStack, MakeInteger (Height (ExecStack)));
	return TRUE;
 }

/*
 * PExecStack ()
 *
 * This routine is in stack.c
 *
 */

static int PQuit ()
 {
 	Cleanup ();
	exit (0);
	
	return TRUE; /* never will */
 }

static int PStart ()
 {
 	return TRUE;
 }
