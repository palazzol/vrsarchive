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

extern void itpinit();
extern int itpregister();
extern void itpopen();
extern void itpstrategy();
extern void itpioctl();
extern void itpclose();

extern void hwopen();
extern void hwstrategy();
extern void hwioctl();
extern void hwclose();

extern int errno;
struct user u;

main()
{	int dev;
	struct buf buf;
	char buffer[BSIZE];
	FILE *fp;

	/*
	 *	Do driver init stuff
	 */
	dev = itpregister(HWDEV, hwopen, hwclose, hwstrategy, hwioctl);
	if (dev == -1)
		panic("itpregister failed\n");
	dri_printf("itpregister returned %d\n", dev);
	dev = makedev(TPMAJOR, dev);
	/*
	 *	Open the device for writing.
	*/
	itpopen(dev, 1);
	fp = fopen("/etc/termcap", "r");
	/*
	 *	Write some data to tape.
	*/
	while (u.u_count = fread(buffer, 1, sizeof(buffer), fp)) {
		if (u.u_count % 512)
			u.u_count += 512 - u.u_count%512;
		u.u_base = buffer;
		u.u_error = 0;
		itpwrite(dev);
		if (u.u_error) {
			errno = u.u_error;
			perror("tape write");
		}
	}
	itpclose(dev);
	return(0);
}
