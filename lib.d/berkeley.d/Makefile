#
#	%W% %G%
#
ALL	= libberkeley.a # Slibberkeley.a Mlibberkeley.a Llibberkeley.a
OPTIONS	= -I./h -DPORT=\"/tmp/sockXXXXXX\"
include $(HOME)/CONFIG
BRKSYS	= $(INCDIR)/sys
HFILES	= h/*.h h/sys/*.h
#
OBJS	= _close.o _common.o _read.o _write.o _sock_alloc.o accept.o \
	  alloca.o bcmp.o bcopy.o bfill.o bind.o bmove.o bzero.o close.o \
	  closedir.o connect.o doprnt.o errno.o exit.o ffs.o ftruncate.o \
	  getdtab.o getsockname.o getsockopt.o listen.o opendir.o random.o \
	  recv.o recvfrom.o recvmsg.o read.o readdir.o seekdir.o select.o \
	  send.o sendmsg.o sendto.o shutdown.o socket.o socketpair.o \
	  sockopt.o telldir.o vfork.o write.o
SOBJS	= $(OBJS:.o=.Sm)
MOBJS	= $(OBJS:.o=.Md)
LOBJS	= $(OBJS:.o=.Lg)
#
#	Sure wish I could figure out how to force 'make' to compute this:
#
LIB	= libberkeley.a(_close.o) \
	  libberkeley.a(_common.o) \
	  libberkeley.a(_read.o) \
	  libberkeley.a(_write.o) \
	  libberkeley.a(_sock_alloc.o) \
	  libberkeley.a(accept.o) \
	  libberkeley.a(alloca.o) \
	  libberkeley.a(bcmp.o) \
	  libberkeley.a(bcopy.o) \
	  libberkeley.a(bfill.o) \
	  libberkeley.a(bind.o) \
	  libberkeley.a(bmove.o) \
	  libberkeley.a(bzero.o) \
	  libberkeley.a(close.o) \
	  libberkeley.a(closedir.o) \
	  libberkeley.a(connect.o) \
	  libberkeley.a(doprnt.o) \
	  libberkeley.a(errno.o) \
	  libberkeley.a(exit.o) \
	  libberkeley.a(ffs.o) \
	  libberkeley.a(ftruncate.o) \
	  libberkeley.a(getdtab.o) \
	  libberkeley.a(getsockname.o) \
	  libberkeley.a(getsockopt.o) \
	  libberkeley.a(listen.o) \
	  libberkeley.a(opendir.o) \
	  libberkeley.a(random.o) \
	  libberkeley.a(recv.o) \
	  libberkeley.a(recvfrom.o) \
	  libberkeley.a(recvmsg.o) \
	  libberkeley.a(read.o) \
	  libberkeley.a(readdir.o) \
	  libberkeley.a(seekdir.o) \
	  libberkeley.a(select.o) \
	  libberkeley.a(send.o) \
	  libberkeley.a(sendmsg.o) \
	  libberkeley.a(sendto.o) \
	  libberkeley.a(shutdown.o) \
	  libberkeley.a(socket.o) \
	  libberkeley.a(socketpair.o) \
	  libberkeley.a(sockopt.o) \
	  libberkeley.a(telldir.o) \
	  libberkeley.a(vfork.o) \
	  libberkeley.a(write.o)
SLIB	= Slibberkeley.a(_close.Sm) \
	  Slibberkeley.a(_common.Sm) \
	  Slibberkeley.a(_read.Sm) \
	  Slibberkeley.a(_write.Sm) \
	  Slibberkeley.a(_sock_alloc.Sm) \
	  Slibberkeley.a(accept.Sm) \
	  Slibberkeley.a(alloca.Sm) \
	  Slibberkeley.a(bcmp.Sm) \
	  Slibberkeley.a(bcopy.Sm) \
	  Slibberkeley.a(bfill.Sm) \
	  Slibberkeley.a(bind.Sm) \
	  Slibberkeley.a(bmove.Sm) \
	  Slibberkeley.a(bzero.Sm) \
	  Slibberkeley.a(close.Sm) \
	  Slibberkeley.a(closedir.Sm) \
	  Slibberkeley.a(connect.Sm) \
	  Slibberkeley.a(doprnt.Sm) \
	  Slibberkeley.a(errno.Sm) \
	  Slibberkeley.a(exit.Sm) \
	  Slibberkeley.a(ffs.Sm) \
	  Slibberkeley.a(ftruncate.Sm) \
	  Slibberkeley.a(getdtab.Sm) \
	  Slibberkeley.a(getsockname.Sm) \
	  Slibberkeley.a(getsockopt.Sm) \
	  Slibberkeley.a(listen.Sm) \
	  Slibberkeley.a(opendir.Sm) \
	  Slibberkeley.a(random.Sm) \
	  Slibberkeley.a(recv.Sm) \
	  Slibberkeley.a(recvfrom.Sm) \
	  Slibberkeley.a(recvmsg.Sm) \
	  Slibberkeley.a(read.Sm) \
	  Slibberkeley.a(readdir.Sm) \
	  Slibberkeley.a(seekdir.Sm) \
	  Slibberkeley.a(select.Sm) \
	  Slibberkeley.a(send.Sm) \
	  Slibberkeley.a(sendmsg.Sm) \
	  Slibberkeley.a(sendto.Sm) \
	  Slibberkeley.a(shutdown.Sm) \
	  Slibberkeley.a(socket.Sm) \
	  Slibberkeley.a(socketpair.Sm) \
	  Slibberkeley.a(sockopt.Sm) \
	  Slibberkeley.a(telldir.Sm) \
	  Slibberkeley.a(vfork.Sm) \
	  Slibberkeley.a(write.Sm)
MLIB	= Mlibberkeley.a(_close.M) \
	  Mlibberkeley.a(_common.M) \
	  Mlibberkeley.a(_read.M) \
	  Mlibberkeley.a(_write.M) \
	  Mlibberkeley.a(_sock_alloc.M) \
	  Mlibberkeley.a(accept.M) \
	  Mlibberkeley.a(alloca.M) \
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
	  Llibberkeley.a(alloca.L) \
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
.SUFFIXES:	.a .o .s .c .Sm .Md .Lg
.PRECIOUS:	$(ALL)

.c.Sm:
	$(CC) -c $(CFLAGS) -Ms $<
	mv $*.o $@

.c.Md:
	$(CC) -c $(CFLAGS) -Mm $<
	mv $*.o $@

.c.Lg:
	$(CC) -c $(CFLAGS) -Ml $<
	mv $*.o $@

.s.Sm:
	$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s
	$(CC) -Ml -o$@ tmp.s
	rm tmp.s

.s.Md:
	$(CC) $(CFLAGS) -Mm -E $*.s | sed '/^#/d' >tmp.s
	$(CC) -Ml -o$@ tmp.s
	rm tmp.s

.s.Lg:
	$(CC) $(CFLAGS) -Ml -E $*.s | sed '/^#/d' >tmp.s
	$(CC) -Ml -o$@ tmp.s
	rm tmp.s

.c.a:
	make $%
	ar rv $@ $%
	rm -f $%

.s.a:
	make $%
	ar rv $@ $%
	rm -f $%

libberkeley.a: $(LIB)
	$(RANLIB) $@

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
	rm -f $(SOBJS) $(MOBJS) $(LOBJS) $(ALL) make.out

#
#	Build the works and install it.
#
install:	$(ALL) $(CCLIB) $(INCDIR) $(BRKSYS)
		cp $(ALL) $(CCLIB)
		cd $(CCLIB); chmod 644 $(ALL)
		for i in $(ALL); do $(RANLIB) $(CCLIB)/$$i; done
		cp $(ALL) $(LIBDIR)
		cd $(LIBDIR); chmod 644 $(ALL)
		for i in $(ALL); do $(RANLIB) $(LIBDIR)/$$i; done
		cp h/*.h $(INCDIR)
		chmod 644 $(INCDIR)/*.h
		cp h/sys/*.h $(BRKSYS)
		chmod 644 $(BRKSYS)/*.h

$(BRKSYS):
		mkdir $@
		chmod 755 $@
