CFLAGS	= -O
LFLAGS	= -F 2000
LIBS	= -ltermcap
OBJ	= ctc.o termcap.o

all:    ctc

ctc:    $(OBJ)
	cc ${LFLAGS} ${OBJ} -o ctc ${LIBS}

clean:
	rm -f ${OBJ} ctc make.out core
