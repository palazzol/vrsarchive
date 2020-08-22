/*
 * date.h
 * datelib fuction declarations
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
 * $Header: /home/Vince/cvs/net/starchart/observe/date.h,v 1.1 1990-03-30 16:37:44 vrs Exp $
 */


void cal_to_jd();
void cal_to_str();
void jd_to_cal();
void jd_to_str();
void str_to_jd();
void str_to_cal();

int tr_mname();

/* put here for VMS and pure-ANSI systems */
double atof();
