/*
 * Reverse Assembler
 * Constant and Structure Definitions
 *
 */
#include <stdio.h>
#include <strings.h>

#define DEBUG 
#define MAXID 20

/* addressing mode constants */
#define ACC    0
#define IMM    1
#define ZER    2
#define ZPX    3
#define ZPY    4
#define ABS    5
#define ABX    6
#define ABY    7
#define IMP    8
#define REL    9
#define INDX  10
#define INDY  11
#define IND   12
 
typedef char byte;

struct label {
  struct label *next; /* forward linked list  */
  char *name;
  unsigned flag;
  unsigned offset;
};

