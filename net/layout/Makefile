main: pcb

pcb: pmain.c pparm.h pcdst.h part1.o part2.o pplace.o pwide.o pupdate.o cmu.o
	cc -O -o pcb pmain.c part1.o part2.o pplace.o pwide.o pupdate.o cmu.o -lm

part1.o: pparm.h pcdst.h paed.o psub.o pdiag.o pcif.o pmap.o patr.o pfio.o pstat.o
	ld -r -o part1.o paed.o psub.o pdiag.o pcif.o pmap.o patr.o pfio.o pstat.o

part2.o: pcdst.h pparm.h pwork.o pcmdl.o pmnrt.o pleer.o pleer1.o pleer2.o pplow.o
	ld -r -o part2.o pwork.o pcmdl.o pmnrt.o pleer.o pleer1.o pleer2.o pplow.o

pplace.o: pplace.c pparm.h pcdst.h
	cc -O -c pplace.c

pmap.o: pmap.c pparm.h pcdst.h
	cc -O -c pmap.c

pdiag.o: pdiag.c pparm.h pcdst.h
	cc -O -c pdiag.c

pleer.o: pleer.c pparm.h pcdst.h pleer.h
	cc -O -c pleer.c

pleer1.o: pleer1.c pparm.h pcdst.h pleer.h
	cc -O -c pleer1.c

pleer2.o: pleer2.c pparm.h pcdst.h pleer.h
	cc -O -c pleer2.c

pmnrt.o: pmnrt.c pparm.h pcdst.h
	cc -O -c pmnrt.c

patr.o: patr.c pparm.h pcdst.h pleer.h
	cc -O -c patr.c

psub.o: psub.c pparm.h pcdst.h
	cc -O -c psub.c

paed.o: paed.c pparm.h pcdst.h
	cc -O -c paed.c

pfio.o: pfio.c pparm.h pcdst.h
	cc -O -c pfio.c

pwork.o: pwork.c pparm.h pcdst.h
	cc -O -c pwork.c

pcif.o: pcif.c pparm.h pcdst.h
	cc -O -c pcif.c

pcmdl.o: pcmdl.c pparm.h pcdst.h
	cc -O -c pcmdl.c

pplow.o: pplow.c pparm.h pcdst.h
	cc -O -c pplow.c

pstat.o: pstat.c pparm.h pcdst.h
	cc -O -c pstat.c

pwide.o: pwide.c pparm.h pcdst.h
	cc -O -c pwide.c

pupdate.o: pupdate.c pparm.h pcdst.h
	cc -O -c pupdate.c

cmu.o: getbool.o getint.o getfloat.o wantread.o wantwrite.o fwantread.o fwantwrite.o openp.o getstr.o searchp.o
	ld -r -o cmu.o getbool.o getint.o getfloat.o wantread.o wantwrite.o fwantread.o fwantwrite.o openp.o getstr.o searchp.o

getbool.o: getbool.c
	cc -O -c getbool.c

getfloat.o: getfloat.c
	cc -O -c getfloat.c

getint.o: getint.c
	cc -O -c getint.c

wantread.o: wantread.c
	cc -O -c wantread.c

wantwrite.o: wantwrite.c
	cc -O -c wantwrite.c

fwantread.o: fwantread.c
	cc -O -c fwantread.c

fwantwrite.o: fwantwrite.c
	cc -O -c fwantwrite.c

searchp.o: searchp.c
	cc -O -c searchp.c

openp.o: openp.c
	cc -O -c openp.c

getstr.o: getstr.c
	cc -O -c getstr.c
