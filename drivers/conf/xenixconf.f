*
*	@(#) conf/xenixconf.f 1.1 84/05/07 Copyright Microsoft Corporation, 1983
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
ramd	1
lp      0
sm	0
debug   0
root    ramd 1
pipe    ramd 1
swap    ramd 0 1 300
*
*	Local parameters
*
timezone (8*60)
daylight 1
cmask	0
*
*	Tunable Parameters
*
*	Dont change them unless you're sure you know what you're doing!
*
maxprocmem	376
