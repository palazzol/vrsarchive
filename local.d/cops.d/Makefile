#  Simple Makefile for the COPS system; compiles, and chmods 
# the programs.
#
#	make all	    -- makes everything
#	make <program_name> -- make a given program
EXECUTABLE	= home.chk user.chk is_readable is_writable pass.chk \
			  addto clearfiles filewriters members tilde
C_SRC		= home.chk.c user.chk.c is_readable.c is_writable.c pass.c \
			  addto.c clearfiles.c filewriters.c members.c tilde.c
SHELL_PROGS	= chk_strings root.chk dev.chk dir.chk cron.chk \
			  file.chk cops group.chk rc.chk passwd.chk \
			  suid.chk kuang init_kuang reconfig
SUPPORT		= dir.chklst file.chklst makefile stop.sample \
COPS.READ.1ST Beta.info SUID.README MANIFEST
DOCS		= COPS.report.ms suid.man.ms kuang.man.ms
MAN			= cops.1 cron.1 dev.1 dir.1 file.1 group.1 passwd.1 \
			  is_able.1 home.1 user.1 pass.1
OPTIONS		= -g
ALL			= $(EXECUTABLE) $(DOCS) $(MAN)
include /etc/CONFIG
#
# Where the programs are....
#
CHMOD		= chmod
MKDIR		= mkdir
CP			= cp

# make all
install: $(ALL)
	$(CHMOD) 755 $(SHELL_PROGS)

clean:
	rm -f $(EXECUTABLE) $(DOCS)
	rm -f $(MAN) Success
	$(CHMOD) 555 $(SHELL_PROGS)

# make the programs
addto: src/addto.c
	$(CC) $(CFLAGS) -o $@ src/addto.c

clearfiles: src/clearfiles.c
	$(CC) $(CFLAGS) -o $@ src/clearfiles.c

filewriters: src/filewriters.c
	$(CC) $(CFLAGS) -o $@ src/filewriters.c

members: src/members.c
	$(CC) $(CFLAGS) -o $@ src/members.c

home.chk: src/home.chk.c
	$(CC) $(CFLAGS) -o $@ src/home.chk.c

user.chk: src/user.chk.c
	$(CC) $(CFLAGS) -o $@ src/user.chk.c

is_readable: src/is_readable.c
	$(CC) $(CFLAGS) -o $@ src/is_readable.c

is_writable: src/is_writable.c
	$(CC) $(CFLAGS) -o $@ src/is_writable.c

pass.chk: src/pass.c
	$(CC) $(CFLAGS) -o $@ src/pass.c

tilde: src/tilde.c
	$(CC) $(CFLAGS) -o tilde src/tilde.c

# 'roff out those docs
COPS.report.ms: docs/COPS.report
	$(NROFF) -ms docs/COPS.report > docs/$@
	mv docs/$@ $@

kuang.man.ms: docs/kuang.man
	$(NROFF) -ms docs/kuang.man > docs/$@
	mv docs/$@ $@

suid.man.ms: docs/suid.man
	$(NROFF) -ms docs/suid.man > docs/$@
	mv docs/$@ $@

cops.1: docs/cops
	$(NROFF) -man docs/cops > docs/$@
	mv docs/$@ $@

cron.1: docs/cron
	$(NROFF) -man docs/cron > docs/$@
	mv docs/$@ $@

dev.1: docs/dev
	$(NROFF) -man docs/dev > docs/$@
	mv docs/$@ $@

dir.1: docs/dir
	$(NROFF) -man docs/dir > docs/$@
	mv docs/$@ $@

file.1: docs/file
	$(NROFF) -man docs/file > docs/$@
	mv docs/$@ $@

group.1: docs/group
	$(NROFF) -man docs/group > docs/$@
	mv docs/$@ $@

passwd.1: docs/passwd
	$(NROFF) -man docs/passwd > docs/$@
	mv docs/$@ $@

pass.1: docs/pass
	$(NROFF) -man docs/pass > docs/$@
	mv docs/$@ $@

is_able.1: docs/is_able
	$(NROFF) -man docs/is_able > docs/$@
	mv docs/$@ $@

home.1: docs/home
	$(NROFF) -man docs/home > docs/$@
	mv docs/$@ $@

user.1: docs/user
	$(NROFF) -man docs/user > docs/$@
	mv docs/$@ $@

# the end
