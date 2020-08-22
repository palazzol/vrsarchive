
/**********************************************************
 *							  *
 *                  TKO -- The Boxing Game		  *
 *							  *
 **********************************************************/


/*
Copyright (c) 1979  Mike Gancarz
Permission is hereby granted to freely distribute, copy, modify and otherwise
use this software for nonprofit purposes as long as this copyright appears
in the file and remains intact.
*/


#include "def.h"

/** boxing action card **/
struct action{
	byte	acrp;		/* ring position */
	char	acrpchk;	/* check ring position */
	byte	accf;		/* control factor */
	char	achand;		/* left- or right-hand punch */
	byte	accutk;		/* check for cuts or knockdowns: 100=kd */
	byte	acpunch;	/* punch number */
	byte	ackd;		/* knockdown attempt */
	byte	ackdr;		/* knockdown rating */
	byte	ackor;		/* knockout rating */
	char	acj1;		/* judge 1 scoring */
	char	acj2;		/* judge 2 scoring */
	char	acref;		/* referee scoring */
} act[81] = {
	0,'\0',0,'\0',0,0,0,0,0,'\0','\0','\0',		/* null card */
	6,'n',12,'R',100,79,10,4,8,'H','L','H',		/* card 1 */
	0,'n',17,'R',100,72,3,3,10,'E','H','H',		/* card 2 */
	0,'n',5,'L',50,49,11,10,15,'H','E','H',		/* card 3 */
	0,'n',2,'R',50,21,19,18,6,'H','E','E',		/* card 4 */
	0,'n',7,'R',50,7,7,1,12,'L','E','E',		/* card 5 */
	0,'n',9,'R',50,17,8,14,9,'L','L','H',		/* card 6 */
	0,'n',3,'L',100,32,2,20,17,'L','H','L',		/* card 7 */
	1,'y',4,'R',50,25,1,9,13,'H','H','H',		/* card 8 */
	0,'n',10,'R',50,41,9,2,7,'H','L','H',		/* card 9 */
	2,'n',14,'L',3,11,17,19,20,'H','H','E',		/* card 10 */
	0,'n',6,'L',50,55,12,7,1,'H','L','H',		/* card 11 */
	8,'n',13,'R',5,62,7,7,4,'H','H','H',		/* card 12 */
	0,'n',20,'L',100,70,5,11,3,'L','H','H',		/* card 13 */
	2,'y',15,'R',50,23,6,16,5,'H','H','L',		/* card 14 */
	0,'n',8,'R',50,43,4,12,11,'H','E','H',		/* card 15 */
	0,'n',3,'L',50,10,15,14,7,'L','H','L',		/* card 16 */
	8,'n',11,'L',50,35,14,5,5,'H','E','H',		/* card 17 */
	7,'n',11,'L',50,12,12,1,20,'H','H','H',		/* card 18 */
	0,'n',10,'R',50,36,10,13,8,'E','L','L',		/* card 19 */
	0,'n',16,'L',50,15,2,8,16,'L','L','H',		/* card 20 */
	0,'n',19,'R',50,71,3,6,12,'H','H','L',		/* card 21 */
	0,'n',18,'L',100,37,4,2,2,'H','E','L',		/* card 22 */
	0,'n',1,'R',50,6,13,15,14,'L','E','H',		/* card 23 */
	5,'n',9,'L',10,47,16,17,18,'L','H','L',		/* card 24 */
	7,'n',7,'L',50,65,18,9,10,'E','H','E',		/* card 25 */
	0,'n',3,'R',100,77,1,10,3,'L','H','E',		/* card 26 */
	0,'n',16,'R',50,27,8,5,14,'E','E','L',		/* card 27 */
	6,'y',12,'L',50,67,20,8,12,'H','H','H',		/* card 28 */
	0,'n',20,'L',100,24,4,11,4,'H','H','H',		/* card 29 */
	0,'n',1,'R',50,3,9,13,19,'H','L','H',		/* card 30 */
	5,'n',18,'R',7,78,3,12,11,'H','E','E',		/* card 31 */
	0,'n',2,'R',50,30,11,17,16,'E','H','L',		/* card 32 */
	0,'n',33,'R',50,46,5,4,1,'H','H','H',		/* card 33 */
	5,'n',20,'L',100,61,20,19,1,'E','H','L',	/* card 34 */
	7,'n',4,'R',50,2,6,14,10,'H','H','H',		/* card 35 */
	0,'y',10,'R',50,5,2,11,2,'E','L','E',		/* card 36 */
	0,'n',6,'L',50,38,13,15,15,'H','L','E',		/* card 37 */
	1,'n',5,'R',100,66,1,3,6,'L','H','H',		/* card 38 */
	0,'n',17,'L',100,54,14,16,13,'H','L','H',	/* card 39 */
	0,'n',8,'R',50,40,16,18,9,'L','H','H',		/* card 40 */
	0,'n',5,'R',50,28,12,6,12,'H','L','L',		/* card 41 */
	0,'n',20,'R',50,64,1,1,10,'E','L','H',		/* card 42 */
	0,'y',13,'L',50,73,6,17,3,'H','E','L',		/* card 43 */
	7,'n',14,'R',50,53,19,10,4,'E','H','E',		/* card 44 */
	0,'n',15,'L',50,33,18,5,3,'H','E','L',		/* card 45 */
	6,'n',5,'R',1,19,7,14,17,'H','H','H',		/* card 46 */
	0,'n',10,'L',50,66,2,12,9,'L','H','H',		/* card 47 */
	0,'n',11,'L',50,50,9,1,11,'H','H','L',		/* card 48 */
	0,'n',9,'L',9,26,13,13,7,'H','L','E',		/* card 49 */
	0,'n',6,'L',4,13,17,16,11,'H','H','E',		/* card 50 */
	6,'n',1,'R',100,45,3,3,6,'E','L','H',		/* card 51 */
	8,'n',7,'R',50,34,8,20,8,'L','H','H',		/* card 52 */
	8,'n',2,'R',50,18,15,19,18,'H','H','H',		/* card 53 */
	0,'n',13,'L',50,48,10,7,5,'E','E','H',		/* card 54 */
	0,'n',4,'L',2,60,20,18,19,'L','H','H',		/* card 55 */
	0,'n',12,'L',50,42,11,20,19,'L','L','H',	/* card 56 */
	0,'n',19,'R',100,27,19,18,13,'H','L','H',	/* card 57 */
	0,'n',18,'L',100,75,11,9,1,'L','H','H',		/* card 58 */
	0,'n',8,'L',100,51,16,15,7,'L','H','E',		/* card 59 */
	2,'n',1,'L',100,59,13,16,9,'H','E','L',		/* card 60 */
	0,'n',6,'L',50,39,10,8,5,'L','E','H',		/* card 61 */
	1,'n',16,'L',100,68,5,12,8,'L','H','H',		/* card 62 */
	0,'y',9,'R',50,44,9,17,15,'E','L','H',		/* card 63 */
	0,'n',8,'R',50,9,14,4,2,'L','L','L',		/* card 64 */
	0,'n',14,'L',6,80,18,20,4,'E','E','L',		/* card 65 */
	0,'n',2,'L',8,14,15,10,17,'H','L','L',		/* card 66 */
	0,'y',18,'L',50,1,4,2,14,'H','H','L',		/* card 67 */
	0,'n',3,'L',50,20,6,15,16,'E','H','L',		/* card 68 */
	2,'n',15,'R',100,29,8,2,15,'E','H','H',		/* card 69 */
	1,'n',17,'L',50,16,5,13,9,'L','H','H',		/* card 70 */
	0,'n',13,'L',50,22,17,9,20,'E','H','E',		/* card 71 */
	0,'n',12,'R',50,8,7,6,14,'H','L','L',		/* card 72 */
	5,'y',4,'L',50,58,16,19,13,'H','H','L',		/* card 73 */
	0,'n',19,'R',100,63,12,3,6,'L','E','H',		/* card 74 */
	0,'n',17,'R',50,76,14,11,16,'H','L','L',	/* card 75 */
	0,'n',7,'L',100,57,17,4,18,'H','L','E',		/* card 76 */
	0,'n',11,'R',50,69,20,8,20,'L','L','E',		/* card 77 */
	0,'n',16,'L',50,31,18,5,19,'L','H','L',		/* card 78 */
	0,'n',15,'R',50,4,19,7,17,'H','L','H',		/* card 79 */
	0,'n',14,'R',50,74,15,6,18,'H','H','E'		/* card 80 */
};


/** ring positions **/
/*int rp[] = {	0,		/* ring center */
/*		1,		/* near ropes */
/*		2,		/* far ropes */
/*		3,		/* left ropes */
/*		4,		/* right ropes */
/*		5,		/* boxer a's corner */
/*		6,		/* boxer b's corner */
/*		7,		/* left neutral corner */
/*		8		/* right neutral corner */
/*};*/

/** weight classes **/
char *wc[12] = {
		"null",
	/* 1 */	"Heavyweight",
	/* 2 */	"Light heavyweight",
	/* 3 */	"Middleweight",
	/* 4 */	"Jr. Middleweight",
	/* 5 */	"Welterweight",
	/* 6 */	"Jr. Welterweight",
	/* 7 */	"Lightweight",
	/* 8 */	"Jr. Lightweight",
	/* 9 */	"Bantamweight",
	/* 10*/	"Featherweight",
	/* 11*/	"Flyweight"
};

/** run-time statistics **/
struct rtstat{
	int	bxptr;		/* pointer to boxer data structure */
	byte	ccf;		/* current control factor */
	byte	chp;		/* current hit power */
	byte	ckdr1;		/* current knockdown rating 1 */
	byte	cagg;		/* current aggressiveness rating */
	int	cend;		/* amount of endurance left */
	byte	cdef;		/* current defense rating */
	byte	cpl;		/* current punches landed upper limit */
	byte	cnpm;		/* current number of punches missed */
	byte	cnpts;		/* current number of points */
	byte	catko1;		/* auto-tko score from previous round */
	byte	catko2;		/* auto-tko score from previous round */
	byte	pncpts;		/* previous number of cut points */
	byte	cncpts;		/* current number of cut points */
	byte	cj1scr;		/* current judge #1 score */
	byte	cj2scr;		/* current judge #2 score */
	byte	crscr;		/* current referee score */
	byte	ckdflg;		/* current knockdown flag */
} rt[2] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/** knockdown/knockout table **/
byte kdtab[200] = {
	0,0,0,0,0,2,0,0,0,1,0,0,0,0,1,0,0,0,0,0,
	0,2,0,0,0,1,0,0,2,0,1,0,0,2,0,0,0,1,0,0,
	1,0,0,2,1,0,2,0,1,0,0,0,2,0,1,0,0,2,0,0,
	0,1,2,0,1,2,1,0,0,2,2,0,1,1,0,0,2,0,0,2,
	2,0,0,2,0,1,2,0,1,2,1,0,0,2,2,1,1,0,1,2,
	1,2,2,0,2,0,0,2,2,1,2,2,1,0,0,2,2,1,1,1,
	2,1,1,2,1,2,1,0,2,1,0,1,2,2,2,0,0,2,2,2,
	1,1,2,2,0,2,2,2,1,2,2,2,0,1,2,2,2,0,2,1,
	2,2,2,1,2,0,2,2,1,1,1,2,2,0,1,2,2,2,2,2,
	1,2,1,1,2,2,2,2,2,2,2,2,0,2,2,2,1,2,1,2
};

/** primary cut table **/
byte cuttab[90] = {
	/*  a  b  c  d  e  f  g  h */
	68,72,76,76,76,76,77,78,79,
	60,60,64,68,68,68,72,76,78,
	52,56,60,64,64,64,68,72,76,
	44,48,52,56,60,64,68,72,76,
	40,44,48,52,56,60,64,72,76,
	40,44,48,52,52,52,60,68,76,
	32,40,44,48,48,48,52,64,72,
	24,32,40,44,48,52,56,60,72,
	24,32,40,44,44,44,48,56,68,
	24,32,40,44,44,44,48,52,68
};

/** automatic tko table **/
byte atko[15] = {
	/* 1  2  3  4  5 -- tko rating */
	  35,30,27,25,23, /* 1 round total */
	  50,45,42,40,38, /* 2 round total */
	  60,55,52,50,48  /* 3 round total */
};

/** miscellaneous variables **/
int nboxers;	/* number of boxers in the names file */
int nrounds;	/* number of rounds in the fight */
int round;	/* current round number */
int a;		/* boxer a  bx[] offset */
int b;		/* boxer b bx[] offset */
int fagg;	/* fight aggressor */
int offense;	/* fighter on offense */
int actcnt;	/* boxing action card counter */
int ccard;	/* current boxing action card */
int lcard;	/* last boxing action card */
byte killer;	/* killer instinct flag */
byte kdcount;	/* knockdown count */
byte deck;	/* which half of the boxing action card deck is being used */
byte w;		/* weight class for this fight */
int speed;	/* speed at which the fight will take place */
char temp[LINE];/* temporary string buffer */
int cused[80];	/* boxing action cards already used this round */
char ptype[20];	/* verbal description of type of punch landed */
