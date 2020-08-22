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


Colour NewGray (level) float level;
 {
 	return NewHSBColour (0.0, 0.0, level);
 }

Colour NewColour (h, s, b) float h, s, b;
 {
 	return NewHSBColour (h, s, b);
 }

Colour NewHSBColour (h, s, b) float h, s, b;
 {
	Colour res;
	
	res.hue = h;
	res.saturation = s;
	res.brightness = b;
	
	return res;
 }

Colour NewRGBColour (R, G, B) float R, G, B;
 {
 	float H, S, L, m, M, r, g, b;
 	
 	M = R > G ? R : G; M = M > B ? M : B;
 	m = R < G ? R : G; m = m < B ? m : B;
 	if (M != m)
 	 {
 	 	r = (M - R) / (M - m);
 	 	g = (M - G) / (M - m);
 	 	b = (M - B) / (M - m);
 	 }
 	L = (M + m) / 2;
 	
 	if (M == m)
 	 	S = 0;
 	else if (L <= 0.5)
 		S = (M - m) / (M + m);
 	else
 		S = (M - m) / (2 - M - m);
 	
 	if (S == 0)
 		H = 0;
 	else if (R == M)
 		H = 2 + b - g;
 	else if (G == M)
 		H = 4 + r - b;
 	else
 		H = 6 + g - r;
 	
 	H /= 6;
	
	return NewHSBColour (H, S, L);
 }

void ColourHSB (colour, h, s, b) Colour colour; float *h, *s, *b;
 {
 	*h = colour.hue;
 	*s = colour.saturation;
 	*b = colour.brightness;
 }

float Value (m, M, hue) float m, M, hue;
 {
 	if (hue < 0.0)
 		hue += 2 * PI;
 	if (hue < PI / 3)
 		return m + (M - m) * hue / (PI / 3);
 	else if (hue < PI)
 		return M;
 	else if (hue < 4 * PI / 3)
 		return m + (M - m) * (4 * PI / 3 - hue) / (PI / 3);
 	else
 		return m;
 }

void ColourRGB (colour, r, g, b) Colour colour; float *r, *g, *b;
 {
 	float H = colour.hue, S = colour.saturation, L = colour.brightness;
 	float m, M;
	
	if (L <= .5)
		M = L * (1 + S);
	else
		M = (L + S) - (L * S);
	m = 2 * L - M;
	H *= 2 * PI;
	*r = Value (m, M, H);
	*g = Value (m, M, H - 2 * PI / 3);
	*b = Value (m, M, H - 4 * PI / 3);
 }

float Brightness (colour) Colour colour;
 {
 	return colour.brightness;
 }


/* 
 * The RGB colour model and Hue Saturation Brightness/Lightness model
 * are derived from the paper:
 * 
 * 	"Colour Gamut Transform Pairs"
 *	by Alvy Ray Smith in Computer Graphics Volume 12 #3. August 1978
 * 
 * PostScript uses the NTSC video colour weights.
 */

#define RED_WEIGHT	0.3333	/* .3	*/
#define GREEN_WEIGHT	0.3333	/* .59	*/
#define BLUE_WEIGHT	0.3333	/* .11	*/

#define a0	120.0 * PI / 180		/* 156.58 * PI / 180 */
#define a1	120.0 * PI / 180		/* 115.68 * PI / 180 */

#define A0	0.0		/* -21.60 * PI / 180 */
#define A1	0.0		/* 14.98 * PI / 180 */
#define A2	0.0		/* 10.65 * PI / 180 */


/*
static int SetRGB (red, green, blue) Object red, green, blue;
 {
 	float R = BodyReal (red), G = BodyReal (green), B = BodyReal (blue);
 	float r, g, b, r_, g_, b_, d, x, rr, gg, bb, wr_, k0, k1, min, H, S, L;
 	
 	if (R < 0 || R > 1 || G < 0 || G > 1 || B < 0 || B > 1)
 		return Error (PRangeCheck);
 	L = R * RED_WEIGHT + G * GREEN_WEIGHT + B * BLUE_WEIGHT;
 	r_ = R/L; g_ = G/L; b_ = B/L;
 	r = RED_WEIGHT * r_; g = GREEN_WEIGHT * g_; b = BLUE_WEIGHT * b_;
 	rr = r - RED_WEIGHT; gg = g - GREEN_WEIGHT; bb = b - BLUE_WEIGHT;
 	min = (r_ < g_ ? r_ : g_); min = min < b_ ? min : b_;
 	S = 1 - min;
 	if (S != 0)
 	 {
 		k0 = sqrt (rr*rr + gg*gg + bb*bb);
 		wr_ = 1 - RED_WEIGHT;
 		d = wr_ * rr - GREEN_WEIGHT * gg + BLUE_WEIGHT * bb;
 		k1 = sqrt (wr_ * wr_ + GREEN_WEIGHT * GREEN_WEIGHT - BLUE_WEIGHT * BLUE_WEIGHT);
 		x = d / (k0 * k1);
 		H = PI / 2 - atan2 (x, sqrt (1 - x*x));
 		if (b_ > g_)
 			H = 2 * PI - H;
 		H /= 2 * PI;
 	 }
 	if (H < 0 || H > 1 || S < 0 || S > 1 || L < 0 || L > 1)
 		return Error (PRangeCheck);
	gstate->colour = NewHSLColour (H, S, L);
 	return TRUE;
 }

static int GetRGB ()
 {
 	float H = gstate->colour.hue, S = gstate->colour.saturation, L = gstate->colour.brightness;
 	float r, g, b, Wr = RED_WEIGHT, Wg = GREEN_WEIGHT, Wb = BLUE_WEIGHT;
 	
 	H *= 2 * PI;
 	if (0 <= H && H <= a0)
 	 {
 	 	H -= A0;
 	 	b = Wb * (1 - S);
 	 	r = Wr + Wb * S * cos (H) / cos (PI / 3 - H);
 	 	g = 1 - (r + b);
 	 }
 	else if (a0 <= H  && H <= (a0 + a1))
 	 {
 	 	H -= a0 + A1;
 	 	r = Wr * (1 - S);
 	 	g = Wg + Wr * S * cos (H) / cos (PI / 3 - H);
 	 	b = 1 - (r + g);
 	 }
 	else
 	 {
 	 	H -= a0 + a1 + A2;
 	 	g = Wg * (1 - S);
 	 	b = Wb + Wg * S * cos (H) / cos (PI / 3 - H);
 	 	r = 1 - (g + b);
 	 }
 	
 	VOID Push (OpStack, MakeReal (L* r / Wr));
 	VOID Push (OpStack, MakeReal (L* g / Wg));
 	VOID Push (OpStack, MakeReal (L* b / Wb));
	return TRUE;
 }

*/
