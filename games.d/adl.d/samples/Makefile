include /etc/CONFIG
# AR = shar -c -v
AR = post -c -v -p 8 17
B = ..
D = samples/

all install:
	@(cd aard;	make $@)
	@(cd mpu;	make $@)
	@(cd demos;	make $@)

touch		:
	(cd aard;	make all -t)
	(cd mpu;	make all -t)
	(cd demos;	make all -t)

shar		: samples.shar
	(cd aard;	make shar)
	(cd mpu;	make shar)
	(cd demos;	make shar)

samples.shar	: README Makefile standard.adl
	( cd ${B} ; ${AR} \
		${D}README	${D}COPYRIGHT	${D}Makefile	\
		${D}standard.adl	\
	> ${D}samples.shar )

clean	:
	@(cd aard;	make clean)
	@(cd mpu;	make clean)
	@(cd demos;	make clean)
	rm -f core samples.shar
