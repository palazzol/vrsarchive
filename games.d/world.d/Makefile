# Makefile for world and support programs for Unix
#
# The program has been tested and compiles and runs properly on the
# following systems:
# 
# 1)  VAX/VMS Digital CC compiler. No particular tricks are needed.
# 2)  IBM PC-AT Xenix 1.00. It seems to be necessary to use the large
#     memory model, although I'm not sure why since it runs on the
#     same machine under DOS in the small model.
# 3)  IBM PC under Microsoft C version 4.00. It works in the default
#     small memory model if you compile with the -Os switch. Note
#     that on this setup you MUST link the main program World with
#     binmode.obj, provided by Microsoft, for it to work right.
#     Vtxtcn and vcnvrt must NOT be linked with binmode.obj.
#     This compiler generates a few harmless warning messages.
#     You may need to use EXEMOD to increase the stack size.
# 4)  VAX/4.3bsd Unix. No particular tricks needed.
#
# NOTE: Edit helper.c to adjust the pathname for q1text.dat for
# your particular system.
#
ALL	= world$(EXE)
OPTIONS	= -O -DQ1TEXT=\"$(GAMLIB)/q1text.dat\"
MODEL	= -i
include $(HOME)/CONFIG
WHEADERS= arrays.h variab.h
WOBJS	= demons.o helper.o motion.o parser.o verbs1.o verbs2.o world.o

install: $(ALL) $(GAMDIR) $(GAMLIB)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp q1text.dat $(GAMLIB)
	cd $(GAMLIB); chmod 644 q1text.dat

convert:	vcnvrt vtxtcn vtext.dat
	@echo "creating data files..."
	./vtxtcn
	./vcnvrt
	touch convert

vcnvrt$(EXE):	vcnvrt.c
	$(CC) $(CFLAGS) -o $@ vcnvrt.c
	rm -f vcnvrt.o

vtxtcn$(EXE):	vtxtcn.c
	$(CC) $(CFLAGS) -o $@ vtxtcn.c
	rm -f vtxtcn.o

world$(EXE):	convert $(WOBJS) $(WHEADERS) 
	$(CC) $(CFLAGS) -o $@ $(WOBJS)

clean:
	rm -f $(ALL) $(WOBJS) convert vcnvrt vtxtcn
	rm -f *.inc q1text.dat

helper.o: Makefile
