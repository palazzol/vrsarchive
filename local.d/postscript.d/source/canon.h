/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

#define ISWIN	1

struct hardware
 {
 	union hard {
 		char *addr;
 		int handle;
 	} hard;
 	int flags;
 	DevicePoint extent;
 	struct hardware *aux, *clip;
 };

extern void InitTransfer ();
extern int pixels_per_inch;

extern int single_rop[];

extern struct hardware *GraySync ();
