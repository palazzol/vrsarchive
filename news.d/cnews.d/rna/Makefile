DEFINES=-Dstrchr=index -Dstrrchr=rindex
CFLAGS=-O $(DEFINES) # -s is a stupid idea
INSTALL=:# install bigpdp
NFLAG =
I=/usr/include
C=/lib/libc.a
LIBS=lib/*.a
LIBDIR=/usr/lib/news
BINDIR=/bin
COMMANDS = postnews uurec readnews uusend expire postgroup
FILES = help
LCOMMANDS = postnews.lint uurec.lint readnews.lint uusend.lint expire.lint
PFILES = header.c postnews.c funcs.c active.c history.c maketime.c mtempnam.c
RFILES = header.c readnews.c funcs.c active.c newsrc.c history.c maketime.c
ROFILES= header.o readnews.o funcs.o active.o newsrc.o history.o maketime.o
EFILES = expire.c funcs.c active.c
LINT = lint -ha $(DEFINES)

.c.lint:
	$(LINT) $< > $@

.c:
	$(CC) $(CFLAGS) $*.c $(NFLAG) -o $(@F)
	$(INSTALL) $* bin 711 $@

.sh:
	$(INSTALL) -c $*.sh bin 755 $@

all: defs.h $(LIBDIR) $(COMMANDS) $(FILES)

defs.h:	at.h $C
	touch defs.h

$(LIBDIR):
	mkdir $(LIBDIR)
	chown news $(LIBDIR)
	chmod 755 $(LIBDIR)

lint: $(LCOMMANDS)

help: $(LIBDIR)/help
$(LIBDIR)/help: news.help
	$(INSTALL) -c news.help news 644 $(LIBDIR)/help

postnews: $(BINDIR)/postnews
$(BINDIR)/postnews: $(PFILES) defs.h
	$(CC) $(CFLAGS) $(PFILES) -o postnews
	$(INSTALL) - postnews news 6711 $(BINDIR)/postnews

postgroup: $(BINDIR)/postgroup
$(BINDIR)/postgroup: postgroup.sh

uurec: $(LIBDIR)/uurec
$(LIBDIR)/uurec: uurec.c defs.h

readnews: $(BINDIR)/readnews
$(BINDIR)/readnews: $(ROFILES) defs.h
	$(CC) $(CFLAGS) $(ROFILES) $(LIBS) -o readnews
	$(INSTALL) - readnews bin 711 $(BINDIR)/readnews $(BINDIR)/news
$(ROFILES): defs.h

uusend: $(LIBDIR)/uusend
$(LIBDIR)/uusend: defs.h uusend.c

expire: $(LIBDIR)/expire
$(LIBDIR)/expire: $(EFILES) defs.h
	$(CC) $(CFLAGS) $(EFILES) $(NFLAG) -o expire
	$(INSTALL) expire news 700 $(LIBDIR)/expire

postnews.lint: $(PFILES)
	$(LINT) $(PFILES) > postnews.lint

readnews.lint: $(RFILES)
	$(LINT) $(RFILES) > readnews.lint

expire.lint: $(EFILES)
	$(LINT) $(EFILES) > expire.lint

uurec.lint: uurec.c defs.h
uusend.lint: uusend.c defs.h

.FINISH:
	rm -s *.o
clean:
	rm -f *.o core a.out readnews
