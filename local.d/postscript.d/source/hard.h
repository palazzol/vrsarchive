/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

#include "mat.h"
#include "rop.h"
#include "colour.h"
#include "path.h"

typedef struct device_point { int dx, dy; } DevicePoint;

extern struct hardware *HardwareFromString (), *NewBitmapHardware (), *NewWindowHardware (), *InitHardware ();
extern DevicePoint HardwareExtent ();
extern char *StringFromHardware ();

extern void BitBlt (), BitBltBlob (), BitBltLine (), DestroyHardware ();
extern void HardUpdate (), UpdateControl ();
extern Matrix DeviceMatrix ();

extern void RasterTile (), BitBltTrapezoid ();

extern int IsWindowHardware (), TransferSize ();
extern void SetTransfer ();
extern void Paint (), PaintLine (), PaintTrapezoid ();
extern int ScreenSize ();
extern void BuildScreen (), SetScreen ();
extern DevicePoint NewDevicePoint ();
