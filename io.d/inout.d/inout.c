/*
 *	Low level I/O Port Driver
 *
 *	This device driver allows users to do low level I/O directly.
 *	It is intended that the user will develop drivers in user mode,
 *	then move them into the kernel if performance is a problem.
 *	This allows developement without the need of a dedicated rebootable
 *	system.
*/
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/inline.h"

/*ARGSUSED*/
inoutread(dev)
dev_t dev;
{	long data;

	switch (u.u_count) {
	case sizeof(char):
		data = inb(u.u_offset);
		break;
	case sizeof(short):
		data = inw(u.u_offset);
		break;
	case sizeof(long):
		data = inl(u.u_offset);
		break;
	default:
		u.u_error = EIO;
		return;
	}
	tenmicrosec();
	if (copyout((caddr_t)&data, u.u_base, u.u_count) == -1) {
		u.u_error = EFAULT;
		return;
	}
	u.u_count = 0;
}

/*ARGSUSED*/
inoutwrite(dev)
dev_t dev;
{	long data;

	if (copyin(u.u_base, (caddr_t)&data, u.u_count) == -1) {
		u.u_error = EFAULT;
		return;
	}
	switch (u.u_count) {
	case sizeof(char):
		outb(u.u_offset, data);
		break;
	case sizeof(short):
		outw(u.u_offset, data);
		break;
	case sizeof(long):
		outl(u.u_offset, data);
		break;
	default:
		u.u_error = EIO;
		return;
	}
	tenmicrosec();
	u.u_count = 0;
}
