#
# Makefile:	makefile for pm
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
SHELL	= /bin/sh
#
# Configuration section
#
DAEMON	= cron
GAMDIR	= /usr/games
GAMLIB	= $(GAMDIR)/lib
PM_ROLL	= $(GAMLIB)/pm_roll
PM_USER	= $(GAMLIB)/pm_user
USRMAN	= /usr/man
MANDIR	= $(USRMAN)/man6
OS_VERS = SYS3
DEFS	= -DPM_ROLL=\"$(PM_ROLL)\" -DPM_USER=\"$(PM_USER)\" \
	 -D$(OS_VERS) -DWIZARD_UID=0
#
# Flags section
#
CFLAGS	= -O $(DEFS)
LDFLAGS	= -i -F 2000
LINTARGS= -ahxc $(DEFS) -DLINT
LIBS	= -lcurses -ltermlib -lx
#
# Files
#
HDRS	= pm.h
OBJS	= config.o flsbuf.o init.o make_moves.o\
	 misc.o monsters.o msg.o pm.o random.o\
	 rip.o score.o screen.o timing.o warning.o _doprnt.o
SRCS1	= config.c flsbuf.c init.c make_moves.c\
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
		cc $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(GAMDIR) $(GAMLIB) $(USRMAN) $(MANDIR):
		mkdir $@
		chmod 755 $@

install:	pm $(GAMDIR) $(GAMLIB) $(PM_ROLL) $(PM_USER) $(USRMAN) $(MANDIR)
		cp pm $(GAMDIR)
		chown $(DAEMON) $(GAMDIR)/pm
		chmod 4711 $(GAMDIR)/pm
		cp $(PMDOC) $(MANDIR)
		cd $(MANDIR); chmod 644 $(PMDOC)

clean:
		rm -f $(OBJS) pm make.out core

$(OBJS):	$(HDRS)

tags:		$(HDRS) $(SRCS)
		ctags -u $?
		sort tags -o tags

$(PM_ROLL) $(PM_USER):
		touch $@
		chown $(DAEMON) $@

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
