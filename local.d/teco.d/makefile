##
##  TECO MAKEFILE
##
ALL	= teco$(EXE)
OPTIONS	= -O
MODEL	= -i -F 2000
include $(HOME)/CONFIG

OBJECTS	= te_data.o te_exec0.o te_exec1.o te_exec2.o te_main.o te_rdcmd.o \
	te_srch.o te_subs.o te_utils.o te_window.o

teco$(EXE):	$(OBJECTS) te_chario.o
	$(CC) -o $@ $(CFLAGS) $(OBJECTS) te_chario.o $(LIBS)

tex$(EXE):	$(OBJECTS) te_chx.o
	$(CC) -o $@ $(CFLAGS) $(OBJECTS) te_chx.o $(LIBS)

$(OBJECTS) te_chario.o te_chx.o:	te_defs.h
te_chx.o:				te_chario.c
	cp te_chario.c te_chx.c
	$(CC) -DDEBUG $(CFLAGS) -c te_chx.c
	rm -f te_chx.c

clean:
	rm -f $(OBJECTS) teco$(EXE) tex$(EXE) te_chario.o te_chx.o

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
