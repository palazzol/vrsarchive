/* Header for Athena widgets starchart */
/*
 *
 * $Header: /home/Vince/cvs/net/starchart/starchart/starXaw.h,v 1.1 1990-03-30 16:39:04 vrs Exp $
 * 
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

typedef struct {
  char *blabel;
  void (*func)();
} Button_set;


/* VMS linker is not case sensitive */
#ifdef VMS
#define Edit_mapwins Ed_mwin_f
#define edit_mapwins e_mapw_bool
#endif
