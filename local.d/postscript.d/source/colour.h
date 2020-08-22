/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

typedef struct { float hue, saturation, brightness; } Colour;

extern Colour Black, White;

extern Colour NewColour (), NewHSBColour (), NewRGBColour (), NewGray ();
extern void ColourHSB (), ColourRGB ();
extern float Gray (), Brightness ();
