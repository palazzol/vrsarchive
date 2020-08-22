/*
 * Reverse Assembler Global Data
 * Filename: rad1.c
 */
#include "ra.h"
#ifdef LOCDBG
#undef LOCDBG             /* turn off local debug */
#endif LOCDBG
#define NUMOPS 57

struct label *xdefhd,*xdeftl; /* head and tail of xdef label list */
struct label *xrefhd,*xreftl; /* head and tail of xref label list */
FILE lst;                 /* listing file */
FILE obj;                 /* object file */
unsigned bias,codesz,relsz,xrefsz,datasz;
unsigned prompt;          /* 1 => prompt for format */
char *codebuf;            /* code buffer     */
unsigned *relbuf;         /* relocation data */

char *mtable[]={ /* mnemonic table */
      "???",              /* 0     */
      "adc","and","asl",  /* 01-03 */
      "bcc","bcs","beq",  /* 04-06 */
      "bit","bmi","bne",  /* 07-09 */
      "bpl","brk","bvc",  /* 10-12 */
      "bvs","clc","cld",  /* 13-15 */
      "cli","clv","cmp",  /* 16-18 */
      "cpx","cpy","dec",  /* 19-21 */
      "dex","dey","eor",  /* 22-24 */
      "inc","inx","iny",  /* 25-27 */
      "jmp","jsr","lda",  /* 28-30 */
      "ldx","ldy","lsr",  /* 31-33 */
      "nop","ora","pha",  /* 34-36 */
      "php","pla","plp",  /* 37-39 */
      "rol","ror","rti",  /* 40-42 */
      "rts","sbc","sec",  /* 43-45 */
      "sed","sei","sta",  /* 46-48 */
      "stx","sty","tax",  /* 49-51 */
      "tay","tsx","txa",  /* 52-54 */
      "txs","tya"         /* 55-56 */
};

char opcodes[256]={/* opcode to mnemonic translation */

/* 00 */  11, 35, 00, 00, 00, 35, 03, 00,
/* 08 */  37, 35, 03, 00, 00, 35, 03, 00,
/* 10 */  10, 35, 00, 00, 00, 35, 03, 00, 
/* 18 */  14, 35, 00, 00, 00, 35, 03, 00,
/* 20 */  29, 02, 00, 00, 07, 02, 40, 00,
/* 28 */  39, 02, 40, 00, 07, 02, 40, 00,
/* 30 */   8, 02, 00, 00, 00, 02, 40, 00,
/* 38 */  45, 02, 00, 00, 00, 02, 40, 00,
/* 40 */  42, 24, 00, 00, 00, 24, 33, 00,
/* 48 */  36, 24, 33, 00, 28, 24, 33, 00,
/* 50 */  12, 24, 00, 00, 00, 24, 33, 00,
/* 58 */  16, 24, 00, 00, 00, 24, 33, 00,
/* 60 */  43, 01, 00, 00, 00, 01, 41, 00,
/* 68 */  38, 01, 41, 00, 28, 01, 41, 00,
/* 70 */  13, 01, 00, 00, 00, 01, 41, 00,
/* 78 */  47, 01, 00, 00, 00, 01, 41, 00,
/* 80 */  00, 48, 00, 00, 50, 48, 49, 00,
/* 88 */  23, 00, 54, 00, 50, 48, 49, 00,
/* 90 */  04, 48, 00, 00, 50, 48, 49, 00,
/* 98 */  56, 48, 55, 00, 00, 48, 00, 00,
/* a0 */  32, 30, 31, 00, 32, 30, 31, 00,
/* a8 */  52, 30, 51, 00, 32, 30, 31, 00,
/* b0 */  05, 30, 00, 00, 32, 30, 31, 00,
/* b8 */  17, 30, 53, 00, 32, 30, 31, 00,
/* c0 */  20, 18, 00, 00, 20, 18, 21, 00,
/* c8 */  27, 18, 22, 00, 20, 18, 21, 00,
/* d0 */   9, 18, 00, 00, 00, 18, 21, 00,
/* d8 */  15, 18, 00, 00, 00, 18, 21, 00,
/* e0 */  19, 44, 00, 00, 19, 44, 25, 00,
/* e8 */  26, 44, 34, 00, 19, 44, 25, 00,
/* f0 */  06, 44, 00, 00, 00, 44, 25, 00,
/* f8 */  46, 44, 00, 00, 00, 44, 25, 00

};

char amodes[256] = { /* opcode to addressing mode */
/* 00 */ IMP , INDX, IMP , IMP , IMP , ZER , ZER , IMP ,
/* 08 */ IMP , IMM , ACC , IMP , IMP , ABS , ABS , IMP ,
/* 10 */ REL , INDY, IMP , IMP , IMP , ZPX , ZPX , IMP ,
/* 18 */ IMP , ABY , IMP , IMP , IMP , ABX , ABX , IMP ,
/* 20 */ ABS , INDX, IMP , IMP , ZER , ZER , ZER , IMP ,
/* 28 */ IMP , IMM , ACC , IMP , ABS , ABS , ABS , IMP ,
/* 30 */ REL , INDY, IMP , IMP , IMP , ZPX , ZPX , IMP ,
/* 38 */ IMP , ABY , IMP , IMP , IMP , ABX , ABX , IMP ,
/* 40 */ IMP , INDX, IMP , IMP , IMP , ZER , ZER , IMP ,
/* 48 */ IMP , IMM , ACC , IMP , ABS , ABS , ABS , IMP ,
/* 50 */ REL , INDY, IMP , IMP , IMP , ZPX , ZPX , IMP ,
/* 58 */ IMP , ABY , IMP , IMP , IMP , ABX , ABX , IMP ,
/* 60 */ IMP , INDX, IMP , IMP , IMP , ZER , ZER , IMP ,
/* 68 */ IMP , IMM , ACC , IMP , IND , ABS , ABS , IMP ,
/* 70 */ REL , INDY, IMP , IMP , IMP , ZPX , ZPX , IMP ,
/* 78 */ IMP , ABY , IMP , IMP , IMP , ABX , ABX , IMP ,
/* 80 */ IMP , INDX, IMP , IMP , ZER , ZER , ZER , IMP ,
/* 88 */ IMP , IMP , IMP , IMP , ABS , ABS , ABS , IMP ,
/* 90 */ REL , INDY, IMP , IMP , ZPX , ZPX , ZPY , IMP ,
/* 98 */ IMP , ABY , IMP , IMP , IMP , ABX , IMP , IMP ,
/* a0 */ IMM , INDX, IMM , IMP , ZER , ZER , ZER , IMP ,
/* a8 */ IMP , IMM , IMP , ABS , ABS , ABS , ABS , IMP ,
/* b0 */ REL , INDY, IMP , IMP , ZPX , ZPX , ZPY , IMP ,
/* b8 */ IMP , ABY , IMP , IMP , ABX , ABX , ABY , IMP ,
/* c0 */ IMM , INDX, IMP , IMP , ZER , ZER , ZER , IMP ,
/* c8 */ IMP , IMM , IMP , IMP , ABS , ABS , ABS , IMP ,
/* d0 */ REL , INDY, IMP , IMP , IMP , ZPX , ZPX , IMP ,
/* d8 */ IMP , ABY , IMP , IMP , IMP , ABX , ABX , IMP ,
/* e0 */ IMM , INDX, IMP , IMP , ZER , ZER , ZER , IMP ,
/* e8 */ IMP , IMM , IMP , IMP , ABS , ABS , ABS , IMP ,
/* f0 */ REL , INDY, IMP , IMP , IMP , ZPX , ZPX , IMP ,
/* f8 */ IMP , ABY , IMP , IMP , IMP , ABX , ABX , IMP
};

char modesz[13] = {
 /* ACC */ 0,
 /* IMM */ 1,
 /* ZER */ 1,
 /* ZPX */ 1,
 /* ZPY */ 1,
 /* ABS */ 2,
 /* ABX */ 2,
 /* ABY */ 2,
 /* IMP */ 0,
 /* REL */ 1,
 /* INDX*/ 1,
 /* INDY*/ 1,
 /* IND */ 2
};
 
char *modename[] = {
 "accumulator",
 "immediate",
 "zero page",
 "zero page,x",
 "zero page,y",
 "absolute",
 "absolute,x",
 "absolute,y",
 "implied",
 "relative",
 "(indirect,x)",
 "(indirect),y",
 "(indirect)"
};

#ifdef LOCDBG
prtop(op)
unsigned op;
{

  printf(" %02x %-3s %-12s",
    op,mtable[opcodes[op]],
    modename[amodes[op]]); 

}

main() /* for local test only */
{
 unsigned i,j;

 for (i=0;i<=0xc0;i+= 0x40) {
   for (j=0;j<0x20;j++) {
     prtop(i+j);
     printf("  ");
     prtop(i+0x20+j);
     putchar('\n');
   }
   printf("\n\n");
 }
  putchar('\n');

}
#endif
