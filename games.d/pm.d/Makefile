#
#  pm $Header: /home/Vince/cvs/games.d/pm.d/Makefile,v 1.3 1987-06-14 17:44:32 vrs Exp $
#
#	Tunables:
#		PM_ROLL -	pathname of score file
#		PM_USER -	pathname of user log file
#		OS_VERS -	operating system version
#				(BSD29, BSD41, BSD42, BSD43)
#		SEND	-	pathname of binary export directory
#
#	[pm by Peter Costantinidis, Jr. @ University of California at Davis]
#
# Configuration section
#
ALL	= pm
PM_ROLL	= $(GAMLIB)/pm_roll
PM_USER	= $(GAMLIB)/pm_user
MODEL	= -i -F 2000
OPTIONS	= -DPM_ROLL=\"$(PM_ROLL)\" -DPM_USER=\"$(PM_USER)\" -DWIZARD_UID=0 -O
LINTARGS= -ahxc $(DEFS) -DLINT
include ../../CONFIG
#
# Files
#
HDRS	= pm.h
OBJS	= config.o init.o make_moves.o \
	  misc.o monsters.o msg.o pm.o random.o \
	  rip.o score.o screen.o timing.o warning.o #flsbuf.o _doprnt.o
SRCS1	= config.c flsbuf.c init.c make_moves.c \
	 misc.c monsters.c msg.c
SRCS2	= pm.c random.c rip.c score.c screen.c timing.c warning.c
SRCS	= $(SRCS1) $(SRCS2) _doprnt.c
#
# Misc.
#
PMDOC	= pm.6
REST	= READ_ME Makefile Makefile.b TODO $(HDRS) $(PMDOC) shar
EX1	= $(REST) $(SRCS2)
EX2	= $(SRCS1)

pm:		$(OBJS)
		$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

install:	$(ALL) $(GAMDIR) $(GAMLIB) $(MAN6)
		cp $(ALL) $(GAMDIR)
		cd $(GAMDIR); chown $(SAFEUID) $(ALL)
		cd $(GAMDIR); chmod 4711 $(ALL)
		touch $(PM_ROLL) $(PM_USER)
		chown $(SAFEUID) $(PM_ROLL) $(PM_USER)
		cp $(PMDOC) $(MAN6)
		cd $(MAN6); chmod 644 $(PMDOC)

clean:
		rm -f $(OBJS) $(ALL) make.out core

$(OBJS):	$(HDRS)

tags:		$(HDRS) $(SRCS)
		ctags -u $?
		sort tags -o tags

Print:		$(HDRS) $(SRCS)
		pr -l60 -w132 $? | lpr -d "-tpm"
		-touch Print

P_all:
		pr -l60 -w132 Tags $(HDRS) $(SRCS) | lpr -mc -d "-tpm"
		-touch Print

lint:
		lint $(LINTARGS) $(SRCS) -lcurses

count:
		wc $(HDRS) $(SRCS)

export:
		/bin/sh shar $(EX1) > pm1.shar
		/bin/sh shar $(EX2) > pm2.shar

crypt:		pm.tar
		crypt < pm.tar > pm.tar.c

Save:		$(HDRS) $(SRCS)
		cp $? Makefile xsave

Cxref:		$(SRCS) $(HDRS)
		cxref $(SRCS) $(HDRS) > $@
