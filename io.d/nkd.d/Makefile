MYNAME	= nkd
CONF	= /etc/conf
MODULES	= $(CONF)/modules
DRIVER	= $(MODULES)/$(MYNAME)
CFLAGS	= -DINKERNEL -I.. -O

all:	$(MYNAME).o space.o

$(CONF) $(MODULES) $(DRIVER):
	mkdir $@
	chmod 755 $@

install: $(CONF) $(MODULES) $(DRIVER) config $(MYNAME).o space.c
	cp config $(MYNAME).o space.c $(DRIVER)
	cp ../sys/$(MYNAME).h /usr/include/sys

clean:
	rm -f $(MYNAME).o space.o
