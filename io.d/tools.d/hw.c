/*
 *	Pretend to be a kernel and excercise the itp driver.
 */
#include "sys/types.h"
#include "sys/param.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/inode.h"
#include "sys/proc.h"
#include "sys/sysmacros.h"
#include "sys/buf.h"
#include "sys/errno.h"
#include "sys/conf.h"
#include "sys/fs/s5dir.h"
#include "sys/systm.h"
#include "sys/tss.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/iobuf.h"
#include "sys/cmn_err.h"
#include "sys/elog.h"
#include "sys/tape.h"
#include "sys/itp.h"
#include <stdio.h>

#define HWMAJOR	42
#define HWMINOR	13
#define TPMAJOR	37
#define HWDEV	makedev(HWMAJOR, HWMINOR)

FILE *tape;

void
hwopen(dev, flag)
dev_t dev;
int flag;
{
	if (dev != HWDEV)
		panic("hwopen: wrong dev\n");
	dri_printf("hwopen: flag == %d\n", flag);
	tape = fopen("tape", flag? "w" : "r");
	if (tape == NULL)
		panic("hwopen: Can't open media\n");
}

void
hwclose(dev)
dev_t dev;
{
	if (dev != HWDEV)
		panic("hwclose: wrong dev\n");
	if (tape == 0)
		panic("hwclose: media not open\n");
	fclose(tape);
	dri_printf("hwclose: closed\n");
}

void
hwstrategy(bp)
struct buf *bp;
{
	if (bp->b_dev != HWDEV)
		panic("hwstrategy: wrong dev\n");
	if ((bp->b_flags & B_PHYS) == B_PHYS)
		cmn_err(CE_NOTE, "hwstrategy: called straight from physio??\n");
	if ((bp->b_flags & B_BUSY) != B_BUSY)
		panic("hwstrategy: buffer not busy??\n");
	if ((bp->b_flags & B_READ) != B_READ)
		bp->b_resid = fwrite(bp->b_un.b_addr, 1, bp->b_bcount, tape);
	else
		bp->b_resid = fread(bp->b_un.b_addr, 1, bp->b_bcount, tape);
	bp->b_resid = bp->b_bcount - bp->b_resid;
	dri_printf("addr == %x, count == %d\n", bp->b_un.b_addr, bp->b_bcount);
	iodone(bp);
}

hwioctl(dev, cmd, cmdarg, flag)
dev_t dev;
int cmd;
caddr_t cmdarg;
int flag;
{
	if (dev != HWDEV)
		panic("hwioctl: wrong dev\n");
	switch (cmd) {
	case T_ERASE:
		fprintf(tape, "*** Erase Tape ***\n");
		break;
	case T_RWD:
	case T_LOAD:
	case T_UNLOAD:
		fprintf(tape, "*** Rewind Tape ***\n");
		break;
	case T_RETENSION:
		fprintf(tape, "*** Retension Tape ***\n");
		break;
	case T_WRFILEM:
		fprintf(tape, "*** File Mark ***\n");
		break;
	case T_RST:
		fprintf(tape, "*** Reset Drive ***\n");
		break;
	case T_RDSTAT:
		fprintf(tape, "*** Read Status ***\n");
		break;
	case T_SFF:
		fprintf(tape, "*** Skip %d files ***\n", (int)cmdarg);
		break;
	case T_SBF:
		fprintf(tape, "*** Skip %d Blocks ***\n", (int)cmdarg);
		break;
	default:
		panic("hwioctl: unknown request %x\n", cmd);
	}
}
