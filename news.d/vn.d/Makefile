# CFLAGS:
#	set -DJOBCONTROL if you have job control (BSD).
#	set -DSYSV -Dindex=strchr -Drindex=strrchr on Sytem V systems
#	set -Dregfree=free if you DON'T include reg.o (SYSV + some BSD)
#
# JOBCONTROL could be done as #ifndef SYSV, I suppose, but this clearly
# marks that particular difference.
#
# LIBS:
# should point to any extra libraries needed, such as termcap.  You
# may want to change this to use the curses termcap cover.  If you need
# to pull in another library to get regex / regcmp or strtok on non-SYSV
# systems, you may want to put that here
#
# EXTRAOBJS:
# may be used to include tmpnam.o, strtok.o, reg.o in the list.
#
# These objects implement SYSV utilities for BSD machines.
#
# strtok.o implements strtok() / strpbrk().  reg.o implements regex()/regcmp()
# on top of the BSD regular expression library (regex() allows multiple
# regular expressions).  tmpnam.o implements tmpnam() of course.
#
# If you have them, use your own regex/regcmp, because:
#
#	i) They should be faster than the reg.c code, which
#	recompiles the "current" ucb string every time you
#	switch regular expressions.
#
#	ii) I briefly checked out reg.c once, and it seemed to
#	work.  Our system has the "real" calls available, so I
#	run with those.  reg.c hasn't been used much by me, although
#	I've had nobody tell me it doesn't work.
#
# if you DON'T include reg.o, be sure you set -Dregfree=free in CFLAGS.
#
# As with regex, if you have a system strtok(), it is likely more efficient -
# string routines will often be done in assembler on a given machine.
#
# Even if you have it, you can use tmpnam.o anyhow.  This version will tailor
# the temp file name to vnXXXXXX, instead of a generic tmpXXXXXX.
#
# "vanilla" BSD:
#LIBS = -ltermcap
#EXTRAOBJS = tmpnam.o strtok.o reg.o
#CFLAGS = -O -DJOBCONTROL
#
# "vanilla" SYSV:
#LIBS = -ltermcap
#EXTRAOBJS = tmpnam.o
#CFLAGS = -O -DSYSV -Dregfree=free -Dindex=strchr -Drindex=strrchr
#
# BSD with strtok() / regex(), such as ULTRIX.  These are the rules
# used for our installation (rtech is a microvax running ULTRIX):
#LIBS = -ltermcap
#EXTRAOBJS = tmpnam.o
#CFLAGS = -O -DJOBCONTROL -Dregfree=free

# SERVEROBJS defines the object(s) for the vns_xxxx "server" interface.
#
#	std.o is the version for "standard" news, making use of the
#	users .newsrc file, and  resident articles / active file.
#
SERVEROBJS = std.o

# normal vn objects
#
VNOBJS=	hash.o envir_set.o pagefile.o reader.o storage.o sig_set.o term_set.o tty_set.o userlist.o vn.o vnglob.o digest.o strings.o session.o printex.o getch.o help.o newdisp.o stat.o svart.o

# This is to force you to read the makefile.  Once you have, comment this rule,
# and uncomment the "real" rule.  At the minimum, you will also have to
# uncomment one of the three sets of LIBS / EXTRAOBJS & CFLAGS definitions
# above.
#
vn:
	@echo "PLEASE READ THE MAKEFILE"
#vn:	$(VNOBJS) $(EXTRAOBJS) $(SERVEROBJS)
#	cc -o vn $(VNOBJS) $(EXTRAOBJS) $(SERVEROBJS) $(LIBS)
