*
*	%Z% %M% %I% %D% %Q%
*
*	This Module contains Proprietary Information of Microsoft
*	Corporation and AT&T, and should be treated as Confidential.
*
*
* THIS FILE CONTAINS CODE WHICH IS SPECIFIC TO THE
* INTEL 286/310 COMPUTER AND MAY REQUIRE MODIFICATION
* WHEN ADAPTING XENIX TO NEW HARDWARE.
*
*
*	Devices
*
scsi    1
i530    1
lp      0
ramd    0
debug   0
root    scsi 1
pipe    scsi 1
swap    scsi 2 1 1188
*
*	Local parameters
*
timezone (8*60)
daylight 1
cmask	0
*
