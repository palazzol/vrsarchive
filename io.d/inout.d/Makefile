MYNAME	= inout
CONF	= /etc/conf
MODULES	= $(CONF)/modules
DRIVER	= $(MODULES)/$(MYNAME)
CFLAGS	= -DINKERNEL -I.. -O
BINARY	= config $(MYNAME).o # space.c

all:	$(MYNAME).o # space.o

$(CONF) $(MODULES) $(DRIVER):
	mkdir $@
	chmod 755 $@

install: $(CONF) $(MODULES) $(DRIVER) $(BINARY)
	cp $(BINARY) $(DRIVER)
#	cp ../sys/$(MYNAME).h /usr/include/sys

clean:
	rm -f $(MYNAME).o # space.o
