INCRT	= .
CFLAGS	= -g -n -I$(INCRT) -DINKERNEL -DSYSV $(MORECPP)
MASTER	= master.d
ITP		= $(MASTER)/itp/space.c io/itp.c
SRC		= $(MASTER)/i214/space.c io/i214.c $(ITP) llib-lkernel.c
#SRC 	= $(ITP) llib-lkernel.c
OBJ		= space.o itp.o

lint: $(SRC)
	lint $(CFLAGS) $(SRC) |\
		grep -v '?' |\
		grep -v 'comparison of unsigned with negative' |\
		grep -v 'struct/union.*never defined' |\
		tee $@+
	mv $@+ $@

tptest: $(OBJ) tptest.o debuglib.o
	$(CC) -o $@ $(CFLAGS) $(OBJ) tptest.o debuglib.o

space.o: itp/space.c
	$(CC) $(CFLAGS) $?

clean:
	rm -f $(OBJ) tptest.o debuglib.o tptest lint
