#include <stdio.h>
#include <ar.h>

struct ar_hdr arhdr;
int state;

main(argc, argv)
int argc;
char *argv[];
{ FILE *fd1, *fd2;
  int byt1, byt2;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <file1> file2>\n", argv[0]);
    exit(1);
  }

  fd1 = fopen(argv[1], "r");
  if (fd1 == NULL) {
    perror(argv[1]);
    exit(1);
  }
  fd2 = fopen(argv[2], "r");
  if (fd2 == NULL) {
    perror(argv[2]);
    exit(1);
  }
  /*
   *	Special check for libraries -- ignore the first member (__.SYMDEF).
  */
  byt1 = getw(fd1);		/* Get first magic number, if any	*/
  byt1 = getw(fd2);		/* Get second magic number, if any	*/
  if ((byt1 == ARMAG) && (byt1 == ARMAG)) {
    /*
     *	Skip the first member (presumed to be __.SYMDEF)
    */
    fread((char *)&arhdr, sizeof arhdr, 1, fd1);
    fread((char *)&arhdr, sizeof arhdr, 1, fd2);
  } else {
    lseek(fd1, 0L, 0);		/* Rewind first file			*/
    lseek(fd2, 0L, 0);		/* Rewind second file			*/
  }
  /*
   *	State machine to compare the two files, ignoring date stamps.
  */
  byt1 = getc(fd1);
  byt2 = getc(fd2);
  state = '$';
  while ((byt1 != EOF) || (byt2 != EOF)) {
    if (byt1 != byt2) {
      fprintf(stderr, "%s and %s are different\n", argv[1], argv[2]);
      exit(1);
    }
    if (byt1 == state) {
      switch (state) {		/* Move to next state	*/
        case '$': state = 'D'; goto foo;
        case 'D': state = 'a'; goto foo;
        case 'a': state = 't'; goto foo;
        case 't': state = 'e'; goto foo;
        case 'e': state = ':';
foo:              byt1 = getc(fd1);
                  byt2 = getc(fd2);
		  break;
        case ':':		/* Skip date stamp	*/
           { while ((byt1 = getc(fd1)) != '$')
               if (byt1 == EOF)
                 break;
             while ((byt2 = getc(fd2)) != '$')
               if (byt2 == EOF)
                 break;
             state = '$';
           }
      }
    } else {
      state = '$';
      byt1 = getc(fd1);
      byt2 = getc(fd2);
    }
  }
  exit(0);
}
