/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

#include "device.h"
#include "point.h"
#include "font.h"

#define PI 3.14159265358979

#define MAXDASH	11

#define MAXGSAVES	20

#define BBOX_LEFT	0
#define BBOX_TOP	1
#define BBOX_RIGHT	2
#define BBOX_BOTTOM	3

struct show_context
 {
 	char InShow;
 	char space;
	Point Width;
	HardPoint shim, space_shim;
 	Object CharName;
 	struct cache *ccache;
 	Matrix mat;
 };

struct state
 {
 	Matrix CTM;
 	Colour colour;
 	HardPoint cp; int cp_defined;
 	Path path;
 	Path clip;
 	Object font;
 	float line_width;
 	int line_cap;
 	int line_join;
 	struct
 	 {
 	 	float frequency, rotation, *thresh;
 	 	int count;
 	 	Object spot_function;
 	  } screen;
 	struct {
 		Object transfn;
 		float *tran;
 		int tcount;
 	} transfer;
 	int flatness;
 	float miter_limit;
 	float dash_offset, dash_array [MAXDASH];
 	int dash_length;
	struct device *device;
	struct show_context *show;
	struct device *clipdevice;
 };

extern struct state *gstate;

extern int stroke_method, fill_method;
