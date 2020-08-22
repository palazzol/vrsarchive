/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

typedef struct matrix { float A, B, C, D, tx, ty; } Matrix;

typedef struct vector { float vx, vy, vt; } Vector;

extern Matrix NewMatrix (), MatMult (), MatInvert (), Translate (), Rotate (), Scale ();
extern Vector NewVector (), DiffVector ();
extern Vector Transform (), ITransform (), DTransform (), IDTransform ();
