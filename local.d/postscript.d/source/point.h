/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

typedef struct point { float x, y; } Point;

extern Matrix PointTranslate ();
extern Point NewPoint (), IntToExt (), MovePoint ();
extern int MoveTo (), LineTo ();
extern Object AssignMatrix ();
