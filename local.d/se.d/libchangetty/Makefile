# 
# $Header: /home/Vince/cvs/local.d/se.d/libchangetty/Makefile,v 1.3 1987-02-13 10:07:49 vrs Exp $
# 
# $Log: not supported by cvs2svn $
# Version 1.2  87/02/07  20:27:48  vrs
# Look for ranlib in both /bin and /usr/bin
# 
# Revision 1.2  86/07/11  15:23:24  osadr
# Changed to ensure the proper 'ar' program is used.
# 
# Revision 1.1  86/05/06  13:31:30  osadr
# Initial revision
# 
# 
# 
# makefile for libchangetty.a -- terminal resetting functions
ALL	= libchangetty.a
MODEL	= -M2l
OPTIONS	= -O `cat ../flags`
include ../../../CONFIG
SRCS	= changetty.c
OBJS	= changetty.o
PRINTS	= $(SRCS) Makefile


$(ALL): $(OBJS) ../flags
	/bin/ar ruv $(ALL) $(OBJS)
	if test -x /usr/bin/ranlib; then /usr/bin/ranlib $(ALL); fi
	if test -x /bin/ranlib; then /bin/ranlib $(ALL); fi

install: ;

clean:
	rm -fr $(OBJS) $(ALL)

print:
	prt $(PRINTS) | lpr -b 'tty lib'

print2: $(PRINTS)
	prt $? | lpr -b 'new tty lib'
