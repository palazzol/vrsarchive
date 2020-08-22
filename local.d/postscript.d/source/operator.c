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

struct op_struct
 {
 	Object name;
 	int (*fn)(), arguments, results, execpop, execpush;
 	Type *argtypes;
 };

int ExecOperator ();
static int EqEq (), Cvs (), Eq (), Signature ();

InitOperator ()
 {
 	
	TypeInstallOp (Operator, "==", 	EqEq,			1, 0, 0, 0, Operator);
  	TypeInstallOp (Operator, "cvs", Cvs,			2, 1, 0, 0, Operator, String);
  	TypeInstallOp (Operator, "exec",ExecOperator,		1, 0, 0, 0, Operator);
  	TypeInstallOp (Operator, "eq",	Eq,			2, 1, 0, 0, Operator, Operator);
  	TypeInstallOp (Operator, "signature",	Signature,	1, 0, 0, 0, Operator);
 }

/*VARARGS6*/
/*ARGSUSED*/
Object MakeOp (name, fn, arguments, results, execpop, execpush, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
		char *name;
		int (*fn)(), arguments, results, execpop, execpush;
		Type arg1, arg2, arg3, arg4, arg5, arg6, arg7;
 {
 	Object res;
 	struct op_struct *op;
 	int i;
 	
 	res = Cvx (MakeObject (Operator));
 	res.u.Operator = op = (struct op_struct *) Malloc (sizeof (struct op_struct));
	
	op->name	= NameFrom (name);
	op->fn		= fn;
 	op->arguments	= arguments;
 	op->results	= results;
 	op->execpop	= execpop;
 	op->execpush	= execpush;
 	
 	op->argtypes = (Type *) Malloc ((unsigned) sizeof (Type) * arguments);
 	
 	for (i = 0; i < arguments; i++)
 		op->argtypes[i] = (&arg1)[i];
 	
 	return res;
 }

/*VARARGS6*/
/*ARGSUSED*/
InstallOp (name, fn, arguments, results, execpop, execpush, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
		char *name;
		int (*fn)(), arguments, results, execpop, execpush;
		Type arg1, arg2, arg3, arg4, arg5, arg6, arg7;
 {
 	Object res;
 	struct op_struct *op;
 	int i;
 	res = Cvx (MakeObject (Operator));
 	res.u.Operator = op = (struct op_struct *) Malloc (sizeof (struct op_struct));
	
	op->name	= NameFrom (name);
	op->fn		= fn;
 	op->arguments	= arguments;
 	op->results	= results;
 	op->execpop	= execpop;
 	op->execpush	= execpush;
 	
 	op->argtypes = (Type *) Malloc ((unsigned) sizeof (Type) * arguments);
	
/* 	for (i = 0; i < arguments; i++)
 		op->argtypes[i] = (&arg1)[i];
 */	switch (arguments)
 	 {
 	 	default:
 	 		Panic ("too many arguments in InstallOp");
 	 		break;
 	 	case 7: op->argtypes[6] = arg7;
 		case 6: op->argtypes[5] = arg6;
 		case 5: op->argtypes[4] = arg5;
 		case 4: op->argtypes[3] = arg4;
 		case 3: op->argtypes[2] = arg3;
 		case 2: op->argtypes[1] = arg2;
 		case 1: op->argtypes[0] = arg1;
 		case 0: break;
 	 }
	DictStore (SysDict, NameOperator (res), res);
 }

/*VARARGS7*/
/*ARGSUSED*/
TypeInstallOp (type, name, fn, arguments, results, execpop, execpush, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
		char *name;
		int (*fn)(), arguments, results, execpop, execpush;
		Type type, arg1, arg2, arg3, arg4, arg5, arg6, arg7;
 {
 	Object res;
 	struct op_struct *op;
 	int i;
 	
 	res = Cvx (MakeObject (Operator));
 	res.u.Operator = op = (struct op_struct *) Malloc (sizeof (struct op_struct));
	
	op->name	= NameFrom (name);
	op->fn		= fn;
 	op->arguments	= arguments;
 	op->results	= results;
 	op->execpop	= execpop;
 	op->execpush	= execpush;
 	
 	op->argtypes = (Type *) Malloc ((unsigned) sizeof (Type) * arguments);
 	
 	for (i = 0; i < arguments; i++)
 		op->argtypes[i] = (&arg1)[i];
 	
 	TypeInstall (type, name, res);
 }

static struct op_struct *Body (item) Object item;
 {
 	return item.u.Operator;
 }

#define Body(op)	((op).u.Operator)

static int Eq (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (Body (a) == Body (b)));
 }

Object NameOperator (item) Object item;
 {
 	return Body (item)->name;
 }

#define NameOperator(op)	((op).u.Operator->name)

static int Cvs (v, string) Object v, string;
 {
 	Object vv;
 	int length;
 	
 	vv = NameOperator (v);
 	
	if (lengthString (string) < (length = lengthName (vv)))
		return Error (PRangeCheck);
	VOID strncpy (BodyString (string), BodyName (vv), length);
	return Push (OpStack, getIString (string, 0, length));
 }

static int EqEq (v) Object v;
 {
 	VOID Push (OpStack, NameOperator (v));
 	VOID Push (ExecStack, Cvx (NameFrom ("==")));
	return TRUE;
 }

/*
 * Operator types.
 *
 *	PostScript has 200 or so operators which can be divided into groups by the
 *	arguments required.
 *
 *	The type Real 		causes an Integer argument to get floated.
 *	The type Unchecked 	causes an argument to be passed through whatever type it has.
 *	The type Number		causes all Number arguments to be Integer or Real
 *
 *	An operator object comprises several pieces of information:
 *
 *	1	Number of arguments required
 *	2	Number of results which may be returned
 *	3	Number of items required on the ExecStack
 *	4	Number of items added to the ExecStack
 *	5	A List of argument types
 *
 *	Functions requiring strange arguments can declare themselves to take few
 *	arguments and then use some directly from the OpStack.
 *
 *	The first four arguments are only required for checking Stack over and underflow,
 *	thus operators are not required to hold to their requests.
 *
 *	After resolving any peculiar type rules, Exec will lookup the operator name in the type
 *	dictionary for the argument type which is specified as being the controlling argument.
 *	The operator will receive the correct number of arguments and will be expected to stack
 *	its own results. There will be a fixed maximum limit of the number of arguments an operator
 *	can declare for itself. This is likely to be the maximum that PostScript uses, i.e. 6 or 7.
 *
 */

int ExecOperator (item) Object item;
 {
	struct op_struct *op = Body (item);
	int i, res, (*fn)() = op->fn;
	Object arg[7];
	
	Self = NameOperator (item);
	     if (op->results	> MaxStack (OpStack)   - Height (OpStack))	return Error (POpOverflow);
	else if (op->execpush	> MaxStack (ExecStack) - Height (ExecStack))	return Error (PExecOverflow);
	else if (op->arguments  > Height   (OpStack))				return Error (POpUnderflow);
	else if (op->execpop	> Height   (ExecStack))				return Error (PExecUnderflow);
	
	for (i = op->arguments - 1; i >= 0; i--)
		arg[i] = Pop (OpStack);
	for (i = op->arguments - 1; i >= 0; i--)
	 {
	 	Type formal = op->argtypes[i], actual = TypeOf (arg[i]);
	 	
	 	if (formal == Float && actual == Integer)
 	 		arg[i] = RealInteger (arg[i]);
		if (formal == actual ||
				formal == Poly ||
				formal == Float &&
					(actual == Integer || actual == Real))
	 	 	continue;
	 	for (i = 0; i < op->arguments; i++)
			VOID Push (OpStack, arg[i]);
		return Error (PTypeCheck);
	 }
	
	/*if (setjmp (env))
	 	res = Error (PUnResult);
	else */
		switch (op->arguments)
		 {
		 	case 0: res = (*fn) (); break;
		 	case 1: res = (*fn) (arg[0]); break;
		 	case 2: res = (*fn) (arg[0], arg[1]); break;
		 	case 3: res = (*fn) (arg[0], arg[1], arg[2]); break;
		 	case 4: res = (*fn) (arg[0], arg[1], arg[2], arg[3]); break;
		 	case 5: res = (*fn) (arg[0], arg[1], arg[2], arg[3], arg[4]); break;
		 	case 6: res = (*fn) (arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]); break;
		 	case 7: res = (*fn) (arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]); break;
		 	
		 	default: Panic ("primitve with too many arguments");
		 }
	 	/*	res = (*fn) (arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]);	*/
	
	if (!res)
		for (i = 0; i < op->arguments; i++)
			VOID Push (OpStack, arg[i]);
	return res;
 }

static int Signature (op) Object op;
 {
	Object tn;
 	struct op_struct *ops = Body (op);
 	int i;
 	
 	tn = NameFrom ("type");
 	if (!OpCheck (0, ops->arguments + 1))
 		return FALSE;
 	for (i = 0; i < ops->arguments; i++)
 		VOID Push (OpStack, Lookup (ops->argtypes[i], tn));
 	VOID Push (OpStack, MakeInteger (ops->arguments));
 	VOID Push (OpStack, MakeInteger (ops->results));
 	
 	return TRUE;
 }
