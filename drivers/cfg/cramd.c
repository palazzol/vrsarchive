/*
 * Copyright (C) 1983, 1984  Intel Corp.
 * 
 * INTEL CORPORATION PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with Intel Corporation and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/*
 * RAM disk configuration file
 * not a lot to configure here
 */
#include "../h/param.h"
#include "../h/ramd.h"
#include "../h/buf.h"

#define	NUMRAMD	2
/* Number of ram disks to be configured */
int	numramd = NUMRAMD ;

struct ramdcfg ramdcfg[NUMRAMD] = {
				24,	0x5e000L,	RAMD_NOFS,
				112,	0xc0000L,	RAMD_FS,					};

struct 	ramddev 	ramddev[NUMRAMD];
struct 	buf		ramdtab[NUMRAMD];
struct 	buf      	ramd_buf[NUMRAMD];
