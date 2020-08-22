ALL		= fixcpio
include $(HOME)/CONFIG

fixcpio:	fixcpio.c
	$(CC) $(CFLAGS) -o fixcpio fixcpio.c

install:	fixcpio
	cp fixcpio $(LOCAL)
	cp fixcpio.1 $(MAN1)/
	cd $(MAN1)/; chmod 644 fixcpio.1
