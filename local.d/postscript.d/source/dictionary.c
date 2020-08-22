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

extern int SizeUserDict, SizeSysDict;

static Object OpForDict;

Object Absent, Nil, SysDict;

Object DictLookup (), MakeDict (), DictLoad ();

static int LengthDict (), CopyDict (), forDict (), ForDict (), PutDict (), GetDict ();
static int PDict (), PBegin (), PEnd (), PDef (), PStore (), PKnown (), PLoad ();
static int PrCheck (), PwCheck (), PReadOnly (), EqDict (); 
static int PWhere (), PMaxLength (), PCurrentDict (); 

static int hash_tries = 0, hash_collisions = 0, hash_attempts = 0;
static int PHashStats (), PDictHash ();

InitDictionary ()
 {
	
	SysDict 	= MakeDict (SizeSysDict);
	
	Absent	= MakeObject (Condition);
	
 	OpForDict	= MakeOp ("(foralldict)", forDict, 0, 2, 3, 5);
 	
 	Install ("systemdict",	SysDict);
	
	InstallOp ("dict",	PDict,		1, 1, 0, 0, Integer);
	InstallOp ("begin",	PBegin,		1, 0, 0, 0, Dictionary);
	InstallOp ("end",	PEnd,		0, 0, 0, 0);
	InstallOp ("def",	PDef,		2, 0, 0, 0, Poly, Poly);
	InstallOp ("store",	PStore,		2, 0, 0, 0, Poly, Poly);
	InstallOp ("known",	PKnown,		2, 1, 0, 0, Dictionary, Poly);
	InstallOp ("load",	PLoad,		1, 1, 0, 0, Poly);
	InstallOp ("where",	PWhere,		1, 2, 0, 0, Poly);
	InstallOp ("maxlength",	PMaxLength,	1, 1, 0, 0, Dictionary);
	InstallOp ("currentdict",PCurrentDict,	0, 1, 0, 0);
	InstallOp ("hashstats", PHashStats,	0, 0, 0, 0);
	InstallOp ("dicthash", 	PDictHash,	1, 0, 0, 0, Dictionary);
	
	TypeInstallOp (Dictionary, "eq", 	EqDict,		2, 1, 0, 0, Dictionary, Dictionary);
	TypeInstallOp (Dictionary, "put", 	PutDict,	3, 0, 0, 0, Dictionary, Poly, Poly);
	TypeInstallOp (Dictionary, "get", 	GetDict,	2, 1, 0, 0, Dictionary, Poly);
	TypeInstallOp (Dictionary, "length", 	LengthDict,	1, 1, 0, 0, Dictionary);
	TypeInstallOp (Dictionary, "copy", 	CopyDict,	2, 0, 0, 0, Dictionary, Dictionary);
	TypeInstallOp (Dictionary, "forall", 	ForDict,	2, 0, 0, 4, Dictionary, Array);
 	TypeInstallOp (Dictionary, "readonly", 	PReadOnly,	1, 1, 0, 0, Dictionary);
 	TypeInstallOp (Dictionary, "rcheck", 	PrCheck,	1, 1, 0, 0, Dictionary);
 	TypeInstallOp (Dictionary, "wcheck", 	PwCheck,	1, 1, 0, 0, Dictionary);
		
	TypeInstallOp (Null, "eq", EqTrue, 2, 1, 0, 0, Null, Null);
 }

Type MakeType (size) int size;
 {
	Type dict;
	int i;
	struct dict_entry *hash;
	
	dict = (struct dict_struct *) Malloc ((unsigned) sizeof (struct dict_struct));
	dict->dict_size = size;
	dict->dict_fill = 0;
	dict->dict_flags = READABLE | WRITEABLE;
	dict->dict_body = hash = (struct dict_entry *) Malloc ((unsigned) sizeof (struct dict_entry) * size);
	
	for (i = 0; i < size; i++)
		hash[i].entry_key = Nil;
	return dict;
 }

void EmptyDict (dict) Type dict;
 {
 	int i, size = dict->dict_size;
 	struct dict_entry *hash = dict->dict_body;
	
	for (i = 0; i < size; i++)
		hash[i].entry_key = Nil;
 }
#ifdef notdef
Type TypeOf (item) Object item;
 {
 	return item.type;
 }
#endif
Object MakeDict (size) int size;
 {
	Object res;
	
	res = MakeObject (Dictionary);
	res.u.Dictionary = MakeType (size);
	return res;
 }

Object DictFrom (dict) Type dict;
 {
 	Object res;
 	
 	res = MakeObject (Dictionary);
 	res.u.Dictionary = dict;
 	return res;
 }

/*ARGSUSED*/
int EqTrue (a, b) Object a, b;
 {
 	return Push (OpStack, True);
 }

static Type Body (item) Object item;
 {
	return item.u.Dictionary;
 }

Type BodyDict (item) Object item;
 {
	return item.u.Dictionary;
 }

static int PHashStats ()
 {
 	extern int name_tries, name_hits;
 	
 	printf ("Attempts = %d, Successs = %d, Collisions = %d\n", hash_attempts, hash_tries, hash_collisions);
  	printf ("Name tries = %d, Name hits = %d\n", name_tries, name_hits);
	return TRUE;
 }

static int PDictHash (dict) Object dict;
 {
	struct dict_entry *hash = Body (dict)->dict_body;
 	int i, l = Body (dict)->dict_size;
 	
 	for (i = 0; i < l; i++)
 		if (TypeOf (hash[i].entry_key) == Null)
 			putchar (' ');
 		else
 			putchar ('@');
 	putchar ('\n');
 	return TRUE;
 }

static int EqDict (a, b) Object a, b;
 {
 	return Push (OpStack, MakeBoolean (Body (a) == Body (b)));
 }

int Equal (a, b) Object a, b;
 {
	return TypeOf (a) == TypeOf (b) &&
		a.Length == b.Length &&
		(a.u.Integer == b.u.Integer || TypeOf (a) == Array && a.Length == 0);
 }

static DictReplace (hash, key, value, size, h) struct dict_entry *hash; Object key, value; int size, h;
 {
 	int i;
 	
 	for (i = h;;)
	 {
	 	if (TypeOf (hash[i].entry_key) == Null)
	 		return FALSE;
	 	if (Equal (key, hash[i].entry_key))
	 	 {
			hash[i].entry_value = value;
			return TRUE;
		 }
		if (++i == size)
			i = 0;
		if (i == h)
			return FALSE;
	 }
 }

static DictAddition (dict, hash, key, value, size, h)
		struct dict_struct *dict; struct dict_entry *hash; Object key, value; int size, h;
 {
 	int i;
 	
	for (i = h;;)
	 {
	 	if (TypeOf (hash[i].entry_key) == Null)
		 {
			hash[i].entry_value = value;
			hash[i].entry_key = key;
			if (TypeOf (key) != Null)
				++dict->dict_fill;
			return TRUE;
		 }
		if (++i == size)
			i = 0;
		if (i == h)
			return FALSE;
	 }
 }

TypeInstall (dict, name, value) Type dict; char *name; Object value;
 {
	struct dict_entry *hash = dict->dict_body;
	int h, size = dict->dict_size;
	Object key;
	
	key = NameFrom (name);
	
	PanicIf (size == 0, "dictionary full in Install");
/*	h = BodyInteger (key) % size;	*/
	h = (key.Length + BodyInteger (key) + (int) TypeOf (key)) % size;
	if (TypeOf (key) == String)
		key = Cvn (key);
	if (DictReplace (hash, key, value, size, h))
		return;
	if (DictAddition (dict, hash, key, value, size, h))
		return;
	Panic ("dictionary full in Install");
 }

DictStore (dict, key, value) Object dict, key, value;
 {
	struct dict_entry *hash = Body (dict)->dict_body;
	int h, size = Body (dict)->dict_size;
	
	PanicIf (size == 0, "dictionary full in DictStore");
/*	h = BodyInteger (key) % size;	*/
	h = (key.Length + BodyInteger (key) + (int) TypeOf (key)) % size;
	if (TypeOf (key) == String)
		key = Cvn (key);
	if (DictReplace (hash, key, value, size, h))
		return;
	if (DictAddition (Body (dict), hash, key, value, size, h))
		return;
	Panic ("dictionary full in DictStore");
 }

Install (key, value) char *key; Object value;
 {
	DictStore (SysDict, NameFrom (key), value);
 }

static Object DictFind (hash, key, size) struct dict_entry *hash; Object key; int size;
 {
 	int i, h;
 	
 	++hash_attempts;
 	
 	if (size == 0)
 		return Absent;
 	if (TypeOf (key) == String)
		key = Cvn (key);
/*	h = BodyInteger (key) % size;	*/
	h = (key.Length + BodyInteger (key) + (int) TypeOf (key)) % size;
	for (i = h;;)
	 {
	 	if (TypeOf (hash[i].entry_key) == Null)
	 		return Absent;
	 	if (Equal (key, hash[i].entry_key))
	 	 {
			++hash_tries;
			return hash[i].entry_value;
		 }
		if (++i == size)
			i = 0;
		if (i == h)
			break;
		++hash_collisions;
	 }
	return Absent;
 }

Object DictLoad (dict, key) Object dict, key;
 {
	return DictFind (Body (dict)->dict_body, key, Body (dict)->dict_size);
 }

Object Lookup (dict, key) Type dict; Object key;
 {
	return DictFind (dict->dict_body, key, dict->dict_size);
 }

Object Load (key) Object key;
 {
 	int i;
 	
 	for (i = Height (DictStack) - 1; i >= 0; i--)
 	 {
 	 	Object res;
 	 	
 	 	res = DictLoad (DictStack->stack_body[i], key);
		if (TypeOf (res) != Condition)
			return res;
	 }
	return Absent;
 }

static int ForDict (dict, proc) Object dict, proc;
 {
 	VOID Push (ExecStack, dict);
 	VOID Push (ExecStack, MakeInteger (0));
 	VOID Push (ExecStack, proc);
	VOID Push (ExecStack, OpForDict);
	
	return TRUE;
 }

static int forDict () 
 {
 	Object dict, index, proc, t; /* VAX Compiler Broken. SUN optimiser broken */
 	int i;
 	
 	proc =	Pop (ExecStack);
 	index =	Pop (ExecStack);
 	dict =	Pop (ExecStack);
 	
 	for (i = BodyInteger (index); ; i++)
 	 {
 		if (i >= maxDict (dict))
 			return TRUE;
 		t = Body (dict)->dict_body[i].entry_key;
 		if (TypeOf (t) != Null)
 			break;
 	 }
 	VOID Push (ExecStack, dict);
 	VOID Push (ExecStack, MakeInteger (i + 1));
 	VOID Push (ExecStack, proc);
 	VOID Push (ExecStack, OpForDict);
 	VOID Push (ExecStack, proc);
 	
 	VOID Push (OpStack, t = Body (dict)->dict_body[i].entry_key);
 	VOID Push (OpStack, t = Body (dict)->dict_body[i].entry_value);
 	return TRUE;
 }
	
static int GetDict (object, key) Object object, key;
 {
 	Object res;
 	
	res = DictLoad (object, key);
	if (TypeOf (res) != Condition)
		return Push (OpStack, res);
	else
	 	return Error (PUndefined);
 }

static int PutDict (object, key, value) Object object, key, value;
 {
	if (lengthDict (object) == maxDict (object))
	 {
	 	Object t;	/* SUN optimiser broken */
	 	
	 	t = DictLoad (object, key);
		if (TypeOf (t) == Condition)
	 		return Error (PDictFull);
	 }
	DictStore (object, key, value);
	return TRUE;
 }

static int CopyDict (object1, object2) Object object1, object2;
 {
	if (lengthDict (object1) <= maxDict (object2) && lengthDict (object2) == 0)
	 {
		copyDict (object1, object2);
		VOID Push (OpStack, object2);
		return TRUE;
	 }
	return Error (PRangeCheck);
 }

int copyDict (from, to) Object from, to;
 {
 	int i, l = maxDict (from);
 	struct dict_entry *body = Body (from)->dict_body;
 	
 	for (i = 0; i < l; i++)
 		if (TypeOf (body[i].entry_key) != Null)
 			DictStore (to, body[i].entry_key, body[i].entry_value);
 }

static int PDict (size) Object size;
 {
 	if (BodyInteger (size) < 0)
 		return Error (PRangeCheck);
	else
		return Push (OpStack, MakeDict (BodyInteger (size)));
 }

static int PBegin (dict) Object dict;
 {
 	
 	if (Height (DictStack) == MaxStack (DictStack))
 		return Error (PDictOverflow);
 	else
 	 {
 	 	VOID Push (DictStack, dict);
 	 	return TRUE;
 	 }
 }

static int PEnd ()
 {
 	if (Height (DictStack) == 2)
 	 	return Error (PDictUnderflow);
 	else
 		VOID Pop (DictStack);
 	return TRUE;
 }

static int PDef (key, value) Object key, value;
 {
 	Object dict,t ;	/* SUN optimiser broken */
 	
 	if (!wCheck (dict = Top (DictStack)))
  		return Error (PInvAccess);
 	else if (maxDict (dict) == lengthDict (dict) && (t = DictLoad (dict, key), TypeOf (t)) == Condition)
 		return Error (PDictFull);
 	else
 	 {
 	 	DictStore (dict, key, value);
 	 	return TRUE;
 	 }
 }

static int PStore (key, value)	Object key, value;
 {
 	Object dict, t;	/* SUN optimiser broken */
 	
 	dict = Where (key);	/* SUN optimiser broken */
 	if (TypeOf (dict) == Condition)
 		dict = Top (DictStack);
 	if (TypeOf (key) == Null)
 		return Error (PTypeCheck);
 	else if (!wCheck (dict))
 		return Error (PInvAccess);
 	else if (maxDict (dict) == lengthDict (dict) && (t = DictLoad (dict, key), TypeOf (t)) == Condition)
 		return Error (PDictFull);
 	else
 	 {
 	 	DictStore (dict, key, value);
 	 	return TRUE;
 	 }
 }

static int PKnown (dict, key) Object dict, key;
 {
 	Object t;
 	
 	t = DictLoad (dict, key);
 	return Push (OpStack, MakeBoolean (TypeOf (t) != Condition));
 }

static int PLoad (key) Object key;
 {
 	Object res;
 	
  	res = Load (key);
 	if (TypeOf (res) == Condition)
 		return Error (PUndefined);
 	else if (TypeOf (key) == Null)
 		return Error (PTypeCheck);
 	else
 		return Push (OpStack, res);
 }

static int PWhere (key) Object key;	/* key --- dict true */
				/* key --- false */
 {
 	Object where;
 	
	if (TypeOf (key) == Null)
 		return Error (PTypeCheck);
 	where = Where (key);
 	if (TypeOf (where) == Condition)
 		return Push (OpStack, False);
 	else if (!rCheck (where))
 		return Error (PInvAccess);
 	else
	 	return Push (OpStack, where), Push (OpStack, True);
 }

int maxDict (dict) Object dict;
 {
 	return Body (dict)->dict_size;
 }

static int PMaxLength (dict) Object dict;
 {
 	return Push (OpStack, MakeInteger (maxDict (dict)));
 }

int lengthDict (dict) Object dict;
 {
 	return Body (dict)->dict_fill;
 }

static int LengthDict (dict) Object dict;
 {
 	return Push (OpStack, MakeInteger (lengthDict (dict)));
 }

static int PCurrentDict ()
 {
  	return Push (OpStack, Top (DictStack));
 }

static int PReadOnly (item) Object item;
 {
 	return Push (OpStack, ReadOnly (item));
 }

static int PrCheck (v) Object v;
 {
 	return Push (OpStack, MakeBoolean (rCheck (v)));
 }

static int PwCheck (v) Object v;
 {
 	return Push (OpStack, MakeBoolean (wCheck (v)));
 }
