OBJECTS=array.o boolean.o config.o control.o dictionary.o file.o\
	integer.o main.o math.o misc.o name.o operator.o\
	poly.o property.o real.o save.o stack.o string.o unix.o
LIBS=libww.a -lsuntool -lsunwindow -lpixrect -g
GRAPHICS=cache.o colour.o device.o fill.o font.o gsave.o image.o mat.o matrix.o\
	pat.o path.o state.o stroke.o
CFLAGS=-O

PS:	$(OBJECTS) $(GRAPHICS) hard.o canon.a
	cc $(CFLAGS)  $(OBJECTS) $(GRAPHICS) hard.o canon.a -lm `libs` -o PS

sunPS:	$(OBJECTS) $(GRAPHICS) hard.o canon.a pixrect
	cc $(CFLAGS)  $(OBJECTS) $(GRAPHICS) hard.o canon.a -lm -lpixrect -o sunPS

CPS:	$(OBJECTS) $(GRAPHICS) colour-ww.o trapezoid.o canon.o
	cc $(CFLAGS)  $(OBJECTS) $(GRAPHICS) colour-ww.o canon.o trapezoid.o -lm `libs` -o CPS

postscript:	$(OBJECTS) $(GRAPHICS) adapter.o protocol.o
	cc $(CFLAGS) $(OBJECTS) $(GRAPHICS) adapter.o protocol.o -lm -o postscript

XPS:	$(OBJECTS) $(GRAPHICS) X.o
	cc $(CFLAGS)  $(OBJECTS) $(GRAPHICS) X.o -lm -lX -o XPS

canon.a:	canon.o screen.o trapezoid.o paint.o
	ar ruv canon.a canon.o screen.o trapezoid.o paint.o
	ranlib canon.a

viewer:	protocol.o viewer.o hard.o canon.a
	cc protocol.o viewer.o hard.o canon.a `libs` -o viewer

all:	PS postscript viewer

ww:	ww.o wwlib installww

pixrect:	pixrect.o
	cp pixrect.o hard.o

sun:	ww wwsun

orion:	orion.o installorion orionlib

X.o:	
	cc -c X.c

wwlib:
	if [ -f libww.a ]; \
	then \
		echo 'echo libww.a' >lww; \
	else \
		echo 'echo -lww' >lww; \
	fi; \
	chmod +x lww
	echo "echo `lww`" >libs; chmod +x libs

wwsun:	
	echo "echo `lww` -lsuntool -lsunwindow -lpixrect" >libs; chmod +x libs

orionlib:
	echo 'echo -lG' >libs; chmod +x libs

installww:
	cp ww.o hard.o

installorion:
	cp orion.o hard.o
