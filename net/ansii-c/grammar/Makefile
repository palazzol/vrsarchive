YFLAGS	= -d
CFLAGS	= -O
LFLAGS	=

SRC	= gram.y scan.l main.c
OBJ	= gram.o scan.o main.o

a.out	: $(OBJ)
	cc $(OBJ)

scan.o	: y.tab.h

clean	:
	rm -f a.out y.tab.h *.o
