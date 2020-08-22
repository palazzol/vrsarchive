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

static struct state gstate_stack [MAXGSAVES];

struct state *gstate = gstate_stack;

static int gstate_height = 0;

int GSave (), GRestore (), GRestoreAll ();

InitGSave ()
 {
 	InstallOp ("gsave",		GSave, 		0, 0, 0, 0);
 	InstallOp ("grestore",		GRestore, 	0, 0, 0, 0);
 	InstallOp ("grestoreall",	GRestoreAll, 	0, 0, 0, 0);
 	
 	gstate->screen.count = 0;	/* stops thresh getting freed accidentally */
 	gstate->transfer.tcount = 0;	/* stops tran getting freed accidentally */
 }

int GSave ()
 {
 	if (gstate_height == MAXGSAVES - 1)
 		return Error (PLimitCheck);
 	gstate_stack [gstate_height + 1] = gstate_stack [gstate_height];
 	++gstate_height; ++gstate;
 	gstate->path = PathCopy (gstate->path);
 	gstate->clip = PathCopy (gstate->clip);
 	LinkDevice (gstate->device);
 	LinkDevice (gstate->clipdevice);
 	++gstate->screen.count;
 	++gstate->transfer.tcount;
 	
 	return TRUE;
 }

int GRestore ()
 {
 	if (gstate_height == 0)
 		VOID InitGraphics ();
 	else
 	 {
 	 	int sflag = FALSE, tflag = FALSE;
 	 	
 		UnlinkDevice (gstate->device);
  		UnlinkDevice (gstate->clipdevice);
		PathFree (gstate->path);
 		PathFree (gstate->clip);
 		
 		if (gstate->screen.count == 1)
 		 {
 			Free ((char *) gstate->screen.thresh);
 			sflag = TRUE;
 		 }
  		if (gstate->transfer.tcount == 1)
 		 {
 			Free ((char *) gstate->transfer.tran);
 			tflag = TRUE;
 		 }
  		--gstate_height;
 		--gstate;
 		if (sflag)
  			SetScreen (gstate->screen.frequency, gstate->screen.rotation, gstate->screen.thresh);
  		if (tflag)
  			SetTransfer (gstate->transfer.tran);
  		SetClipHardware (gstate->device->dev, (gstate->clipdevice ? gstate->clipdevice->dev : NULL));
	 }
 	
 	return TRUE;
 }

int GRestoreAll ()
 {
	while (gstate != gstate_stack)
		VOID GRestore ();
 	InitGraphics ();
 	
 	return TRUE;
 }
