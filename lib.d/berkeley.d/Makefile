#
#	%W% %G%
#
SHELL	= /bin/sh
USRLCL	= ..
USRBRK	= $(USRLCL)/berkeley
USRINCL	= $(USRBRK)/include
INCLSYS	= $(USRINCL)/sys
USRLIB	= $(USRBRK)/lib
HFILES	= h/*.h h/sys/*.h
CFLAGS	= -I./h
#
#
#	Use a no-op for $(RANLIB) if you are running on an AT&T UNIX system.
#	UNIX is a trademark of AT&T.  Of course, you'll need to recode the
#	shared data calls.
#
RANLIB	= ranlib
SACRED	= Slibberkeley.a Mlibberkeley.a Llibberkeley.a
#
OBJS	= _close.o _common.o _read.o _write.o _sock_alloc.o \
	  accept.o bcmp.o bcopy.o bfill.o bind.o bmove.o bzero.o close.o \
	  closedir.o connect.o doprnt.o errno.o exit.o ffs.o ftruncate.o \
	  getdtab.o getsockname.o getsockopt.o listen.o opendir.o random.o \
	  recv.o recvfrom.o recvmsg.o read.o readdir.o seekdir.o select.o \
	  send.o sendmsg.o sendto.o shutdown.o socket.o socketpair.o \
	  sockopt.o telldir.o vfork.o write.o
SOBJS	= $(OBJS:.o=.S)
MOBJS	= $(OBJS:.o=.M)
LOBJS	= $(OBJS:.o=.L)
#
#	Sure wish I could figure out how to force 'make' to compute this:
#
SLIB	= Slibberkeley.a(_close.S) \
	  Slibberkeley.a(_common.S) \
	  Slibberkeley.a(_read.S) \
	  Slibberkeley.a(_write.S) \
	  Slibberkeley.a(_sock_alloc.S) \
	  Slibberkeley.a(accept.S) \
	  Slibberkeley.a(bcmp.S) \
	  Slibberkeley.a(bcopy.S) \
	  Slibberkeley.a(bfill.S) \
	  Slibberkeley.a(bind.S) \
	  Slibberkeley.a(bmove.S) \
	  Slibberkeley.a(bzero.S) \
	  Slibberkeley.a(close.S) \
	  Slibberkeley.a(closedir.S) \
	  Slibberkeley.a(connect.S) \
	  Slibberkeley.a(doprnt.S) \
	  Slibberkeley.a(errno.S) \
	  Slibberkeley.a(exit.S) \
	  Slibberkeley.a(ffs.S) \
	  Slibberkeley.a(ftruncate.S) \
	  Slibberkeley.a(getdtab.S) \
	  Slibberkeley.a(getsockname.S) \
	  Slibberkeley.a(getsockopt.S) \
	  Slibberkeley.a(listen.S) \
	  Slibberkeley.a(opendir.S) \
	  Slibberkeley.a(random.S) \
	  Slibberkeley.a(recv.S) \
	  Slibberkeley.a(recvfrom.S) \
	  Slibberkeley.a(recvmsg.S) \
	  Slibberkeley.a(read.S) \
	  Slibberkeley.a(readdir.S) \
	  Slibberkeley.a(seekdir.S) \
	  Slibberkeley.a(select.S) \
	  Slibberkeley.a(send.S) \
	  Slibberkeley.a(sendmsg.S) \
	  Slibberkeley.a(sendto.S) \
	  Slibberkeley.a(shutdown.S) \
	  Slibberkeley.a(socket.S) \
	  Slibberkeley.a(socketpair.S) \
	  Slibberkeley.a(sockopt.S) \
	  Slibberkeley.a(telldir.S) \
	  Slibberkeley.a(vfork.S) \
	  Slibberkeley.a(write.S)
MLIB	= Mlibberkeley.a(_close.M) \
	  Mlibberkeley.a(_common.M) \
	  Mlibberkeley.a(_read.M) \
	  Mlibberkeley.a(_write.M) \
	  Mlibberkeley.a(_sock_alloc.M) \
	  Mlibberkeley.a(accept.M) \
	  Mlibberkeley.a(bcmp.M) \
	  Mlibberkeley.a(bcopy.M) \
	  Mlibberkeley.a(bfill.M) \
	  Mlibberkeley.a(bind.M) \
	  Mlibberkeley.a(bmove.M) \
	  Mlibberkeley.a(bzero.M) \
	  Mlibberkeley.a(close.M) \
	  Mlibberkeley.a(closedir.M) \
	  Mlibberkeley.a(connect.M) \
	  Mlibberkeley.a(doprnt.M) \
	  Mlibberkeley.a(errno.M) \
	  Mlibberkeley.a(exit.M) \
	  Mlibberkeley.a(ffs.M) \
	  Mlibberkeley.a(ftruncate.M) \
	  Mlibberkeley.a(getdtab.M) \
	  Mlibberkeley.a(getsockname.M) \
	  Mlibberkeley.a(getsockopt.M) \
	  Mlibberkeley.a(listen.M) \
	  Mlibberkeley.a(opendir.M) \
	  Mlibberkeley.a(random.M) \
	  Mlibberkeley.a(recv.M) \
	  Mlibberkeley.a(recvfrom.M) \
	  Mlibberkeley.a(recvmsg.M) \
	  Mlibberkeley.a(read.M) \
	  Mlibberkeley.a(readdir.M) \
	  Mlibberkeley.a(seekdir.M) \
	  Mlibberkeley.a(select.M) \
	  Mlibberkeley.a(send.M) \
	  Mlibberkeley.a(sendmsg.M) \
	  Mlibberkeley.a(sendto.M) \
	  Mlibberkeley.a(shutdown.M) \
	  Mlibberkeley.a(socket.M) \
	  Mlibberkeley.a(socketpair.M) \
	  Mlibberkeley.a(sockopt.M) \
	  Mlibberkeley.a(telldir.M) \
	  Mlibberkeley.a(vfork.M) \
	  Mlibberkeley.a(write.M)
LLIB	= Llibberkeley.a(_close.L) \
	  Llibberkeley.a(_common.L) \
	  Llibberkeley.a(_read.L) \
	  Llibberkeley.a(_write.L) \
	  Llibberkeley.a(_sock_alloc.L) \
	  Llibberkeley.a(accept.L) \
	  Llibberkeley.a(bcmp.L) \
	  Llibberkeley.a(bcopy.L) \
	  Llibberkeley.a(bfill.L) \
	  Llibberkeley.a(bind.L) \
	  Llibberkeley.a(bmove.L) \
	  Llibberkeley.a(bzero.L) \
	  Llibberkeley.a(close.L) \
	  Llibberkeley.a(closedir.L) \
	  Llibberkeley.a(connect.L) \
	  Llibberkeley.a(doprnt.L) \
	  Llibberkeley.a(errno.L) \
	  Llibberkeley.a(exit.L) \
	  Llibberkeley.a(ffs.L) \
	  Llibberkeley.a(ftruncate.L) \
	  Llibberkeley.a(getdtab.L) \
	  Llibberkeley.a(getsockname.L) \
	  Llibberkeley.a(getsockopt.L) \
	  Llibberkeley.a(listen.L) \
	  Llibberkeley.a(opendir.L) \
	  Llibberkeley.a(random.L) \
	  Llibberkeley.a(recv.L) \
	  Llibberkeley.a(recvfrom.L) \
	  Llibberkeley.a(recvmsg.L) \
	  Llibberkeley.a(read.L) \
	  Llibberkeley.a(readdir.L) \
	  Llibberkeley.a(seekdir.L) \
	  Llibberkeley.a(select.L) \
	  Llibberkeley.a(send.L) \
	  Llibberkeley.a(sendmsg.L) \
	  Llibberkeley.a(sendto.L) \
	  Llibberkeley.a(shutdown.L) \
	  Llibberkeley.a(socket.L) \
	  Llibberkeley.a(socketpair.L) \
	  Llibberkeley.a(sockopt.L) \
	  Llibberkeley.a(telldir.L) \
	  Llibberkeley.a(vfork.L) \
	  Llibberkeley.a(write.L)
#
#	Define how to make model dependant objects.  Objects with .S extensions
#	are small model, .M are Middle Model, and .L are large model.
#
.SUFFIXES:
.SUFFIXES:	.a .S .M .L .o .s .c
.PRECIOUS:	$(SACRED)

.c.S:
	$(CC) -c $(CFLAGS) -Ms $<
	mv $*.o $@

.c.M:
	$(CC) -c $(CFLAGS) -Mm $<
	mv $*.o $@

.c.L:
	$(CC) -c $(CFLAGS) -Ml $<
	mv $*.o $@

.s.S:
	$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s
	masm -Ml -o$@ tmp.s
	rm tmp.s

.s.M:
	$(CC) $(CFLAGS) -Mm -E $*.s | sed '/^#/d' >tmp.s
	masm -Ml -o$@ tmp.s
	rm tmp.s

.s.L:
	$(CC) $(CFLAGS) -Ml -E $*.s | sed '/^#/d' >tmp.s
	masm -Ml -o$@ tmp.s
	rm tmp.s

.c.a:
	make $%
	ar rv $@ $%
	rm -f $%

.s.a:
	make $%
	ar rv $@ $%
	rm -f $%

all:	$(SACRED)

Slibberkeley.a: $(SLIB)
	$(RANLIB) $@

Mlibberkeley.a: $(MLIB)
	$(RANLIB) $@

Llibberkeley.a: $(LLIB)
	$(RANLIB) $@

$(SOBJS): $(HFILES)
$(MOBJS): $(HFILES)
$(LOBJS): $(HFILES)

clean:
#	rm -f $(SOBJS) $(MOBJS) $(LOBJS) $(SACRED) make.out

#
#	Build the works and install it.
#
install:	$(SACRED) # $(USRLCL) $(USRBRK) $(USRLIB) $(USRINCL) $(INCLSYS)
#		cp $(SACRED) $(USRLIB)
#		cd $(USRLIB); chmod 644 $(SACRED)
#		for i in $(SACRED); do $(RANLIB) $(USRLIB)/$$i; done
#		cp h/*.h $(USRINCL)
#		chmod 644 $(USRINCL)/*.h
#		cp h/sys/*.h $(INCLSYS)
#		chmod 644 $(INCLSYS)/*.h

$(USRLCL) $(USRBRK) $(USRLIB) $(USRINCL) $(INCLSYS):
		mkdir $@
		chmod 755 $@
