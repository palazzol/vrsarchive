/* 
 * Reverse Assembler
 * Translation Routines
 * Filename: ra2.c
 *
 */
#include "ra.h"

/* external data */
extern FILE obj;
extern FILE lst;
extern struct label *xdefhd;
extern struct label *xrefhd;
extern char *codebuf;
extern unsigned codesz,bias,datasz,prompt;
extern unsigned *relbuf;

extern char *mtable[];  /* mnemonic strings */
extern char opcodes[];  /* opcode to mnemonic index translation */
extern char amodes[];   /* opcode to addressing mode translation */
extern char modesz[];   /* mode to address field size */
/* 
 * Package global data
 *
 */
char *codeptr; /* code buffer pointer */
int count;     /* code counter */
char efunc;    /* true when end of function detected (c$106) */
unsigned loc;  /* current location */
int *relptr;   /* relocation info pointer */
struct label *cursym; /* current symbol */
struct label *xdefptr,*xrefptr; /* pointer to next xdef, xref */

/*
 * Print address value as either a symbolic quantity or hex value
 * Called with:
 *   Address, addressing mode, size (0-2)
 */
pav(a,m,s)
unsigned a,m,s;
{
 if (!ckref()) {    /* xref pending?  */
   if (m != IMM) {  /* don't do symbol search for immediate operands */
     if (cksym(a)) return;
   }
   if (s==2) fprintf(lst,"$%04x",a);
   else fprintf(lst,"$%02x",a);
 }
}

/*
 * Print address field based on opcode
 * Called with:
 *   address
 *   addressing mode
 *   size (0-2)
 */
paf(a,m,s)
unsigned a,m,s;
{
  unsigned addr; /* address value */

  switch (m) /* prefix? */
  {
  case ACC: fputc('A',lst);
            break;

  case IMM: fputc('#',lst);
            pav(a,m,s);
            break;
  case ZER:
  case ABS:    
            pav(a,m,s);
            break;

  case ZPX:
  case ABX: pav(a,m,s);
            fprintf(lst,",X");
            break;

  case ZPY:
  case ABY: pav(a,m,s);
            fprintf(lst,",Y");
            break;

  case IMP: break;

  case REL: 
            if (a>=128) /* sign extend */
              addr = (0xff00 | a);
            else
              addr = a;
            addr=addr+loc+2; 
            pav(addr,m,s);
            break;

  case INDX: fputc('(',lst);
             pav(a,m,s);
             fprintf(lst,",X)");
             break;

  case INDY: fputc('(',lst);
             pav(a,m,s);
             fprintf(lst,"),Y");
             break;

  case IND: fputc('(',lst);
            pav(a,m,s);
            fputc(')',lst);
            break;

  default: fprintf(lst,"????");

  }
}

/* 
 * Main routine for this package
 *
 */
rvrs()
{
  char op;
  unsigned a1,a2,defcnt;
  unsigned mode,imode,size,i;

  codeptr = codebuf;
  relptr = relbuf;
  xdefptr = xdefhd;
  xrefptr = xrefhd;
  loc = bias;
  count = codesz;
  imode = 1; /* always start in instruction mode */
  efunc = 0;
  fprintf(lst,"  *= $%04x\n",bias);
  while (count) {
    if (ckdef() || efunc) imode=ckmode();
    efunc = 0;
    if (!opcodes[*codeptr]) { /* invalid opcode? */
      fprintf(lst,";!!!Invalid opcode - switching to byte mode.\n");
      imode=0;
    } 
    if (imode==0) {
      dobyte();
      if (count) imode = ckmode();
    }
    else {         /* instruction mode */
      op = *codeptr++;
      count--;
      mode = amodes[op];
      size = modesz[mode];
      a1=0;
      a2=0;
      if (size) {
        a1 = *codeptr++;
        count--;
        if (size==2) {
          a2 = *codeptr++;
          count--;
        }
      }
      if (count<0) {
        fprintf(lst,"--- code underflow ---\n");
        abort();
      }
#ifdef DEBUG
      fprintf(lst,"%02x %02x %02x / %04x  ",op,a1,a2,loc);
#endif
      fprintf(lst," %s ",mtable[opcodes[op]]);
      if (size==2)
        a1 = a1 | (a2 << 8);
      paf(a1,mode,size); /* print address field */
      fputc('\n',lst);
      loc=loc+size+1;
    }                    /* instruction mode */
  } /* while */
  dodata();              /* output data blocks */
  fputc('\n',lst);
}

/* 
 * Format data in .byte mode until a next label definition.
 *
 */
dobyte()
{
  unsigned bytecnt=8,first=1;

  while (count) {
    if (ckdef()) break;  
    if (bytecnt==8) {
      if (!first) fputc('\n',lst);
      first=0;
      bytecnt=0;
#ifdef DEBUG
      fprintf(lst,"           %04x  ",loc);
#endif
      fprintf(lst," .byte ");
    }
    fprintf(lst,"%c $%02x ",(bytecnt>0 ? ',' : ' '),*codeptr++);
    bytecnt++;
    count--;
    loc++;
  } /* while */
  fputc('\n',lst);
}
  
/*
 * See if current location has one or more labels defined.
 * Returns a count of labels defined for this location.
 */
ckdef()
{
 unsigned dc;

 dc = 0;
 while (xdefptr && (loc == xdefptr->offset)) {
     if (!dc) {
       fputc('\n',lst); /* skip line on first label */
       fprintf(lst,";--------------------\n");
     }
     dc++;
     fprintf(lst,"%s\n",xdefptr->name);
     cursym = xdefptr; /* save current symbol */
     xdefptr = xdefptr->next;
 }
 return(dc);
}

/* 
 * See if current location has an external reference.
 *
 */
ckref()
{
 if (xrefptr && (loc == xrefptr->offset)) {
/*
 * Here lies a kludge which assists in the determination of the formatting
 * mode to be used.  If a reference to runtime routine C$106 is being made,
 * we are about to exit a function.  What follows could be another function
 * or it could be the beginning of unlabeled preset string or array data.  
 * The main loop tests the flag 'efunc', which is set here, to see if a mode
 * check is necessary.
 */
   efunc = !strcmp(xrefptr->name,"c$106");
   if (xrefptr->flag == 1) fputc('<',lst); /* high order byte */
   else if (xrefptr->flag == 2) fputc('>',lst); /* low order byte */
   fprintf(lst,"%s",xrefptr->name);
   xrefptr = xrefptr->next;
   if (efunc) fputc('\n',lst);
   return(1);
 }
 else return(0);
}

/*
 * See if address value matches external symbol definition offset 
 * Returns symbol offset if found, zero otherwise.  This assumes
 * that zero is an invalid offset, which in fact, it is, since the
 * code at zero is always a jmp c$start.
 */
cksym(a)
unsigned a;
{
  struct label *ptr;

  ptr = xdefhd; /* get xdef list pointer */

  while (ptr) {  /* nothing fancy - just a linear search */
    if (a == ptr->offset) {
      fprintf(lst,"%s",ptr->name);
      return(ptr->offset);
    }
    ptr = ptr->next;
  }
  return(0);
}

/* 
 * Check for formatting mode change.
 * Returns 1 for instruction mode, 0 for .byte mode
 *
 */
ckmode()
{
  char c1,c2; /* next 2 byte values */
  char s[20]; /* response string */  
  c1 = *codeptr;
  c2 = *(codeptr+1);

  if (prompt) { /* user wants authority */
    printf("current symbol: %s\nfirst 2 bytes: %02x %02x\n",cursym->name,c1,c2);
    for (;;) {
      printf("Instruction or Byte mode? ");
      gets(s);
      switch(*s) {
      case 'I':
      case 'i': return(1);
      case 'B':
      case 'b': return(0);
      default: printf("Enter an I or a B, please\n");
      }
    }
  }
  else {
    /*********************************************************************
     * Here comes the big kludge, folks!  In order to decide whether the *
     * code that follows is instruction or data, we test for the pattern *
     * 'sta $fb' which seems to occur at the entry to every procedure.   *
     * If this pattern is not detected, then we switch to data mode and  *
     * output .byte directives.                                          *
     *********************************************************************/

    if ((c1==0x85) && (c2==0xfb))
      return(1); /* set instruction mode */
    else
      return(0);
  }
}

/*
 * Output Data Blocks
 *
 */

dodata()
{
  unsigned cnt,size;
  char id[MAXID];

  cnt = getw(obj); /* get number of entries */
  if (cnt) {
    fprintf(lst,"\n;Module data blocks.\n");

#ifdef DEBUG
    fprintf(lst,"%d data blocks.\n",cnt);
#endif

    while (cnt--) {
      getid(id,obj); /* get identifier */
      size = getw(obj);
      fprintf(lst,"%-10s *=*+%d\n",id,size);
    }
  }
}

