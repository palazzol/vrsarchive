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
#include <sys/param.h>
#include <sys/times.h>
#ifndef HZ
#	define HZ 60
#endif

static int PUserTime ();
 	
InitMisc ()
 {
  	InstallOp ("usertime",	PUserTime,	0, 1, 0, 0);
 	InstallOp ("==", 	PolyFirst, 	1, 1, 0, 0, Poly);
  }

static int PUserTime ()
 {
 	struct tms tbuf;
 	long times ();
 	
	times (&tbuf);
 	return Push (OpStack, MakeInteger ((int) (tbuf.tms_utime * 1000 / HZ)));
 }
