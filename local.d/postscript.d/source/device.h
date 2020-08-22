/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

#include "hard.h"

struct device
 {
 	Matrix default_matrix;
 	Path default_clip;
 	int link_count;
 	struct hardware *dev;
 };

extern struct device *NewDevice (), *NewCacheDevice (), *LinkDevice (), *DeviceFrom (), *UniqueDevice ();
extern struct device *NewBitmapDevice ();
