#
# Something quick and dirty... you must put things where you want...
#
include $(HOME)/CONFIG

all:	src/munge/munge$(EXE) src/adv/adv$(EXE)

src/munge/munge$(EXE): force
	@cd src/kio; make
	@cd src/munge ; make

src/adv/adv$(EXE): force
	@cd src/kio ; make
	@cd src/adv ; make

adv.key adv.rec: src/munge/munge$(EXE)
	src/munge/munge$(EXE) < comcave

lint print:
	@cd src/kio ; make $@
	@cd src/munge ; make $@
	@cd src/adv ; make $@

clean:
	@cd src/kio ; make $@
	@cd src/munge ; make $@
	@cd src/adv ; make $@
	rm -f adv.key adv.rec

install: adv.key adv.rec src/adv/adv
	cp adv.key adv.rec $(GAMLIB)
	cd $(GAMLIB); chmod 644 adv.key adv.rec
	cp src/adv/adv$(EXE) $(GAMDIR)/adventure$(EXE)
	cd $(GAMDIR); chmod 711 adventure$(EXE)

force: ;
