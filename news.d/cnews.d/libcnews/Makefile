# libcnews makefile

CFLAGS= -O -I../include -p # -pg -Dvoid=int -DCHARBITS=0377
LIB=libcnews.a

$(LIB): readline.c lock.c ngmatch.c path.c strlower.c time.c
	$(CC) $(CFLAGS) -c $?
	ar rv $(LIB) *.o
	rm *.o
	ranlib $(LIB)

# header dependencies for libcnews.a
lock.o: ../include/news.h
ngmatch.o: ../include/news.h
path.o: ../include/news.h ../include/newspaths.h
time.o: ../include/news.h
