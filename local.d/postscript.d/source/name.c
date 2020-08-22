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

struct name_struct
 {
	struct name_struct *next_name, *prev_name;
 	int string_length;
 	unsigned char *string_body;
 };

static int Cvs (), EqEq (), Eq (), ExecName ();
static Object OpExecName;

Object Self;

InitName ()
 {
 	OpExecName = MakeOp ("exec", ExecName,	1, 1, 0, 4, Name);
 	
	TypeInstall (Name, "exec", 	OpExecName);
	
 	TypeInstallOp (Name, "==", 	EqEq,	1, 0, 0, 0, Name);
  	TypeInstallOp (Name, "cvs", 	Cvs,	2, 1, 0, 0, Name, String);
	TypeInstallOp (Name, "eq", 	Eq, 	2, 1, 0, 0, Name, Name);
 }

unsigned char *BodyName (item) Object item;
 {
 	return item.u.Name->string_body;
 }

int lengthName (item) Object item;
 {
 	return item.u.Name->string_length;
 }

static int Cvs (v, string) Object v, string;
 {
 	int length;
 	
	if (lengthString (string) < (length = lengthName (v)))
		return Error (PRangeCheck);
	VOID Bcopy (BodyString (string), BodyName (v), length);
	return Push (OpStack, getIString (string, 0, length));
 }

static int EqEq (v) Object v;
 {
	if (!xCheck (v))
		putchar ('/');
	PrintName (v);
	return TRUE;
 }

static int Eq (a, b) Object a, b;
 {
	return Push (OpStack, MakeBoolean (BodyName (a) == BodyName (b) && lengthName (a) == lengthName (b)));
 }

static int ExecName (item) Object item;
 {
 	Object v;
 	
 	v = DictLookup (item);
	if (TypeOf (v) == Condition)
	 	return Error (PUndefined);
	else
		return Push (ExecStack, v);
 }

static struct name_struct *name_tree = NULL;

static struct name_struct *FindTreeName (s, length, root) unsigned char *s; int length; struct name_struct **root;
 {
	if (*root)
	 {
	 	int cmp = strncmp (s, (*root)->string_body, min (length, (*root)->string_length));
	 	
		if (cmp == 0 && (cmp = length - (*root)->string_length) == 0)
			return *root;
		else if (cmp < 0)
			return FindTreeName (s, length, &(*root)->prev_name);
		else
			return FindTreeName (s, length, &(*root)->next_name);
	 }
	else
	 {
 		struct name_struct *r = *root = (struct name_struct *) Malloc (sizeof (struct name_struct));
 		
		r->next_name = r->prev_name = NULL;
		r->string_body = Bcopy (Malloc ((unsigned) length), s, length);
		r->string_length = length;
		
		return *root;
	 }
 }

static int HashName (s, length) unsigned char *s; int length;
 {
	int i, res = 0;
	
	while (length--)
		res += *s++;
	return res;
 }

int name_tries = 0, name_hits = 0;

#define HASH_NAME_SIZE	1024

static struct name_struct *hash_name [HASH_NAME_SIZE];

static struct name_struct *FindName (s, length) unsigned char *s; int length;
 {
 	int hash = HashName (s, length);
 	struct name_struct *p;
 	
 	hash &= (HASH_NAME_SIZE - 1);
 	
 	p = hash_name [hash];
 	++name_tries;
 	++name_hits;
 	
 	if (p == NULL || p->string_length != length || strncmp (s, p->string_body, length))
 	 {
		p = FindTreeName (s, length, &name_tree);
 	 	
 	 	if (p != NULL)
 	 	 {
 	 		hash_name [hash] = p;
 	 		--name_hits;
 	 	 }
 	 }
 	return p;
 }

Object MakeName (s, length) unsigned char *s; int length;
 {
 	Object res;
 	
 	res = MakeObject (Name);
 	res.u.Name = FindName (s, length);
 	
 	return res;
 }

Object NameFrom (s) unsigned char *s;
 {
 	Object res;
	
 	res = MakeObject (Name);
 	res.u.Name = FindName (s, strlen (s));
 	
 	return res;
 }

Object Cvn (o) Object o;
 {
	Object res;
	
	res = MakeObject (Name);
	res.u.Name = FindName (BodyString (o), lengthString (o));
	
	return res;
 }

Object StringName (o) Object o;
 {
 	return MakeString (BodyName (o), lengthName (o));
 }

PrintName (n) Object n;
 {
 	printf ("%.*s", lengthName (n), BodyName (n));
 }

Object ParseId (o) Object o;
 {
	unsigned char buf[BUFSIZE], *p = buf;
	int c, length = 0, immediate = FALSE;
	Object number;
	
	if ((c = Getch (o)) == '/')
		immediate = TRUE;
	else
		Ungetch (o, c);
	
	for (;;)
	 {
	 	switch (c = Getch (o))
		 {
		 	case EOF: case ' ': case '\t': case '\n':
		 	    break;
		 	
		 	case '/': case '<': case '>': case '(': case ')':
			case '%': case '{': case '}': case '[': case ']':
				Ungetch (o, c);
			    break;
			
			default:
				*p++ = c; ++length;
			    continue;
		 }
		break;
	 }
	if (length == 0)
		return Absent;
	
	number = ParseNumber (buf, length);
	if (TypeOf (number) != Null)
		return number;
	else if (immediate)
	 	return Load (MakeName (buf, length));
	else
		return MakeName (buf, length);
 }
