SHELL	= /bin/sh
BIN	= $(DESTDIR)/usr/local
LIB	= $(BIN)/lib
ALL	= $(BIN) $(LIB) $(BIN)/ash $(LIB)/ash.help $(LIB)/ash.inst

install: $(ALL)
clean:

$(BIN) $(LIB):
		mkdir $@
		chmod 755 $@

$(BIN)/ash:	ash.sh
			cp ash.sh $@
			chmod 755 $@

$(LIB)/ash.help: ash.help
		cp ash.help $@
		chmod 644 $@

$(LIB)/ash.inst: ash.inst
		cp ash.inst $@
		chmod 644 $@
