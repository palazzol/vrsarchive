# makefile for centipede
# edit sys_dep.c to adjust pathnames for your system
#
CFLAGS = -O -DSYSV -Dindex=strchr

OBJECTS = cent.o stuff.o input.o interrupts.o score.o move.o rand.o \
		save.o shoot.o la.o extern.o options.o sys_dep.o

cent:   $(OBJECTS)
	cc -o cent $(OBJECTS) -lcurses -lm

cent.o stuff.o input.o move.o shoot.o save.o extern.o options.o: cent.h
input.o options.o: sys_dep.h
