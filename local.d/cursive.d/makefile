CFLAGS = -g

cursive: cursive.o font.o
	$(CC) $(CFLAGS) -o $@ cursive.o font.o

cursive.shar: cursive.c font.c cursive.h cursive.6 Makefile shar
	shar cursive.c font.c cursive.h cursive.6 Makefile shar > cursive.shar
