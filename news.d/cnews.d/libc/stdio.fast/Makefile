# makefile for dmr.faster

SRC=fgets.c fputs.c rdwr.c # memcpy.c
DEFINES = -DBSD # -pg # -Dvoid=int
CFLAGS = $(DEFINES) -O
LINTFLAGS = $(DEFINES) -ha

dmr.faster: $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)
	touch $@

lint:
	lint $(LINTFLAGS) -u $(SRC)
lintport:
	lint -p $(LINTFLAGS) -u $(SRC)
