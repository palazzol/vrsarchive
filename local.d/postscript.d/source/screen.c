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
#include "canon.h"

/********************************** SET TRANSFER STUFF **********************************/

#define TRANSFER_SIZE	256

static int transfer [TRANSFER_SIZE];

void InitTransfer (ppi) int ppi;
 {
 	int i;
 	
 	pixels_per_inch = ppi;
 	for (i = 0; i < TRANSFER_SIZE; i++)
 		transfer [i] = i;
 }

int HardColour (colour) Colour colour;
 {
	return transfer [(int) ((TRANSFER_SIZE - 1) * colour.brightness + .5)];
 }

int TransferSize ()
 {
	return TRANSFER_SIZE;
 }

void SetTransfer (tran) float *tran;
 {
 	int i;
 	
 	for (i = 0; i < TRANSFER_SIZE; i++)
 		transfer [i] = (TRANSFER_SIZE - 1) * tran[i] + .5;
 }

/********************************** SET SCREEN STUFF *******************************/

struct screen
 {
 	float val;
 	int sx, sy;
 	struct hardware *shade;
 } *screen = NULL;

static int screen_size, screen_side;

static int FreqSize (freq) float freq;
 {
 	int i = pixels_per_inch / freq + 0.5;
 	
 	if (i < 2)
		return 2;
	return i;
 }

struct hardware *GraySync (col) int col;
 {
 	col = col * (float) screen_size / TRANSFER_SIZE + 0.5;
 	
 	return screen[col].shade;
 }

int ScreenSize (freq, rot) float freq, rot;
 {
 	int size = FreqSize (freq);
 	
 	return size * size;
 }

void BuildScreen (freq, rotation, x, y) float freq, rotation, *x, *y;
 {
 	int size = FreqSize (freq);
 	int i, j;
 	
 	for (i = 0; i < size; i++)
 		for (j = 0; j < size; j++)
 			*x++ = (2 * i - size + 1) / (float) size,
 			*y++ = (2 * j - size + 1) / (float) size;
 }

static sgn (a) float a;
 {
	if (a == 0)
		return 0;
	else if (a < 0)
		return -1;
	else
		return 1;
 }

static int screen_cmp (a, b) char *a, *b;
 {
 	struct screen *aa = (struct screen *) a, *bb = (struct screen *) b;
 	
 	return sgn (aa->val - bb->val);
 }

void SetScreen (freq, rotation, thresh) float freq, rotation, *thresh;
 {
 	struct hardware *temp;
 	int i, j, size = FreqSize (freq);
 	struct screen *p;
 	
 	if (screen)
 	 {
 	 	for (i = 0; i < screen_size; i++)
 	 		DestroyHardware (screen [i].shade);
 	 	free ((char *) screen);
 	 }
 	p = screen = (struct screen *) Malloc ((unsigned) (((screen_size = size * size) + 1) * sizeof (struct screen)));
 	screen_side = size;
 	for (i = 0; i < size; i++)
 		for (j = 0; j < size; j++)
 		 {
 		 	p->val = *thresh++;
 		 	p->sx = i;
 		 	p->sy = j;
 		 	++p;
 		 }
 	qsort ((char *) screen, screen_size, sizeof (struct screen), screen_cmp);
 	temp = NewBitmapHardware (size, size);
 	BitBlt ((struct hardware *) NULL, temp, NewDevicePoint (0, 0), NewDevicePoint (0, 0), NewDevicePoint (size, size), ROP_TRUE);
 	
 	for (i = 0; i < screen_size; i++)
 	 {
 	 	screen [i].shade = NewBitmapHardware (size, size);
 	 	BitBlt (temp, screen[i].shade,
 	 			NewDevicePoint (0, 0), NewDevicePoint (0, 0),
 	 			NewDevicePoint (size, size), ROP_SOURCE);
 	 	BitBlt ((struct hardware *) NULL, temp,
 	 			NewDevicePoint (0, 0), NewDevicePoint (screen[i].sx, screen[i].sy),
 	 			NewDevicePoint (1, 1), ROP_FALSE);
 	 }
 	screen[screen_size].shade = temp;
 }
