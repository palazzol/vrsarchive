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

static Object OpForString, Make ();

int putIString ();

Object ParseString (), ParseHexString (), StringFrom ();
static unsigned char *Body ();
static int forString ();
	
static int Exec (), Token (), PString (), Search (), AnchorSearch (), Copy (), EqEq ();
static int Length (), ForAll (), Get (), Put (), GetInterval (), PutInterval (), Eq (), Lt (), Le (), Gt (), Ge (), PrCheck (), PwCheck ();
static int Cvi (), Cvr (), Cvs (), PReadOnly (), PExecOnly ();

InitString ()
 {
 	OpForString = MakeOp ("(forallstring)", forString, 0, 0, 3, 5);
 	
	TypeInstallOp (String, "cvi", 		Cvi, 		1, 1, 0, 0, String);
 	TypeInstallOp (String, "cvr", 		Cvr, 		1, 1, 0, 0, String);
  	TypeInstallOp (String, "cvs", 		Cvs, 		2, 1, 0, 0, String, String);
	TypeInstallOp (String, "==", 		EqEq, 		1, 0, 0, 0, String);
 	TypeInstallOp (String, "exec", 		Exec,		1, 0, 0, 1, String);
  	TypeInstallOp (String, "token", 	Token,		1, 3, 0, 2, String);
	TypeInstallOp (String, "eq", 		Eq, 		2, 1, 0, 0, String, String);
 	TypeInstallOp (String, "lt", 		Lt, 		2, 1, 0, 0, String, String);
  	TypeInstallOp (String, "le", 		Le, 		2, 1, 0, 0, String, String);
 	TypeInstallOp (String, "gt", 		Gt, 		2, 1, 0, 0, String, String);
 	TypeInstallOp (String, "ge", 		Ge, 		2, 1, 0, 0, String, String);
 	TypeInstallOp (String, "length", 	Length,		1, 1, 0, 0, String);
 	TypeInstallOp (String, "forall", 	ForAll,		2, 0, 0, 4, String, Array);
 	TypeInstallOp (String, "copy",		Copy,		2, 1, 0, 0, String, String);
  	TypeInstallOp (String, "get", 		Get,		2, 1, 0, 0, String, Integer);
 	TypeInstallOp (String, "put", 		Put,		3, 0, 0, 0, String, Integer, Integer);
 	TypeInstallOp (String, "getinterval", 	GetInterval,	3, 1, 0, 0, String, Integer, Integer);
 	TypeInstallOp (String, "putinterval", 	PutInterval,	3, 0, 0, 0, String, Integer, String);
 	TypeInstallOp (String, "executeonly", 	PExecOnly,	1, 1, 0, 0, String);
 	TypeInstallOp (String, "readonly", 	PReadOnly,	1, 1, 0, 0, String);
 	TypeInstallOp (String, "rcheck", 	PrCheck,	1, 1, 0, 0, String);
 	TypeInstallOp (String, "wcheck", 	PwCheck,	1, 1, 0, 0, String);
 	TypeInstallOp (String, "token", 	Token,		1, 3, 0, 0, String);

	InstallOp ("string",		PString,	1, 1, 0, 0, Integer);
 	InstallOp ("search",		Search,		2, 4, 0, 0, String, String);
 	InstallOp ("anchorsearch",	AnchorSearch,	2, 3, 0, 0, String, String);
 }

static int Cvi (rep) Object rep;
 {
 	Object v;
 	
	v = ParseNumber (BodyString (rep), lengthString (rep));
	if (TypeOf (v) == Integer)
		return Push (OpStack, v);
	else if (TypeOf (v) == Real)
		return Push (OpStack, IntReal (v));
	else if (TypeOf (v) == Condition)
		return Error (PUnResult);
	else
		return Error (PSyntaxError);
 }

static int Cvr (rep) Object rep;
 {
	Object v;
	
	v = ParseNumber (BodyString (rep), lengthString (rep));
	if (TypeOf (v) == Real)
		return Push (OpStack, v);
	else if (TypeOf (v) == Integer)
		return Push (OpStack, RealInteger (v));
	else if (TypeOf (v) == Condition)
		return Push (OpStack, rep), Error (PUnResult);
	else
		return Push (OpStack, rep), Error (PSyntaxError);
 }

static int Cvs (v, string) Object v, string;
 {
 	int length;
 	
	if (lengthString (string) < (length = lengthString (v)))
		return Error (PRangeCheck);
	VOID Bcopy (BodyString (string), BodyString (v), length);
	return Push (OpStack, getIString (string, 0, length));
 }

static int EqEq (v) Object v;
 {
 	int c, i, l = lengthString (v);
 	unsigned char *s = BodyString (v);
 	
	putchar ('(');
	for (i = 0; i < l && !Interrupted (); i++)
		switch (c = s[i])
		 {
			default:
				if (c < ' ' || c > 126)
					printf ("\\%o", c);
				else
					putchar (c);
	   			break;
	
			case '\n':	printf ("\\n"); break;
			case '\r':	printf ("\\r"); break;
			case '\t':	printf ("\\t"); break;
			case '\b':	printf ("\\b"); break;
			case '\f':	printf ("\\f"); break;
		 }
	printf (")");
	return TRUE;
 }

static int Exec (item) Object item;
 {
 	return Push (ExecStack, Cvx (FileString (item)));
 }

static int Token (f) Object f;
 {
 	Object res, fs;
 	
 	res = Parse (fs = FileString (f));
 	if (TypeOf (res) == Condition)
	 	return Error (PSyntaxError);
	else if (TypeOf (res) == Null)
		return Error (PUnResult);
	else if (TypeOf (res) != Boolean)
	 {
		VOID Push (OpStack, Cvx (SameFlags (f, getIString (f,
						lengthString (f) - BodyFile (fs)->available,
						BodyFile (fs)->available))));
		VOID Push (OpStack, res);
		VOID Push (OpStack, True);
	 }
	else if (BodyBoolean (res))
	 {
	 	VOID Push (OpStack, f);
		VOID Push (OpStack, False);
	 }
	else
		return Error (PSyntaxError);
	return TRUE;
 }

Object MakeString (s, length) unsigned char *s; int length;
 {
 	Object res;
  	char *new;
	
 	res = MakeObject (String);
	res.u.String = (unsigned char *) (new = Malloc ((unsigned) length));
 	bcopy (s, new, length);
 	res.Length = length;
 	
 	return res;
 }

static Object Make (s, length) unsigned char *s; int length;
 {
 	Object res;
 	char *new;
 	
 	res = MakeObject (String);
 	res.u.String = (unsigned char *) (new = Malloc ((unsigned) length));
 	bcopy (s, new, length);
 	res.Length = length;
 	
 	return res;
 }

Object StringFrom (s) unsigned char *s;
 {
	Object res;
	
	res = MakeObject (String);
	res.u.String = s;
	res.Length = strlen ((char *) s);
	
	return res;
 }

static int Eq (a, b) Object a, b;
 {
	return Push (OpStack, MakeBoolean (EqString (a, b)));
 }

int EqString (a, b) Object a, b;
 {
 	int 	al = lengthString (a),
 		bl = lengthString (b);
	
	return al == bl && 0 == strncmp ((char *) Body (a), (char *) Body (b), al);
 }

static int Lt (a, b) Object a, b;
 {
 	int 	al = lengthString (a),
 		bl = lengthString (b),
		cmp = strncmp ((char *) Body (a), (char *) Body (b), min (al, bl));
	
	return Push (OpStack, MakeBoolean (cmp < 0 || cmp == 0 && al < bl));
 }

static int Le (a, b) Object a, b;
 {
	return Push (OpStack,
		MakeBoolean (
			strncmp ((char *) Body (a),
				 (char *) Body (b),
				 min (lengthString (a), lengthString (b))) <= 0));
 }

static int Gt (a, b) Object a, b;
 {
 	int 	al = lengthString (a),
 		bl = lengthString (b),
		cmp = strncmp ((char *) Body (a), (char *) Body (b), min (al, bl));
	
	return Push (OpStack, MakeBoolean (cmp > 0 || cmp == 0 && al > bl));
 }

static int Ge (a, b) Object a, b;
 {
	return Push (OpStack,
		MakeBoolean (
			strncmp ((char *) Body (a),
				 (char *) Body (b),
				 min (lengthString (a), lengthString (b))) >= 0));
 }

int lengthString (object) Object object;
 {
 	return object.Length;
 }

static int Length (object) Object object;
 {
 	return Push (OpStack, MakeInteger (lengthString (object)));
 }

unsigned char *BodyString (object) Object object;
 {
 	return object.u.String;
 }

static unsigned char *Body (object) Object object;
 {
 	return object.u.String;
 }

static int Get (object, key) Object object, key;
 {
 	int index;
 	
	if (TypeOf (key) == Integer)
		index = BodyInteger (key);
	else if (TypeOf (key) == Real)
		index = (int) BodyReal (key);
	else
	 	return Error (PTypeCheck);
	if (index >= 0 && index < lengthString (object))
		return Push (OpStack, MakeInteger ((int) Body (object)[index]));
	else
		return Error (PRangeCheck);
 }

int getString (object, index) Object object; int index;
 {
	return Body (object)[index];
 }

static int GetInterval (object, begin, length) Object object, begin, length;
 {
 	int b = BodyInteger (begin), l = BodyInteger (length);
 	
	if (l < 0 || b < 0 || b + l > lengthString (object))
		return Error (PRangeCheck);
	return Push (OpStack, getIString (object, b, l));
 }

Object getIString (object, begin, length) Object object; int begin, length;
 {
 	return Make (Body (object) + begin, length);
 }

static int Put (object, key, value) Object object, key, value;
 {
 	int index;
 	
	if (TypeOf (key) == Integer)
		index = BodyInteger (key);
	else if (TypeOf (key) == Real)
		index = (int) BodyReal (key);
	else
	 	return Error (PTypeCheck);
	if (index < 0 || index >= lengthString (object))
		return Error (PRangeCheck);
	else if (TypeOf (value) != Integer)
		return Error (PTypeCheck);
	Body (object)[index] = BodyInteger (value);
	return TRUE;
 }

putString (object, index, value) Object object; int index, value;
 {
 	Body (object)[index] = value;
 }
 
static int PutInterval (object1, begin, object2) Object object1, begin, object2;
 {
 	int b = BodyInteger (begin);
 	
	if (lengthString (object2) + b > lengthString (object1))
		return Error (PRangeCheck);
	VOID putIString (object1, b, object2);
	return TRUE;
 }

int putIString (object1, begin, object2) Object object1, object2; int begin;
 {
 	int l = lengthString (object2);
 	unsigned char *from = Body (object2), *to = Body (object1) + begin;
 	
 	while (l--)
 		*to++ = *from++;
 	
 	return TRUE;
 }

static int Copy (object1, object2) Object object1, object2;
 {
	if (lengthString (object1) <= lengthString (object2))
	 {
		VOID putIString (object2, 0, object1);
		VOID Push (OpStack, getIString (object2, 0, lengthString (object1)));
		return TRUE;
	 }
	else
		return Error (PRangeCheck);
 }

static int match (a, b, max) unsigned char *a, *b; int max;
 {
 	int i;
 	
 	for (i = 0; i < max && *a++ == *b++; i++)
 		;
 	return i;
 }

static ForAll (string, proc) Object string, proc;
 {
  	VOID Push (ExecStack, string);
 	VOID Push (ExecStack, MakeInteger (0));
 	VOID Push (ExecStack, proc);
	VOID Push (ExecStack, OpForString);
 	
	return TRUE;
 }

static int forString ()
 {
 	Object string, index, proc;
 	
 	proc   = Pop (ExecStack);
 	index  = Pop (ExecStack);
 	string = Pop (ExecStack);
 	
 	if (BodyInteger (index) >= lengthString (string))
 		return TRUE;
 	
 	VOID Push (ExecStack, string);
 	VOID Push (ExecStack, MakeInteger (BodyInteger (index) + 1));
 	VOID Push (ExecStack, proc);
 	VOID Push (ExecStack, OpForString);
 	VOID Push (ExecStack, proc);
 	
 	VOID Push (OpStack, MakeInteger (getString (string, BodyInteger (index))));
 	return TRUE;
 }

Object ParseString (o) Object o;
 {
 	unsigned char buf[BUFSIZE], *p = buf;
 	int c, length = 0, level = 0, foo;
 	
  	for (;;)
  	 {
		if (length == BUFSIZE)
			return Absent;
 	 	switch (c = Getch (o))
 		 {
  			default:	*p++ = c; ++length;    continue;
  			
 		 	case EOF:     return Nil;
  		 	
			case '(':	++level; *p++ = c; ++length;	    continue;
			
 			case ')':
 				if (--level < 0)
 					break;
 				else
 				 {
				   	*p++ = c;
 				   	++length;
 				 }
 			    continue;
			
		 	case '\\':
 		 		if (BodyFile (o)->file_type != StringFile)
 		 			switch (c = Getch (o))
 	 				 {
 	 				 	default:  *p++ = c; ++length;			break;
 	 				 	
 	 				 	case EOF: 					return Nil;
 	 			 		
 						case '\n': /* nothing */			break;
	 		 			case 'n': c = '\n'; *p++ = c; ++length;		break;
 	 		 			case 'r': c = '\r'; *p++ = c; ++length;		break;
 	 		 			case 't': c = '\t'; *p++ = c; ++length;		break;
 	 		 			case 'b': c = '\b'; *p++ = c; ++length;		break;
 	 		 			case 'f': c = '\f'; *p++ = c; ++length;		break;
 	 				 	
 	 			 		case '0': case '1': case '2': case '3':
						case '4': case '5': case '6': case '7':
							c = c - '0';
							foo = Getch (o);	/* breaks PNX C compiler when inline */
							c = (c << 3) + foo - '0';
							foo = Getch (o);	/* breaks PNX C compiler when inline */
							c = (c << 3) + foo - '0';
							*p++ = c; ++length;
						    break;
					 }
				else
					*p++ = c, ++length;
		 	   continue;
  		 }
  		break;
  	 }
 	
 	return Make (buf, length);
 }

Object ParseHexString (o) Object o;
 {
 	unsigned char buf [BUFSIZE], *p = buf;
 	int c, count = 0, val = 0, length = 0;
 	
 	while ((c = Getch (o)) != '>' && c != EOF && length < BUFSIZE)
 	 {
		if (c >= '0' && c <= '9')
 			val = val * 16 + c - '0';
 		else if (c >= 'A' && c <= 'F')
 			val = val * 16 + 10 + c - 'A';
 		else if (c >= 'a' && c <= 'z')
 			val = val * 16 + 10 + c - 'a';
 		else
 			--count;
		if (++count == 2)
 		 {
 			*p++ = val; ++length;
 			count = val = 0;
 		 }
  	 }
 	if (length == BUFSIZE)
 		return Absent;
 	return c == EOF ? Nil : Make (buf, length);
 }

static int PString (length) Object length;
 {
 	int l;
 	unsigned char *body, *p;
 	
 	if ((l = BodyInteger (length)) < 0)
 		return Error (PRangeCheck);
 	body = (unsigned char *) Malloc ((unsigned) l);
 	
 	for (p = body; p < &body[l]; p++)
 		*p = '\0';
 	return Push (OpStack, Make (body, l));
 }

static int AnchorSearch (string, seek) Object string, seek;
 {
	unsigned char *str = Body (string), *see = Body (seek);
	int m, lstr = lengthString (string), lsee = lengthString (seek);
	
 	if (lsee > lstr || (m = match (str, see, lsee)) != lsee)
 	 	VOID Push (OpStack, string),
 	 	VOID Push (OpStack, False);
 	else
 	 {
	 	VOID Push (OpStack, Make (str + m, lstr - m));
	 	VOID Push (OpStack, Make (str, m));
		VOID Push (OpStack, True);
	 }
	return TRUE;
 }

static int Search (string, seek) Object string, seek;
 {
	unsigned char *str = Body (string), *body = str, *see = Body (seek);
	int i, m, lstr = lengthString (string), lsee = lengthString (seek);
	
	for (i = lstr - lsee; i >= 0; i--)
		if ((m = match (str, see, lsee)) == lsee)
		 {
		 	VOID Push (OpStack, Make (body + (str - body) + m, lstr - (str - body + m)));
		 	VOID Push (OpStack, Make (body + (str - body), m));
	 		VOID Push (OpStack, Make (body, str - body));
		 	VOID Push (OpStack, True);
		 	return TRUE;
		 }
		else
			++str;
	VOID Push (OpStack, string);
	VOID Push (OpStack, False);
	
	return TRUE;
 }

static int PExecOnly (item) Object item;
 {
 	return Push (OpStack, ExecOnly (item));
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

unsigned char *Bcopy (a, b, n) unsigned char *a, *b; int n;
 {
 	unsigned char *res = a;
 	
 	while (n--)
 		*a++ = *b++;
 	return res;
 }
