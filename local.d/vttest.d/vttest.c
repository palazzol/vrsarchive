/*
                               VTTEST.C

             Written nov 1983 - mar 1984 by Per Lindberg,
          Stockholm University Computer Center (QZ), Sweden.

                  THE MAD PROGRAMMER STRIKES AGAIN!

                   This software is (c) 1984 by QZ
               Non-commercial use and copying allowed.

This is a program to test the compatibility (or to demonstrate the
non-compatibility) of so-called "VT100-compatible" terminals. In
conformance of the good old hacker traditions, the only documentation
of this program is the source code itself. To understand it, you also
need a copy of the original VT100 manual from DEC.

This version of the program (which is the original) is written for the
Sargasso C compiler for the DECsystem-10. I would like to hear from
anyone who has made a UNIXification of it.

This program does not have support for all the different variations of
VT100, like VT125, VT131 or the new VT200 series. I might fix that
someday, but don't hold your breath... In the meantime, feel free to
add that yourself. Happy Hacking!

Comments and bug reports are welcome. I cannot guarantee any
bug fixes or improvements, but if I feel like it, I'll do it.
My adress is:

The COM system at QZ:    Per Lindberg QZ

Network-mail adress:     Per_Lindberg_QZ%QZCOM.MAILNET@MIT-MULTICS.ARPA

Real-world-mail address: Per Lindberg
                         Stockholm University Computing Center, QZ
			 Box 27 322
			 S - 102 54  Stockholm
			 SWEDEN

May your terminal be truly VT100 compatible!

                 /TMP

*/
#ifdef SYS5
#define USG
#endif
#ifdef SIII
#define USG
#endif
#ifdef USG
#define UNIX
#endif

/* Implementation dependent stuff for the Sargasso C compiler  */
#ifdef SARGASSO
#define _UNIXCON            /* Make UNIX-flavored I/O      */
#endif

#include <stdio.h>          /* Make UNIX-flavored I/O      */
#ifdef SARGASSO
/*#strings low                /* put strings in lowseg mem   */
                            /* so that we can modify them. */
#endif
#ifdef UNIX
#include <ctype.h>
#ifdef USG
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <fcntl.h>
struct termio sgttyOrg, sgttyNew;
#else
#include <sgtty.h>
struct sgttyb sgttyOrg, sgttyNew;
#endif
#endif

char inchar(), *instr(), *lookup();

struct table {
    int key;
    char *msg;
} paritytable[] = {
    { 1, "NONE" },
    { 4, "ODD"  },
    { 5, "EVEN" },
    { -1, "" }
},nbitstable[] = {
    { 1, "8" },
    { 2, "7" },
    { -1,"" }
},speedtable[] = {
    {   0,    "50" },
    {   8,    "75" },
    {  16,   "110" },
    {  24,   "132.5"},
    {  32,   "150" },
    {  40,   "200" },
    {  48,   "300" },
    {  56,   "600" },
    {  64,  "1200" },
    {  72,  "1800" },
    {  80,  "2000" },
    {  88,  "2400" },
    {  96,  "3600" },
    { 104,  "4800" },
    { 112,  "9600" },
    { 120, "19200" },
    { -1, "" }
};

int ttymode;

main() {

  int menuchoice;

  static char *mainmenu[] = {
      "Exit",
      "Test of cursor movements",
      "Test of screen features",
      "Test of character sets",
      "Test of double-sized characters",
      "Test of keyboard",
      "Test of terminal reports",
      "Test of VT52 mode",
      "Test of known bugs",
      ""
    };

  initterminal();
  do {
    menuchoice = menu(mainmenu, "Choose test type");
    switch (menuchoice) {
      case 1:  tst_movements();   break;
      case 2:  tst_screen();      break;
      case 3:  tst_characters();  break;
      case 4:  tst_doublesize();  break;
      case 5:  tst_keyboard();    break;
      case 6:  tst_reports();     break;
      case 7:  tst_vt52();        break;
      case 8:  tst_bugs();        break;
    }
  } while (menuchoice);
  bye();
}

tst_movements() {

  /* Test of:
     CUF (Cursor Forward)
     CUB (Cursor Backward)
     CUD (Cursor Down)      IND (Index)  NEL (Next Line)
     CUU (Cursor Up)        RI  (Reverse Index)
     CUP (Cursor Position)  HVP (Horizontal and Vertical Position)
     ED  (Erase in Display)
     EL  (Erase in Line)
     DECALN (Screen Alignment Display)
     <CR> <BS>
  */

  int row, col;

/*  decaln();  */
  ed(2);
  decaln();
  cup( 9,10); ed(1);
  cup(18,60); ed(0); el(1);
  cup( 9,71); el(0);
  for (row = 10; row <= 16; row++) {
    cup(row, 10); el(1);
    cup(row, 71); el(0);
  }
  cup(17,30); el(2);
  for (col = 1; col <= 80; col++) {
    hvp(24, col); printf("*");
    hvp( 1, col); printf("*");
  }
  cup(2,2);
  for (row = 2; row <= 23; row++) {
    printf("+");
    cub(1);
    ind();
  }
  cup(23,79);
  for (row = 23; row >=2; row--) {
    printf("+");
    cub(1); ri();
  }
  cup(2,1);
  for (row = 2; row <= 23; row++) {
    printf("*");  cup(row, 80);
    printf("*");
    cub(10);
    if (row < 10) nel();
    else          printf("\n");
  }
  cup(2,10);
  cub(42); cuf(2);
  for (col = 3; col <= 78; col++) {
    printf("+");
    cuf(0); cub(2); cuf(1);
  }
  cup(23,70);
  cuf(42); cub(2);
  for (col = 78; col >= 3; col--) {
    printf("+");
    cub(1); cuf(1); cub(0); printf("%c", 8);
  }
  cup( 1, 1); cuu(10); cuu(1); cuu(0);
  cup(24,80); cud(10); cud(1); cud(0);

  cup(10,12);
  for (row = 10; row <= 15; row++) {
    for (col = 12; col <= 69; col++) printf(" ");
    cud(1); cub(58);
  }
  cuu(5); cuf(1);
  printf("The screen should be cleared, and have a unbroken border");
  cup(12,13);
  printf("of *'s and +'s around the edge, and exactly in the");
  cup(13,13);
  printf("middle there should be a frame of E's around this text");
  cup(14,13);
  printf("with one (1) free position around it. ");
  pause1();
}

tst_screen() {

  /* Test of:
     - DECSTBM (Set Top and Bottom Margins)
     - TBC     (Tabulation Clear)
     - HTS     (Horizontal Tabulation Set)
     - SM RM   (Set/Reset mode): - 80/132 chars
                                 - Origin: Realtive/absolute
				 - Scroll: Smooth/jump
				 - Wraparound
     - SGR     (Select Graphic Rendition)
     - SM RM   (Set/Reset Mode) - Inverse
     - DECSC   (Save Cursor)
     - DECRC   (Restore Cursor)
  */

  int i, j, cset, row, col, down, soft, background;

  char *attr[5], *tststr;

  tststr = "*qx`";
  attr[0] = ";0";  attr[1] = ";1";  attr[2] = ";4";
  attr[3] = ";5";  attr[4] = ";7";

  ed(2);
  sm("?7");  /* Wrap Around ON */
  cup(1,1);
  for (col = 1; col <= 160; col++) printf("*");
  rm("?7");  /* Wrap Around OFF */
  cup(3,1);
  for (col = 1; col <= 160; col++) printf("*");
  sm("?7");  /* Wrap Around ON */
  cup(5,1);
  println("This should be three identical lines of *'s completely filling");
  println("the top of the screen without any empty lines between.");
  println("(Test of WRAP AROUND mode setting.)");
  pause1();

  ed(2);
  tbc(3);
  cup(1,1);
  for (col = 1; col <= 78; col += 3) {
    cuf(3); hts();
  }
  cup(1,4);
  for (col = 4; col <= 78; col += 6) {
    tbc(0); cuf(6);
  }
  cup(1,7); tbc(1); tbc(2); /* no-op */
  cup(1,1); for (col = 1; col <= 78; col += 6) printf("\t*");
  cup(2,2); for (col = 2; col <= 78; col += 6) printf("     *");
  cup(4,1);
  println("Test of TAB setting/resetting. These two lines");
  printf("%s", "should look the same. ");
  pause1();
  for (background = 0; background <= 1; background++) {
    if (background) rm("?5");
    else            sm("?5");
    sm("?3"); /* 132 cols */
    ed(2);    /* VT100 clears screen on SM3/RM3, but not obviously, so... */
    cup(1,1); tbc(3);
    for (col = 1; col <= 132; col += 8) {
      cuf(8); hts();
    }
    cup(1,1); for (col = 1; col <= 130; col += 10) printf("1234567890");
    printf("12");
    for (row = 3; row <= 20; row++) {
      cup(row,row);
      printf("This is 132 column mode, %s background.",
      background ? "dark" : "light");
    }
    pause1();
    rm("?3"); /* 80 cols */
    ed(2);    /* VT100 clears screen on SM3/RM3, but not obviously, so... */
    cup(1,1); for (col = 1; col <= 80; col += 10) printf("1234567890");
    for (row = 3; row <= 20; row++) {
      cup(row,row);
      printf("This is 80 column mode, %s background.",
      background ? "dark" : "light");
    }
    pause1();
  }
  ed(2);
  sm("?6"); /* Origin mode (relative) */
  for (soft = -1; soft <= 0; soft++) {
    if (soft) sm("?4");
    else      rm("?4");
    for (row = 12; row >= 1; row -= 11) {
      decstbm(row, 24-row+1);
      ed(2);
      for (down = 0; down >= -1; down--) {
        if (down) cuu(24);
	else      cud(24);
	for (i = 1; i <= 30; i++) {
	  printf("%s scroll %s region %d Line %d\n",
		 soft ? "Soft" : "Jump",
		 down ? "down" : "up",
		 2*(13-row), i);
	  if (down) { ri(); ri(); }
	}
      }
      pause1();
    }
  }
  ed(2);
  decstbm(23,24);
  printf(
  "\nOrigin mode test. This line should be at the bottom of the screen.");
  cup(1,1);
  printf("%s",
  "This line should be the one above the bottom of the screeen. ");
  pause1();
  ed(2);
  rm("?6"); /* Origin mode (absolute) */
  cup(24,1);
  printf(
  "Origin mode test. This line should be at the bottom of the screen.");
  cup(1,1);
  printf("%s", "This line should be at the top if the screen. ");
  pause1();
  decstbm(1,24);

  ed(2);
  cup( 1,20); printf("Graphic rendition test pattern:");
  cup( 4, 1); sgr("0");         printf("vanilla");
  cup( 4,40); sgr("0;1");       printf("bold");
  cup( 6, 6); sgr(";4");        printf("underline");
  cup( 6,45);sgr(";1");sgr("4");printf("bold underline");
  cup( 8, 1); sgr("0;5");       printf("blink");
  cup( 8,40); sgr("0;5;1");     printf("bold blink");
  cup(10, 6); sgr("0;4;5");     printf("underline blink");
  cup(10,45); sgr("0;1;4;5");   printf("bold underline blink");
  cup(12, 1); sgr("1;4;5;0;7"); printf("negative");
  cup(12,40); sgr("0;1;7");     printf("bold negative");
  cup(14, 6); sgr("0;4;7");     printf("underline negative");
  cup(14,45); sgr("0;1;4;7");   printf("bold underline negative");
  cup(16, 1); sgr("1;4;;5;7");  printf("blink negative");
  cup(16,40); sgr("0;1;5;7");   printf("bold blink negative");
  cup(18, 6); sgr("0;4;5;7");   printf("underline blink negative");
  cup(18,45); sgr("0;1;4;5;7"); printf("bold underline blink negative");
  sgr("");

  rm("?5"); /* Inverse video off */
  cup(23,1); el(0); printf("Dark background. "); pause1();
  sm("?5"); /* Inverse video */
  cup(23,1); el(0); printf("Light background. "); pause1();
  rm("?5");
  ed(2);
  cup(8,12); printf("normal");
  cup(8,24); printf("bold");
  cup(8,36); printf("underscored");
  cup(8,48); printf("blinking");
  cup(8,60); printf("reversed");
  cup(10,1); printf("stars:");
  cup(12,1); printf("line:");
  cup(14,1); printf("x'es:");
  cup(16,1); printf("diamonds:");
  for (cset = 0; cset <= 3; cset++) {
    for (i = 0; i <= 4; i++) {
    cup(10 + 2 * cset, 12 + 12 * i);
    sgr(attr[i]);
    if (cset == 0 || cset == 2) scs(0,'B');
    else                        scs(0,'0');
      for (j = 0; j <= 4; j++) {
        printf("%c", tststr[cset]);
      }
      decsc();
      cup(cset + 1, i + 1); sgr(""); scs(0,'B'); printf("A");
      decrc();
      for (j = 0; j <= 4; j++) {
        printf("%c", tststr[cset]);
      }
    }
  }
  sgr("0"); scs(0,'B'); cup(21,1);
  println("Test of the SAVE/RESTORE CURSOR feature. There should");
  println("be ten characters of each flavour, and a rectangle");
  println("of 5 x 4 A's filling the top left of the screen.");
  pause1();
}

tst_characters() {
  /* Test of:
     SCS    (Select character Set)
  */

  int i, j, g, cset;
  char chcode[5], *setmsg[5];

  chcode[0] = 'A';
  chcode[1] = 'B';
  chcode[2] = '0';
  chcode[3] = '1';
  chcode[4] = '2';
  setmsg[0] = "UK / national";
  setmsg[1] = "US ASCII";
  setmsg[2] = "Special graphics and line drawing";
  setmsg[3] = "Alternate character ROM standard characters";
  setmsg[4] = "Alternate character ROM special graphics";

  ed(2);
  cup(1,10); printf("Selected as G0 (with SI)");
  cup(1,48); printf("Selected as G1 (with SO)");
  for (cset = 0; cset <= 4; cset++) {
    scs(1,'B');
    cup(3 + 4 * cset, 1);
    sgr("1");
    printf("Character set %c (%s)",chcode[cset], setmsg[cset]);
    sgr("0");
    for (g = 0; g <= 1; g++) {
      scs(g,chcode[cset]);
      for (i = 1; i <= 3; i++) {
        cup(3 + 4 * cset + i, 10 + 38 * g);
        for (j = 0; j <= 31; j++) {
	  printf("%c", i * 32 + j);
	}
      }
    }
  }
  scs(1,'B');
  cup(24,1); printf("These are the installed character sets. ");
  pause1();
}

tst_doublesize() {
  /* Test of:
     DECSWL  (Single Width Line)
     DECDWL  (Double Width Line)
     DECDHL  (Double Height Line) (also implicit double width)
  */

  int i, w, w1;

  /* Print the test pattern in both 80 and 132 character width  */

  for(w = 0; w <= 1; w++) {
    w1 = 13 * w;

    ed(2);
    cup(1, 1);
    if (w) { sm("?3"); printf("132 column mode"); }
    else   { rm("?3"); printf(" 80 column mode"); }

    cup( 5, 3 + 2 * w1);
    printf("v------- left margin");

    cup( 7, 3 + 2 * w1);
    printf("This is a normal-sized line");
    decdhl(0); decdhl(1); decdwl(); decswl();

    cup( 9, 2 + w1);
    printf("This is a Double-width line");
    decswl(); decdhl(0); decdhl(1); decdwl();

    cup(11, 2 + w1);
    decdwl(); decswl(); decdhl(1); decdhl(0);
    printf("This is a Double-width-and-height line");
    cup(12, 2 + w1);
    decdwl(); decswl(); decdhl(0); decdhl(1);
    printf("This is a Double-width-and-height line");

    cup(14, 2 + w1);
    decdwl(); decswl(); decdhl(1); decdhl(0); el(2);
    printf("This is another such line");
    cup(15, 2 + w1);
    decdwl(); decswl(); decdhl(0); decdhl(1);
    printf("This is another such line");

    cup(17, 3 + 2 * w1);
    printf("^------- left margin");

    cup(21, 1);
    printf("This is not a double-width line");
    for (i = 0; i <= 1; i++) {
      cup(21,6);
      if (i) { printf("**is**"); decdwl(); }
      else   { printf("is not"); decswl(); }
      cup(23,1); pause1();
    }
  }

  rm("?3");
  ed(2);

  scs(0,'0');
  cup( 8,1); decdhl(0); printf("lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");
  cup( 9,1); decdhl(1); printf("lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");
  cup(10,1); decdhl(0); printf("x                                   x");
  cup(11,1); decdhl(1); printf("x                                   x");
  cup(12,1); decdhl(0); printf("x                                   x");
  cup(13,1); decdhl(1); printf("x                                   x");
  cup(14,1); decdhl(0); printf("x                                   x");
  cup(15,1); decdhl(1); printf("x                                   x");
  cup(16,1); decdhl(0); printf("mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj");
  cup(17,1); decdhl(1); printf("mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj");
  scs(0,'B'); sgr("1;5");
  cup(12,3); printf("The mad programmer strikes again!");
  cup(13,3); printf("The mad programmer strikes again!");
  sgr("0");
  cup(22,1);
  println("Another test pattern...  a frame with blinking bold text,");
  printf("%s", "all in double-height double-width size. ");
  pause1();

  decstbm(8,24);
  cup(8,1);	/* We are in absolute origin mode */
  for (i = 1; i <= 12; i++)
    ri();
  cup(1,1);
  printf("%s", "Exactly half of the box should remain. ");
  pause1();
}

tst_keyboard() {

/* Test of:
     - DECLL   (Load LEDs)
     - Keyboard return messages
     - SM RM   (Set/Reset Mode) - Cursor Keys
                                - Auto repeat
     - DECKPAM (Keypad Application Mode)
     - DECKPNM (Keypad Numeric Mode)

The standard VT100 keayboard layout:
 
                                                        UP   DN   LE  RI

ESC   1!   2@   3#   4$   5%   6^   7&   8*   9(   0)   -_   =+   `~  BS

TAB*    qQ   wW   eE   rR   tT   yY   uU   iI   oO   pP   [{   ]}      DEL

**   **   aA   sS   dD   fF   gG   hH   jJ   kK   lL   ;:   ,"   RETN  \|

**   ****   zZ   xX   cC   vV   bB   nN   mM   ,<   .>   /?   ****   LF

             ****************SPACE BAR****************

                                                           PF1 PF2 PF3 PF4

                                                           *7* *8* *9* *-*

                                                           *4* *5* *6* *,*

                                                           *1* *2* *3*

                                                           ***0*** *.* ENT
*/

  char *ledmsg[6], *ledseq[6];

  int  i, j, okflag;
  int  key1boardlayout;
  int  cur1keymode, fn1keymode;
  char kbdc;
  char *kbds = " ";
  char *curkeystr, *fnkeystr, *abmstr;
  char arptstring[500];

  static struct key {
      char c;
      int  row;
      int  col;
      char *symbol;
  } keytab [] = {
      { 27, 1,  0, "ESC" },
      { '1', 1,  6, "1" },    { '!', 1,  7, "!" },
      { '2', 1, 11, "2" },    { '@', 1, 12, "@" },
      { '3', 1, 16, "3" },    { '#', 1, 17, "#" },
      { '4', 1, 21, "4" },    { '$', 1, 22, "$" },
      { '5', 1, 26, "5" },    {'\%', 1, 27, "%" },
      { '6', 1, 31, "6" },    { '^', 1, 32, "^" },
      { '7', 1, 36, "7" },    { '&', 1, 37, "&" },
      { '8', 1, 41, "8" },    { '*', 1, 42, "*" },
      { '9', 1, 46, "9" },    { '(', 1, 47, "(" },
      { '0', 1, 51, "0" },    { ')', 1, 52, ")" },
      { '-', 1, 56, "-" },    { '_', 1, 57, "_" },
      { '=', 1, 61, "=" },    { '+', 1, 62, "+" },
      { '`', 1, 66, "`" },    { '~', 1, 67, "~" },
      {   8, 1, 70, "BS" },
      {   9, 2,  0, " TAB " },
      { 'q', 2,  8, "q" },    { 'Q', 2,  9, "Q" },
      { 'w', 2, 13, "w" },    { 'W', 2, 14, "W" },
      { 'e', 2, 18, "e" },    { 'E', 2, 19, "E" },
      { 'r', 2, 23, "r" },    { 'R', 2, 24, "R" },
      { 't', 2, 28, "t" },    { 'T', 2, 29, "T" },
      { 'y', 2, 33, "y" },    { 'Y', 2, 34, "Y" },
      { 'u', 2, 38, "u" },    { 'U', 2, 39, "U" },
      { 'i', 2, 43, "i" },    { 'I', 2, 44, "I" },
      { 'o', 2, 48, "o" },    { 'O', 2, 49, "O" },
      { 'p', 2, 53, "p" },    { 'P', 2, 54, "P" },
      { '[', 2, 58, "[" },    { '{', 2, 59, "{" },
      { ']', 2, 63, "]" },    { '}', 2, 64, "}" },
      { 127, 2, 71, "DEL" },
      { 'a', 3, 10, "a" },    { 'A', 3, 11, "A" },
      { 's', 3, 15, "s" },    { 'S', 3, 16, "S" },
      { 'd', 3, 20, "d" },    { 'D', 3, 21, "D" },
      { 'f', 3, 25, "f" },    { 'F', 3, 26, "F" },
      { 'g', 3, 30, "g" },    { 'G', 3, 31, "G" },
      { 'h', 3, 35, "h" },    { 'H', 3, 36, "H" },
      { 'j', 3, 40, "j" },    { 'J', 3, 41, "J" },
      { 'k', 3, 45, "k" },    { 'K', 3, 46, "K" },
      { 'l', 3, 50, "l" },    { 'L', 3, 51, "L" },
      { ';', 3, 55, ";" },    { ':', 3, 56, ":" },
      {'\'', 3, 60, "'" },    { '"', 3, 61,"\"" },
      {  13, 3, 65, "RETN"},
      {'\\', 3, 71,"\\" },    { '|', 3, 72, "|" },
      { 'z', 4, 12, "z" },    { 'Z', 4, 13, "Z" },
      { 'x', 4, 17, "x" },    { 'X', 4, 18, "X" },
      { 'c', 4, 22, "c" },    { 'C', 4, 23, "C" },
      { 'v', 4, 27, "v" },    { 'V', 4, 28, "V" },
      { 'b', 4, 32, "b" },    { 'B', 4, 33, "B" },
      { 'n', 4, 37, "n" },    { 'N', 4, 38, "N" },
      { 'm', 4, 42, "m" },    { 'M', 4, 43, "M" },
      { ',', 4, 47, "," },    { '<', 4, 48, "<" },
      { '.', 4, 52, "." },    { '>', 4, 53, ">" },
      { '/', 4, 57, "/" },    { '?', 4, 58, "?" },
      {  10, 4, 69, "LF" },
      { ' ', 5, 13, "                SPACE BAR                "},
      {'\0', 0,  0, ""  }
    };

  static struct natkey {
      char natc;
      int  natrow;
      int  natcol;
      char *natsymbol;
  } natkeytab [][29] = {
      {
        { '"', 1, 12, "\""},
        { '&', 1, 32, "&" },
        { '/', 1, 37, "/" },
        { '(', 1, 42, "(" },
        { ')', 1, 47, ")" },
        { '=', 1, 52, "=" },
        { '+', 1, 56, "+" },    { '?', 1, 57, "?" },
        { '`', 1, 61, "`" },    { '@', 1, 62, "@" },
        { '<', 1, 66, "<" },    { '>', 1, 67, ">" },
        { '}', 2, 58, "}" },    { ']', 2, 59, "]" },
        { '^', 2, 63, "^" },    { '~', 2, 64, "~" },
        { '|', 3, 55, "|" },    {'\\', 3, 56,"\\" },
        { '{', 3, 60, "{" },    { '[', 3, 61, "[" },
        {'\'', 3, 71, "'" },    { '*', 3, 72, "*" },
        { ',', 4, 47, "," },    { ';', 4, 48, ";" },
        { '.', 4, 52, "." },    { ':', 4, 53, ":" },
        { '-', 4, 57, "-" },    { '_', 4, 58, "_" },
        {'\0', 0,  0, ""  }
      },
      {
        { '"', 1, 12, "\""},
        { '&', 1, 32, "&" },
        { '/', 1, 37, "/" },
        { '(', 1, 42, "(" },
        { ')', 1, 47, ")" },
        { '=', 1, 52, "=" },
        { '+', 1, 56, "+" },    { '?', 1, 57, "?" },
        { '`', 1, 61, "`" },    { '@', 1, 62, "@" },
        { '<', 1, 66, "<" },    { '>', 1, 67, ">" },
        { '}', 2, 58, "}" },    { ']', 2, 59, "]" },
        { '~', 2, 63, "~" },    { '^', 2, 64, "^" },
        { '|', 3, 55, "|" },    {'\\', 3, 56,"\\" },
        { '{', 3, 60, "{" },    { '[', 3, 61, "[" },
        {'\'', 3, 71, "'" },    { '*', 3, 72, "*" },
        { ',', 4, 47, "," },    { ';', 4, 48, ";" },
        { '.', 4, 52, "." },    { ':', 4, 53, ":" },
        { '-', 4, 57, "-" },    { '_', 4, 58, "_" },
        {'\0', 0,  0, ""  }
      }
  };

  static struct curkey {
      char *curkeymsg[3];
      int  curkeyrow;
      int  curkeycol;
      char *curkeysymbol;
      char *curkeyname;
  } curkeytab [] = {

      /* A Reset, A Set,  VT52  */

      {{"\033[A","\033OA","\033A"}, 0, 56, "UP",  "Up arrow"   },
      {{"\033[B","\033OB","\033B"}, 0, 61, "DN",  "Down arrow" },
      {{"\033[D","\033OD","\033D"}, 0, 66, "LT",  "Left arrow" },
      {{"\033[C","\033OC","\033C"}, 0, 71, "RT",  "Right arrow"},
      {{"",      "",       ""     }, 0,  0, "",    "" }
  };

  static struct fnkey {
      char *fnkeymsg[4];
      int  fnkeyrow;
      int  fnkeycol;
      char *fnkeysymbol;
      char *fnkeyname;
  } fnkeytab [] = {

      /* ANSI-num,ANSI-app,VT52-nu,VT52-ap,  r, c,  symb   name         */

      {{"\033OP","\033OP","\033P","\033P" }, 6, 59, "PF1", "PF1"        },
      {{"\033OQ","\033OQ","\033Q","\033Q" }, 6, 63, "PF2", "PF2"        },
      {{"\033OR","\033OR","\033R","\033R" }, 6, 67, "PF3", "PF3"        },
      {{"\033OS","\033OS","\033S","\033S" }, 6, 71, "PF4", "PF4"        },
      {{"7",     "\033Ow","7",    "\033?w"}, 7, 59, " 7 ", "Numeric 7"  },
      {{"8",     "\033Ox","8",    "\033?x"}, 7, 63, " 8 ", "Numeric 8"  },
      {{"9",     "\033Oy","9",    "\033?y"}, 7, 67, " 9 ", "Numeric 9"  },
      {{"-",     "\033Om","-",    "\033?m"}, 7, 71, " - ", "Minus"      },
      {{"4",     "\033Ot","4",    "\033?t"}, 8, 59, " 4 ", "Numeric 4"  },
      {{"5",     "\033Ou","5",    "\033?u"}, 8, 63, " 5 ", "Numeric 5"  },
      {{"6",     "\033Ov","6",    "\033?v"}, 8, 67, " 6 ", "Numeric 6"  },
      {{",",     "\033Ol",",",    "\033?l"}, 8, 71, " , ", "Comma"      },
      {{"1",     "\033Oq","1",    "\033?q"}, 9, 59, " 1 ", "Numeric 1"  },
      {{"2",     "\033Or","2",    "\033?r"}, 9, 63, " 2 ", "Numeric 2"  },
      {{"3",     "\033Os","3",    "\033?s"}, 9, 67, " 3 ", "Numeric 3"  },
      {{"0",     "\033Op","0",    "\033?p"},10, 59,"   O   ","Numeric 0"},
      {{".",     "\033On",".",    "\033?n"},10, 67, " . ", "Point"      },
      {{"\015",  "\033OM","\015", "\033?M"},10, 71, "ENT", "ENTER"      },
      {{"","","",""},       0,  0, "",    ""           }
    };

  static struct ckey {
      int  ccount;
      char *csymbol;
  } ckeytab [] = {
      { 0, "NUL (CTRL-@ or CTRL-Space)" },
      { 0, "SOH (CTRL-A)" },
      { 0, "STX (CTRL-B)" },
      { 0, "ETX (CTRL-C)" },
      { 0, "EOT (CTRL-D)" },
      { 0, "ENQ (CTRL-E)" },
      { 0, "ACK (CTRL-F)" },
      { 0, "BEL (CTRL-G)" },
      { 0, "BS  (CTRL-H) (BACK SPACE)" },
      { 0, "HT  (CTRL-I) (TAB)" },
      { 0, "LF  (CTRL-J) (LINE FEED)" },
      { 0, "VT  (CTRL-K)" },
      { 0, "FF  (CTRL-L)" },
      { 0, "CR  (CTRL-M) (RETURN)" },
      { 0, "SO  (CTRL-N)" },
      { 0, "SI  (CTRL-O)" },
      { 0, "DLE (CTRL-P)" },
      { 0, "DC1 (CTRL-Q) (X-On)" },
      { 0, "DC2 (CTRL-R)" },
      { 0, "DC3 (CTRL-S) (X-Off)" },
      { 0, "DC4 (CTRL-T)" },
      { 0, "NAK (CTRL-U)" },
      { 0, "SYN (CTRL-V)" },
      { 0, "ETB (CTRL-W)" },
      { 0, "CAN (CTRL-X)" },
      { 0, "EM  (CTRL-Y)" },
      { 0, "SUB (CTRL-Z)" },
      { 0, "ESC (CTRL-[) (ESCAPE)" },
      { 0, "FS  (CTRL-\\ or CTRL-? or CTRL-_)" },
      { 0, "GS  (CTRL-])" },
      { 0, "RS  (CTRL-^ or CTRL-~ or CTRL-`)" },
      { 0, "US  (CTRL-_ or CTRL-?)" }
  };

  static char *keyboardmenu[] = {
      "Standard American ASCII layout",
      "Swedish national layout D47",
      "Swedish national layout E47",
      /* add new keyboard layouts here */
      ""
    };

  static char *curkeymodes[3] = {
      "ANSI / Cursor key mode RESET",
      "ANSI / Cursor key mode SET",
      "VT52 Mode"
  };

  static char *fnkeymodes[4] = {
      "ANSI Numeric mode",
      "ANSI Application mode",
      "VT52 Numeric mode",
      "VT52 Application mode"
  };

  ledmsg[0] = "L1 L2 L3 L4"; ledseq[0] = "1;2;3;4";
  ledmsg[1] = "   L2 L3 L4"; ledseq[1] = "1;0;4;3;2";
  ledmsg[2] = "   L2 L3";    ledseq[2] = "1;4;;2;3";
  ledmsg[3] = "L1 L2";       ledseq[3] = ";;2;1";
  ledmsg[4] = "L1";          ledseq[4] = "1";
  ledmsg[5] = "";            ledseq[5] = "";

  ed(2);
  cup(10,1);
  println("These LEDs (\"lamps\") on the keyboard should be on:");
  for (i = 0; i <= 5; i++) {
    cup(10,52); el(0); printf("%s", ledmsg[i]);
    decll("0");
    decll(ledseq[i]);
    cup(12,1); pause1();
  }

  ed(2);
  cup(10,1);
  println("Test of the AUTO REPEAT feature");
  println("");
  println("Hold down an alphanumeric key for a while, then push RETURN.");
  printf("%s", "Auto Repeat OFF: ");
  rm("?8");
  scanf("%s", arptstring);
#ifndef UNIX
  readnl();
#endif
  if (strlen(arptstring) == 0)      println("No characters read!??");
  else if (strlen(arptstring) == 1) println("OK.");
  else                              println("Too many character read.");
  println("");
  println("Hold down an alphanumeric key for a while, then push RETURN.");
  printf("%s", "Auto Repeat ON: ");
  sm("?8");
  scanf("%s", arptstring);
#ifndef UNIX
  readnl();
#endif
  if (strlen(arptstring) == 0)      println("No characters read!??");
  else if (strlen(arptstring) == 1) println("Not enough character read.");
  else                              println("OK.");
  println("");
  pause1();

  key1boardlayout = menu(keyboardmenu, "Choose keyboard layout");
  if (key1boardlayout) {
    key1boardlayout--;
    for (j = 0; natkeytab[key1boardlayout][j].natc != '\0'; j++) {
      for (i = 0; keytab[i].c != '\0'; i++) {
	if (keytab[i].row == natkeytab[key1boardlayout][j].natrow &&
	    keytab[i].col == natkeytab[key1boardlayout][j].natcol) {
	  keytab[i].c = natkeytab[key1boardlayout][j].natc;
	  keytab[i].symbol = natkeytab[key1boardlayout][j].natsymbol;
	  break;
	}
      }
    }
  }

  ed(2);
  for (i = 0; keytab[i].c != '\0'; i++) {
    cup(1 + 2 * keytab[i].row, 1 + keytab[i].col);
    sgr("7");
    printf("%s", keytab[i].symbol);
    sgr("");
  }
  cup(22,1);
#ifdef UNIX
#ifdef USG
  sgttyNew.c_iflag &= ~ICRNL;
  sgttyNew.c_lflag &= ~(ICANON|ISIG|ECHO);
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags &= ~CRMOD;
  sgttyNew.sg_flags &= ~ECHO;
  stty(0, &sgttyNew);
#endif
#endif
  inflush();
  printf("Press each key, both shifted and unshifted. Finish with RETURN:");
  do { /* while (kbdc != 13) */
    cup(23,1); kbdc = inchar();
    cup(23,1); el(0);
    sprintf(kbds, "%c", kbdc);
    chrprint(kbds);
    for (i = 0; keytab[i].c != '\0'; i++) {
      if (keytab[i].c == kbdc) {
        cup(1 + 2 * keytab[i].row, 1 + keytab[i].col);
	printf("%s", keytab[i].symbol);
	break;
      }
    }
  } while (kbdc != 13);
#ifdef SARGASSO
  inchar();  /* Local hack: Read LF that TOPS-10 adds to CR */
#endif
  cup(23,1); el(0);

  for (cur1keymode = 0; cur1keymode <= 2; cur1keymode++) {
    if (cur1keymode) sm("?1");
    else            rm("?1");
    for (i = 0; curkeytab[i].curkeysymbol[0] != '\0'; i++) {
      cup(1 + 2 * curkeytab[i].curkeyrow, 1 + curkeytab[i].curkeycol);
      sgr("7");
      printf("%s", curkeytab[i].curkeysymbol);
      sgr("");
    }
    cup(20,1); printf("<%s>%20s", curkeymodes[cur1keymode], "");
    cup(22,1); el(0);
    cup(22,1); printf("%s", "Press each cursor key. Finish with TAB.");
    for(;;) {
      cup(23,1);
      if (cur1keymode == 2) rm("?2"); /* VT52 mode */
      curkeystr = instr();
      esc("<");                      /* ANSI mode */
      cup(23,1); el(0);
      cup(23,1); chrprint(curkeystr);
      if (!strcmp(curkeystr,"\t")) break;
      for (i = 0; curkeytab[i].curkeysymbol[0] != '\0'; i++) {
	if (!strcmp(curkeystr,curkeytab[i].curkeymsg[cur1keymode])) {
	  sgr("7");
	  printf(" (%s key) ", curkeytab[i].curkeyname);
	  sgr("");
	  cup(1 + 2 * curkeytab[i].curkeyrow,
	      1 + curkeytab[i].curkeycol);
	  printf("%s", curkeytab[i].curkeysymbol);
	  break;
	}
      }
      if (i == sizeof(curkeytab) / sizeof(struct curkey) - 1) {
	sgr("7");
	printf("%s", " (Unknown cursor key) ");
	sgr("");
      }
    }
  }

  for (fn1keymode = 0; fn1keymode <= 3; fn1keymode++) {
    for (i = 0; fnkeytab[i].fnkeysymbol[0] != '\0'; i++) {
      cup(1 + 2 * fnkeytab[i].fnkeyrow, 1 + fnkeytab[i].fnkeycol);
      sgr("7");
      printf("%s", fnkeytab[i].fnkeysymbol);
      sgr("");
    }
    cup(20,1); printf("<%s>%20s", fnkeymodes[fn1keymode], "");
    cup(22,1); el(0);
    cup(22,1); printf("%s", "Press each function key. Finish with TAB.");
    for(;;) {
      cup(23,1);
      if (fn1keymode >= 2)  rm("?2");    /* VT52 mode */
      if (fn1keymode % 2)   deckpam();   /* Application mode */
      else                 deckpnm();	/* Numeric mode     */
      fnkeystr = instr();
      esc("<");				/* ANSI mode */
      cup(23,1); el(0);
      cup(23,1); chrprint(fnkeystr);
      if (!strcmp(fnkeystr,"\t")) break;
      for (i = 0; fnkeytab[i].fnkeysymbol[0] != '\0'; i++) {
	if (!strcmp(fnkeystr,fnkeytab[i].fnkeymsg[fn1keymode])) {
	  sgr("7");
	  printf(" (%s key) ", fnkeytab[i].fnkeyname);
	  sgr("");
	  cup(1 + 2 * fnkeytab[i].fnkeyrow, 1 + fnkeytab[i].fnkeycol);
	  printf("%s", fnkeytab[i].fnkeysymbol);
	  break;
	}
      }
      if (i == sizeof(fnkeytab) / sizeof(struct fnkey) - 1) {
	sgr("7");
	printf("%s", " (Unknown function key) ");
	sgr("");
      }
    }
  }

#ifdef UNIX
#ifdef USG
  sgttyNew.c_iflag |= ICRNL;
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags |= CRMOD;
  stty(0, &sgttyNew);
#endif
#endif
  ed(2);
  cup(5,1);
  println("Finally, a check of the ANSWERBACK MESSAGE, which can be sent");
  println("by pressing CTRL-BREAK. The answerback message can be loaded");
  println("in SET-UP B by pressing SHIFT-A and typing e.g.");
  println("");
  println("         \" H e l l o , w o r l d Return \"");
  println("");
  println("(the double-quote characters included).  Do that, and then try");
  println("to send an answerback message with CTRL-BREAK.  If it works,");
  println("the answerback message should be displayed in reverse mode.");
  println("Finish with a single RETURN.");

#ifdef UNIX
#ifdef USG
  sgttyNew.c_iflag &= ~ICRNL;
  sgttyNew.c_lflag &= ~(ICANON|ISIG|ECHO);
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags &= ~CRMOD;
  stty(0, &sgttyNew);
#endif
#endif
  do {
    cup(17,1);
    inflush();
    abmstr = instr();
    cup(17,1);
    el(0);
    chrprint(abmstr);
  } while (strcmp(abmstr,"\r"));

  ed(2);
  for (i = 0; i < 32; i++) {
    cup(1 + (i % 16), 1 + 40 * (i / 16));
    sgr("7");
    printf("%s", ckeytab[i].csymbol);
    sgr("0");
  }
  cup(19,1);
#ifdef UNIX
#ifdef USG
  sgttyNew.c_iflag |= ICRNL;
  sgttyNew.c_lflag |= ICANON|ISIG|ECHO;
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags |= CRMOD;
  stty(0, &sgttyNew);
#endif
#endif
  println(
  "Push each CTRL-key TWICE. Note that you should be able to send *all*");
  println(
  "CTRL-codes twice, including CTRL-S (X-Off) and CTRL-Q (X-Off)!");
  println(
  "Finish with DEL (also called DELETE or RUB OUT), or wait 1 minute.");
#ifdef UNIX
#ifdef USG
  sgttyNew.c_iflag &= ~(ICRNL|IXON);
  sgttyNew.c_lflag &= ~(ICANON|ISIG|ECHO);
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags &= ~CBREAK;
  sgttyNew.sg_flags |= RAW;
  stty(0, &sgttyNew);
#endif
#endif
  ttybin(1);
  do {
    cup(23,1); kbdc = inchar();
    cup(23,1); el(0);
    if (kbdc < 32) printf("  %s", ckeytab[kbdc].csymbol);
    else {
      sprintf(kbds, "%c", kbdc);
      chrprint(kbds);
      printf("%s", " -- not a CTRL key");
    }
    if (kbdc < 32) ckeytab[kbdc].ccount++;
    if (ckeytab[kbdc].ccount == 2) {
      cup(1 + (kbdc % 16), 1 + 40 * (kbdc / 16));
      printf("%s", ckeytab[kbdc].csymbol);
    }
  } while (kbdc != '\177');
#ifdef UNIX
#ifdef USG
  sgttyNew.c_iflag |= ICRNL|IXON;
  sgttyNew.c_lflag &= ~ICANON;
  sgttyNew.c_lflag |= ISIG|ECHO;
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags &= ~RAW;
  sgttyNew.sg_flags |= ECHO;
  sgttyNew.sg_flags |= CBREAK;
  stty(0, &sgttyNew);
#endif
#endif
  ttybin(0);
  cup(24,1);
  okflag = 1;
  for (i = 0; i < 32; i++) if (ckeytab[i].ccount < 2) okflag = 0;
  if (okflag) printf("%s", "OK. ");
  else        printf("%s", "You have not been able to send all CTRL keys! ");
  pause1();
}

tst_reports() {
  /* Test of:
       <ENQ>       (AnswerBack Message)
       SM RM       (Set/Reset Mode) - LineFeed / Newline
       DSR         (Device Status Report)
       DA          (Device Attributes)
       DECREQTPARM (Request Terminal Parameters)
  */

  int parity, nbits, xspeed, rspeed, clkmul, flags;
  int i, reportpos;
  char *report, *report2;
  static char *attributes[][2] = {
    { "\033[?1;0c",   "No options (vanilla VT100)" },
    { "\033[?1;1c",   "STP" },
    { "\033[?1;2c",   "AVO (could be a VT102)" },
    { "\033[?1;3c",   "STP and AVO" },
    { "\033[?1;4c",   "GPO" },
    { "\033[?1;5c",   "STP and GPO" },
    { "\033[?1;6c",   "AVO and GPO" },
    { "\033[?1;7c",   "STP, AVO and GPO" },
    { "\033[?12;5c",  "VT125" },
    { "\033[?12;7c",  "VT125 with AVO" },
    { "\033[?7c",     "VT131" },
    { "\033[?5;0c",   "VK100 (GIGI)" },
    { "\033[?5c",     "VK100 (GIGI)" },
    { "", "" }
  };

#ifdef UNIX
#ifdef USG
  sgttyNew.c_lflag &= ~(ICANON|ISIG|ECHO);
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags &= ~ECHO;
  sgttyNew.sg_flags |= RAW;
  stty(0, &sgttyNew);
#endif
#endif
  ed(2);
  cup(5,1);
  println("This is a test of the ANSWERBACK MESSAGE. (To load the A.B.M.");
  println("see the TEST KEYBOARD part of this program). Below here, the");
  println("current answerback message in your terminal should be");
  println("displayed. Finish this test with RETURN.");
  cup(10,1);
  inflush();
  printf("%c", 5); /* ENQ */
  report = instr();
  cup(10,1);
  chrprint(report);
  cup(12,1);
  pause1();

  ed(2);
  cup(1,1);
  println("Test of LineFeed/NewLine mode.");
  cup(3,1);
  sm("20");
#ifdef UNIX
#ifdef USG
  sgttyNew.c_iflag &= ~ICRNL;
  sgttyNew.c_lflag &= ~(ICANON|ISIG|ECHO);
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags &= ~CRMOD;
  stty(0, &sgttyNew);
#endif
#endif
  printf("NewLine mode set. Push the RETURN key: ");
  report = instr();
  cup(4,1);
  el(0);
  chrprint(report);
  if (!strcmp(report, "\015\012")) printf(" -- OK");
  else                             printf(" -- Not expected");
  cup(6,1);
  rm("20");
  printf("NewLine mode reset. Push the RETURN key: ");
  report = instr();
  cup(7,1);
  el(0);
  chrprint(report);
  if (!strcmp(report, "\015")) printf(" -- OK");
  else                         printf(" -- Not expected");
  cup(9,1);
#ifdef UNIX
#ifdef USG
  sgttyNew.c_iflag |= ICRNL;
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags |= CRMOD;
  stty(0, &sgttyNew);
#endif
#endif
  pause1();

  ed(2);
  cup(1,1);
  printf("Test of Device Status Report 5 (report terminal status).");
  cup(2,1);
  dsr(5);
  report = instr();
  cup(2,1);
  el(0);
  printf("Report is: ");
  chrprint(report);
  if      (!strcmp(report,"\033[0n")) printf(" -- means \"TERMINAL OK\"");
  else if (!strcmp(report,"\033[3n")) printf(" -- means \"TERMINAL OK\"");
  else                                printf(" -- Unknown response!");

  cup(4,1);
  println("Test of Device Status Report 6 (report cursor position).");
  cup(5,1);
  dsr(6);
  report = instr();
  cup(5,1);
  el(0);
  printf("Report is: ");
  chrprint(report);
  if (!strcmp(report,"\033[5;1R")) printf(" -- OK");
  else                             printf(" -- Unknown response!");

  cup(7,1);
  println("Test of Device Attributes report (what are you)");
  cup(8,1);
  da(0);
  report = instr();
  cup(8,1);
  el(0);
  printf("Report is: ");
  chrprint(report);
  for (i = 0; *attributes[i][0] != '\0'; i++) {
    if (!strcmp(report,attributes[i][0])) break;
  }
  if (*attributes[i][0] == '\0')
  printf(" -- Unknown response, refer to the manual");
  else {
    printf(" -- means %s", attributes[i][1]);
    if (i) {
      cup(9,1);
      println("Legend: STP = Processor Option");
      println("        AVO = Advanced Video Option");
      println("        GPO = Graphics Processor Option");
    }
  }

  cup(13,1);
  println("Test of the \"Request Terminal Parameters\" feature, argument 0.");
  cup(14,1);
  decreqtparm(0);
  report = instr();
  cup(14,1);
  el(0);
  printf("Report is: ");
  chrprint(report);
  if (strlen(report) < 16
   || report[0] != '\033'
   || report[1] != '['
   || report[2] != '2'
   || report[3] != ';')
  println(" -- Bad format");
  else {
    reportpos = 4;
    parity = scanto(report, &reportpos, ';');
    nbits  = scanto(report, &reportpos, ';');
    xspeed = scanto(report, &reportpos, ';');
    rspeed = scanto(report, &reportpos, ';');
    clkmul = scanto(report, &reportpos, ';');
    flags  = scanto(report, &reportpos, 'x');
    if (parity == 0 || nbits == 0 || clkmul == 0) println(" -- Bad format");
    else                                          println(" -- OK");
    printf(
    "This means: Parity %s, %s bits, xmitspeed %s, recvspeed %s.\n",
    lookup(paritytable, parity),
    lookup(nbitstable, nbits),
    lookup(speedtable, xspeed),
    lookup(speedtable, rspeed));
    printf("(CLoCk MULtiplier = %d, STP option flags = %d)\n", clkmul, flags);
  }

  cup(18,1);
  println("Test of the \"Request Terminal Parameters\" feature, argument 1.");
  cup(19,1);
  decreqtparm(1);	/* Does the same as decreqtparm(0), reports "3" */
  report2 = instr();
  cup(19,1);
  el(0);
  printf("Report is: ");
  chrprint(report2);
  if (strlen(report2) < 3
   || report2[2] != '3')
  println(" -- Bad format");
  else {
    report2[2] = '2';
    if (!strcmp(report,report2)) println(" -- OK");
    else                         println(" -- Bad format");
  }
  cup(24,1);
  pause1();
#ifdef UNIX
#ifdef USG
  sgttyNew.c_lflag |= ECHO|ISIG;
  ioctl(0, TCSETA, &sgttyNew);
#else
  sgttyNew.sg_flags |= ECHO;
  sgttyNew.sg_flags &= ~RAW;
  stty(0, &sgttyNew);
#endif
#endif
}

tst_vt52() {

  static struct rtabl {
      char *rcode;
      char *rmsg;
  } resp1onstable[] = {
      { "\033/K", " -- OK (means Standard VT52)" },
      { "\033/Z", " -- OK (means VT100 emulating VT52)" },
      { "",       " -- Unknown response"}
  };

  int i,j;
  char *response;

  rm("?2");  /* Reset ANSI (VT100) mode, Set VT52 mode	*/
  esc("H");  /* Cursor home	*/
  esc("J");  /* Erase to end of screen	*/
  esc("H");  /* Cursor home	*/
  for (i = 0; i <= 23; i++) {
    for (j = 0; j <= 9; j++)
    printf("%s", "FooBar ");
    println("Bletch");
  }
  esc("H");  /* Cursor home	*/
  esc("J");  /* Erase to end of screen	*/

  vt52cup(7,47);
  printf("nothing more.");
  for (i = 1; i <= 10; i++) printf("THIS SHOULD GO AWAY! ");
  for (i = 1; i <= 5; i++) {
    vt52cup(1,1);
    printf("%s", "Back scroll (this should go away)");
    esc("I"); 		/* Reverse LineFeed (with backscroll!)	*/
  }
  vt52cup(12,60);
  esc("J");  /* Erase to end of screen	*/
  for (i = 2; i <= 6; i++) {
    vt52cup(i,1);
    esc("K");		/* Erase to end of line	*/
  }

  for (i = 2; i <= 23; i++) {
    vt52cup(i,70); printf("%s", "**Foobar");
  }
  vt52cup(23,10); 
  for (i = 23; i >= 2; i--) {
    printf("%s", "*");
    printf("%c", 8);	/* BS */
    esc("I");		/* Reverse LineFeed (LineStarve)	*/
  }
  vt52cup(1,70);
  for (i = 70; i >= 10; i--) {
    printf("%s", "*");
    esc("D"); esc("D");	/* Cursor Left */
  }
  vt52cup(24,10);
  for (i = 10; i <= 70; i++) {
    printf("%s", "*");
    printf("%c", 8);	/* BS */
    esc("C");		/* Cursor Right	*/
  }
  vt52cup(2,11);
  for (i = 2; i <= 23; i++) {
    printf("%s", "!");
    printf("%c", 8);	/* BS */
    esc("B");		/* Cursor Down	*/
  }
  vt52cup(23,69);
  for (i = 23; i >= 2; i--) {
    printf("%s", "!");
    printf("%c", 8);	/* BS */
    esc("A");		/* Cursor Up	*/
  }
  for (i = 2; i <= 23; i++) {
    vt52cup(i,71);
    esc("K");		/* Erase to end of line	*/
  }

  vt52cup(10,16);
  printf("%s", "The screen should be cleared, and have a centered");
  vt52cup(11,16);
  printf("%s", "rectangle of \"*\"s with \"!\"s on the inside to the");
  vt52cup(12,16);
  printf("%s", "left and right. Only this, and");
  vt52cup(13,16);
  pause1();

  esc("H");  /* Cursor home	*/
  esc("J");  /* Erase to end of screen	*/
  printf("%s", "This is the normal character set:");
  for (j =  0; j <=  1; j++) {
    vt52cup(3 + j, 16);
    for (i = 0; i <= 47; i++)
    printf("%c", 32 + i + 48 * j);
  }
  vt52cup(6,1);
  printf("%s", "This is the special graphics character set:");
  esc("F");	/* Select Special Graphics character set	*/
  for (j =  0; j <=  1; j++) {
    vt52cup(8 + j, 16);
    for (i = 0; i <= 47; i++)
    printf("%c", 32 + i + 48 * j);
  }
  esc("G");	/* Select ASCII character set	*/
  vt52cup(12,1);
  pause1();

  esc("H");  /* Cursor home	*/
  esc("J");  /* Erase to end of screen	*/
  println("Test of terminal response to IDENTIFY command");
  esc("Z");	/* Identify	*/
  response = instr();
  println("");
  printf("Response was");
  esc("<");  /* Enter ANSI mode (VT100 mode) */
  chrprint(response);
  for(i = 0; resp1onstable[i].rcode[0] != '\0'; i++)
    if (!strcmp(response, resp1onstable[i].rcode))
      break;
  printf("%s", resp1onstable[i].rmsg);
  println("");
  println("");
  pause1();
}

tst_bugs() {
  /*
   * Test of
   *	VT100 "Wrap around / cursor position 80" bug.
   *	VT100 "Toggle origin mode, forget rest" bug.
   *	VT100 "Scroll while toggling softscroll" bug.
   *
   *	- RIS    (Reset to Initial State)
   *	- DECTST (invoke terminal test)
   */

  int i, row, col;

  ed (2);
  cup (1, 1);
  for (col = 1; col <= 79; col++)
      printf ("+");
  for (row = 1; row <= 24; row++) {
      hvp (row, 80);
      printf ("*");
  }
  cup (10, 10);
  printf ("This illustrates the \"wrap around bug\" which exists on a");
  cup (11, 10);
  printf ("standard VT100. On the top of the screen there should be");
  cup (12, 10);
  printf ("a row of +'s, and the rightmost column should be filled");
  cup (13, 10);
  printf ("with *'s. But if the bug is present, some of the *'s may");
  cup (14, 10);
  printf ("be placed in other places, e.g. in the leftmost column,");
  cup (15, 10);
  printf ("and the top line of +'s may be scrolled away.");
  cup (17, 10);
  printf ("Of course, a good VT100-compatible terminal should have");
  cup (18, 10);
  printf ("means of shutting this bug off (if it has it at all!)");
  cup (20, 10);
  pause1();

 /*
  *  VT100 "toggle origin mode, forget rest" bug.  If you try to set
  *	(or clear) parameters and one of them is the "origin mode"
  *	("?6") parameter, parameters that appear after the "?6"
  *	remain unaffected.  This is also true on CIT-101 terminals.
  */
  ed (2);
  sm ("?5");				/* Set reverse mode		*/
  sm ("?3");				/* Set 132 column mode		*/
  println("Test VT100 'Toggle origin mode, forget rest' bug, part 1.");
  println("The screen should be in reverse, 132 column mode.");
  pause1();
  ed (2);
  rm ("?6;5;3");		/* Reset (origin, reverse, 132 col)	*/
  println("Test VT100 'Toggle origin mode, forget rest' bug, part 2.\n");
  println("The screen should be in non-reverse, 80 column mode.");
  printf("%s", "(Otherwise, RESET the terminal).  ");
  pause1();
  /*
   * Test VT100 "Scroll while toggling softscroll" bug:
   * The cursor may disappear entirely after you type in a few carriage
   * returns.  It may reappear, moving UP the screen.  You may see
   * multiple copies of some lines.  You must RESET the terminal to
   * cure the problem.
   */
  ed (2);
  cup (10, 1);
  printf ("Next is a test of the VT100 'Scroll while toggle softscroll'\n");
  printf ("bug.  The cursor may disappear, or move UP the screen, or\n");
  printf ("multiple copies of some lines may appear.  After this test,\n");
  printf ("may want to RESET the terminal.\n\n\n");
  /*
   * Invoke the bug
   */
  esc ("[24H");				/* Simplified cursor movement	*/
  sm ("?4");				/* Set soft scroll		*/
  nel ();				/* "NextLine", move down	*/
  rm ("?4");				/* Reset soft scroll		*/
  nel ();				/* "NextLine", move down	*/
  for (i = 1; i <= 10; i++) {		/* Show the bug			*/
      printf ("Softscroll bug test, line %d.  ", i);
      pause1();
  }
  /*
   * Get rid of the bug.
   */
  printf ("End of softscroll test.  The terminal will now be RESET. ");
  pause1();
  ris();
#ifdef SARGASSO
  sleep(5000);		/* Wait 5.0 seconds */
#endif
#ifdef UNIX
  sleep(5);
#endif
  println("The terminal is now RESET. Next, the built-in confidence test");
  printf("%s", "will be invoked. ");
  pause1();
  ed(2);
  dectst(1);
#ifdef SARGASSO
  sleep(5000);		/* Wait 5.0 seconds */
#endif
#ifdef UNIX
  sleep(5);
#endif
  cup(5,1);
  println("If the built-in confidence test found any errors, a code");
  printf("%s", "is visible above. ");
  pause1();
}

initterminal() {

#ifdef UNIX
  setbuf(stdout,NULL);
#ifdef USG
  ioctl(0, TCGETA, &sgttyOrg);
  ioctl(0, TCGETA, &sgttyNew);
  sgttyNew.c_lflag &= ~ICANON;
  sgttyNew.c_lflag |= ISIG;
  sgttyNew.c_cc[VMIN]  = 1;
  sgttyNew.c_cc[VTIME] = 0;
  ioctl(0, TCSETA, &sgttyNew);
  close(2);
  open("/dev/tty",O_RDWR|O_NDELAY);
#else
  setbuf(stdout,NULL);
  gtty(0,&sgttyOrg);
  gtty(0,&sgttyNew);
  sgttyNew.sg_flags |= CBREAK;
  stty(0,&sgttyNew);
#endif
#endif
#ifdef SARGASSO
  /* Set up neccesary TOPS-10 terminal parameters	*/

  trmop(02002, 0);	/* tty no tape	*/
  trmop(02003, 1);	/* tty lc	*/
  trmop(02005, 1);	/* tty tab	*/
  trmop(02010, 1);	/* tty no crlf	*/
  trmop(02020, 0);	/* tty no tape	*/
  trmop(02021, 1);	/* tty page	*/
  trmop(02025, 0);	/* tty blanks	*/
  trmop(02026, 1);	/* tty no alt	*/
  trmop(02040, 1);	/* tty defer	*/
  trmop(02041, `VT100`);	/* tty type vt100	*/
#endif
  /* Set up my personal prejudices	*/

  esc("<");	/* Enter ANSI mode (if in VT52 mode)	*/
  rm("?3");	/* 80 col mode		*/
  sm("?7");	/* Wrap around on	*/
  rm("?8");	/* Auto repeat off	*/
  decstbm(0,0);	/* No scroll region	*/

}

bye () {
  /* Force my personal prejudices upon the poor luser	*/

  rm("?3");	/* 80 col mode		*/
  rm("?6");	/* Absolute origin mode	*/
  sm("?7");	/* Wrap around on	*/
  sm("?8");	/* Auto repeat on	*/
  decstbm(0,0);	/* No scroll region	*/

  /* Say goodbye */

  ed(2);
  cup(12,30);
  printf("That's all, folks!\n");
  printf("\n\n\n");
  inflush();
#ifdef UNIX
#ifdef USG
  ioctl(0, TCSETA, &sgttyOrg);
#else
  stty(0,&sgttyOrg);
#endif
#endif
  exit();
}

pause1 () {
  printf("Push <RETURN>");
  readnl();
}

readnl() {
#ifdef UNIX
  char ch;
  do { read(0,&ch,1); } while(ch != '\n');
#endif
#ifdef SARGASSO
  while (getchar() != '\n')
  ;
#endif
}

scanto(str, pos, toc) char *str; int *pos; char toc; {
  char c;
  int result = 0;

  while (toc != (c = str[(*pos)++])) {
    if (isdigit(c)) result = result * 10 + c - '0';
    else break;
  }
  if (c == toc) return(result);
  else          return(0);
}

char *lookup(t, k) struct table t[]; int k; {

  int i;
  for (i = 0; t[i].key != -1; i++) {
    if (t[i].key == k) return(t[i].msg);
  }
  return("BAD VALUE");
}

menu(tableptr, header) char *tableptr[]; char *header; {

  int i, tablesize, choice;
  char choicec;

  ed(2);
  cup(1, 10);
  printf("%s", header);

  tablesize = 0;
  for (i = 0; *tableptr[i] != '\0'; i++) {
    cup(4 + 2 * tablesize, 10);
    printf("%d. %s", i, tableptr[i]);
    tablesize++;
  }
  tablesize--;

  cup(7 + 2 * tablesize, 10);
  printf("Enter choice number (0 - %d): ", tablesize);
  for(;;) {
    scanf("%1s", &choicec);
    readnl();
    choice = choicec - '0';
    if (choice >= 0 && choice <= tablesize)
      return (choice);
    printf("%9sBad choice, try again: ", "");
  }
}

chrprint (s) char *s; {

  int i;

  printf("  ");
  sgr("7");
  printf(" ");
  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] <= ' ' || s[i] == '\177')
    printf("<%d> ", s[i]);
    else printf("%c ", s[i]);
  }
  sgr("");
}

#ifdef SARGASSO
sleep(t) int t; {
  calli(072,t);		/* (HIBER) t milliseconds */
}
#endif

inflush () {

  /*
   *   Flush input buffer, make sure no pending input character
   */

  int val;

#ifdef UNIX
#ifndef FIONREAD
  while(read(2,&val,1));
#else
  long l1;
  ioctl (0, FIONREAD, &l1);
  while(l1-- > 0L) read(0,&val,1);
#endif
#endif
#ifdef SARGASSO
  while(uuo(051,2,&val))	/* TTCALL 2, (INCHRS)  */
    ;
#endif
}

char inchar() {

  /*
   *   Wait until a character is typed on the terminal
   *   then read it, without waiting for CR.
   */

  int val, waittime;

#ifdef UNIX
  char ch;
  read(0,&ch,1); val = ch;
#endif
#ifdef SARGASSO
  waittime = 0;
  while(!uuo(051,2,&val)) {		/* TTCALL 2, (INCHRS)	*/
    sleep(100);				/* Wait 0.1 seconds	*/
    if ((waittime += ttymode) > 600)	/* Time-out, in case	*/
      return('\177');			/* of hung in ttybin(1)	*/
  }
#endif
  return(val);
}

char *instr() {

  /*
   *   Get an unfinished string from the terminal:
   *   wait until a character is typed on the terminal,
   *   then read it, and all other available characters.
   *   Return a pointer to that string.
   */


  int i, val, crflag; long l1; char ch;
  static char result[80];

  i = 0;
  result[i++] = inchar();
#ifdef SARGASSO
  sleep(100);           /* Wait 0.1 seconds */
#endif
#ifdef UNIX
  sleep(1);             /* can't sleep 0.1 seconds in vanilla UNIX */
#ifndef FIONREAD
  while(read(2,result+i,1) == 1)
    if (i++ == 78) break;
#else
  while(ioctl(0,FIONREAD,&l1), l1 > 0L) {
    while(l1-- > 0L) {
      read(0,result+i,1);
      if (i++ == 78) goto out1;
    }
  }
out1:
#endif
#endif
#ifdef SARGASSO
  while(uuo(051,2,&val)) {	/* TTCALL 2, (INCHRS)  */
    if (!(val == '\012' && crflag))	/* TOPS-10 adds LF to CR */
      result[i++] = val;
    crflag = val == '\015';
    if (i == 79) break;
    sleep(50);          /* Wait 0.05 seconds */
  }
#endif
  result[i] = '\0';
  return(result);
}

ttybin(bin) int bin; {
#ifdef SARGASSO
  #define OPEN 050
  #define IO_MOD 0000017
  #define _IOPIM 2
  #define _IOASC 0
  #define _TOPAG 01021
  #define _TOSET 01000

  int v;
  static int arglst[] = {
    _IOPIM,
    `TTY`,
    0    
  };
  arglst[0] = bin ? _IOPIM : _IOASC;
  v = uuo(OPEN, 1, &arglst[0]);
  if (!v) { printf("OPEN failed"); exit(); }
  trmop(_TOPAG + _TOSET, bin ? 0 : 1);
  ttymode = bin;
#endif
}

trmop(fc,arg) int fc, arg; {
#ifdef SARGASSO
  int retvalp;
  int arglst[3];

  arglst[0] = fc;		/* function code	*/
  arglst[1] = -1;		/* UDX, -1 denotes TTY:	*/
  arglst[2] = arg;		/* Optional argument	*/

      /* TRMOP. */
  if (calli(0116, 3 // &arglst[0], &retvalp))
  return (retvalp);
  else {
    printf("?Error return in TRMOP.");
    exit();
  }
#endif
}

println(s) char *s; {
  printf("%s\n", s);
}

esc(s) char *s; {
  printf("%c%s", 27, s);
}

esc2(s1, s2) char s1, s2; {
  printf("%c%s%s", 27, s1, s2);
}

brcstr(ps, c) char *ps, c; {
  printf("%c[%s%c", 27, ps, c);
}

brc(pn,c) int pn; char c; {
  char *str, *ptr;
  ptr = str = "[    ";
  ptr++;
  if (pn > 99) { *ptr++ = (pn / 100) + 48; pn %= 100; }
  if (pn >  9) { *ptr++ = (pn /  10) + 48; pn %=  10; }
  *ptr++ = pn + 48;
  *ptr++ = c;
  *ptr = '\0';
  printf("%c%s", 27, str);
}

brc2(pn1, pn2 ,c) int pn1, pn2; char c; {
  char *str, *ptr;
  ptr = str = "[        ";
  ptr++;
  if (pn1 > 99) { *ptr++ = (pn1 / 100) + 48; pn1 %= 100; }
  if (pn1 >  9) { *ptr++ = (pn1 /  10) + 48; pn1 %=  10; }
  *ptr++ = pn1 + 48;
  *ptr++ = ';';
  if (pn2 > 99) { *ptr++ = (pn2 / 100) + 48; pn2 %= 100; }
  if (pn2 >  9) { *ptr++ = (pn2 /  10) + 48; pn2 %=  10; }
  *ptr++ = pn2 + 48;
  *ptr++ = c;
  *ptr = '\0';
  printf("%c%s", 27, str);
}

cub(pn) int pn; {  /* Cursor Backward */
  brc(pn,'D');
}
cud(pn) int pn; {  /* Cursor Down */
  brc(pn,'B');
}
cuf(pn) int pn; {  /* Cursor Forward */
  brc(pn,'C');
}
cup(pn1, pn2) int pn1, pn2; {  /* Cursor Position */
  brc2(pn1, pn2, 'H');
}
cuu(pn) int pn; {  /* Cursor Up */
  brc(pn,'A');
}
da() {  /* Device Attributes */
  brc(0,'c');
}
decaln() {  /* Screen Alignment Display */
  esc("#8");
}
decdhl(lower) int lower; {  /* Double Height Line (also double width) */
  if (lower) esc("#4");
  else       esc("#3");
}
decdwl() {  /* Double Wide Line */
  esc("#6");
}
deckpam() {  /* Keypad Application Mode */
  esc("=");
}
deckpnm() {  /* Keypad Numeric Mode */
  esc(">");
}
decll(ps) char *ps; {  /* Load LEDs */
  brcstr(ps, 'q');
}
decrc() {  /* Restore Cursor */
  esc("8");
}
decreqtparm(pn) int pn; {  /* Request Terminal Parameters */
  brc(pn,'x');
}
decsc() {  /* Save Cursor */
  esc("7");;
}
decstbm(pn1, pn2) int pn1, pn2; {  /* Set Top and Bottom Margins */
  brc2(pn1, pn2, 'r');
}
decswl() {  /* Single With Line */
  esc("#5");
}
dectst(pn) int pn; {  /* Invoke Confidence Test */
  brc2(2, pn, 'y');
}
dsr(pn) int pn; {  /* Device Status Report */
  brc(pn, 'n');
}
ed(pn) int pn; {  /* Erase in Display */
  brc(pn, 'J');
}
el(pn) int pn; {  /* Erase in Line */
  brc(pn,'K');
}
hts() {  /* Horizontal Tabulation Set */
  esc("H");
}
hvp(pn1, pn2) int pn1, pn2; {  /* Horizontal and Vertical Position */
  brc2(pn1, pn2, 'f');
}
ind() {  /* Index */
  esc("D");
}
nel() {  /* Next Line */
  esc("E");
}
ri() {  /* Reverse Index */
  esc("M");
}
ris() { /*  Reset to Initial State */
  esc("c");
}
rm(ps) char *ps; {  /* Reset Mode */
  brcstr(ps, 'l');
}
scs(g,c) int g; char c; {  /* Select character Set */
  printf("%c%c%c%c%c%c%c", 27, g ? ')' : '(', c,
                           27, g ? '(' : ')', 'B',
			   g ? 14 : 15);
}
sgr(ps) char *ps; {  /* Select Graphic Rendition */
  brcstr(ps, 'm');
}
sm(ps) char *ps; {  /* Set Mode */
  brcstr(ps, 'h');
}
tbc(pn) int pn; {  /* Tabulation Clear */
  brc(pn, 'g');
}

vt52cup(l,c) int l,c; {
  printf("%cY%c%c", 27, l + 31, c + 31);
}

