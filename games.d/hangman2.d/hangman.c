/**             hangman.c               **/

/**  Plays the game of hangman!         **/

/** (C) 1985 Dave Taylor: ihnp4!hpfcla!d_taylor -or- hplabs!hpcnof!dat **/

#include <stdio.h>
#include <signal.h>
#include "curses.h"

#define word_file       "/usr/games/lib/.hang-words"
#define MAX_CHARS       25
#define MAX_GUESSES     10

#define GUESSED_IT      0
#define TRIED_GUESS     1
#define GOOD_GUESS      2
#define BAD_GUESS       3

#define delay(n)        {register int i; for (i=0;i<n;i++); }

char  word[MAX_CHARS];
char  guessed[26], part_word[MAX_CHARS];
int   bad_guesses, words_gotten = 0, words_tried = 0;
int   debug = 0;

main(argc)
int argc;
{
        char ch;
        int  leave();

        debug = (argc>1);

        switch(InitScreen()) {
          case -1 : exit(printf("Unknown terminal\n"));
          case -2 : exit(printf("Not Cursor Addressable\n"));
          default : break;
        }
     
        if (debug) printf("initscreen succeeded\n");

        signal(SIGINT, leave);
        signal(SIGQUIT,leave);
     
        do {
          initialize(word);
     
          ClearScreen();
          build_gallows();
          show_word(part_word);
          show_guessed();
          if (words_tried)
            PutLine(9,50,"Success: %2d%%",
            (int) ((float) 100*((float) words_gotten/(float) words_tried)));
     
          while (bad_guesses < MAX_GUESSES && not_guessed()) {
            switch (guess(&ch)) {
              case BAD_GUESS  : show_gallows(++bad_guesses);
                                add_to_guessed(ch);
                                show_guessed();
                                break;
              case GOOD_GUESS : add_to_guessed(ch);
                                show_guessed();
                                fill_in(ch);
                                show_word(part_word);
                                break;
              case GUESSED_IT : show_word(word);
                                strcpy(part_word,word);
                                break;
              case TRIED_GUESS: break;    /** no penalty **/
              default         : PutLine(22,0,"Huh?");
            }
          }

        if (bad_guesses < MAX_GUESSES)
          words_gotten++;
        else
          PutLine(19,0,"The word you were trying to guess was '%s'",word);
     
        words_tried++;

        PutLine(17,0,"Would you like to try another word? (y/n) ");
      } while (tolower(ReadACh()) != 'n');

      PutLine(19,0,"Your final success rate was %d%%",
      (int) ((float) 100*((float) words_gotten/(float) words_tried)));
      CleartoEOLN();
     
      leave();
}

int
ReadACh()
{
        char ch;

        Raw(ON);
        ch = ReadCh();
        Raw(OFF);
        return(ch);
}

initialize(word)
char *word;
{
        /** pick word, and initialize all variables! **/

        int max_words, get_word;
        register int i;
        FILE *wordfile;

        if (debug) printf("initialize()\n");
        srand(time(0));
        if ((wordfile = fopen(word_file, "r")) == NULL)
          leave(printf("Cannot open wordfile %s!\n", word_file));

        fgets(word,MAX_CHARS, wordfile);
        sscanf(word, "%d", &max_words);

        get_word = rand() % max_words + 1;

        while (get_word-- > 0)
          fgets(word,MAX_CHARS, wordfile);
     
        word[strlen(word)-1] = '\0';    /** remove the '\n' **/

        fclose(wordfile);

        for (i= 0; i < 26; i++)
          guessed[i] = 0;

        bad_guesses = 0;

        for (i = 0; i < strlen(word); i++)
          part_word[i] = '_';
        part_word[i] = '\0';

}

int
not_guessed()
{
        /** returns non-zero iff there are any underlines in the
            variable part_word (ie the word isn't fully guessed yet!) **/

        register int i = 0;

        while (i < strlen(part_word) && part_word[i] != '_')
            i++;

        return(i != strlen(part_word));
}

int
guess(ch)
char *ch;
{
        /**     Guess a letter.  If RETURN is keyed in, try to
                guess the entire word.  There is NO penalty for
                attempting to guess the word.   Returns either
                GOOD_GUESS, BAD_GUESS, GUESSED_IT or TRIED_GUESS **/

        char guessed_word[MAX_CHARS];
        register int i;

        if (debug) printf("guess()\n");
        MoveCursor(21,0); CleartoEOLN();

        PutLine(17,0,"Guess a character: ");
        CleartoEOLN();
        *ch = tolower(ReadACh());
        MoveCursor(19,0); CleartoEOLN();
        switch (*ch) {
          case 127 :  leave();          /** hit DELETE! **/
          case 13  :  PutLine(19,0,"You think the word is: ");  /** <ret> **/
                      gets(guessed_word,MAX_CHARS);
                      if (strcmp(guessed_word, word) == 0) {
                        PutLine(19,0,"You Got It!");
                        CleartoEOLN();
                        return(GUESSED_IT);
                      }
                      else {
                        PutLine(19,0,"Nope.");
                        CleartoEOLN();
                        return(TRIED_GUESS);
                      }
          default  :  if (*ch < (char) 30)
                        leave();
                      else if (*ch > 'z' || *ch < 'a') {
                        PutLine(19,0,"Guess must be 'A' thru 'Z'");
                        return(TRIED_GUESS);
                       }
                      if (guessed[*ch - 'a']) {
                        PutLine(19,0,"You already guessed that letter!");
                        return(TRIED_GUESS);
                      }
                      for (i = 0; i < strlen(word); i++)
                        if (word[i] == *ch)
                          return(GOOD_GUESS);
                      return(BAD_GUESS);
        }
}

add_to_guessed(ch)
char ch;
{
        /** add ch to guessed.  Keep guessed in alphabetical order! **/

        if (debug) printf("add_to_guessed(%c)\n",ch);
        guessed[ch - 'a']++;
}

show_guessed()
{
        /** show what characters have been guessed so far **/
        register int i;

        if (debug) printf("show_guessed()\n");
        PutLine(3,50, "Guessed: ");
        for (i = 0; i < 26; i++)
          if (guessed[i])
            putchar((char) i + 'a');
        fflush(stdout);
}

fill_in(ch)
char ch;
{
        /** fill in all occurances of 'ch' in word **/
        register int i;

        if (debug) printf("fill_in(%c)\n", ch);
        for (i=0; i < strlen(word); i++)
          if (word[i] == ch)
            part_word[i] = ch;
}
     
show_word(word)
char *word;
{
        PutLine(6,50,"Word: %s", word);
        CleartoEOLN();
}

leave()
{
        MoveCursor(23,0);
        exit(0);
}

/******* and now the cheery part: the hanging graphics routines! ********/

/**
     
                        0                       @
     man to be hung -> (|)      the hangman -> /|\
                       / \                     / \

**/

char hangee[3][3];

#define put_man(who,l,c)        PutLine(l-2,c,"%3.3s",who[0]); \
                                PutLine(l-1,c,"%3.3s",who[1]); \
                                PutLine(l,c,"%3.3s", who[2]);
#define clear_man(l,c)          PutLine(l-2,c,"   "); \
                                PutLine(l-1,c,"   "); \
                                PutLine(l,c,"   ");
show_gallows(n)
int n;
{
        /** draw gallows...as n increases, draw the parts of the
            man to be hung... **/

        int line, column;

        if (debug) printf("show_gallows(%d)\n", n);
        if (n == 1) { /** place hangee! **/
          strcpy(hangee[0]," 0 ");
          strcpy(hangee[1],"(|)");
          strcpy(hangee[2],"/ \\");
          place_man(1, &line, &column);
          put_man(hangee,line, column);
        }
        else if (n < 10) {
          place_man(n-1, &line, &column); /* remove old hangee  */
          clear_man(line, column);
          place_man(n, &line, &column);   /* put him at new loc */
          put_man(hangee,line, column);
          fix_gallows(n-1);               /* and patch it up    */
        }
        else { /** too late: this is the big death scene! **/
          place_man(n-1, &line, &column);
          clear_man(line, column);
          place_man(n, &line, &column);
          put_man(hangee,line, column);
          delay(300);
          PutLine(4,24,"|");
          PutLine(5,24,"|");
          PutLine(6,24,"#");
          PutLine(7,27,"- Augh!");
        }
}

place_man(n, line, col)
int n, *line, *col;
{
        switch(n) {
          case 1 : *line = 12;  *col = 1;       break;
          case 2 : *line = 10;  *col = 4;       break;
          case 3 : *line = 9;   *col = 7;       break;
          case 4 : *line = 8;   *col = 10;      break;
          case 5 : *line = 8;   *col = 16;      break;
          case 6 : *line = 8;   *col = 17;      break;
          case 7 : *line = 8;   *col = 18;      break;
          case 8 : *line = 8;   *col = 20;      break;
          case 9 : *line = 7;   *col = 23;      break;
          case 10: *line = 9;   *col = 23;      break;
        }
     
}

fix_gallows(n)
int n;
{
        /** fix the pieces being eaten by the little characters as
            the execution progresses! **/

        register int line, col, fix;

        switch(n) {
          case 0 :      return;
          case 1 : line = 12;   col = 1;  fix = 3;  break;
          case 2 : line = 10;   col = 4;  fix = 3;  break;
          case 3 : line = 9;    col = 7;  fix = 3;  break;
          case 4 : line = 8;    col = 10; fix = 3;  break;
          case 5 : line = 8;    col = 16; fix = 1;  break;
          case 6 : line = 8;    col = 17; fix = 1;  break;
          case 7 : line = 8;    col = 18; fix = 2;  break;
          case 8 : line = 8;    col = 20; fix = 3;  break;
        }

        switch(fix) {
          case 3: PutLine(line, col, "___");    break;
          case 2: PutLine(line, col, "__");     break;
          case 1: PutLine(line, col, "_");      break;
        }
}

     
build_gallows()
{
        /** this is a macabre gallows...  **/
        PutLine(0,36,"- Hangman -");

        MoveCursor(2,0);
        printf("              ===========\n");
        printf("              |/        |\n");
        printf("              ||        #\n");
        printf("              ||        O\n");
        printf("              ||\n");
        printf("              ||\n");
        printf("          ____||_______TTT_________\n");
        printf("       ___|  |  |            |  |\n");
        printf("    ___|     |  |            |  |\n");
        printf("    |        |  |            |  |\n");
        printf("____|________|__|____________|__|_____\n");
        fflush(stdout);
}
