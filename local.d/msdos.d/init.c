/*
 * Initialize a MSDOS diskette.  Get the media signature (1st FAT entry)
 * and switch to the proper floppy disk device to match the format
 * of the disk.  Sets a bunch of global variables.  Returns 0 on success,
 * or 1 on failure.
 */

#include <stdio.h>
#include <ctype.h>
#include "msdos.h"

extern int fd, dir_len, dir_start, clus_size, fat_len, num_clus;
extern unsigned char *fatbuf;
extern char *mcwd;

int
init(mode)
int mode;
{
	int code = 0, buflen;
	unsigned char read_fat(), fat;
	char *getenv(), *fixmcwd(), *malloc(), version, get_dos_ver();
	long lseek();
	void perror(), exit();

					/* open the 'generic' floppy */
	if ((fd = open(FLOPPY, mode)) < 0) {
		perror("init: open");
		exit(1);
	}
					/* read media signature (1st FAT) */
	fat = read_fat();
	switch(fat) {
		case 0xfe:
#ifndef FLP_40_8_1
			fprintf(stderr, "40 track, 8 sector, single sided: not supported\n");
			code = 1;
#else FLP_40_8_1
			dir_start = 3;
			dir_len = 4;
			clus_size = 1;
			fat_len = 1;
			num_clus = 313;
			close(fd);
			if ((fd = open(FLP_40_8_1, mode)) < 0) {
				perror("init: open");
				return(1);
			}
#endif FLP_40_8_1
			break;
		case 0xff:
#ifndef FLP_40_8_2
			fprintf(stderr, "40 track, 8 sector, double sided: not supported\n");
			code = 1;
#else FLP_40_8_2
			dir_start = 3;
			dir_len = 7;
			clus_size = 2;
			fat_len = 1;
			num_clus = 315;
			close(fd);
			if ((fd = open(FLP_40_8_2, mode)) < 0) {
				perror("init: open");
				return(1);
			}
#endif FLP_40_8_2
			break;
		case 0xfc:
#ifndef FLP_40_9_1
			fprintf(stderr, "40 track, 9 sector, single sided: not supported\n");
			code = 1;
#else FLP_40_9_1
			dir_start = 5;
			dir_len = 4;
			clus_size = 1;
			fat_len = 2;
			num_clus = 351;
			close(fd);
			if ((fd = open(FLP_40_9_1, mode)) < 0) {
				perror("init: open");
				return(1);
			}
#endif FLP_40_9_1
			break;
		case 0xfd:
#ifndef FLP_40_9_2
			fprintf(stderr, "40 track, 9 sector, double sided: not supported\n");
			code = 1;
#else FLP_40_9_2
			dir_start = 5;
			dir_len = 7;
			clus_size = 2;
			fat_len = 2;
			num_clus = 354;
			close(fd);
			if ((fd = open(FLP_40_9_2, mode)) < 0) {
				perror("init: open");
				return(1);
			}
#endif FLP_40_9_2
			break;
		case 0xf9:		/* all 80 track disks */
			version = get_dos_ver();
			switch (version) {
				case '0':
				case '1':
#ifndef FLP_80_15_2
					fprintf(stderr, "80 track, 15 sector, double sided: not supported\n");
					code = 1;
#else FLP_80_15_2
					dir_start = 15;
					dir_len = 14;
					clus_size = 1;
					fat_len = 7;
					num_clus = 1193;
					close(fd);
					if ((fd = open(FLP_80_15_2, mode)) < 0) {
						perror("init: open");
						return(1);
					}
#endif FLP_80_15_2
					break;
				case '2':
#ifndef FLP_80_9_2
					fprintf(stderr, "3.5 inch 80 track, 9 sector, double sided: not supported\n");
					code = 1;
#else FLP_80_9_2
					dir_start = 7;
					dir_len = 7;
					clus_size = 2;
					fat_len = 3;
					num_clus = 713;
					close(fd);
					if ((fd = open(FLP_80_9_2, mode)) < 0) {
						perror("init: open");
						return(1);
					}
#endif FLP_80_9_2
					break;
				default:
					fprintf(stderr, "Unknown DOS version '%02x'\n", version);
					code = 1;
					break;
			}
			break;
		default:
			fprintf(stderr, "Unknown format '%02x'\n", fat);
			code = 1;
			break;
	}
	if (code)
		return(1);

	buflen = fat_len * MSECSIZ;
	fatbuf = (unsigned char *) malloc(buflen);
	move(1);
					/* read the FAT sectors */
	if (read(fd, fatbuf, buflen) != buflen) {
		perror("init: read");
		exit(1);
	}
					/* set dir_chain to root directory */
	reset_dir();
					/* get Current Working Directory */
	mcwd = fixmcwd(getenv("MCWD"));
					/* test it out.. */
	if (subdir("")) {
		fprintf(stderr, "Environmental variable MCWD needs updating\n");
		exit(1);
	}
	return(0);
}

/*
 * Move the read/write head to the next location.  Tries to optimize
 * the movement by moving relative to current location.  The argument
 * is a logical sector number.  All errors are fatal.
 */

move(sector)
int sector;
{
	long cur_loc, next, lseek();
	void exit(), perror();
					/* get current location */
	if ((cur_loc = lseek(fd, 0L, 1)) < 0) {
		perror("move: lseek");
		exit(1);
	}
	next = (long) (MSECSIZ * sector) - cur_loc;
					/* we're already there */
	if (next == 0L)
		return;
					/* move to next location */
	if (lseek(fd, next, 1) < 0) {
		perror("move: lseek");
		exit(1);
	}
	return;
}

/*
 * Fix MCWD to be a proper directory name.  Always has a leading separator.
 * Never has a trailing separator (unless it is the path itself).
 */

char *
fixmcwd(dirname)
char *dirname;
{
	char *s, *malloc(), *strcpy(), *strcat();
	static char *ans;

	ans = malloc(strlen(dirname)+2);
					/* add a leading separator */
	if (*dirname != '/' && *dirname != '\\') {
		strcpy(ans, "/");
		strcat(ans, dirname);
	}
	else
		strcpy(ans, dirname);
					/* translate to upper case */
	for (s = ans; *s; ++s) {
		if (islower(*s))
			*s = toupper(*s);
	}
					/* if separator alone */
	if (strlen(ans) == 1)
		return(ans);
					/* zap the trailing separator */
	s--;
	if (*s == '/' || *s == '\\')
		*s = NULL;
	return(ans);
}

/*
 * Read the first byte of the FAT table.  This code serves as a media
 * signature for the diskette.
 */

unsigned char
read_fat()
{
	unsigned char buf[MSECSIZ];
	static unsigned char ans;
					/* move to boot sector */
	if (lseek(fd, (long) MSECSIZ, 0) < 0) {
		perror("init: lseek");
		exit(1);
	}
					/* read the first FAT sector */
	if (read(fd, buf, MSECSIZ) != MSECSIZ) {
		perror("init: read");
		exit(1);
	}
	ans = *buf;
	return(ans);
}

/*
 * Read the Boot Sector to figure out the DOS version.
 */

char
get_dos_ver()
{
	unsigned char bootbuf[MSECSIZ];
	static char ans;
					/* move to boot sector */
	if (lseek(fd, 0L, 0) < 0) {
		perror("init: lseek");
		exit(1);
	}
					/* read the boot sector */
	if (read(fd, bootbuf, MSECSIZ) != MSECSIZ) {
		perror("init: read");
		exit(1);
	}
	ans = *(bootbuf+10);
	return(ans);
}
