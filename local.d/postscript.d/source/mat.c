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
#include "graphics.h"

Matrix identity =   { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };

Matrix NewMatrix (A, B, C, D, tx, ty) float A, B, C, D, tx, ty;
 {
 	Matrix m;
 	
 	m.A = A; m.B = B; m.C = C; m.D = D; m.tx = tx; m.ty = ty;
 	
 	return m;
 }

Vector NewVector (A, B, vt) float A, B, vt;
 {
 	Vector v;
 	
 	v.vx = A; v.vy = B; v.vt = vt;
 	
 	return v;
 }

Matrix Translate (m, x, y) Matrix m; float x, y;
 {
 	return NewMatrix (m.A,				   m.B,
 			   m.C,				   m.D,
 			   x * m.A + y * m.C + m.tx, x * m.B + y * m.D + m.ty);
 }

HereTranslate (pm, p) Matrix *pm; Point p;
 {
 	pm->tx += p.x * pm->A + p.y * pm->C;
 	pm->ty += p.x * pm->B + p.y * pm->D;
 }

Matrix Scale (m, x, y) Matrix m; float x, y;
 {
	return NewMatrix (m.A * x, 	m.B * x,
			   m.C * y,	m.D * y,
			   m.tx,	m.ty);
 }

Matrix Rotate (m, a) Matrix m; float a;
 {
 	float ca = cos(a), sa = sin(a);
 	
 	return NewMatrix (m.A * ca + m.C * sa, m.B * ca + m.D * sa,
 			   m.C * ca - m.A * sa, m.D * ca - m.B * sa,
 			   m.tx,		m.ty);
 }

Matrix MatMult (a, b) Matrix a, b;
 {
 	return NewMatrix ( a.A  * b.A + a.B * b.C, 	   a.A * b.B + a.B * b.D,
 			   a.C  * b.A + a.D * b.C, 	   a.C * b.B + a.D * b.D,
 			   a.tx * b.A + a.ty * b.C + b.tx, a.tx * b.B + a.ty * b.D + b.ty);
 }

Matrix MatInvert (m) Matrix m;		/* know any good matrix inversion algorithms ? */
 {					/* this one will be simplistic */
 	float det = m.A * m.D - m.B * m.C;
 	
	return NewMatrix (m.D / det, 			  -m.B / det,
			 -m.C  / det, 			   m.A / det,
			 (m.C * m.ty - m.D * m.tx) / det, -(m.A * m.ty - m.B * m.tx) / det);
 }

Vector Transform (v, m) Vector v; Matrix m;
 {
 	return NewVector (v.vx * m.A + v.vy * m.C + v.vt * m.tx,
 			   v.vx * m.B + v.vy * m.D + v.vt * m.ty,
 			   v.vt);
 }

Vector DTransform (v, m) Vector v; Matrix m;
 {
 	return NewVector (v.vx * m.A + v.vy * m.C,
 			   v.vx * m.B + v.vy * m.D,
 			   v.vt);
 }

Vector ITransform (v, mi) Vector v; Matrix mi;
 {
 	Matrix m;
 	
 	m = MatInvert (mi);
 	return NewVector (v.vx * m.A + v.vy * m.C + v.vt * m.tx,
 			   v.vx * m.B + v.vy * m.D + v.vt * m.ty,
 			   v.vt);
 }

Vector IDTransform (v, mi) Vector v; Matrix mi;
 {
 	Matrix m;
 	
 	m = MatInvert (mi);
 	return NewVector (v.vx * m.A + v.vy * m.C,
 			   v.vx * m.B + v.vy * m.D,
 			   v.vt);
 }

Vector DiffVector (a, o) Vector a, o;
 {
 	return NewVector (a.vx - o.vx, a.vy - o.vy, 1.0);
 }
