/* 
 *	badsect - Find and mark bad sectors.
 *
 * Badsect takes a list of file-system relative sector numbers and makes
 * files containing the blocks of which these sectors are a part.  It can
 * be used to contain sectors which have problems if these sectors are not
 * part of the bad track table for the winchester.  For instance, this prog-
 * ram can be used if the driver for the file-system in question does not
 * support bad block forwarding.
 *
 * It uses a directory called BAD which is located at the root of the file
 * system it was invoked from (i.e. /BAD, /usr/BAD, etc).  These files should
 * never be deleted (i.e., when fsck reports DUP blocks, delete all such
 * files EXCEPT ones in the BAD directory).
 *
*/

#include <sys/param.h>
#include <stdio.h>
#include <sys/dir.h>
#include <sys/filsys.h>
#include <sys/stat.h>
#define TRUE 1

char name[DIRSIZ+6];		/* /dev/[DIRSIZ chars]\0		*/
int errs;
struct stat buf;		/* Scratch area for stat syscalls	*/
int fd;				/* A file descriptor			*/
unsigned b;			/* Current block number			*/
unsigned size;			/* Size of device in blocks		*/
dev_t mydev;			/* Device number of this filesystem	*/
char block[BSIZE];		/* A block sized buffer			*/

go_home()
{
  while (TRUE) {
    if (stat(".",&buf) < 0) {
      fprintf(stderr,"Cannot stat '.' on way to filesystem root\n");
      exit(1);
    }
    if (buf.st_ino == ROOTINO)
      break;
    if (chdir("..") < 0) {
      fprintf(stderr,"Cannot chdir '..' on way to filesystem root\n");
      exit(1);
    }
  }
  mydev = buf.st_dev;
  if (chdir("BAD") < 0) {
    system("mkdir BAD");
    if (chdir("BAD") < 0) {
      fprintf(stderr,"Cannot chdir to BAD at filesystem root\n");
      exit(1);
    }
  }
}

get_dev()
{ struct direct dirent;
  FILE *fd;			/* A buffered file descriptor for /dev	*/

  if ((fd = fopen("/dev","r")) == NULL) {
    fprintf(stderr,"Cannot open /dev directory\n");
    exit(1);
  }
  while (fread((char *)&dirent, sizeof dirent, 1, fd) == 1) {
    if (dirent.d_ino == 0)
      continue;
    strcpy(name,"/dev/");
    strncat(name,dirent.d_name,DIRSIZ+1);
    if (stat(name,&buf) < 0) {
      fprintf(stderr,"Cannot stat %s - continuing\n",name);
      continue;
    }
    if (buf.st_mode & S_IFCHR)
      if (buf.st_rdev == mydev) {
        fclose(fd);
        return;			/* name contains result			*/
      }
  }
  fprintf(stderr,"Cannot find device name in /dev directory\n");
  exit(1);
}

main(argc,argv)
int argc;
char *argv[];
{ go_home();			/* Chdir to root/BAD		*/
  get_dev();			/* Figure out device name	*/
  if ((fd = open(name,0)) < 0) {
    fprintf(stderr,"Cannot open %s\n",name);
    exit(1);
  }
  lseek(fd, (long)BSIZE, 0);	/* Seek the superblock		*/
  if (read(fd, block, sizeof block) != sizeof block) {
    fprintf(stderr,"Cannot read superblock - deep yogurt\n",block);
    exit(1);
  }
  size = (unsigned) (*(struct filsys *)block).fs_fsize;
  for (b = 2; b < size; b++) {	/* Test each block		*/
    lseek(fd, ((long)BSIZE)*b, 0);
    if (read(fd, block, sizeof block) != sizeof block) {
      sprintf(block,"%05u", b);
      if (mknod(block, 0, b)) {	/* Mark a bad block		*/
        fprintf(stderr,"Cannot mark %s bad - continuing\n",block);
      }
    }
  }
  printf("*****\n");
  printf("***** Do not clear the inodes for BAD/*\n");
  printf("*****\n");
  execlp("fsck","fsck",name,(char *)NULL);
}
