# 
# $Header: /home/Vince/cvs/local.d/se.d/libchangetty/Makefile,v 1.2 1987-02-07 20:27:48 vrs Exp $
# 
# $Log: not supported by cvs2svn $
# Revision 1.2  86/07/11  15:23:24  osadr
# Changed to ensure the proper 'ar' program is used.
# 
# Revision 1.1  86/05/06  13:31:30  osadr
# Initial revision
# 
# 
# 
# makefile for libchangetty.a -- terminal resetting functions

TARGET= libchangetty.a
SRCS= changetty.c
OBJS= changetty.o
PRINTS= $(SRCS) makefile

CFLAGS= -O `cat ../flags`

$(TARGET): $(OBJS) ../flags
	/bin/ar ruv $(TARGET) $(OBJS)
	if test -x /usr/bin/ranlib; then /usr/bin/ranlib $(TARGET); fi
	if test -x /bin/ranlib; then /bin/ranlib $(TARGET); fi

clean:
	rm -fr $(OBJS)

clobber: clean
	rm -fr $(TARGET) print2

print:
	prt $(PRINTS) | lpr -b 'tty lib'
	touch print2

print2: $(PRINTS)
	prt $? | lpr -b 'new tty lib'
	touch print2
