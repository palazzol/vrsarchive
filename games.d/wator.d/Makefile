#
# Makefile for wator.  Note that on 2.8 and 2.9 you may
# need to change the library from "termcap" to "termlib".
#

CC=cc
CFLAGS= -O
LIBS= -ltermcap
#LIBS= -ltermlib
OBJS=	wator.o screen_control.o item_control.o init.o  	parse_args.o fish_cycle.o shark_cycle.o

wator:	$(OBJS)
	$(CC) -o wator $(OBJS) $(LIBS)

wator.o: wator.c wator.h
	$(CC) $(CFLAGS) -c wator wator.c

screen_control.o: screen_control.c wator.h
	$(CC) $(CFLAGS) -c screen_control screen_control.c

item_control.o: item_control.c wator.h
	$(CC) $(CFLAGS) -c item_control item_control.c

init.o: init.c wator.h
	$(CC) $(CFLAGS) -c init init.c

parse_args.o: parse_args.c wator.h
	$(CC) $(CFLAGS) -c parse_args parse_args.c

fish_cycle.o: fish_cycle.c wator.h
	$(CC) $(CFLAGS) -c fish_cycle fish_cycle.c

shark_cycle.o: shark_cycle.c wator.h
	$(CC) $(CFLAGS) -c shark_cycle shark_cycle.c

