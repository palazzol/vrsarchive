/*
 * TREK73: ships.c
 *
 * Global Ship Defs for TREK73
 *
 */

#include "externs.h"

/*
 * starting stats for the ships depending on the type
 */

struct ship_stat stats[] = {
{ "DN", 0, 6, 8, 8,  10, 2.,   1.5,  15., 200, 300, 600, 450, 5., 4.,  2, 4,
   135, 225, 125, 235, 4, 4},
{ "CA", 1, 4, 6, 9,  11, 1.,   0.75, 10., 150, 200, 450, 350, 3., 2.,  5, 2,
   135, 225, 125, 235, 4, 4},
{ "CL", 2, 4, 4, 9,  11, 0.75, 0.5,  10., 125, 175, 350, 250, 3., 2.,  6, 2,
   150, 210, 140, 220, 4, 4},
{ "DD", 3, 2, 4, 10, 12, 0.5,  0.5,  8.,  100, 150, 200, 150, 2., 1.5, 7, 1,
   160, 200, 150, 210, 4, 4}
};
