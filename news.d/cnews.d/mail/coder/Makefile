# makefile for uuencode and uudecode
CFLAGS=-O # -p
LIBS= dmr.faster/*.o

all: uuencode uudecode

uuencode: uuencode.o
	$(CC) $(CFLAGS) -o $@ uuencode.o $(LIBS)

uudecode: uudecode.o
	$(CC) $(CFLAGS) -o $@ uudecode.o $(LIBS)

clean:
	rm -f uuencode uudecode *.o
