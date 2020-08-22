MYNAME	= i8751
CONF	= ../..
MODULES	= $(CONF)/modules
NODED	= $(CONF)/node.d
PACKD	= $(CONF)/pack.d
MDEVICE	= $(CONF)/cf.d
SDEVICE	= $(CONF)/sdevice.d
DRVR30	= $(MODULES)/$(MYNAME)
DRVR31	= $(PACKD)/$(MYNAME)
CFLAGS	= -DMB1 -DINKERNEL -I. -O

all:	$(MYNAME).o space.o

$(CONF) $(MODULES) $(PACKD) $(SDEVICE) $(DRVR30) $(DRVR31):
	mkdir $@
	chmod 755 $@

3.0: $(CONF) $(MODULES) $(DRVR30) $(MYNAME).o space.c
	cp $(MYNAME).o $(DRVR30)/
	cp space.c $(DRVR30)/space.c+
	mv $(DRVR30)/space.c+ $(DRVR30)/space.c
	cp sys/$(MYNAME).h /usr/include/sys

3.1: $(CONF) $(PACKD) $(DRVR31) $(MYNAME).o space.c
	cp $(MYNAME).o $(DRVR31)/Driver.o
	cp space.c $(DRVR31)/space.c+
	mv $(DRVR31)/space.c+ $(DRVR31)/space.c
	cp sys/$(MYNAME).h /usr/include/sys
	(sed "/^$(MYNAME)[ 	]/d" $(MDEVICE)/mdevice; cat mdevice) >mdevice+
	mv mdevice+ $(MDEVICE)/mdevice
	cp node $(NODED)/$(MYNAME)
	cp sdev $(SDEVICE)/$(MYNAME)

clean:
	rm -f $(MYNAME).o space.o
