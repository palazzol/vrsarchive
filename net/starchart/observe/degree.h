/*
 * degree.h
 * functions to deal with trigonometry in degrees
 *
 * Copyright (c) 1990 by Craig Counterman. All rights reserved.
 *
 * This software may be redistributed freely, not sold.
 * This copyright notice and disclaimer of warranty must remain
 *    unchanged. 
 *
 * No representation is made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty, to the extent permitted by applicable law.
 *
 */
/*
 * $Header: /home/Vince/cvs/net/starchart/observe/degree.h,v 1.1 1990-03-30 16:37:46 vrs Exp $
 */


#define DEG_TO_RAD 0.01745329251994329600
#define RAD_TO_DEG 57.29577951308232
#define DSIN(x) (sin((x)*DEG_TO_RAD))
#define DCOS(x) (cos((x)*DEG_TO_RAD))
#define DTAN(x) (tan((x)*DEG_TO_RAD))
#define DASIN(x) (asin(x)*RAD_TO_DEG)
#define DACOS(x) (acos(x)*RAD_TO_DEG)
#define DATAN(x) (atan(x)*RAD_TO_DEG)
#define DATAN2(x,y) (atan2(x,y)*RAD_TO_DEG)
