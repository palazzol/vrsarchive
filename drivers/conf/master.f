*
*	%Z% %M% %I% %D% %Q%
*
*	This Module contains Proprietary Information of Microsoft
*	Corporation, Intel Corporation, and AT&T,
*	and should be treated as Confidential.
*
*
*
* The following devices are those that can be specified in the system
* description file.  The name specified must agree with the name shown.
*
* The first twelve entries in both the "bdevsw" and the "cdevsw" are
* reserved for use as block devices. The last four of these entries
* are reserved for additional Intel devices and customer block devices.
* All block devices have the same "bdevsw" and "cdevsw" number.
* The "cmaj" number 1 is reserved for use by the memory driver.
*
*name vsiz msk  typ  hndlr na bmaj cmaj #  na  vec1 vec2 vec3 vec4
*  1    2    3   4       5  6   7   8   9  10   11    12    13  14
*-----------------------------------------------------------------
scsi    1  0137 014   scsi  0   0   0   2  -1 0001     0     0  0a
i218    1  0137 014   i218  0   4   4   2  -1 0005     0     0  0a
ramd    0  0136 054   ramd  0   5   5   1  -1    0     0     0  0a
xlog    1  0137 014   xlog  0   6   6   2  -1 0005     0     0  0a
sm      0   036 010     sm  0   7   0   1  -1    0     0     0  0a
*
* The next twelve entries in the "cdevsw" are reserved for character
* devices. The "cmaj" number 12 is reserved for use by the tty driver.
*
lp      1  0137 004   lp    0   0  13   1  -1 0107     0     0  0a
i530    1  0137 004   i530  0   0  14   1  -1 0006     0     0  0a
*
* These are Intel devices that use an interrupt vector but do not
* have any "bdevsw" or "cdevsw" entry.
*
debug   1     0   0    dbg  0   0   0   1  -1 0001     0     0  0a
slave7	1     0   0    sl   0   0   0   1  -1 0007     0     0  0a
*
* The following devices must not be specified in the system description
* file (xenixconf).  These are pseudo drivers and the clock driver.
*
memory  0   06  0324    mm  0  -1   1   1   0    0     0     0  0a
tty     0   027 0324    sy  0  -1   12  1   0    0     0     0  0a
clock   1   000 0321    .   0  -1  -1   1   0    0     0     0  0a
$$$
*
* The following are the line discipline table entries.
*
tty     ttopen	ttclose	ttread	ttwrite	ttioctl	ttin	ttout	nulldev
$$$$$
*
* The following entries form the alias table.
*
scsi    disk
i530    serial
sm      sim
$$$
*
* The following entries form the tunable parameter table.
*
buffers NBUF      1
sabufs  NSABUF    8
hashbuf NHBUF   128
inodes  NINODE  100
files   NFILE   100
mounts  NMOUNT    6
coremap CMAPSIZ  (NPROC*2)
swapmap SMAPSIZ  (NPROC*2)
calls   NCALL  25
procs   NPROC  50
texts   NTEXT  40
clists  NCLIST 120
locks   NFLOCKS 50
maxproc MAXUPRC 15
timezone TIMEZONE (8*60)
pages   NCOREL 0
daylight DSTFLAG 1
cmask   CMASK  0
maxprocmem MAXMEM 0
shdata	NSDSEGS	25
maxbuf	MAXBUF	192
ttys MAXCBUFS 4
*
* The following is the default configuration for ulimit
* The number is the number of 512 byte blocks which can be allocated.
* 8192 * 512 = 4MB max. file size
*
ulimcfg ULIMITCFG 8192
