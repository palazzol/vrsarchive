#  Simple Makefile for the COPS system; compiles, and chmods 
# the programs.
#
#	make all	    -- makes everything
#	make <program_name> -- make a given program
EXECUTABLE = home.chk user.chk is_readable is_writable pass.chk \
	     addto clearfiles filewriters members tilde
C_SRC      = home.chk.c user.chk.c is_readable.c is_writable.c pass.c \
	     addto.c clearfiles.c filewriters.c members.c tilde.c
SHELL_PROGS= chk_strings root.chk dev.chk dir.chk cron.chk \
	     file.chk cops group.chk rc.chk passwd.chk \
	     suid.chk kuang init_kuang reconfig
SUPPORT    = dir.chklst file.chklst makefile stop.sample \
	     COPS.READ.1ST Beta.info SUID.README MANIFEST
DOCS	   = COPS.report.ms suid.man.ms kuang.man.ms
MAN        = cops.1 cron.1 dev.1 dir.1 file.1 group.1 passwd.1 \
	     is_able.1 home.1 user.1 pass.1
CFLAGS     = -O
ROFFLAGS   = -ms

#
# Where the programs are....
#
CHMOD=/bin/chmod
MKDIR=/bin/mkdir
CP=/bin/cp
CC=/bin/cc
NROFF=/usr/ucb/nroff

# make all
all:	$(EXECUTABLE) $(DOCS) $(MAN)
	$(CHMOD) 700 $(SHELL_PROGS)

# make the programs
addto: src/addto.c
	$(CC) $(CFLAGS) -o addto src/addto.c

clearfiles: src/clearfiles.c
	$(CC) $(CFLAGS) -o clearfiles src/clearfiles.c

filewriters: src/filewriters.c
	$(CC) $(CFLAGS) -o filewriters src/filewriters.c

members: src/members.c
	$(CC) $(CFLAGS) -o members src/members.c

home.chk: src/home.chk.c
	$(CC) $(CFLAGS) -o home.chk src/home.chk.c

user.chk: src/user.chk.c
	$(CC) $(CFLAGS) -o user.chk src/user.chk.c

is_readable: src/is_readable.c
	$(CC) $(CFLAGS) -o is_readable src/is_readable.c

is_writable: src/is_writable.c
	$(CC) $(CFLAGS) -o is_writable src/is_writable.c

pass.chk: src/pass.c
	$(CC) $(CFLAGS) -o pass.chk src/pass.c

tilde: src/tilde.c
	$(CC) $(CFLAGS) -o tilde src/tilde.c

# 'roff out those docs
COPS.report.ms: docs/COPS.report
	$(NROFF) $(ROFFLAGS) docs/COPS.report > docs/COPS.report.ms

kuang.man.ms: docs/kuang.man
	$(NROFF) $(ROFFLAGS) docs/kuang.man > docs/kuang.man.ms

suid.man.ms: docs/suid.man
	$(NROFF) $(ROFFLAGS) docs/suid.man > docs/suid.man.ms

cops.1: docs/cops
	$(NROFF) -man docs/cops > docs/cops.1

cron.1: docs/cron
	$(NROFF) -man docs/cron > docs/cron.1

dev.1: docs/dev
	$(NROFF) -man docs/dev > docs/dev.1

dir.1: docs/dir
	$(NROFF) -man docs/dir > docs/dir.1

file.1: docs/file
	$(NROFF) -man docs/file > docs/file.1

group.1: docs/group
	$(NROFF) -man docs/group > docs/group.1

passwd.1: docs/passwd
	$(NROFF) -man docs/passwd > docs/passwd.1

pass.1: docs/pass
	$(NROFF) -man docs/pass > docs/pass.1

is_able.1: docs/is_able
	$(NROFF) -man docs/is_able > docs/is_able.1

home.1: docs/home
	$(NROFF) -man docs/home > docs/home.1

user.1: docs/user
	$(NROFF) -man docs/user > docs/user.1

# the end
