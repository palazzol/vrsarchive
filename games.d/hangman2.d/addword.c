/***                    addword.c               ***/

/**  Add a word to the hangman word file.  Note this must be setuid games. **/

/** (C) 1985 Dave Taylor:  ihnp4!hpfcla!d_taylor -or-  hplabs!hpcnof!dat **/

#include <stdio.h>

#define WLEN            40
#define word_file       "/usr/games/lib/.hang-words"
#define temp_file       "/tmp/hang"
#define mode            300     /* -rw------ */

main(argc, argv)
int argc;
char *argv[];
{
        int words;
        char theword[WLEN], filename[40];
        FILE *w, *temp;
     
        if (argc != 2)
          exit(printf("Usage: %s <word>\n", argv[0]));

        if ((w = fopen(word_file,"r")) == NULL)
          exit(printf("Couldn't open word file!\n"));

        sprintf(filename,"%s.%d", temp_file, getpid());
        if ((temp = fopen(filename,"w")) == NULL)
          exit(printf("Couldn't open temp file!\n"));

        fgets(theword, WLEN, w);

        sscanf(theword, "%d", &words);

        printf("Adding this word makes %d words!\n", ++words);

        fprintf(temp, "%d\n", words);

        while (fgets(theword, WLEN, w) != NULL)
          fputs(theword, temp);

        fprintf(temp, "%s\n", argv[1]);
        fclose(temp);
        fclose(w);
        unlink(word_file);
        link(filename,word_file);
        unlink(filename);
        chmod(word_file, mode);
}
