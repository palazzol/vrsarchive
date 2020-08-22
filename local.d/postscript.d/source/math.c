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

#define PI 3.14159265358979

static int PRand (), PSrand (), PRrand (), Idiv ();

static catchmath ();
extern int errno;
jmp_buf env;

InitMath ()
 {
	InstallOp ("abs",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("add",	PolyPair,	2, 1, 0, 0, Poly, Poly);
	InstallOp ("div",	PolyPair,	2, 1, 0, 0, Poly, Poly);
	InstallOp ("mod",	PolyPair,	2, 1, 0, 0, Poly, Poly);
	InstallOp ("mul",	PolyPair,	2, 1, 0, 0, Poly, Poly);
	InstallOp ("neg",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("sub",	PolyPair,	2, 1, 0, 0, Poly, Poly);
	InstallOp ("sqrt",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("exp",	PolyPair,	2, 1, 0, 0, Poly, Poly);
	InstallOp ("ceiling",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("floor",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("round",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("truncate",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("atan",	PolyPair,	2, 1, 0, 0, Poly, Poly);
	InstallOp ("cos",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("sin",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("ln",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("log",	PolyFirst,	1, 1, 0, 0, Poly);
	InstallOp ("rand",	PRand,		0, 1, 0, 0);
	InstallOp ("srand",	PSrand,		1, 0, 0, 0, Integer);
	InstallOp ("rrand",	PRrand,		0, 1, 0, 0);
 	InstallOp ("idiv", 	Idiv,		2, 1, 0, 0, Float, Float);
	
/*	PanicIf (setjmp (env), "Unexpected floating point error");
	VOID signal (SIGFPE, catchmath);
*/ }

static catchmath ()
 {
	VOID signal (SIGFPE, catchmath);
 	longjmp (env, TRUE);
 }

float Deg (r) float r;
 {
	float res = 360 * r / (2 * PI);
	return res < 0 ? res + 360 : res;
 }

float Rad (d) float d;
 {
	return 2 * PI * d / 360;
 }

float BodyFloat (ob) Object ob;
 {
	if (TypeOf (ob) == Integer)
		return (float) BodyInteger (ob);
	else if (TypeOf (ob) == Real)
		return BodyReal (ob);
	else
		Panic ("BodyFloat passed arg of bad type");
	return 0.0;
 }

Object ParseNumber (s, length) char *s; int length;
 {
 	int c, sign, ival;
 	float fval;
 	
 	/*if (setjmp (env))
 		return Absent;*/
 	if (length == 0)
 		return Nil;
 	sign = ParseSign (&s, &length);
 	if (*s == '.')
 	 {
 	 	c = *s++;
 	 	--length;
 	 	ival = 0;
 	 }
 	else
 	 {
 	 	ival = ParseInteger (&s, &length, 10);
 		if (length < 0)
 			return Nil;
 		else if (length == 0)
 			return MakeInteger (ival * sign);
 		
 		c = *s++; --length;
 	 }
 	if (c == '.')
 	 {
 	 	int olength = length, dval;
 	 	
		fval = ival;
 		dval = ParseInteger (&s, &length, 10);
 		fval += dval * pow (10.0, (float)(length - olength));
 		if (length < 0)
 			return Nil;
 		else if (length == 0)
 			return MakeReal (sign * fval);
 	 }
 	else if (c == '#')
  	 {
  	 	int base = ival;
  	 	
  	 	if (base < 2 || base > 36)
  	 		return Nil;
  	 	ival = ParseInteger (&s, &length, base);
  		if (length >= 0)
  			return MakeInteger (ival * sign);
  		return Nil;
	 }
	else
	 {
		--s;
		++length;
		fval = ival;
	 }
	
	fval *= sign;
	c = *s++; --length;
	if (c == 'e' || c == 'E')
	 {
	  	Object res;
	  	int sign = ParseSign (&s, &length), power = ParseInteger (&s, &length, 10);
	 	
 		if (length < 0)
 			return Nil;
 		res = MakeReal (fval * pow (10.0, (double) power * sign));
 		return res;
 	 }
 	else
 		return Nil;
 }

static int ParseSign (s, length) char **s; int *length;
 {
 	int c;
 	
 	if (*length == 0)
 		return 1;
 	c = *(*s)++;
 	--*length;
	if (c == '+')
 		return 1;
 	else if (c == '-')
 		return -1;
	++*length;
	--*s;
	return 1;
 }

static int ParseInteger (p, length, base) char **p; int *length, base;
 {
 	int present = FALSE, ival = 0, digit;
 	if (*length == 0)
 	 {
 		*length = -1;
 		return 0;
 	 }
 	for (;;)
 	 {
 	 	int c = *(*p)++;
 	 	
 	 	if (c >= '0' && c <= '9')
 	 		digit = c - '0';
 	 	else if (c >= 'a' && c <= 'z')
 	 		digit = c - 'a' + 10;
 	 	else if (c >= 'A' && c <= 'Z')
 	 		digit = c - 'A' + 10;
 	 	else
 	 		break;
 	 	if (digit < base)
 	 		/*ival = StrictAdd (StrictMul (ival, base), digit);*/
 	 		ival = ival * base + digit;
 	 	else
 	 		break;
 		++present;
 	 	if (--*length == 0)
 	 		return ival;
 	 }
 	--*p;
 	if (!present)
 		*length = -1;
 	return ival;
 }

static int seed = 1;

static int random ()
 {
	return seed = ((seed * 1103515245 + 12345) & 0x7fffffff);
 }

static int PRand ()		/* --- integer */
 {
 	VOID random ();
 	return Push (OpStack, MakeInteger (seed));
 }

static int PSrand (see) Object see;
 {
	srand ((unsigned) (seed = BodyInteger (see)));
 	return TRUE;
 }

static int PRrand ()
 {
 	return Push (OpStack, MakeInteger (seed));
 }

static int Idiv (a, b) Object a, b;
 {
  	if (BodyReal (b) == 0)
 		return Error (PUnResult);
	return Push (OpStack, MakeInteger ((int) BodyReal (a) / (int) BodyReal (b)));
 }
