#
# Something quick and dirty... you must put things where you want...
#
include ../../CONFIG

all:	src/munge/munge src/adv/adv

src/munge/munge: force
	@cd src/kio; make
	@cd src/munge ; make

src/adv/adv: force
	@cd src/kio ; make
	@cd src/adv ; make

adv.key adv.rec: src/munge/munge
	src/munge/munge < comcave

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
	cp src/adv/adv $(GAMDIR)/adventure
	cd $(GAMDIR); chmod 711 adventure

force: ;
